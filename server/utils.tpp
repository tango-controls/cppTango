//=============================================================================
//
// file :               utils.tpp
//
// description :        C++ source code for the Attribute class template
//                      methods
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
//=============================================================================

#ifndef _UTILS_TPP
#define _UTILS_TPP


template <typename T>
void Util::fill_attr_polling_buffer(DeviceImpl *dev,
				      string &att_name,
				      AttrHistoryStack<T>  &data)
{
//
// Check that the device is polled
//

    if (dev->is_polled() == false)
    {
        TangoSys_OMemStream o;
        o << "Device " << dev->get_name() << " is not polled" << ends;

        Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
                    (const char *)"Util::fill_attr_polling_buffer");
    }

//
// Attribute name in lower case letters and check that it is marked as polled
//

    string obj_name(att_name);
    transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);

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
        o << " of device " << dev->get_name() << " is WRITE only" << ends;

        Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
                    (const char *)"Util::fill_attr_polling_buffer");
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
                o << "It is defined for record number " << i + 1 << ends;

                Except::throw_exception((const char *)"API_NotSupportedFeature",o.str(),
                                        (const char *)"Util::fill_attr_polling_buffer");
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
        o << " DEV_ENCODED data type is supported starting with IDL V4" << ends;

        Except::throw_exception((const char *)"API_NotSupportedFeature",o.str(),
                    (const char *)"Util::fill_attr_polling_buffer");
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
                o << "It is defined for record number " << i + 1 << ends;

                Except::throw_exception((const char *)"API_NotSupportedFeature",o.str(),
                                        (const char *)"Util::fill_attr_polling_buffer");
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
        o << "Please update your device to IDL 3 or more" << ends;

        Except::throw_exception((const char *)"API_NotSupportedFeature",o.str(),
                    (const char *)"Util::fill_attr_polling_buffer");
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
        o << " which is less than " << nb_elt << "!" << ends;

        Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
                    (const char *)"Util::fill_attr_polling_buffer");
    }

//
// A loop on each record
//

    unsigned long i;
    Tango::DevFailed *save_except;
    Tango::AttributeValueList_3 *back_3;
    Tango::AttributeValueList_4 *back_4;
    bool attr_failed;

    struct timeval zero,when;
    zero.tv_sec = zero.tv_usec = 0;

//
// Take the device monitor before the loop
// In case of large element number, it is time cousuming to take/release
// the monitor in the loop
//

    dev->get_poll_monitor().get_monitor();

//
// The loop for each element
//

    for (i = 0;i < nb_elt;i++)
    {
        save_except = NULL;
        back_3 = NULL;
        back_4 = NULL;
        attr_failed = false;

        if ((data.get_data())[i].err.length() != 0)
        {
            attr_failed = true;
            try
            {
                save_except = new Tango::DevFailed((data.get_data())[i].err);
            }
            catch (bad_alloc)
            {
                dev->get_poll_monitor().rel_monitor();
                Except::throw_exception((const char *)"API_MemoryAllocation",
                                (const char *)"Can't allocate memory in server",
                                (const char *)"Util::fill_attr_polling_buffer");
            }
        }
        else
        {

//
// Allocate memory for the AttributeValueList sequence
//

            try
            {
                if (idl_vers == 4)
                {
                    back_4 = new Tango::AttributeValueList_4(1);
                    back_4->length(1);
                    (*back_4)[0].value.union_no_data(true);
                }
                else
                {
                    back_3 = new Tango::AttributeValueList_3(1);
                    back_3->length(1);
                }
            }
            catch (bad_alloc)
            {
                dev->get_poll_monitor().rel_monitor();
                Except::throw_exception((const char *)"API_MemoryAllocation",
                                (const char *)"Can't allocate memory in server",
                                (const char *)"Util::fill_attr_polling_buffer");
            }

//
// Init name,date and quality factor
//

            Tango::AttrQuality qu = (data.get_data())[i].qual;
            if (idl_vers==4)
            {
                (*back_4)[0].time.tv_sec = (data.get_data())[i].t_val.tv_sec;
                (*back_4)[0].time.tv_usec = (data.get_data())[i].t_val.tv_usec;
                (*back_4)[0].time.tv_nsec = 0;

                (*back_4)[0].quality = qu;
                (*back_4)[0].name = CORBA::string_dup(att_name.c_str());

                (*back_4)[0].w_dim.dim_x = 0;
                (*back_4)[0].w_dim.dim_y = 0;
                (*back_4)[0].r_dim.dim_x = 0;
                (*back_4)[0].r_dim.dim_y = 0;

                (*back_4)[0].data_format = att.get_data_format();
            }
            else
            {
                (*back_3)[0].time.tv_sec = (data.get_data())[i].t_val.tv_sec;
                (*back_3)[0].time.tv_usec = (data.get_data())[i].t_val.tv_usec;
                (*back_3)[0].time.tv_nsec = 0;

                (*back_3)[0].quality = qu;
                (*back_3)[0].name = CORBA::string_dup(att_name.c_str());

                (*back_3)[0].w_dim.dim_x = 0;
                (*back_3)[0].w_dim.dim_y = 0;
                (*back_3)[0].r_dim.dim_x = 0;
                (*back_3)[0].r_dim.dim_y = 0;
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

                dev->data_into_net_object(att,back_3,back_4,0,w_type,true);

//
// Init remaining fields
//

                if (idl_vers == 4)
                {
                    (*back_4)[0].r_dim.dim_x = (data.get_data())[i].x;
                    (*back_4)[0].r_dim.dim_y = (data.get_data())[i].y;

                    if ((w_type == Tango::READ_WRITE) || (w_type == Tango::READ_WITH_WRITE))
                    {
                        WAttribute &assoc_att = dev->get_device_attr()->get_w_attr_by_ind(att.get_assoc_ind());
                        (*back_4)[0].w_dim.dim_x = assoc_att.get_w_dim_x();
                        (*back_4)[0].w_dim.dim_y = assoc_att.get_w_dim_y();
                    }
                }
                else
                {
                    (*back_3)[0].r_dim.dim_x = (data.get_data())[i].x;
                    (*back_3)[0].r_dim.dim_y = (data.get_data())[i].y;

                    if ((w_type == Tango::READ_WRITE) || (w_type == Tango::READ_WITH_WRITE))
                    {
                        WAttribute &assoc_att = dev->get_device_attr()->get_w_attr_by_ind(att.get_assoc_ind());
                        (*back_3)[0].w_dim.dim_x = assoc_att.get_w_dim_x();
                        (*back_3)[0].w_dim.dim_y = assoc_att.get_w_dim_y();
                    }
                }
            }
        }

//
// Fill one slot of polling buffer
//

        try
        {
            vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_ATTR,obj_name);

            if (attr_failed == false)
            {
                if (idl_vers == 4)
                {
                    when.tv_sec  = (*back_4)[0].time.tv_sec - DELTA_T;
                    when.tv_usec = (*back_4)[0].time.tv_usec;
                    (*ite)->insert_data(back_4,when,zero);
                }
                else
                {
                    when.tv_sec  = (*back_3)[0].time.tv_sec - DELTA_T;
                    when.tv_usec = (*back_3)[0].time.tv_usec;
                    (*ite)->insert_data(back_3,when,zero);
                }
            }
            else
            {
                when = (data.get_data())[i].t_val;
                when.tv_sec = when.tv_sec - DELTA_T;
                (*ite)->insert_except(save_except,when,zero);
            }
        }
        catch (Tango::DevFailed &)
        {
            if (attr_failed == false)
                if (idl_vers == 4)
                    delete back_4;
            else
                    delete back_3;
            else
                delete save_except;
        }

    }

    dev->get_poll_monitor().rel_monitor();
}


template <typename T>
void Util::fill_cmd_polling_buffer(DeviceImpl *dev,
                 string &cmd_name,
                 CmdHistoryStack<T>  &data)
{

//
// Check that the device is polled
//

    if (dev->is_polled() == false)
    {
        TangoSys_OMemStream o;
        o << "Device " << dev->get_name() << " is not polled" << ends;

        Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
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

        Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
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
                Except::throw_exception((const char *)"API_MemoryAllocation",
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
                Except::throw_exception((const char *)"API_MemoryAllocation",
                                (const char *)"Can't allocate memory in server",
                                (const char *)"Util::fill_cmd_polling_buffer");
            }

//
// Set command value in Any object
// If the Any insertion is by pointer, with omniORB, it copy data into the Any
// and delete memory. Therefore, no need to delete memory if release is
// true. If release is false, uses the insertion by reference which only
// copy data.
//

            T *tmp_ptr = (data.get_data())[i].ptr;
            if ((data.get_data())[i].release == true)
                (*any_ptr) <<= tmp_ptr;
            else
                (*any_ptr) <<= (*tmp_ptr);

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



#endif /* UTILS_TPP */
