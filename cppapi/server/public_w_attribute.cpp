#include <tango/server/w_attribute.h>

// TODO: Should include w_attribute.h. See similar comment in public_attribute.cpp.
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
    X(DevBoolean) \
    X(DevUShort) \
    X(DevUChar) \
    X(DevULong) \
    X(DevULong64) \
    X(DevState)

WAttribute::WAttribute(std::unique_ptr<WAttributePrivate> impl_) :
    Attribute(std::move(impl_)),
    impl(dynamic_cast<WAttributePrivate*>(&Attribute::get_impl()))
{
    assert(impl != nullptr);
}

const WAttributePrivate& WAttribute::get_impl() const
{
    return *impl;
}

WAttributePrivate& WAttribute::get_impl()
{
    return *impl;
}

bool WAttribute::is_min_value()
{
    return impl->is_min_value();
}

template <typename T>
void WAttribute::set_min_value(const T& min_value)
{
    return impl->set_min_value(min_value);
}

void WAttribute::set_min_value(char* min_value)
{
    return impl->set_min_value(min_value);
}

void WAttribute::set_min_value(const char* min_value)
{
    return impl->set_min_value(min_value);
}

template <typename T>
void WAttribute::get_min_value(T& min_value)
{
    return impl->get_min_value(min_value);
}

bool WAttribute::is_max_value()
{
    return impl->is_max_value();
}

template <typename T>
void WAttribute::set_max_value(const T& max_value)
{
    return impl->set_max_value(max_value);
}

void WAttribute::set_max_value(char* max_value)
{
    return impl->set_max_value(max_value);
}

void WAttribute::set_max_value(const char* max_value)
{
    return impl->set_max_value(max_value);
}

template <typename T>
void WAttribute::get_max_value(T& max_value)
{
    return impl->get_max_value(max_value);
}

long WAttribute::get_write_value_length()
{
    return impl->get_write_value_length();
}

struct timeval& WAttribute::get_write_date()
{
    return impl->get_write_date();
}

#define X(Type) \
void WAttribute::get_write_value(Type& val) \
{ \
    return impl->get_write_value(val); \
} \
void WAttribute::get_write_value(const Type*& ptr) \
{ \
    return impl->get_write_value(ptr); \
}
X_SUPPORTED_DATA_TYPES
X(DevEncoded)
#undef X

void WAttribute::get_write_value(DevString& val)
{
    return impl->get_write_value(val);
}

void WAttribute::get_write_value(const ConstDevString *&ptr)
{
    return impl->get_write_value(ptr);
}

#define X(Type) \
void WAttribute::set_write_value(Type val) \
{ \
    return impl->set_write_value(val); \
} \
void WAttribute::set_write_value(Type* val, long x, long y) \
{ \
    return impl->set_write_value(val, x, y); \
} \
void WAttribute::set_write_value(std::vector<Type>& val, long x, long y) \
{ \
    return impl->set_write_value(val, x, y); \
}
X_SUPPORTED_DATA_TYPES
#undef X

void WAttribute::set_write_value(DevString val)
{
    return impl->set_write_value(val);
}

void WAttribute::set_write_value(std::string& val)
{
    return impl->set_write_value(val);
}

void WAttribute::set_write_value(DevString* val, long x, long y)
{
    return impl->set_write_value(val, x, y);
}

void WAttribute::set_write_value(std::vector<std::string>& val, long x, long y)
{
    return impl->set_write_value(val, x, y);
}

void WAttribute::set_write_value(DevEncoded* val, long x, long y)
{
    return impl->set_write_value(val, x, y);
}

template <typename T>
void WAttribute::get_write_value(T& val)
{
    return impl->get_write_value(val);
}

template <typename T>
void WAttribute::get_write_value(const T*& val)
{
    return impl->get_write_value(val);
}

template <typename T>
void WAttribute::set_write_value(T val)
{
    return impl->set_write_value(val);
}

template <typename T>
void WAttribute::set_write_value(T* val, long x, long y)
{
    return impl->set_write_value(val, x, y);
}

template <typename T>
void WAttribute::set_write_value(std::vector<T>& val, long x, long y)
{
    return impl->set_write_value(val, x, y);
}

#undef X_SUPPORTED_DATA_TYPES

} // namespace Tango
