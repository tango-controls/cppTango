#ifndef _TYPE_UTILS_H_
#define _TYPE_UTILS_H_

#include <type_traits>
#include <utility>

#include <tango.h>

namespace Tango
{

template <typename SeqVarType_>
struct TangoTypeTraitsBase
{
    using SeqVarType = SeqVarType_;
    using SeqType = typename std::remove_reference<decltype(std::declval<SeqVarType>().inout())>::type;
    using ElementType = typename std::remove_reference<decltype(*std::declval<SeqType>().get_buffer())>::type;
};

template <AttributeDataType Type>
struct TangoTypeTraits {};

#define ENTRY(TYPE_ENUM, SEQ_VAR_TYPE) \
    template <> struct TangoTypeTraits<TYPE_ENUM> : TangoTypeTraitsBase<SEQ_VAR_TYPE> {};
ENTRY(ATT_BOOL,     DevVarBooleanArray_var)
ENTRY(ATT_SHORT,    DevVarShortArray_var)
ENTRY(ATT_LONG,     DevVarLongArray_var)
ENTRY(ATT_LONG64,   DevVarLong64Array_var)
ENTRY(ATT_FLOAT,    DevVarFloatArray_var)
ENTRY(ATT_DOUBLE,   DevVarDoubleArray_var)
ENTRY(ATT_UCHAR,    DevVarCharArray_var)
ENTRY(ATT_USHORT,   DevVarUShortArray_var)
ENTRY(ATT_ULONG,    DevVarULongArray_var)
ENTRY(ATT_ULONG64,  DevVarULong64Array_var)
ENTRY(ATT_STRING,   DevVarStringArray_var)
ENTRY(ATT_STATE,    DevVarStateArray_var)
ENTRY(ATT_ENCODED,  DevVarEncodedArray_var)
#undef ENTRY

template <
    AttributeDataType Type,
    typename TangoTypeTraits<Type>::SeqVarType DeviceAttribute::*dev_attr_seq_,
    typename TangoTypeTraits<Type>::SeqType& (AttrValUnion::*attr_val_union_get_)(),
    const typename TangoTypeTraits<Type>::SeqType& (AttrValUnion::*attr_val_union_get_const_)() const,
    void (AttrValUnion::*attr_val_union_set_)(const typename TangoTypeTraits<Type>::SeqType&)
    >
struct TangoTypeOpsBase
{
    static constexpr auto dev_attr_seq = dev_attr_seq_;
    static constexpr auto attr_val_union_get = attr_val_union_get_;
    static constexpr auto attr_val_union_get_const = attr_val_union_get_const_;
    static constexpr auto attr_val_union_set = attr_val_union_set_;
};

template <AttributeDataType Type>
struct TangoTypeOps {};

#define ENTRY(TYPE_ENUM, DA_SEQ, UNION) \
    template <> struct TangoTypeOps<TYPE_ENUM> : TangoTypeOpsBase< \
        TYPE_ENUM, \
        &DeviceAttribute::DA_SEQ, \
        &AttrValUnion::UNION, \
        &AttrValUnion::UNION, \
        &AttrValUnion::UNION> {};
ENTRY(ATT_BOOL,     BooleanSeq, bool_att_value)
ENTRY(ATT_SHORT,    ShortSeq,   short_att_value)
ENTRY(ATT_LONG,     LongSeq,    long_att_value)
ENTRY(ATT_LONG64,   Long64Seq,  long64_att_value)
ENTRY(ATT_FLOAT,    FloatSeq,   float_att_value)
ENTRY(ATT_DOUBLE,   DoubleSeq,  double_att_value)
ENTRY(ATT_UCHAR,    UCharSeq,   uchar_att_value)
ENTRY(ATT_USHORT,   UShortSeq,  ushort_att_value)
ENTRY(ATT_ULONG,    ULongSeq,   ulong_att_value)
ENTRY(ATT_ULONG64,  ULong64Seq, ulong64_att_value)
ENTRY(ATT_STRING,   StringSeq,  string_att_value)
ENTRY(ATT_STATE,    StateSeq,   state_att_value)
ENTRY(ATT_ENCODED,  EncodedSeq, encoded_att_value)
#undef ENTRY

namespace detail {

template <AttributeDataType Type>
typename TangoTypeTraits<Type>::SeqType& attr_val_union_get(AttrValUnion& u)
{
    return (u.*TangoTypeOps<Type>::attr_val_union_get)();
}

template <AttributeDataType Type>
typename TangoTypeTraits<Type>::SeqType& attr_val_union_get(::CORBA::Any& a)
{
    typename TangoTypeTraits<Type>::SeqType* seq;
    a >>= seq;
    return *seq;
}

} // namespace detail

/*
 * Access value union stored in (Zmq)AttributeValue_N.
 * Returns AttrValUnion& (for AttributeValue_4 and better) or ::CORBA::Any&.
 */
template <typename AttributeValueT>
auto attr_val_union(AttributeValueT& av) -> decltype((av.value))
{
    return av.value;
}

inline AttrValUnion& attr_val_union(ZmqAttributeValue_4& av) { return av.zvalue; }
inline AttrValUnion& attr_val_union(ZmqAttributeValue_5& av) { return av.zvalue; }
inline const AttrValUnion& attr_val_union(const ZmqAttributeValue_4& av) { return av.zvalue; }
inline const AttrValUnion& attr_val_union(const ZmqAttributeValue_5& av) { return av.zvalue; }

/*
 * Access underlying sequence stored in (Zmq)AttributeValue_N.
 * Returns DevVar*Array with corresponding sequence.
 */
template <AttributeDataType Type, typename AttributeValueT>
typename TangoTypeTraits<Type>::SeqType& attr_val_seq(AttributeValueT& av)
{
    return detail::attr_val_union_get<Type>(attr_val_union(av));
}

/*
 * Access underlying sequence stored in DeviceAttribute.
 * Returns DevVar*Array_var with corresponding sequence.
 */
template <AttributeDataType Type>
typename TangoTypeTraits<Type>::SeqVarType& dev_attr_seq(DeviceAttribute& da)
{
    return da.*TangoTypeOps<Type>::dev_attr_seq;
}

} // namespace Tango

#endif
