//===================================================================================================================
//
// file :		attrdesc.h
//
// description :	Include file for the Attr classes hierarchy. Three classes are declared in this file :
//				The Attr class
//				The SpectrumAttr class
//				The ImageAttr class
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
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
//
//===================================================================================================================

#ifndef _ATTRDESC_H
#define _ATTRDESC_H

#include <tango.h>

namespace Tango
{

class AttrProperty;
class WAttribute;


/**
 * User class to set attribute default properties.
 *
 * This class is used to set attribute default properties. Three levels of
 * attributes properties setting are implemented within Tango. The highest
 * property setting level is the database. Then the user default (set using
 * this UserDefaultAttrProp class) and finally a Tango library default
 * value
 *
 *
 * @headerfile tango.h
 * @ingroup Server
 */

class UserDefaultAttrProp
{
public:

/**@name Constructor
 * Only one constructor is defined for this class */
//@{
/**
 * Constructs a newly allocated UserDefaultAttrProp object.
 */
	UserDefaultAttrProp():ext(nullptr) {}
//@}

/**@name Set default property methods */
//@{
/**
 * Set default label property
 *
 * @param	def_label	The user default label property
 */
	void set_label(const char *def_label)
	{
		label = def_label;
	}

/**
 * Set default description property
 *
 * @param	def_desc	The user default description property
 */
	void set_description(const char *def_desc)
	{
		description = def_desc;
	}

/**
 * Set default unit property
 *
 * @param	def_unit	The user default unit property
 */
	void set_unit(const char *def_unit)
	{
		unit = def_unit;
	}

/**
 * Set default standard unit property
 *
 * @param	def_std_unit	The user default standard unit property
 */
	void set_standard_unit(const char *def_std_unit)
	{
		standard_unit = def_std_unit;
	}

/**
 * Set default display unit property
 *
 * @param	def_disp_unit	The user default display unit property
 */
	void set_display_unit(const char *def_disp_unit)
	{
		display_unit = def_disp_unit;
	}

/**
 * Set default format property
 *
 * @param	def_format	The user default format property
 */
	void set_format(const char *def_format)
	{
		format = def_format;
	}

/**
 * Set default min_value property
 *
 * @param	def_min_value	The user default min_value property
 */
	void set_min_value(const char *def_min_value)
	{
		min_value = def_min_value;
	}

/**
 * Set default max_value property
 *
 * @param	def_max_value	The user default max_value property
 */
	void set_max_value(const char *def_max_value)
	{
		max_value = def_max_value;
	}

/**
 * Set default min_alarm property
 *
 * @param	def_min_alarm	The user default min_alarm property
 */
	void set_min_alarm(const char *def_min_alarm)
	{
		min_alarm = def_min_alarm;
	}

/**
 * Set default max_alarm property
 *
 * @param	def_max_alarm	The user default max_alarm property
 */
	void set_max_alarm(const char *def_max_alarm)
	{
		max_alarm = def_max_alarm;
	}

/**
 * Set default min_warning property
 *
 * @param	def_min_warning	The user default min_warning property
 */
	void set_min_warning(const char *def_min_warning)
	{
		min_warning = def_min_warning;
	}

/**
 * Set default max_warning property
 *
 * @param	def_max_warning	The user default max_warning property
 */
	void set_max_warning(const char *def_max_warning)
	{
		max_warning = def_max_warning;
	}

/**
 * Set default RDS alarm delta_t property
 *
 * @param	def_delta_t	The user default RDS alarm delta_t property
 */
	void set_delta_t(const char *def_delta_t)
	{
		delta_t = def_delta_t;
	}

/**
 * Set default RDS alarm delta_val property
 *
 * @param	def_delta_val	The user default RDS alarm delta_val property
 */
	void set_delta_val(const char *def_delta_val)
	{
		delta_val = def_delta_val;
	}

/**
 * Set default change event abs_change property
 *
 * @param	def_abs_change	The user default change event abs_change property
 */
	void set_event_abs_change(const char *def_abs_change)
	{
		abs_change = def_abs_change;
	}

/**
 * Set default change event rel_change property
 *
 * @param	def_rel_change	The user default change event rel_change property
 */
	void set_event_rel_change(const char *def_rel_change)
	{
		rel_change = def_rel_change;
	}

/**
 * Set default periodic event period property
 *
 * @param	def_period	The user default periodic event period property
 */
	void set_event_period(const char *def_period)
	{
		period = def_period;
	}

/**
 * Set default archive event abs_change property
 *
 * @param	def_archive_abs_change	The user default archive event abs_change property
 */
	void set_archive_event_abs_change(const char *def_archive_abs_change)
	{
		archive_abs_change = def_archive_abs_change;
	}

/**
 * Set default archive event rel_change property
 *
 * @param	def_archive_rel_change	The user default archive event rel_change property
 */
	void set_archive_event_rel_change(const char *def_archive_rel_change)
	{
		archive_rel_change = def_archive_rel_change;
	}

/**
 * Set default archive event period property
 *
 * @param	def_archive_period	The user default archive event period property
 */
	void set_archive_event_period(const char *def_archive_period)
	{
		archive_period = def_archive_period;
	}

/**
 * Set default enumeration labels
 *
 * @param	def_enum_labels	The enumeration labels
 */
	void set_enum_labels(std::vector<std::string> &def_enum_labels)
	{
		for (size_t loop = 0;loop < def_enum_labels.size();loop++)
		{
			enum_labels = enum_labels + def_enum_labels[loop];
			if (loop != def_enum_labels.size() - 1)
				enum_labels = enum_labels + ',';
		}
	}
//@}

/// @privatesection
	~UserDefaultAttrProp() {}

	void set_abs_change(const char *def_abs_change)
	{
		set_event_abs_change(def_abs_change);
	}

	void set_rel_change(const char *def_rel_change)
	{
		set_event_rel_change(def_rel_change);
	}

	void set_period(const char *def_period)
	{
		set_event_period(def_period);
	}

	void set_archive_abs_change(const char *def_archive_abs_change)
	{
		set_archive_event_abs_change(def_archive_abs_change);
	}

	void set_archive_rel_change(const char *def_archive_rel_change)
	{
		set_archive_event_rel_change(def_archive_rel_change);
	}

	void set_archive_period(const char *def_archive_period)
	{
		set_archive_event_period(def_archive_period);
	}

	std::string 			label;
	std::string 			description;
	std::string 			unit;
	std::string 			standard_unit;
	std::string 			display_unit;
	std::string 			format;
	std::string 			min_value;
	std::string			max_value;
	std::string			min_alarm;
	std::string			max_alarm;
	std::string			min_warning;
	std::string			max_warning;
	std::string			delta_val;
	std::string			delta_t;
	std::string			abs_change;
	std::string			rel_change;
	std::string			period;
	std::string			archive_abs_change;
	std::string			archive_rel_change;
	std::string			archive_period;
	std::string			enum_labels;

private:
    class UserDefaultAttrPropExt
    {
    };

    std::unique_ptr<UserDefaultAttrPropExt>  ext;           // Class extension
};

/**
 * User class to create a no dimension attribute object.
 *
 * Information from this class and information fetched out from the Tango
 * database allows the Tango core software to create the Attribute object
 * for the attribute created by the user.
 *
 *
 * @headerfile tango.h
 * @ingroup Server
 */

class Attr
{
public:

/**@name Constructors
 * Two constructors are defined for this class */
//@{
/**
 * Constructs a newly allocated Attr object.
 * The attribute display level is set to OPERATOR.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	w_type	The attribute type (read, write, read with write ...)
 * @param	assoc	Name of the associated writable attribute. This is used
 * only the read with write attribute
 *
 */
	Attr(const char *name,long data_type,
	     Tango::AttrWriteType w_type = Tango::READ,
	     const char *assoc = AssocWritNotSpec);

/**
 * Constructs a newly allocated Attr object.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	disp	The attribute display level
 * @param	w_type	The attribute type (read, write, read with write ...)
 * @param	assoc	Name of the associated writable attribute. This is used
 * only the read with write attribute
 *
 */
	Attr(const char *name,long data_type,Tango::DispLevel disp,
	     Tango::AttrWriteType w_type = Tango::READ,
	     const char *assoc = AssocWritNotSpec);

/**
 * Constructs a newly allocated Attr object.
 *
 * @param 	name	The attribute name
 * @param	disp	The attribute display level
 *
 */
	Attr(const char *name,Tango::DispLevel disp = Tango::OPERATOR);

//@}

/**@name Destructor
 * Only one destructor is defined for this class */
//@{
/**
 * The object destructor.
 */
	virtual ~Attr();
//@}

/**@name Miscellaneous methods*/
//@{
/**
 * Set default attribute properties
 *
 * @param	prop	The user default property class
 */
 	void set_default_properties(UserDefaultAttrProp &prop);
/**
 * Set the attribute display level
 *
 * @param	level	The attribute display level
 */
 	void set_disp_level(Tango::DispLevel level) {disp_level = level;}
/**
 * Set the attribute polling update period
 *
 * @param	update	The attribute polling period (in mS)
 */
 	void set_polling_period(long update) {poll_period = update;}
/**
 * Set the attribute as memorized in database (only for scalar and writable
 * attribute)
 * By default the setpoint will be written to the attribute during initialisation!
 * Use method set_memorized_init() with false as argument if you don't want this feature
 */
 	void set_memorized();
/**
 * Set the initialisation flag for memorized attributes
 * 	true  = the setpoint value will be written to the attribute on initialisation
 *  false = only the attribute setpoint is initialised. No action is taken on the attribute
 *
 * @param	write_on_init	If true the setpoint value will be written to the attribute on initialisation
 */
 	void set_memorized_init(bool write_on_init) {mem_init = write_on_init;}
/**
 * Set a flag to indicate that the server fires change events manually without
 * the polling to be started for the attribute.
 * If the detect parameter is set to true, the criteria specified for the change
 * event are verified and the event is only pushed if they are fulfilled.
 * If detect is set to false the event is fired without checking!
 *
 * @param implemented True when the server fires change events manually.
 * @param detect Triggers the verification of the change event properties when set to true.
 */
	void set_change_event(bool implemented, bool detect)
    {
        fire_change_event  = implemented;check_change_event = detect;
    }

/**
 * Check if the change event is fired manually for this attribute.
 *
 * @return A boolean set to true if a manual fire change event is implemented.
 */
	bool is_change_event() {return fire_change_event;}
/**
 * Check if the change event criteria should be checked when firing
 * the event manually.
 *
 * @return A boolean set to true if a change event criteria will be checked.
 */
	bool is_check_change_criteria() {return check_change_event;}
/**
 * Set a flag to indicate that the server fires archive events manually without
 * the polling to be started for the attribute
 * If the detect parameter is set to true, the criteria specified for the archive
 * event are verified and the event is only pushed if they are fulfilled.
 * If detect is set to false the event is fired without checking!
 *
 * @param implemented True when the server fires archive events manually.
 * @param detect Triggers the verification of the archive event properties when set to true.
 */
	void set_archive_event(bool implemented, bool detect)
    {
        fire_archive_event  = implemented;check_archive_event = detect;
    }

/**
 * Check if the archive event is fired manually for this attribute.
 *
 * @return A boolean set to true if a manual fire archive event is implemented.
 */
	bool is_archive_event() {return fire_archive_event;}
/**
 * Check if the archive event criteria should be checked when firing
 * the event manually.
 *
 * @return A boolean set to true if a archive event criteria will be checked.
 */
	bool is_check_archive_criteria() {return check_archive_event;}

/**
 * Set a flag to indicate that the server fires data ready events
 *
 * @param implemented True when the server fires data ready events
 */
	void set_data_ready_event(bool implemented) { fire_dr_event  = implemented;}
/**
 * Check if the data ready event is fired for this attribute.
 *
 * @return A boolean set to true if firing data ready event is implemented.
 */
	bool is_data_ready_event() {return fire_dr_event;}
//@}

/// @privatesection
    Attr(const Attr &);
	std::string  &get_name() {return name;}
	Tango::AttrDataFormat get_format() {return format;}
	Tango::AttrWriteType get_writable() {return writable;}
	long get_type() {return type;}
	Tango::DispLevel get_disp_level() {return disp_level;}
	long get_polling_period() {return poll_period;}
	bool get_memorized() {return mem;}
	bool get_memorized_init() {return mem_init;}
	std::string	&get_assoc() {return assoc_name;}
	const std::string &get_cl_name() {return cl_name;}
	void set_cl_name(const std::string &cl) {cl_name = cl;}
	bool is_assoc() {if (assoc_name != AssocWritNotSpec)return true;else return false;}

	std::vector<AttrProperty>	&get_class_properties() {return class_properties;}
	std::vector<AttrProperty>	&get_user_default_properties() {return user_default_properties;}
	void set_class_properties(std::vector<AttrProperty> &in_prop) {class_properties=in_prop;}
	void check_type();

	virtual bool is_fwd() {return false;}

	virtual void read(DeviceImpl *,Attribute &) {};
	virtual void write(DeviceImpl *,WAttribute &) {};
	virtual bool is_allowed(DeviceImpl *,AttReqType) {return true;}

	virtual bool same_type(const std::type_info &) {return false;}
	virtual std::string get_enum_type() {return std::string("Unknown");}


protected:
/// @privatesection
	std::string					name;
	Tango::AttrDataFormat	format;
	Tango::AttrWriteType	writable;
	long					type;
	std::string					assoc_name;
	bool					mem;
	bool					mem_init;

//
// Ported from the extension class
//

    Tango::DispLevel		disp_level;			// Display  level
    long					poll_period;		// Polling period

    bool					fire_change_event;
    bool					fire_archive_event;
    bool					check_change_event;
    bool					check_archive_event;
    bool					fire_dr_event;

	std::vector<AttrProperty>	class_properties;
	std::vector<AttrProperty>	user_default_properties;

 	void convert_def_prop(const std::string &, double &);
 	void validate_def_prop(const std::string &, const char *);
 	void validate_def_change_prop(const std::string &, const char *);
 	void throw_incoherent_def_prop(const char *, const char *);
 	void throw_invalid_def_prop(const char *, const char *);

private:
    class AttrExt
    {
    public:
        AttrExt() {}
    };

    std::unique_ptr<AttrExt>     ext;           // Class extension

    std::string					cl_name;
};

/**
 * User class to create a one dimension attribute object.
 *
 * Information from this class and information fetched out from the Tango
 * database allows the Tango core software to create the Attribute object
 * for the attribute created by the user.
 *
 *
 * @headerfile tango.h
 * @ingroup Server
 */

class SpectrumAttr: public Attr
{
public:

/**@name Constructors
 * Two constructors are defined for this class */
//@{
/**
 * Constructs a newly allocated SpectrumAttr object.
 * The attribute display level is set to OPERATOR.
 * The attribute write type is set to READ
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	max_x	The attribute maximum x dimension
 *
 */
	SpectrumAttr(const char *name,long data_type,long max_x);

/**
 * Constructs a newly allocated SpectrumAttr object.
 * The attribute display level is set to OPERATOR.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	w_type  The attribute write type (READ, WRITE, READ_WRITE)
 * @param	max_x	The attribute maximum x dimension
 *
 */
	SpectrumAttr(const char *name,long data_type,Tango::AttrWriteType w_type,long max_x);

/**
 * Constructs a newly allocated SpectrumAttr object.
 * The attribute write type is set to READ
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	max_x	The attribute maximum x dimension
 * @param	level 	The attribute display type
 *
 */
	SpectrumAttr(const char *name,long data_type,long max_x,DispLevel level);

/**
 * Constructs a newly allocated SpectrumAttr object.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	w_type  The attribute write type (READ, WRITE, READ_WRITE)
 * @param	max_x	The attribute maximum x dimension
 * @param	level 	The attribute display type
 *
 */
	SpectrumAttr(const char *name,long data_type,Tango::AttrWriteType w_type,long max_x,DispLevel level);

/**
 * Constructs a newly allocated SpectrumAttr object.
 *
 * @param 	name	The attribute name
 *
 */
	SpectrumAttr(const char *name):Attr(name) {}
//@}

/**@name Destructor
 * Only one destructor is defined for this class */
//@{
/**
 * The object destructor.
 */
	~SpectrumAttr() {}
//@}

/// @privatesection
    SpectrumAttr(const SpectrumAttr &);
	long 			get_max_x() {return max_x;}

protected:
	long			max_x;

private:
    class SpectrumAttrExt
    {
    };

    std::unique_ptr<SpectrumAttrExt>     ext;           // Class extension
};


/**
 * User class to create a two dimensions attribute object.
 *
 * Information from this class and information fetched out from the Tango
 * database allows the Tango core software to create the Attribute object
 * for the attribute created by the user.
 *
 *
 * @headerfile tango.h
 * @ingroup Server
 */

class ImageAttr: public SpectrumAttr
{
public:

/**@name Constructors
 * Two constructors are defined for this class */
//@{
/**
 * Constructs a newly allocated ImageAttr object.
 * The attribute display level is set to OPERATOR.
 * The attribute write type is set to READ
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	max_x	The attribute maximum x dimension
 * @param	max_y	The attribute maximum y dimension
 *
 */
	ImageAttr(const char *name,long data_type,long max_x, long max_y);

/**
 * Constructs a newly allocated ImageAttr object.
 * The attribute display level is set to OPERATOR.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	w_type  The attribute write type (READ, WRITE, READ_WRITE)
 * @param	max_x	The attribute maximum x dimension
 * @param	max_y	The attribute maximum y dimension
 *
 */
	ImageAttr(const char *name,long data_type,Tango::AttrWriteType w_type,long max_x, long max_y);

/**
 * Constructs a newly allocated ImageAttr object.
 * The attribute write type is set to READ
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	max_x	The attribute maximum x dimension
 * @param	max_y	The attribute maximum y dimension
 * @param	level	The attribute display type
 *
 */
	ImageAttr(const char *name,long data_type,long max_x,
		  long max_y, Tango::DispLevel level);

/**
 * Constructs a newly allocated ImageAttr object.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	w_type  The attribute write type (READ, WRITE, READ_WRITE)
 * @param	max_x	The attribute maximum x dimension
 * @param	max_y	The attribute maximum y dimension
 * @param	level	The attribute display type
 *
 */
	ImageAttr(const char *name,long data_type,Tango::AttrWriteType w_type,
		  long max_x, long max_y, Tango::DispLevel level);

/**
 * Constructs a newly allocated ImageAttr object.
 *
 * @param 	name	The attribute name
 *
 */

	ImageAttr(const char *name):SpectrumAttr(name) {}
//@}

/**@name Destructor
 * Only one destructor is defined for this class */
//@{
/**
 * The object destructor.
 */
	~ImageAttr() {}
//@}

/// @privatesection
    ImageAttr(const ImageAttr &);
	long 			get_max_y() {return max_y;}

protected:
/// @privatesection
	long			max_y;

private:
    class ImageAttrExt
    {
    };

    std::unique_ptr<ImageAttrExt>    ext;           // Class extension
};

} // End of Tango namespace

#endif /* _ATTRDESC_H */
