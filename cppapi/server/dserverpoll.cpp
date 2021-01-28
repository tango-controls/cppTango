//+==================================================================================================================
//
// file :               DServer.cpp
//
// description :        C++ source for the DServer class and its commands. The class is derived from Device. It
//						represents the CORBA servant object which will be accessed from the network.
//						All commands which can be executed on a DServer object are implemented in this file.
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
//-=================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#ifdef _TG_WINDOWS_
	#include <sys/timeb.h>
#else
	#include <sys/time.h>
#endif

#include <iomanip>

namespace Tango
{

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::polled_device()
//
// description :
//		command to read all the devices actually polled by the device server
//
// retrun :
//		The device name list in a strings sequence
//
//-----------------------------------------------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::polled_device()
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In polled_device command" << std::endl;

	long nb_class = class_list.size();
	std::vector<std::string> dev_name;

	try
	{
		for (int i = 0;i < nb_class;i++)
		{
			long nb_dev = class_list[i]->get_device_list().size();
			for (long j = 0;j < nb_dev;j++)
			{
				if ((class_list[i]->get_device_list())[j]->is_polled() == true)
				{
					dev_name.push_back((class_list[i]->get_device_list())[j]->get_name().c_str());
				}
			}
		}
	}
	catch (std::bad_alloc &)
	{
		TANGO_THROW_EXCEPTION(API_MemoryAllocation, "Can't allocate memory in server");
	}

//
// Return an empty sequence if no devices are polled
//

	if (dev_name.empty() == true)
	{
		Tango::DevVarStringArray *ret = new Tango::DevVarStringArray();
		ret->length(0);
		return ret;
	}

//
// Returned device name list to caller (sorted)
//

	sort(dev_name.begin(),dev_name.end());
	long nb_dev = dev_name.size();
	Tango::DevVarStringArray *ret = new Tango::DevVarStringArray(nb_dev);
	ret->length(nb_dev);
	for (long k = 0;k < nb_dev;k++)
		(*ret)[k] = dev_name[k].c_str();

	return(ret);

}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::dev_polled_status()
//
// description :
//		command to read device polling status
//
// args :
//		in :
//			- dev_name : The device name
//
// return :
//		The device polling status as a string (multiple lines)
//
//-----------------------------------------------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::dev_poll_status(std::string &dev_name)
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In dev_poll_status method" << std::endl;

//
// Find the device
//

	Tango::Util *tg = Tango::Util::instance();
	DeviceImpl *dev;

	dev = tg->get_device_by_name(dev_name);

	std::vector<PollObj *> &poll_list = dev->get_poll_obj_list();
	long nb_poll_obj = poll_list.size();

//
// Return an empty sequence if nothing is polled for this device
//

	if (nb_poll_obj == 0)
	{
		Tango::DevVarStringArray *ret;
		ret = new DevVarStringArray();
		ret->length(0);
		return ret;
	}


	long i,nb_cmd,nb_attr;

//
// Compute how many cmds and/or attributes are polled. Since IDL V3, state and status are polled as attributes
// For compatibility, if one of these "attributes" is polled, also returns the info as the command is polled
//

	nb_cmd = nb_attr = 0;
	long nb_to_add = 0;
	for (i = 0;i < nb_poll_obj;i++)
	{
		if (poll_list[i]->get_type() == Tango::POLL_CMD)
			nb_cmd++;
		else
		{
			nb_attr++;
			if ((poll_list[i]->get_name() == "state") || (poll_list[i]->get_name() == "status"))
			{
				nb_cmd++;
				nb_to_add++;
			}
		}
	}

//
// Allocate memory for returned strings
//

	Tango::DevVarStringArray *ret;
	ret = new DevVarStringArray(nb_poll_obj + nb_to_add);
	ret->length(nb_poll_obj + nb_to_add);

//
// Create map of polled attributes read by the same call
//

    std::map<int,std::vector<std::string> > polled_together;

    bool poll_bef_9 = false;
    if (polling_bef_9_def == true)
        poll_bef_9 = polling_bef_9;
    else if (tg->is_polling_bef_9_def() == true)
        poll_bef_9 = tg->get_polling_bef_9();

    if (poll_bef_9 == false)
    {
        for (i = 0;i < nb_poll_obj;i++)
        {
            if (poll_list[i]->get_type() == Tango::POLL_CMD)
                continue;
            else
            {
                long po = poll_list[i]->get_upd();
                std::map<int,std::vector<std::string> >::iterator ite = polled_together.find(po);

                Attribute &att = dev->get_device_attr()->get_attr_by_name(poll_list[i]->get_name().c_str());

                if (ite == polled_together.end())
                {
                    std::vector<std::string> tmp_name;
                    tmp_name.push_back(att.get_name());
                    polled_together.insert(std::pair<int,std::vector<std::string> >(po,tmp_name));
                }
                else
                    ite->second.push_back(att.get_name());
            }
        }
    }

//
// Populate returned strings
//

	long cmd_ind = 0;
	long attr_ind = nb_cmd;
	std::string returned_info;
	std::map<std::string,std::vector<std::string> *> root_dev_poll_status;

	for(i = 0;i < nb_poll_obj;i++)
	{
		bool duplicate = false;

		if (i == nb_poll_obj - 1)
		{
			for (auto &elem:root_dev_poll_status)
				delete elem.second;
		}

//
// First, the name
//

		Tango::PollObjType type = poll_list[i]->get_type();
		if (type == Tango::POLL_CMD)
		{
			returned_info = "Polled command name = ";
			long k;
			long nb_cmd = dev->get_device_class()->get_command_list().size();
			for (k = 0;k < nb_cmd;k++)
			{
				if (dev->get_device_class()->get_command_list()[k]->get_lower_name() == poll_list[i]->get_name())
				{
					returned_info = returned_info + dev->get_device_class()->get_command_list()[k]->get_name();
					break;
				}
			}
		}
		else
		{
			returned_info = "Polled attribute name = ";

			if (poll_list[i]->get_name() == "state")
			{
				duplicate = true;
				returned_info = returned_info + "State";
			}
			else if (poll_list[i]->get_name() == "status")
			{
				duplicate = true;
				returned_info = returned_info + "Status";
			}
			else
			{
				Attribute &att = dev->get_device_attr()->get_attr_by_name(poll_list[i]->get_name().c_str());
				returned_info = returned_info + att.get_name();
			}
		}

//
// Add update period
//

		std::stringstream s;
		std::string tmp_str;
		std::string per;

		long po = poll_list[i]->get_upd();

		if (po == 0)
		{
			returned_info = returned_info + "\nPolling externally triggered";
		}
		else
		{
			returned_info = returned_info + "\nPolling period (mS) = ";
			s << po;
			s >> tmp_str;
			returned_info = returned_info + tmp_str;
			s.clear();	// clear the stream eof flag
		}

		s.str("");	// clear the underlying string

//
// Add ring buffer depth
//

		returned_info = returned_info + "\nPolling ring buffer depth = ";
		long depth = 0;

		if (type == Tango::POLL_CMD)
			depth = dev->get_cmd_poll_ring_depth(poll_list[i]->get_name());
		else
			depth = dev->get_attr_poll_ring_depth(poll_list[i]->get_name());

		s << depth;
		returned_info = returned_info + s.str();

		s.str(""); // Clear the underlying string

//
// Add a message if the data ring is empty
//

		if (poll_list[i]->is_ring_empty() == true)
		{
			returned_info = returned_info + "\nNo data recorded yet";
		}
		else
		{

//
// Take polled object ownership in order to have coherent info returned to caller. We don't want the polling thread
// to insert a new elt in polling ring while we are getting these data. Therefore, use the xxx_i methods
//

			{
			omni_mutex_lock sync(*(poll_list[i]));

//
// Add needed time to execute last command
//

			double tmp_db = poll_list[i]->get_needed_time_i();
			if (tmp_db == 0.0)
			{
				returned_info = returned_info + "\nThe polling buffer is externally filled in";
			}
			else
			{
				if (po != 0)
				{
					returned_info = returned_info + "\nTime needed for the last ";
					if (type == Tango::POLL_CMD)
						returned_info = returned_info + "command execution (mS) = ";
					else
                    {
                        std::map<int,std::vector<std::string> >::iterator ite = polled_together.find(po);
                        if (ite != polled_together.end())
                        {
                            if (ite->second.size() == 1)
                                returned_info = returned_info + "attribute reading (mS) = ";
                            else
                            {
                                returned_info = returned_info + "attributes (";
                                for (size_t loop = 0;loop < ite->second.size();loop++)
                                {
                                    returned_info = returned_info + ite->second[loop];
                                    if (loop <= ite->second.size() - 2)
                                        returned_info = returned_info + " + ";
                                }
                                returned_info = returned_info + ") reading (mS) = ";
                            }
                        }
                        else
                            returned_info = returned_info + "attribute reading(ms) = ";
                    }

					s.setf(std::ios::fixed);
					s << std::setprecision(3) << tmp_db;
					returned_info = returned_info + s.str();

					s.str("");

//
// Add not updated since... info
//

					returned_info = returned_info + "\nData not updated since ";
					double since = poll_list[i]->get_last_insert_date_i();
					struct timeval now;
#ifdef _TG_WINDOWS_
					struct _timeb now_win;
					_ftime(&now_win);
					now.tv_sec = (unsigned long)now_win.time;
					now.tv_usec = (long)now_win.millitm * 1000;
#else
					gettimeofday(&now,NULL);
#endif
					now.tv_sec = now.tv_sec - DELTA_T;
					double now_d = (double)now.tv_sec + ((double)now.tv_usec / 1000000);
					double diff_t = now_d - since;
					diff_t = diff_t - (tmp_db / 1000);
					if (diff_t < 1.0)
					{
						long nb_msec = (long)(diff_t * 1000);
						s << nb_msec;

						returned_info = returned_info + s.str() + " mS";
						s.str("");
					}
					else if (diff_t < 60.0)
					{
						long nb_sec = (long)diff_t;
						long nb_msec = (long)((diff_t - nb_sec) * 1000);

						s << nb_sec;

						returned_info = returned_info + s.str() + " S and ";
						s.str("");

						s << nb_msec;
						returned_info = returned_info + s.str() + " mS";
						s.str("");
					}
					else
					{
						long nb_min = (long)(diff_t / 60);
						long nb_sec = (long)(diff_t - (60 * nb_min));
						long nb_msec = (long)((diff_t - (long)diff_t) * 1000);

						s << nb_min;
						returned_info = returned_info + s.str() + " MN";
						s.str("");

						if (nb_sec != 0)
						{
							s << nb_sec ;
							returned_info = returned_info + " ," + s.str() + " S";
							s.str("");
						}

						if (nb_msec != 0)
						{
							s << nb_msec;
							returned_info = returned_info + " and " + s.str() + " mS";
							s.str("");
						}
					}
				}
			}

//
// Add delta_t between last record(s)
//

			try
			{
				std::vector<double> delta;
				poll_list[i]->get_delta_t_i(delta,4);

				returned_info = returned_info + "\nDelta between last records (in mS) = ";
				for (unsigned long j = 0;j < delta.size();j++)
				{
					long nb_msec = (long)(delta[j] * 1000);
					s << nb_msec;
					returned_info = returned_info + s.str();
					s.str("");
					if (j != (delta.size() - 1))
						returned_info = returned_info + ", ";
				}
			}
			catch (Tango::DevFailed &)
			{
			}


//
// Add last polling exception fields (if any)
//

			long dev_vers;
			bool last_err;

			dev_vers = dev->get_dev_idl_version();
			if (dev_vers < 3)
				last_err = poll_list[i]->is_last_an_error_i();
			else
				last_err = poll_list[i]->is_last_an_error_i_3();
			if (last_err == true)
			{
				if (type == Tango::POLL_CMD)
					returned_info = returned_info + "\nLast command execution FAILED :";
				else
					returned_info = returned_info + "\nLast attribute read FAILED :";

				Tango::DevFailed *exe_ptr = poll_list[i]->get_last_except_i();
				returned_info = returned_info + "\n\tReason = " + exe_ptr->errors[0].reason.in();
				returned_info = returned_info + "\n\tDesc = " + exe_ptr->errors[0].desc.in();
				returned_info = returned_info + "\n\tOrigin = " + exe_ptr->errors[0].origin.in();
			}

//
// Release polled object monitor (only a compiler block end)
//
			}
		}

//
// Init. string in sequence
//

		if (type == Tango::POLL_CMD)
		{
			(*ret)[cmd_ind] = Tango::string_dup(returned_info.c_str());
			cmd_ind++;
		}
		else
		{
			(*ret)[attr_ind] = Tango::string_dup(returned_info.c_str());
			attr_ind++;

//
// If the attribute is state or status, also add the string in command list after replacing all occurences of
// "attributes" by "command" in the returned string
//

			if (duplicate == true)
			{
				std::string::size_type pos = returned_info.find("attribute");
				while (pos != std::string::npos)
				{
					returned_info.replace(pos,9,"command");
					std::string::size_type npos;
					npos = returned_info.find("attribute",pos);
					pos = npos;
				}
				(*ret)[cmd_ind] = Tango::string_dup(returned_info.c_str());
				cmd_ind++;
			}
		}
	}

	return(ret);

}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::add_obj_polling()
//
// description :
//		command to add one object to be polled
//
// args :
// 		in :
//			- argin : The polling parameters :
//						device name
//						object type (command or attribute)
//						object name
//						update period in mS (in the long array)
//			- with_db_upd : set to true if db has to be updated
//			- delta_ms :
//
//-------------------------------------------------------------------------------------------------------------------

void DServer::add_obj_polling(const Tango::DevVarLongStringArray *argin,bool with_db_upd,int delta_ms)
{
	NoSyncModelTangoMonitor nosyn_mon(this);

	cout4 << "In add_obj_polling method" << std::endl;
	unsigned long i;
	for (i = 0;i < argin->svalue.length();i++)
		cout4 << "Input string = " << (argin->svalue)[i].in() << std::endl;
	for (i = 0;i < argin->lvalue.length();i++)
		cout4 << "Input long = " << (argin->lvalue)[i] << std::endl;

//
// Check that parameters number is correct
//

	if ((argin->svalue.length() != 3) || (argin->lvalue.length() != 1))
	{
		TANGO_THROW_EXCEPTION(API_WrongNumberOfArgs, "Incorrect number of inout arguments");
	}

//
// Find the device
//

	Tango::Util *tg = Tango::Util::instance();
	DeviceImpl *dev = NULL;
	try
	{
		dev = tg->get_device_by_name((argin->svalue)[0]);
	}
	catch (Tango::DevFailed &e)
	{
		TangoSys_OMemStream o;
		o << "Device " << (argin->svalue)[0] << " not found" << std::ends;

		TANGO_RETHROW_EXCEPTION(e, API_DeviceNotFound, o.str());
	}

//
// If the device is locked and if the client is not the lock owner, refuse to do the job
//

	check_lock_owner(dev,"add_obj_polling",(argin->svalue)[0]);

//
// Check that the command (or the attribute) exists. For command, also checks that it does not need input value.
//

	std::string obj_type((argin->svalue)[1]);
	std::transform(obj_type.begin(),obj_type.end(),obj_type.begin(),::tolower);
	std::string obj_name((argin->svalue)[2]);
	std::transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);
	PollObjType type = Tango::POLL_CMD;
	Attribute *attr_ptr;

	bool local_request = false;
	std::string::size_type pos = obj_type.rfind(LOCAL_POLL_REQUEST);
	if (pos == obj_type.size() - LOCAL_REQUEST_STR_SIZE)
	{
		local_request = true;
		obj_type.erase(pos);
	}

	if (obj_type == PollCommand)
	{
		dev->check_command_exists(obj_name);
		type = Tango::POLL_CMD;
	}
	else if (obj_type == PollAttribute)
	{
		Attribute &att = dev->get_device_attr()->get_attr_by_name((argin->svalue)[2]);
		attr_ptr = &att;
		type = Tango::POLL_ATTR;
	}
	else
	{
		TangoSys_OMemStream o;
		o << "Object type " << obj_type << " not supported" << std::ends;
		TANGO_THROW_EXCEPTION(API_NotSupported, o.str());
	}

//
// If it's for the Init command, refuse to poll it
//

	if (obj_type == PollCommand)
	{
		if (obj_name == "init")
		{
			TangoSys_OMemStream o;
			o << "It's not possible to poll the Init command!" << std::ends;
			TANGO_THROW_EXCEPTION(API_NotSupported, o.str());
		}

//
// Since IDl release 3, state and status command must be polled as attributes to be able to generate event on state or
// status.
//

		else if ((dev->get_dev_idl_version() >= 3) && ((obj_name == "state") || (obj_name == "status")))
			type = Tango::POLL_ATTR;
	}

//
// Check that the object is not already polled
//

	std::vector<PollObj *> &poll_list = dev->get_poll_obj_list();
	for (i = 0;i < poll_list.size();i++)
	{
		if (poll_list[i]->get_type() == type)
		{
			std::string name_lower = poll_list[i]->get_name();
			std::transform(name_lower.begin(),name_lower.end(),name_lower.begin(),::tolower);
			if (name_lower == obj_name)
			{
				TangoSys_OMemStream o;
				if (type == Tango::POLL_CMD)
					o << "Command ";
				else
					o << "Attribute ";
				o << obj_name << " already polled" << std::ends;
				TANGO_THROW_EXCEPTION(API_AlreadyPolled, o.str());
			}
		}
	}

//
// Check that the update period is not to small
//

	int upd = (argin->lvalue)[0];
	if ((upd != 0) && (upd < MIN_POLL_PERIOD))
	{
		TangoSys_OMemStream o;
		o << (argin->lvalue)[0] << " is below the min authorized period (" << MIN_POLL_PERIOD << " mS)" << std::ends;
		TANGO_THROW_EXCEPTION(API_NotSupported, o.str());
	}

//
// Check that the requested polling period is not below the one authorized (if defined)
// 0 as polling period is always authorized for polling buffer externally filled
//

	if (upd != 0)
		check_upd_authorized(dev,upd,type,obj_name);

//
// Refuse to do the job for forwarded attribute
//

	if (obj_type == PollAttribute && attr_ptr->is_fwd_att() == true)
	{
		std::stringstream ss;
		ss << "Attribute " << obj_name << " is a forwarded attribute.\n";
		ss << "It's not supported to poll a forwarded attribute.\n";
		FwdAttribute *fwd = static_cast<FwdAttribute *>(attr_ptr);
		ss << "Polling has to be done on the root attribute (";
		ss << fwd->get_fwd_dev_name() << "/" << fwd->get_fwd_att_name() << ")";

		TANGO_THROW_EXCEPTION(API_NotSupportedFeature, ss.str());
	}

//
// Create a new PollObj instance for this object. Protect this code by a monitor in case of the polling thread using
// one of the vector element.
//

	long depth = 0;

	if (obj_type == PollCommand)
		depth = dev->get_cmd_poll_ring_depth(obj_name);
	else
		depth = dev->get_attr_poll_ring_depth(obj_name);

	dev->get_poll_monitor().get_monitor();
	poll_list.push_back(new PollObj(dev,type,obj_name,upd,depth));
	dev->get_poll_monitor().rel_monitor();

	PollingThreadInfo *th_info;
	int thread_created = -2;

//
// Find out which thread is in charge of the device. If none exists already, create one
//

	int poll_th_id = tg->get_polling_thread_id_by_name((argin->svalue)[0]);
	if (poll_th_id == 0)
	{
		cout4 << "POLLING: Creating a thread to poll device " << (argin->svalue)[0] << std::endl;

        bool poll_bef_9 = false;
        if (polling_bef_9_def == true)
            poll_bef_9 = polling_bef_9;
        else if (tg->is_polling_bef_9_def() == true)
            poll_bef_9 = tg->get_polling_bef_9();

		thread_created = tg->create_poll_thread((argin->svalue)[0],false,poll_bef_9);
		poll_th_id = tg->get_polling_thread_id_by_name((argin->svalue)[0]);
	}

	cout4 << "POLLING: Thread in charge of device " << (argin->svalue)[0] << " is thread " << poll_th_id << std::endl;
	th_info = tg->get_polling_thread_info_by_id(poll_th_id);

//
// Send command to the polling thread but wait in case of previous cmd still not executed
//

	cout4 << "Sending cmd to polling thread" << std::endl;

	TangoMonitor &mon = th_info->poll_mon;
	PollThCmd &shared_cmd = th_info->shared_data;

	int th_id = omni_thread::self()->id();
	if (th_id != poll_th_id)
	{
		omni_mutex_lock sync(mon);
		if (shared_cmd.cmd_pending == true)
		{
			mon.wait();
		}
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_ADD_OBJ;
		shared_cmd.dev = dev;
		shared_cmd.index = poll_list.size() - 1;
		shared_cmd.new_upd = delta_ms;

		mon.signal();

		cout4 << "Cmd sent to polling thread" << std::endl;

//
// Wait for thread to execute command except if the command is requested by the polling thread itself
//

		omni_thread *th = omni_thread::self();
		int th_id = th->id();
		if (th_id != poll_th_id && local_request == false)
		{
			while (shared_cmd.cmd_pending == true)
			{
				int interupted = mon.wait(DEFAULT_TIMEOUT);
				if ((shared_cmd.cmd_pending == true) && (interupted == false))
				{
					cout4 << "TIME OUT" << std::endl;
					delete poll_list.back();
					poll_list.pop_back();

//
// If the thread has been created by this request, try to kill it
//

					if (thread_created == -1)
					{
						shared_cmd.cmd_pending = true;
						shared_cmd.cmd_code = POLL_EXIT;

						mon.signal();
					}
					TANGO_THROW_EXCEPTION(API_CommandTimedOut, "Polling thread blocked !!!");
				}
			}
		}
	}
	else
	{
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_ADD_OBJ;
		shared_cmd.dev = dev;
		shared_cmd.index = poll_list.size() - 1;
		shared_cmd.new_upd = delta_ms;

		PollThread *poll_th = th_info->poll_th;
		poll_th->set_local_cmd(shared_cmd);
		poll_th->execute_cmd();
	}


	cout4 << "Thread cmd normally executed" << std::endl;
	th_info->nb_polled_objects++;

//
// Update polling parameters in database (if wanted and possible). If the property is already there
// (it should not but...), only update its polling period
//

	if ((with_db_upd == true) && (Tango::Util::_UseDb == true))
	{
		TangoSys_MemStream s;
		std::string upd_str;
		s << upd;
		s >> upd_str;
		bool found = false;

		DbDatum db_info("polled_cmd");
		if (type == Tango::POLL_CMD)
		{
			std::vector<std::string> &non_auto_list = dev->get_non_auto_polled_cmd();
			std::vector<std::string>::iterator ite;
			for (ite = non_auto_list.begin();ite < non_auto_list.end();++ite)
			{
				if (TG_strcasecmp((*ite).c_str(),obj_name.c_str()) == 0)
				{
					non_auto_list.erase(ite);
					db_info.name = "non_auto_polled_cmd";
					db_info << non_auto_list;
					found = true;
					break;
				}
			}
			if (found == false)
			{
				std::vector<std::string> &cmd_list = dev->get_polled_cmd();
				for (i = 0;i < cmd_list.size();i = i+2)
				{
					if (TG_strcasecmp(cmd_list[i].c_str(),obj_name.c_str()) == 0)
					{
						cmd_list[i + 1] = upd_str;
						break;
					}
				}
				if (i == cmd_list.size())
				{
					cmd_list.push_back(obj_name);
					cmd_list.push_back(upd_str);
				}
				db_info << cmd_list;
			}
		}
		else
		{
			std::vector<std::string> &non_auto_list = dev->get_non_auto_polled_attr();
			std::vector<std::string>::iterator ite;
			for (ite = non_auto_list.begin();ite < non_auto_list.end();++ite)
			{
				if (TG_strcasecmp((*ite).c_str(),obj_name.c_str()) == 0)
				{
					non_auto_list.erase(ite);
					db_info.name = "non_auto_polled_attr";
					db_info << non_auto_list;
					found = true;
					break;
				}
			}
			if (found == false)
			{
				db_info.name = "polled_attr";
				std::vector<std::string> &attr_list = dev->get_polled_attr();
				for (i = 0;i < attr_list.size();i = i+2)
				{
					if (TG_strcasecmp(attr_list[i].c_str(),obj_name.c_str()) == 0)
					{
						attr_list[i + 1] = upd_str;
						break;
					}
				}
				if (i == attr_list.size())
				{
					attr_list.push_back(obj_name);
					attr_list.push_back(upd_str);
				}
				db_info << attr_list;
			}
		}

		DbData send_data;
		send_data.push_back(db_info);
		dev->get_db_device()->put_property(send_data);
	}

//
// If a polling thread has just been created, ask it to poll
//

	if (thread_created == -1)
	{
		start_polling(th_info);
	}

//
// Also update the polling threads pool conf if one thread has been created by this call
//

	if (thread_created != -2)
	{
		std::string dev_name((argin->svalue)[0]);
		std::transform(dev_name.begin(),dev_name.end(),dev_name.begin(),::tolower);
		cout4 << "thread_created = " << thread_created << std::endl;
		if (thread_created == -1)
		{
			tg->get_poll_pool_conf().push_back(dev_name);
		}
		else if (thread_created >= 0)
		{
			std::string &conf_entry = (tg->get_poll_pool_conf())[thread_created];
			conf_entry = conf_entry + ',' + dev_name;
		}

		if ((with_db_upd == true) && (Tango::Util::_UseDb == true))
		{
			DbData send_data;
			send_data.push_back(DbDatum("polling_threads_pool_conf"));

			std::vector<std::string> &ppc = tg->get_poll_pool_conf();

			std::vector<std::string>::iterator iter;
			std::vector<std::string> new_ppc;

			for (iter = ppc.begin();iter != ppc.end();++iter)
			{
				std::string v_entry = *iter;
				unsigned int length = v_entry.size();
				int nb_lines = (length / MaxDevPropLength) + 1;

				if (nb_lines > 1)
				{
					std::string::size_type start;
					start = 0;

					for (int i = 0;i < nb_lines;i++)
					{
						std::string sub = v_entry.substr(start,MaxDevPropLength);
						if (i < (nb_lines - 1))
							sub = sub + '\\';
						start = start + MaxDevPropLength;
						new_ppc.push_back(sub);
					}
				}
				else
					new_ppc.push_back(v_entry);
			}

			send_data[0] << new_ppc;
			tg->get_dserver_device()->get_db_device()->put_property(send_data);
		}
	}

	cout4 << "Polling properties updated" << std::endl;

//
// Mark the device as polled
//

	dev->is_polled(true);
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::upd_obj_polling_period()
//
// description :
//		command to upadte an already polled object update period
//
// args :
// 		in :
//			- argin : The polling parameters :
//						device name
//						object type (command or attribute)
//						object name
//						update period in mS (in the long array)
//			- with_db_upd : Flag set to true if db has to be updated
//
//------------------------------------------------------------------------------------------------------------------

void DServer::upd_obj_polling_period(const Tango::DevVarLongStringArray *argin,bool with_db_upd)
{
	NoSyncModelTangoMonitor nosync_mon(this);

	cout4 << "In upd_obj_polling_period method" << std::endl;
	unsigned long i;
	for (i = 0;i < argin->svalue.length();i++)
		cout4 << "Input string = " << (argin->svalue)[i].in() << std::endl;
	for (i = 0;i < argin->lvalue.length();i++)
		cout4 << "Input long = " << (argin->lvalue)[i] << std::endl;

//
// Check that parameters number is correct
//

	if ((argin->svalue.length() != 3) || (argin->lvalue.length() != 1))
	{
		TANGO_THROW_EXCEPTION(API_WrongNumberOfArgs, "Incorrect number of inout arguments");
	}

//
// Find the device
//

	Tango::Util *tg = Tango::Util::instance();
	DeviceImpl *dev = NULL;
	try
	{
		dev = tg->get_device_by_name((argin->svalue)[0]);
	}
	catch (Tango::DevFailed &e)
	{
		TangoSys_OMemStream o;
		o << "Device " << (argin->svalue)[0] << " not found" << std::ends;

		TANGO_RETHROW_EXCEPTION(e, API_DeviceNotFound, o.str());
	}

//
// Check that the device is polled
//

	if (dev->is_polled() == false)
	{
		TangoSys_OMemStream o;
		o << "Device " << (argin->svalue)[0] << " is not polled" << std::ends;

		TANGO_THROW_EXCEPTION(API_DeviceNotPolled, o.str());
	}

//
// If the device is locked and if the client is not the lock owner, refuse to do the job
//

	check_lock_owner(dev,"upd_obj_polling_period",(argin->svalue)[0]);

//
// Find the wanted object in the list of device polled object
//

	std::string obj_type((argin->svalue)[1]);
	std::transform(obj_type.begin(),obj_type.end(),obj_type.begin(),::tolower);
	std::string obj_name((argin->svalue)[2]);
	std::transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);
	PollObjType type = Tango::POLL_CMD;

	std::string::size_type pos = obj_type.rfind(LOCAL_POLL_REQUEST);
	if (pos == obj_type.size() - LOCAL_REQUEST_STR_SIZE)
	{
		obj_type.erase(pos);
	}


	if (obj_type == PollCommand)
	{
		type = Tango::POLL_CMD;
//
// Since IDl release 3, state and status command must be polled as attributes to be able to generate event on state or
// status.
//

		if ((obj_name == "state") || (obj_name == "status"))
			type = Tango::POLL_ATTR;
	}
	else if (obj_type == PollAttribute)
	{
		type = Tango::POLL_ATTR;
	}
	else
	{
		TangoSys_OMemStream o;
		o << "Object type " << obj_type << " not supported" << std::ends;
		TANGO_THROW_EXCEPTION(API_NotSupported, o.str());
	}

	std::vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(type,obj_name);

//
// Check that the requested polling period is not below the one authorized (if defined)
//

	int upd = (argin->lvalue)[0];
	check_upd_authorized(dev,upd,type,obj_name);

//
// Check that it is not an externally triggered polling object. In this case, throw exception
//

/*	long tmp_upd = (*ite)->get_upd();
	if (tmp_upd == 0)
	{
		TangoSys_OMemStream o;

		o << "Polling for ";
		if (type == Tango::POLL_CMD)
			o << "command ";
		else
			o << "attribute ";
		o << (argin->svalue)[2];
		o << " (device " << (argin->svalue)[0] << ") ";
		o << " is externally triggered. Remove and add object to change its polling period";
		o << std::ends;
		TANGO_THROW_EXCEPTION(API_NotSupported, o.str());
	}*/

//
// Check that the update period is not to small
//

	if ((upd != 0) && (upd < MIN_POLL_PERIOD))
	{
		TangoSys_OMemStream o;
		o << (argin->lvalue)[0] << " is below the min authorized period (" << MIN_POLL_PERIOD << " mS)" << std::ends;
		TANGO_THROW_EXCEPTION(API_NotSupported, o.str());
	}

//
// Find out which thread is in charge of the device. If none exists already, create one
//

	PollingThreadInfo *th_info;

	int poll_th_id = tg->get_polling_thread_id_by_name((argin->svalue)[0]);
	if (poll_th_id == 0)
	{
		TangoSys_OMemStream o;
		o << "Can't find a polling thread for device " << (argin->svalue)[0] << std::ends;
		TANGO_THROW_EXCEPTION(API_NotSupported, o.str());
	}

	th_info = tg->get_polling_thread_info_by_id(poll_th_id);

//
// Update polling period
//

	(*ite)->update_upd(upd);

//
// Send command to the polling thread
//

	TangoMonitor &mon = th_info->poll_mon;
	PollThCmd &shared_cmd = th_info->shared_data;

	int th_id = omni_thread::self()->id();
	if (th_id != poll_th_id)
	{
		omni_mutex_lock sync(mon);
		if (shared_cmd.cmd_pending == true)
		{
			mon.wait();
		}
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_UPD_PERIOD;
		shared_cmd.dev = dev;
		shared_cmd.name = obj_name;
		shared_cmd.type = type;
		shared_cmd.new_upd = (argin->lvalue)[0];

		shared_cmd.index = distance(dev->get_poll_obj_list().begin(),ite);

		mon.signal();
	}
	else
	{
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_UPD_PERIOD;
		shared_cmd.dev = dev;
		shared_cmd.name = obj_name;
		shared_cmd.type = type;
		shared_cmd.new_upd = (argin->lvalue)[0];

		shared_cmd.index = distance(dev->get_poll_obj_list().begin(),ite);

		PollThread *poll_th = th_info->poll_th;
		poll_th->set_local_cmd(shared_cmd);
		poll_th->execute_cmd();
	}

//
// Update database property --> Update polling period if this object is already defined in the polling property.
// Add object name and update period if the object is not known in the property
//

	if ((with_db_upd == true) && (Tango::Util::_UseDb == true))
	{
		TangoSys_MemStream s;
		std::string upd_str;
		s << (argin->lvalue)[0] << std::ends;
		s >> upd_str;

		DbDatum db_info("polled_attr");
		if (type == Tango::POLL_CMD)
		{
			db_info.name = "polled_cmd";
			std::vector<std::string> &cmd_list = dev->get_polled_cmd();
			for (i = 0;i < cmd_list.size();i = i+2)
			{
				if (TG_strcasecmp(cmd_list[i].c_str(),obj_name.c_str()) == 0)
				{
					cmd_list[i + 1] = upd_str;
					break;
				}
			}
			if (i == cmd_list.size())
			{
				cmd_list.push_back(obj_name);
				cmd_list.push_back(upd_str);
			}
			db_info << cmd_list;
		}
		else
		{
			std::vector<std::string> &attr_list = dev->get_polled_attr();
			for (i = 0;i < attr_list.size();i = i+2)
			{
				if (TG_strcasecmp(attr_list[i].c_str(),obj_name.c_str()) == 0)
				{
					attr_list[i + 1] = upd_str;
					break;
				}
			}
			if (i == attr_list.size())
			{
				attr_list.push_back(obj_name);
				attr_list.push_back(upd_str);
			}
			db_info << attr_list;
		}

		DbData send_data;
		send_data.push_back(db_info);
		dev->get_db_device()->put_property(send_data);
	}
}


//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::rem_obj_polling()
//
// description :
//		command to remove an already polled object from the device polled object list
//
// args :
// 		in :
//			- argin: The polling parameters :
//				    device name
//				    object type (command or attribute)
//				    object name
//      	- with_db_upd : Update db flag
//
//-----------------------------------------------------------------------------------------------------------------

void DServer::rem_obj_polling(const Tango::DevVarStringArray *argin,bool with_db_upd)
{
	NoSyncModelTangoMonitor nosync_mon(this);

	cout4 << "In rem_obj_polling method" << std::endl;
	unsigned long i;
	for (i = 0;i < argin->length();i++)
		cout4 << "Input string = " << (*argin)[i].in() << std::endl;

//
// Check that parameters number is correct
//

	if (argin->length() != 3)
	{
		TANGO_THROW_EXCEPTION(API_WrongNumberOfArgs, "Incorrect number of inout arguments");
	}

//
// Find the device
//

	Tango::Util *tg = Tango::Util::instance();
	DeviceImpl *dev = NULL;
	try
	{
		dev = tg->get_device_by_name((*argin)[0]);
	}
	catch (Tango::DevFailed &e)
	{
		TangoSys_OMemStream o;
		o << "Device " << (*argin)[0] << " not found" << std::ends;

		TANGO_RETHROW_EXCEPTION(e, API_DeviceNotFound, o.str());
	}

//
// Check that the device is polled
//

	if (dev->is_polled() == false)
	{
		TangoSys_OMemStream o;
		o << "Device " << (*argin)[0] << " is not polled" << std::ends;

		TANGO_THROW_EXCEPTION(API_DeviceNotPolled, o.str());
	}

//
// If the device is locked and if the client is not the lock owner, refuse to do the job
//

	check_lock_owner(dev,"rem_obj_polling",(*argin)[0]);

//
// Find the wanted object in the list of device polled object
//

	std::string obj_type((*argin)[1]);
	std::transform(obj_type.begin(),obj_type.end(),obj_type.begin(),::tolower);
	std::string obj_name((*argin)[2]);
	std::transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);

	bool local_request = false;
	std::string::size_type pos = obj_type.rfind(LOCAL_POLL_REQUEST);
	if (pos == obj_type.size() - LOCAL_REQUEST_STR_SIZE)
	{
		local_request = true;
		obj_type.erase(pos);
	}

	PollObjType type = Tango::POLL_CMD;

	if (obj_type == PollCommand)
	{
		type = Tango::POLL_CMD;
		if ((obj_name == "state") || (obj_name == "status"))
			type = Tango::POLL_ATTR;
	}
	else if (obj_type == PollAttribute)
	{
		type = Tango::POLL_ATTR;
	}
	else
	{
		TangoSys_OMemStream o;
		o << "Object type " << obj_type << " not supported" << std::ends;
		TANGO_THROW_EXCEPTION(API_NotSupported, o.str());
	}

	std::vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(type,obj_name);
	long tmp_upd = (*ite)->get_upd();

	PollingThreadInfo *th_info = nullptr;
	int poll_th_id = 0;
	int th_id = omni_thread::self()->id();

//
// Find out which thread is in charge of the device.
//

	if (tg->is_svr_shutting_down() == false)
	{
		poll_th_id = tg->get_polling_thread_id_by_name((*argin)[0]);
		if (poll_th_id == 0)
		{
			TangoSys_OMemStream o;
			o << "Can't find a polling thread for device " << (*argin)[0] << std::ends;
			TANGO_THROW_EXCEPTION(API_NotSupported, o.str());
		}

		cout4 << "Thread in charge of device " << (*argin)[0] << " is thread " << poll_th_id << std::endl;
		th_info = tg->get_polling_thread_info_by_id(poll_th_id);

//
// Test whether the polling thread is still running!
//

		if ( th_info->poll_th != NULL )
		{

//
// Send command to the polling thread
//

			cout4 << "Sending cmd to polling thread" << std::endl;
			TangoMonitor &mon = th_info->poll_mon;
			PollThCmd &shared_cmd = th_info->shared_data;

			if (th_id != poll_th_id)
			{
				omni_mutex_lock sync(mon);
				if (shared_cmd.cmd_pending == true)
				{
					mon.wait();
				}
				shared_cmd.cmd_pending = true;
				if (tmp_upd == 0)
					shared_cmd.cmd_code = POLL_REM_EXT_TRIG_OBJ;
				else
					shared_cmd.cmd_code = POLL_REM_OBJ;
				shared_cmd.dev = dev;
				shared_cmd.name = obj_name;
				shared_cmd.type = type;

				mon.signal();

				cout4 << "Cmd sent to polling thread" << std::endl;

//
// Wait for thread to execute command except if the command is requested by the polling thread itself
//


				if (th_id != poll_th_id)
				{
					if (local_request == false)
					{
						while (shared_cmd.cmd_pending == true)
						{
							int interupted = mon.wait(DEFAULT_TIMEOUT);
							if ((shared_cmd.cmd_pending == true) && (interupted == false))
							{
								cout4 << "TIME OUT" << std::endl;
								TANGO_THROW_EXCEPTION(API_CommandTimedOut, "Polling thread blocked !!!");
							}
						}
					}
				}
			}
			else
			{
				shared_cmd.cmd_pending = true;
				if (tmp_upd == 0)
					shared_cmd.cmd_code = POLL_REM_EXT_TRIG_OBJ;
				else
					shared_cmd.cmd_code = POLL_REM_OBJ;
				shared_cmd.dev = dev;
				shared_cmd.name = obj_name;
				shared_cmd.type = type;

				shared_cmd.index = distance(dev->get_poll_obj_list().begin(),ite);

				PollThread *poll_th = th_info->poll_th;
				poll_th->set_local_cmd(shared_cmd);
				poll_th->execute_cmd();
			}
			cout4 << "Thread cmd normally executed" << std::endl;
		}
		else
		{
			cout4 << "Polling thread is no longer running!!!!" << std::endl;
		}
	}

//
// Remove the object from the polled object list
//

	std::vector<PollObj *> &poll_list = dev->get_poll_obj_list();

	dev->get_poll_monitor().get_monitor();
	delete(*ite);
	poll_list.erase(ite);
	dev->get_poll_monitor().rel_monitor();

//
// Set attribute polling period to 0
//

    if (type == Tango::POLL_ATTR)
    {
        Attribute &att = dev->get_device_attr()->get_attr_by_name(obj_name.c_str());
        att.set_polling_period(0);
    }

//
// Mark the device as non polled if this was the last polled object
//

	if (poll_list.empty() == true)
		dev->is_polled(false);

//
// Update database property. This means remove object entry in the polling properties if they exist or add it to the
// list of device not polled for automatic polling defined at command/attribute level.
// Do this if possible and wanted.
//

	if ((with_db_upd == true) && (Tango::Util::_UseDb == true))
	{
		DbData send_data;
		DbDatum db_info("polled_attr");
		bool update_needed = false;

		if (type == Tango::POLL_CMD)
		{
			db_info.name = "polled_cmd";
			std::vector<std::string> &cmd_list = dev->get_polled_cmd();
			std::vector<std::string>::iterator s_ite;
			for (s_ite = cmd_list.begin();s_ite < cmd_list.end();s_ite += 2)
			{
				if (TG_strcasecmp((*s_ite).c_str(),obj_name.c_str()) == 0)
				{
					s_ite = cmd_list.erase(s_ite);
					cmd_list.erase(s_ite);
					db_info << cmd_list;
					update_needed = true;
					break;
				}
			}
			if (update_needed == false)
			{
				std::vector<std::string> &non_auto_cmd = dev->get_non_auto_polled_cmd();
				for (s_ite = non_auto_cmd.begin();s_ite < non_auto_cmd.end();++s_ite)
				{
					if (TG_strcasecmp((*s_ite).c_str(),obj_name.c_str()) == 0)
						break;
				}
				if (s_ite == non_auto_cmd.end())
				{
					non_auto_cmd.push_back(obj_name);
					db_info.name = "non_auto_polled_cmd";
					db_info << non_auto_cmd;
					update_needed = true;
				}
			}
		}
		else
		{
			std::vector<std::string> &attr_list = dev->get_polled_attr();
			std::vector<std::string>::iterator s_ite;
			for (s_ite = attr_list.begin();s_ite < attr_list.end();s_ite += 2)
			{
				if (TG_strcasecmp((*s_ite).c_str(),obj_name.c_str()) == 0)
				{
					s_ite = attr_list.erase(s_ite);
					attr_list.erase(s_ite);
					db_info << attr_list;
					update_needed = true;
					break;
				}
			}
			if (update_needed == false)
			{
				std::vector<std::string> &non_auto_attr = dev->get_non_auto_polled_attr();
				for (s_ite = non_auto_attr.begin();s_ite < non_auto_attr.end();++s_ite)
				{
					if (TG_strcasecmp((*s_ite).c_str(),obj_name.c_str()) == 0)
						break;
				}
				if (s_ite == non_auto_attr.end())
				{
					non_auto_attr.push_back(obj_name);
					db_info.name = "non_auto_polled_attr";
					db_info << non_auto_attr;
					update_needed = true;
				}
			}
		}

		if (update_needed == true)
		{
			DbData send_data;
			send_data.push_back(db_info);
			if (db_info.size() == 0)
				dev->get_db_device()->delete_property(send_data);
			else
				dev->get_db_device()->put_property(send_data);

			cout4 << "Database polling properties updated" << std::endl;
		}
	}

//
// If the device is not polled any more, update the pool conf first locally. Also update the map<device name,thread id>
// If this device was the only one for a polling thread, kill the thread then in Db if possible
//

	bool kill_thread = false;
	if (poll_list.empty() == true)
	{
		int ind;
		std::string dev_name((*argin)[0]);
		std::transform(dev_name.begin(),dev_name.end(),dev_name.begin(),::tolower);

		if ((ind = tg->get_dev_entry_in_pool_conf(dev_name)) ==  -1)
		{
			TangoSys_OMemStream o;
			o << "Can't find entry for device " << (*argin)[0] << " in polling threads pool configuration !"<< std::ends;
			TANGO_THROW_EXCEPTION(API_NotSupported, o.str());
		}

		std::vector<std::string> &pool_conf = tg->get_poll_pool_conf();
		std::string &conf_entry = pool_conf[ind];
		std::string::size_type pos;
		if ((pos = conf_entry.find(',')) != std::string::npos)
		{
			pos = conf_entry.find(dev_name);
			if ((pos + dev_name.size()) != conf_entry.size())
				conf_entry.erase(pos,dev_name.size() + 1);
			else
				conf_entry.erase(pos - 1);
		}
		else
		{
			std::vector<std::string>::iterator iter = pool_conf.begin() + ind;
			pool_conf.erase(iter);
			kill_thread = true;
		}

		tg->remove_dev_from_polling_map(dev_name);

//
// Kill the thread if needed and join but don do this now if the executing thread is the polling thread itself
// (case of a polled command which itself decide to stop its own polling)
//

		if (kill_thread == true  && tg->is_svr_shutting_down() == false && th_id != poll_th_id)
		{
			TangoMonitor &mon = th_info->poll_mon;
			PollThCmd &shared_cmd = th_info->shared_data;

			{
				omni_mutex_lock sync(mon);

				shared_cmd.cmd_pending = true;
				shared_cmd.cmd_code = POLL_EXIT;

				mon.signal();
			}

			void *dummy_ptr;
			cout4 << "POLLING: Joining with one polling thread" << std::endl;
			th_info->poll_th->join(&dummy_ptr);

			tg->remove_polling_thread_info_by_id(poll_th_id);
		}

//
// Update db
//

		if ((with_db_upd == true) && (Tango::Util::_UseDb == true))
		{
			DbData send_data;
			send_data.push_back(DbDatum("polling_threads_pool_conf"));
			send_data[0] << tg->get_poll_pool_conf();

			tg->get_dserver_device()->get_db_device()->put_property(send_data);
		}
	}

//
// In case there are some subscribers for event on this attribute relying on polling, fire event with error
//

	if (type == POLL_ATTR)
	{
		Attribute &att = dev->get_device_attr()->get_attr_by_name((*argin)[2]);

		DevFailed ex;
		ex.errors.length(1);

		ex.errors[0].severity = ERR;
		ex.errors[0].reason = Tango::string_dup(API_PollObjNotFound);
		ex.errors[0].origin = Tango::string_dup("DServer::rem_obj_polling");
		std::stringstream ss;
		ss << "No event possible on attribute " << obj_name << ". Polling has just being stopped!";
		ex.errors[0].desc = Tango::string_dup(ss.str().c_str());

		if (att.periodic_event_subscribed() == true)
			att.fire_error_periodic_event(&ex);
		if (att.archive_event_subscribed() == true && att.is_archive_event() == false)
			dev->push_archive_event(obj_name,&ex);
		if (att.change_event_subscribed() == true && att.is_change_event() == false)
			dev->push_change_event(obj_name,&ex);
	}

//
// In case of local_request and executing thread is the polling thread, ask our self to exit now that eveything else
// is done
//

	if (kill_thread == true  && tg->is_svr_shutting_down() == false && th_id == poll_th_id && local_request == true)
	{
		tg->remove_polling_thread_info_by_id(poll_th_id);

		PollThCmd &shared_cmd = th_info->shared_data;
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_EXIT;
	}
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::stop_polling()
//
// description :
//		command to stop the polling thread
//
//-------------------------------------------------------------------------------------------------------------------

void DServer::stop_polling()
{
	NoSyncModelTangoMonitor nosync_mon(this);

	cout4 << "In stop_polling method" << std::endl;

//
// Send command to the polling thread and wait for its execution
//

	int interupted;
	Tango::Util *tg = Tango::Util::instance();

	std::vector<PollingThreadInfo *> &th_info = tg->get_polling_threads_info();
	std::vector<PollingThreadInfo *>:: iterator iter;

	for (iter = th_info.begin();iter != th_info.end();++iter)
	{
		TangoMonitor &mon = (*iter)->poll_mon;
		PollThCmd &shared_cmd = (*iter)->shared_data;

		{
			omni_mutex_lock sync(mon);
			if (shared_cmd.cmd_pending == true)
			{
				mon.wait();
			}
			shared_cmd.cmd_pending = true;
			shared_cmd.cmd_code = POLL_STOP;

			mon.signal();

			while (shared_cmd.cmd_pending == true)
			{
				interupted = mon.wait(DEFAULT_TIMEOUT);

				if ((shared_cmd.cmd_pending == true) && (interupted == false))
				{
					cout4 << "TIME OUT" << std::endl;
					TANGO_THROW_EXCEPTION(API_CommandTimedOut, "Polling thread blocked !!!");
				}
			}
		}
	}

//
// Update polling status
//

	tg->poll_status(false);
	std::string &str = get_status();
	str = "The device is ON\nThe polling is OFF";
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::start_polling()
//
// description :
//		command to start the polling thread
//
//------------------------------------------------------------------------------------------------------------------

void DServer::start_polling()
{
	NoSyncModelTangoMonitor nosync_mon(this);

	cout4 << "In start_polling method" << std::endl;

//
// Send command to the polling thread(s) and wait for its execution
//

	Tango::Util *tg = Tango::Util::instance();

	std::vector<PollingThreadInfo *> &th_info = tg->get_polling_threads_info();
	std::vector<PollingThreadInfo *>:: iterator iter;

	for (iter = th_info.begin();iter != th_info.end();++iter)
	{
		TangoMonitor &mon = (*iter)->poll_mon;
		PollThCmd &shared_cmd = (*iter)->shared_data;

		{
			omni_mutex_lock sync(mon);
			if (shared_cmd.cmd_pending == true)
			{
				mon.wait();
			}
			shared_cmd.cmd_pending = true;
			shared_cmd.cmd_code = POLL_START;

			mon.signal();

			while (shared_cmd.cmd_pending == true)
			{
				int interupted = mon.wait(DEFAULT_TIMEOUT);

				if ((shared_cmd.cmd_pending == true) && (interupted == false))
				{
					cout4 << "TIME OUT" << std::endl;
					TANGO_THROW_EXCEPTION(API_CommandTimedOut, "Polling thread blocked !!!");
				}
			}
		}
	}

//
// Update polling status
//

	tg->poll_status(true);
	std::string &str = get_status();
	str = "The device is ON\nThe polling is ON";
}

void DServer::start_polling(PollingThreadInfo *th_info)
{
	TangoMonitor &mon = th_info->poll_mon;
	PollThCmd &shared_cmd = th_info->shared_data;

	{
		omni_mutex_lock sync(mon);
		if (shared_cmd.cmd_pending == true)
		{
			mon.wait();
		}
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_START;

		mon.signal();

		while (shared_cmd.cmd_pending == true)
		{
			int interupted = mon.wait(DEFAULT_TIMEOUT);

			if ((shared_cmd.cmd_pending == true) && (interupted == false))
			{
				cout4 << "TIME OUT" << std::endl;
				TANGO_THROW_EXCEPTION(API_CommandTimedOut, "Polling thread blocked while trying to start thread polling!!!");
			}
		}
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::add_event_heartbeat()
//
// description :
//		command to ask the heartbeat thread to send the event heartbeat every 9 seconds
//
//--------------------------------------------------------------------------------------------------------------------

void DServer::add_event_heartbeat()
{
	NoSyncModelTangoMonitor nosyn_mon(this);

	cout4 << "In add_event_heartbeat method" << std::endl;

//
// Send command to the heartbeat thread but wait in case of previous cmd still not executed
//

	cout4 << "Sending cmd to polling thread" << std::endl;
	Tango::Util *tg = Tango::Util::instance();

	TangoMonitor &mon = tg->get_heartbeat_monitor();
	PollThCmd &shared_cmd = tg->get_heartbeat_shared_cmd();

	{
		omni_mutex_lock sync(mon);
		if (shared_cmd.cmd_pending == true)
		{
			mon.wait();
		}
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_ADD_HEARTBEAT;

		mon.signal();

		cout4 << "Cmd sent to polling thread" << std::endl;

//
// Wait for thread to execute command except if the command is requested by the polling thread itself
//

		int th_id = omni_thread::self()->id();
		if (th_id != tg->get_heartbeat_thread_id())
		{
			while (shared_cmd.cmd_pending == true)
			{
				int interupted = mon.wait(DEFAULT_TIMEOUT);
				if ((shared_cmd.cmd_pending == true) && (interupted == false))
				{
					cout4 << "TIME OUT" << std::endl;
					TANGO_THROW_EXCEPTION(API_CommandTimedOut, "Polling thread blocked !!!");
				}
			}
		}
	}
	cout4 << "Thread cmd normally executed" << std::endl;
}


//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::rem_event_heartbeat()
//
// description :
//		command to ask the heartbeat thread to stop sending the event heartbeat
//
//-----------------------------------------------------------------------------------------------------------------

void DServer::rem_event_heartbeat()
{
	NoSyncModelTangoMonitor nosyn_mon(this);

	cout4 << "In rem_event_heartbeat method" << std::endl;

//
// Send command to the heartbeat thread but wait in case of previous cmd still not executed
//

	cout4 << "Sending cmd to polling thread" << std::endl;
	Tango::Util *tg = Tango::Util::instance();
	TangoMonitor &mon = tg->get_heartbeat_monitor();
	PollThCmd &shared_cmd = tg->get_heartbeat_shared_cmd();

	{
		omni_mutex_lock sync(mon);
		if (shared_cmd.cmd_pending == true)
		{
			mon.wait();
		}
		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_REM_HEARTBEAT;

		mon.signal();

		cout4 << "Cmd sent to polling thread" << std::endl;

//
// Wait for thread to execute command except if the command is requested by the polling thread itself
//

		int th_id = omni_thread::self()->id();
		if (th_id != tg->get_heartbeat_thread_id())
		{
			while (shared_cmd.cmd_pending == true)
			{
				int interupted = mon.wait(DEFAULT_TIMEOUT);
				if ((shared_cmd.cmd_pending == true) && (interupted == false))
				{
					cout4 << "TIME OUT" << std::endl;
					TANGO_THROW_EXCEPTION(API_CommandTimedOut, "Polling thread blocked !!!");
				}
			}
		}
	}
	cout4 << "Thread cmd normally executed" << std::endl;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::check_upd_authorized()
//
// description :
//		In case a minimun update polling period is defined (via the min_poll_period, cmd_min_poll_period,
//		attr_min_poll_period) check if the requested period is not smaller
//
// args :
// 		in:
//			- dev : The device
//		  	- upd : The requested update period
//		  	- type : The polled object type (cmd / attr)
//		  	- obj_name : The polled object name
//
//------------------------------------------------------------------------------------------------------------------

void DServer::check_upd_authorized(DeviceImpl *dev,int upd,PollObjType obj_type,std::string &obj_name)
{
	int min_upd = 0;

//
// Get first the xxx_min_poll_period then if not defined the min_poll_period
//

	std::vector<std::string> *v_ptr;
	if (obj_type == Tango::POLL_CMD)
		v_ptr = &(dev->get_cmd_min_poll_period());
	else
		v_ptr = &(dev->get_attr_min_poll_period());

	std::vector<std::string>::iterator ite = find(v_ptr->begin(),v_ptr->end(),obj_name);
	if (ite != v_ptr->end())
	{
		++ite;
		TangoSys_MemStream s;
		s << *ite;
		if (!(s >> min_upd))
		{
			TangoSys_OMemStream o;
			o << "System property ";
			if (obj_type == Tango::POLL_CMD)
				o << "cmd_min_poll_period";
			else
				o << "attr_min_poll_period";
			o << " for device " << dev->get_name() << " has wrong syntax" << std::ends;
			TANGO_THROW_EXCEPTION(API_BadConfigurationProperty, o.str());
		}
	}
	else
	{
		min_upd = dev->get_min_poll_period();
	}

//
// Check with user request
//

	if ((min_upd != 0) && (upd < min_upd))
	{
		TangoSys_OMemStream o;
		o << "Polling period for ";
		if (obj_type == Tango::POLL_CMD)
			o << "command ";
		else
			o << "attribute ";
		o << obj_name << " is below the min authorized (" << min_upd << ")" << std::ends;
		TANGO_THROW_EXCEPTION(API_MethodArgument, o.str());
	}

}

} // End of Tango namespace

