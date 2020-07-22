#ifndef TANGO_PUBLIC_W_ATTRIBUTE_H
#define TANGO_PUBLIC_W_ATTRIBUTE_H

#include <idl/tango.h>
#include <tango/common/common_types.h>
#include <tango/server/attribute.h>

#include <memory>
#include <string>
#include <vector>

#ifdef _TG_WINDOWS_
#include <winsock2.h> // for timeval
#else // _TG_WINDOWS_
#include <sys/time.h> // for timeval
#endif // _TG_WINDOWS_

namespace Tango
{

class WAttributePrivate;

/**
 * This class represents a writable attribute. It inherits from the Attribute
 * class and only add what is specific to writable attribute.
 *
 * @headerfile tango/server/w_attribute.h
 * @ingroup Server
 */

class WAttribute : public Attribute
{
public:

/// @privatesection

	WAttribute(std::unique_ptr<WAttributePrivate>);

	const WAttributePrivate& get_impl() const;
	WAttributePrivate& get_impl();

/// @publicsection

/**@name Attribute configuration methods
 * Miscellaneous methods dealing with attribute min and max value property
 */
//@{
/**
 * Check if the attribute has a minimum value.
 *
 * @return A boolean set to true if the attribute has a minimum value
 * defined
 */
	bool is_min_value();
/**
 * Set attribute minimum value
 *
 * @param min_value Reference to a variable which represents the new min value
 */
	template <typename T>
	void set_min_value(const T &min_value);

/**
 * Set attribute minimum value
 *
 * @param min_value The new min value
 */
	void set_min_value(char *min_value);

/**
 * Set attribute minimum value
 *
 * @param min_value The new min value
 */
	void set_min_value(const char *min_value);
/**
 * Gets attribute minimum value or throws an exception if the
 * attribute does not have a minimum value
 *
 * @param min_value Reference to a variable which value will be set to the attribute's
 * minimum value
 */
	template <typename T>
	void get_min_value(T &min_value);
/**
 * Check if the attribute has a maximum value.
 *
 * @return check_max_value A boolean set to true if the attribute has a maximum value
 * defined
 */
	bool is_max_value();
/**
 * Set attribute maximum value
 *
 * @param max_value Reference to a variable which represents the new max value
 */
	template <typename T>
	void set_max_value(const T &max_value);

/**
 * Set attribute maximum value
 *
 * @param max_value The new max value
 */
	void set_max_value(char *max_value);
/**
 * Set attribute maximum value
 *
 * @param max_value The new max value
 */
	void set_max_value(const char *max_value);
/**
 * Get attribute maximum value or throws an exception if the
 * attribute does not have a maximum value
 *
 * @param max_value Reference to a variable which value will be set to the attribute's
 * maximum value
 */
	template <typename T>
	void get_max_value(T &max_value);
//@}

/**@name Get new value for attribute
 * Miscellaneous method to retrieve from the WAttribute object the new value
 * for the attribute.
 */
 //@{
 /**
 * Retrieve the new value length (data number) for writable attribute.
 *
 * @return  The new value data length
 */
 	long get_write_value_length();

 /**
 * Retrieve the date of the last attribute writing. This is set only
 * if the attribute has a read different than set alarm. Otherwise,
 * date is set to 0.
 *
 * @return  The written date
 */
 	struct timeval &get_write_date();

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevShort.
 *
 * @param val A reference to a Tango::DevShort data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevShort &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevShort and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer wich will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevShort *&ptr);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong.
 *
 * @param val A reference to a Tango::DevLong data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevLong &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevLong *&ptr);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong64.
 *
 * @param val A reference to a Tango::DevLong64 data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevLong64 &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong64 and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevLong64 *&ptr);


/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevFloat.
 *
 * @param val A reference to a Tango::DevFloat data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevFloat &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevFloat and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevFloat *&ptr);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevDouble.
 *
 * @param val A reference to a Tango::DevDouble data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevDouble &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevDouble and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevDouble *&ptr);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevString.
 *
 * @param val A reference to a Tango::DevString data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevString &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevString and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::ConstDevString *&ptr);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevBoolean.
 *
 * @param val A reference to a Tango::DevBoolean data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevBoolean &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevBoolean and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevBoolean *&ptr);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevUShort.
 *
 * @param val A reference to a Tango::DevUShort data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevUShort &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevUShort and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevUShort *&ptr);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevUChar.
 *
 * @param val A reference to a Tango::DevUChar data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevUChar &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevUChar and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevUChar *&ptr);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevULong.
 *
 * @param val A reference to a Tango::DevULong data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevULong &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevULong and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevULong *&ptr);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevULong64.
 *
 * @param val A reference to a Tango::DevULong64 data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevULong64 &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong64 and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevULong64 *&ptr);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevState.
 *
 * @param val A reference to a Tango::DevState data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevState &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong64 and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevState *&ptr);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevEncoded.
 *
 * @param val A reference to a Tango::DevEncoded data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevEncoded &val);

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevEncoded and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevEncoded *&ptr);

//@}

/**@name Set new value for attribute
 * Miscellaneous method to set a WAttribute value
 */
 //@{
/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevShort.
 *
 * @param val A reference to a Tango::DevShort data
 */
	void set_write_value(Tango::DevShort val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevShort.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevShort *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevShort.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<Tango::DevShort> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevLong.
 *
 * @param val A reference to a Tango::DevLong data
 */
	void set_write_value(Tango::DevLong val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevLong.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevLong *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevLong.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<Tango::DevLong> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevLong64.
 *
 * @param val A reference to a Tango::DevLong64 data
 */
	void set_write_value(Tango::DevLong64 val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevLong64.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevLong64 *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevLong64.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<Tango::DevLong64> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevDouble.
 *
 * @param val A reference to a Tango::DevDouble
 */
	void set_write_value(Tango::DevDouble val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevDouble.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevDouble *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevDouble.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<Tango::DevDouble> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevString.
 *
 * @param val A reference to a Tango::DevString
 */
	void set_write_value(Tango::DevString val);
/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevString.
 *
 * @param val A reference to a std::string
 */
	void set_write_value(std::string &val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevString.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(Tango::DevString *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevString.
 *
 * @param val A vector of string containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<std::string> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevFloat.
 *
 * @param val A reference to a Tango::DevFloat
 */
	void set_write_value(Tango::DevFloat val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevFloat.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevFloat *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevFloat.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<Tango::DevFloat> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevBoolean.
 *
 * @param val A reference to a Tango::DevBoolean
 */
	void set_write_value(Tango::DevBoolean val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevBoolean.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevBoolean *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevBoolean.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<Tango::DevBoolean> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevUShort.
 *
 * @param val A reference to a Tango::DevUShort
 */
	void set_write_value(Tango::DevUShort val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevUShort.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevUShort *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevUShort.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<Tango::DevUShort> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevUChar.
 *
 * @param val A reference to a Tango::DevUChar
 */
	void set_write_value(Tango::DevUChar val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevUChar.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevUChar *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevUChar.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<Tango::DevUChar> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevULong.
 *
 * @param val A reference to a Tango::DevULong data
 */
	void set_write_value(Tango::DevULong val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevULong.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevULong *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevULong.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<Tango::DevULong> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevULong64.
 *
 * @param val A reference to a Tango::DevULong64 data
 */
	void set_write_value(Tango::DevULong64 val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevULong64.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevULong64 *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevULong64.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<Tango::DevULong64> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevState.
 *
 * @param val A reference to a Tango::DevState data
 */
	void set_write_value(Tango::DevState val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevState.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(Tango::DevState *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevState.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(std::vector<Tango::DevState> &val, long x = 1, long y = 0);
//@}

/// @privatesection

	template <typename T>
	void get_write_value(T&);

	template <typename T>
	void get_write_value(const T*&);

	template <typename T>
	void set_write_value(T);

	template <typename T>
	void set_write_value(T*, long, long);

	void set_write_value(Tango::DevEncoded *, long x = 1,long y = 0); // Dummy method for compiler

	template <typename T>
	void set_write_value(std::vector<T>&, long, long);

private:
	DevShort get_write_value_enum_impl(const std::type_info&);
	const DevShort* get_write_value_ptr_enum_impl(const std::type_info&);
	void set_write_value_enum_impl(const std::type_info&, short);
	void set_write_value_enum_impl(
		const std::type_info&,
		const std::function<short(std::size_t)>&,
		long, long);
	void set_write_value_enum_impl(
		const std::type_info&,
		const std::function<short(std::size_t)>&,
		std::size_t,
		long, long);

	WAttributePrivate* impl;
};

namespace
{

template <typename T>
void assert_type_is_enum()
{
    static_assert(std::is_enum<T>::value, "Value of enum attribute must be enumeration.");
    static_assert(
        std::is_same<std::underlying_type_t<T>, short>::value ||
        std::is_same<std::underlying_type_t<T>, unsigned int>::value,
        "Underlying type of enum attribute must be `short` or `unsigned int`.");
}

} // namespace

template <typename T>
void WAttribute::get_write_value(T& data)
{
    assert_type_is_enum<T>();
    data = static_cast<T>(get_write_value_enum_impl(typeid(T)));
}

template <typename T>
void WAttribute::get_write_value(const T*& data)
{
    assert_type_is_enum<T>();
    data = static_cast<const T*>(get_write_value_ptr_enum_impl(typeid(T)));
}

template <typename T>
void WAttribute::set_write_value(T data)
{
    assert_type_is_enum<T>();
    set_write_value_enum_impl(typeid(T), short(data));
}

template <typename T>
void WAttribute::set_write_value(T* data, long x, long y)
{
    assert_type_is_enum<T>();
    set_write_value_enum_impl(
        typeid(T),
        [&](std::size_t index){ return short(data[index]); },
        x, y);
}

template <typename T>
void WAttribute::set_write_value(std::vector<T>& data, long x, long y)
{
    assert_type_is_enum<T>();
    set_write_value_enum_impl(
        typeid(T),
        [&](std::size_t index){ return short(data[index]); },
        data.size(),
        x, y);
}

} // namespace Tango

#endif // TANGO_PUBLIC_W_ATTRIBUTE_H
