#ifndef _DEVICE_ATTRIBUTE_CONVERSIONS_H_
#define _DEVICE_ATTRIBUTE_CONVERSIONS_H_

#include "tango.h"
#include "data_type_utils.h"

namespace Tango
{

namespace detail {

template <bool CopySeq, AttributeDataType Type, typename AttributeValueT>
void move_seq_att_val_to_dev_attr(
    AttributeValueT& attr_val,
    DeviceAttribute& dev_attr)
{
    using SeqType = typename TangoTypeTraits<Type>::SeqType;

    auto& src = attr_val_seq<Type>(attr_val);
    auto& dest = dev_attr_seq<Type>(dev_attr);

    const auto length = src.length();
    const auto maximum = src.maximum();

    constexpr bool orphan = true;
    constexpr bool owner = true;

    if (src.release())
    {
        auto* buffer = src.get_buffer(orphan);
        dest = new SeqType(maximum, length, buffer, owner);
    }
    else
    {
        if (CopySeq)
        {
            // If release() is false, sequence received from CORBA call does not
            // own the memory. This can happen only if client and server are
            // located within the same process. We refuse to use such a sequence
            // and create a copy to prevent direct access to server's memory.

            dest = new SeqType(src);
        }
        else
        {
            auto* buffer = src.get_buffer();
            dest = new SeqType(maximum, length, buffer, !owner);
        }
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
inline void copy_attribute_value_fields_to_device_attribute<AttributeValue>(
    const AttributeValue& attr_val,
    DeviceAttribute& dev_attr)
{
    dev_attr.name = attr_val.name;
    dev_attr.quality = attr_val.quality;
    dev_attr.time = attr_val.time;
    dev_attr.dim_x = attr_val.dim_x;
    dev_attr.dim_y = attr_val.dim_y;
}

} // namespace detail

template <bool CopySeq, typename AttributeValueT>
void attribute_value_to_device_attribute_3(
    AttributeValueT& attr_value,
    DeviceAttribute& dev_attr)
{
    using namespace detail;

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
    case CORBA::tk_boolean:     return move_seq_att_val_to_dev_attr<CopySeq, ATT_BOOL>(attr_value, dev_attr);
    case CORBA::tk_short:       return move_seq_att_val_to_dev_attr<CopySeq, ATT_SHORT>(attr_value, dev_attr);
    case CORBA::tk_long:        return move_seq_att_val_to_dev_attr<CopySeq, ATT_LONG>(attr_value, dev_attr);
    case CORBA::tk_longlong:    return move_seq_att_val_to_dev_attr<CopySeq, ATT_LONG64>(attr_value, dev_attr);
    case CORBA::tk_float:       return move_seq_att_val_to_dev_attr<CopySeq, ATT_FLOAT>(attr_value, dev_attr);
    case CORBA::tk_double:      return move_seq_att_val_to_dev_attr<CopySeq, ATT_DOUBLE>(attr_value, dev_attr);
    case CORBA::tk_octet:       return move_seq_att_val_to_dev_attr<CopySeq, ATT_UCHAR>(attr_value, dev_attr);
    case CORBA::tk_ushort:      return move_seq_att_val_to_dev_attr<CopySeq, ATT_USHORT>(attr_value, dev_attr);
    case CORBA::tk_ulong:       return move_seq_att_val_to_dev_attr<CopySeq, ATT_ULONG>(attr_value, dev_attr);
    case CORBA::tk_ulonglong:   return move_seq_att_val_to_dev_attr<CopySeq, ATT_ULONG64>(attr_value, dev_attr);
    case CORBA::tk_string:      return move_seq_att_val_to_dev_attr<CopySeq, ATT_STRING>(attr_value, dev_attr);
    case CORBA::tk_enum:        return move_seq_att_val_to_dev_attr<CopySeq, ATT_STATE>(attr_value, dev_attr);
    default:
        break;
    }
}

template <bool CopySeq, typename AttributeValueT>
void attribute_value_to_device_attribute_5(
    AttributeValueT& attr_value,
    DeviceAttribute& dev_attr)
{
    using namespace detail;

    copy_attribute_value_fields_to_device_attribute(attr_value, dev_attr);
    dev_attr.data_format = attr_value.data_format;

    if (attr_value.quality == Tango::ATTR_INVALID)
    {
        return;
    }

    switch (attr_val_union(attr_value)._d())
    {
    case ATT_BOOL:      return move_seq_att_val_to_dev_attr<CopySeq, ATT_BOOL>(attr_value, dev_attr);
    case ATT_SHORT:     return move_seq_att_val_to_dev_attr<CopySeq, ATT_SHORT>(attr_value, dev_attr);
    case ATT_LONG:      return move_seq_att_val_to_dev_attr<CopySeq, ATT_LONG>(attr_value, dev_attr);
    case ATT_LONG64:    return move_seq_att_val_to_dev_attr<CopySeq, ATT_LONG64>(attr_value, dev_attr);
    case ATT_FLOAT:     return move_seq_att_val_to_dev_attr<CopySeq, ATT_FLOAT>(attr_value, dev_attr);
    case ATT_DOUBLE:    return move_seq_att_val_to_dev_attr<CopySeq, ATT_DOUBLE>(attr_value, dev_attr);
    case ATT_UCHAR:     return move_seq_att_val_to_dev_attr<CopySeq, ATT_UCHAR>(attr_value, dev_attr);
    case ATT_USHORT:    return move_seq_att_val_to_dev_attr<CopySeq, ATT_USHORT>(attr_value, dev_attr);
    case ATT_ULONG:     return move_seq_att_val_to_dev_attr<CopySeq, ATT_ULONG>(attr_value, dev_attr);
    case ATT_ULONG64:   return move_seq_att_val_to_dev_attr<CopySeq, ATT_ULONG64>(attr_value, dev_attr);
    case ATT_STRING:    return move_seq_att_val_to_dev_attr<CopySeq, ATT_STRING>(attr_value, dev_attr);
    case ATT_STATE:     return move_seq_att_val_to_dev_attr<CopySeq, ATT_STATE>(attr_value, dev_attr);
    case ATT_ENCODED:   return move_seq_att_val_to_dev_attr<CopySeq, ATT_ENCODED>(attr_value, dev_attr);
    case DEVICE_STATE:
        dev_attr.d_state = attr_val_union(attr_value).dev_state_att();
        dev_attr.d_state_filled = true;
        break;
    case ATT_NO_DATA:
        break;
    }
}

} // namespace Tango

#endif
