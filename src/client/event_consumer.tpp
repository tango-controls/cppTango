//
// Created by ingvord on 10/24/16.
//

#pragma once


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::attr_to_device()
//
// description :
//		Method to initialize in the DeviceAttribute instance given to the user the attribute value which are received
//      in a AttrValUnion (or in a class inheriting from)
//
// argument :
//		in :
//			- attr_value :
//		out :
//			- dev_attr :
//
//-------------------------------------------------------------------------------------------------------------------
template <typename T>
inline void Tango::EventConsumer::base_attr_to_device(const T *attr_value,DeviceAttribute *dev_attr)
{
    dev_attr->name = attr_value->name;
    dev_attr->quality = attr_value->quality;
    dev_attr->time = attr_value->time;
    dev_attr->dim_x = attr_value->r_dim.dim_x;
    dev_attr->dim_y = attr_value->r_dim.dim_y;
    dev_attr->set_w_dim_x(attr_value->w_dim.dim_x);
    dev_attr->set_w_dim_y(attr_value->w_dim.dim_y);
    dev_attr->err_list = new DevErrorList(attr_value->err_list);
    dev_attr->data_format = attr_value->data_format;

    if (dev_attr->quality != Tango::ATTR_INVALID)
    {
        att_union_to_device(&attr_value->zvalue,dev_attr);
    }
}