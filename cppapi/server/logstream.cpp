//+=============================================================================
//
// file :		      LogStream.cpp
//
// description :  A TLS helper class
//
// project :	    TANGO
//
// author(s) :	  N.Leclercq - SOLEIL
//
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
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>


namespace Tango
{

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const DevFailed &e)
{
  //split exception stack into several logs [use a tag to identify the exception]
  static unsigned long exception_tag = 0;
  unsigned long num_errors = e.errors.length();
  for (unsigned long i = 0; i < num_errors; i++) {
    TangoSys_OMemStream msg;
    msg << "[Ex:" << exception_tag << "-Err:" << i << "] "
        << "Rsn: " << e.errors[i].reason.in() << " "
        << "Dsc: " << e.errors[i].desc.in() << " "
        << "Org: " << e.errors[i].origin.in();
    ls << msg.str();
    if (i != num_errors - 1) {
      ls << std::endl;
    }
  }
  exception_tag++;
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const DevVarCharArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << std::endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const DevVarShortArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << std::endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const DevVarLongArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << std::endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const DevVarFloatArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << std::endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const DevVarDoubleArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << std::endl;
  }
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const DevVarUShortArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << std::endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const DevVarULongArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << std::endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const DevVarStringArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i].in();
		if (i < (nb_elt - 1))
			ls << std::endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const Attribute &a)
{
	Tango::AttributeConfig conf;
	(const_cast<Attribute &>(a)).get_properties(conf);

        ls << "Attribute name: " << conf.name.in() << std::endl;
	ls << "Attribute data_type: ";
	switch (conf.data_type)
	{
	  case Tango::DEV_SHORT :
		  ls << "Tango::DevShort" << std::endl;
		  break;

	  case Tango::DEV_LONG :
		  ls << "Tango::DevLong" << std::endl;
		  break;

	  case Tango::DEV_DOUBLE :
		  ls << "Tango::DevDouble" << std::endl;
		  break;

	  case Tango::DEV_STRING :
		  ls << "Tango::DevString" << std::endl;
		  break;

	  case Tango::DEV_FLOAT :
		  ls << "Tango::DevFloat" << std::endl;
		  break;

	  case Tango::DEV_BOOLEAN :
		  ls << "Tango::DevBoolean" << std::endl;
		  break;

	  case Tango::DEV_USHORT :
		  ls << "Tango::DevUShort" << std::endl;
		  break;

	  case Tango::DEV_UCHAR :
		  ls << "Tango::DevUChar" << std::endl;
		  break;

	  case Tango::DEV_STATE :
	  	  ls << "Tango::DevState" << std::endl;
		  break;

	  case Tango::DEV_ENUM :
		  ls << "Tango::DevEnum" << std::endl;
		  break;
	}
	ls << "Attribute data_format: ";
	switch (conf.data_format)
	{
	  case Tango::FMT_UNKNOWN:
	  	  break;

	  case Tango::SCALAR :
		  ls << "scalar" << std::endl;
		  break;

	  case Tango::SPECTRUM :
		  ls << "spectrum, max_dim_x: "
            << conf.max_dim_x << std::endl;
		  break;

	  case Tango::IMAGE :
		  ls << "image, max_dim_x: "
            << conf.max_dim_x
            << ", max_dim_y: "
            << conf.max_dim_y
            << std::endl;
		  break;
	}

	if (conf.writable == static_cast<unsigned char>(true))
	  ls << "Attribute is writable" << std::endl;
	else
	  ls << "Attribute is not writable" << std::endl;
	ls << "Attribute label: " << conf.label.in() << std::endl;
	ls << "Attribute description: " << conf.description.in() << std::endl;
	ls << "Attribute unit: " << conf.unit.in() << std::endl;
	ls << "Attribute standard unit: " << conf.standard_unit.in() << std::endl;
	ls << "Attribute display unit: " << conf.display_unit.in() << std::endl;
	ls << "Attribute format: " << conf.format.in() << std::endl;
	ls << "Attribute min alarm: " << conf.min_alarm.in() << std::endl;
	ls << "Attribute max alarm: " << conf.max_alarm.in() << std::endl;
	ls << "Attribute min value: " << conf.min_value.in() << std::endl;
	ls << "Attribute max value: " << conf.max_value.in() << std::endl;
	ls << "Attribute writable_attr_name: " << conf.writable_attr_name.in() << std::endl;
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream&  operator<< (log4tango::LoggerStream& ls, const AttrProperty& ap)
{
  AttrProperty& ap_ = const_cast<AttrProperty&>(ap);
	ls << "Attr.Property: name:" << ap_.get_name() << " - value:" << ap_.get_value() << std::endl;
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const Attr& a)
{
  std::vector<AttrProperty> v = (const_cast<Attr&>(a)).get_class_properties();
  unsigned int n = v.size();
  if (n) {
	  for (unsigned  i = 0; i < n; i++) {
      ls << "Attr: "
         << const_cast<Attr&>(a).get_name()
         << " Property: name:"
         << v[i].get_name()
         << " - value:"
         << v[i].get_value();
      if (i <= (n - 2)) ls << std::endl;
	  }
  } else {
    ls << "Attr. " << const_cast<Attr&>(a).get_name() << " has no class properties";
  }
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
//
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const AttrManip& m)
{
  ls << m.to_string();
	return ls;
}

} // Tango namespace
