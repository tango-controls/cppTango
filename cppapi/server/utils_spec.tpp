//===================================================================================================================
//
// file :               utils_spec.tpp
//
// description :        C++ source code for the Util::fill_cmd_polling_buffer method template specialization
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2011,2012
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation, either version 3 of the License, or
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
//==================================================================================================================

#ifndef _UTILS_SPEC_TPP
#define _UTILS_SPEC_TPP

namespace Tango
{

//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::fill_cmd_polling_buffer
//
// description :
//		Fill attribute polling buffer with your own data
//		These are template specialization for data type
//				DevBoolean
//				DevUChar
//		This is required because the insertion of these data type in a CORBA Any object requires specific code
//
// args :
//		in :
// 			- dev : The device
//			- cmd_name : The command name
//			- data : The command data to be stored in the polling buffer
//
//----------------------------------------------------------------------------------------------------------------

template <>
inline void Util::fill_cmd_polling_buffer(DeviceImpl *dev,string &cmd_name,CmdHistoryStack<DevBoolean>  &data)
{

//
// Check that the device is polled
//

    if (dev->is_polled() == false)
    {
        TangoSys_OMemStream o;
        o << "Device " << dev->get_name() << " is not polled" << ends;

        Except::throw_exception((const char *)API_DeviceNotPolled,o.str(),
                    (const char *)"Util::fill_cmd_polling_buffer");
    }

//
// Command name in lower case letters and check that it is marked as polled
//

    string obj_name(cmd_name);
    transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);

    dev->get_polled_obj_by_type_name(Tango::POLL_CMD,obj_name);

//
// Check that history is not larger than polling buffer
//

    size_t nb_elt = data.length();
    long nb_poll = dev->get_cmd_poll_ring_depth(cmd_name);

    if (nb_elt > (size_t)nb_poll)
    {
        TangoSys_OMemStream o;
        o << "The polling buffer depth for command " << cmd_name;
        o << " for device " << dev->get_name();
        o << " is only " << nb_poll;
        o << " which is less than " << nb_elt << "!" << ends;

        Except::throw_exception((const char *)API_DeviceNotPolled,o.str(),
                    (const char *)"Util::fill_cmd_polling_buffer");
    }

//
// Take the device monitor before the loop
// In case of large element number, it is time cousuming to take/release
// the monitor in the loop
//

    dev->get_poll_monitor().get_monitor();

//
// A loop on each record
//

    size_t i;
    Tango::DevFailed *save_except;
    bool cmd_failed;
    CORBA::Any *any_ptr;

    struct timeval zero,when;
    zero.tv_sec = zero.tv_usec = 0;

    for (i = 0;i < nb_elt;i++)
    {
        save_except = NULL;
        cmd_failed = false;

        if ((data.get_data())[i].err.length() != 0)
        {
            cmd_failed = true;
            try
            {
                save_except = new Tango::DevFailed((data.get_data())[i].err);
            }
            catch (bad_alloc)
            {
                dev->get_poll_monitor().rel_monitor();
                Except::throw_exception((const char *)API_MemoryAllocation,
                                (const char *)"Can't allocate memory in server",
                                (const char *)"Util::fill_cmd_polling_buffer");
            }
        }
        else
        {

//
// Allocate memory for the Any object
//

            try
            {
                any_ptr = new CORBA::Any();
            }
            catch (bad_alloc)
            {
                dev->get_poll_monitor().rel_monitor();
                Except::throw_exception((const char *)API_MemoryAllocation,
                                (const char *)"Can't allocate memory in server",
                                (const char *)"Util::fill_cmd_polling_buffer");
            }

//
// Set command value in Any object
//

            DevBoolean *tmp_ptr = (data.get_data())[i].ptr;
			CORBA::Any::from_boolean tmp(*tmp_ptr);
			(*any_ptr) <<= tmp;

			if ((data.get_data())[i].release == true)
				delete tmp_ptr;
        }

//
// Fill one slot of polling buffer
//

        try
        {
            vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_CMD,obj_name);
            when.tv_sec = (data.get_data())[i].t_val.tv_sec - DELTA_T;
            when.tv_usec = (data.get_data())[i].t_val.tv_usec;
            if (cmd_failed == false)
                (*ite)->insert_data(any_ptr,when,zero);
            else
                (*ite)->insert_except(save_except,when,zero);
        }
        catch (Tango::DevFailed &)
        {
            if (cmd_failed == false)
                delete any_ptr;
            else
                delete save_except;
        }

    }

    dev->get_poll_monitor().rel_monitor();
}


template <>
inline void Util::fill_cmd_polling_buffer(DeviceImpl *dev,string &cmd_name,CmdHistoryStack<DevUChar>  &data)
{

//
// Check that the device is polled
//

    if (dev->is_polled() == false)
    {
        TangoSys_OMemStream o;
        o << "Device " << dev->get_name() << " is not polled" << ends;

        Except::throw_exception((const char *)API_DeviceNotPolled,o.str(),
                    (const char *)"Util::fill_cmd_polling_buffer");
    }

//
// Command name in lower case letters and check that it is marked as polled
//

    string obj_name(cmd_name);
    transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);

    dev->get_polled_obj_by_type_name(Tango::POLL_CMD,obj_name);

//
// Check that history is not larger than polling buffer
//

    size_t nb_elt = data.length();
    long nb_poll = dev->get_cmd_poll_ring_depth(cmd_name);

    if (nb_elt > (size_t)nb_poll)
    {
        TangoSys_OMemStream o;
        o << "The polling buffer depth for command " << cmd_name;
        o << " for device " << dev->get_name();
        o << " is only " << nb_poll;
        o << " which is less than " << nb_elt << "!" << ends;

        Except::throw_exception((const char *)API_DeviceNotPolled,o.str(),
                    (const char *)"Util::fill_cmd_polling_buffer");
    }

//
// Take the device monitor before the loop
// In case of large element number, it is time cousuming to take/release
// the monitor in the loop
//

    dev->get_poll_monitor().get_monitor();

//
// A loop on each record
//

    size_t i;
    Tango::DevFailed *save_except;
    bool cmd_failed;
    CORBA::Any *any_ptr;

    struct timeval zero,when;
    zero.tv_sec = zero.tv_usec = 0;

    for (i = 0;i < nb_elt;i++)
    {
        save_except = NULL;
        cmd_failed = false;

        if ((data.get_data())[i].err.length() != 0)
        {
            cmd_failed = true;
            try
            {
                save_except = new Tango::DevFailed((data.get_data())[i].err);
            }
            catch (bad_alloc)
            {
                dev->get_poll_monitor().rel_monitor();
                Except::throw_exception((const char *)API_MemoryAllocation,
                                (const char *)"Can't allocate memory in server",
                                (const char *)"Util::fill_cmd_polling_buffer");
            }
        }
        else
        {

//
// Allocate memory for the Any object
//

            try
            {
                any_ptr = new CORBA::Any();
            }
            catch (bad_alloc)
            {
                dev->get_poll_monitor().rel_monitor();
                Except::throw_exception((const char *)API_MemoryAllocation,
                                (const char *)"Can't allocate memory in server",
                                (const char *)"Util::fill_cmd_polling_buffer");
            }

//
// Set command value in Any object
//

            DevUChar *tmp_ptr = (data.get_data())[i].ptr;
			CORBA::Any::from_octet tmp(*tmp_ptr);
			(*any_ptr) <<= tmp;

			if ((data.get_data())[i].release == true)
				delete tmp_ptr;
        }

//
// Fill one slot of polling buffer
//

        try
        {
            vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_CMD,obj_name);
            when.tv_sec = (data.get_data())[i].t_val.tv_sec - DELTA_T;
            when.tv_usec = (data.get_data())[i].t_val.tv_usec;
            if (cmd_failed == false)
                (*ite)->insert_data(any_ptr,when,zero);
            else
                (*ite)->insert_except(save_except,when,zero);
        }
        catch (Tango::DevFailed &)
        {
            if (cmd_failed == false)
                delete any_ptr;
            else
                delete save_except;
        }

    }

    dev->get_poll_monitor().rel_monitor();
}

} // End of Tango namespace

#endif /* UTILS_SPEC_TPP */
