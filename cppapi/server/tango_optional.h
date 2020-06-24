#ifndef _TANGO_OPTIONAL_H
#define _TANGO_OPTIONAL_H

#include <utility>
#include <cassert>

namespace Tango
{

struct tango_nullopt_t
{
    struct prevent_init {};
    explicit constexpr tango_nullopt_t(prevent_init) {}
};

constexpr tango_nullopt_t tango_nullopt{ tango_nullopt_t::prevent_init{} };

template <typename T>
class tango_optional
{
public:
    tango_optional()
        : has_value_(false)
        , value_()
    {
    }

    tango_optional(tango_nullopt_t)
        : has_value_(false)
        , value_()
    {
    }

    tango_optional(T value)
        : has_value_(true)
        , value_(std::move(value))
    {
    }

    tango_optional& operator=(tango_nullopt_t)
    {
        has_value_ = false;
        value_ = {};
        return *this;
    }

    tango_optional& operator=(T value)
    {
        has_value_ = true;
        value_ = std::move(value);
        return *this;
    }

    bool has_value() const
    {
        return has_value_;
    }

    T& operator*()
    {
        assert(has_value_);
        return value_;
    }

    const T& operator*() const
    {
        assert(has_value_);
        return value_;
    }

    T* operator->()
    {
        assert(has_value_);
        return &operator*();
    }

    const T* operator->() const
    {
        assert(has_value_);
        return &operator*();
    }

private:
    bool has_value_;
    T value_;
};

}

#endif
