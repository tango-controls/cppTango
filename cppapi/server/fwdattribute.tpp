//+===================================================================================================================
//
// file :               FWdAttribute.tpp
//
// description :        C++ source code for the FwdAttribute class template methods when they are not specialized
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
// $Revision: 17240 $
//
//-===================================================================================================================

#ifndef _FWDATTRIBUTE_TPP
#define _FWDATTRIBUTE_TPP

namespace Tango
{

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::set_local_attribute
//
// description :
//		Set value in the local attribute
//
// argument :
//		in :
//			- da : The DeviceAttribute coming from the root device read_attribute()
//
//--------------------------------------------------------------------------------------------------------------------

template<typename T>
void FwdAttributePrivate::set_local_attribute(DeviceAttribute &da, T *&seq_ptr)
{
    qual = da.get_quality();

    TimeVal local_tv = da.get_date();
#ifdef _TG_WINDOWS_
    tv.time = local_tv.tv_sec;
    tv.millitm = local_tv.tv_usec / 1000;
#else
    tv.tv_sec = local_tv.tv_sec;
    tv.tv_usec = local_tv.tv_usec;
#endif

    da >> seq_ptr;

    if (writable == READ_WRITE || writable == READ_WITH_WRITE)
    {
        set_write_value(seq_ptr->get_buffer() + da.get_nb_read(), da.get_written_dim_x(), da.get_written_dim_y());
    }

    if (seq_ptr->release() == true)
        set_value_date_quality(seq_ptr->get_buffer(true), tv, qual, da.get_dim_x(), da.get_dim_y(), true);
    else
        set_value_date_quality(seq_ptr->get_buffer(), tv, qual, da.get_dim_x(), da.get_dim_y());

    delete seq_ptr;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::propagate_writen_data
//
// description :
//		Pass the data received by a writable attribute into a DeviceAttribute used to write another attribute
//
// argument :
//		in :
//			- attr : The attribute
//			- ptr : Intermediate pointer
//			- seq_ptr : Poniter to sequence used to fill in the DeviceAttribute
//		out :
//			- da : The DeviceAttribute coming from the root device read_attribute()
//
//--------------------------------------------------------------------------------------------------------------------

template<typename T, typename V>
void FwdAttributePrivate::propagate_writen_data(DeviceAttribute &da, WAttributePrivate &attr, T *&ptr, V *&seq_ptr)
{
    attr.get_write_value(const_cast<const T *&>(ptr));
    int data_length = attr.get_write_value_length();
    long w_dim_x = attr.get_w_dim_x();
    long w_dim_y = attr.get_w_dim_y();

    seq_ptr = new V(data_length, data_length, ptr, false);
    da.insert(seq_ptr, w_dim_x, w_dim_y);
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::new_att_conf_base
//
// description :
//
//
// argument :
//		in :
//			- conf : The new attribute configuration
//
// return :
//		True if the new configuration is not the same than the actual one. Return false otherwise
//
//--------------------------------------------------------------------------------------------------------------------

template<typename T>
bool FwdAttributePrivate::new_att_conf_base(const T &conf)
{
    if (std::string(conf.name.in()) != name)
        return true;

    if (conf.writable != writable)
        return true;

    if (conf.data_format != data_format)
        return true;

    if (conf.data_type != data_type)
        return true;

    if (std::string(conf.description.in()) != description)
        return true;

    if (std::string(conf.unit.in()) != unit)
        return true;

    if (std::string(conf.standard_unit.in()) != standard_unit)
        return true;

    if (std::string(conf.display_unit.in()) != display_unit)
        return true;

    if (std::string(conf.format.in()) != format)
        return true;

    if (std::string(conf.min_value.in()) != min_value_str)
        return true;

    if (std::string(conf.max_value.in()) != max_value_str)
        return true;

    if (conf.level != disp_level)
        return true;

    if (conf.writable_attr_name.in() != writable_attr_name)
        return true;

    if (std::string(conf.att_alarm.min_alarm.in()) != min_alarm_str)
        return true;

    if (std::string(conf.att_alarm.max_alarm.in()) != max_alarm_str)
        return true;

    if (std::string(conf.att_alarm.min_warning.in()) != min_warning_str)
        return true;

    if (std::string(conf.att_alarm.max_warning.in()) != max_warning_str)
        return true;

    std::string tmp_prop(conf.att_alarm.delta_t);
    if (tmp_prop == AlrmValueNotSpec)
    {
        if (delta_t_str != "0")
            return true;
    }
    else
    {
        if (tmp_prop != delta_t_str)
            return true;
    }

    tmp_prop = conf.att_alarm.delta_val;
    if (tmp_prop == AlrmValueNotSpec)
    {
        if (delta_val_str != AlrmValueNotSpec)
            return true;
    }
    else
    {
        if (tmp_prop != delta_val_str)
            return true;
    }

    double tmp_array[2];

//
// rel_change
//

    tmp_prop = conf.event_prop.ch_event.rel_change;
    if (tmp_prop == AlrmValueNotSpec)
    {
        if (rel_change[0] != INT_MAX || rel_change[1] != INT_MAX)
            return true;
    }
    else
    {
        if (rel_change[0] == INT_MAX && rel_change[1] == INT_MAX)
            return true;
        else
        {
            convert_event_prop(tmp_prop, tmp_array);
            if (tmp_array[0] != rel_change[0] || tmp_array[1] != rel_change[1])
                return true;
        }
    }

//
// abs_change
//

    tmp_prop = conf.event_prop.ch_event.abs_change;
    if (tmp_prop == AlrmValueNotSpec)
    {
        if (abs_change[0] != INT_MAX || abs_change[1] != INT_MAX)
            return true;
    }
    else
    {
        if (abs_change[0] == INT_MAX && abs_change[1] == INT_MAX)
            return true;
        else
        {
            convert_event_prop(tmp_prop, tmp_array);
            if (tmp_array[0] != abs_change[0] || tmp_array[1] != abs_change[1])
                return true;
        }
    }

//
// archive rel_change
//

    tmp_prop = conf.event_prop.arch_event.rel_change;
    if (tmp_prop == AlrmValueNotSpec)
    {
        if (archive_rel_change[0] != INT_MAX || archive_rel_change[1] != INT_MAX)
            return true;
    }
    else
    {
        if (archive_rel_change[0] == INT_MAX && archive_rel_change[1] == INT_MAX)
            return true;
        else
        {
            convert_event_prop(tmp_prop, tmp_array);
            if (tmp_array[0] != archive_rel_change[0] || tmp_array[1] != archive_rel_change[1])
                return true;
        }
    }

//
// archive abs_change
//

    tmp_prop = conf.event_prop.arch_event.abs_change;
    if (tmp_prop == AlrmValueNotSpec)
    {
        if (archive_abs_change[0] != INT_MAX || archive_abs_change[1] != INT_MAX)
            return true;
    }
    else
    {
        if (archive_abs_change[0] == INT_MAX && archive_abs_change[1] == INT_MAX)
            return true;
        else
        {
            convert_event_prop(tmp_prop, tmp_array);
            if (tmp_array[0] != archive_abs_change[0] || tmp_array[1] != archive_abs_change[1])
                return true;
        }
    }

//
// event period
//

    tmp_prop = conf.event_prop.per_event.period;
    if (tmp_prop == AlrmValueNotSpec)
    {
        if (event_period != DEFAULT_EVENT_PERIOD)
            return true;
    }
    else
    {
        std::stringstream ss;
        int tmp_per;

        ss << tmp_prop;
        ss >> tmp_per;

        if (tmp_per != event_period)
            return true;
    }

//
// archive event period
//

    tmp_prop = conf.event_prop.arch_event.period;
    if (tmp_prop == AlrmValueNotSpec)
    {
        if (archive_period != INT_MAX)
            return true;
    }
    else
    {
        std::stringstream ss;
        int tmp_per;

        ss << tmp_prop;
        ss >> tmp_per;

        if (tmp_per != archive_period)
            return true;
    }

    return false;
}

} // End of Tango namespace
#endif // _FWDATTRIBUTE_TPP
