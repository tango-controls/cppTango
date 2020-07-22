#include <tango/server/attribute.h>

// TODO: Here we should just need to include <attribute.h>, but it then includes
// tango.h, which includes device.h which finally includes attribute.h followed
// by w_attribute.h. w_attribute.h depends on definitions from attribute.h which
// are not yet visible at w_attribute.h inclusion point.
#include <tango.h>

#include <utility>

namespace Tango
{

#define X_SUPPORTED_DATA_TYPES \
    X(DevShort) \
    X(DevLong) \
    X(DevLong64) \
    X(DevFloat) \
    X(DevDouble) \
    X(DevString) \
    X(DevBoolean) \
    X(DevUShort) \
    X(DevUChar) \
    X(DevULong) \
    X(DevULong64) \
    X(DevState) \
    X(DevEncoded)

Attribute::Attribute(std::unique_ptr<AttributePrivate> impl_) :
    impl(std::move(impl_))
{
}

Attribute::~Attribute() = default;

const AttributePrivate& Attribute::get_impl() const
{
    return *impl;
}

AttributePrivate& Attribute::get_impl()
{
    return *impl;
}

long Attribute::get_attr_idx()
{
    return impl->get_attr_idx();
}

std::string& Attribute::get_name_lower()
{
    return impl->get_name_lower();
}

bool Attribute::is_writ_associated()
{
    return impl->is_writ_associated();
}

bool Attribute::is_min_alarm()
{
    return impl->is_min_alarm();
}

bool Attribute::is_max_alarm()
{
    return impl->is_max_alarm();
}

bool Attribute::is_min_warning()
{
    return impl->is_min_warning();
}

bool Attribute::is_max_warning()
{
    return impl->is_max_warning();
}

bool Attribute::is_rds_alarm()
{
    return impl->is_rds_alarm();
}

std::bitset<Attribute::NUM_ALARM_FLAGS>& Attribute::is_alarmed()
{
    return impl->is_alarmed();
}

bool Attribute::is_polled()
{
    return impl->is_polled();
}

bool Attribute::check_alarm()
{
    return impl->check_alarm();
}

AttrWriteType Attribute::get_writable()
{
    return impl->get_writable();
}

std::string& Attribute::get_name()
{
    return impl->get_name();
}

long Attribute::get_data_type()
{
    return impl->get_data_type();
}

AttrDataFormat Attribute::get_data_format()
{
    return impl->get_data_format();
}

std::string& Attribute::get_assoc_name()
{
    return impl->get_assoc_name();
}

long Attribute::get_assoc_ind()
{
    return impl->get_assoc_ind();
}

void Attribute::set_assoc_ind(long val)
{
    return impl->set_assoc_ind(val);
}

TimeVal& Attribute::get_date()
{
    return impl->get_date();
}

void Attribute::set_date(TimeVal &new_date)
{
    return impl->set_date(new_date);
}

#ifdef _TG_WINDOWS_
void Attribute::set_date(struct _timeb &t)
{
    return impl->set_date(t);
}
#endif // _TG_WINDOWS_

void Attribute::set_date(struct timeval &t)
{
    return impl->set_date(t);
}

void Attribute::set_date(time_t new_date)
{
    return impl->set_date(new_date);
}

std::string& Attribute::get_label()
{
    return impl->get_label();
}

AttrQuality& Attribute::get_quality()
{
    return impl->get_quality();
}

void Attribute::set_quality(AttrQuality qua, bool send_event)
{
    return impl->set_quality(qua, send_event);
}

long Attribute::get_data_size()
{
    return impl->get_data_size();
}

long Attribute::get_x()
{
    return impl->get_x();
}

long Attribute::get_max_dim_x()
{
    return impl->get_max_dim_x();
}

long Attribute::get_y()
{
    return impl->get_y();
}

long Attribute::get_max_dim_y()
{
    return impl->get_max_dim_y();
}

long Attribute::get_polling_period()
{
    return impl->get_polling_period();
}

template <typename T>
void Attribute::get_properties(Tango::MultiAttrProp<T> &props)
{
    return impl->get_properties(props);
}

template <typename T>
void Attribute::set_properties(Tango::MultiAttrProp<T> &props)
{
    return impl->set_properties(props);
}

void Attribute::set_attr_serial_model(AttrSerialModel ser_model)
{
    return impl->set_attr_serial_model(ser_model);
}

AttrSerialModel Attribute::get_attr_serial_model()
{
    return impl->get_attr_serial_model();
}

void Attribute::set_user_attr_mutex(omni_mutex *mut_ptr)
{
    return impl->set_user_attr_mutex(mut_ptr);
}

#define X(Type) \
void Attribute::set_value(Type* p_data, long x, long y, bool release) \
{ \
    return impl->set_value(p_data, x, y, release); \
}
X_SUPPORTED_DATA_TYPES
#undef X

void Attribute::set_value(DevString* p_data_str, DevUChar* p_data, long size, bool release)
{
    return impl->set_value(p_data_str, p_data, size, release);
}

void Attribute::set_value(EncodedAttribute* attr)
{
    return impl->set_value(attr);
}

#define X(Type) \
void Attribute::set_value_date_quality( \
    Type* p_data, time_t t, AttrQuality qual, long x, long y, bool release) \
{ \
    return impl->set_value_date_quality(p_data, t, qual, x, y, release); \
}
X_SUPPORTED_DATA_TYPES
#undef X

void Attribute::set_value_date_quality(
    DevString* p_data_str, DevUChar* p_data, long size, time_t t, AttrQuality qual, bool release)
{
    return impl->set_value_date_quality(p_data_str, p_data, size, t, qual, release);
}

#ifdef _TG_WINDOWS_

#define X(Type) \
void Attribute::set_value_date_quality( \
    Type* p_data, struct _timeb& t, AttrQuality qual, long x, long y, bool release) \
{ \
    return impl->set_value_date_quality(p_data, t, qual, x, y, release); \
}
X_SUPPORTED_DATA_TYPES
#undef X

void Attribute::set_value_date_quality(
    DevString* p_data_str, DevUChar* p_data, long size, struct _timeb& t, AttrQuality qual, bool release)
{
    return impl->set_value_date_quality(p_data_str, p_data, size, t, qual, release);
}

#else // _TG_WINDOWS_

#define X(Type) \
void Attribute::set_value_date_quality( \
    Type* p_data, struct timeval& t, AttrQuality qual, long x, long y, bool release) \
{ \
    return impl->set_value_date_quality(p_data, t, qual, x, y, release); \
}
X_SUPPORTED_DATA_TYPES
#undef X

void Attribute::set_value_date_quality(
    DevString* p_data_str, DevUChar* p_data, long size, struct timeval& t, AttrQuality qual, bool release)
{
    return impl->set_value_date_quality(p_data_str, p_data, size, t, qual, release);
}

#endif // _TG_WINDOWS_

void Attribute::fire_change_event(DevFailed *except)
{
    return impl->fire_change_event(except);
}

void Attribute::set_change_event(bool implemented, bool detect)
{
    return impl->set_change_event(implemented, detect);
}

bool Attribute::is_change_event()
{
    return impl->is_change_event();
}

bool Attribute::is_check_change_criteria()
{
    return impl->is_check_change_criteria();
}

void Attribute::fire_archive_event(DevFailed *except)
{
    return impl->fire_archive_event(except);
}

void Attribute::set_archive_event(bool implemented, bool detect)
{
    return impl->set_archive_event(implemented, detect);
}

bool Attribute::is_archive_event()
{
    return impl->is_archive_event();
}

bool Attribute::is_check_archive_criteria()
{
    return impl->is_check_archive_criteria();
}

void Attribute::set_data_ready_event(bool implemented)
{
    return impl->set_data_ready_event(implemented);
}

bool Attribute::is_data_ready_event()
{
    return impl->is_data_ready_event();
}

void Attribute::fire_event(std::vector<std::string>& filt_names, std::vector<double>& filt_vals, DevFailed* except)
{
    return impl->fire_event(filt_names, filt_vals, except);
}

void Attribute::remove_configuration()
{
    return impl->remove_configuration();
}

template <typename T>
void Attribute::set_min_alarm(const T& new_min_alarm)
{
    return impl->set_min_alarm(new_min_alarm);
}

void Attribute::set_min_alarm(char* new_min_alarm)
{
    return impl->set_min_alarm(new_min_alarm);
}

void Attribute::set_min_alarm(const char* new_min_alarm)
{
    return impl->set_min_alarm(new_min_alarm);
}

template <typename T>
void Attribute::get_min_alarm(T& min_al)
{
    return impl->get_min_alarm(min_al);
}

template <typename T>
void Attribute::set_max_alarm(const T& new_max_alarm)
{
    return impl->set_max_alarm(new_max_alarm);
}

void Attribute::set_max_alarm(char* new_max_alarm)
{
    return impl->set_max_alarm(new_max_alarm);
}

void Attribute::set_max_alarm(const char* new_max_alarm)
{
    return impl->set_max_alarm(new_max_alarm);
}

template <typename T>
void Attribute::get_max_alarm(T& max_al)
{
    return impl->get_max_alarm(max_al);
}

template <typename T>
void Attribute::set_min_warning(const T& new_min_warning)
{
    return impl->set_min_warning(new_min_warning);
}

void Attribute::set_min_warning(char* new_min_warning)
{
    return impl->set_min_warning(new_min_warning);
}

void Attribute::set_min_warning(const char* new_min_warning)
{
    return impl->set_min_warning(new_min_warning);
}

template <typename T>
void Attribute::get_min_warning(T& min_war)
{
    return impl->get_min_warning(min_war);
}

template <typename T>
void Attribute::set_max_warning(const T& new_max_warning)
{
    return impl->set_max_warning(new_max_warning);
}

void Attribute::set_max_warning(char* new_max_warning)
{
    return impl->set_max_warning(new_max_warning);
}

void Attribute::set_max_warning(const char* new_max_warning)
{
    return impl->set_max_warning(new_max_warning);
}

template <typename T>
void Attribute::get_max_warning(T& max_war)
{
    return impl->get_max_warning(max_war);
}

bool Attribute::is_fwd_att()
{
    return impl->is_fwd_att();
}

void Attribute::delete_seq_if_quality_is_invalid(AttrQuality qual)
{
    impl->delete_seq_if_quality_is_invalid(qual);
}

void Attribute::set_value_enum_impl(
    void* enum_ptr,
    const std::function<short(std::size_t)>& get_enum_value,
    const std::function<void()>& safe_delete,
    const std::type_info& original_type_info,
    long x, long y, bool release)
{
    return impl->set_value_enum_impl(
        enum_ptr,
        get_enum_value,
        safe_delete,
        original_type_info,
        x, y, release);
}

#define X(Type) \
template void Attribute::get_properties(MultiAttrProp<Type>&); \
template void Attribute::set_properties(MultiAttrProp<Type>&); \
template void Attribute::set_min_alarm(const Type&); \
template void Attribute::get_min_alarm(Type&); \
template void Attribute::set_max_alarm(const Type&); \
template void Attribute::get_max_alarm(Type&); \
template void Attribute::set_min_warning(const Type&); \
template void Attribute::get_min_warning(Type&); \
template void Attribute::set_max_warning(const Type&); \
template void Attribute::get_max_warning(Type&);
X_SUPPORTED_DATA_TYPES
#undef X

#undef X_SUPPORTED_DATA_TYPES

} // namespace Tango
