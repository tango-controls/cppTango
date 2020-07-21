#ifndef TANGO_PUBLIC_ATTRIBUTE_H
#define TANGO_PUBLIC_ATTRIBUTE_H

#include <idl/tango.h>
#include <tango/server/server_types.h>

#include <bitset>
#include <memory>
#include <string>
#include <vector>

#include <time.h> // for time_t

#ifdef _TG_WINDOWS_
#include <sys/timeb.h> // for timeb_
#include <winsock2.h> // for timeval
#else // _TG_WINDOWS_
#include <sys/time.h> // for timeval
#endif // _TG_WINDOWS_

namespace Tango
{

class AttributePrivate;

template <typename T>
class MultiAttrProp;

class EncodedAttribute;

/**
 * This class represents a Tango attribute.
 *
 * @headerfile tango/server/attribute.h
 * @ingroup Server
 */

class Attribute
{
public:

/// @privatesection

	constexpr static unsigned int NUM_ALARM_FLAGS = 5;

	Attribute(std::unique_ptr<AttributePrivate>);
	virtual ~Attribute();

	const AttributePrivate& get_impl() const;
	AttributePrivate& get_impl();

	// Used by Pogo in DeviceClass::erase_dynamic_attributes.
	long get_attr_idx();
	std::string& get_name_lower();

/// @publicsection

/**@name Check attribute methods
 * Miscellaneous method returning boolean flag according to attribute state
 */
//@{
/**
 * Check if the attribute has an associated writable attribute.
 *
 * This method returns a boolean set to true if the attribute has a writable
 * attribute associated to it.
 *
 * @return A boolean set to true if there is an associated writable attribute
 */
	bool is_writ_associated();
/**
 * Check if the attribute is in minimum alarm condition .
 *
 * @return A boolean set to true if the attribute is in alarm condition (read
 * value below the min. alarm).
 */
	bool is_min_alarm();
/**
 * Check if the attribute is in maximum alarm condition .
 *
 * @return A boolean set to true if the attribute is in alarm condition (read
 * value above the max. alarm).
 */
	bool is_max_alarm();
/**
 * Check if the attribute is in minimum warning condition .
 *
 * @return A boolean set to true if the attribute is in warning condition (read
 * value below the min. warning).
 */
	bool is_min_warning();
/**
 * Check if the attribute is in maximum warning condition .
 *
 * @return A boolean set to true if the attribute is in warning condition (read
 * value above the max. warning).
 */
	bool is_max_warning();
/**
 * Check if the attribute is in RDS alarm condition .
 *
 * @return A boolean set to true if the attribute is in RDS condition (Read
 * Different than Set).
 */
	bool is_rds_alarm();
/**
 * Check if the attribute has an alarm defined.
 *
 * This method returns a set of bits. Each alarm type is defined by one
 * bit.
 *
 * @return A bitset. Each bit is set if the coresponding alarm is on
 */
	std::bitset<NUM_ALARM_FLAGS> &is_alarmed();
/**
 * Check if the attribute is polled .
 *
 * @return A boolean set to true if the attribute is polled.
 */
	bool is_polled();
/**
 * Check if the attribute read value is below/above the alarm level.
 *
 * @return A boolean set to true if the attribute is in alarm condition.
 * @exception DevFailed If no alarm level is defined.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	bool check_alarm();
//@}

/**@name Get/Set object members.
 * These methods allow the external world to get/set DeviceImpl instance
 * data members
 */
//@{
/**
 * Get the attribute writable type (RO/WO/RW).
 *
 * @return The attribute write type.
 */
	Tango::AttrWriteType get_writable();
/**
 * Get attribute name
 *
 * @return The attribute name
 */
	std::string &get_name();
/**
 * Get attribute data type
 *
 * @return The attribute data type
 */
	long get_data_type();
/**
 * Get attribute data format
 *
 * @return The attribute data format
 */
	Tango::AttrDataFormat get_data_format();
/**
 * Get name of the associated writable attribute
 *
 * @return The associated writable attribute name
 */
	std::string &get_assoc_name();
/**
 * Get index of the associated writable attribute
 *
 * @return The index in the main attribute vector of the associated writable
 * attribute
 */
	long get_assoc_ind();
/**
 * Set index of the associated writable attribute
 *
 * @param val The new index in the main attribute vector of the associated writable
 * attribute
 */
	void set_assoc_ind(long val);
/**
 * Get attribute date
 *
 * @return The attribute date
 */
	Tango::TimeVal &get_date();
/**
 * Set attribute date
 *
 * @param new_date The attribute date
 */
	void set_date(Tango::TimeVal &new_date);
#ifdef _TG_WINDOWS_
/**
 * Set attribute date
 *
 * @param The attribute date
 */
	void set_date(struct _timeb &t);
#endif
/**
 * Set attribute date
 *
 * @param t The attribute date
 */
	void set_date(struct timeval &t);
/**
 * Set attribute date
 *
 * @param new_date The attribute date
 */
	void set_date(time_t new_date);
/**
 * Get attribute label property
 *
 * @return The attribute label
 */
 	std::string &get_label();
/**
 * Get attribute data quality
 *
 * @return The attribute data quality
 */
	Tango::AttrQuality &get_quality();
/**
 * Set attribute data quality
 *
 * @param qua	The new attribute data quality
 * @param send_event Boolean set to true if a change event should be sent
 */
	void set_quality(Tango::AttrQuality qua, bool send_event=false);

/**
 * Get attribute data size
 *
 * @return The attribute data size
 */
	long get_data_size();
/**
 * Get attribute data size in x dimension
 *
 * @return The attribute data size in x dimension. Set to 1 for scalar attribute
 */
	long get_x();
/**
 * Get attribute maximum data size in x dimension
 *
 * @return The attribute maximum data size in x dimension. Set to 1 for scalar attribute
 */
	long get_max_dim_x();
/**
 * Get attribute data size in y dimension
 *
 * @return The attribute data size in y dimension. Set to 0 for scalar and
 * spectrum attribute
 */
	long get_y();
/**
 * Get attribute maximum data size in y dimension
 *
 * @return The attribute maximum data size in y dimension. Set to 0 for scalar and
 * spectrum attribute
 */
	long get_max_dim_y();
/**
 * Get attribute polling period
 *
 * @return The attribute polling period in mS. Set to 0 when the attribute is
 * not polled
 */
	long get_polling_period();
/**
 * Get all modifiable attribute properties in one call
 *
 * This method initializes the members of a MultiAttrProp object with the modifiable
 * attribute properties values
 *
 * @param props A MultiAttrProp object.
 */
	template <typename T>
	void get_properties(Tango::MultiAttrProp<T> &props);
/**
 * Set all modifiable attribute properties in one call
 *
 * This method sets the modifiable attribute properties with the values
 * provided as members of MultiAttrProps object
 *
 * @param props A MultiAttrProp object.
 */
	template <typename T>
	void set_properties(Tango::MultiAttrProp<T> &props);
/**
 * Set attribute serialization model
 *
 * This method allows the user to choose the attribute serialization
 * model.
 *
 * @param ser_model The new serialisation model. The serialization model must be
 * one of ATTR_BY_KERNEL, ATTR_BY_USER or ATTR_NO_SYNC
 */
	void set_attr_serial_model(AttrSerialModel ser_model);
/**
 * Get attribute serialization model
 *
 * Get the attribute serialization model
 *
 * @return The attribute serialization model
 */
	AttrSerialModel get_attr_serial_model();
/**
 * Set attribute user mutex
 *
 * This method allows the user to give to the attribute object the pointer to
 * the omni_mutex used to protect its buffer. The mutex has to be locked when passed
 * to this method. The Tango kernel will unlock it when the data will be transferred
 * to the client.
 *
 * @param mut_ptr The user mutex pointer
 */
	void set_user_attr_mutex(omni_mutex *mut_ptr);
//@}


/**@name Set attribute value methods.
 * These methods allows the external world to set attribute object internal
 * value
 */
//@{
/**
 * Set internal attribute value (for Tango::DevShort attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevShort *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevLong attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevLong *p_data,long x = 1, long y = 0,bool release = false);

/**
 * Set internal attribute value (for Tango::DevLong64 attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevLong64 *p_data,long x = 1,long y = 0,bool release = false);

/**
 * Set internal attribute value (for Tango::DevFloat attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevFloat *p_data,long x = 1,long y = 0,bool release = false);

/**
 * Set internal attribute value (for Tango::DevDouble attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevDouble *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevString attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevString *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevBoolean attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevBoolean *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevUShort attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevUShort *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevUChar attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevUChar *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevULong attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevULong *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevULong64 attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevULong64 *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevState attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevState *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevEncoded attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevEncoded *p_data,long x = 1, long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevEncoded attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data_str The attribute string part read value
 * @param p_data The attribute raw data part read value
 * @param size Size of the attribute raw data part
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevString *p_data_str,Tango::DevUChar *p_data,long size,bool release = false);
/**
 * Set internal attribute value (for Tango::DevEncoded attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param attr Handle to EncodedAttribute object
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::EncodedAttribute *attr);

//---------------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevShort *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);

#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevShort *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevShort *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevLong attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevLong *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevLong *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevLong attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevLong *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevLong64 attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevLong64 *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevLong64 *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevLong64 attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevLong64 *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevFloat attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	void set_value_date_quality(Tango::DevFloat *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevFloat *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevFloat attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevFloat *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif


//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevDouble attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	void set_value_date_quality(Tango::DevDouble *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevDouble *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevDouble attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevDouble *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevString attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevString *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevString *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevString attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevString *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevBoolean attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	void set_value_date_quality(Tango::DevBoolean *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevBoolean *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevBoolean attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevBoolean *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevUShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	void set_value_date_quality(Tango::DevUShort *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevUShort *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevUShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevUShort *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevUChar attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	void set_value_date_quality(Tango::DevUChar *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevUChar *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevUChar attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevUChar *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevULong attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevULong *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevULong *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevULong attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevULong *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevULong64 attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevULong64 *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevULong64 *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevULong64 attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevULong64 *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevState attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevState *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevState *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevState attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevState *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevEncoded attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevEncoded *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
/**
 * Set internal attribute value, date and quality factor (for Tango::DevEncoded attribute data type
 * when splitted in format and data).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data_str The attribute coded format string
 * @param p_data The attribute raw data
 * @param size Size of the attribute raw data part
 * @param t The date
 * @param qual The attribute quality factor
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevString *p_data_str,
					Tango::DevUChar *p_data,
					long size,
				    time_t t,
				    Tango::AttrQuality qual,
				    bool release = false);
#ifdef _TG_WINDOWS_
	void set_value_date_quality(Tango::DevEncoded *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);

	void set_value_date_quality(Tango::DevString *p_data_str,
					Tango::DevUChar *p_data,
					long size,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevEncoded attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevEncoded *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
/**
 * Set internal attribute value, date and quality factor (for Tango::DevEncoded attribute data type
 * when splitted in data format and data themselves).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a
 * "_timeb" structure instead of a "timeval" structure to set date.
 *
 * @param p_data_str The attribute format string
 * @param p_data The attribute raw data
 * @param size Size of the attribute raw data part
 * @param t The date
 * @param qual The attribute quality factor
 * @param release The release flag. If true, memory pointed to by p_data will be
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevString *p_data_str,
					Tango::DevUChar *p_data,
					long size,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    bool release = false);
#endif


/**
 * Fire a change event for the attribute value. The event is pushed to the notification
 * daemon.
 * The attribute data must be set with one of the Attribute::set_value or
 * Attribute::setvalue_date_quality methods before fireing the event.
 * The event is triggered with or without the change event criteria depending
 * on the configuration choosen with set_change_event().
 * ATTENTION: The couple set_value() and fire_change_event() needs to be protected
 * against concurrent accesses to the same attribute. Such an access might happen during
 * a synchronous read or by a reading from the polling thread.
 * Inside all methods reading or writing commands and attributes this protection is
 * automatically done by the Tango serialisation monitor.
 * When fireing change events in your own code, you should use the push_change_event
 * methods of the DeviceImpl class or protect your code with the
 * Tango::AutoTangoMonitor on your device.
 * Example:
 *
 *	{
 * 		Tango::AutoTangoMonitor synch(this);
 *		att_temp_seq.set_value (temp_seq, 100);
 * 		att_temp_seq.fire_archive_event ();
 *	}
 *
 * @param except A pointer to a DevFailed exception to be thrown as archive event.
 */
	void fire_change_event(DevFailed *except = NULL);

/**
 * Set a flag to indicate that the server fires change events manually, without
 * the polling to be started for the attribute.
 * If the detect parameter is set to true, the criteria specified for the change
 * event are verified and the event is only pushed if they are fulfilled.
 * If detect is set to false the event is fired without any value checking!
 *
 * @param implemented True when the server fires change events manually.
 * @param detect Triggers the verification of the change event properties when set to true.
 */
	void set_change_event(bool implemented, bool detect = true);
/**
 * Check if the change event is fired manually (without polling) for this attribute.
 *
 * @return A boolean set to true if a manual fire change event is implemented.
 */
	bool is_change_event();
/**
 * Check if the change event criteria should be checked when firing
 * the event manually.
 *
 * @return A boolean set to true if a change event criteria will be checked.
 */
	bool is_check_change_criteria();


/**
 * Fire an archive event for the attribute value. The event is pushed to the notification
 * daemon.
 * The attribute data must be set with one of the Attribute::set_value or
 * Attribute::setvalue_date_quality methods before fireing the event.
 * The event is triggered with or without the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 * ATTENTION: The couple set_value() and fire_archive_event() needs to be protected
 * against concurrent accesses to the same attribute. Such an access might happen during
 * a synchronous read or by a reading from the polling thread.
 * Inside all methods reading or writing commands and attributes this protection is
 * automatically done by the Tango serialisation monitor.
 * When fireing archive events in your own code, you should use the push_archive_event
 * methods of the DeviceImpl class or protect your code with the
 * Tango::AutoTangoMonitor on your device.
 * Example:
 *
 *	{
 * 		Tango::AutoTangoMonitor synch(this);
 *		att_temp_seq.set_value (temp_seq, 100);
 * 		att_temp_seq.fire_archive_event ();
 *	}
 *
 * @param except A pointer to a DevFailed exception to be thrown as archive event.
 */
	void fire_archive_event(DevFailed *except = NULL);

/**
 * Set a flag to indicate that the server fires archive events manually, without
 * the polling to be started for the attribute
 * If the detect parameter is set to true, the criteria specified for the archive
 * event are verified and the event is only pushed if they are fulfilled.
 * If detect is set to false the event is fired without any value checking!
 *
 * @param implemented True when the server fires archive events manually.
 * @param detect Triggers the verification of the archive event properties when set to true.
 */
	void set_archive_event(bool implemented, bool detect = true);

/**
 * Check if the archive event is fired manually for this attribute.
 *
 * @return A boolean set to true if a manual fire archive event is implemented.
 */
	bool is_archive_event();
/**
 * Check if the archive event criteria should be checked when firing
 * the event manually.
 *
 * @return A boolean set to true if a archive event criteria will be checked.
 */
	bool is_check_archive_criteria();

/**
 * Set a flag to indicate that the server fires data ready events
 *
 * @param implemented True when the server fires change events manually.
 */
	void set_data_ready_event(bool implemented);
/**
 * Check if the data ready event is fired for this attribute.
 *
 * @return A boolean set to true if a fire data ready event is implemented.
 */
	bool is_data_ready_event();


/**
 * Fire a user event for the attribute value. The event is pushed to the notification
 * daemon.
 * The attribute data must be set with one of the Attribute::set_value or
 * Attribute::setvalue_date_quality methods before fireing the event.
 * ATTENTION: The couple set_value() and fire_event() needs to be protected
 * against concurrent accesses to the same attribute. Such an access might happen during
 * a synchronous read or by a reading from the polling thread.
 * Inside all methods reading or writing commands and attributes this protection is
 * automatically done by the Tango serialisation monitor.
 * When fireing archive events in your own code, you should use the push_event
 * methods of the DeviceImpl class or protect your code with the
 * Tango::AutoTangoMonitor on your device.
 * Example:
 *
 *	{
 * 		Tango::AutoTangoMonitor synch(this);
 *		att_temp_seq.set_value (temp_seq, 100);
 * 		att_temp_seq.fire_event ();
 *	}
 *
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param except A pointer to a DevFailed exception to be thrown as archive event.
 */
	void fire_event(std::vector<std::string> &filt_names,std::vector<double> &filt_vals,DevFailed *except = NULL);

/**
 * Remove the attribute configuration from the database.
 * This method can be used to clean-up all the configuration of an attribute to come back to
 * its default values or the remove all configuration of a dynamic attribute before deleting it.
 *
 * The method removes all configured attribute properties and removes the attribute from the
 * list of polled attributes.
 *
 * @exception DevFailed In case of database access problems.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void remove_configuration();
//@}


/**@name Set/Get attribute ranges (min_alarm, min_warning, max_warning, max_alarm) methods.
 * These methods allow the external world to set attribute object min_alarm, min_warning,
 * max_warning and max_alarm values
 */
//@{

/**
 * Set attribute minimum alarm.
 *
 * This method sets the attribute minimum alarm.
 *
 * @param new_min_alarm The new attribute minimum alarm value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	template <typename T>
	void set_min_alarm(const T &new_min_alarm);

/**
 * Set attribute minimum alarm.
 *
 * This method sets the attribute minimum alarm.
 *
 * @param new_min_alarm The new attribute minimum alarm value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_min_alarm(char *new_min_alarm);
/**
 * Set attribute minimum alarm.
 *
 * This method sets the attribute minimum alarm.
 *
 * @param new_min_alarm The new attribute minimum alarm value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_min_alarm(const char *new_min_alarm);

/**
 * Get attribute minimum alarm or throw an exception if the attribute
 * does not have the minimum alarm
 *
 * @param min_al Reference to a variable which value will be set to the attribute's
 * minimum alarm
 */
	template <typename T>
	void get_min_alarm(T &min_al);

/**
 * Set attribute maximum alarm.
 *
 * This method sets the attribute maximum alarm.
 *
 * @param new_max_alarm The new attribute maximum alarm value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	template <typename T>
	void set_max_alarm(const T &new_max_alarm);
/**
 * Set attribute maximum alarm.
 *
 * This method sets the attribute maximum alarm.
 *
 * @param new_max_alarm The new attribute maximum alarm value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_max_alarm(char *new_max_alarm);
/**
 * Set attribute maximum alarm.
 *
 * This method sets the attribute maximum alarm.
 *
 * @param new_max_alarm The new attribute maximum alarm value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_max_alarm(const char *new_max_alarm);

/**
 * Get attribute maximum alarm or throw an exception if the attribute
 * does not have the maximum alarm set
 *
 * @param max_al Reference to a variable which value will be set to the attribute's
 * maximum alarm
 */
	template <typename T>
	void get_max_alarm(T &max_al);

/**
 * Set attribute minimum warning.
 *
 * This method sets the attribute minimum warning.
 *
 * @param new_min_warning The new attribute minimum warning value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	template <typename T>
	void set_min_warning(const T &new_min_warning);
/**
 * Set attribute minimum warning.
 *
 * This method sets the attribute minimum warning.
 *
 * @param new_min_warning The new attribute minimum warning value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_min_warning(char *new_min_warning);
/**
 * Set attribute minimum warning.
 *
 * This method sets the attribute minimum warning.
 *
 * @param new_min_warning The new attribute minimum warning value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_min_warning(const char *new_min_warning);

/**
 * Get attribute minimum warning or throw an exception if the attribute
 * does not have the minimum warning set
 *
 * @param min_war Reference to a variable which value will be set to the attribute's
 * minimum warning
 */
	template <typename T>
	void get_min_warning(T &min_war);

/**
 * Set attribute maximum warning.
 *
 * This method sets the attribute maximum warning.
 *
 * @param new_max_warning The new attribute maximum warning value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	template <typename T>
	void set_max_warning(const T &new_max_warning);
/**
 * Set attribute maximum warning.
 *
 * This method sets the attribute maximum warning.
 *
 * @param new_max_warning The new attribute maximum warning value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_max_warning(char *new_max_warning);
/**
 * Set attribute maximum warning.
 *
 * This method sets the attribute maximum warning.
 *
 * @param new_max_warning The new attribute maximum warning value
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_max_warning(const char *new_max_warning);

/**
 * Get attribute maximum warning or throw an exception if the attribute
 * does not have the maximum warning set
 *
 * @param max_war Reference to a variable which value will be set to the attribute's
 * maximum warning
 */
	template <typename T>
	void get_max_warning(T &max_war);
//@}


/// @privatesection

	template <typename T>
	void set_value(T *,long x = 1,long y = 0,bool release = false);

	template <typename T>
	void set_value_date_quality(T *,time_t,Tango::AttrQuality,long x=1,long y=0,bool rel=false);
#ifdef _TG_WINDOWS_
	template <typename T>
	void set_value_date_quality(T *,struct _timeb &,Tango::AttrQuality,long x=1,long y=1,bool rel=false);
#else
	template <typename T>
	void set_value_date_quality(T *,struct timeval &,Tango::AttrQuality,long x=1,long y=1,bool rel=false);
#endif

private:
	std::unique_ptr<AttributePrivate> impl;
};

} // namespace Tango

#endif // TANGO_PUBLIC_ATTRIBUTE_H
