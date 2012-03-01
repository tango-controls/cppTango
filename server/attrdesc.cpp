static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               attrdesc.cpp
//
// description :        C++ source code for the BlackBoxElt and BlackBox
//			classes. These classes are used to implement the
//			tango device server black box. There is one
//			black box for each Tango device. This black box
//			keeps info. on all the activities on a device.
//			A client is able to retrieve these data via a Device
//			attribute
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// $Revision$
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <attrdesc.h>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		Attr::Attr
//
// description : 	Constructor for the Attr class.
//			This constructor simply set the internal values
//
//--------------------------------------------------------------------------

Attr::Attr(const char *att_name,long att_type,AttrWriteType att_writable,
           const char *assoc)
:name(att_name),writable(att_writable),type(att_type),assoc_name(assoc),
mem(false),mem_init(true),ext(new AttrExt)
{
	format = Tango::SCALAR;

	ext->fire_change_event = false;
	ext->check_change_event = true;
	ext->fire_archive_event = false;
	ext->check_archive_event = true;
	ext->fire_dr_event = false;

	if (name != "State")
		check_type();

	if ((writable == Tango::WRITE) && (assoc_name != AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Associated attribute is not supported" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::Attr");
	}

	if ((writable == Tango::READ_WITH_WRITE) && (assoc_name == AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Associated attribute not defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::Attr");
	}

	if (writable == READ_WRITE)
		assoc_name = name;

}

Attr::Attr(const char *att_name,long att_type,DispLevel level,
	   AttrWriteType att_writable, const char *assoc)
:name(att_name),writable(att_writable),type(att_type),assoc_name(assoc),mem(false),
ext(new AttrExt(level))
{
	format = Tango::SCALAR;

	if (name != "State")
		check_type();

	if ((writable == Tango::WRITE) && (assoc_name != AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Associated attribute is not supported" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::Attr");
	}

	if ((writable == Tango::READ_WITH_WRITE) && (assoc_name == AssocWritNotSpec))
	{
		cout3 << "Attr::Attr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Associated attribute not defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::Attr");
	}

	if (writable == READ_WRITE)
		assoc_name = name;

}

Attr::~Attr()
{
#ifndef HAS_UNIQUE_PTR
	delete ext;
#endif
}

//+-------------------------------------------------------------------------
//
// method : 		Attr::check_type
//
// description : 	This method checks data type and throws an exception
//			in case of unsupported data type
//
//--------------------------------------------------------------------------

void Attr::check_type()
{
	bool unsuported = true;

	if (type == Tango::DEV_SHORT)
		unsuported = false;
	else if (type == Tango::DEV_LONG)
		unsuported = false;
	else if (type == Tango::DEV_LONG64)
		unsuported = false;
	else if (type == Tango::DEV_DOUBLE)
		unsuported = false;
	else if (type == Tango::DEV_STRING)
		unsuported = false;
	else if (type == Tango::DEV_FLOAT)
		unsuported = false;
	else if (type == Tango::DEV_BOOLEAN)
		unsuported = false;
	else if (type == Tango::DEV_USHORT)
		unsuported = false;
	else if (type == Tango::DEV_UCHAR)
		unsuported = false;
	else if (type == Tango::DEV_ULONG)
		unsuported = false;
	else if (type == Tango::DEV_ULONG64)
		unsuported = false;
	else if (type == Tango::DEV_STATE)
		unsuported = false;
	else if (type == Tango::DEV_ENCODED)
		unsuported = false;

	if (unsuported == true)
	{
		cout3 << "Attr::check_type throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Data type is not supported" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::check_type");
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attr::set_default_properties
//
// description : 	This method set the default user properties in the
//			Attr object. At this level, each attribute property
//			is represented by one instance of the Attrproperty
//			class.
//
//--------------------------------------------------------------------------

void Attr::set_default_properties(UserDefaultAttrProp &prop_list)
{
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
		user_default_properties.push_back(AttrProperty("min_value",prop_list.min_value));

	if (prop_list.max_value.empty() == false &&
		(TG_strcasecmp(prop_list.max_value.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.max_value.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("max_value",prop_list.max_value));

	if (prop_list.min_alarm.empty() == false &&
		(TG_strcasecmp(prop_list.min_alarm.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.min_alarm.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("min_alarm",prop_list.min_alarm));

	if (prop_list.max_alarm.empty() == false &&
		(TG_strcasecmp(prop_list.max_alarm.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.max_alarm.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("max_alarm",prop_list.max_alarm));

	if (prop_list.min_warning.empty() == false &&
		(TG_strcasecmp(prop_list.min_warning.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.min_warning.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("min_warning",prop_list.min_warning));

	if (prop_list.max_warning.empty() == false &&
		(TG_strcasecmp(prop_list.max_warning.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.max_warning.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("max_warning",prop_list.max_warning));

	if (prop_list.delta_val.empty() == false &&
		(TG_strcasecmp(prop_list.delta_val.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.delta_val.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("delta_val",prop_list.delta_val));

	if (prop_list.delta_t.empty() == false &&
		(TG_strcasecmp(prop_list.delta_t.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.delta_t.c_str(),"0") != 0) &&
		(TG_strcasecmp(prop_list.delta_t.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("delta_t",prop_list.delta_t));

	if (prop_list.abs_change.empty() == false &&
		(TG_strcasecmp(prop_list.abs_change.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.abs_change.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("abs_change",prop_list.abs_change));

	if (prop_list.rel_change.empty() == false &&
		(TG_strcasecmp(prop_list.rel_change.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.rel_change.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("rel_change",prop_list.rel_change));

	TangoSys_MemStream def_event_period;
	def_event_period << (int)(DEFAULT_EVENT_PERIOD);
	if (prop_list.period.empty() == false &&
		(TG_strcasecmp(prop_list.period.c_str(),AlrmValueNotSpec) != 0) &&
		(prop_list.period != def_event_period.str()) &&
		(TG_strcasecmp(prop_list.period.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("event_period",prop_list.period));

	if (prop_list.archive_abs_change.empty() == false &&
		(TG_strcasecmp(prop_list.archive_abs_change.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.archive_abs_change.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("archive_abs_change",prop_list.archive_abs_change));

	if (prop_list.archive_rel_change.empty() == false &&
		(TG_strcasecmp(prop_list.archive_rel_change.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.archive_rel_change.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("archive_rel_change",prop_list.archive_rel_change));

	TangoSys_MemStream def_archive_period;
	def_archive_period << (int)(INT_MAX);
	if (prop_list.archive_period.empty() == false &&
		(TG_strcasecmp(prop_list.archive_period.c_str(),AlrmValueNotSpec) != 0) &&
		(prop_list.archive_period != def_archive_period.str()) &&
		(TG_strcasecmp(prop_list.archive_period.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("archive_period",prop_list.archive_period));

}

//+-------------------------------------------------------------------------
//
// method : 		Attr::set_memorized
//
// description : 	This method set the attribute as memorized in database
//			This is allowed only for scalar attribute and for
//			writable one
//
//--------------------------------------------------------------------------

void Attr::set_memorized()
{
	if (format != Tango::SCALAR)
	{
		cout3 << "Attr::set_memorized() throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name;
		o << " is not scalar and can not be memorized" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::set_memorized");
	}

	if ((type == DEV_STATE) || (type == DEV_ENCODED))
	{
		cout3 << "Attr::set_memorized() throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name;
		o << " can not be memorized" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::set_memorized");
	}

	if ((writable == READ) || (writable == READ_WITH_WRITE))
	{
		cout3 << "Attr::set_memorized() throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name;
		o << " is not writable and therefore can not be memorized" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"Attr::set_memorized");
	}

	mem = true;
}



//+-------------------------------------------------------------------------
//
// method : 		SpectrumAttr::SpectrumAttr
//
// description : 	Constructor for the SpectrumAttr class.
//			This constructor simply set the internal values
//
//--------------------------------------------------------------------------

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,long x)
:Attr(att_name,att_type),ext(Tango_NullPtr)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum x dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"SpectrumAttr::SpectrumAttr");
	}

	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",o.str(),
							(const char *)"SpectrumAttr::SpectrumAttr");
	}
	max_x = x;
}

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,long x)
:Attr(att_name,att_type,w_type),ext(Tango_NullPtr)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum x dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"SpectrumAttr::SpectrumAttr");
	}

	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",o.str(),
							(const char *)"SpectrumAttr::SpectrumAttr");
	}
	max_x = x;
}

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,long x,DispLevel level)
:Attr(att_name,att_type,level),ext(Tango_NullPtr)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum x dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"SpectrumAttr::SpectrumAttr");
	}

	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",o.str(),
							(const char *)"SpectrumAttr::SpectrumAttr");
	}
	max_x = x;
}

SpectrumAttr::SpectrumAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,long x,DispLevel level)
:Attr(att_name,att_type,level,w_type),ext(Tango_NullPtr)
{
	format = Tango::SPECTRUM;
	if (x <= 0)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum x dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"SpectrumAttr::SpectrumAttr");
	}

	if (type == DEV_ENCODED)
	{
		cout3 << "SpectrumAttr::SpectrumAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute: " << name << ": ";
		o << "DevEncode data type allowed only for SCALAR attribute" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",o.str(),
							(const char *)"SpectrumAttr::SpectrumAttr");
	}
	max_x = x;
}

//+-------------------------------------------------------------------------
//
// method : 		ImageAttr::ImageAttr
//
// description : 	Constructor for the ImageAttr class.
//			This constructor simply set the internal values
//
//--------------------------------------------------------------------------

ImageAttr::ImageAttr(const char *att_name,long att_type,long x,long y)
:SpectrumAttr(att_name,att_type,x),ext(Tango_NullPtr)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum y dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"ImageAttr::ImageAttr");
	}
	max_y = y;
}

ImageAttr::ImageAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,
		     long x,long y)
:SpectrumAttr(att_name,att_type,w_type,x),ext(Tango_NullPtr)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum y dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"ImageAttr::ImageAttr");
	}
	max_y = y;
}

ImageAttr::ImageAttr(const char *att_name,long att_type,long x,
		     long y,DispLevel level)
:SpectrumAttr(att_name,att_type,x,level),ext(Tango_NullPtr)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum y dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"ImageAttr::ImageAttr");
	}
	max_y = y;
}

ImageAttr::ImageAttr(const char *att_name,long att_type,Tango::AttrWriteType w_type,
		     long x, long y,DispLevel level)
:SpectrumAttr(att_name,att_type,w_type,x,level),ext(Tango_NullPtr)
{
	format = Tango::IMAGE;
	if (y <= 0)
	{
		cout3 << "ImageAttr::ImageAttr throwing exception" << endl;
		TangoSys_OMemStream o;

		o << "Attribute : " << name << ": ";
		o << " Maximum y dim. wrongly defined" << ends;
		Except::throw_exception((const char *)"API_AttrWrongDefined",
				      o.str(),
				      (const char *)"ImageAttr::ImageAttr");
	}
	max_y = y;
}

} // End of Tango namespace
