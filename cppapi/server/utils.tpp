//===================================================================================================================
//
// file :               utils.tpp
//
// description :        C++ source code for the Attribute class template methods
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2011,2012,2013,2014,2015
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
//
//==================================================================================================================

#ifndef _UTILS_TPP
#define _UTILS_TPP

#include <poll_clock.h>

namespace Tango
{

//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::fill_attr_polling_buffer
//
// description :
//		Fill attribute polling buffer with your own data
//
// args :
//		in :
// 			- dev : The device
//			- att_name : The attribute name
//			- data : The attribute data to be stored in the polling buffer
//
//----------------------------------------------------------------------------------------------------------------

template <typename T>
void Util::fill_attr_polling_buffer(DeviceImpl *dev,std::string &att_name,AttrHistoryStack<T>  &data)
{
//
// Check that the device is polled
//

    if (dev->is_polled() == false)
    {
        TangoSys_OMemStream o;
        o << "Device " << dev->get_name() << " is not polled" << std::ends;

        Except::throw_exception(API_DeviceNotPolled,o.str(),"Util::fill_attr_polling_buffer");
    }

//
// Attribute name in lower case letters and check that it is marked as polled
//

    std::string obj_name(att_name);
    std::transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);

    dev->get_polled_obj_by_type_name(Tango::POLL_ATTR,obj_name);

//
// Get a reference on the Attribute object
//

    Tango::Attribute &att = dev->get_device_attr()->get_attr_by_name(att_name.c_str());
    Tango::WAttribute *w_att_ptr = NULL;
    Tango::AttrWriteType w_type = att.get_writable();
    if (w_type == Tango::READ_WRITE)
        w_att_ptr = &(dev->get_device_attr()->get_w_attr_by_name(att_name.c_str()));

//
// Check that it is not a WRITE only attribute
//

    if (w_type == Tango::WRITE)
    {
        TangoSys_OMemStream o;
        o << "Attribute " << att_name;
        o << " of device " << dev->get_name() << " is WRITE only" << std::ends;

        Except::throw_exception(API_DeviceNotPolled,o.str(),"Util::fill_attr_polling_buffer");
    }

//
// If the attribute is not READ_WRITE, the ptr to written part should always be NULL
//

    size_t nb_elt = data.length();
    if (w_type != READ_WRITE)
    {
        for (size_t i = 0;i < nb_elt;i++)
        {
            if ((data.get_data())[i].wr_ptr != NULL)
            {
                TangoSys_OMemStream o;
                o << "The attribute " << att_name;
                o << " for device " << dev->get_name();
                o << " is not a READ_WRITE attribute. You can't set the attribute written part.";
                o << "It is defined for record number " << i + 1 << std::ends;

                Except::throw_exception(API_NotSupportedFeature,o.str(),"Util::fill_attr_polling_buffer");
            }
        }
    }

//
// For device implementing IDL 3, DevEncoded data type is not supported
//

    long idl_vers = dev->get_dev_idl_version();
    if ((att.get_data_type() == DEV_ENCODED) && (idl_vers == 3))
    {
        TangoSys_OMemStream o;
        o << "The attribute " << att_name;
        o << " for device " << dev->get_name();
        o << " is of type DEV_ENCODED. Your device supports only IDL V3.";
        o << " DEV_ENCODED data type is supported starting with IDL V4" << std::ends;

        Except::throw_exception(API_NotSupportedFeature,o.str(),"Util::fill_attr_polling_buffer");
    }

//
// DevEncoded data type is not supported for spectrum or image attribute
// Paranoid code? This case should never happens!
//

    if ((att.get_data_type() == DEV_ENCODED) &&
        (att.get_data_format() != Tango::SCALAR) &&
        (w_type == Tango::READ_WRITE))
    {
        for (size_t i = 0;i < nb_elt;i++)
        {
            if ((data.get_data())[i].wr_ptr != NULL)
            {
                TangoSys_OMemStream o;
                o << "The attribute " << att_name;
                o << " for device " << dev->get_name();
                o << " is of type DEV_ENCODED. Only Scalar attribute are supported for DEV_ENCODED";
                o << "It is defined for record number " << i + 1 << std::ends;

                Except::throw_exception(API_NotSupportedFeature,o.str(),"Util::fill_attr_polling_buffer");
            }
        }
    }

//
// Check that the device IDL is at least 3
//

    if (idl_vers <= 2)
    {
        TangoSys_OMemStream o;
        o << "The device " << dev->get_name() << " is too old to support this feature. ";
        o << "Please update your device to IDL 3 or more" << std::ends;

        Except::throw_exception(API_NotSupportedFeature,o.str(),"Util::fill_attr_polling_buffer");
    }

//
// Check that history is not larger than polling buffer
//

    unsigned long nb_poll = dev->get_attr_poll_ring_depth(att_name);

    if (nb_elt > nb_poll)
    {
        TangoSys_OMemStream o;
        o << "The polling buffer depth for attribute " << att_name;
        o << " for device " << dev->get_name();
        o << " is only " << nb_poll;
        o << " which is less than " << nb_elt << "!" << std::ends;

        Except::throw_exception(API_DeviceNotPolled,o.str(),"Util::fill_attr_polling_buffer");
    }

//
// A loop on each record
//

    unsigned long i;
    Tango::DevFailed *save_except;
    AttributeIdlData aid;
    bool attr_failed;

//
// Take the device monitor before the loop
// In case of large element number, it is time cousuming to take/release the monitor in the loop
//

    dev->get_poll_monitor().get_monitor();

//
// The loop for each element
//

    for (i = 0;i < nb_elt;i++)
    {
        save_except = NULL;
        attr_failed = false;

        if ((data.get_data())[i].err.length() != 0)
        {
            attr_failed = true;
            try
            {
                save_except = new Tango::DevFailed((data.get_data())[i].err);
            }
            catch (std::bad_alloc &)
            {
                dev->get_poll_monitor().rel_monitor();
                Except::throw_exception(API_MemoryAllocation,
                                "Can't allocate memory in server",
                                "Util::fill_attr_polling_buffer");
            }
        }
        else
        {

//
// Allocate memory for the AttributeValueList sequence
//

            try
            {
            	if (idl_vers >= 5)
				{
					aid.data_5 = new Tango::AttributeValueList_5(1);
					aid.data_5->length(1);
                    (*aid.data_5)[0].value.union_no_data(true);
				}
                else if (idl_vers == 4)
                {
                    aid.data_4 = new Tango::AttributeValueList_4(1);
                    aid.data_4->length(1);
                    (*aid.data_4)[0].value.union_no_data(true);
                }
                else
                {
                    aid.data_3 = new Tango::AttributeValueList_3(1);
                    aid.data_3->length(1);
                }
            }
            catch (std::bad_alloc &)
            {
                dev->get_poll_monitor().rel_monitor();
                Except::throw_exception(API_MemoryAllocation,
                                "Can't allocate memory in server","Util::fill_attr_polling_buffer");
            }

//
// Init name,date and quality factor
//

            Tango::AttrQuality qu = (data.get_data())[i].qual;
            if (idl_vers >= 5)
            {
                (*aid.data_5)[0].time.tv_sec = (data.get_data())[i].t_val.tv_sec;
                (*aid.data_5)[0].time.tv_usec = (data.get_data())[i].t_val.tv_usec;
                (*aid.data_5)[0].time.tv_nsec = 0;

                (*aid.data_5)[0].quality = qu;
                (*aid.data_5)[0].name = Tango::string_dup(att_name.c_str());

                (*aid.data_5)[0].w_dim.dim_x = 0;
                (*aid.data_5)[0].w_dim.dim_y = 0;
                (*aid.data_5)[0].r_dim.dim_x = 0;
                (*aid.data_5)[0].r_dim.dim_y = 0;

                (*aid.data_5)[0].data_format = att.get_data_format();
                (*aid.data_5)[0].data_type = att.get_data_type();
            }
            else if (idl_vers == 4)
            {
                (*aid.data_4)[0].time.tv_sec = (data.get_data())[i].t_val.tv_sec;
                (*aid.data_4)[0].time.tv_usec = (data.get_data())[i].t_val.tv_usec;
                (*aid.data_4)[0].time.tv_nsec = 0;

                (*aid.data_4)[0].quality = qu;
                (*aid.data_4)[0].name = Tango::string_dup(att_name.c_str());

                (*aid.data_4)[0].w_dim.dim_x = 0;
                (*aid.data_4)[0].w_dim.dim_y = 0;
                (*aid.data_4)[0].r_dim.dim_x = 0;
                (*aid.data_4)[0].r_dim.dim_y = 0;

                (*aid.data_4)[0].data_format = att.get_data_format();
            }
            else
            {
                (*aid.data_3)[0].time.tv_sec = (data.get_data())[i].t_val.tv_sec;
                (*aid.data_3)[0].time.tv_usec = (data.get_data())[i].t_val.tv_usec;
                (*aid.data_3)[0].time.tv_nsec = 0;

                (*aid.data_3)[0].quality = qu;
                (*aid.data_3)[0].name = Tango::string_dup(att_name.c_str());

                (*aid.data_3)[0].w_dim.dim_x = 0;
                (*aid.data_3)[0].w_dim.dim_y = 0;
                (*aid.data_3)[0].r_dim.dim_x = 0;
                (*aid.data_3)[0].r_dim.dim_y = 0;
            }
            if ((qu == Tango::ATTR_VALID) ||
                (qu == Tango::ATTR_ALARM) ||
                (qu == Tango::ATTR_WARNING) ||
                (qu == Tango::ATTR_CHANGING))
            {

//
// Set Attribute object value
//

                att.set_value((T *)(data.get_data())[i].ptr,
                          (data.get_data())[i].x,
                          (data.get_data())[i].y,
                          (data.get_data())[i].release);
                att.set_date((data.get_data())[i].t_val);
                att.set_quality(qu,false);

//
// Init remaining fields in AttributeValueList
//

                if (w_type == Tango::READ_WITH_WRITE)
                    dev->get_device_attr()->add_write_value(att);
                else if (w_type == Tango::READ_WRITE)
                {
                    if ((data.get_data())[i].wr_ptr != NULL)
                    {
                        w_att_ptr->set_write_value((T *)(data.get_data())[i].wr_ptr,
                                                (data.get_data())[i].wr_x,
                                                (data.get_data())[i].wr_y);
                        dev->get_device_attr()->add_write_value(att);

                        if ((data.get_data())[i].release == true)
                        {
                            if (att.get_data_format() == Tango::SCALAR)
                                delete (data.get_data())[i].wr_ptr;
                            else
                                delete [] (data.get_data())[i].wr_ptr;
                        }
                    }
                    else
                    {
                        dev->get_device_attr()->add_write_value(att);
                    }
                }

//
// Insert data into the AttributeValue object
//

                dev->data_into_net_object(att,aid,0,w_type,true);

//
// Init remaining fields
//

				if (idl_vers >= 5)
				{
                    (*aid.data_5)[0].r_dim.dim_x = (data.get_data())[i].x;
                    (*aid.data_5)[0].r_dim.dim_y = (data.get_data())[i].y;

                    if ((w_type == Tango::READ_WRITE) || (w_type == Tango::READ_WITH_WRITE))
                    {
                        WAttribute &assoc_att = dev->get_device_attr()->get_w_attr_by_ind(att.get_assoc_ind());
                        (*aid.data_5)[0].w_dim.dim_x = assoc_att.get_w_dim_x();
                        (*aid.data_5)[0].w_dim.dim_y = assoc_att.get_w_dim_y();
                    }
				}
                else if (idl_vers == 4)
                {
                    (*aid.data_4)[0].r_dim.dim_x = (data.get_data())[i].x;
                    (*aid.data_4)[0].r_dim.dim_y = (data.get_data())[i].y;

                    if ((w_type == Tango::READ_WRITE) || (w_type == Tango::READ_WITH_WRITE))
                    {
                        WAttribute &assoc_att = dev->get_device_attr()->get_w_attr_by_ind(att.get_assoc_ind());
                        (*aid.data_4)[0].w_dim.dim_x = assoc_att.get_w_dim_x();
                        (*aid.data_4)[0].w_dim.dim_y = assoc_att.get_w_dim_y();
                    }
                }
                else
                {
                    (*aid.data_3)[0].r_dim.dim_x = (data.get_data())[i].x;
                    (*aid.data_3)[0].r_dim.dim_y = (data.get_data())[i].y;

                    if ((w_type == Tango::READ_WRITE) || (w_type == Tango::READ_WITH_WRITE))
                    {
                        WAttribute &assoc_att = dev->get_device_attr()->get_w_attr_by_ind(att.get_assoc_ind());
                        (*aid.data_3)[0].w_dim.dim_x = assoc_att.get_w_dim_x();
                        (*aid.data_3)[0].w_dim.dim_y = assoc_att.get_w_dim_y();
                    }
                }
            }
        }

//
// Fill one slot of polling buffer
//

        try
        {
            std::vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_ATTR,obj_name);

            if (attr_failed == false)
            {
            	if (idl_vers >= 5)
				{
                    auto when = make_poll_time((*aid.data_5)[0].time);
                    auto zero = PollClock::duration::zero();
                    (*ite)->insert_data(aid.data_5,when,zero);
				}
                else if (idl_vers == 4)
                {
                    auto when = make_poll_time((*aid.data_4)[0].time);
                    auto zero = PollClock::duration::zero();
                    (*ite)->insert_data(aid.data_4,when,zero);
                }
                else
                {
                    auto when = make_poll_time((*aid.data_3)[0].time);
                    auto zero = PollClock::duration::zero();
                    (*ite)->insert_data(aid.data_3,when,zero);
                }
            }
            else
            {
                auto when = make_poll_time((data.get_data())[i].t_val);
                auto zero = PollClock::duration::zero();
                (*ite)->insert_except(save_except,when,zero);
            }
        }
        catch (Tango::DevFailed &)
        {
            if (attr_failed == false)
			{
				if (idl_vers >= 5)
					delete aid.data_5;
                else if (idl_vers == 4)
                    delete aid.data_4;
				else
                    delete aid.data_3;
			}
            else
                delete save_except;
        }

    }

    dev->get_poll_monitor().rel_monitor();
}

//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		Util::fill_cmd_polling_buffer
//
// description :
//		Fill command polling buffer with your own data
//
// args :
//		in :
// 			- dev : The device
//			- cmd_name : The command name
//			- data : The command data to be stored in the polling buffer
//
//----------------------------------------------------------------------------------------------------------------

template <typename T>
void Util::fill_cmd_polling_buffer(DeviceImpl *dev,std::string &cmd_name,CmdHistoryStack<T>  &data)
{

//
// Check that the device is polled
//

    if (dev->is_polled() == false)
    {
        TangoSys_OMemStream o;
        o << "Device " << dev->get_name() << " is not polled" << std::ends;

        Except::throw_exception((const char *)API_DeviceNotPolled,o.str(),
                    (const char *)"Util::fill_cmd_polling_buffer");
    }

//
// Command name in lower case letters and check that it is marked as polled
//

    std::string obj_name(cmd_name);
    std::transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);

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
        o << " which is less than " << nb_elt << "!" << std::ends;

        Except::throw_exception((const char *)API_DeviceNotPolled,o.str(),
                    (const char *)"Util::fill_cmd_polling_buffer");
    }

//
// Take the device monitor before the loop
// In case of large element number, it is time cousuming to take/release the monitor in the loop
// In case of large element number, it is time cousuming to take/release the monitor in the loop
//

    dev->get_poll_monitor().get_monitor();

//
// A loop on each record
//

    size_t i;
    Tango::DevFailed *save_except;
    bool cmd_failed;
    CORBA::Any *any_ptr;

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
            catch (std::bad_alloc &)
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
            catch (std::bad_alloc &)
            {
                dev->get_poll_monitor().rel_monitor();
                Except::throw_exception((const char *)API_MemoryAllocation,
                                (const char *)"Can't allocate memory in server",
                                (const char *)"Util::fill_cmd_polling_buffer");
            }

//
// Set command value in Any object
//

            T *tmp_ptr = (data.get_data())[i].ptr;
			(*any_ptr) <<= (*tmp_ptr);

			if ((data.get_data())[i].release == true)
				delete tmp_ptr;
        }

//
// Fill one slot of polling buffer
//

        try
        {
            std::vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_CMD,obj_name);
            auto when = make_poll_time((data.get_data())[i].t_val);
            auto zero = PollClock::duration::zero();
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

#endif /* UTILS_TPP */
