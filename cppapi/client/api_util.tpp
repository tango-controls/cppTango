
//+==================================================================================================================
//
// C++ source code file for TANGO api class ApiUtil template methods
//
// programmer 	- Emmanuel Taurel (taurel@esrf.fr)
//
// Copyright (C) :      2014,2015
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Lesser Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// original 	- May 2002
//
// $Revision: 25091 $
//
//+==================================================================================================================

#include "data_type_utils.h"

namespace Tango
{

template <AttributeDataType Type, typename AttributeValueT>
void convert_attribute_value_to_device_attribute(
    AttributeValueT& attr_val,
    DeviceAttribute& dev_attr)
{
    using Traits = TangoTypeTraits<Type>;

    auto& seq = attr_val_union_get<Type>(attr_val.value);
    const auto length = seq.length();
    const auto maximum = seq.maximum();

    constexpr bool orphan = true;
    constexpr bool owner = true;

    if (seq.release())
    {
        auto* buffer = seq.get_buffer(orphan);
        dev_attr_seq<Type>(dev_attr) = new typename Traits::SeqType(maximum, length, buffer, owner);
    }
    else
    {
        auto* buffer = seq.get_buffer();
        dev_attr_seq<Type>(dev_attr) = new typename Traits::SeqType(maximum, length, buffer, not owner);
    }
}

//-----------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::attr_to_device_base()
//
// description :
//
//
//-------------------------------------------------------------------------------------------------------------------

template <typename AttributeValueT>
inline void ApiUtil::attr_to_device_base(
    const AttributeValueT *attr_value_const,
    DeviceAttribute *device_attr)
{
    // const must be dropped because underlying sequence may be released.
    // Signature cannot be corrected because this is a public method.
    auto& attr_value = const_cast<AttributeValueT&>(*attr_value_const);
    auto& dev_attr = *device_attr;

    dev_attr.name = attr_value.name;
    dev_attr.quality = attr_value.quality;
    dev_attr.data_format = attr_value.data_format;
    dev_attr.time = attr_value.time;
    dev_attr.dim_x = attr_value.r_dim.dim_x;
    dev_attr.dim_y = attr_value.r_dim.dim_y;
    dev_attr.set_w_dim_x(attr_value.w_dim.dim_x);
    dev_attr.set_w_dim_y(attr_value.w_dim.dim_y);
    dev_attr.err_list = new DevErrorList(attr_value.err_list);

    if (attr_value.quality == Tango::ATTR_INVALID)
    {
        return;
    }

    switch(attr_value.value._d())
    {
    case ATT_BOOL:      return convert_attribute_value_to_device_attribute<ATT_BOOL>(attr_value, dev_attr);
    case ATT_SHORT:     return convert_attribute_value_to_device_attribute<ATT_SHORT>(attr_value, dev_attr);
    case ATT_LONG:      return convert_attribute_value_to_device_attribute<ATT_LONG>(attr_value, dev_attr);
    case ATT_LONG64:    return convert_attribute_value_to_device_attribute<ATT_LONG64>(attr_value, dev_attr);
    case ATT_FLOAT:     return convert_attribute_value_to_device_attribute<ATT_FLOAT>(attr_value, dev_attr);
    case ATT_DOUBLE:    return convert_attribute_value_to_device_attribute<ATT_DOUBLE>(attr_value, dev_attr);
    case ATT_UCHAR:     return convert_attribute_value_to_device_attribute<ATT_UCHAR>(attr_value, dev_attr);
    case ATT_USHORT:    return convert_attribute_value_to_device_attribute<ATT_USHORT>(attr_value, dev_attr);
    case ATT_ULONG:     return convert_attribute_value_to_device_attribute<ATT_ULONG>(attr_value, dev_attr);
    case ATT_ULONG64:   return convert_attribute_value_to_device_attribute<ATT_ULONG64>(attr_value, dev_attr);
    case ATT_STRING:    return convert_attribute_value_to_device_attribute<ATT_STRING>(attr_value, dev_attr);
    case ATT_STATE:     return convert_attribute_value_to_device_attribute<ATT_STATE>(attr_value, dev_attr);
    case ATT_ENCODED:   return convert_attribute_value_to_device_attribute<ATT_ENCODED>(attr_value, dev_attr);
    case DEVICE_STATE:
        dev_attr.d_state = attr_value.value.dev_state_att();
        dev_attr.d_state_filled = true;
        break;
    case ATT_NO_DATA:
        break;
	}
}

} // End of tango namespace
