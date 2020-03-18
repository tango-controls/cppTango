#ifndef _DEVICE_ATTRIBUTE_CONVERSIONS_H_
#define _DEVICE_ATTRIBUTE_CONVERSIONS_H_

#include "tango.h"
#include "data_type_utils.h"

namespace Tango
{

template <AttributeDataType Type, typename AttributeValueT>
void convert_attribute_value_to_device_attribute(
    AttributeValueT& attr_val,
    DeviceAttribute& dev_attr)
{
    using Traits = TangoTypeTraits<Type>;

    auto& seq = attr_val_seq_get<Type>(attr_val);
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
        // If release() is false, sequence received from CORBA call does not
        // own the memory. This can happen only if client and server are
        // located within the same process. We refuse to use such a sequence
        // and create a copy to prevent direct access to server's memory.

        dev_attr_seq<Type>(dev_attr) = new typename Traits::SeqType(seq);
    }
}

template <typename AttributeValueT>
void copy_attribute_value_fields_to_device_attribute(
    const AttributeValueT& attr_val,
    DeviceAttribute& dev_attr)
{
    dev_attr.name = attr_val.name;
    dev_attr.quality = attr_val.quality;
    dev_attr.time = attr_val.time;
    dev_attr.dim_x = attr_val.r_dim.dim_x;
    dev_attr.dim_y = attr_val.r_dim.dim_y;
    dev_attr.set_w_dim_x(attr_val.w_dim.dim_x);
    dev_attr.set_w_dim_y(attr_val.w_dim.dim_y);
    dev_attr.err_list = new DevErrorList(attr_val.err_list);
}

template <>
void copy_attribute_value_fields_to_device_attribute<AttributeValue>(
    const AttributeValue& attr_val,
    DeviceAttribute& dev_attr)
{
    dev_attr.name = attr_val.name;
    dev_attr.quality = attr_val.quality;
    dev_attr.time = attr_val.time;
    dev_attr.dim_x = attr_val.dim_x;
    dev_attr.dim_y = attr_val.dim_y;
}

template <typename AttributeValueT>
void attribute_value_to_device_attribute_3(
    AttributeValueT& attr_value,
    DeviceAttribute& dev_attr)
{
    copy_attribute_value_fields_to_device_attribute(attr_value, dev_attr);

    if (dev_attr.quality == Tango::ATTR_INVALID)
    {
        return;
    }

    const auto type = attr_value.value.type();

    if (type->kind() == CORBA::tk_enum)
    {
        attr_value.value >>= dev_attr.d_state;
        dev_attr.d_state_filled = true;
        return;
    }

    const auto type_alias = type->content_type();
    const auto type_seq = type_alias->content_type();

    dev_attr.data_type = Tango::DEV_SHORT;

    switch (type_seq->kind())
    {
    case CORBA::tk_boolean:     return convert_attribute_value_to_device_attribute<ATT_BOOL>(attr_value, dev_attr);
    case CORBA::tk_short:       return convert_attribute_value_to_device_attribute<ATT_SHORT>(attr_value, dev_attr);
    case CORBA::tk_long:        return convert_attribute_value_to_device_attribute<ATT_LONG>(attr_value, dev_attr);
    case CORBA::tk_longlong:    return convert_attribute_value_to_device_attribute<ATT_LONG64>(attr_value, dev_attr);
    case CORBA::tk_float:       return convert_attribute_value_to_device_attribute<ATT_FLOAT>(attr_value, dev_attr);
    case CORBA::tk_double:      return convert_attribute_value_to_device_attribute<ATT_DOUBLE>(attr_value, dev_attr);
    case CORBA::tk_octet:       return convert_attribute_value_to_device_attribute<ATT_UCHAR>(attr_value, dev_attr);
    case CORBA::tk_ushort:      return convert_attribute_value_to_device_attribute<ATT_USHORT>(attr_value, dev_attr);
    case CORBA::tk_ulong:       return convert_attribute_value_to_device_attribute<ATT_ULONG>(attr_value, dev_attr);
    case CORBA::tk_ulonglong:   return convert_attribute_value_to_device_attribute<ATT_ULONG64>(attr_value, dev_attr);
    case CORBA::tk_string:      return convert_attribute_value_to_device_attribute<ATT_STRING>(attr_value, dev_attr);
    case CORBA::tk_enum:        return convert_attribute_value_to_device_attribute<ATT_STATE>(attr_value, dev_attr);
    default:
        break;
    }
}

template <typename AttributeValueT>
void attribute_value_to_device_attribute_5(
    AttributeValueT& attr_value,
    DeviceAttribute& dev_attr)
{
    copy_attribute_value_fields_to_device_attribute(attr_value, dev_attr);
    dev_attr.data_format = attr_value.data_format;

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

} // namespace Tango

#endif
