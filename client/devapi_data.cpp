static const char *RcsId = "$Id$\n$Name$";
//
// devapi_data.cpp 	- C++ source code file for TANGO devapi class DeviceData
//
// programmer(s) 	- Andy Gotz (goetz@esrf.fr)
//
// original 		- March 2001
//
// log			- $Log$
// log			- Revision 3.2.2.1  2004/03/02 07:40:23  taurel
// log			- - Fix compiler warnings (gcc used with -Wall)
// log			- - Fix bug in DbDatum insertion operator fro vectors
// log			- - Now support "modulo" as periodic filter
// log			-
// log			- Revision 3.2  2003/05/28 14:42:56  taurel
// log			- Add (conditionaly) autoconf generated include file
// log			-
// log			- Revision 3.1  2003/04/03 15:21:54  taurel
// log			- Added methods to print DeviceData, DeviceAttribute, DeviceDataHistory
// log			- and DeviceAttributeHistory instance
// log			-
// log			- Revision 3.0  2003/03/25 16:30:48  taurel
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
// log			- Revision 2.3  2002/12/16 11:58:37  taurel
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
// log			- Revision 1.13  2002/04/29 12:11:26  goetz
// log			- New change in Database::delete_device_attribute_property. The fix done in the previous release was not enough
// log			-
// log			- Revision 1.12  2002/04/29 05:43:10  goetz
// log			- Check in by ET. Fix bug (i=i+n_props+1) in Database:delete_device_attribute_property, delete_class_attribute_property and put_class_attribute_property
// log			-
// log			- Revision 1.11  2002/03/18 07:20:56  goetz
// log			- new DeviceProxy() stateless now; added inline method name()
// log			-
// log			- Revision 1.10  2002/03/01 15:48:06  goetz
// log			- added get_attribute_list() method
// log			-
// log			- Revision 1.9  2002/02/28 17:00:52  goetz
// log			- intermediate checkin
// log			-
// log			- Revision 1.8  2002/02/18 20:42:59  goetz
// log			- supports attributes, added lots of new methods, changed some old ones
// log			-
// log			- Revision 1.7  2002/02/11 20:16:59  goetz
// log			- added inline methods for Tango CORBA sequence types
// log			-
// log			- Revision 1.6  2001/12/04 21:34:47  goetz
// log			- fixed bug in Database::put_device_attribute_property
// log			-
// log			- Revision 1.5  2001/09/27 13:04:47  goetz
// log			- added (const char*) to all throw_exceptions; ported to dll on Windows
// log			-
// log			- Revision 1.4  2001/08/13 13:40:01  goetz
// log			- improved exception handling; ported to Windows
// log			-
// log			- Revision 1.3  2001/08/03 14:24:39  goetz
// log			- implemented reconnect; catching CORBA::Exception's and rethrowing them
// log			-
// log			- Revision 1.2  2001/08/02 14:50:12  goetz
// log			- added support for bool, DevState and DevVarCharArray types
// log			-
// log			- Revision 1.1.1.1  2001/07/18 08:42:24  goetz
// log			- Imported sources
// log			-
// log			- Revision 1.2  2001/05/28 11:50:07  goetz
// log			- intermediate checkin
// log			-
// log			- Revision 1.1.1.1  2001/04/02 12:33:35  goetz
// log			- Imported sources
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
// DeviceData::DeviceData() - constructor to create DeviceData 
//
//-----------------------------------------------------------------------------

DeviceData::DeviceData()
{
//
// For omniORB, it is necessary to do the ORB::init before creating the Any.
// Otherwise, string insertion into the Any will not be possible
//

	ApiUtil *au = ApiUtil::instance();
	if (CORBA::is_nil(au->get_orb()) == true)
		au->create_orb();
		
	any = new CORBA::Any();
}

DeviceData::DeviceData(const DeviceData & source) 
{
	exceptions_flags = source.exceptions_flags;
	any = const_cast<DeviceData &>(source).any._retn();
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator=() - assignement operator
//
//-----------------------------------------------------------------------------

DeviceData & DeviceData::operator=(const DeviceData &rval)
{
	exceptions_flags = rval.exceptions_flags;
	any = const_cast<DeviceData &>(rval).any._retn();
	return *this;
}

//-----------------------------------------------------------------------------
//
// DeviceData::~DeviceData() - destructor to destroy DeviceData
//
//-----------------------------------------------------------------------------

DeviceData::~DeviceData()
{
}

//-----------------------------------------------------------------------------
//
// DeviceData::test_any() - test any type for equality to null
//
//-----------------------------------------------------------------------------

bool DeviceData::any_is_null()
{
	CORBA::TypeCode_ptr tc;

	tc = any->type();
	if (tc->equal(CORBA::_tc_null)) 
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDeviceData",
					        (const char*)"cannot extract, no data in DeviceData object ",
					        (const char*)"DeviceData::any_is_null");
		}
		return(true);
	}
	CORBA::release(tc);

	return(false);
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(short &) - extract a short from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (bool& datum)
{
	bool ret = true;
	
	ret = any >>= CORBA::Any::to_boolean(datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
					       	(const char*)"Cannot extract, data in DeviceData object is not a boolean",
					        (const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(short &) - extract a short from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (short& datum)
{
	bool ret = true;
	
	ret = any >>= datum;
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
					       	(const char*)"Cannot extract, data in DeviceData object is not a short",
					        (const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(unsigned short &) - extract a unsigned short from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (unsigned short& datum)
{
	bool ret;
	
	ret = any >>= datum;
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
					       	(const char*)"Cannot extract, data in DeviceData object is not an unsigned short",
					        (const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(long &) - extract a long from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (long& datum)
{
	bool ret;

	ret = any >>= datum;
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
					       	(const char*)"Cannot extract, data in DeviceData object is not a long",
					        (const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(unsigned long &) - extract a unsigned short from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (unsigned long& datum)
{
	bool ret;
	
	ret = any >>= datum;
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an unsigned long",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(float &) - extract a float from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (float& datum)
{
	bool ret;
	
	ret = any >>= datum;
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				     		(const char*)"Cannot extract, data in DeviceData object is not a float",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(double &) - extract a double from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (double& datum)
{
	bool ret = true;
	
	ret = any >>= datum;
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not a double",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(string &) - extract a string from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (string& datum)
{
	bool ret;
	
	const char *c_string;
	ret = any >>= c_string;
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not a string",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	
	datum = c_string;
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(const char* &) - extract a const char* from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (const char*& datum)
{
	bool ret;
	
	ret = any >>= datum;
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of char",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevState &) - extract a DevState from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (DevState& datum)
{
	bool ret;
	
	ret = (any.inout() >>= datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not a DevState",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<unsigned char> &) - insert a vector<unsigned char> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<unsigned char>& datum)
{
	DevVarCharArray *char_array = new DevVarCharArray();
	char_array->length(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		(*char_array)[i] = datum[i];
	}
	any.inout() <<= char_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<unsigned char> &) - extract a vector<unsigned char> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<unsigned char>& datum)
{
	bool ret;
	
	const DevVarCharArray *char_array;
	ret = (any.inout() >>= char_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of char",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
		datum.resize(char_array->length());
		for (unsigned int i=0; i<char_array->length(); i++)
		{
			datum[i] = (*char_array)[i];
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarCharArray *) - extract a DevVarCharArray from DeviceData
//
//-----------------------------------------------------------------------------


bool DeviceData::operator >> (const DevVarCharArray* &datum)
{
	bool ret;
	
	ret = (any.inout() >>= datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of char",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}



//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<short> &) - insert a vector<short> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<short>& datum)
{
	DevVarShortArray *short_array = new DevVarShortArray();
	short_array->length(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		(*short_array)[i] = datum[i];
	}
	any.inout() <<= short_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<short> &) - extract a vector<short> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<short>& datum)
{
	bool ret;
	
	const DevVarShortArray *short_array;
	ret = (any.inout() >>= short_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of short",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
		datum.resize(short_array->length());
		for (unsigned int i=0; i<short_array->length(); i++)
		{
			datum[i] = (*short_array)[i];
		}		
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarShortArray *) - extract a DevVarShortArray from DeviceData
//
//-----------------------------------------------------------------------------


bool DeviceData::operator >> (const DevVarShortArray* &datum)
{
	bool ret;
	
	ret = (any.inout() >>= datum);
		
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of short",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<unsigned short> &) - insert a vector<unsigned short> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<unsigned short>& datum)
{
	DevVarUShortArray *ushort_array = new DevVarUShortArray();
	ushort_array->length(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		(*ushort_array)[i] = datum[i];
	}
	any.inout() <<= ushort_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<unsigned short> &) - extract a vector<unsigned short> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<unsigned short>& datum)
{
	bool ret = true;
	
	const DevVarUShortArray *ushort_array;
	ret = (any.inout() >>= ushort_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of unsigned short",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
		datum.resize(ushort_array->length());
		for (unsigned int i=0; i<ushort_array->length(); i++)
		{
			datum[i] = (*ushort_array)[i];
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarUShortArray *) - extract a DevVarUShortArray from DeviceData
//
//-----------------------------------------------------------------------------


bool DeviceData::operator >> (const DevVarUShortArray* &datum)
{
	bool ret = true;
	
	ret = (any.inout() >>= datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of unusigned short",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<long> &) - insert a vector<long> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<long>& datum)
{
	DevVarLongArray *long_array = new DevVarLongArray();
	long_array->length(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		(*long_array)[i] = datum[i];
	}
	any.inout() <<= long_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<long> &) - extract a vector<long> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<long>& datum)
{
	bool ret = true;
	
	const DevVarLongArray *long_array;
	ret = (any.inout() >>= long_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of long",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
		datum.resize(long_array->length());
		for (unsigned int i=0; i<long_array->length(); i++)
		{
			datum[i] = (*long_array)[i];
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarLongArray *) - extract a DevVarLongArray from DeviceData
//
//-----------------------------------------------------------------------------


bool DeviceData::operator >> (const DevVarLongArray* &datum)
{
	bool ret = true;
	
	ret = (any.inout() >>= datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of long",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<unsigned long> &) - insert a vector<unsigned long> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<unsigned long>& datum)
{
	DevVarULongArray *ulong_array = new DevVarULongArray();
	ulong_array->length(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		(*ulong_array)[i] = datum[i];
	}
	any.inout() <<= ulong_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<unsigned long> &) - extract a vector<unsigned long> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<unsigned long>& datum)
{
	bool ret = true;
	
	const DevVarULongArray *ulong_array;
	ret = (any.inout() >>= ulong_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of unsigned long",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
		datum.resize(ulong_array->length());
		for (unsigned int i=0; i<ulong_array->length(); i++)
		{
			datum[i] = (*ulong_array)[i];
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarULongArray *) - extract a DevVarULongArray from DeviceData
//
//-----------------------------------------------------------------------------


bool DeviceData::operator >> (const DevVarULongArray* &datum)
{
	bool ret = true;
	
	ret = (any.inout() >>= datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of unsigned long",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<float> &) - insert a vector<float> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<float>& datum)
{
	DevVarFloatArray *float_array = new DevVarFloatArray();
	float_array->length(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		(*float_array)[i] = datum[i];
	}
	any.inout() <<= float_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<float> &) - extract a vector<float> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<float>& datum)
{
	bool ret = true;
	
	const DevVarFloatArray *float_array;
	ret = (any.inout() >>= float_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of float",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
		datum.resize(float_array->length());
		for (unsigned int i=0; i<float_array->length(); i++)
		{
			datum[i] = (*float_array)[i];
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarFloatArray *) - extract a DevVarFloatArray from DeviceData
//
//-----------------------------------------------------------------------------


bool DeviceData::operator >> (const DevVarFloatArray* &datum)
{
	bool ret = true;
	
	ret = (any.inout() >>= datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of float",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<double> &) - insert a vector<double> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<double>& datum)
{
	DevVarDoubleArray *double_array = new DevVarDoubleArray();
	double_array->length(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		(*double_array)[i] = datum[i];
	}
	any.inout() <<= double_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<double> &) - extract a vector<double> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<double>& datum)
{
	bool ret = true;
	const DevVarDoubleArray *double_array;
	
	ret = (any.inout() >>= double_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of double",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
		datum.resize(double_array->length());
		for (unsigned int i=0; i<double_array->length(); i++)
		{
			datum[i] = (*double_array)[i];
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarDoubleArray *) - extract a DevVarDoubleArray from DeviceData
//
//-----------------------------------------------------------------------------


bool DeviceData::operator >> (const DevVarDoubleArray* &datum)
{
	bool ret;
	
	ret = (any.inout() >>= datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of double",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<string> &) - insert a vector<string> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<string>& datum)
{	
	DevVarStringArray *string_array = new DevVarStringArray();
	string_array->length(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		(*string_array)[i] = string_dup(datum[i].c_str());
	}
	any.inout() <<= string_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<string> &) - extract a vector<string> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<string>& datum)
{
	bool ret;
	const DevVarStringArray *string_array;
	
	ret = (any.inout() >>= string_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of string",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
		datum.resize(string_array->length());
		for (unsigned int i=0; i<string_array->length(); i++)
		{
			datum[i] = (*string_array)[i];
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarStringArray *) - extract a DevVarStringArray from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (const DevVarStringArray* &datum)
{
	bool ret = true;
	ret = (any.inout() >>= datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of string",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::insert (vector<long>, vector<string> &) - insert a pair of
//             vector<long>,vector<string> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::insert (vector<long> &long_datum, vector<string>& string_datum)
{
	unsigned int i;

	DevVarLongStringArray *long_string_array = new DevVarLongStringArray();
	long_string_array->lvalue.length(long_datum.size());
	for (i=0; i<long_datum.size(); i++)
	{
		(long_string_array->lvalue)[i] = long_datum[i];
	}
	long_string_array->svalue.length(string_datum.size());
	for (i=0; i<string_datum.size(); i++)
	{
		(long_string_array->svalue)[i] = string_dup(string_datum[i].c_str());
	}
	any.inout() <<= long_string_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::extract (vector<long>, vector<string> &) - extract a pair of
//             vector<long>,vector<string> into DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::extract (vector<long> &long_datum, vector<string>& string_datum)
{
	bool ret = true;
	unsigned int i;
	const DevVarLongStringArray *long_string_array; 
	ret = (any.inout() >>= long_string_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not a boolean",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
		long_datum.resize(long_string_array->lvalue.length());
		for (i=0; i<long_datum.size(); i++)
		{
			long_datum[i] = (long_string_array->lvalue)[i];
		}
		string_datum.resize(long_string_array->svalue.length());
		for (i=0; i<string_datum.size(); i++)
		{
			string_datum[i] = (long_string_array->svalue)[i];
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarLongStringArray *) - extract a DevVarLongStringArray from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (const DevVarLongStringArray* &datum)
{
	bool ret = true;
	ret = (any.inout() >>= datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not a structure with sequences of string(s) and long(s) ",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::insert (vector<double>, vector<string> &) - insert a pair of
//             vector<double>,vector<string> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::insert (vector<double> &double_datum, vector<string>& string_datum)
{
	unsigned int i;

	DevVarDoubleStringArray *double_string_array = new DevVarDoubleStringArray();
	double_string_array->dvalue.length(double_datum.size());
	for (i=0; i<double_datum.size(); i++)
	{
		(double_string_array->dvalue)[i] = double_datum[i];
	}
	double_string_array->svalue.length(string_datum.size());
	for (i=0; i<string_datum.size(); i++)
	{
		(double_string_array->svalue)[i] = string_dup(string_datum[i].c_str());
	}
	any.inout() <<= double_string_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::extract (vector<double>, vector<string> &) - extract a pair of
//             vector<double>,vector<string> into DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::extract (vector<double> &double_datum, vector<string>& string_datum)
{
	bool ret;
	unsigned int i;
	const DevVarDoubleStringArray *double_string_array; 
	ret = (any.inout() >>= double_string_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not a boolean",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
		double_datum.resize(double_string_array->dvalue.length());
		for (i=0; i<double_datum.size(); i++)
		{
			double_datum[i] = (double_string_array->dvalue)[i];
		}
		string_datum.resize(double_string_array->svalue.length());
		for (i=0; i<string_datum.size(); i++)
		{
			string_datum[i] = (double_string_array->svalue)[i];
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarDoubleStringArray *) - extract a DevVarDoubleStringArray from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (const DevVarDoubleStringArray* &datum)
{
	bool ret = true;
	ret = (any.inout() >>= datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not a structure with sequences of string(s) and double(s) ",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			DeviceData class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,DeviceData &dd)
{
	if (dd.any_is_null() == true)
		o_str << "No data in DeviceData object";
	else
	{
		CORBA::TypeCode_ptr tc;
		CORBA::TypeCode_var tc_al;
		CORBA::TypeCode_var tc_seq;
		CORBA::TypeCode_var tc_field;
		
		tc = dd.any->type();
		switch(tc->kind()) 
		{
		case CORBA::tk_boolean:
			bool bo_tmp;
			dd.any >>= CORBA::Any::to_boolean(bo_tmp);
			if (bo_tmp == true)
				o_str << "true" ;
			else
				o_str << "false" ;
			break;
			
		case CORBA::tk_short:
			short tmp;
			dd.any >>= tmp;
			o_str << tmp;
			break;
			
		case CORBA::tk_long:
			long l_tmp;
			dd.any >>= l_tmp;
			o_str << l_tmp;
			break;
			
		case CORBA::tk_float:
			float f_tmp;
			dd.any >>= f_tmp;
			o_str << f_tmp;
			break;
			
		case CORBA::tk_double:
			double db_tmp;
			dd.any >>= db_tmp;
			o_str << db_tmp;
			break;
			
		case CORBA::tk_ushort:
			unsigned short us_tmp;
			dd.any >>= us_tmp;
			o_str << us_tmp;
			break;
			
		case CORBA::tk_ulong:
			unsigned long ul_tmp;
			dd.any >>= ul_tmp;
			o_str << ul_tmp;
			break;
			
		case CORBA::tk_string:
			char *str_tmp;
			dd.any >>= str_tmp;
			o_str << str_tmp;
			break;
			
		case CORBA::tk_alias:
			tc_al = tc->content_type();
			tc_seq = tc_al->content_type();
			switch (tc_seq->kind())
			{
			case CORBA::tk_octet:
				Tango::DevVarCharArray *ch_arr;
				dd.any.inout() >>= ch_arr;
				o_str << *ch_arr;
				break;
				
			case CORBA::tk_short:
				Tango::DevVarShortArray *sh_arr;
				dd.any.inout() >>= sh_arr;
				o_str << *sh_arr;
				break;
				
			case CORBA::tk_long:
				Tango::DevVarLongArray *lg_arr;
				dd.any.inout() >>= lg_arr;
				o_str << *lg_arr;
				break;
				
			case CORBA::tk_float:
				Tango::DevVarFloatArray *fl_arr;
				dd.any.inout() >>= fl_arr;
				o_str << *fl_arr;
				break;
				
			case CORBA::tk_double:
				Tango::DevVarDoubleArray *db_arr;
				dd.any.inout() >>= db_arr;
				o_str << *db_arr;
				break;
				
			case CORBA::tk_ushort:
				Tango::DevVarUShortArray *us_arr;
				dd.any.inout() >>= us_arr;
				o_str << *us_arr;
				break;
				
			case CORBA::tk_ulong:
				Tango::DevVarULongArray *ul_arr;
				dd.any.inout() >>= ul_arr;
				o_str << *ul_arr;
				break;
				
			case CORBA::tk_string:
				Tango::DevVarStringArray *str_arr;
				dd.any.inout() >>= str_arr;
				o_str << *str_arr;
				break;
			}
			break;
			
		case CORBA::tk_struct:
			tc_field = tc->member_type(0);
			tc_al = tc_field->content_type();
			tc_seq = tc_al->content_type();
			switch (tc_seq->kind())
			{
			case CORBA::tk_long:
				Tango::DevVarLongStringArray *lgstr_arr;
				dd.any.inout() >>= lgstr_arr;
				o_str << lgstr_arr->lvalue << endl;
				o_str << lgstr_arr->svalue;
				break;
				
			case CORBA::tk_double:
				Tango::DevVarDoubleStringArray *dbstr_arr;
				dd.any.inout() >>= dbstr_arr;
				o_str << dbstr_arr->dvalue << endl;
				o_str << dbstr_arr->svalue;
				break;
			}
			break;
			
		case CORBA::tk_enum:
			Tango::DevState tmp_state;
			dd.any.inout() >>= tmp_state;
			o_str << Tango::DevStateName[tmp_state];
			break;
			
		}
		
		CORBA::release(tc);
	}
	
	return o_str;
}


} // End of Tango namepsace
