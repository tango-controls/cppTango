static const char *RcsId = "$Id$";

//+=============================================================================
//
// file :               utils_polling.cpp
//
// description :        C++ source for all the methods of the Util class related
//						to polling
//
// project :            TANGO
//
// author(s) :         	E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#include <iostream>
#include <algorithm>

namespace Tango
{
//+----------------------------------------------------------------------------
//
// method : 		Util::polling_configure()
//
// description : 	This method sends command to the polling thread for
//			all cmd/attr with polling configuration stored in db.
//			This is done in separate thread in order to equally
//			spread all the polled objects polling time on the
//			smallest polling period.
//
//-----------------------------------------------------------------------------

void Util::polling_configure()
{
	cout4 << "Entering polling_configure()" << endl;

//
// Get the polling threads pool conf from the DServer device
//

	DServer *admin_dev = get_dserver_device();
	set_polling_threads_pool_size(admin_dev->get_poll_th_pool_size());
	ext->poll_pool_conf = admin_dev->get_poll_th_conf();

//
// Check the coherency of the polling thread pool configuration
//

	check_pool_conf(admin_dev,ext->poll_pool_size);

//
// Send a stop polling command to thread in order not to poll devices
//

	admin_dev->stop_polling();

	vector<DeviceClass *> &tmp_cl_list = admin_dev->get_class_list();
	unsigned long i,j,k;
	int upd;
	TangoSys_MemStream s;

//
// Create the structure used to send data to the polling thread
// and store them in a vector
//

	vector<DevVarLongStringArray *> v_poll_cmd;
	vector<DevDbUpd> dev_db_upd;

//
// A loop on each class and each device in class
//

	int smallest_upd = 0;
	bool first_loop = true;

	for (i = 0;i < tmp_cl_list.size();i++)
	{
		vector<DeviceImpl *> &dev_list = tmp_cl_list[i]->get_device_list();
		for (j = 0;j < dev_list.size();j++)
		{
			v_poll_cmd.clear();

			vector<string> &poll_cmd_list = dev_list[j]->get_polled_cmd();
			vector<string> &poll_attr_list = dev_list[j]->get_polled_attr();

//
// Check that cmd_list and attr_list have a correct syntax
//

			if ((poll_cmd_list.size() % 2) == 1)
			{
				TangoSys_OMemStream o;
				o << "System property polled_cmd for device " << dev_list[j]->get_name() << " has wrong syntax" << ends;
				Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        		o.str(),
				        		(const char *)"Util::polling_configure");
			}

			if ((poll_attr_list.size() % 2) == 1)
			{
				TangoSys_OMemStream o;
				o << "System property polled_attr for device " << dev_list[j]->get_name() << " has wrong syntax" << ends;
				Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        		o.str(),
				        		(const char *)"Util::polling_configure");
			}

//
// Check the device polling definition (existing polled cmd/attr ?)
// If the polling needs to be modified, memorize the index in class and
// device loop to re-find it later.
//

			int ret;
			if ((ret = check_dev_poll(poll_cmd_list,poll_attr_list,dev_list[j])) != 0)
			{
				DevDbUpd tmp;
				tmp.class_ind = i;
				tmp.dev_ind = j;
				tmp.mod_prop = ret;
				dev_db_upd.push_back(tmp);
			}

//
// A loop on each command
//

			for (k = 0;k < poll_cmd_list.size();k++)
			{
				DevVarLongStringArray *send = new DevVarLongStringArray();
				send->lvalue.length(1);
				send->svalue.length(3);

				send->svalue[0] = CORBA::string_dup(dev_list[j]->get_name().c_str());
				send->svalue[1] = CORBA::string_dup("command");
				send->svalue[2] = CORBA::string_dup(poll_cmd_list[k].c_str());

//
// Convert polling period to a number and store polling thread command params
//

				s << poll_cmd_list[k + 1];
				s >> upd;
				s.clear();
				s.str("");
				send->lvalue[0] = upd;

                if (upd < 0)
                    upd = -upd;

				if (first_loop == true)
				{
					smallest_upd = upd;
					first_loop = false;
				}
				else
				{
					if (upd < smallest_upd)
						smallest_upd = upd;
				}

				v_poll_cmd.push_back(send);
				k++;
			}

//
// A loop on each attribute
//

			for (k = 0;k < poll_attr_list.size();k++)
			{
				DevVarLongStringArray *send = new DevVarLongStringArray();
				send->lvalue.length(1);
				send->svalue.length(3);

				send->svalue[0] = CORBA::string_dup(dev_list[j]->get_name().c_str());
				send->svalue[1] = CORBA::string_dup("attribute");
				send->svalue[2] = CORBA::string_dup(poll_attr_list[k].c_str());

//
// Convert polling period to a number and store polling thread command params
//

				s << poll_attr_list[k + 1];
				s >> upd;
				s.clear();
				s.str("");
				send->lvalue[0] = upd;

                if (upd < 0)
                    upd = -upd;

				if (first_loop == true)
				{
					smallest_upd = upd;
					first_loop = false;
				}
				else
				{
					if (upd < smallest_upd)
						smallest_upd = upd;
				}

				v_poll_cmd.push_back(send);
				k++;
			}

//
// Make sure we have a thread in the pool to poll this device
//

			unsigned long nb_cmd = v_poll_cmd.size();
			if (nb_cmd != 0)
			{
				create_poll_thread(v_poll_cmd[0]->svalue[0],true,smallest_upd);
				first_loop = true;

//
// Copy the list of commands to send to the threads in its structure
//

				PollingThreadInfo *th_info = get_polling_thread_info_by_id(get_polling_thread_id_by_name(v_poll_cmd[0]->svalue[0]));
				for (unsigned long loop = 0;loop < nb_cmd;++loop)
					th_info->v_poll_cmd.push_back(v_poll_cmd[loop]);
			}
		}
	}

//
// Send command to polling thread one by one for each threads
// In the following computation , I try to take into account the
// non real time aspect of our OS. I remove 15 mS from each
// sleeping time due to thread wake-up time
//

	unsigned long nb_thread = ext->poll_ths.size();
	cout4 << "POLLING: " << nb_thread << " thread(s) needed for polling from a pool of " << get_polling_threads_pool_size() << endl;
	for (unsigned long loop = 0;loop < nb_thread;++loop)
	{
		unsigned long nb_cmd = ext->poll_ths[loop]->v_poll_cmd.size();

		int sleeping_time = ext->poll_ths[loop]->smallest_upd / nb_cmd;
		int delta_time = 0;

		long delta_os = 15;
		if (delta_os < sleeping_time)
			sleeping_time = sleeping_time - delta_os;

		cout4 << "PollConfigureThread: smallest_upd = " << ext->poll_ths[loop]->smallest_upd;
		cout4 << ", delta_time = " << sleeping_time;
		cout4 << ", nb_poll_objects = " << nb_cmd << endl;

		for (unsigned long cmd_loop = 0;cmd_loop  < nb_cmd;++cmd_loop)
		{
			try
			{
			    bool upd_db = false;
			    int upd = ext->poll_ths[loop]->v_poll_cmd[cmd_loop]->lvalue[0];
			    if (upd < 0)
			    {
			        ext->poll_ths[loop]->v_poll_cmd[cmd_loop]->lvalue[0] = -upd;
                    upd_db = true;
			    }
				admin_dev->add_obj_polling(ext->poll_ths[loop]->v_poll_cmd[cmd_loop],upd_db,delta_time);
			}
			catch (Tango::DevFailed &e)
			{
				bool throw_ex = true;

				if (::strcmp(e.errors[0].reason.in(),"API_AlreadyPolled") == 0)
				{
					try
					{
						admin_dev->upd_obj_polling_period(ext->poll_ths[loop]->v_poll_cmd[cmd_loop],false);
						throw_ex = false;
					}
					catch (Tango::DevFailed &) {}
				}

				if (throw_ex == true)
				{
					TangoSys_OMemStream o;
					o << "Error when configuring polling for device " << ext->poll_ths[loop]->v_poll_cmd[cmd_loop]->svalue[0].in();
					if (::strcmp(ext->poll_ths[loop]->v_poll_cmd[cmd_loop]->svalue[1].in(),"command") == 0)
						o << ", cmd = ";
					else
						o << ", attr = ";
					o << ext->poll_ths[loop]->v_poll_cmd[cmd_loop]->svalue[2].in() << ends;
					Except::re_throw_exception(e,(const char *)"API_BadConfigurationProperty",
												o.str(),
													(const char *)"Util::polling_configure");
				}
			}

			if (nb_cmd > 1)
			{
				delta_time = delta_time + sleeping_time;
			}
		}

//
// Delete allocated memory
//

		for (unsigned long l = 0;l < nb_cmd;l++)
			delete ext->poll_ths[loop]->v_poll_cmd[l];
	}

//
// Now, start the real polling
//

	admin_dev->start_polling();

//
// If some polling related prop. (polling conf or dev conf) has to be
// updated in db, do it now
// Add a check to verify that it is possible to store polling pool
// conf in database. Property length cannot be longer than 256 characters
//

	if (ext->poll_pool_conf.empty() == true)
	{
		build_first_pool_conf(ext->poll_pool_conf);
		ext->conf_needs_db_upd = true;
	}
	else
	{
	    vector<string> tmp_pool_conf;
	    build_first_pool_conf(tmp_pool_conf);
	    if (tmp_pool_conf != ext->poll_pool_conf)
	    {
	        ext->poll_pool_conf = tmp_pool_conf;
	        ext->conf_needs_db_upd = true;
	    }
	}

	if (((dev_db_upd.empty() == false) || (ext->conf_needs_db_upd == true)) && (_UseDb == true))
		upd_polling_prop(dev_db_upd,admin_dev);

	cout4 << "Leaving polling_configure()" << endl;

}

//+----------------------------------------------------------------------------
//
// method : 		Util::trigger_attr_polling()
//
// description : 	Trigger the polling thread for polled attributes
//			registered with a polling update period set as
//			"externally triggered" (0 mS)
//
//-----------------------------------------------------------------------------

void Util::trigger_attr_polling(Tango::DeviceImpl *dev,const string &name)
{
	cout4 << "Sending trigger to polling thread" << endl;

//
// Check that the device is polled
//

	if (dev->is_polled() == false)
	{
		TangoSys_OMemStream o;
		o << "Device " << dev->get_name() << " is not polled" << ends;

		Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   	(const char *)"Util::trigger_attr_polling");
	}

//
// Find the wanted object in the list of device polled object
//

	string obj_name(name);
	transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);

	vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_ATTR,obj_name);

//
// Check that it is an externally triggered polling object. If it is not the
// case, throw exception
//

	long tmp_upd = (*ite)->get_upd();
	if (tmp_upd != 0)
	{
		TangoSys_OMemStream o;

		o << "Polling for attribute ";
		o << name;
		o << " (device " << dev->get_name() << ") ";
		o << " is not externally triggered.";
		o << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
					(const char *)"Util::trigger_attr_polling");
	}

//
// Find out which thread is in charge of the device.
//

	PollingThreadInfo *th_info;

	int poll_th_id = get_polling_thread_id_by_name(dev->get_name_lower().c_str());
	if (poll_th_id == 0)
	{
		TangoSys_OMemStream o;
		o << "Can't find a polling thread for device " << dev->get_name() << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
							(const char *)"Util::trigger_cmd_polling");
	}

	th_info = get_polling_thread_info_by_id(poll_th_id);

//
// Send command to the polling thread but wait in case of previous cmd
// still not executed
//

	TangoMonitor &mon = th_info->poll_mon;
	PollThCmd &shared_cmd = th_info->shared_data;

	{
		omni_mutex_lock sync(mon);
		if (shared_cmd.trigger == true)
		{
			mon.wait();
		}
		shared_cmd.trigger = true;
		shared_cmd.dev = dev;
		shared_cmd.name = obj_name;
		shared_cmd.type = Tango::POLL_ATTR;

		mon.signal();

		cout4 << "Trigger sent to polling thread" << endl;

//
// Wait for thread to execute command
//

		TangoMonitor &dev_mon = dev->get_dev_monitor();
		omni_thread *th = omni_thread::self();

		while (shared_cmd.trigger == true)
		{

//
// Warning: It's possible to have a deadlock here (experienced under
// Windows) in case of this method being called from a command (or attribute
// methods) which are rapidly sent by the client.
// Client request cmd1 which send trigger to the polling thread
// The polling thread wake up clear shared_cmd.trigger and try to
// execute the command. But cmd 1 thread still owns the device monitor and
// polling thread wait. cmd 1 finished and client immediately send the
// command a new time. On Windows, it may happens that the polling
// thread is not activated just after the cmd thread has released the
// device monitor. As the client sent a new command, the device monitor
// is immediately re-taken by the thread executing the new command sent by
// the client. An order is sent to the polling thread and the cmd
// thread reach this code. It will wait for polling thread to clear
// shared_cmd.trigger. But, the polling thread is already waiting for
// the device monitor and ..... deadlock....
//

			bool deadlock = false;
			long lock_ctr = 0;
			if (th->id() == dev_mon.get_locking_thread_id())
			{
				cout4 << "Possible deadlock detected!" << endl;
				deadlock = true;
				lock_ctr = dev_mon.get_locking_ctr();
				for (long loop = 0;loop < lock_ctr;loop++)
					dev_mon.rel_monitor();
			}

			int interupted = mon.wait(DEFAULT_TIMEOUT);
			if (deadlock == true)
			{
				for (long loop = 0;loop < lock_ctr;loop++)
					dev_mon.get_monitor();
			}

			if ((shared_cmd.trigger == true) && (interupted == 0))
			{
				cout4 << "TIME OUT" << endl;
				Except::throw_exception((const char *)"API_CommandTimedOut",
					        	(const char *)"Polling thread blocked !!!",
					        	(const char *)"Util::trigger_attr_polling");
			}
		}
	}
	cout4 << "Thread cmd normally executed" << endl;
}


//+----------------------------------------------------------------------------
//
// method : 		Util::trigger_cmd_polling()
//
// description : 	Trigger the polling thread for polled command
//			registered with a polling update period set as
//			"externally triggered" (0 mS)
//
//-----------------------------------------------------------------------------

void Util::trigger_cmd_polling(Tango::DeviceImpl *dev,const string &name)
{
	cout4 << "Sending trigger to polling thread" << endl;

//
// Check that the device is polled
//

	if (dev->is_polled() == false)
	{
		TangoSys_OMemStream o;
		o << "Device " << dev->get_name() << " is not polled" << ends;

		Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   	(const char *)"Util::trigger_cmd_polling");
	}

//
// Find the wanted object in the list of device polled object
//

	string obj_name(name);
	transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);

	vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_CMD,obj_name);

//
// Check that it is an externally triggered polling object. If it is not the
// case, throw exception
//

	long tmp_upd = (*ite)->get_upd();
	if (tmp_upd != 0)
	{
		TangoSys_OMemStream o;

		o << "Polling for command ";
		o << name;
		o << " (device " << dev->get_name() << ") ";
		o << " is not externally triggered.";
		o << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
					(const char *)"Util::trigger_cmd_polling");
	}

//
// Find out which thread is in charge of the device.
//

	PollingThreadInfo *th_info;

	int poll_th_id = get_polling_thread_id_by_name(dev->get_name_lower().c_str());
	if (poll_th_id == 0)
	{
		TangoSys_OMemStream o;
		o << "Can't find a polling thread for device " << dev->get_name() << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
							(const char *)"Util::trigger_cmd_polling");
	}

	th_info = get_polling_thread_info_by_id(poll_th_id);

//
// Send command to the polling thread but wait in case of previous cmd
// still not executed
//

	TangoMonitor &mon = th_info->poll_mon;
	PollThCmd &shared_cmd = th_info->shared_data;

	{
		omni_mutex_lock sync(mon);
		if (shared_cmd.trigger == true)
		{
			mon.wait();
		}
		shared_cmd.trigger = true;
		shared_cmd.dev = dev;
		shared_cmd.name = obj_name;
		shared_cmd.type = Tango::POLL_CMD;

		mon.signal();

		cout4 << "Trigger sent to polling thread" << endl;

//
// Wait for thread to execute command
//

		TangoMonitor &dev_mon = dev->get_dev_monitor();
		omni_thread *th = omni_thread::self();

		while (shared_cmd.trigger == true)
		{

//
// Warning: It's possible to have a deadlock here (experienced under
// Windows) in case of this method being called from a command (or attribute
// methods) which are rapidly sent by the client.
// Client request cmd1 which send trigger to the polling thread
// The polling thread wake up clear shared_cmd.trigger and try to
// execute the command. But cmd 1 thread still owns the device monitor and
// polling thread wait. cmd 1 finished and client immediately send the
// command a new time. On Windows, it may happens that the polling
// thread is not activated just after the cmd thread has released the
// device monitor. As the client sent a new command, the device monitor
// is immediately re-taken by the thread executing the new command sent by
// the client. An order is sent to the polling thread and the cmd
// thread reach this code. It will wait for polling thread to clear
// shared_cmd.trigger. But, the polling thread is already waiting for
// the device monitor and ..... deadlock....
//

			bool deadlock = false;
			long lock_ctr = 0;
			if (th->id() == dev_mon.get_locking_thread_id())
			{
				cout4 << "Possible deadlock detected!" << endl;
				deadlock = true;
				lock_ctr = dev_mon.get_locking_ctr();
				for (long loop = 0;loop < lock_ctr;loop++)
					dev_mon.rel_monitor();
			}

			int interupted = mon.wait(DEFAULT_TIMEOUT);
			if (deadlock == true)
			{
				for (long loop = 0;loop < lock_ctr;loop++)
					dev_mon.get_monitor();
			}

			if ((shared_cmd.trigger == true) && (interupted == 0))
			{
				cout4 << "TIME OUT" << endl;
				Except::throw_exception((const char *)"API_CommandTimedOut",
					        	(const char *)"Polling thread blocked !!!",
					        	(const char *)"Util::trigger_cmd_polling");
			}
		}
	}
	cout4 << "Thread cmd normally executed" << endl;
}

//+----------------------------------------------------------------------------
//
// method : 		Util::clean_attr_polled_prop()
//
// description : 	Clean in database the prop used to memorized which attribute
//					are polled
//
//-----------------------------------------------------------------------------

void Util::clean_attr_polled_prop()
{
	if (Tango::Util::_UseDb == true)
	{
		DbData send_data;
		DbDatum db_info("polled_attr");

		for (unsigned int loop = 0;loop < ext->polled_dyn_attr_names.size();loop++)
		{
			vector<string>::iterator ite_attr = find(ext->polled_att_list.begin(),ext->polled_att_list.end(), ext->polled_dyn_attr_names[loop]);
			if (ite_attr != ext->polled_att_list.end())
			{
				ite_attr = ext->polled_att_list.erase(ite_attr);
				ext->polled_att_list.erase(ite_attr);
			}
			else
			{
				TangoSys_OMemStream o;

				o << "Polling properties for attribute " << ext->polled_dyn_attr_names[loop] << " on device " << ext->dyn_att_dev_name;
				o << " not found device in polled attribute list!" << ends;

				Except::throw_exception((const char *)"API_MethodArgument",o.str(),
										(const char *)"Util::clean_attr_polling_prop");
			}
		}

		db_info << ext->polled_att_list;
		send_data.push_back(db_info);

		if (db_info.size() == 0)
			db->delete_device_property(ext->dyn_att_dev_name,send_data);
		else
			db->put_device_property(ext->dyn_att_dev_name,send_data);
	}
}


//+----------------------------------------------------------------------------
//
// method : 		Util::create_poll_thread()
//
// description : 	Create a polling thread for the device specified as parameter
//					If the thread already exist, do nothing
//					If the pool is full, associate this device to an already existing
//					thread
//
// argin : - dev_name : The device name
//		   - startup : True if this method is called during DS startup.
//					   In such a case, some exception should not be thrown
//		   - smallest_upd : The smallest upd !
//
// This method returns -2 if the pool conf. does not need any update
// It returns -1 if a new thread has been created
// It returns the index in the pool conf of the entry which has to be modified
// when the device is associated to an already existing thread
//
//-----------------------------------------------------------------------------

int Util::create_poll_thread(const char *dev_name,bool startup,int smallest_upd)
{
	int ret = -2;
	string local_dev_name(dev_name);
	transform(local_dev_name.begin(),local_dev_name.end(),local_dev_name.begin(),::tolower);

//
// Is there is already a polling thread for this device, simply returns
//

	map<string,int>::iterator ite;
	ite = ext->dev_poll_th_map.find(local_dev_name);
	if (ite != ext->dev_poll_th_map.end())
	{
		if (smallest_upd != -1)
		{
			PollingThreadInfo *th_info = get_polling_thread_info_by_id(ite->second);
			if (smallest_upd < th_info->smallest_upd)
				th_info->smallest_upd = smallest_upd;
		}
		return ret;
	}

//
// Check if the pool is full
//

	if (ext->poll_ths.size() != ext->poll_pool_size)
	{

//
// Get from the pool conf which device(s) have the same thread than this one
//

		vector<string> asso_devs;
		int ind;
		ind = get_th_polled_devs(local_dev_name,asso_devs);

		if (asso_devs.empty() != true)
		{
			vector<string>::iterator it;

//
// If we find a thread for one of the associated device, no need to create a new one
// simply add the device entry in the map <dev_name,thread id>
//

			for (it = asso_devs.begin();it != asso_devs.end();++it)
			{
				if (*it == local_dev_name)
					continue;

				if ((ite = ext->dev_poll_th_map.find(*it)) != ext->dev_poll_th_map.end())
				{
					ext->dev_poll_th_map.insert(make_pair(local_dev_name,ite->second));
                    PollingThreadInfo *th_info = get_polling_thread_info_by_id(ite->second);
                    th_info->polled_devices.push_back(local_dev_name);
                    th_info->nb_polled_objects++;
					if (smallest_upd != -1)
					{
						if (smallest_upd < th_info->smallest_upd)
							th_info->smallest_upd = smallest_upd;
					}
					ret = ind;
					return ret;
				}
			}
		}

//
// Create a new polling thread and start it
//

		PollingThreadInfo *pti_ptr = new PollingThreadInfo();
		if (smallest_upd != -1)
			pti_ptr->smallest_upd = smallest_upd;
		pti_ptr->poll_th = new PollThread(pti_ptr->shared_data,pti_ptr->poll_mon,false);
		pti_ptr->poll_th->start();
		int poll_th_id = pti_ptr->poll_th->id();
		pti_ptr->thread_id = poll_th_id;
		pti_ptr->polled_devices.push_back(local_dev_name);
		pti_ptr->nb_polled_objects++;

		ext->poll_ths.push_back(pti_ptr);

		ext->dev_poll_th_map.insert(make_pair(local_dev_name,poll_th_id));
		ret = -1;
	}
	else
	{

//
// Get from the pool conf which device(s) have the same thread than this one
//

		vector<string> asso_devs;
		int ind;
		ind = get_th_polled_devs(local_dev_name,asso_devs);

		if (asso_devs.empty() != true)
		{
			vector<string>::iterator it;

//
// If we find a thread for one of the associated device,
// simply add the device entry in the map <dev_name,thread id>
//

			for (it = asso_devs.begin();it != asso_devs.end();++it)
			{
				if (*it == local_dev_name)
					continue;

				if ((ite = ext->dev_poll_th_map.find(*it)) != ext->dev_poll_th_map.end())
				{
					ext->dev_poll_th_map.insert(make_pair(local_dev_name,ite->second));
					if (smallest_upd != -1)
					{
						PollingThreadInfo *th_info = get_polling_thread_info_by_id(ite->second);
                        th_info->polled_devices.push_back(local_dev_name);
                        th_info->nb_polled_objects++;
						if (smallest_upd < th_info->smallest_upd)
							th_info->smallest_upd = smallest_upd;
					}
					ret = ind;
					return ret;
				}
			}
		}

//
// Find the thread with the lowest polled object number
//

		vector<PollingThreadInfo *>::iterator iter,lower_iter;
		int lower_polled_objects = ext->poll_ths[0]->nb_polled_objects;
		lower_iter = ext->poll_ths.begin();

		for (iter = ext->poll_ths.begin();iter != ext->poll_ths.end();++iter)
		{
			if ((*iter)->nb_polled_objects <= lower_polled_objects)
			{
				lower_polled_objects = (*iter)->nb_polled_objects;
				lower_iter = iter;
			}
		}

//
// Find a device already assigned to this thread and then get the device entry in
// the thread pool
//

		string d_name;

		for (ite = ext->dev_poll_th_map.begin();ite != ext->dev_poll_th_map.end();++ite)
		{
			if (ite->second == (*lower_iter)->thread_id)
			{
				d_name = ite->first;
				break;
			}
		}

		if (ite == ext->dev_poll_th_map.end())
		{
			TangoSys_OMemStream o;

			o << "The polling threads pool is full.\n";
			o << "Device " << dev_name << " should be polled by the thread already polling " << d_name;
			o << " but this device is not defined in the polled device map!!" << ends;

			Except::throw_exception((const char *)"API_PolledDeviceNotInPoolMap",o.str(),
									(const char *)"Util::create_poll_thread");
		}

		ind = get_dev_entry_in_pool_conf(d_name);

		if ((ind == -1) && (startup == false))
		{
			TangoSys_OMemStream o;

			o << "The polling threads pool is full.\n";
			o << "Device " << dev_name << " should be polled by the thread already polling " << d_name;
			o << " but this device is not defined in the pool configuration!!" << ends;

			Except::throw_exception((const char *)"API_PolledDeviceNotInPoolConf",o.str(),
									(const char *)"Util::create_poll_thread");
		}

//
// Assign this device to the thread which "seems" to have the less work
//

		(*lower_iter)->polled_devices.push_back(local_dev_name);
		(*lower_iter)->nb_polled_objects++;
		ext->dev_poll_th_map.insert(make_pair(local_dev_name,(*lower_iter)->thread_id));
		if (smallest_upd != -1)
		{
			if (smallest_upd < (*lower_iter)->smallest_upd)
				(*lower_iter)->smallest_upd = smallest_upd;
		}
		ret = ind;
	}

	return ret;
}

//+----------------------------------------------------------------------------
//
// method : 		Util::stop_all_polling_threads()
//
// description : 	Stop all polling threads used in the polling thread pool
//
//-----------------------------------------------------------------------------

void Util::stop_all_polling_threads()
{
	vector<PollingThreadInfo *>::iterator iter;

	for (iter = ext->poll_ths.begin();iter != ext->poll_ths.end();++iter)
	{
		TangoMonitor &mon = (*iter)->poll_mon;
		PollThCmd &shared_cmd = (*iter)->shared_data;

		{
			omni_mutex_lock sync(mon);

			shared_cmd.cmd_pending = true;
			shared_cmd.cmd_code = POLL_EXIT;

			mon.signal();
		}

//
// join with the polling thread
//

		void *dummy_ptr;
		(*iter)->poll_th->join(&dummy_ptr);
	}

	for (iter = ext->poll_ths.begin();iter != ext->poll_ths.end();++iter)
		delete (*iter);
	ext->poll_ths.clear();
}

//+----------------------------------------------------------------------------
//
// method : 		Util::stop_heartbeat_thread()
//
// description : 	Stop the heartbeat thread
//
//-----------------------------------------------------------------------------

void Util::stop_heartbeat_thread()
{
	TangoMonitor &mon = ext->poll_mon;
	PollThCmd &shared_cmd = ext->shared_data;

	{
		omni_mutex_lock sync(mon);

		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_EXIT;

		mon.signal();
	}

//
// join with the polling thread
//

	void *dummy_ptr;
	ext->heartbeat_th->join(&dummy_ptr);
}

//+----------------------------------------------------------------------------
//
// method : 		Util::get_poll_th_id_by_name()
//
// description : 	Get the ID of the thread in charge of the device polling
//
// argin() : dev_name : The device name
//
// If there is no thread dedicated to the device, the return value is 0
//
//-----------------------------------------------------------------------------

int Util::get_polling_thread_id_by_name(const char *dev_name)
{
	int ret = 0;

	string local_dev_name(dev_name);
	transform(local_dev_name.begin(),local_dev_name.end(),local_dev_name.begin(),::tolower);

	map<string,int>::iterator iter;
	iter = ext->dev_poll_th_map.find(local_dev_name);
	if (iter != ext->dev_poll_th_map.end())
		ret = iter->second;

	return ret;
}


//+----------------------------------------------------------------------------
//
// method : 		Util::get_polling_thread_info_by_id()
//
// description : 	Return the PollingThreadInfo for the thread with the ID
//					specified as the input argument
//
// argin() : id : The polling thread identifier
//
// If there is no polling thread with this ID, throws an exception
//
//-----------------------------------------------------------------------------

PollingThreadInfo *Util::get_polling_thread_info_by_id(int th_id)
{
	PollingThreadInfo *ret_ptr = NULL;
	vector<PollingThreadInfo *>::iterator iter;

	for (iter = ext->poll_ths.begin();iter != ext->poll_ths.end();++iter)
	{
		if ((*iter)->thread_id == th_id)
		{
			ret_ptr = (*iter);
			break;
		}
	}

	if (iter == ext->poll_ths.end())
	{
		TangoSys_OMemStream o;

		o << "There is no polling thread with ID = " << th_id << " in the polling threads pool"<< ends;

		Except::throw_exception((const char *)"API_PollingThreadNotFound",o.str(),
								(const char *)"Util::get_polling_thread_info_by_id");
	}

	return ret_ptr;
}

//+----------------------------------------------------------------------------
//
// method : 		Util::check_poll_conf()
//
// description : 	Check the pool configuration coherency.
//					This means:
//					- All devices defined in the pool conf has to be created
//					  in the DS
//					- All objects to be polled has to be coherent
//					- The pool size must be coherent with the conf
//
// argin() : - admin_dev : The DS admin device
//			 - pool_size : The pool size (max number of thread)
//
//-----------------------------------------------------------------------------

void Util::check_pool_conf(DServer *admin_dev,unsigned long pool_size)
{

//
// Simply returns if the conf. is empty
//

	if (ext->poll_pool_conf.empty() == true)
		return;


	vector<string> mod_conf = ext->poll_pool_conf;

//
// First, get the list of devices instanciated in this server and check if the polled
// devices are defined within the server
//

	Tango::DevVarStringArray *dev_list = admin_dev->query_device();
	unsigned int nb_dev = dev_list->length();
	unsigned int loop;
	int stop = 0;
	vector<string> splitted;

	vector<string>::iterator iter,iter_entry;
	for (iter = mod_conf.begin();iter != mod_conf.end();++iter)
	{
		split_string(*iter,',',splitted);
		stop++;

		string mod_entry;
		for (iter_entry = splitted.begin(); iter_entry != splitted.end();++iter_entry)
		{

//
// Check that this device is not already defined
//

			string::size_type po;
			if ((po = mod_entry.find(*iter_entry)) != string::npos)
				continue;
			else if (is_dev_already_in_pool_conf(*iter_entry,mod_conf,stop - 1) == true)
				continue;

//
// Check if the device is instanciated in this server
//

			string entry_lower(*iter_entry);
			transform(entry_lower.begin(),entry_lower.end(),entry_lower.begin(),::tolower);

			for (loop = 0;loop < nb_dev;++loop)
			{
				string real_dev_name((*dev_list)[loop]);
				string::size_type pos;
				pos = real_dev_name.find(':');
				pos = pos + 2;
				real_dev_name = real_dev_name.substr(pos);

				transform(real_dev_name.begin(),real_dev_name.end(),real_dev_name.begin(),::tolower);

				if (entry_lower == real_dev_name)
					break;
			}

//
// The device is not defined in the DS, remove it from the conf.
// (Do not copy it in the new conf built by this method)
//

			if (loop == nb_dev)
			{
				cout << "WARNING: Device " << *iter_entry << " is used in polling threads pool configuration";
				cout << " but it is not defined in DS"<< endl;
				cout << "The pool configuration will be automatically updated" << endl;
			}
			else
			{

//
// Now, we know that the device exists in the server, but is it really polled ?
//

				DeviceImpl *dev = get_device_by_name(*iter_entry);

				vector<string> &poll_cmd_list = dev->get_polled_cmd();
				vector<string> &poll_attr_list = dev->get_polled_attr();

				if ((poll_cmd_list.empty() == true) && (poll_attr_list.empty() == true))
				{
					cout << "WARNING: Device " << *iter_entry << " is used in polling threads pool configuration";
					cout << " but it does not have any cmd/attr polled"<< endl;
					cout << "The pool configuration will be automatically updated" << endl;
				}
				else
				{
					if (mod_entry.empty() == true)
						mod_entry = *iter_entry;
					else
						mod_entry = mod_entry + ',' + *iter_entry;
				}
			}
		}

//
// Modify the conf. if some incoherencies have been found
//

		if (mod_entry.empty() == true)
		{
			iter = mod_conf.erase(iter);
			if (iter == mod_conf.end())
				break;
            else
            {
                if (iter == mod_conf.begin())
                    --stop;
                --iter;
            }
		}
		else if (mod_entry != *iter)
			*iter = mod_entry;
	}
	delete dev_list;

//
// Now, check that the thread defined by the conf. is <= than the pool size
//

	if (mod_conf.size() > pool_size)
	{
		cout << "WARNING: More threads defined in the polling threads pool configuration";
		cout << " than in its size ("<< mod_conf.size() << " > " << pool_size << ")" << endl;
		cout << "The pool configuration will be automatically updated" << endl;

//
// If we have more threads in the conf than in the pool, distribute the extra thread
// devices to the still existing threads
//

		long nb_extra_th = mod_conf.size() - pool_size;
		for (long i = 0;i < nb_extra_th;i++)
		{
			vector<string> polled_dev_th;
			get_th_polled_devs(i + pool_size,polled_dev_th);
			unsigned long loop = 0;
			vector<string>::iterator it;
			for (it = polled_dev_th.begin();it != polled_dev_th.end();++it)
			{
				mod_conf[loop] = mod_conf[loop] + ',' + *it;
				loop++;
				if (loop == pool_size)
					loop = 0;
			}
		}

		vector<string>::iterator it = mod_conf.begin();
		it = it + pool_size;
		mod_conf.erase(it,mod_conf.end());
	}

//
// If necessary, update the conf in db
//

	if (mod_conf != ext->poll_pool_conf)
	{
		ext->conf_needs_db_upd = true;
		ext->poll_pool_conf = mod_conf;
	}

}

//+----------------------------------------------------------------------------
//
// method : 		Util::check_dev_poll()
//
// description : 	Check if all the cmd/attr to be polled for a device are
//					really supported by the device
//
// argin() : - poll_cmd_list : The polled command(s)
//			 - poll_attr_list : The polled attr(s)
//			 - dev : The device pointer
//
// This method returns 0 if no polling conf has been modified.
// Otherwise, it returns -1 if only the command list has been modified,
// -2 if the only the attribute list has been modified and -3 if both
// of them have been modified
//-----------------------------------------------------------------------------

int Util::check_dev_poll(vector<string> &poll_cmd_list,vector<string> &poll_attr_list,DeviceImpl *dev)
{
	int ret = 0;

//
// First, get device commands list
//

	vector<Command *> &cmd_list = dev->get_device_class()->get_command_list();

//
// Check polled cmds
//

	vector<string>::iterator iter;

	for (iter = poll_cmd_list.begin();iter != poll_cmd_list.end();iter = iter + 2)
	{
		string polled_cmd = *iter;
		transform(polled_cmd.begin(),polled_cmd.end(),polled_cmd.begin(),::tolower);

		vector<Command *>::iterator i_cmd;

		for (i_cmd = cmd_list.begin();i_cmd < cmd_list.end();++i_cmd)
		{
			if ((*i_cmd)->get_lower_name() == polled_cmd)
				break;
		}

		if (i_cmd == cmd_list.end())
		{
			cout << "WARNING: Device " << dev->get_name() << " is configured to be polled with";
			cout << " a command which does not exist anymore"<< endl;
			cout << "The device polling configuration will be automatically updated" << endl;

			ret = -1;
			iter  = poll_cmd_list.erase(iter,iter + 2);
			if (iter == poll_cmd_list.end())
				break;
		}
	}

//
// Now, get device attribute list
//

	vector<Attribute *> &att_list = dev->get_device_attr()->get_attribute_list();

//
// Check polled attributes
//

	for (iter = poll_attr_list.begin();iter != poll_attr_list.end();iter = iter + 2)
	{
		string polled_attr = *iter;
		transform(polled_attr.begin(),polled_attr.end(),polled_attr.begin(),::tolower);

		vector<Attribute *>::iterator i_attr;

		for (i_attr = att_list.begin();i_attr < att_list.end();++i_attr)
		{
			if ((*i_attr)->get_name_lower() == polled_attr)
				break;
		}

		if (i_attr == att_list.end())
		{
			cout << "WARNING: Device " << dev->get_name() << " is configured to be polled with";
			cout << " an attribute which does not exist anymore (" << polled_attr << ")" << endl;
			cout << "The device polling configuration will be automatically updated" << endl;

			if (ret == -1)
				ret = -3;
			else
				ret = -2;
			iter  = poll_attr_list.erase(iter,iter + 2);
			if (iter == poll_attr_list.end())
				break;
		}
	}

	return ret;
}

//+----------------------------------------------------------------------------
//
// method : 		Util::split_string()
//
// description : 	Split a string according to a delimiter
//
// argin() : - the_str : ref to the string to be plitted
//			 - delim : The delimiter character
//			 - plited_str : The splitted string returned in a vector of individual elt
//
//-----------------------------------------------------------------------------

void Util::split_string(string &the_str,char delim,vector<string> &splitted_str)
{
	string::size_type pos,start;
	splitted_str.clear();

	start = 0;
	while ((pos = the_str.find(delim,start)) != string::npos)
	{
		splitted_str.push_back(the_str.substr(start,pos - start));
		start = pos + 1;
	}

//
// The last element
//

	splitted_str.push_back(the_str.substr(start));
}

//+----------------------------------------------------------------------------
//
// method : 		Util::upd_polling_prop()
//
// description : 	Update polling related properties in Db
//
// argin() : - upd_devs : ref to a vector with one elt for each dev with polling
//						  prop. to be updated
//			 - admin_dev : The DS admin device
//
//-----------------------------------------------------------------------------

void Util::upd_polling_prop(vector<DevDbUpd> &upd_devs,DServer *admin_dev)
{
	vector<DeviceClass *> &tmp_cl_list = admin_dev->get_class_list();
	unsigned long i;

//
// A loop on each device with prop. to be updated
//

	for (i = 0;i < upd_devs.size();i++)
	{
		vector<DeviceImpl *> &dev_list = tmp_cl_list[upd_devs[i].class_ind]->get_device_list();

		vector<string> &poll_cmd_list = dev_list[upd_devs[i].dev_ind]->get_polled_cmd();
		vector<string> &poll_attr_list = dev_list[upd_devs[i].dev_ind]->get_polled_attr();

		DbData del_prop;
		DbData upd_prop;

//
// Is it necessary to update or to delete the property?
//

		if ((upd_devs[i].mod_prop == -1) || (upd_devs[i].mod_prop == -3))
		{
			if (poll_cmd_list.size() == 0)
				del_prop.push_back(DbDatum("polled_cmd"));
			else
			{
				upd_prop.push_back(DbDatum("polled_cmd"));
				upd_prop[0] << poll_cmd_list;
			}
		}

		if ((upd_devs[i].mod_prop == -2) || (upd_devs[i].mod_prop == -3))
		{
			if (poll_attr_list.size() == 0)
				del_prop.push_back(DbDatum("polled_attr"));
			else
			{
				upd_prop.push_back(DbDatum("polled_attr"));
				upd_prop[0] << poll_attr_list;
			}
		}

//
// Update the database
//

		bool retry = true;
		long db_retries = DB_START_PHASE_RETRIES;

		while (retry == true)
		{
			try
			{
				if (del_prop.empty() == false)
					get_database()->delete_device_property(dev_list[upd_devs[i].dev_ind]->get_name(),del_prop);
				if (upd_prop.empty() == false)
					get_database()->put_device_property(dev_list[upd_devs[i].dev_ind]->get_name(),upd_prop);
				retry = false;
			}
			catch (Tango::CommunicationFailed &)
			{
				if (is_svr_starting() == true)
				{
					db_retries--;
					if (db_retries == 0)
						throw;
				}
				else
					throw;
			}
		}

//
// If now the device does not have any objects polled (no cmd, no attr), it must
// be removed from the pool conf.
//

		if ((poll_attr_list.empty() == true) && (poll_cmd_list.empty() == true))
		{
			vector<string>::iterator iter;

			for (iter = ext->poll_pool_conf.begin();iter != ext->poll_pool_conf.end();++iter)
			{
				string tmp = *iter;
				string &d_name = dev_list[upd_devs[i].dev_ind]->get_name();
				if (tmp.find(d_name) != string::npos)
				{
					string::size_type pos;
					if ((pos = tmp.find(',')) == string::npos)
					{
						iter = ext->poll_pool_conf.erase(iter);
					}
					else
					{
						tmp.erase(pos,d_name.size());
					}
					ext->conf_needs_db_upd = true;
					break;
				}
			}
		}
	}

//
// Update the polling pool conf if needed
//

	if (ext->conf_needs_db_upd == true)
	{
		DbData upd_conf;
		upd_conf.push_back(DbDatum("polling_threads_pool_conf"));

		bool retry = true;
		long db_retries = DB_START_PHASE_RETRIES;

		while (retry == true)
		{
			try
			{
				if (ext->poll_pool_conf.empty() == true)
				{
					get_database()->delete_device_property(admin_dev->get_name(),upd_conf);
				}
				else
				{

//
// The max device property size in db is limited to 255.
// If we have only one thread, it is easy to catch this threshold.
// In case this threshold is reached, split entry in several sub-entries
// using the \ characters at the end of each sub-entries
//

                    vector<string>::iterator iter;
                    vector<string> new_vect;

                    for (iter = ext->poll_pool_conf.begin();iter != ext->poll_pool_conf.end();++iter)
                    {
                        string v_entry = *iter;
                        unsigned int length = v_entry.size();
                        int nb_lines = (length / MaxDevPropLength) + 1;
                        if (nb_lines > 1)
                        {
                            string::size_type start;
                            start = 0;

                            for (int i = 0;i < nb_lines;i++)
                            {
                                string sub = v_entry.substr(start,MaxDevPropLength);
                                if (i < (nb_lines - 1))
                                    sub = sub + '\\';
                                cout << "Sub string = " << sub << endl;
                                start = start + MaxDevPropLength;
                                new_vect.push_back(sub);
                            }
                        }
                        else
                            new_vect.push_back(v_entry);
                    }

					upd_conf[0] << new_vect;
					get_database()->put_device_property(admin_dev->get_name(),upd_conf);
				}
				retry = false;
			}
			catch (Tango::CommunicationFailed &)
			{
				if (is_svr_starting() == true)
				{
					db_retries--;
					if (db_retries == 0)
						throw;
				}
				else
					throw;
			}
		}
	}
}

//+----------------------------------------------------------------------------
//
// method : 		Util::get_th_polled_devs()
//
// description : 	Get from the polling pool configuration which devices should
//					be polled by the thread which is in charge of device dev
//
// argin() : - dev : The device name
//			 - th_polled_devs : List of devices also polled by the thread in
//								charge of dev
//
//-----------------------------------------------------------------------------

int Util::get_th_polled_devs(string &dev,vector<string> &th_polled_devs)
{
	th_polled_devs.clear();

	vector<string>::iterator iter;
	unsigned int dev_nb_char = dev.size();
	unsigned int pool_dev_nb_char;

	for (iter = ext->poll_pool_conf.begin();iter != ext->poll_pool_conf.end();++iter)
	{
		string tmp = *iter;
		string::size_type pos,end_pos;
		pos = tmp.find(dev);
		if (pos != string::npos)
		{
            end_pos = tmp.find(',',pos);
            if (end_pos != string::npos)
                pool_dev_nb_char = end_pos - pos;
            else
                pool_dev_nb_char = tmp.size() - pos;
            if (dev_nb_char == pool_dev_nb_char)
            {
                split_string(tmp,',',th_polled_devs);
                break;
            }
		}
	}

	return iter - ext->poll_pool_conf.begin();
}

void Util::get_th_polled_devs(long i,vector<string> &th_polled_devs)
{
	th_polled_devs.clear();

	string tmp = ext->poll_pool_conf[i];
	split_string(tmp,',',th_polled_devs);
}

//+----------------------------------------------------------------------------
//
// method : 		Util::build_first_pool_conf()
//
// description : 	Create a polling tread pool configuration when this data is
//					not defined
//
//-----------------------------------------------------------------------------

void Util::build_first_pool_conf(vector<string> &pool_conf)
{
	vector<PollingThreadInfo *>::iterator iter;

	for (iter = ext->poll_ths.begin();iter != ext->poll_ths.end();++iter)
	{
		string tmp;
		vector<string>::iterator it;
		for (it = (*iter)->polled_devices.begin();it != (*iter)->polled_devices.end();++it)
		{
			if (it != (*iter)->polled_devices.begin())
				tmp = tmp + ',' + *it;
			else
				tmp = *it;
		}
		pool_conf.push_back(tmp);
	}
}

//+----------------------------------------------------------------------------
//
// method : 		Util::is_dev_already_in_pool_conf()
//
// description : 	Check if a device is already defined in the pool conf
//
// args() : - dev_name : The device name
//			- pool : The polling threads pool configuration
//          - stop : The index in poll conf where the search should be stopped
//
// This method returns true if the device is already defined in the pool.
// Otherwise, returns false (amazing no!)
//
//-----------------------------------------------------------------------------

bool Util::is_dev_already_in_pool_conf(string &dev_name,vector<string>& pool,int stop)
{
	vector<string>::iterator iter;

	for (iter = pool.begin();iter != pool.begin() + stop;++iter)
	{
		vector<string> dev_list;
		vector<string>::iterator it;

		split_string(*iter,',',dev_list);
		for (it = dev_list.begin();it != dev_list.end();++it)
		{
			if (*it == dev_name)
				return true;
		}
	}

	return false;
}

//+----------------------------------------------------------------------------
//
// method : 		Util::get_dev_entry_in_pool_conf()
//
// description : 	Get in which entry a device is used in the pool conf
//
// args() : - dev_name : The device name
//
// This method returns the index in the pool conf if the device has been
// found. Otherwise, it returns -1
//
//-----------------------------------------------------------------------------

int Util::get_dev_entry_in_pool_conf(string &dev_name)
{
	vector<string>::iterator iter;
	unsigned int dev_nb_char = dev_name.size();
	unsigned int pool_dev_nb_char;

	for (iter = ext->poll_pool_conf.begin();iter != ext->poll_pool_conf.end();++iter)
	{
		string tmp = *iter;
		string::size_type pos,end_pos;
		pos = tmp.find(dev_name);
		if (pos != string::npos)
		{
           end_pos = tmp.find(',',pos);
            if (end_pos != string::npos)
                pool_dev_nb_char = end_pos - pos;
            else
                pool_dev_nb_char = tmp.size() - pos;
            if (dev_nb_char == pool_dev_nb_char)
            {
                break;
            }
		}
	}

	if (iter != ext->poll_pool_conf.end())
		return iter - ext->poll_pool_conf.begin();
	else
		return -1;
}

//+----------------------------------------------------------------------------
//
// method : 		Util::remove_dev_from_polling_map()
//
// description : 	Remove the device from the polled device map
//
//-----------------------------------------------------------------------------

void Util::remove_dev_from_polling_map(string &dev_name)
{
	map<string,int>::iterator iter;
	iter = ext->dev_poll_th_map.find(dev_name);
	if (iter != ext->dev_poll_th_map.end())
		ext->dev_poll_th_map.erase(iter);
}

//+----------------------------------------------------------------------------
//
// method : 		Util::remove_polling_thread_info_by_id()
//
// description : 	Remove all the polling thread info from the vector of
//					polling thread info for a specific thread
//
// argin(s) : - id : The polling thread id
//
//-----------------------------------------------------------------------------

void Util::remove_polling_thread_info_by_id(int th_id)
{
	vector<PollingThreadInfo *>::iterator iter;

	for (iter = ext->poll_ths.begin();iter != ext->poll_ths.end();++iter)
	{
		if ((*iter)->thread_id == th_id)
		{
			delete (*iter);
			ext->poll_ths.erase(iter);
			return;
		}
	}

	if (iter == ext->poll_ths.end())
	{
		TangoSys_OMemStream o;

		o << "There is no polling thread with ID = " << th_id << " in the polling threads pool"<< ends;

		Except::throw_exception((const char *)"API_PollingThreadNotFound",o.str(),
								(const char *)"Util::remove_polling_thread_info_by_id");
	}

	return;
}

} // End of Tango namespace
