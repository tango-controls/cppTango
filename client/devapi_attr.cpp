static const char *RcsId = "$Id$\n$Name$";
//
// devapi_attrib.cpp 	- C++ source code file for TANGO devapi class DeviceAttribute
//
// programmer(s) 	- Andy Gotz (goetz@esrf.fr)
//
// original 		- February 2002
//
// log			- $Log$
// log			- Revision 3.1  2003/04/03 15:21:52  taurel
// log			- Added methods to print DeviceData, DeviceAttribute, DeviceDataHistory
// log			- and DeviceAttributeHistory instance
// log			-
// log			- Revision 3.0  2003/03/25 16:30:49  taurel
// log			- Change revision number to 3.0 before release 3.0.0 of Tango lib
// log			-
// log			- Revision 2.5  2003/03/20 08:54:54  taurel
// log			- Updated to support asynchronous calls
// log			-
// log			- Revision 2.4  2003/01/09 12:00:33  taurel
// log			- - Ported to gcc 3.2
// log			- - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// log			- - Replace some ORB * by ORB_ptr
// log			- - Use CORBA::ORB::is_nil() instead of comparing to NULL
// log			-
// log			- Revision 2.3  2002/12/16 11:58:36  taurel
// log			- - Change the underlying ORB fom ORBacus to omniORB
// log			- - New method get_device_list() in Util class
// log			- - Util::get_class_list() takes DServer device into account
// log			- - Util::get_device_by_name() takes DSErver device into account
// log			- - Util::get_device_list_by_class() takes DServer device into account
// log			- - New parameter to the attribute::set_value() method to ebnable CORBA to frre memory allocated for the attribute
// log			-
// log			- Revision 2.2  2002/10/14 09:32:43  taurel
// log			- Fix bugs in devapi_base.cpp file :
// log			- - In read_attribute and read_attributes method of the DeviceProxy class
// log			-   Do not create sequence the same way if the call is local or remote.
// log			- - Add reconnection in the Connection::set_timeout_millis method
// log			- - Add flags to the Connection::set_timeout_millis method
// log			- - Fix bug in the DeviceProxy constructor when device is not marked as exported
// log			-   in the database. The constructor was not stateless in this case.
// log			-
// log			- Revision 2.1  2002/08/12 12:43:24  taurel
// log			- Fix bug in DeviceProxy::write_attributes method when writing several
// log			- attributes in one call. (File devapi_base.cpp)
// log			-
// log			- Revision 2.0  2002/06/28 13:43:08  taurel
// log			- Lot of changes since last releases :
// log			- 	- Database object managed as a singleton per control system
// log			- 	- Support all tango device naming syntax (using TANGO_HOST env.
// log			-  	  variable, without env variable and non database device)
// log			- 	- No more copy during read_attribute and command_inout
// log			- 	- Added some missing methods
// log			- 	- Build an exception class hierarchy
// log			- 	- Added correct management of device time-out
// log			- 	- Support all Tango device interface release 2 features
// log			- 	  (data/attribute comming from polling buffer, polling related methods,
// log			- 	   command/attribute result history)
// log			-
// log			- Revision 1.6  2002/04/29 12:11:25  goetz
// log			- New change in Database::delete_device_attribute_property. The fix done in the previous release was not enough
// log			-
// log			- Revision 1.5  2002/04/29 05:43:09  goetz
// log			- Check in by ET. Fix bug (i=i+n_props+1) in Database:delete_device_attribute_property, delete_class_attribute_property and put_class_attribute_property
// log			-
// log			- Revision 1.4  2002/03/18 07:20:56  goetz
// log			- new DeviceProxy() stateless now; added inline method name()
// log			-
// log			- Revision 1.3  2002/03/01 15:48:06  goetz
// log			- added get_attribute_list() method
// log			-
// log			- Revision 1.2  2002/02/28 17:00:52  goetz
// log			- intermediate checkin
// log			-
// log			- Revision 1.1  2002/02/18 20:42:59  goetz
// log			- supports attributes, added lots of new methods, changed some old ones
// log			-
//
// version 		- $Version$
//

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
                                                      
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - default constructor to create DeviceAttribute 
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute()
{
	name = "Name not set";
	dim_x = 0;
	dim_y = 0;
	time.tv_sec = 0;
	time.tv_usec = 0;
	time.tv_nsec = 0;
	quality = Tango::ATTR_INVALID;
}

DeviceAttribute::DeviceAttribute(const DeviceAttribute & source) 
{
	name = source.name;
	exceptions_flags = source.exceptions_flags;
	dim_x = source.dim_x;
	dim_y = source.dim_y;
	quality = source.quality;
	time = source.time;

	DeviceAttribute &nc_source = const_cast<DeviceAttribute &>(source);
	if (nc_source.LongSeq.operator->() != NULL)	
		LongSeq = nc_source.LongSeq._retn();
	if (nc_source.ShortSeq.operator->() != NULL)	
		ShortSeq = nc_source.ShortSeq._retn();	
	if (nc_source.DoubleSeq.operator->() != NULL)	
		DoubleSeq = nc_source.DoubleSeq._retn();
	if (nc_source.StringSeq.operator->() != NULL)	
		StringSeq = nc_source.StringSeq._retn();
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator=() - assignement operator
//
//-----------------------------------------------------------------------------

DeviceAttribute & DeviceAttribute::operator=(const DeviceAttribute &rval)
{
	name = rval.name;
	exceptions_flags = rval.exceptions_flags;
	dim_x = rval.dim_x;
	dim_y = rval.dim_y;
	quality = rval.quality;
	time = rval.time;

	DeviceAttribute &nc_rval = const_cast<DeviceAttribute &>(rval);
	if (nc_rval.LongSeq.operator->() != NULL)	
		LongSeq = nc_rval.LongSeq._retn();
	if (nc_rval.ShortSeq.operator->() != NULL)	
		ShortSeq = nc_rval.ShortSeq._retn();	
	if (nc_rval.DoubleSeq.operator->() != NULL)	
		DoubleSeq = nc_rval.DoubleSeq._retn();
	if (nc_rval.StringSeq.operator->() != NULL)	
		StringSeq = nc_rval.StringSeq._retn();
			
	return *this;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for short
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string &new_name, short datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	ShortSeq = new(DevVarShortArray);
	ShortSeq->length(1);
	ShortSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for long
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, long datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	LongSeq  = new(DevVarLongArray);
	LongSeq->length(1);
	LongSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for double
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, double datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	DoubleSeq  = new(DevVarDoubleArray);
	DoubleSeq->length(1);
	DoubleSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for string
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, string& datum)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	StringSeq = new(DevVarStringArray);
	StringSeq->length(1);
	StringSeq[0] = string_dup(datum.c_str());
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::~DeviceAttribute() - destructor to destroy DeviceAttribute
//
//-----------------------------------------------------------------------------

DeviceAttribute::~DeviceAttribute()
{
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::is_empty() - returns true or false if datum is empty
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::is_empty()
{
	if (LongSeq.operator->() != NULL)
	{	
		if (LongSeq->length() != 0)
			return false;
	}
	
	if (ShortSeq.operator->() != NULL)
	{	
		if (ShortSeq->length() != 0)
			return false;
	}		
		
	if (DoubleSeq.operator->() != NULL)
	{	
		if (DoubleSeq->length() != 0)
			return false;
	}		
		
	if (StringSeq.operator->() != NULL)
	{	
		if (StringSeq->length() != 0)
			return false;
	}		

	if (exceptions_flags.test(isempty_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_EmptyDeviceAttribute",
					(const char*)"cannot extract, no data in DeviceAttribute object ",
					(const char*)"DeviceAttribute::is_empty");
	}		
	return true;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(short &) - extract a short from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (short &datum)
{
	bool ret = true;

	if (is_empty() == true)
		return false;
		
	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
			datum = ShortSeq[0];
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not a short",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(short) - insert a short into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (short datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	
        DevVarShortArray *short_vararr = new(DevVarShortArray);
        short_vararr->length(1);
        (*short_vararr)[0] = datum;
        ShortSeq = short_vararr;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();                                               
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(long &) - extract a long from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (long &datum)
{
	bool ret = true;
	
	if (is_empty() == true)
		return false;
	
	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
			datum = LongSeq[0];
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not a long",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(long) - insert a long into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (long datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;

        DevVarLongArray *long_vararr = new(DevVarLongArray);
        long_vararr->length(1);
        (*long_vararr)[0] = datum;
        LongSeq = long_vararr; 
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();                                               
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(double &) - extract a double from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (double &datum)
{
	bool ret = true;
	
	if (is_empty() == true)
		return false;
	
	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
			datum = DoubleSeq[0];
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not a double",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(double) - insert a double into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (double datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;

        DevVarDoubleArray *double_vararr = new(DevVarDoubleArray);
        double_vararr->length(1);
        (*double_vararr)[0] = datum;
        DoubleSeq = double_vararr;
	
	if (StringSeq.operator->() != NULL)
		delete StringSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn(); 	                                              
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(string &) - extract a string from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (string& datum)
{
	bool ret = true;
	
	if (is_empty() == true)
		return false;
	
	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
			datum = StringSeq[0];
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not a string",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(string &) - insert a string into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (string& datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;

        DevVarStringArray *string_vararr = new(DevVarStringArray);
        string_vararr->length(1);
        (*string_vararr)[0] = string_dup(datum.c_str());
        StringSeq = string_vararr;
	
	if (DoubleSeq.operator->() != NULL)
		delete DoubleSeq._retn();
	if (ShortSeq.operator->() != NULL)
		delete ShortSeq._retn();
	if (LongSeq.operator->() != NULL)
		delete LongSeq._retn();                                             
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<string> &) - extract a vector<string> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<string>& datum)
{
	bool ret = true;
	
	if (is_empty() == true)
		return false;
	
	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
		{
                	datum.resize(StringSeq->length());

                	for (unsigned int i=0; i<StringSeq->length(); i++)
                	{
                        	datum[i] = StringSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of string",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<short> &) - extract a vector<short> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<short>& datum)
{
	bool ret = true;
	
	if (is_empty() == true)
		return false;
	
	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
		{
                	datum.resize(ShortSeq->length());

                	for (unsigned int i=0; i<ShortSeq->length(); i++)
                	{
                        	datum[i] = ShortSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of short",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<long> &) - extract a vector<long> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<long>& datum)
{
	bool ret = true;
	
	if (is_empty() == true)
		return false;
	
	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
		{
                	datum.resize(LongSeq->length());

                	for (unsigned int i=0; i<LongSeq->length(); i++)
                	{
                        	datum[i] = LongSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of long",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<double> &) - extract a vector<double> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<double>& datum)
{
	bool ret = true;
	
	if (is_empty() == true)
		return false;
	
	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
		{
                	datum.resize(DoubleSeq->length());

                	for (unsigned int i=0; i<DoubleSeq->length(); i++)
                	{
                        	datum[i] = DoubleSeq[i];
               	 	}
		}			
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of double",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarShortArray *) - extract a sequence of
// short from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarShortArray* &datum)
{
	bool ret = true;
	
	if (is_empty() == true)
		return false;
	
	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
		{
			datum = ShortSeq._retn();
		}			
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of short",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarLongArray *) - extract a sequence of
// long from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarLongArray* &datum)
{
	bool ret = true;
	
	if (is_empty() == true)
		return false;
	
	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
		{
			datum = LongSeq._retn();
		}			
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of long",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarDoubleArray *) - extract a sequence of
// double from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarDoubleArray* &datum)
{
	bool ret = true;
	
	if (is_empty() == true)
		return false;
	
	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
		{
			datum = DoubleSeq._retn();
		}			
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of double",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarStringArray *) - extract a sequence of
// string from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarStringArray* &datum)
{
	bool ret = true;
	
	if (is_empty() == true)
		return false;
	
	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
		{
			datum = StringSeq._retn();
		}			
		else
			ret = false;
	}
	else
                ret = false;
		
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of string",
					(const char*)"DeviceAttribute::operator>>");
	}		
	return ret;
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			DeviceAttribute class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,DeviceAttribute &da)
{

//
// Print date
//

	if (da.time.tv_sec != 0)
	{
		char *tmp_date = asctime(localtime((time_t *)&da.time.tv_sec));
		tmp_date[strlen(tmp_date) - 1] = '\0';
		o_str << tmp_date;
		o_str << " (" << da.time.tv_sec << "," << da.time.tv_usec << " sec) : ";
	}

//
// print attribute name
//

	o_str << da.name;
	
//
// print dim_x and dim_y
//

	o_str << " (dim_x = " << da.dim_x << ", dim_y = " << da.dim_y << ", ";
	
//
// Print quality
//

	o_str << "Data quality factor = ";
	switch (da.quality)
	{
	case Tango::ATTR_VALID:
		o_str << "VALID)" << endl;
		break;
		
	case Tango::ATTR_INVALID:
		o_str << "INVALID)";
		break;
		
	case Tango::ATTR_ALARM:
		o_str << "ALARM)" << endl;
		break;
	}
	
//
// Print data (if valid)
//

	if (da.quality != Tango::ATTR_INVALID)
	{
		if (da.is_empty() == true)
			o_str << "No data in DeviceData object";
		else
		{
			if (da.LongSeq.operator->() != NULL)
				o_str << *(da.LongSeq.operator->());	
			else if (da.ShortSeq.operator->() != NULL)
				o_str << *(da.ShortSeq.operator->());	
			else if (da.DoubleSeq.operator->() != NULL)
				o_str << *(da.DoubleSeq.operator->());	
			else
				o_str << *(da.StringSeq.operator->());	
		}
	}
	
	return o_str;
}


} // End of Tango namespace
