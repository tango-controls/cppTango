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
// copyleft :	    European Synchrotron Radiation Facility
//			          BP 220, Grenoble 38043
//			          FRANCE
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#ifdef TANGO_HAS_LOG4TANGO

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
        << "Org: " << e.errors[i].origin.in() << ends;
    ls << msg.str();
    if (i != num_errors - 1) {
      ls << endl;
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
			ls << endl;
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
			ls << endl;
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
			ls << endl;
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
			ls << endl;
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
			ls << endl;
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
			ls << endl;
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
			ls << endl;
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
			ls << endl;
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
	
  ls << "Attribute name: " << conf.name.in() << endl;
	ls << "Attribute data_type: ";
	switch (conf.data_type)
	{
	  case Tango::DEV_SHORT :
		  ls << "Tango::DevShort" << endl;
		  break;
		  
	  case Tango::DEV_LONG :
		  ls << "Tango::DevLong" << endl; 
		  break;
		  
	  case Tango::DEV_DOUBLE :
		  ls << "Tango::DevDouble" << endl; 
		  break;
		  
	  case Tango::DEV_STRING :
		  ls << "Tango::DevString" << endl;
		  break;
	}
	ls << "Attribute data_format: ";
	switch (conf.data_format)
	{
	  case Tango::SCALAR :
		  ls << "scalar" << endl;
		  break;
		  
	  case Tango::SPECTRUM :
		  ls << "spectrum, max_dim_x: " 
            << conf.max_dim_x << endl;
		  break;
		  
	  case Tango::IMAGE :
		  ls << "image, max_dim_x: " 
            << conf.max_dim_x 
            << ", max_dim_y: " 
            << conf.max_dim_y 
            << endl;
		  break;
	}

	if (conf.writable == static_cast<unsigned char>(true))
	  ls << "Attribute is writable" << endl;
	else
	  ls << "Attribute is not writable" << endl;
	ls << "Attribute label: " << conf.label.in() << endl;
	ls << "Attribute description: " << conf.description.in() << endl;
	ls << "Attribute unit: " << conf.unit.in() << endl;
	ls << "Attribute standard unit: " << conf.standard_unit.in() << endl;
	ls << "Attribute display unit: " << conf.display_unit.in() << endl;
	ls << "Attribute format: " << conf.format.in() << endl;
	ls << "Attribute min alarm: " << conf.min_alarm.in() << endl;
	ls << "Attribute max alarm: " << conf.max_alarm.in() << endl;
	ls << "Attribute min value: " << conf.min_value.in() << endl;
	ls << "Attribute max value: " << conf.max_value.in() << endl;
	ls << "Attribute writable_attr_name: " << conf.writable_attr_name.in() << endl;
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
	ls << "Attr.Property: name:" << ap_.get_name() << " - value:" << ap_.get_value() << endl;
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LoggerStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LoggerStream& operator<< (log4tango::LoggerStream& ls, const Attr& a)
{
  vector<AttrProperty> v = (const_cast<Attr&>(a)).get_class_properties();
  unsigned int n = v.size();
  if (n) {
	  for (unsigned  i = 0; i < n; i++) {
      ls << "Attr: " 
         << const_cast<Attr&>(a).get_name()
         << " Property: name:" 
         << v[i].get_name() 
         << " - value:" 
         << v[i].get_value();
      if (i <= (n - 2)) ls << endl;
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

/*

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const Devfailed &e)
{
  static unsigned long exception_tag = 0;
  //split exception stack into several logs (with the same timestamp)
  unsigned long num_errors = e.errors.length();
  for (unsigned long i = 0; i < num_errors; i++) {
    TangoSys_OMemStream msg;
    msg << "[Ex: " << ++exception_tag << "-" << i << "] "
        << "Rsn: " << e.errors[i].reason.in() << " - "
        << "Dsc: " << e.errors[i].desc.in() << " - "
        << "Org: " << e.errors[i].origin.in() << ends;
    ls << msg.str();
    if (i != num_errors - 1) {
      ls << endl;
    }
  }
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const DevVarCharArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const DevVarShortArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const DevVarLongArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const DevVarFloatArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const DevVarDoubleArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << endl;
  }
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const DevVarUShortArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const DevVarULongArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i];
		if (i < (nb_elt - 1))
			ls << endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const DevVarStringArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		ls << "Element number [" << i << "]: " << v[i].in();
		if (i < (nb_elt - 1))
			ls << endl;
	}
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const Attribute &a)
{
	Tango::AttributeConfig conf;

	(const_cast<Attribute &>(a)).get_properties(conf);

	ls << "Attribute name: " << conf.name.in() << endl;
	ls << "Attribute data_type: ";
	switch (conf.data_type)
	{
	  case Tango::DEV_SHORT:
		  ls << "Tango::DevShort" << endl;
		  break;
	  case Tango::DEV_LONG:
		  ls << "Tango::DevLong" << endl;
		  break;
	  case Tango::DEV_DOUBLE:
		  ls << "Tango::DevDouble" << endl;
		  break;
    case Tango::DEV_STRING:
		  ls << "Tango::DevString" << endl;
		  break;
	}
	
	ls << "Attribute data_format: ";
	switch (conf.data_format)
	{
	  case Tango::SCALAR:
		  ls << "scalar" 
         << endl;
		  break;
	  case Tango::SPECTRUM:
		  ls << "spectrum, max_dim_x: " 
         << conf.max_dim_x 
         << endl;
		  break;
	  case Tango::IMAGE:
		  ls << "image, max_dim_x: " 
         << conf.max_dim_x 
         << ", max_dim_y: " 
         << conf.max_dim_y 
         << endl;
		  break;
	}

	if (conf.writable == static_cast<unsigned char>(true))
		ls << "Attribute is writable" << endl;
	else
		ls << "Attribute is not writable" << endl;

	ls << "Attribute label: " << conf.label.in() << endl;
	ls << "Attribute description: " << conf.description.in() << endl;
	ls << "Attribute unit: " << conf.unit.in() << endl;
	ls << "Attribute standard unit: " << conf.standard_unit.in() << endl;
	ls << "Attribute display unit: " << conf.display_unit.in() << endl;
	ls << "Attribute format: " << conf.format.in() << endl;
	ls << "Attribute min alarm: " << conf.min_alarm.in() << endl;
	ls << "Attribute max alarm: " << conf.max_alarm.in() << endl;
	ls << "Attribute min value: " << conf.min_value.in() << endl;
	ls << "Attribute max value: " << conf.max_value.in() << endl;	
	ls << "Attribute writable_attr_name: " << conf.writable_attr_name.in() << endl;

	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const AttrProperty &p)
{
	AttrProperty &ap = const_cast<AttrProperty &>(p);
	ls << "Attr.Property: name:" 
     << ap.get_name() 
     << " - Property value:" 
     << ap.get_value();
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const Attr& a)
{
  vector<AttrProperty> v = (const_cast<Attr&>(a)).get_class_properties();
  unsigned int n = v.size();
  if (n) {
	  for (unsigned  i = 0; i < n; i++) {
      ls << "Attr: " 
         << const_cast<Attr&>(a).get_name()
         << " Property: name:" 
         << v[i].get_name() 
         << " - value:" 
         << v[i].get_value();
      if (i <= (n - 2)) ls << endl;
	  }
  } else {
    ls << "Attr. " << const_cast<Attr&>(a).get_name() << " has no class properties";
  }
	return ls;
}

//+----------------------------------------------------------------------------
//
// method : LogStream::operator<<
// 
//-----------------------------------------------------------------------------
log4tango::LogStream& operator<< (log4tango::LogStream& ls, const AttrManip& m)
{
  ls << m.to_string();
	return ls;
}
*/

} // Tango namespace

#endif // TANGO_HAS_LOG4TANGO
