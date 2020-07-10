//+=================================================================================================================
//
// file :               PollThread.cpp
//
// description :        C++ source code for the PollThread class. This class is used for the polling thread. The rule
//						of this thread is to regulary exceute command on device or read attribute and store result
//						in a ring buffer.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <eventsupplier.h>
#include <pollthread.tpp>

#ifdef _TG_WINDOWS_
	#include <sys/timeb.h>
#else
	#include <sys/time.h>
#endif

#include <iomanip>

extern omni_thread::key_t key_py_data;
namespace Tango
{

namespace
{

constexpr auto TIME_NEEDED_HEARTBEAT = std::chrono::microseconds(2000);
constexpr int POLL_LOOP_NB = 500;
constexpr auto DISCARD_THRESHOLD = std::chrono::milliseconds(20);

template <typename Dur>
Dur duration_abs(Dur d)
{
    return d >= Dur::zero() ? d : -d;
}

} // namespace

DeviceImpl *PollThread::dev_to_del = NULL;
std::string PollThread::name_to_del = "";
PollObjType PollThread::type_to_del = Tango::POLL_CMD;

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::Pollthread
//
// description :
//		The polling thread constructor.
//
// args :
//		in :
// 			- cmd : The buffer used to pass command to the polling thread
//			- m : The mutex used to protect the previous buffer
//			- heartbeat : Flag set to true if this polling thread is the thread used to send the heartbeat event
//
//------------------------------------------------------------------------------------------------------------------

PollThread::PollThread(PollThCmd &cmd,TangoMonitor &m,bool heartbeat): shared_cmd(cmd),p_mon(m),
					    sleep(std::chrono::milliseconds(1)),polling_stop(true),
					    attr_names(1),tune_ctr(1),
					    need_two_tuning(false),send_heartbeat(heartbeat),heartbeat_ctr(0)
{
    local_cmd.cmd_pending = false;

	attr_names.length(1);

	cci = 0;
	dummy_cl_id.cpp_clnt(cci);
	previous_nb_late = 0;
	polling_bef_9 = false;

	if (heartbeat == true)
		polling_stop = false;

    dummy_att5.value.union_no_data(true);
    dummy_att5.quality = ATTR_INVALID;
    dummy_att4.value.union_no_data(true);
    dummy_att4.quality = ATTR_INVALID;
    dummy_att3.quality = ATTR_INVALID;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::run_undetached
//
// description :
//		The polling thread main code
//
//-------------------------------------------------------------------------------------------------------------------



void *PollThread::run_undetached(TANGO_UNUSED(void *ptr))
{
	PollCmdType received;
	bool per_thread_data_created = false;

//
// If the thread is the event heartbeat thread, use it also for the storage of sub device properties.
// Declare a work item to check the for new sub devices regularly.
//

	if (send_heartbeat == true)
	{
		WorkItem wo;

		wo.dev = NULL;
		wo.poll_list = NULL;
		wo.type = STORE_SUBDEV;
		wo.update = std::chrono::minutes(30);
		wo.name.push_back(std::string("Sub device property storage"));
		wo.needed_time = PollClock::duration::zero();

		now = PollClock::now();
		wo.wake_up_date = now;
		insert_in_list(wo);
	}

//
// The infinite loop
//

	while(1)
	{
		try
		{
			received = get_command();

//
// Create the per thread data if it is not already done (For Python DS)
//

			if (per_thread_data_created == false)
			{
				omni_thread::self()->set_value(key_py_data,new PyData());
				per_thread_data_created = true;
			}

			now = PollClock::now();

			switch (received)
			{
			case POLL_COMMAND:
				execute_cmd();
				break;

			case POLL_TIME_OUT:
				one_more_poll();
				break;

			case POLL_TRIGGER:
				one_more_trigg();
				break;
			}

			after = PollClock::now();

			if (tune_ctr <= 0)
			{
				tune_list(true);
				if (need_two_tuning == true)
				{
					unsigned long nb_works = works.size();
					tune_ctr = (nb_works << 2);
					need_two_tuning = false;
				}
				else
					tune_ctr = POLL_LOOP_NB;
			}

			compute_sleep_time();
		}
		catch (omni_thread_fatal &)
		{
			std::cerr << "OUPS !! A omni thread fatal exception received by a polling thread !!!!!!!!" << std::endl;
#ifndef _TG_WINDOWS_
			time_t t = time(NULL);
			std::cerr << ctime(&t);
#endif
			std::cerr << "Trying to re-enter the main loop" << std::endl;
		}
		catch (const std::exception &ex)
		{
			std::cerr << "OUPS !! An unforeseen standard exception has been received by a polling thread !!!!!!" << std::endl;
			std::cerr << ex.what() << std::endl;
#ifndef _TG_WINDOWS_
			time_t t = time(NULL);
			std::cerr << ctime(&t);
#endif
			std::cerr << "Trying to re-enter the main loop" << std::endl;
		}
	}

	return NULL;

}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::get_command
//
// description :
//		This method wait on the shared monitor for a new command to be sent to the polling thread. The thread waits
//		with a timeout. If the thread is awaken due to the timeout, false is returned.
//		If the work list is empty, the thread waits for ever.
//
// returns :
// 		Enumeration indicating the type of detected event when waiting for next command.
//
//------------------------------------------------------------------------------------------------------------------

PollCmdType PollThread::get_command()
{
	if (sleep.has_value() && *sleep == PollClock::duration::zero())
	{
		return POLL_TIME_OUT;
	}

	omni_mutex_lock sync(p_mon);
	PollCmdType ret;

//
// Wait on monitor
//

	if ((shared_cmd.cmd_pending == false) && (shared_cmd.trigger == false))
	{
		if (works.empty() == true)
			p_mon.wait();
		else
		{
			if (sleep.has_value())
				p_mon.wait(std::chrono::duration_cast<std::chrono::milliseconds>(*sleep).count());
		}
	}

//
// Test if it is a new command. If yes, copy its data locally
//

	if (shared_cmd.cmd_pending == true)
	{
		local_cmd = shared_cmd;
		ret = POLL_COMMAND;
	}
	else if (shared_cmd.trigger == true)
	{
		local_cmd = shared_cmd;
		ret = POLL_TRIGGER;
	}
	else
		ret = POLL_TIME_OUT;

	return ret;
}

//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::execute_cmd and two unary predicates
//
// description :
//		This method is called when a command has been received. It execute the command!
//
// args :
//		in :
// 			- w : The work item
//
//------------------------------------------------------------------------------------------------------------------

bool pred_dev(const WorkItem &w)
{
	return w.dev == PollThread::dev_to_del;
}

void PollThread::execute_cmd()
{
	WorkItem wo;
	std::list<WorkItem>::iterator ite;
	std::vector<WorkItem>::iterator et_ite;

	switch (local_cmd.cmd_code)
	{

//
// Add a new object
//

	case Tango::POLL_ADD_OBJ :
    {
		cout5 << "Received a Add object command" << std::endl;

        wo.dev = local_cmd.dev;
        wo.poll_list = &(wo.dev->get_poll_obj_list());
        auto new_upd = (*wo.poll_list)[local_cmd.index]->get_upd();
        PollObjType new_type = (*wo.poll_list)[local_cmd.index]->get_type();

        bool found = false;
        if (new_type == POLL_ATTR && wo.dev->get_dev_idl_version() >= 4 && polling_bef_9 == false)
        {
            ite = find_if(works.begin(),works.end(),
                [&] (const WorkItem &wi) {return wi.dev == local_cmd.dev && wi.update == new_upd && wi.type == new_type;});
            if (ite != works.end())
            {
                 ite->name.push_back((*wo.poll_list)[local_cmd.index]->get_name());
                 found = true;
            }
        }

        if (found == false)
        {
            wo.type = new_type;
            wo.update = new_upd;
            wo.name.push_back((*wo.poll_list)[local_cmd.index]->get_name());
            wo.needed_time = PollClock::duration::zero();

            if (wo.update != PollClock::duration::zero())
            {
                wo.wake_up_date = now;
                if (local_cmd.new_upd != PollClock::duration::zero())
                {
                    cout5 << "Received a delta from now of "
                        << std::fixed << duration_ms(local_cmd.new_upd) << " ms"
                        << std::endl;
                    wo.wake_up_date += local_cmd.new_upd;
                }
                insert_in_list(wo);
                unsigned long nb_works = works.size();
                tune_ctr = (nb_works << 2);
                need_two_tuning = true;
            }
            else
            {
                wo.wake_up_date = {};
                ext_trig_works.push_back(wo);
            }
        }
		break;
    }

//
// Remove an already polled object
//

	case Tango::POLL_REM_OBJ :
		cout5 << "Received a Rem object command" << std::endl;

		dev_to_del = local_cmd.dev;
		name_to_del = local_cmd.name;
		type_to_del = local_cmd.type;

		size_t i,nb_elt;
		nb_elt = works.size();
		ite = works.begin();
		for (i = 0;i < nb_elt;i++)
		{
			if (ite->dev == PollThread::dev_to_del)
			{
				if (ite->type == PollThread::type_to_del)
				{
				    std::vector<std::string>::iterator ite_str;
				    bool found = false;
				    for (ite_str = ite->name.begin();ite_str != ite->name.end();++ite_str)
                    {
                        if (*ite_str == PollThread::name_to_del)
                        {
                            ite->name.erase(ite_str);
                            if (ite->name.empty() == true)
                                works.erase(ite);
                            found = true;
                            break;
                        }
                    }
                    if (found == true)
                        break;
				}
			}
			++ite;
		}
		break;

//
// Remove an already externally triggered polled object
//

	case Tango::POLL_REM_EXT_TRIG_OBJ :
		cout5 << "Received a Ext Trig Rem object command" << std::endl;

		for (et_ite = ext_trig_works.begin();
		     et_ite != ext_trig_works.end();++et_ite)
		{
			if (et_ite->dev == local_cmd.dev)
			{
				if (et_ite->type == local_cmd.type)
				{
					if (et_ite->name[0] == local_cmd.name)
					{
						ext_trig_works.erase(et_ite);
						break;
					}
				}
			}
		}
		break;

//
// Remove all objects belonging to a device.
// Take care, the same device could have several objects --> No break after the successfull if in loop
//

	case Tango::POLL_REM_DEV :
		cout5 << "Received a Rem device command" << std::endl;

		dev_to_del = local_cmd.dev;
#ifdef _TG_WINDOWS_
		nb_elt = works.size();
		ite = works.begin();
		for (i = 0;i < nb_elt;i++)
		{
			if (ite->dev == PollThread::dev_to_del)
			{
				ite = works.erase(ite);
			}
			else
				++ite;
		}
		nb_elt = ext_trig_works.size();
		et_ite = ext_trig_works.begin();
		for (i = 0;i < nb_elt;i++)
		{
			if (et_ite->dev == PollThread::dev_to_del)
			{
				et_ite = ext_trig_works.erase(et_ite);
			}
			else
				++et_ite;
		}
#else
		works.remove_if(pred_dev);

		ext_trig_works.erase(remove_if(ext_trig_works.begin(),
					       ext_trig_works.end(),
					       pred_dev),
				     ext_trig_works.end());
#endif

		break;

//
// Update polling period
// Several cases has to be implemented here.
// 1 - A classical command from the external world. In this case updating
//     polling period means removing the already inserted object from the work list,
//     compute its new polling time and insert it in the work list with its new
//     polling time and polling period
// 2 - This is executed by the polling thread itself
// 2-1 - The command updates polling period for another object: idem than previous
// 2-2 - The commands updates polling period for the object it is actually polled.
//       In this case, the object is not in the work list. It has been removed from the
//	 work list at the beginning of the "one_more_poll" method and is memorized there
//	 Therefore, simply stores new polling period in a data member. The "one_more_poll"
//	 method will get its new polling period before re-inserting the object in the work
// 	 list with the new update period.
//	 We detect this case because the object is not in any work list (either the work
//	 list or the trigger list)
//

	case Tango::POLL_UPD_PERIOD :
    {
		cout5 << "Received a update polling period command" << std::endl;

		dev_to_del = local_cmd.dev;
		name_to_del = local_cmd.name;
		type_to_del = local_cmd.type;

		bool found_in_work_list = false;

        if (local_cmd.new_upd != PollClock::duration::zero())
        {
            WorkItem tmp_work;
            size_t i,nb_elt;
            nb_elt = works.size();
            ite = works.begin();

            if (nb_elt != 0)
            {
                bool found = false;

                for (i = 0;i < nb_elt;i++)
                {
                    if (ite->dev == PollThread::dev_to_del)
                    {
                        if (ite->type == PollThread::type_to_del)
                        {

                            std::vector<std::string>::iterator ite_str;
                            for (ite_str = ite->name.begin();ite_str != ite->name.end();++ite_str)
                            {
                                if (*ite_str == PollThread::name_to_del)
                                {
                                    ite->name.erase(ite_str);
                                    if (ite->name.empty() == true)
                                    {
                                        works.erase(ite);
                                    }

                                    found = true;
                                    found_in_work_list = true;
                                    break;
                                }
                            }

                            if (found == true)
                                break;
                        }
                    }
                    ++ite;
                }

                tmp_work.dev = PollThread::dev_to_del;
                tmp_work.poll_list = &(tmp_work.dev->get_poll_obj_list());
                tmp_work.type = PollThread::type_to_del;
                tmp_work.update = local_cmd.new_upd;
                tmp_work.name.push_back(PollThread::name_to_del);
                tmp_work.needed_time = PollClock::duration::zero();
                now += local_cmd.new_upd;
                tmp_work.wake_up_date = now;
                add_insert_in_list(tmp_work);
                tune_ctr = 0;
                found_in_work_list = true;

                if (found == false)
                {
                    rem_upd.emplace_back(local_cmd.new_upd, PollThread::name_to_del);
                }

            }
        }
        else
        {

//
// First, remove object from polling list and insert it in externally triggered list
//

            size_t i,nb_elt;
            nb_elt = works.size();
            ite = works.begin();
            for (i = 0;i < nb_elt;i++)
            {
                if (ite->dev == PollThread::dev_to_del)
                {
                    if (ite->type == PollThread::type_to_del)
                    {
                        bool found = false;
                        std::vector<std::string>::iterator ite_str;
                        for (ite_str = ite->name.begin();ite_str != ite->name.end();++ite_str)
                        {
                            if (*ite_str == PollThread::name_to_del)
                            {
                                ite->name.erase(ite_str);
                                if (ite->name.empty() == true)
                                    works.erase(ite);
                                found = true;
                                found_in_work_list = true;
                                break;
                            }
                        }
                        if (found == true)
                            break;
                    }
                }
                ++ite;
            }

            wo.dev = local_cmd.dev;
            wo.poll_list = &(wo.dev->get_poll_obj_list());
            wo.type = (*wo.poll_list)[local_cmd.index]->get_type();
            wo.update = (*wo.poll_list)[local_cmd.index]->get_upd();
            wo.name.push_back((*wo.poll_list)[local_cmd.index]->get_name());
            wo.wake_up_date = {};

            ext_trig_works.push_back(wo);
        }

//
// If not found in work list, it should be in the externally triggered object. Therefore, remove it from externally
// triggered list and insert it in work list. If not found in work list and in trig list, we are in case
// 2-2 as described above (polling thread itself updating polling period of the object it actually polls)
//

        if (found_in_work_list == false)
        {
			bool found = false;
			for (et_ite = ext_trig_works.begin();
			     et_ite != ext_trig_works.end();++et_ite)
			{
				if (et_ite->dev == local_cmd.dev)
				{
					if (et_ite->type == local_cmd.type)
					{
						if (et_ite->name[0] == local_cmd.name)
						{
							ext_trig_works.erase(et_ite);
							found = true;
							break;
						}
					}
				}
			}

			if (found == true)
			{
				wo.dev = local_cmd.dev;
				wo.poll_list = &(wo.dev->get_poll_obj_list());
				wo.type = type_to_del;
				wo.update = local_cmd.new_upd;
				wo.name.push_back(name_to_del);
				wo.wake_up_date = now;
				insert_in_list(wo);
			}
			else
			{
				auto_upd.emplace_back(local_cmd.new_upd, name_to_del);
			}
		}
		break;
    }

//
// Add the event heartbeat every 9 seconds
//

	case Tango::POLL_ADD_HEARTBEAT:
		cout5 << "Received a add heartbeat command" << std::endl;
		wo.dev = NULL;
		wo.poll_list = NULL;
		wo.type = EVENT_HEARTBEAT;
		wo.update = std::chrono::milliseconds(9000);
		wo.name.push_back(std::string("Event heartbeat"));
		wo.needed_time = TIME_NEEDED_HEARTBEAT;

		wo.wake_up_date = now;
		insert_in_list(wo);
		break;

//
// Remove the event heartbeat
//

	case Tango::POLL_REM_HEARTBEAT:
		cout5 << "Received a remove heartbeat command" << std::endl;
		unsigned int ii,nb_elem;
		nb_elem = works.size();
		ite = works.begin();

		for (ii = 0;ii < nb_elem;ii++)
		{
			if (ite->type == EVENT_HEARTBEAT)
			{
				works.erase(ite);
				break;
			}
			++ite;
		}
		break;

//
// Start polling
//

	case Tango::POLL_START :
		cout5 << "Received a Start polling command" << std::endl;
		polling_stop = false;
		break;

//
// Stop polling
//

	case Tango::POLL_STOP :
		cout5 << "Received a Stop polling command" << std::endl;
		polling_stop = true;
		break;

//
// Ask polling thread to exit
//

	case Tango::POLL_EXIT :
		cout5 << "Received an exit command" << std::endl;
		omni_thread::exit();
		break;
	}

//
// Inform requesting thread that the work is done
//


	{
		omni_mutex_lock sync(p_mon);
		shared_cmd.cmd_pending = false;
		p_mon.signal();
	}

	if (Tango::Util::_tracelevel >= 4)
		print_list();

}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::one_more_poll
//
// description :
//
//-------------------------------------------------------------------------------------------------------------------

void PollThread::one_more_poll()
{
	WorkItem tmp = works.front();
	works.pop_front();

	if (polling_stop == false)
	{
		switch (tmp.type)
		{
		case Tango::POLL_CMD:
			poll_cmd(tmp);
			break;

		case Tango::POLL_ATTR:
			poll_attr(tmp);
			break;

		case Tango::EVENT_HEARTBEAT:
			eve_heartbeat();
			heartbeat_ctr++;
			if (heartbeat_ctr % 3 == 0)
				auto_unsub();
			break;

		case Tango::STORE_SUBDEV:
			store_subdev();
			break;
		}
	}

//
// For case where the polling thread itself modify the polling period of the object it already polls
//

    if (auto_upd.empty() == false)
    {
        for (const auto& entry : auto_upd)
        {
            auto pos = remove(tmp.name.begin(),tmp.name.end(), entry.second);
            tmp.name.erase(pos,tmp.name.end());
        }

        if (tmp.name.empty() == false)
        {
            tmp.wake_up_date += tmp.update;
            insert_in_list(tmp);
        }

        for (const auto& entry : auto_upd)
        {
            size_t nb_elt = works.size();
            auto ite = works.begin();

            bool found = false;

            for (size_t i = 0;i < nb_elt;i++)
            {
                if (ite->dev == tmp.dev &&
                    ite->type == tmp.type &&
                    ite->update == entry.first)
                {
                    ite->name.push_back(entry.second);
                    found = true;
                    break;
                }
                ++ite;
            }

            if (found == false)
            {
                WorkItem new_tmp;
                new_tmp.update = entry.first;
                new_tmp.name.push_back(entry.second);
                new_tmp.dev = tmp.dev;
                new_tmp.poll_list = tmp.poll_list;
                new_tmp.type = tmp.type;
                new_tmp.needed_time = PollClock::duration::zero();
                now += local_cmd.new_upd;
                new_tmp.wake_up_date = now;
                insert_in_list(new_tmp);
            }
        }

        auto_upd.clear();
    }

//
// Compute new polling date and insert work in list
//

    else
    {
        if (rem_upd.empty() == false)
        {
            for (const auto& entry : rem_upd)
            {
                auto pos = remove(tmp.name.begin(), tmp.name.end(), entry.second);
                tmp.name.erase(pos, tmp.name.end());
            }

            if (tmp.name.empty() == false)
            {
                tmp.wake_up_date += tmp.update;
                insert_in_list(tmp);
            }

            rem_upd.clear();
        }
        else
        {
            tmp.wake_up_date += tmp.update;
            insert_in_list(tmp);
        }
    }

    tune_ctr--;
}

//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::one_more_trigg
//
// description :
//		This method is called when a trigger command has been received
//
//----------------------------------------------------------------------------------------------------------------


void PollThread::one_more_trigg()
{
	cout5 << "Polling thread has received a trigger" << std::endl;

//
// Check that the object is registered
//

	dev_to_del = local_cmd.dev;
	name_to_del = local_cmd.name;
	type_to_del = local_cmd.type;

	std::vector<WorkItem>::iterator et_ite;
	for (et_ite = ext_trig_works.begin();et_ite != ext_trig_works.end();++et_ite)
    {
        if (et_ite->dev == PollThread::dev_to_del)
        {
            if (et_ite->type == PollThread::type_to_del)
            {
                if (et_ite->name[0] == PollThread::name_to_del)
                    break;
            }
        }
    }

//
// Check that the object to poll has been installed. If not, simply returns. This case should never happens because
// it is tested in the Util::trigger_polling() method before the trigger is effectively sent to this thread.
//

	if (et_ite == ext_trig_works.end())
	{
		cout5 << "Object externally triggered not found !!!" << std::endl;
		{
			omni_mutex_lock sync(p_mon);
			shared_cmd.trigger = false;
			p_mon.signal();
		}
		return;
	}

//
// Do the job
//

	WorkItem tmp = *et_ite;
	if (polling_stop == false)
	{
		if (tmp.type == Tango::POLL_CMD)
			poll_cmd(tmp);
		else
			poll_attr(tmp);
	}

//
// Inform requesting thread that the work is done
//

	{
		omni_mutex_lock sync(p_mon);
		shared_cmd.trigger = false;
		p_mon.signal();
	}
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::print_list
//
// description :
//		To print work list
//
//-----------------------------------------------------------------------------------------------------------------

void PollThread::print_list()
{
	std::list<WorkItem>::iterator ite;
	long nb_elt,i;

	nb_elt = works.size();
	ite = works.begin();
	for (i = 0;i < nb_elt;i++)
	{
		if (ite->type != EVENT_HEARTBEAT )
		{
			if ( ite->type != STORE_SUBDEV)
			{
				std::string obj_list;
				for (size_t ctr = 0;ctr < ite->name.size();ctr++)
				{
					obj_list = obj_list + ite->name[ctr];
					if (ctr < (ite->name.size() - 1))
					{
						obj_list = obj_list + ", ";
					}
				}

				cout5 << "Dev name = " << ite->dev->get_name() << ", obj name = " << obj_list << ", next wake_up at "
					<< std::fixed << duration_s(ite->wake_up_date.time_since_epoch()) << " s "
					<< std::fixed << "(in " << duration_ms(ite->wake_up_date - PollClock::now()) << " ms)"
					<< std::endl;
			}
			else
			{
				cout5 << ite->name[0] << ", next wake_up at "
					<< std::fixed << duration_s(ite->wake_up_date.time_since_epoch()) << " s "
					<< std::fixed << "(in " << duration_ms(ite->wake_up_date - PollClock::now()) << " ms)"
					<< std::endl;
			}
		}
		else
		{
			cout5 << "Event heartbeat, next wake_up at "
			<< std::fixed << duration_s(ite->wake_up_date.time_since_epoch()) << " s "
			<< std::fixed << "(in " << duration_ms(ite->wake_up_date - PollClock::now()) << " ms)"
			<< std::endl;
		}

		++ite;
	}
}


//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::insert_in_list
//
// description :
//		To insert (at the correct place) a new Work Item in the work list
//
// args :
//		in :
// 			- new_work : The new work item
//
//-----------------------------------------------------------------------------------------------------------------

void PollThread::insert_in_list(WorkItem &new_work)
{
	std::list<WorkItem>::iterator ite;
	for (ite = works.begin();ite != works.end();++ite)
	{
		if (ite->wake_up_date < new_work.wake_up_date)
		{
			continue;
		}
		else
		{
			works.insert(ite,new_work);
			return;
		}
	}
	if (ite == works.end())
		works.push_back(new_work);
}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::add_insert_in_list
//
// description :
//		To insert (at the correct place) a new Work Item in the work list but before this for attribute, if we
//      already have a work item with the same device at the same period, simply add the obj name in it.
//      (in order to use read_attributes() in a optimal way)
//
// args :
//		in :
// 			- new_work : The new work item
//
//-----------------------------------------------------------------------------------------------------------------

void PollThread::add_insert_in_list(WorkItem &new_work)
{
    if (new_work.type == POLL_ATTR && new_work.dev->get_dev_idl_version() >= 4 && polling_bef_9 == false)
    {
        std::list<WorkItem>::iterator ite;
        ite = find_if(works.begin(),works.end(),
                [&] (const WorkItem &wi) {return wi.dev == new_work.dev && wi.update == new_work.update && wi.type == new_work.type;});

        if (ite != works.end())
        {
            ite->name.push_back(new_work.name[0]);
        }
        else
            insert_in_list(new_work);
    }
    else
        insert_in_list(new_work);
}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::tune_list
//
// description :
//		This method tunes the work list.
//
// args :
//		in :
// 			- from_needed : Set to true if the delta between work should be at least equal to the
//							time needed to execute the previous work
//
//----------------------------------------------------------------------------------------------------------------

void PollThread::tune_list(bool from_needed)
{
	std::list<WorkItem>::iterator ite,ite_next,ite_prev;

	unsigned long nb_works = works.size();
	cout4 << "Entering tuning list. The list has " << nb_works << " item(s)" << std::endl;

//
// Nothing to do if only one let in list
//

	if (nb_works < 2)
		return;

//
// If we try to tune the list with respect to works needed time, compute works needed time sum and find minimun update
// period
//

	if (from_needed == true)
	{
		PollClock::duration needed_sum = PollClock::duration::zero();
		PollClock::duration min_upd = PollClock::duration::zero();

		for (ite = works.begin();ite != works.end();++ite)
		{
			needed_sum += ite->needed_time;

			auto update_usec = ite->update;

			if (ite == works.begin())
			{
				min_upd = update_usec;
			}
			else
			{
				if (min_upd > update_usec)
				{
					min_upd = update_usec;
				}
			}
		}

//
// In some cases, it is impossible to tune
//

		if (needed_sum > min_upd)
		{
			return;
		}

		auto sleeping = min_upd - needed_sum;
		auto max_delta_needed = sleeping / nb_works;

//
// Now build a new tuned list
//
// To tune the list
// - Take obj j and compute when it should be polled (next_work)
// - Compute when object j-1 should be polled (prev_obj_work)
// - Compute the number of poll between these two dates (n)
// - Compute date of previous object polling just before "next_work"
// - Assign next_work to this date and add
//       the time needed to execute previous object polling
//		 the delta computed from the smallest upd and the obj number
//

		auto next_tuning = now + (POLL_LOOP_NB * min_upd);

		std::list<WorkItem> new_works;
		new_works.push_front(works.front());

		ite = works.begin();
		ite_prev = new_works.begin();

		for (++ite;ite != works.end();++ite,++ite_prev)
		{
			auto needed_time_usec = ite_prev->needed_time;
			WorkItem wo = *ite;
			auto next_work = wo.wake_up_date;

			PollClock::time_point next_prev;
			if (next_work < next_tuning)
			{
				auto prev_obj_work = ite_prev->wake_up_date;
				if (next_work > prev_obj_work)
				{
					// Explicit calculation of n (as integer) is needed and cannot be skipped.
					auto n = std::uint64_t((next_work - prev_obj_work) / ite_prev->update);
					next_prev = prev_obj_work + (n * ite_prev->update);
				}
				else
					next_prev = prev_obj_work;

				wo.wake_up_date = next_prev;

				wo.wake_up_date += (needed_time_usec + max_delta_needed);
			}
			new_works.push_back(wo);
		}

//
// Replace work list
//

		works = new_works;
	}
	else
	{
		ite_next = works.begin();
		ite = ite_next;
		++ite_next;

		for (unsigned int i = 1;i < nb_works;i++)
		{
			auto diff = ite_next->wake_up_date - ite->wake_up_date;

//
// If delta time between works is less than min, shift following work
//

			if (diff < PollClock::duration::zero())
				ite_next->wake_up_date -= diff;

			++ite;
			++ite_next;
		}
	}

	cout4 << "Tuning list done" << std::endl;
	print_list();
}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::compute_sleep_time
//
// description :
//		This method computes how many mS the thread should sleep before the next poll time. If this time is
//		negative and greater than a pre-defined threshold, the polling is discarded.
//
//----------------------------------------------------------------------------------------------------------------

void PollThread::compute_sleep_time()
{
	print_list();

	if (works.empty() == false)
	{
        bool discard = false;
        u_int nb_late = 0;

        if (polling_bef_9 == false)
        {

//
// Compute for how many items the polling thread is late
//

            std::list<WorkItem>::iterator ite;

            for (ite = works.begin();ite != works.end();++ite)
            {
                auto next = ite->wake_up_date;
                if ((after - next) > DISCARD_THRESHOLD)
                {
                    nb_late++;
                }
            }

//
// If we are late for some item(s):
//  - Seriously late (number of late items equal number of items) --> We will discard items
//  - Late for the first time: Poll immediately but memorize the number of items for which we are late
//  - Late again: If the number of late items increase --> Discard items
//

            if (nb_late != 0)
            {
                if (nb_late == works.size())
                {
                    cout5 << "Setting discard to true because nb_late == works.size() --> " << nb_late << std::endl;
                    discard = true;
                }
                else
                {
                    if (previous_nb_late != 0)
                    {
                        if (nb_late < previous_nb_late)
                        {
                            previous_nb_late = nb_late;
                            cout5 << "Late but trying to catch up"  << std::endl;
                        }
                        else
                        {
                            previous_nb_late = 0;
                            discard = true;
                        }
                    }
                    else
                        previous_nb_late = nb_late;
                    sleep = tango_nullopt;
                }
            }
        }
        else
            discard = true;

//
// Analyse work list
//

//        cout5 << "discard = " << boolalpha << discard << std::endl;
        if (nb_late == 0 || discard == true)
        {
            previous_nb_late = 0;

            auto next = works.front().wake_up_date;

            if (next < after)
            {
                if (after - next < DISCARD_THRESHOLD)
                {
                    sleep = tango_nullopt;
                }
                else
                {
                    while (after - next > DISCARD_THRESHOLD)
                    {
                        cout5 << "Discard one elt !!!!!!!!!!!!!" << std::endl;
                        WorkItem tmp = works.front();
                        if (tmp.type == POLL_ATTR)
                        {
                            err_out_of_sync(tmp);
                        }

                        tmp.wake_up_date += tmp.update;
                        insert_in_list(tmp);
                        works.pop_front();
                        tune_ctr--;

                        next = works.front().wake_up_date;
                    }

                    if (duration_abs(next - after) < DISCARD_THRESHOLD)
                    {
                        sleep = tango_nullopt;
                    }
                    else
                    {
                        sleep = next - after;
                    }
                }
            }
            else
            {
                sleep = next - after;
            }
        }

        if (sleep.has_value())
        {
            cout5 << "Sleep for : " << std::fixed << duration_s(*sleep) << "s" << std::endl;
        }
        else
        {
            cout5 << "Sleep for : -1 (undefined)" << std::endl;
        }
	}
}


//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::err_out_of_sync
//
// description :
//		To force one event if the polling thread has discarded one work item because it is late
//
// args :
//		in :
// 			- to_do : The work item
//
//----------------------------------------------------------------------------------------------------------------

void PollThread::err_out_of_sync(WorkItem &to_do)
{
	EventSupplier *event_supplier_nd = NULL;
	EventSupplier *event_supplier_zmq = NULL;

//
// Retrieve the event supplier(s) for this attribute
//

    size_t nb_obj = to_do.name.size();
    for (size_t ctr = 0;ctr < nb_obj;ctr++)
    {
        Attribute &att = to_do.dev->get_device_attr()->get_attr_by_name(to_do.name[ctr].c_str());

        if (att.use_notifd_event() == true && event_supplier_nd == NULL)
            event_supplier_nd = Util::instance()->get_notifd_event_supplier();
        if (att.use_zmq_event() == true && event_supplier_zmq == NULL)
            event_supplier_zmq = Util::instance()->get_zmq_event_supplier();

        if ((event_supplier_nd != NULL) || (event_supplier_zmq != NULL))
        {
            Tango::DevErrorList errs;
            errs.length(1);

            errs[0].severity = Tango::ERR;
            errs[0].reason = Tango::string_dup("API_PollThreadOutOfSync");
            errs[0].origin = Tango::string_dup("PollThread::err_out_of_sync");
            errs[0].desc = Tango::string_dup("The polling thread is late and discard this object polling.\nAdvice: Tune device server polling");

            Tango::DevFailed except(errs);
            long idl_vers = to_do.dev->get_dev_idl_version();

            struct EventSupplier::SuppliedEventData ad;
            ::memset(&ad,0,sizeof(ad));

            if (idl_vers > 4)
                ad.attr_val_5 = &dummy_att5;
            else if (idl_vers == 4)
                ad.attr_val_4 = &dummy_att4;
            else if (idl_vers == 3)
                ad.attr_val_3 = &dummy_att3;
            else
                ad.attr_val = &dummy_att;

//
// Fire event
//

            SendEventType send_event;
            if (event_supplier_nd != NULL)
                send_event = event_supplier_nd->detect_and_push_events(
                    to_do.dev,
                    ad,
                    &except,
                    to_do.name[ctr],
                    PollClock::now());
            if (event_supplier_zmq != NULL)
            {
                if (event_supplier_nd != NULL)
                {
                    std::vector<std::string> f_names;
                    std::vector<double> f_data;
                    std::vector<std::string> f_names_lg;
                    std::vector<long> f_data_lg;

                    if (send_event.change == true)
                        event_supplier_zmq->push_event_loop(to_do.dev,CHANGE_EVENT,f_names,f_data,f_names_lg,f_data_lg,ad,att,&except);
                    if (send_event.archive == true)
                        event_supplier_zmq->push_event_loop(to_do.dev,ARCHIVE_EVENT,f_names,f_data,f_names_lg,f_data_lg,ad,att,&except);
                    if (send_event.periodic == true)
                        event_supplier_zmq->push_event_loop(to_do.dev,PERIODIC_EVENT,f_names,f_data,f_names_lg,f_data_lg,ad,att,&except);
                }
                else
                    event_supplier_zmq->detect_and_push_events(
                        to_do.dev,
                        ad,
                        &except,
                        to_do.name[ctr],
                        PollClock::now());
            }
        }
	}
}


//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::poll_cmd
//
// description :
//		Execute a command and store the result in the device ring buffer
//
// args :
//		in :
// 			- to_do : The work item
//
//----------------------------------------------------------------------------------------------------------------

void PollThread::poll_cmd(WorkItem &to_do)
{
	cout5 << "----------> Time = " << std::fixed << duration_s(now.time_since_epoch()) << " s"
		<< ", Dev name = " << to_do.dev->get_name()
		<< ", Cmd name = " << to_do.name[0]
		<< std::endl;

	CORBA::Any *argout = NULL;
	Tango::DevFailed *save_except = NULL;

	std::vector<PollObj *>::iterator ite;
	bool cmd_failed = false;

	auto before_cmd = PollClock::now();

	try
	{
		argout = to_do.dev->command_inout(to_do.name[0].c_str(),in_any);
	}
	catch (Tango::DevFailed &e)
	{
		cmd_failed = true;
		save_except = new Tango::DevFailed(e);
	}

	auto after_cmd = PollClock::now();
	auto needed_time = after_cmd - before_cmd;
	to_do.needed_time = needed_time;

//
// Insert result in polling buffer and simply forget this command if it is not possible to insert the result in
// polling buffer
//

	try
	{
		to_do.dev->get_poll_monitor().get_monitor();
		ite = to_do.dev->get_polled_obj_by_type_name(to_do.type,to_do.name[0]);
		if (cmd_failed == false)
			(*ite)->insert_data(argout,before_cmd,needed_time);
		else
			(*ite)->insert_except(save_except,before_cmd,needed_time);
		to_do.dev->get_poll_monitor().rel_monitor();
	}
	catch (Tango::DevFailed &)
	{
		if (cmd_failed == false)
			delete argout;
		else
			delete save_except;
		to_do.dev->get_poll_monitor().rel_monitor();
	}
}

//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::poll_attr
//
// description :
//		Read attribute and store the result in the device ring buffer
//
// args :
//		in :
// 			- to_do : The work item
//
//----------------------------------------------------------------------------------------------------------------

void PollThread::poll_attr(WorkItem &to_do)
{
    size_t nb_obj = to_do.name.size();
    std::string att_list;
    for (size_t ctr = 0;ctr < nb_obj;ctr++)
    {
        att_list =  att_list + to_do.name[ctr];
        if (ctr < (nb_obj - 1))
            att_list = att_list + ", ";
    }

	cout5 << "----------> Time = " << std::fixed << duration_s(now.time_since_epoch()) << " s"
		<< ", Dev name = " << to_do.dev->get_name()
		<< ", Attr name = " << att_list
		<< std::endl;

	Tango::AttributeValueList *argout = NULL;
	Tango::AttributeValueList_3 *argout_3 = NULL;
	Tango::AttributeValueList_4 *argout_4 = NULL;
	Tango::AttributeValueList_5 *argout_5 = NULL;
	Tango::DevFailed *save_except = NULL;
	bool attr_failed = false;
	std::vector<PollObj *>::iterator ite;
	std::map<size_t,Tango::DevFailed *> map_except;

	long idl_vers = to_do.dev->get_dev_idl_version();

	attr_names.length(nb_obj);
	for (size_t ctr = 0;ctr < nb_obj;ctr++)
	{
		attr_names[ctr] = to_do.name[ctr].c_str();
	}

	auto before_cmd = PollClock::now();

	try
	{
		if (idl_vers >= 5)
			argout_5 = (static_cast<Device_5Impl *>(to_do.dev))->read_attributes_5(attr_names,Tango::DEV,dummy_cl_id);
		else if (idl_vers == 4)
			argout_4 = (static_cast<Device_4Impl *>(to_do.dev))->read_attributes_4(attr_names,Tango::DEV,dummy_cl_id);
		else if (idl_vers == 3)
			argout_3 = (static_cast<Device_3Impl *>(to_do.dev))->read_attributes_3(attr_names,Tango::DEV);
		else
			argout = to_do.dev->read_attributes(attr_names);
	}
	catch (Tango::DevFailed &e)
	{
		attr_failed = true;
		save_except = new Tango::DevFailed(e);
	}

	auto after_cmd = PollClock::now();
	auto needed_time = after_cmd - before_cmd;
	to_do.needed_time = needed_time;

//
// Starting with IDl release 3, an attribute in error is not an exception any more. Re-create one.
// Don't forget that it is still possible to receive classical exception (in case of Monitor timeout for instance)
//

	if (idl_vers >= 3)
	{
		if (idl_vers >= 5)
		{
		    if (nb_obj == 1)
            {
                if ((attr_failed == false) && ((*argout_5)[0].err_list.length() != 0))
                {
                    attr_failed = true;
                    save_except = new Tango::DevFailed((*argout_5)[0].err_list);
                    delete argout_5;
                }
            }
            else
            {
                for (size_t ctr = 0;ctr < nb_obj;ctr++)
                {
                    if ((attr_failed == false) && ((*argout_5)[ctr].err_list.length() != 0))
                    {
                        Tango::DevFailed *tmp_except = new Tango::DevFailed((*argout_5)[ctr].err_list);
                        map_except.insert(std::pair<size_t,Tango::DevFailed *>(ctr,tmp_except));
                    }
                }
            }
		}
		else if (idl_vers == 4)
		{
		    if (nb_obj == 1)
            {
                if ((attr_failed == false) && ((*argout_4)[0].err_list.length() != 0))
                {
                    attr_failed = true;
                    save_except = new Tango::DevFailed((*argout_4)[0].err_list);
                    delete argout_4;
                }
            }
            else
            {
                for (size_t ctr = 0;ctr < nb_obj;ctr++)
                {
                    if ((attr_failed == false) && ((*argout_4)[ctr].err_list.length() != 0))
                    {
                        Tango::DevFailed *tmp_except = new Tango::DevFailed((*argout_4)[ctr].err_list);
                        map_except.insert(std::pair<size_t,Tango::DevFailed *>(ctr,tmp_except));
                    }
                }
            }
		}
		else
		{
			if ((attr_failed == false) && ((*argout_3)[0].err_list.length() != 0))
			{
				attr_failed = true;
				save_except = new Tango::DevFailed((*argout_3)[0].err_list);
				delete argout_3;
			}
		}
	}

//
// Events - for each event call the detect_and_push() method this method will fire events if there are clients
// registered and if there is an event (on_change, on_alarm or periodic)
// We also have to retrieve which kind of clients made the subscription (zmq or notifd) and send the event accordingly
//

	EventSupplier *event_supplier_nd = NULL;
	EventSupplier *event_supplier_zmq = NULL;

    for (size_t ctr = 0;ctr < nb_obj;ctr++)
    {
        Attribute &att = to_do.dev->get_device_attr()->get_attr_by_name(to_do.name[ctr].c_str());

        if (att.use_notifd_event() == true && event_supplier_nd == NULL)
            event_supplier_nd = Util::instance()->get_notifd_event_supplier();
        if (att.use_zmq_event() == true && event_supplier_zmq == NULL)
            event_supplier_zmq = Util::instance()->get_zmq_event_supplier();

        if ((event_supplier_nd != NULL) || (event_supplier_zmq != NULL))
        {
            if (attr_failed == true)
            {
                struct EventSupplier::SuppliedEventData ad;
                ::memset(&ad,0,sizeof(ad));

                if (idl_vers > 4)
                    ad.attr_val_5 = &dummy_att5;
                else if (idl_vers == 4)
                    ad.attr_val_4 = &dummy_att4;
                else if (idl_vers == 3)
                    ad.attr_val_3 = &dummy_att3;
                else
                    ad.attr_val = &dummy_att;

//
// Eventually push the event (if detected). When we have both notifd and zmq event supplier, do not detect the event
// two times. The detect_and_push_events() method returns true if the event is detected.
//

                SendEventType send_event;
                if (event_supplier_nd != NULL)
                    send_event = event_supplier_nd->detect_and_push_events(to_do.dev,ad,save_except,to_do.name[ctr], before_cmd);
                if (event_supplier_zmq != NULL)
                {
                    if (event_supplier_nd != NULL)
                    {
                        std::vector<std::string> f_names;
                        std::vector<double> f_data;
                        std::vector<std::string> f_names_lg;
                        std::vector<long> f_data_lg;

                        if (send_event.change == true)
                            event_supplier_zmq->push_event_loop(to_do.dev,CHANGE_EVENT,f_names,f_data,f_names_lg,f_data_lg,ad,att,save_except);
                        if (send_event.archive == true)
                            event_supplier_zmq->push_event_loop(to_do.dev,ARCHIVE_EVENT,f_names,f_data,f_names_lg,f_data_lg,ad,att,save_except);
                        if (send_event.periodic == true)
                            event_supplier_zmq->push_event_loop(to_do.dev,PERIODIC_EVENT,f_names,f_data,f_names_lg,f_data_lg,ad,att,save_except);
                    }
                    else
                        event_supplier_zmq->detect_and_push_events(to_do.dev,ad,save_except,to_do.name[ctr], before_cmd);
                }
            }
            else
            {
                struct EventSupplier::SuppliedEventData ad;
                ::memset(&ad,0,sizeof(ad));

                if (idl_vers > 4)
                    ad.attr_val_5 = &((*argout_5)[ctr]);
                else if (idl_vers == 4)
                    ad.attr_val_4 = &((*argout_4)[ctr]);
                else if (idl_vers == 3)
                    ad.attr_val_3 = &((*argout_3)[ctr]);
                else
                    ad.attr_val = &((*argout)[ctr]);

//
// Eventually push the event (if detected). When we have both notifd and zmq event supplier, do not detect the event
// two times. The detect_and_push_events() method returns true if the event is detected.
//

                SendEventType send_event;

                std::map<size_t,Tango::DevFailed *>::iterator ite2 = map_except.find(ctr);
                Tango::DevFailed *tmp_except;
                if (ite2 == map_except.end())
                    tmp_except = save_except;
                else
                    tmp_except = ite2->second;

                if (event_supplier_nd != NULL)
                    send_event = event_supplier_nd->detect_and_push_events(to_do.dev,ad,tmp_except,to_do.name[ctr], before_cmd);
                if (event_supplier_zmq != NULL)
                {
                    if (event_supplier_nd != NULL)
                    {
                        std::vector<std::string> f_names;
                        std::vector<double> f_data;
                        std::vector<std::string> f_names_lg;
                        std::vector<long> f_data_lg;

                        if (send_event.change == true)
                            event_supplier_zmq->push_event_loop(to_do.dev,CHANGE_EVENT,f_names,f_data,f_names_lg,f_data_lg,ad,att,tmp_except);
                        if (send_event.periodic == true)
                            event_supplier_zmq->push_event_loop(to_do.dev,PERIODIC_EVENT,f_names,f_data,f_names_lg,f_data_lg,ad,att,tmp_except);
                        if (send_event.archive == true)
                            event_supplier_zmq->push_event_loop(to_do.dev,ARCHIVE_EVENT,f_names,f_data,f_names_lg,f_data_lg,ad,att,tmp_except);
                    }
                    else
                        event_supplier_zmq->detect_and_push_events(to_do.dev,ad,tmp_except,to_do.name[ctr], before_cmd);
                }
            }
		}
	}


//
// Insert result in polling buffer and simply forget this attribute if it is not possible to insert the result in
// polling buffer
// Different case according to the number of attributes which has been read by this poll.
// The PollObj object are supposed to store data coming from a read_attributes() executed for 1 attribute.
// Sonce Tango 9.0.8, it is not always the case. This is why we differantiate cases when we read several att
// in once call. Sin the last case, re-create memory layout as the one coming from read_attributes called for
// single attribute.
// This new feature is available only for devices with IDL 4 or more
//

	try
	{
		to_do.dev->get_poll_monitor().get_monitor();
		for (size_t ctr = 0;ctr < nb_obj;ctr++)
        {
            ite = to_do.dev->get_polled_obj_by_type_name(to_do.type,to_do.name[ctr]);
            if (attr_failed == false)
            {
                if (nb_obj == 1)
                {
                    if (idl_vers >= 5)
                        (*ite)->insert_data(argout_5,before_cmd,needed_time);
                    else if (idl_vers == 4)
                        (*ite)->insert_data(argout_4,before_cmd,needed_time);
                    else if (idl_vers == 3)
                        (*ite)->insert_data(argout_3,before_cmd,needed_time);
                    else
                        (*ite)->insert_data(argout,before_cmd,needed_time);
                }
                else
                {
                    if (idl_vers >= 5)
                    {
                        std::map<size_t,Tango::DevFailed *>::iterator ite2 = map_except.find(ctr);
                        if (ite2 == map_except.end())
                        {
                            Tango::AttributeValueList_5 *new_argout_5 = new Tango::AttributeValueList_5(1);
                            new_argout_5->length(1);
                            (*new_argout_5)[0].value.union_no_data(true);
                            robb_data((*argout_5)[ctr],(*new_argout_5)[0]);
                            copy_remaining((*argout_5)[ctr],(*new_argout_5)[0]);
                            (*new_argout_5)[0].data_type = (*argout_5)[ctr].data_type;
                            (*ite)->insert_data(new_argout_5,before_cmd,needed_time);
                        }
                        else
                            (*ite)->insert_except(ite2->second,before_cmd,needed_time);
                    }
                    else
                    {
                        std::map<size_t,Tango::DevFailed *>::iterator ite2 = map_except.find(ctr);
                        if (ite2 == map_except.end())
                        {
                            Tango::AttributeValueList_4 *new_argout_4 = new Tango::AttributeValueList_4(1);
                            new_argout_4->length(1);
                            (*new_argout_4)[0].value.union_no_data(true);
                            robb_data((*argout_4)[ctr],(*new_argout_4)[0]);
                            copy_remaining((*argout_4)[ctr],(*new_argout_4)[0]);
                            (*ite)->insert_data(new_argout_4,before_cmd,needed_time);
                        }
                        else
                            (*ite)->insert_except(ite2->second,before_cmd,needed_time);
                    }
                }
            }
            else
            {
                if (nb_obj == 1)
                    (*ite)->insert_except(save_except,before_cmd,needed_time);
                else
                {
                    if (ctr != nb_obj - 1)
                    {
                        Tango::DevFailed *dup_except = new Tango::DevFailed(save_except->errors);
                        (*ite)->insert_except(dup_except,before_cmd,needed_time);
                    }
                    else
                        (*ite)->insert_except(save_except,before_cmd,needed_time);
                }
            }
        }

        if (nb_obj != 1 && attr_failed == false)
        {
            if (idl_vers >= 5)
                delete argout_5;
            else
                delete argout_4;
        }

		to_do.dev->get_poll_monitor().rel_monitor();
	}
	catch (Tango::DevFailed &)
	{
		if (attr_failed == false)
		{
			if (idl_vers >= 5)
				delete argout_5;
			else if (idl_vers == 4)
				delete argout_4;
			else if (idl_vers == 3)
				delete argout_3;
			else
				delete argout;
		}
		else
			delete save_except;
		to_do.dev->get_poll_monitor().rel_monitor();
	}

}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::eve_heartbeat
//
// description :
//		Send the event heartbeat
//
//-------------------------------------------------------------------------------------------------------------------

void PollThread::eve_heartbeat()
{
	cout5 << "----------> Time = " << std::fixed << duration_s(now.time_since_epoch()) << " s"
		<< " Sending event heartbeat" << std::endl;

	EventSupplier *event_supplier;
	event_supplier = Util::instance()->get_zmq_event_supplier();
	if ((event_supplier != NULL) && (send_heartbeat == true) && (event_supplier->get_one_subscription_cmd() == true))
	{
		event_supplier->push_heartbeat_event();
	}

	event_supplier = Util::instance()->get_notifd_event_supplier();
	if ((event_supplier != NULL) && (send_heartbeat == true) && (event_supplier->get_one_subscription_cmd() == true))
	{
		event_supplier->push_heartbeat_event();
	}
}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::store_subdev
//
// description :
//		Store the sub device properties when needed.
//
//-----------------------------------------------------------------------------------------------------------------

void PollThread::store_subdev()
{
	static bool ignore_call = true;

	cout5 << "----------> Time = " << std::fixed << duration_s(now.time_since_epoch()) << " s"
		<< " Store sub device property data if needed!" << std::endl;


	if ( !ignore_call )
	{
		Tango::Util *tg = Tango::Util::instance();
		tg->get_sub_dev_diag().store_sub_devices();
	}
	else
	{
		// ignore the first call to avoid storage during
		// device server start-up.
		ignore_call = false;
	}
}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::auto_unsub
//
// description :
//		Check if we can unsubscribe on user events on forwarded attribute(s)
//
//-----------------------------------------------------------------------------------------------------------------

void PollThread::auto_unsub()
{
	RootAttRegistry &rar = Util::instance()->get_root_att_reg();
	rar.auto_unsub();
}

} // End of Tango namespace
