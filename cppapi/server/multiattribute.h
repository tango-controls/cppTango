//=============================================================================
//
// file :		MultiAttribute.h
//
// description :	Include file for the MultiAttribute class.
//			Each device has one object of this class. All device
//			attribute objects are stored in this class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
//
//
//=============================================================================

#ifndef _MULTIATTRIBUTE_H
#define _MULTIATTRIBUTE_H

#include <tango.h>
#include "event_subscription_state.h"

namespace Tango
{

class AttrProperty;
class DeviceClass;

//=============================================================================
//
//			The MultiAttribute class
//
//
// description :	There is one instance of this class for each device.
//			This is mainly a helper class. It maintains a vector
//			of all the attribute for the device
//
//=============================================================================

/**
 * There is one instance of this class for each device. This class is mainly
 * an aggregate of Attribute or WAttribute objects. It eases management of
 * multiple attributes
 *
 *
 * @headerfile tango.h
 * @ingroup Server
 */

class MultiAttribute
{
public:
/**@name Constructor
 * Only one constructor is defined for this class */
//@{
/**
 * Create a new MultiAttribute object.
 *
 * This constructor will in-turn call the constructor of the Attribute or
 * WAttribute class of all the device class attributes.
 *
 * @param dev_name The device name
 * @param dev_class Reference to the device DeviceClass object
 * @param dev The device pointer
 * @exception DevFailed If the command sent to the database failed.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	MultiAttribute(std::string &dev_name,DeviceClass *dev_class,DeviceImpl *dev);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The MultiAttribute desctructor.
 */
	~MultiAttribute();
//@}

/**@name Miscellaneous methods */
//@{
/**
 * Get Attribute object from its name.
 *
 * This method returns a reference to the Attribute object with a name passed
 * as parameter. The equality on attribute name is case independant.
 *
 * @param attr_name The attribute name
 * @return A reference to the Attribute object
 * @exception DevFailed If the attribute is not defined.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	Attribute &get_attr_by_name(const char *attr_name);
/**
 * Get Attribute object from its index.
 *
 * This method returns a reference to the Attribute object from the index in the
 * main attribute vector
 *
 * @param ind The attribute index
 * @return A reference to the Attribute object
 */
	Attribute &get_attr_by_ind(const long ind) {return *(attr_list[ind]);}
/**
 * Get Writable Attribute object from its name.
 *
 * This method returns a reference to the WAttribute object with a name passed
 * as parameter. The equality on attribute name is case independant.
 *
 * @param attr_name The attribute name
 * @return A reference to the writable attribute object
 * @exception DevFailed If the attribute is not defined.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	WAttribute &get_w_attr_by_name(const char *attr_name);
/**
 * Get Writable Attribute object from its index.
 *
 * This method returns a reference to the Writable Attribute object from the
 * index in the main attribute vector
 *
 * @param ind The attribute index
 * @return A reference to the WAttribute object
 */
	WAttribute &get_w_attr_by_ind(const long ind) {return static_cast<WAttribute &>(*(attr_list[ind]));}
/**
 * Get Attribute index into the main attribute vector from its name.
 *
 * This method returns the index in the Attribute vector (stored in the
 * MultiAttribute object) of an attribute with a given name. The name equality
 * is case independant
 *
 * @param attr_name The attribute name
 * @return The index in the main attributes vector
 * @exception DevFailed If the attribute is not found in the vector.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	long get_attr_ind_by_name(const char *attr_name);
/**
 * Get list of attribute with an alarm level defined.
 *
 * @return  A vector of long data. Each object is the index in the main
 * attribute vector of attribute with alarm level defined
 */
	std::vector<long> &get_alarm_list() {return alarm_attr_list;}
/**
 * Get attribute number.
 *
 * @return  The attribute number
 */
	unsigned long get_attr_nb() {return (unsigned long)attr_list.size();}

/**
 * Check alarm for one attribute with a given name.
 *
 * This method returns a boolean set to true if the attribute with the given
 * name is in alarm condition
 *
 * @param  attr_name  The attribute name
 * @return A boolean set to true if the attribute is in alarm
 * @exception DevFailed If the attribute does not have any alarm level defined.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 */
	bool check_alarm(const char *attr_name) {return get_attr_by_name(attr_name).check_alarm();}
/**
 * Check alarm for one attribute from its index in the main attributes vector.
 *
 * This method returns a boolean set to true if the attribute with the given
 * index in the attrobite object vector is in alarm condition
 *
 * @param  ind  The attribute index
 * @return A boolean set to true if the attribute is in alarm
 * @exception DevFailed If the attribute does not have any alarm level defined.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 */
	bool check_alarm(const long ind) {return get_attr_by_ind(ind).check_alarm();}
/**
 * Check alarm on all attribute(s) with an alarm defined.
 *
 * This method returns a boolean set to true if one of the attribute with an
 * alarm level defined is in alarm condition.
 *
 * @return A boolean set to true if one attribute is in alarm
 * @exception DevFailed If the alarm level are not defined for one of the
 * attribute in the list of alarmable one
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 */
	bool check_alarm();
/**
 * Add alarm message to device status.
 *
 * This method add alarm mesage to the string passed as parameter. A message
 * is added for each attribute which is in alarm condition
 *
 * @param  status  The string (should be the device status)
 */
	void read_alarm(std::string &status);
/**
 * Get the vector of attribute objects.
 *
 * Returns the vector of attribute objects.
 *
 */
	 std::vector<Attribute *> &get_attribute_list(){return attr_list;}
//@}

protected:
/**@name Class data members */
//@{
/**
 * The Attribute objects vector.
 *
 * This vector is often referred as the main attributes vector
 */
	std::vector<Attribute *>	attr_list;
/**
 * The list of writable attribute.
 *
 * It is a vector of index in the main attribute vector
 */
	std::vector<long>		writable_attr_list;
/**
 * The list of attribute with an alarm level defined.
 *
 * It is a vector of index in the main attribute vector
 */
	std::vector<long>		alarm_attr_list;
//@}

public:
/// @privatesection

	void add_write_value(AttributePrivate &);
	void add_attribute(std::string &,DeviceClass *,long);
	void add_fwd_attribute(std::string &,DeviceClass *,long,Attr *);
	void remove_attribute(std::string &,bool);
	std::vector<long> &get_w_attr_list() {return writable_attr_list;}
	bool is_att_quality_alarmed();
	void get_event_param(EventSubscriptionStates&);
	void set_event_param(const EventSubscriptionStates&);
	void add_alarmed_quality_factor(std::string &);
	void add_default(std::vector<AttrProperty> &,std::string &,std::string &,long);
	void add_attr(Attribute *att);
	void update(Attribute &,std::string &);
	void check_idl_release(DeviceImpl *);
	bool is_opt_prop(const std::string &);

private:
    class MultiAttributeExt
    {
	public:
		struct AttributePtrAndIndex
		{
			Attribute * att_ptr;
			long att_index_in_vector;
		};
		MultiAttributeExt() {}
		std::map<std::string, AttributePtrAndIndex> attr_map;
		void put_attribute_in_map(Attribute * att, long index)
		{
			AttributePtrAndIndex mapElement;
			mapElement.att_ptr = att;
			mapElement.att_index_in_vector = index;
			attr_map[att->get_impl().get_name_lower()] = mapElement;
		}
		void increment_state_and_status_indexes()
		{
			attr_map["state"].att_index_in_vector++;
			attr_map["status"].att_index_in_vector++;
		}
    };

	void concat(std::vector<AttrProperty> &,std::vector<AttrProperty> &,std::vector<AttrProperty> &);
	void add_user_default(std::vector<AttrProperty> &,std::vector<AttrProperty> &);
	void check_associated(long,std::string &);

    std::unique_ptr<MultiAttributeExt>           ext;           // Class extension
};


} // End of Tango namespace

#endif // _MULTIATTRIBUTE_H
