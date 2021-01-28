//+===================================================================================================================
//
// file :               attrdesc.cpp
//
// description :        C++ source code for the Attr, SpectrumAttr and ImageAttr classes. Those classes are used
//						by DS devcelopper to define their attributes (Scalar, Spectrum and Image)
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
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
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <attrdesc.h>

namespace Tango
{

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attr::Attr
//
// description :
//		Constructor for the Attr class. This constructor simply set the internal values
//
//------------------------------------------------------------------------------------------------------------------

Attr::Attr(const char *att_name,long att_type,AttrWriteType att_writable,
           const char *assoc)
:name(att_name),writable(att_writable),type(att_type),assoc_name(assoc),
mem(false),mem_init(true),poll_period(0),fire_change_event(false),
fire_archive_event(false),check_change_event(false),check_archive_event(false),
fire_dr_event(false),ext(new AttrExt),cl_name("Attr")
{
	format = Tango::SCALAR;

	disp_level = Tango::OPERATOR;

	fire_change_event = false;
	check_change_event = true;
	fire_archive_event = false;
	check_archive_event = true;
	fire_dr_event = false;

	if (name != "State")
		check_type();

	if ((writable == Tango::WRITE) && (assoc_name != AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Associated attribute is not supported" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}

	if ((writable == Tango::READ_WITH_WRITE) && (assoc_name == AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Associated attribute not defined" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}

	if (writable == READ_WRITE)
		assoc_name = name;

}

Attr::Attr(const char *att_name,long att_type,DispLevel level,
	   AttrWriteType att_writable, const char *assoc)
:name(att_name),writable(att_writable),type(att_type),assoc_name(assoc),mem(false),
poll_period(0),ext(new AttrExt)
{
	format = Tango::SCALAR;

	disp_level = level;

	fire_change_event = false;
	check_change_event = true;
	fire_archive_event = false;
	check_archive_event = true;
	fire_dr_event = false;

	if (name != "State")
		check_type();

	if ((writable == Tango::WRITE) && (assoc_name != AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Associated attribute is not supported" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}

	if ((writable == Tango::READ_WITH_WRITE) && (assoc_name == AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Associated attribute not defined" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}

	if (writable == READ_WRITE)
		assoc_name = name;

}

Attr::Attr(const char *att_name,Tango::DispLevel disp)
:name(att_name),mem(false),disp_level(disp),poll_period(0),ext(new AttrExt)
{
	fire_change_event = false;
	check_change_event = true;
	fire_archive_event = false;
	check_archive_event = true;
	fire_dr_event = false;
}

Attr::Attr(const Attr &sou)
{
	name = sou.name;
	format = sou.format;
	writable = sou.writable;
	type = sou.type;
	assoc_name = sou.assoc_name;
	mem = sou.mem;
	mem_init = sou.mem_init;
    disp_level = sou.disp_level;
    poll_period = sou.poll_period;
    fire_change_event = sou.fire_change_event;
    fire_archive_event = sou.fire_archive_event;
    check_change_event = sou.check_change_event;
    check_archive_event = sou.check_archive_event;
    fire_dr_event = sou.fire_dr_event;
	class_properties = sou.class_properties;
	user_default_properties = sou.user_default_properties;
}

Attr::~Attr()
{
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attr::check_type
//
// description :
//		This method checks data type and throws an exception in case of unsupported data type
//
//--------------------------------------------------------------------------------------------------------------------

void Attr::check_type()
{
	bool unsuported = true;

	if (type == Tango::DEV_SHORT ||
		type == Tango::DEV_LONG ||
		type == Tango::DEV_LONG64 ||
		type == Tango::DEV_DOUBLE ||
		type == Tango::DEV_STRING ||
		type == Tango::DEV_FLOAT ||
		type == Tango::DEV_BOOLEAN ||
		type == Tango::DEV_USHORT ||
		type == Tango::DEV_UCHAR ||
		type == Tango::DEV_ULONG ||
		type == Tango::DEV_ULONG64 ||
		type == Tango::DEV_STATE ||
		type == Tango::DEV_ENCODED ||
		type == Tango::DEV_ENUM)
		unsuported = false;

	if (unsuported == true)
	{
		cout3 << "Attr::check_type throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Data type is not supported" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attr::set_default_properties
//
// description :
//		This method set the default user properties in the Attr object. At this level, each attribute property
//		is represented by one instance of the Attrproperty class.
//
// arguments :
//		in :
//			- prop_list : The user property list
//
//-------------------------------------------------------------------------------------------------------------------

void Attr::set_default_properties(UserDefaultAttrProp &prop_list)
{
	enum ranges_names
	{
		min_value,
		max_value,
		min_alarm,
		max_alarm,
		min_warning,
		max_warning,
		delta_val,
		delta_t,
		num_ranges
	};

	std::bitset<num_ranges> ranges;

	if ((prop_list.label.empty() == false) &&
		(TG_strcasecmp(prop_list.label.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.label.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("label",prop_list.label));

	if (prop_list.description.empty() == false &&
		(TG_strcasecmp(prop_list.description.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.description.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("description",prop_list.description));

	if (prop_list.unit.empty() == false &&
		(TG_strcasecmp(prop_list.unit.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.unit.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("unit",prop_list.unit));

	if (prop_list.standard_unit.empty() == false &&
		(TG_strcasecmp(prop_list.standard_unit.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.standard_unit.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("standard_unit",prop_list.standard_unit));

	if (prop_list.display_unit.empty() == false &&
		(TG_strcasecmp(prop_list.display_unit.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.display_unit.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("display_unit",prop_list.display_unit));

	if (prop_list.format.empty() == false &&
		(TG_strcasecmp(prop_list.format.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.format.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("format",prop_list.format));

	if (prop_list.min_value.empty() == false &&
		(TG_strcasecmp(prop_list.min_value.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.min_value.c_str(),NotANumber) != 0))
	{
		validate_def_prop(prop_list.min_value, "min_value");
		user_default_properties.push_back(AttrProperty("min_value",prop_list.min_value));
		ranges.set(min_value);
	}

	if (prop_list.max_value.empty() == false &&
		(TG_strcasecmp(prop_list.max_value.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.max_value.c_str(),NotANumber) != 0))
	{
		validate_def_prop(prop_list.max_value, "max_value");
		user_default_properties.push_back(AttrProperty("max_value",prop_list.max_value));
		ranges.set(max_value);
	}

	if (prop_list.min_alarm.empty() == false &&
		(TG_strcasecmp(prop_list.min_alarm.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.min_alarm.c_str(),NotANumber) != 0))
	{
		validate_def_prop(prop_list.min_alarm, "min_alarm");
		user_default_properties.push_back(AttrProperty("min_alarm",prop_list.min_alarm));
		ranges.set(min_alarm);
	}

	if (prop_list.max_alarm.empty() == false &&
		(TG_strcasecmp(prop_list.max_alarm.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.max_alarm.c_str(),NotANumber) != 0))
	{
		validate_def_prop(prop_list.max_alarm, "max_alarm");
		user_default_properties.push_back(AttrProperty("max_alarm",prop_list.max_alarm));
		ranges.set(max_alarm);
	}

	if (prop_list.min_warning.empty() == false &&
		(TG_strcasecmp(prop_list.min_warning.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.min_warning.c_str(),NotANumber) != 0))
	{
		validate_def_prop(prop_list.min_warning, "min_warning");
		user_default_properties.push_back(AttrProperty("min_warning",prop_list.min_warning));
		ranges.set(min_warning);
	}

	if (prop_list.max_warning.empty() == false &&
		(TG_strcasecmp(prop_list.max_warning.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.max_warning.c_str(),NotANumber) != 0))
	{
		validate_def_prop(prop_list.max_warning, "max_warning");
		user_default_properties.push_back(AttrProperty("max_warning",prop_list.max_warning));
		ranges.set(max_warning);
	}

	if (prop_list.delta_val.empty() == false &&
		(TG_strcasecmp(prop_list.delta_val.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.delta_val.c_str(),NotANumber) != 0))
	{
		validate_def_prop(prop_list.delta_val, "delta_val");
		user_default_properties.push_back(AttrProperty("delta_val",prop_list.delta_val));
		ranges.set(delta_val);
	}

	if (prop_list.delta_t.empty() == false &&
		(TG_strcasecmp(prop_list.delta_t.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.delta_t.c_str(),"0") != 0) &&
		(TG_strcasecmp(prop_list.delta_t.c_str(),NotANumber) != 0))
	{
		TangoSys_MemStream str;
		str.precision(TANGO_FLOAT_PRECISION);
		long lg;
		str << prop_list.delta_t;
		if(!(str >> lg && str.eof()))
			throw_invalid_def_prop("delta_t","DevLong");
		user_default_properties.push_back(AttrProperty("delta_t",prop_list.delta_t));
		ranges.set(delta_t);
	}

	if (prop_list.abs_change.empty() == false &&
		(TG_strcasecmp(prop_list.abs_change.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.abs_change.c_str(),NotANumber) != 0))
	{
		validate_def_change_prop(prop_list.abs_change,"abs_change");
		user_default_properties.push_back(AttrProperty("abs_change",prop_list.abs_change));
	}

	if (prop_list.rel_change.empty() == false &&
		(TG_strcasecmp(prop_list.rel_change.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.rel_change.c_str(),NotANumber) != 0))
	{
		validate_def_change_prop(prop_list.rel_change,"rel_change");
		user_default_properties.push_back(AttrProperty("rel_change",prop_list.rel_change));
	}

	TangoSys_MemStream def_event_period;
	def_event_period << (int)(DEFAULT_EVENT_PERIOD);
	if (prop_list.period.empty() == false &&
		(TG_strcasecmp(prop_list.period.c_str(),AlrmValueNotSpec) != 0) &&
		(prop_list.period != def_event_period.str()) &&
		(TG_strcasecmp(prop_list.period.c_str(),NotANumber) != 0))
	{
		TangoSys_MemStream str;
		str.precision(TANGO_FLOAT_PRECISION);
		int i;
		str << prop_list.period;
		if(!(str >> i && str.eof()))
			throw_invalid_def_prop("event_period","DevLong");
		user_default_properties.push_back(AttrProperty("event_period",prop_list.period));
	}

	if (prop_list.archive_abs_change.empty() == false &&
		(TG_strcasecmp(prop_list.archive_abs_change.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.archive_abs_change.c_str(),NotANumber) != 0))
	{
		validate_def_change_prop(prop_list.archive_abs_change,"archive_abs_change");
		user_default_properties.push_back(AttrProperty("archive_abs_change",prop_list.archive_abs_change));
	}

	if (prop_list.archive_rel_change.empty() == false &&
		(TG_strcasecmp(prop_list.archive_rel_change.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.archive_rel_change.c_str(),NotANumber) != 0))
	{
		validate_def_change_prop(prop_list.archive_rel_change,"archive_rel_change");
		user_default_properties.push_back(AttrProperty("archive_rel_change",prop_list.archive_rel_change));
	}

	TangoSys_MemStream def_archive_period;
	def_archive_period << (int)(INT_MAX);
	if (prop_list.archive_period.empty() == false &&
		(TG_strcasecmp(prop_list.archive_period.c_str(),AlrmValueNotSpec) != 0) &&
		(prop_list.archive_period != def_archive_period.str()) &&
		(TG_strcasecmp(prop_list.archive_period.c_str(),NotANumber) != 0))
	{
		TangoSys_MemStream str;
		str.precision(TANGO_FLOAT_PRECISION);
		int i;
		str << prop_list.archive_period;
		if(!(str >> i && str.eof()))
			throw_invalid_def_prop("archive_period","DevLong");
		user_default_properties.push_back(AttrProperty("archive_period",prop_list.archive_period));
	}

	if (prop_list.enum_labels.empty() == false)
		user_default_properties.push_back(AttrProperty("enum_labels",prop_list.enum_labels));

	if(ranges.test(min_value) && ranges.test(max_value))
	{
		double min = 0.0, max = 0.0;
		convert_def_prop(prop_list.min_value, min);
		convert_def_prop(prop_list.max_value, max);
		if(min >= max)
			throw_incoherent_def_prop("min_value","max_value");
	}

	if(ranges.test(min_alarm) && ranges.test(max_alarm))
	{
		double min = 0.0, max = 0.0;
		convert_def_prop(prop_list.min_alarm, min);
		convert_def_prop(prop_list.max_alarm, max);
		if(min >= max)
			throw_incoherent_def_prop("min_alarm","max_alarm");
	}

	if(ranges.test(min_warning) && ranges.test(max_warning))
	{
		double min = 0.0, max = 0.0;
		convert_def_prop(prop_list.min_warning, min);
		convert_def_prop(prop_list.max_warning, max);
		if(min >= max)
			throw_incoherent_def_prop("min_warning","max_warning");
	}

	if(ranges.test(delta_val) ^ ranges.test(delta_t))
	{
		std::string err_msg = "Just one of the user default properties : delta_val or delta_t is set. Both or none of the values have to be set";
		TANGO_THROW_EXCEPTION(API_IncoherentValues, err_msg);
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attr::convert_def_prop
//
// description :
//		Convert a property froma string to a double with a defined precision
//
// arguments :
//		in :
//			- val : The property as a string
//		out :
//			- db : The double data
//
//-------------------------------------------------------------------------------------------------------------------

void Attr::convert_def_prop(const std::string &val, double &db)
{
	TangoSys_MemStream str;
	str.precision(TANGO_FLOAT_PRECISION);

	str << val;
	str >> db;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attr::validate_def_prop
//
// description :
//		Validate the property string definition
//
// arguments :
//		in :
//			- val : The property as a string
//			- prop : The property name for a correct error message
//
//-------------------------------------------------------------------------------------------------------------------

void Attr::validate_def_prop(const std::string &val, const char* prop)
{
	TangoSys_MemStream str;
	str.precision(TANGO_FLOAT_PRECISION);

	str.str("");
	str.clear();
	str << val;

	DevShort sh;
	DevLong lg;
	DevLong64 lg64;
	DevDouble db;
	DevFloat fl;
	DevUShort ush;
	DevULong ulg;
	DevULong64 ulg64;
	switch (type)
	{
	case Tango::DEV_SHORT:
	case Tango::DEV_ENUM:
		if (!(str >> sh && str.eof()))
			throw_invalid_def_prop(prop,"DevShort");
		break;

	case Tango::DEV_LONG:
		if (!(str >> lg && str.eof()))
			throw_invalid_def_prop(prop,"DevLong");
		break;

	case Tango::DEV_LONG64:
		if (!(str >> lg64 && str.eof()))
			throw_invalid_def_prop(prop,"DevLong64");
		break;

	case Tango::DEV_DOUBLE:
		if (!(str >> db && str.eof()))
			throw_invalid_def_prop(prop,"DevDouble");
		break;
	case Tango::DEV_FLOAT:
		if (!(str >> fl && str.eof()))
			throw_invalid_def_prop(prop,"DevFloat");
		break;

	case Tango::DEV_USHORT:
		if (!(str >> ush && str.eof()))
			throw_invalid_def_prop(prop,"DevUShort");
		break;

	case Tango::DEV_UCHAR:
		if (!(str >> sh && str.eof()))
			throw_invalid_def_prop(prop,"DevUChar");
		break;

	case Tango::DEV_ULONG:
		if (!(str >> ulg && str.eof()))
			throw_invalid_def_prop(prop,"DevULong");
		break;

	case Tango::DEV_ULONG64:
		if (!(str >> ulg64 && str.eof()))
			throw_invalid_def_prop(prop,"DevULong64");
		break;

	case Tango::DEV_ENCODED:
		if (!(str >> sh && str.eof()))
			throw_invalid_def_prop(prop,"DevUChar");
		break;
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attr::validate_def_change_prop
//
// description :
//		Validate event change property (defined as negative delta,positive delta)
//
// arguments :
//		in :
//			- val : The property as a string
//			- prop : The property name for a correct error message
//
//-------------------------------------------------------------------------------------------------------------------

void Attr::validate_def_change_prop(const std::string &val, const char * prop)
{
	TangoSys_MemStream str;
	str.precision(TANGO_FLOAT_PRECISION);

	std::string change_prop_str = val;
	double db;

	size_t pos = change_prop_str.find(',');
	if(pos != std::string::npos)
	{
		std::string prop_min = change_prop_str.substr(0,pos);
		std::string prop_max = change_prop_str.erase(0,pos+1);
		str << prop_min;
		if (!(str >> db && str.eof()))
			throw_invalid_def_prop(prop,"DevDouble or \"DevDouble,DevDouble\"");
		str.str("");
		str.clear();
		str << prop_max;
		if (!(str >> db && str.eof()))
			throw_invalid_def_prop(prop,"DevDouble or \"DevDouble,DevDouble\"");
	}
	else
	{
		str.str("");
		str.clear();
		str << change_prop_str;
		if (!(str >> db && str.eof()))
			throw_invalid_def_prop(prop,"DevDouble or \"DevDouble,DevDouble\"");
	}
}

void Attr::throw_incoherent_def_prop(const char* min, const char* max)
{
	std::string err_msg = "User default property " + std::string(min) + " for attribute : " + get_name() + " is greater then or equal " + std::string(max);
	TANGO_THROW_EXCEPTION(API_IncoherentValues, err_msg);
}

void Attr::throw_invalid_def_prop(const char* prop, const char* type)
{
	std::string err_msg = "User default property " + std::string(prop) + " for attribute : " + get_name() + " is defined in unsupported format. Expected " + std::string(type);
	TANGO_THROW_EXCEPTION(API_IncompatibleAttrDataType, err_msg);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attr::set_memorized
//
// description :
//		This method set the attribute as memorized in database. This is allowed only for scalar attribute and for
//		writable one
//
//------------------------------------------------------------------------------------------------------------------

void Attr::set_memorized()
{
	if (format != Tango::SCALAR)
	{
		cout3 << "Attr::set_memorized() throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name;
		o << " is not scalar and can not be memorized" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}

	if ((type == DEV_STATE) || (type == DEV_ENCODED))
	{
		cout3 << "Attr::set_memorized() throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name;
		o << " can not be memorized" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}

	if ((writable == READ) || (writable == READ_WITH_WRITE))
	{
		cout3 << "Attr::set_memorized() throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name;
		o << " is not writable and therefore can not be memorized" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}

	mem = true;
}



//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		SpectrumAttr::SpectrumAttr
//
// description :
//		Constructor for the SpectrumAttr class. This constructor simply set the internal values
//
//-------------------------------------------------------------------------------------------------------------------

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,long x)
:Attr(att_name,att_type),ext(nullptr)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum x dim. wrongly defined" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}

	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}
	max_x = x;
}

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,long x)
:Attr(att_name,att_type,w_type),ext(nullptr)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum x dim. wrongly defined" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}

	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}
	max_x = x;
}

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,long x,DispLevel level)
:Attr(att_name,att_type,level),ext(nullptr)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum x dim. wrongly defined" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}

	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}
	max_x = x;
}

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,long x,DispLevel level)
:Attr(att_name,att_type,level,w_type),ext(nullptr)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum x dim. wrongly defined" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}

	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}
	max_x = x;
}

SpectrumAttr::SpectrumAttr(const SpectrumAttr &sou):Attr(sou)
{
    max_x = sou.max_x;
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		ImageAttr::ImageAttr
//
// description :
//		Constructor for the ImageAttr class. This constructor simply set the internal values
//
//-------------------------------------------------------------------------------------------------------------------

ImageAttr::ImageAttr(const char *att_name,long att_type,long x,long y)
:SpectrumAttr(att_name,att_type,x),ext(nullptr)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum y dim. wrongly defined" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}
	max_y = y;
}

ImageAttr::ImageAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,
		     long x,long y)
:SpectrumAttr(att_name,att_type,w_type,x),ext(nullptr)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum y dim. wrongly defined" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}
	max_y = y;
}

ImageAttr::ImageAttr(const char *att_name,long att_type,long x,
		     long y,DispLevel level)
:SpectrumAttr(att_name,att_type,x,level),ext(nullptr)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum y dim. wrongly defined" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}
	max_y = y;
}

ImageAttr::ImageAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,
		     long x, long y,DispLevel level)
:SpectrumAttr(att_name,att_type,w_type,x,level),ext(nullptr)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << std::endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum y dim. wrongly defined" << std::ends;
		TANGO_THROW_EXCEPTION(API_AttrWrongDefined, o.str());
	}
	max_y = y;
}

ImageAttr::ImageAttr(const ImageAttr &sou):SpectrumAttr(sou)
{
    max_y = sou.max_y;
}

} // End of Tango namespace
