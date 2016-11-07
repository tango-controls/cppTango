static const char *RcsId = "$Id$";

//+==================================================================================================================
//
// file :               utils_polling.cpp
//
// description :        C++ source for all the methods of the Util class related to polling
//
// project :            TANGO
//
// author(s) :         	E.Taurel
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#include <iostream>
#include <algorithm>
#include "polling/trigger_polling_command.hxx"
#include "polling/stop_polling_command.hxx"
#include "polling/polling_queue.hxx"
#include "polling/event_system.hxx"
#include "polling/polling_thread.hxx"
#include "polling/helpers.hxx"

namespace Tango {
//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::polling_configure()
//
// description :
//		This method sends command to the polling thread for all cmd/attr with polling configuration stored in db.
//		This is done in separate thread in order to equally spread all the polled objects polling time on the
//		smallest polling period.
//
//------------------------------------------------------------------------------------------------------------------

    void Util::polling_configure() {
        cout4 << "Entering polling_configure()" << endl;

//
// Get the polling threads pool conf from the DServer device
//

        DServer *admin_dev = get_dserver_device();
        set_polling_threads_pool_size(admin_dev->get_poll_th_pool_size());
        poll_pool_conf = admin_dev->get_poll_th_conf();

//
// Check the coherency of the polling thread pool configuration
//

        check_pool_conf(admin_dev, poll_pool_size);

//
// Send a stop polling command to thread in order not to poll devices
//

        admin_dev->stop_polling();

        vector<DeviceClass *> &tmp_cl_list = admin_dev->get_class_list();
        int upd;
        TangoSys_MemStream s;

//
// Create the structure used to send data to the polling thread and store them in a vector
//

        vector<DevVarLongStringArray *> v_poll_cmd;
        vector<DevVarLongStringArray *> v_poll_cmd_fwd;
        vector<DevDbUpd> dev_db_upd;

//
// A loop on each class and each device in class
//

        int smallest_upd = 0;
        bool first_loop = true;

        for (size_t i = 0, size = tmp_cl_list.size(); i < size; i++) {
            vector<DeviceImpl *> &dev_list = tmp_cl_list[i]->get_device_list();
            for (size_t j = 0; j < dev_list.size(); j++) {
                v_poll_cmd.clear();

                vector<string> &poll_cmd_list = dev_list[j]->get_polled_cmd();
                vector<string> &poll_attr_list = dev_list[j]->get_polled_attr();

//
// Check that cmd_list and attr_list have a correct syntax
//

                if ((poll_cmd_list.size() % 2) == 1) {
                    TangoSys_OMemStream o;
                    o << "System property polled_cmd for device " << dev_list[j]->get_name() << " has wrong syntax"
                      << ends;
                    Except::throw_exception((const char *) API_BadConfigurationProperty,
                                            o.str(),
                                            (const char *) "Util::polling_configure");
                }

                if ((poll_attr_list.size() % 2) == 1) {
                    TangoSys_OMemStream o;
                    o << "System property polled_attr for device " << dev_list[j]->get_name() << " has wrong syntax"
                      << ends;
                    Except::throw_exception((const char *) API_BadConfigurationProperty,
                                            o.str(),
                                            (const char *) "Util::polling_configure");
                }

//
// Check the device polling definition (existing polled cmd/attr ?)
// If the polling needs to be modified, memorize the index in class and device loop to re-find it later.
//

                int ret;
                if ((ret = check_dev_poll(poll_cmd_list, poll_attr_list, dev_list[j])) != 0) {
                    DevDbUpd tmp;
                    tmp.class_ind = i;
                    tmp.dev_ind = j;
                    tmp.mod_prop = ret;
                    dev_db_upd.push_back(tmp);
                }

//
// A loop on each command
//

                for (size_t k = 0, size = poll_cmd_list.size(); k < size; k++) {
                    DevVarLongStringArray *send = new DevVarLongStringArray();
                    send->lvalue.length(1);
                    send->svalue.length(3);

                    send->svalue[0] = Tango::string_dup(dev_list[j]->get_name().c_str());
                    send->svalue[1] = Tango::string_dup("command");
                    send->svalue[2] = Tango::string_dup(poll_cmd_list[k].c_str());

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

                    if (first_loop == true) {
                        smallest_upd = upd;
                        first_loop = false;
                    } else {
                        if (upd < smallest_upd)
                            smallest_upd = upd;
                    }

                    v_poll_cmd.push_back(send);
                    k++;
                }

//
// A loop on each attribute
//

                for (size_t k = 0, size = poll_attr_list.size(); k < size; k++) {
                    DevVarLongStringArray *send = new DevVarLongStringArray();
                    send->lvalue.length(1);
                    send->svalue.length(3);

                    send->svalue[0] = Tango::string_dup(dev_list[j]->get_name().c_str());
                    send->svalue[1] = Tango::string_dup("attribute");
                    send->svalue[2] = Tango::string_dup(poll_attr_list[k].c_str());

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

                    Attribute &att = dev_list[j]->get_device_attr()->get_attr_by_name(poll_attr_list[k].c_str());
                    if (att.is_fwd_att() == true) {
                        v_poll_cmd_fwd.push_back(send);
                    } else {
                        if (first_loop == true) {
                            smallest_upd = upd;
                            first_loop = false;
                        } else {
                            if (upd < smallest_upd)
                                smallest_upd = upd;
                        }
                        v_poll_cmd.push_back(send);
                    }

                    k++;
                }

//
// Make sure we have a thread in the pool to poll this device
//

                unsigned long nb_cmd = v_poll_cmd.size();
                if (nb_cmd != 0) {
                    bool poll_bef_9 = false;
                    if (admin_dev->is_polling_bef_9_def() == true)
                        poll_bef_9 = admin_dev->get_polling_bef_9();
                    else if (polling_bef_9_def == true)
                        poll_bef_9 = polling_bef_9;

                    string device_name{v_poll_cmd[0]->svalue[0].in()};
                    create_poll_thread(device_name, true, poll_bef_9);
                    first_loop = true;

//
// Copy the list of commands to send to the threads in its structure
//

                    auto th_info = get_polling_thread_info_by_id(device_name);
                    for (size_t k = 0; k < nb_cmd; ++k)
                        th_info->v_poll_cmd.push_back(v_poll_cmd[k]);
                }
            }
        }

//
// Send command to polling thread one by one for each threads. In the following computation , I try to take into
// account the non real time aspect of our OS. I remove 15 mS from each sleeping time due to thread wake-up time
//

        unsigned long nb_thread = dev_poll_th_map.size();
        cout4 << "POLLING: " << nb_thread << " thread(s) needed for polling from a pool of "
              << get_polling_threads_pool_size() << endl;
        for_each(dev_poll_th_map.begin(), dev_poll_th_map.end(), [this, &admin_dev](const pair<string, PollingThreadInfoPtr> entry) {
            auto th_info = entry.second;
            auto nb_cmd = th_info->v_poll_cmd.size();

            int sleeping_time = th_info->smallest_upd / nb_cmd;
            int delta_time = 0;

            long delta_os = 5;
            if (delta_os < sleeping_time)
                sleeping_time = sleeping_time - delta_os;

            cout4 << "PollConfigureThread: smallest_upd = " << th_info->smallest_upd;
            cout4 << ", delta_time = " << sleeping_time;
            cout4 << ", nb_poll_objects = " << nb_cmd << endl;

            for (unsigned long cmd_loop = 0; cmd_loop < nb_cmd; ++cmd_loop) {
                try {
                    bool upd_db = false;
                    int upd = th_info->v_poll_cmd[cmd_loop]->lvalue[0];
                    if (upd < 0) {
                        th_info->v_poll_cmd[cmd_loop]->lvalue[0] = -upd;
                        upd_db = true;
                    }
                    admin_dev->add_obj_polling(th_info->v_poll_cmd[cmd_loop], upd_db, delta_time);
                }
                catch (Tango::DevFailed &e) {
                    bool throw_ex = true;

                    if (::strcmp(e.errors[0].reason.in(), API_AlreadyPolled) == 0) {
                        try {
                            admin_dev->upd_obj_polling_period(th_info->v_poll_cmd[cmd_loop], false);
                            throw_ex = false;
                        }
                        catch (Tango::DevFailed &) {}
                    }

                    if (throw_ex == true) {
                        TangoSys_OMemStream o;
                        o << "Error when configuring polling for device "
                          << th_info->v_poll_cmd[cmd_loop]->svalue[0].in();
                        if (::strcmp(th_info->v_poll_cmd[cmd_loop]->svalue[1].in(), "command") == 0)
                            o << ", cmd = ";
                        else
                            o << ", attr = ";
                        o << th_info->v_poll_cmd[cmd_loop]->svalue[2].in() << ends;
                        Except::re_throw_exception(e, (const char *) API_BadConfigurationProperty,
                                                   o.str(),
                                                   (const char *) "Util::polling_configure");
                    }
                }

                if (nb_cmd > 1) {
                    delta_time = delta_time + sleeping_time;
                }
            }

            //
            // Delete allocated memory
            //
            for (size_t i = 0; i < nb_cmd; i++)
                delete th_info->v_poll_cmd[i];
        });




//
// Send command to start polling for all forwarded attributes
//

        for (size_t i = 0; i < v_poll_cmd_fwd.size(); i++) {
            try {
                bool upd_db = false;
                int upd = v_poll_cmd_fwd[i]->lvalue[0];
                if (upd < 0) {
                    v_poll_cmd_fwd[i]->lvalue[0] = -upd;
                    upd_db = true;
                }
                admin_dev->add_obj_polling(v_poll_cmd_fwd[i], upd_db, 0);
            }
            catch (Tango::DevFailed &e) {
                bool throw_ex = true;

                if (::strcmp(e.errors[0].reason.in(), API_AlreadyPolled) == 0) {
                    try {
                        admin_dev->upd_obj_polling_period(v_poll_cmd_fwd[i], false);
                        throw_ex = false;
                    }
                    catch (Tango::DevFailed &) {}
                }

                if (throw_ex == true) {
                    TangoSys_OMemStream o;
                    o << "Error when configuring polling for device " << v_poll_cmd_fwd[i]->svalue[0].in();
                    o << ", attr = ";
                    o << v_poll_cmd_fwd[i]->svalue[2].in() << ends;
                    Except::re_throw_exception(e, (const char *) API_BadConfigurationProperty,
                                               o.str(),
                                               (const char *) "Util::polling_configure");
                }
            }
        }

//
// Delete allocated memory
//

        for (size_t l = 0; l < v_poll_cmd_fwd.size(); l++)
            delete v_poll_cmd_fwd[l];

//
// Now, start the real polling
//

        admin_dev->start_polling();

//
// If some polling related prop. (polling conf or dev conf) has to be updated in db, do it now
// Add a check to verify that it is possible to store polling pool conf in database.
// Property length cannot be longer than 256 characters
//

        if (poll_pool_conf.empty() == true) {
            build_first_pool_conf(poll_pool_conf);
            conf_needs_db_upd = true;
        } else {
            vector<string> tmp_pool_conf;
            build_first_pool_conf(tmp_pool_conf);
            if (tmp_pool_conf != poll_pool_conf) {
                poll_pool_conf = tmp_pool_conf;
                conf_needs_db_upd = true;
            }
        }

        if (((dev_db_upd.empty() == false) || (conf_needs_db_upd == true)) && (_UseDb == true))
            upd_polling_prop(dev_db_upd, admin_dev);

        cout4 << "Leaving polling_configure()" << endl;

    }

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::trigger_attr_polling()
//
// description :
//		Trigger the polling thread for polled attributes registered with a polling update period set as
//		"externally triggered" (0 mS)
//
// args :
// 		in :
//			- dev : The device pointer
//			- name : The polled attribute name
//
//-------------------------------------------------------------------------------------------------------------------

    void Util::trigger_attr_polling(Tango::DeviceImpl *dev, const string &name) {
        cout4 << "Sending trigger to polling thread" << endl;

//
// Check that the device is polled
//

        if (dev->is_polled() == false) {
            TangoSys_OMemStream o;
            o << "Device " << dev->get_name() << " is not polled" << ends;

            Except::throw_exception((const char *) API_DeviceNotPolled, o.str(),
                                    (const char *) "Util::trigger_attr_polling");
        }

//
// Find the wanted object in the list of device polled object
//

        string obj_name(name);
        transform(obj_name.begin(), obj_name.end(), obj_name.begin(), ::tolower);

        vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_ATTR, obj_name);

//
// Check that it is an externally triggered polling object. If it is not the case, throw exception
//

        long tmp_upd = (*ite)->get_upd();
        if (tmp_upd != 0) {
            TangoSys_OMemStream o;

            o << "Polling for attribute ";
            o << name;
            o << " (device " << dev->get_name() << ") ";
            o << " is not externally triggered.";
            o << ends;
            Except::throw_exception((const char *) API_NotSupported, o.str(),
                                    (const char *) "Util::trigger_attr_polling");
        }

//
// Find out which thread is in charge of the device.
//

        auto th_info = get_polling_thread_info_by_id(dev->get_name_lower());

//
// Send command to the polling thread but wait in case of previous cmd still not executed
//

        cout4 << "Sending trigger to polling thread" << endl;

        polling::TriggerPollingCommand attr_trigger{dev, move(obj_name), Tango::POLL_ATTR};

        th_info->poll_th->execute_cmd(move(attr_trigger));

        cout4 << "Trigger sent to polling thread" << endl;

//
// TODO Wait for thread to execute command
//

        cout4 << "Thread cmd normally executed" << endl;
    }


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::trigger_cmd_polling()
//
// description :
//		Trigger the polling thread for polled command registered with a polling update period set as
//		"externally triggered" (0 mS)
//
// args :
// 		in :
//			- dev : The device pointer
//			- name : The polled attribute name
//
//------------------------------------------------------------------------------------------------------------------

    void Util::trigger_cmd_polling(Tango::DeviceImpl *dev, const string &name) {
        cout4 << "Sending trigger to polling thread" << endl;

//
// Check that the device is polled
//

        if (dev->is_polled() == false) {
            TangoSys_OMemStream o;
            o << "Device " << dev->get_name() << " is not polled" << ends;

            Except::throw_exception((const char *) API_DeviceNotPolled, o.str(),
                                    (const char *) "Util::trigger_cmd_polling");
        }

//
// Find the wanted object in the list of device polled object
//

        string obj_name(name);
        transform(obj_name.begin(), obj_name.end(), obj_name.begin(), ::tolower);

        vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_CMD, obj_name);

//
// Check that it is an externally triggered polling object. If it is not the case, throw exception
//

        long tmp_upd = (*ite)->get_upd();
        if (tmp_upd != 0) {
            TangoSys_OMemStream o;

            o << "Polling for command ";
            o << name;
            o << " (device " << dev->get_name() << ") ";
            o << " is not externally triggered.";
            o << ends;
            Except::throw_exception((const char *) API_NotSupported, o.str(),
                                    (const char *) "Util::trigger_cmd_polling");
        }

//
// Find out which thread is in charge of the device.
//

        auto th_info = get_polling_thread_info_by_id(dev->get_name_lower());

//
// Send command to the polling thread but wait in case of previous cmd still not executed
//

        polling::TriggerPollingCommand cmd_trigger{dev, move(obj_name), Tango::POLL_CMD};

        th_info->poll_th->execute_cmd(move(cmd_trigger));

        cout4 << "Trigger sent to polling thread" << endl;

//
// TODO Wait for thread to execute command
//

        cout4 << "Thread cmd normally executed" << endl;
    }

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::clean_attr_polled_prop()
//
// description :
//		Clean in database the prop used to memorized which attribute are polled
//
//-----------------------------------------------------------------------------------------------------------------

    void Util::clean_attr_polled_prop() {
        if (Tango::Util::_UseDb == true) {
            DbData send_data;
            DbDatum db_info("polled_attr");

            for (unsigned int loop = 0; loop < polled_dyn_attr_names.size(); loop++) {
                vector<string>::iterator ite_attr = find(polled_att_list.begin(), polled_att_list.end(),
                                                         polled_dyn_attr_names[loop]);
                if (ite_attr != polled_att_list.end()) {
                    ite_attr = polled_att_list.erase(ite_attr);
                    if (ite_attr != polled_att_list.end())
                        polled_att_list.erase(ite_attr);
                } else {
                    TangoSys_OMemStream o;

                    o << "Polling properties for attribute " << polled_dyn_attr_names[loop] << " on device "
                      << dyn_att_dev_name;
                    o << " not found device in polled attribute list!" << ends;

                    Except::throw_exception((const char *) API_MethodArgument, o.str(),
                                            (const char *) "Util::clean_attr_polling_prop");
                }
            }

            db_info << polled_att_list;
            send_data.push_back(db_info);

            if (db_info.size() == 0)
                db->delete_device_property(dyn_att_dev_name, send_data);
            else
                db->put_device_property(dyn_att_dev_name, send_data);
        }
    }

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::clean_cmd_polled_prop()
//
// description :
//		Clean in database the prop used to memorized which commands are polled
//
//-----------------------------------------------------------------------------------------------------------------

    void Util::clean_cmd_polled_prop() {
        if (Tango::Util::_UseDb == true) {
            DbData send_data;
            DbDatum db_info("polled_cmd");

            for (unsigned int loop = 0; loop < polled_dyn_cmd_names.size(); loop++) {
                vector<string>::iterator ite_cmd = find(polled_cmd_list.begin(), polled_cmd_list.end(),
                                                        polled_dyn_cmd_names[loop]);
                if (ite_cmd != polled_cmd_list.end()) {
                    ite_cmd = polled_cmd_list.erase(ite_cmd);
                    if (ite_cmd != polled_cmd_list.end())
                        polled_cmd_list.erase(ite_cmd);
                } else {
                    TangoSys_OMemStream o;

                    o << "Polling properties for command " << polled_dyn_cmd_names[loop] << " on device "
                      << dyn_cmd_dev_name;
                    o << " not found device in polled command list!" << ends;

                    Except::throw_exception((const char *) API_MethodArgument, o.str(),
                                            (const char *) "Util::clean_cmd_polling_prop");
                }
            }

            db_info << polled_cmd_list;
            send_data.push_back(db_info);

            if (db_info.size() == 0)
                db->delete_device_property(dyn_cmd_dev_name, send_data);
            else
                db->put_device_property(dyn_cmd_dev_name, send_data);
        }
    }

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::create_poll_thread()
//
// description :
//		Create a polling thread for the device specified as parameter. If the thread already exist, do nothing
//		If the pool is full, associate this device to an already existing thread
//
// args :
// 		in :
// 			- dev_name : The device name
//		   	- startup : True if this method is called during DS startup. In such a case, some exception should not
//						be thrown
//          - polling_9 : The polling strict period flag
//		   	- smallest_upd : The smallest upd !
//
// return :
// 		This method returns -2 if the pool conf. does not need any update. It returns -1 if a new thread has been
//		created. It returns the index in the pool conf of the entry which has to be modified when the device is
//		associated to an already existing thread
//
//-------------------------------------------------------------------------------------------------------------------

    PollingThreadInfoPtr Util::create_poll_thread(string &device_name, bool startup, bool polling_9) {
//
// If there is already a polling thread for this device, simply returns
//

        if (dev_poll_th_map.count(device_name) != 0) {
            return dev_poll_th_map.at(device_name);
        }

        vector<string> asso_devs;
        int ind = get_th_polled_devs(device_name, asso_devs);

        if (!asso_devs.empty()) {
// If we find a thread for one of the associated device, no need to create a new one. Simply add the device entry
// in the map <dev_name,thread id>
//

            for (auto it = asso_devs.begin(); it != asso_devs.end(); ++it) {
                if (*it == device_name)
                    continue;

                auto poll_th_info_it = dev_poll_th_map.find(*it);
                if (poll_th_info_it != dev_poll_th_map.end()) {
                    dev_poll_th_map.emplace(device_name, poll_th_info_it->second);//TODO does override or adds
                    auto th_info = get_polling_thread_info_by_id(poll_th_info_it->first);
                    th_info->polled_devices.push_back(device_name);
                    th_info->nb_polled_objects++;
                    return th_info;
                }
            }
        }


//
// Check if the pool is full
//

        //TODO in V10 there is no pool for polling, each device simply has its own thread
        if (dev_poll_th_map.size() != poll_pool_size) {


//
// Create a new polling thread and start it
//

            auto pti_ptr = make_shared<PollingThreadInfo>();


            pti_ptr->poll_th = PollThread::create_instance_ptr("PollEngine[" + device_name + "]", polling_9);


            //TODO revise these code - poll_th_id is now thread::id()
            pti_ptr->polled_devices.push_back(device_name);
            pti_ptr->nb_polled_objects++;

            dev_poll_th_map.emplace(device_name, pti_ptr);
            return pti_ptr;
        } else {
            assert(false);//this is not supported in V10
        }

        return nullptr;
    }

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::stop_all_polling_threads()
//
// description :
//		Stop all polling threads used in the polling thread pool
//
//------------------------------------------------------------------------------------------------------------------

    void Util::stop_all_polling_threads() {
        for(auto entry : dev_poll_th_map){
            polling::StopPollingCommand stop_polling{};

            entry.second->poll_th->execute_cmd(move(stop_polling));
        }
    }


    vector<PollingThreadInfoPtr> Util::get_polling_threads_info() {
        std::vector<PollingThreadInfoPtr> result{};
        result.reserve(dev_poll_th_map.size());

        //TODO map helper method `values`
        std::transform(dev_poll_th_map.begin(), dev_poll_th_map.end(), std::back_inserter(result),
                       [this](const std::pair<std::string, PollingThreadInfoPtr> entry) { return entry.second; });

        return result;
    }




    PollingThreadInfoPtr Util::get_polling_thread_info_by_id(string device) {
        if (dev_poll_th_map.count(device) == 0) {
            return nullptr;
        }

        return dev_poll_th_map.at(device);
    }

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::check_poll_conf()
//
// description :
//		Check the pool configuration coherency. This means:
//		- All devices defined in the pool conf has to be created in the DS
//		- All objects to be polled has to be coherent
//		- The pool size must be coherent with the conf
//
// args :
//		in :
// 			- admin_dev : The DS admin device
//			- pool_size : The pool size (max number of thread)
//
//------------------------------------------------------------------------------------------------------------------

    void Util::check_pool_conf(DServer *admin_dev, unsigned long pool_size) {

//
// Simply returns if the conf. is empty
//

        if (poll_pool_conf.empty() == true)
            return;

        vector<string> mod_conf = poll_pool_conf;

//
// First, get the list of devices instanciated in this server and check if the polled devices are defined within
// the server
//

        Tango::DevVarStringArray *dev_list = admin_dev->query_device();
        unsigned int nb_dev = dev_list->length();
        unsigned int loop;
        int stop = 0;
        vector<string> splitted;

        vector<string>::iterator iter, iter_entry;
        for (iter = mod_conf.begin(); iter != mod_conf.end(); ++iter) {
            split_string(*iter, ',', splitted);
            stop++;

            string mod_entry;
            for (iter_entry = splitted.begin(); iter_entry != splitted.end(); ++iter_entry) {

//
// Check that this device is not already defined
//

                string::size_type po;
                if ((po = mod_entry.find(*iter_entry)) != string::npos)
                    continue;
                else if (is_dev_already_in_pool_conf(*iter_entry, mod_conf, stop - 1) == true)
                    continue;

//
// Check if the device is instanciated in this server
//

                string entry_lower(*iter_entry);
                transform(entry_lower.begin(), entry_lower.end(), entry_lower.begin(), ::tolower);

                for (loop = 0; loop < nb_dev; ++loop) {
                    string real_dev_name((*dev_list)[loop]);
                    string::size_type pos;
                    pos = real_dev_name.find(':');
                    pos = pos + 2;
                    real_dev_name = real_dev_name.substr(pos);

                    transform(real_dev_name.begin(), real_dev_name.end(), real_dev_name.begin(), ::tolower);

                    if (entry_lower == real_dev_name)
                        break;
                }

//
// The device is not defined in the DS, remove it from the conf. (Do not copy it in the new conf built by this method)
//

                if (loop == nb_dev) {
                    cout << "WARNING: Device " << *iter_entry << " is used in polling threads pool configuration";
                    cout << " but it is not defined in DS" << endl;
                    cout << "The pool configuration will be automatically updated" << endl;
                } else {

//
// Now, we know that the device exists in the server, but is it really polled ?
//

                    DeviceImpl *dev = get_device_by_name(*iter_entry);

                    vector<string> &poll_cmd_list = dev->get_polled_cmd();
                    vector<string> &poll_attr_list = dev->get_polled_attr();

                    if ((poll_cmd_list.empty() == true) && (poll_attr_list.empty() == true)) {
                        cout << "WARNING: Device " << *iter_entry << " is used in polling threads pool configuration";
                        cout << " but it does not have any cmd/attr polled" << endl;
                        cout << "The pool configuration will be automatically updated" << endl;
                    } else {
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

            if (mod_entry.empty() == true) {
                iter = mod_conf.erase(iter);
                if (iter == mod_conf.end())
                    break;
                else {
                    if (iter == mod_conf.begin())
                        --stop;
                    --iter;
                }
            } else if (mod_entry != *iter)
                *iter = mod_entry;
        }
        delete dev_list;

//
// Now, check that the thread defined by the conf. is <= than the pool size
//

        if (mod_conf.size() > pool_size) {
            cout << "WARNING: More threads defined in the polling threads pool configuration";
            cout << " than in its size (" << mod_conf.size() << " > " << pool_size << ")" << endl;
            cout << "The pool configuration will be automatically updated" << endl;

//
// If we have more threads in the conf than in the pool, distribute the extra thread devices to the still existing
// threads
//

            long nb_extra_th = mod_conf.size() - pool_size;
            for (long i = 0; i < nb_extra_th; i++) {
                vector<string> polled_dev_th;
                get_th_polled_devs(i + pool_size, polled_dev_th);
                unsigned long loop = 0;
                vector<string>::iterator it;
                for (it = polled_dev_th.begin(); it != polled_dev_th.end(); ++it) {
                    mod_conf[loop] = mod_conf[loop] + ',' + *it;
                    loop++;
                    if (loop == pool_size)
                        loop = 0;
                }
            }

            vector<string>::iterator it = mod_conf.begin();
            it = it + pool_size;
            mod_conf.erase(it, mod_conf.end());
        }

//
// If necessary, update the conf in db
//

        if (mod_conf != poll_pool_conf) {
            conf_needs_db_upd = true;
            poll_pool_conf = mod_conf;
        }

    }

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::check_dev_poll()
//
// description :
//		Check if all the cmd/attr to be polled for a device are really supported by the device
//
// args :
//		in :
// 			- poll_cmd_list : The polled command(s)
//			- poll_attr_list : The polled attr(s)
//			- dev : The device pointer
//
// return :
// 		This method returns 0 if no polling conf has been modified. Otherwise, it returns -1 if only the command list
//		has been modified, -2 if the only the attribute list has been modified and -3 if both of them have been
// 		modified
//
//------------------------------------------------------------------------------------------------------------------

    int Util::check_dev_poll(vector<string> &poll_cmd_list, vector<string> &poll_attr_list, DeviceImpl *dev) {
        int ret = 0;

//
// First, get device commands list
//

        vector<Command *> &cmd_list = dev->get_device_class()->get_command_list();

//
// Check polled cmds
//

        vector<string>::iterator iter;

        for (iter = poll_cmd_list.begin(); iter != poll_cmd_list.end(); iter = iter + 2) {
            string polled_cmd = *iter;
            transform(polled_cmd.begin(), polled_cmd.end(), polled_cmd.begin(), ::tolower);

            vector<Command *>::iterator i_cmd;

            for (i_cmd = cmd_list.begin(); i_cmd < cmd_list.end(); ++i_cmd) {
                if ((*i_cmd)->get_lower_name() == polled_cmd)
                    break;
            }

            if (i_cmd == cmd_list.end()) {
                cout << "WARNING: Device " << dev->get_name() << " is configured to be polled with";
                cout << " a command which does not exist anymore" << endl;
                cout << "The device polling configuration will be automatically updated" << endl;

                ret = -1;
                poll_cmd_list.erase(iter, iter + 2);
                iter = iter - 2;
            }
        }

//
// Now, get device attribute list
//

        vector<Attribute *> &att_list = dev->get_device_attr()->get_attribute_list();

//
// Check polled attributes
//

        for (iter = poll_attr_list.begin(); iter != poll_attr_list.end(); iter = iter + 2) {
            string polled_attr = *iter;
            transform(polled_attr.begin(), polled_attr.end(), polled_attr.begin(), ::tolower);

            vector<Attribute *>::iterator i_attr;

            for (i_attr = att_list.begin(); i_attr < att_list.end(); ++i_attr) {
                if ((*i_attr)->get_name_lower() == polled_attr)
                    break;
            }

            if (i_attr == att_list.end()) {
                cout << "WARNING: Device " << dev->get_name() << " is configured to be polled with";
                cout << " an attribute which does not exist anymore (" << polled_attr << ")" << endl;
                cout << "The device polling configuration will be automatically updated" << endl;

                if (ret == -1)
                    ret = -3;
                else
                    ret = -2;
                poll_attr_list.erase(iter, iter + 2);
                iter = iter - 2;
            }
        }

        return ret;
    }

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::split_string()
//
// description :
//		Split a string according to a delimiter
//
// args :
//		in :
// 			- the_str : ref to the string to be plitted
//			- delim : The delimiter character
//			- splited_str : The splitted string returned in a vector of individual elt
//
//-------------------------------------------------------------------------------------------------------------------

    void Util::split_string(string &the_str, char delim, vector<string> &splitted_str) {
        string::size_type pos, start;
        splitted_str.clear();

        start = 0;
        while ((pos = the_str.find(delim, start)) != string::npos) {
            splitted_str.push_back(the_str.substr(start, pos - start));
            start = pos + 1;
        }

//
// The last element
//

        splitted_str.push_back(the_str.substr(start));
    }

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::upd_polling_prop()
//
// description :
//		Update polling related properties in Db
//
// args :
//		in :
// 			- upd_devs : ref to a vector with one elt for each dev with polling prop. to be updated
//			- admin_dev : The DS admin device
//
//------------------------------------------------------------------------------------------------------------------

    void Util::upd_polling_prop(vector<DevDbUpd> &upd_devs, DServer *admin_dev) {
        vector<DeviceClass *> &tmp_cl_list = admin_dev->get_class_list();
        unsigned long i;

//
// A loop on each device with prop. to be updated
//

        for (i = 0; i < upd_devs.size(); i++) {
            vector<DeviceImpl *> &dev_list = tmp_cl_list[upd_devs[i].class_ind]->get_device_list();

            vector<string> &poll_cmd_list = dev_list[upd_devs[i].dev_ind]->get_polled_cmd();
            vector<string> &poll_attr_list = dev_list[upd_devs[i].dev_ind]->get_polled_attr();

            DbData del_prop;
            DbData upd_prop;

//
// Is it necessary to update or to delete the property?
//

            if ((upd_devs[i].mod_prop == -1) || (upd_devs[i].mod_prop == -3)) {
                if (poll_cmd_list.empty() == true)
                    del_prop.push_back(DbDatum("polled_cmd"));
                else {
                    upd_prop.push_back(DbDatum("polled_cmd"));
                    upd_prop[0] << poll_cmd_list;
                }
            }

            if ((upd_devs[i].mod_prop == -2) || (upd_devs[i].mod_prop == -3)) {
                if (poll_attr_list.empty() == true)
                    del_prop.push_back(DbDatum("polled_attr"));
                else {
                    upd_prop.push_back(DbDatum("polled_attr"));
                    upd_prop[0] << poll_attr_list;
                }
            }

//
// Update the database
//

            bool retry = true;
            long db_retries = DB_START_PHASE_RETRIES;

            while (retry == true) {
                try {
                    if (del_prop.empty() == false)
                        get_database()->delete_device_property(dev_list[upd_devs[i].dev_ind]->get_name(), del_prop);
                    if (upd_prop.empty() == false)
                        get_database()->put_device_property(dev_list[upd_devs[i].dev_ind]->get_name(), upd_prop);
                    retry = false;
                }
                catch (Tango::CommunicationFailed &) {
                    if (is_svr_starting() == true) {
                        db_retries--;
                        if (db_retries == 0)
                            throw;
                    } else
                        throw;
                }
            }

//
// If now the device does not have any objects polled (no cmd, no attr), it must be removed from the pool conf.
//

            if ((poll_attr_list.empty() == true) && (poll_cmd_list.empty() == true)) {
                vector<string>::iterator iter;

                for (iter = poll_pool_conf.begin(); iter != poll_pool_conf.end(); ++iter) {
                    string tmp = *iter;
                    string &d_name = dev_list[upd_devs[i].dev_ind]->get_name();
                    if (tmp.find(d_name) != string::npos) {
                        string::size_type pos;
                        if ((pos = tmp.find(',')) == string::npos) {
                            iter = poll_pool_conf.erase(iter);
                        } else {
                            tmp.erase(pos, d_name.size());
                        }
                        conf_needs_db_upd = true;
                        break;
                    }
                }
            }
        }

//
// Update the polling pool conf if needed
//

        if (conf_needs_db_upd == true) {
            DbData upd_conf;
            upd_conf.push_back(DbDatum("polling_threads_pool_conf"));

            bool retry = true;
            long db_retries = DB_START_PHASE_RETRIES;

            while (retry == true) {
                try {
                    if (poll_pool_conf.empty() == true) {
                        get_database()->delete_device_property(admin_dev->get_name(), upd_conf);
                    } else {

//
// The max device property size in db is limited to 255.
// If we have only one thread, it is easy to catch this threshold. In case this threshold is reached, split entry in
// several sub-entries using the \ characters at the end of each sub-entries
//

                        vector<string>::iterator iter;
                        vector<string> new_vect;

                        for (iter = poll_pool_conf.begin(); iter != poll_pool_conf.end(); ++iter) {
                            string v_entry = *iter;
                            unsigned int length = v_entry.size();
                            int nb_lines = (length / MaxDevPropLength) + 1;
                            if (nb_lines > 1) {
                                string::size_type start;
                                start = 0;

                                for (int i = 0; i < nb_lines; i++) {
                                    string sub = v_entry.substr(start, MaxDevPropLength);
                                    if (i < (nb_lines - 1))
                                        sub = sub + '\\';
                                    start = start + MaxDevPropLength;
                                    new_vect.push_back(sub);
                                }
                            } else
                                new_vect.push_back(v_entry);
                        }

                        upd_conf[0] << new_vect;
                        get_database()->put_device_property(admin_dev->get_name(), upd_conf);
                    }
                    retry = false;
                }
                catch (Tango::CommunicationFailed &) {
                    if (is_svr_starting() == true) {
                        db_retries--;
                        if (db_retries == 0)
                            throw;
                    } else
                        throw;
                }
            }
        }
    }

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::get_th_polled_devs()
//
// description :
//		Get from the polling pool configuration which devices should be polled by the thread which is in charge of
//		device dev
//
// args :
//		in :
// 			- dev : The device name
//			- th_polled_devs : List of devices also polled by the thread in charge of dev
//
//------------------------------------------------------------------------------------------------------------------

    int Util::get_th_polled_devs(string &dev, vector<string> &th_polled_devs) {
        th_polled_devs.clear();

        vector<string>::iterator iter;
        unsigned int dev_nb_char = dev.size();
        unsigned int pool_dev_nb_char;

        for (iter = poll_pool_conf.begin(); iter != poll_pool_conf.end(); ++iter) {
            string tmp = *iter;
            transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
            string::size_type pos, end_pos;
            pos = tmp.find(dev);
            if (pos != string::npos) {
                end_pos = tmp.find(',', pos);
                if (end_pos != string::npos)
                    pool_dev_nb_char = end_pos - pos;
                else
                    pool_dev_nb_char = tmp.size() - pos;
                if (dev_nb_char == pool_dev_nb_char) {
                    split_string(tmp, ',', th_polled_devs);
                    break;
                }
            }
        }

        return iter - poll_pool_conf.begin();
    }

    void Util::get_th_polled_devs(long i, vector<string> &th_polled_devs) {
        th_polled_devs.clear();

        string tmp = poll_pool_conf[i];
        split_string(tmp, ',', th_polled_devs);
    }

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::build_first_pool_conf()
//
// description :
//		Create a polling tread pool configuration when this data is not defined
//
// args :
//		out :
// 			- pool_conf : The polling threads pool configuration
//
//------------------------------------------------------------------------------------------------------------------

    void Util::build_first_pool_conf(vector<string> &pool_conf) {
        transform(dev_poll_th_map.begin(), dev_poll_th_map.end(), back_inserter(pool_conf),
                  [](const pair<string, PollingThreadInfoPtr> entry){ return sequence_to_string(entry.second->polled_devices);}
        );
    }

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::is_dev_already_in_pool_conf()
//
// description :
//		Check if a device is already defined in the pool conf
//
// args :
//		in :
// 			- dev_name : The device name
//			- pool : The polling threads pool configuration
//          - stop : The index in poll conf where the search should be stopped
//
// return :
// 		This method returns true if the device is already defined in the pool. Otherwise, returns false (amazing no!)
//
//--------------------------------------------------------------------------------------------------------------------

    bool Util::is_dev_already_in_pool_conf(string &dev_name, vector<string> &pool, int stop) {
        vector<string>::iterator iter;

        for (iter = pool.begin(); iter != pool.begin() + stop; ++iter) {
            vector<string> dev_list;
            vector<string>::iterator it;

            split_string(*iter, ',', dev_list);
            for (it = dev_list.begin(); it != dev_list.end(); ++it) {
                if (*it == dev_name)
                    return true;
            }
        }

        return false;
    }

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::get_dev_entry_in_pool_conf()
//
// description :
//		Get in which entry a device is used in the pool conf
//
// args :
//		in :
// 			- dev_name : The device name
//
// return :
// 		This method returns the index in the pool conf if the device has been found. Otherwise, it returns -1
//
//---------------------------------------------------------------------------------------------------------------------

    int Util::get_dev_entry_in_pool_conf(string &dev_name) {
        vector<string>::iterator iter;
        unsigned int dev_nb_char = dev_name.size();
        unsigned int pool_dev_nb_char;

        for (iter = poll_pool_conf.begin(); iter != poll_pool_conf.end(); ++iter) {
            string tmp = *iter;
            string::size_type pos, end_pos;
            pos = tmp.find(dev_name);
            if (pos != string::npos) {
                end_pos = tmp.find(',', pos);
                if (end_pos != string::npos)
                    pool_dev_nb_char = end_pos - pos;
                else
                    pool_dev_nb_char = tmp.size() - pos;
                if (dev_nb_char == pool_dev_nb_char) {
                    break;
                }
            }
        }

        if (iter != poll_pool_conf.end())
            return iter - poll_pool_conf.begin();
        else
            return -1;
    }

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::remove_dev_from_polling_map()
//
// description :
//		Remove the device from the polled device map
//
// args :
//		in :
// 			- dev_name : The device name
//
//-------------------------------------------------------------------------------------------------------------------

    void Util::remove_dev_from_polling_map(string &dev_name) {
        auto number_of_deleted = dev_poll_th_map.erase(dev_name);
        cout5 << "Removed " << number_of_deleted << " entries for device[" << dev_name << "] from polled devices map..."
              << endl;
    }

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::remove_polling_thread_info_by_id()
//
// description :
//		Remove all the polling thread info from the vector of polling thread info for a specific thread
//
// args :
//		in :
//			- th_id : The polling thread id
//
//-------------------------------------------------------------------------------------------------------------------

} // End of Tango namespace
