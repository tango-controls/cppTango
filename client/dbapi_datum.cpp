static const char *RcsId = "$Id$\n$Name$";
//
// dbdatum.cpp 	- C++ source code file for TANGO dbapi class DbDatum
//
// programmer 	- Andy Gotz (goetz@esrf.fr)
//
// original 	- October 2000
//
// Copyright (C) :      2000,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010
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
// $Log$
// Revision 3.13  2009/11/02 08:35:03  taurel
// - Fix warnings reported when compiling using the option -Wall
//
// Revision 3.12  2009/08/27 07:22:43  taurel
// - Commit after anothre merge with Release_7_0_2-bugfixes branch
//
// Revision 3.11  2009/06/17 08:52:49  taurel
// - Commit after a merge with branch Release_7_0_2-bugfixes
//
// Revision 3.10.2.1  2009/06/12 09:24:18  taurel
// - Fix some typos in Exception reason field
//
// Revision 3.10  2009/04/20 13:25:50  taurel
// - Fix bug in case of default constructed DeviceProxy and alias() method
// - Add some ctors from "const char *" to make programmer's life easier
//
// Revision 3.9  2009/03/13 09:32:27  taurel
// - Small changes to fix Windows VC8 warnings in Warning level 3
//
// Revision 3.8  2009/01/21 12:45:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.7  2008/10/06 15:02:16  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.6  2008/10/02 16:09:25  taurel
// - Add some licensing information in each files...
//
// Revision 3.5  2007/11/08 12:00:20  taurel
// - Add miscellaneous getter methods in the DbServerCache class
// - The DbDatum::Size() method is now inline
// - Fix bug in database re-connection timeout management
//
// Revision 3.4  2007/03/06 08:20:45  taurel
// - Added 64 bits data types for 64 bits computer...
//
// Revision 3.3  2005/01/13 08:36:36  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.2.2.5  2004/12/07 17:24:02  taurel
// - Fix bug when inserting vectors into a DbDatum for Suse 7.2 (old stream)
//
// Revision 3.2.2.4  2004/11/09 09:55:56  taurel
// - Fix one memory leak and some minor changes
//
// Revision 3.2.2.3  2004/10/27 05:58:10  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.2.2.2  2004/10/22 11:23:16  taurel
// Added warning alarm
// Change attribute config. It now includes alarm and event parameters
// Array attribute property now supported
// subscribe_event throws exception for change event if they are not correctly configured
// Change in the polling thread: The event heartbeat has its own work in the work list
// Also add some event_unregister
// Fix order in which classes are destructed
// Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
// Change in the CORBA info call for the device type
//
// Revision 3.2.2.1  2004/09/15 06:44:43  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.2  2004/07/07 08:39:56  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.1.2.2  2004/03/02 07:40:23  taurel
// - Fix compiler warnings (gcc used with -Wall)
// - Fix bug in DbDatum insertion operator fro vectors
// - Now support "modulo" as periodic filter
//
// Revision 3.1.2.1  2004/01/20 08:30:07  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.1  2003/05/28 14:42:56  taurel
// Add (conditionaly) autoconf generated include file
//
// Revision 3.0  2003/03/25 16:30:41  taurel
// Change revision number to 3.0 before release 3.0.0 of Tango lib
//
// Revision 2.5  2003/03/20 08:54:53  taurel
// Updated to support asynchronous calls
//
// Revision 2.4  2003/01/09 12:00:33  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.3  2002/12/16 11:58:36  taurel
// - Change the underlying ORB fom ORBacus to omniORB
// - New method get_device_list() in Util class
// - Util::get_class_list() takes DServer device into account
// - Util::get_device_by_name() takes DSErver device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to ebnable CORBA to frre memory allocated for the attribute
//
// Revision 2.2  2002/10/14 09:32:43  taurel
// Fix bugs in devapi_base.cpp file :
// - In read_attribute and read_attributes method of the DeviceProxy class
//   Do not create sequence the same way if the call is local or remote.
// - Add reconnection in the Connection::set_timeout_millis method
// - Add flags to the Connection::set_timeout_millis method
// - Fix bug in the DeviceProxy constructor when device is not marked as exported
//   in the database. The constructor was not stateless in this case.
//
// Revision 2.1  2002/08/12 12:43:23  taurel
// Fix bug in DeviceProxy::write_attributes method when writing several
// attributes in one call. (File devapi_base.cpp)
//
// Revision 2.0  2002/06/28 13:43:07  taurel
// Lot of changes since last releases :
// 	- Database object managed as a singleton per control system
// 	- Support all tango device naming syntax (using TANGO_HOST env.
//  	  variable, without env variable and non database device)
// 	- No more copy during read_attribute and command_inout
// 	- Added some missing methods
// 	- Build an exception class hierarchy
// 	- Added correct management of device time-out
// 	- Support all Tango device interface release 2 features
// 	  (data/attribute comming from polling buffer, polling related methods,
// 	   command/attribute result history)
//
// Revision 1.13  2002/04/29 12:11:25  goetz
// New change in Database::delete_device_attribute_property. The fix done in the previous release was not enough
//
// Revision 1.12  2002/04/29 05:43:09  goetz
// Check in by ET. Fix bug (i=i+n_props+1) in Database:delete_device_attribute_property, delete_class_attribute_property and put_class_attribute_property
//
// Revision 1.11  2002/03/18 07:20:56  goetz
// new DeviceProxy() stateless now; added inline method name()
//
// Revision 1.10  2002/03/01 15:48:06  goetz
// added get_attribute_list() method
//
// Revision 1.9  2002/02/28 17:00:52  goetz
// intermediate checkin
//
// Revision 1.8  2002/02/18 20:42:59  goetz
// supports attributes, added lots of new methods, changed some old ones
//
// Revision 1.7  2002/02/11 20:16:58  goetz
// added inline methods for Tango CORBA sequence types
//
// Revision 1.6  2001/12/04 21:34:46  goetz
// fixed bug in Database::put_device_attribute_property
//
// Revision 1.5  2001/09/27 13:04:47  goetz
// added (const char*) to all throw_exceptions; ported to dll on Windows
//
// Revision 1.4  2001/08/13 13:40:00  goetz
// improved exception handling; ported to Windows
//
// Revision 1.3  2001/08/03 14:24:38  goetz
// implemented reconnect; catching CORBA::Exception's and rethrowing them
//
// Revision 1.2  2001/08/02 14:50:12  goetz
// added support for bool, DevState and DevVarCharArray types
//
// Revision 1.1.1.1  2001/07/18 08:42:24  goetz
// Imported sources
//
// Revision 1.12  2001/06/08 12:55:06  goetz
// fixed bug with extracting vectors under Windows
//
//

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>


#ifdef STRSTREAM
#include <iomanip.h>
#else
#include <iomanip>
#endif
                                                      
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// DbDatum::DbDatum() - constructor to create DbDatum specifying name
//
//-----------------------------------------------------------------------------

DbDatum::DbDatum(string p_name):ext(NULL)
{
	name = p_name;
	value_size = 0;
	value_string.resize(0);
}

DbDatum::DbDatum(const char *p_name):name(p_name),ext(NULL)
{
	value_size = 0;
	value_string.resize(0);
}

//-----------------------------------------------------------------------------
//
// DbDatum::DbDatum() - constructor to create DbDatum without arguments
//
//-----------------------------------------------------------------------------

DbDatum::DbDatum():ext(NULL)
{
}

//-----------------------------------------------------------------------------
//
// DbDatum::~DbDatum() - destructor to destroy DbDatum
//
//-----------------------------------------------------------------------------

DbDatum::~DbDatum()
{
}

//-----------------------------------------------------------------------------
//
// DbDatum::DbDatum() - Copy constructor
//
//-----------------------------------------------------------------------------

DbDatum::DbDatum(const DbDatum &source) 
{
	name = source.name;
	value_string = source.value_string;
	value_type = source.value_type;
	value_size = source.value_size;
	exceptions_flags = source.exceptions_flags;
	ext = source.ext;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator() - assignement operator
//
//-----------------------------------------------------------------------------

DbDatum &DbDatum::operator=(const DbDatum &rval)
{
	name = rval.name;
	value_string = rval.value_string;
	value_type = rval.value_type;
	value_size = rval.value_size;
	exceptions_flags = rval.exceptions_flags;
	ext = rval.ext;

	return *this;
}

//-----------------------------------------------------------------------------
//
// DbDatum::is_empty() - returns true or false if datum is empty
//
//-----------------------------------------------------------------------------

bool DbDatum::is_empty()
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"The DbDatum object is empty",
					     (const char*)"DbDatum::is_empty");
		}
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(bool &) - insert a boolean into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (bool datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
	if (datum == true)
		ostream << "true";
	else
		ostream << "false";
	ostream << ends;
#else
	ostringstream ostream;
	ostream << boolalpha << datum;
#endif /* STRSTREAM */
	value_string.resize(1);
	value_string[0] = string(ostream.str());
#ifdef STRSTREAM
	delete [] ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_BOOLEAN;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(bool &) - extract a boolean from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (bool &datum)
{
	bool ret;
	
	if (value_string.size() == 0) 
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"Cannot extract short, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(short)");
		}
		ret = false;
	}
	else
	{

		transform(value_string[0].begin(),
			  value_string[0].end(),
			  value_string[0].begin(),
			  ::tolower);
			  
#ifdef STRSTREAM
		ret = true;
		if (value_string[0] == "true")
			datum = true;
		else if (value_string[0] == "false")
			datum = false;
		else
		{
			if (exceptions_flags.test(wrongtype_flag))
			{
				ApiDataExcept::throw_exception((const char *)"API_IncompatibleArgumentType",
							       (const char *)"Cannot extract, data in DbDatum is not a short",
							       (const char *)"DbDatum::operator >>(short)");
			}
			ret = false;
		}
#else
		istringstream istream(value_string[0]);
		istream >> boolalpha >> datum;
		if (!istream)
		{
			if (exceptions_flags.test(wrongtype_flag))
			{
				ApiDataExcept::throw_exception((const char *)"API_IncompatibleArgumentType",
							       (const char *)"Cannot extract, data in DbDatum is not a short",
							       (const char *)"DbDatum::operator >>(short)");
			}
			ret = false;
		}
		else
			ret = true;
#endif /* STRSTREAM */
	}
	
	return ret;
}


//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(short &) - insert a short into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (short datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	ostream << datum;
#ifdef STRSTREAM
	ostream << ends;
#endif /* STRSTREAM */
	value_string.resize(1);
	value_string[0] = string(ostream.str());
#ifdef STRSTREAM
	delete [] ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_SHORT;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(short &) - extract a short from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (short &datum)
{
	bool ret;
	
	if (value_string.size() == 0) 
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"Cannot extract short, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(short)");
		}
		ret = false;
	}
	else
	{

#ifdef STRSTREAM
		istrstream istream(value_string[0].c_str());
#else
		istringstream istream(value_string[0]);
#endif /* STRSTREAM */
		istream >> datum;
		if (!istream)
		{
			if (exceptions_flags.test(wrongtype_flag))
			{
				ApiDataExcept::throw_exception((const char *)"API_IncompatibleArgumentType",
							       (const char *)"Cannot extract, data in DbDatum is not a short",
							       (const char *)"DbDatum::operator >>(short)");
			}
			ret = false;
		}
		else
			ret = true;
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(unsigned char &) - insert a unsigned char into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (unsigned char datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	ostream << datum;
#ifdef STRSTREAM
	ostream << ends;
#endif /* STRSTREAM */
	value_string.resize(1);
	value_string[0] = string(ostream.str());
#ifdef STRSTREAM
	delete [] ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_UCHAR;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(unsigned char &) - extract a unsigned char from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (unsigned char& datum)
{
	bool ret;
	if (value_string.size() == 0) 
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract unsigned short, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(unsigned short)");
		}
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		istrstream istream(value_string[0].c_str());
#else
		istringstream istream(value_string[0]);
#endif /* STRSTREAM */
		istream >> datum;
		if (!istream)
		{
			if (exceptions_flags.test(wrongtype_flag))
			{
				ApiDataExcept::throw_exception((const char *)"API_IncompatibleArgumentType",
							       (const char *)"Cannot extract, data in DbDatum is not an unsigned short",
							       (const char *)"DbDatum::operator >>(unsigned short)");
			}
			ret = false;
		}
		else
			ret = true;
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(unsigned short &) - insert a unsigned short into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (unsigned short datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	ostream << datum;
#ifdef STRSTREAM
	ostream << ends;
#endif /* STRSTREAM */
	value_string.resize(1);
	value_string[0] = string(ostream.str());
#ifdef STRSTREAM
	delete [] ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_USHORT;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(unsigned short &) - extract a unsigned short from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (unsigned short& datum)
{
	bool ret;
	if (value_string.size() == 0) 
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract unsigned short, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(unsigned short)");
		}
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		istrstream istream(value_string[0].c_str());
#else
		istringstream istream(value_string[0]);
#endif /* STRSTREAM */
		istream >> datum;
		if (!istream)
		{
			if (exceptions_flags.test(wrongtype_flag))
			{
				ApiDataExcept::throw_exception((const char *)"API_IncompatibleArgumentType",
							       (const char *)"Cannot extract, data in DbDatum is not an unsigned short",
							       (const char *)"DbDatum::operator >>(unsigned short)");
			}
			ret = false;
		}
		else
			ret = true;
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(DevLong &) - insert a DevLong into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (DevLong datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	ostream << datum;
#ifdef STRSTREAM
	ostream << ends;
#endif /* STRSTREAM */
	value_string.resize(1);
	value_string[0] = string(ostream.str());
#ifdef STRSTREAM
	delete [] ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_LONG;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(DevLong &) - extract a long from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (DevLong& datum)
{
	bool ret;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDbDatum",
					     (const char*)"cannot extract long, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(long)");
		}
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		istrstream istream(value_string[0].c_str());
#else
		istringstream istream(value_string[0]);
#endif /* STRSTREAM */
		istream >> datum;
		if (!istream)
		{
			if (exceptions_flags.test(wrongtype_flag))
			{
				ApiDataExcept::throw_exception((const char *)"API_IncompatibleArgumentType",
							       (const char *)"Cannot extract, data in DbDatum is not a DevLong (long 32 bits)",
							       (const char *)"DbDatum::operator >>(DevLong)");
			}
			ret = false;
		}
		else
			ret = true;
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(DevULong &) - insert a unsigned short into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (DevULong datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	ostream << datum;
#ifdef STRSTREAM
	ostream << ends;
#endif /* STRSTREAM */
	value_string.resize(1);
	value_string[0] = string(ostream.str());
#ifdef STRSTREAM
	delete [] ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_ULONG;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(DevULong &) - extract a DevULong from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (DevULong& datum)
{
	bool ret;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract unsigned long, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(unsigned long)");
		}
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		istrstream istream(value_string[0].c_str());
#else
		istringstream istream(value_string[0]);
#endif /* STRSTREAM */
		istream >> datum;
		if (!istream)
		{
			if (exceptions_flags.test(wrongtype_flag))
			{
				ApiDataExcept::throw_exception((const char *)"API_IncompatibleArgumentType",
							       (const char *)"Cannot extract, data in DbDatum is not a DevULong (unsigned long 32 bits)",
							       (const char *)"DbDatum::operator >>(DevULong)");
			}
			ret = false;
		}
		else
			ret = true;
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(DevLong64 &) - insert a DevLong64 into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (DevLong64 datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	ostream << datum;
#ifdef STRSTREAM
	ostream << ends;
#endif /* STRSTREAM */
	value_string.resize(1);
	value_string[0] = string(ostream.str());
#ifdef STRSTREAM
	delete [] ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_LONG64;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(DevLong64 &) - extract a DevLong64 from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (DevLong64 &datum)
{
	bool ret;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract unsigned long, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(DevLong64)");
		}
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		istrstream istream(value_string[0].c_str());
#else
		istringstream istream(value_string[0]);
#endif /* STRSTREAM */
		istream >> datum;
		if (!istream)
		{
			if (exceptions_flags.test(wrongtype_flag))
			{
				ApiDataExcept::throw_exception((const char *)"API_IncompatibleArgumentType",
							       (const char *)"Cannot extract, data in DbDatum is not a DevLong64 (long 64 bits)",
							       (const char *)"DbDatum::operator >>(DevULong)");
			}
			ret = false;
		}
		else
			ret = true;
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(DevULong64 &) - insert a DevULong64 into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (DevULong64 datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	ostream << datum;
#ifdef STRSTREAM
	ostream << ends;
#endif /* STRSTREAM */
	value_string.resize(1);
	value_string[0] = string(ostream.str());
#ifdef STRSTREAM
	delete [] ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_ULONG64;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(DevULong64 &) - extract a DevULong64 from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (DevULong64 &datum)
{
	bool ret;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract unsigned long, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(DevLong64)");
		}
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		istrstream istream(value_string[0].c_str());
#else
		istringstream istream(value_string[0]);
#endif /* STRSTREAM */
		istream >> datum;
		if (!istream)
		{
			if (exceptions_flags.test(wrongtype_flag))
			{
				ApiDataExcept::throw_exception((const char *)"API_IncompatibleArgumentType",
							       (const char *)"Cannot extract, data in DbDatum is not a DevULong64 (unsigned long 64 bits)",
							       (const char *)"DbDatum::operator >>(DevULong)");
			}
			ret = false;
		}
		else
			ret = true;
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(float &) - insert a float into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (float datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	ostream << datum;
#ifdef STRSTREAM
	ostream << ends;
#endif /* STRSTREAM */
	value_string.resize(1);
	value_string[0] = string(ostream.str());
#ifdef STRSTREAM
	delete [] ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_FLOAT;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(float &) - extract a float from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (float& datum)
{
	bool ret;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDbDatum",
					     (const char*)"cannot extract float, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(float)");
		}
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		istrstream istream(value_string[0].c_str());
#else
		istringstream istream(value_string[0]);
#endif /* STRSTREAM */
		istream >> datum;
		if (!istream)
		{
			if (exceptions_flags.test(wrongtype_flag))
			{
				ApiDataExcept::throw_exception((const char *)"API_IncompatibleArgumentType",
							       (const char *)"Cannot extract, data in DbDatum is not a float",
							       (const char *)"DbDatum::operator >>(float)");
			}
			ret = false;
		}
		else
			ret = true;
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(double &) - insert a double into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (double datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
#ifndef _HPUX_SOURCE
	ostream << std::setprecision(15) << datum;
#ifdef STRSTREAM
	ostream << ends;
#endif /* STRSTREAM */ 
#else
	ostream << setprecision(15) << datum;
#ifdef STRSTREAM
	ostream << ends;
#endif /* STRSTREAM */
#endif /* _HPUX_SOURCE */
	value_string.resize(1);
	value_string[0] = string(ostream.str());
#ifdef STRSTREAM
	delete [] ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_DOUBLE;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(double &) - extract a double from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (double& datum)
{
	bool ret;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract double, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(double)");
		}
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		istrstream istream(value_string[0].c_str());
#else
		istringstream istream(value_string[0]);
#endif /* STRSTREAM */
		istream >> std::setprecision(15) >> datum;
		if (!istream)
		{
			if (exceptions_flags.test(wrongtype_flag))
			{
				ApiDataExcept::throw_exception((const char *)"API_IncompatibleArgumentType",
							       (const char *)"Cannot extract, data in DbDatum is not a double",
							       (const char *)"DbDatum::operator >>(double)");
			}
			ret = false;
		}
		else
			ret = true;
	}
	
	return ret;

}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(string &) - insert a string into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (string& datum)
{
	value_string.resize(1);
	value_string[0] = datum;
	value_type = DEV_STRING;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(string &) - extract a string from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (string& datum)
{
	bool ret;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract string, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(string)");
		}
		ret = false;
	}
	else
	{
		datum = value_string[0];
		ret = true;
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(char* &) - insert a char* into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (char* datum)
{
	value_string.resize(1);
	value_string[0] = datum;
	value_type = DEV_STRING;
	value_size = 1;
}

/*void DbDatum::operator << (char*& datum)
{
	value_string.resize(1);
	value_string[0] = datum;
	value_type = DEV_STRING;
	value_size = 1;
}*/

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(const char* &) - insert a const char* into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (const char* datum)
{
	value_string.resize(1);
	value_string[0] = datum;
	value_type = DEV_STRING;
	value_size = 1;
}

/*void DbDatum::operator << (const char*& datum)
{
	value_string.resize(1);
	value_string[0] = datum;
	value_type = DEV_STRING;
	value_size = 1;
}*/

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(const char* &) - extract a const char* from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (const char*& datum)
{
	bool ret;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract string, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(string)");
		}
		ret = false;
	}
	else
	{
		datum = value_string[0].c_str();
		ret = true;
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<short> &) - insert a vector<short> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<short>& datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif
		value_string[i] = ostream.str();
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
		ostream.seekp(0);
#else
		ostream.str("");
#endif /* STRSTREAM */
	}
	value_type = DEVVAR_SHORTARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<short> &) - extract a vector<short> from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (vector<short>& datum)
{
	bool ret = true;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract short vector, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(vector<short>)");
		}
		datum.resize(0);
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		strstream iostream;
#else
		stringstream iostream;
#endif /* STRSTREAM */
		datum.resize(value_string.size());
		for (unsigned int i=0; i<value_string.size(); i++)
		{
			iostream.seekp (0); iostream.seekg(0); iostream.clear(); 
			iostream << value_string[i] << ends;
			iostream >> datum[i];
			if (!iostream)
			{
				if (exceptions_flags.test(wrongtype_flag))
				{
					TangoSys_OMemStream desc;
					desc << "Cannot extract short vector, elt number ";
					desc << i+1 << " is not a short" << ends;

					ApiDataExcept::throw_exception((const char*)"API_IncompatibleArgumentType",
					     desc.str(),
					     (const char*)"DbDatum::operator >>(vector<short>)");
				}
				ret = false;
				break;
			}
		}
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<unsigned short> &) - insert a vector<unsigned short> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<unsigned short>& datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif
		value_string[i] = ostream.str();
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
		ostream.seekp(0);
#else
		ostream.str("");
#endif /* STRSTREAM */
	}
	value_type = DEVVAR_USHORTARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<unsigned short> &) - extract a vector<unsigned short> from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (vector<unsigned short>& datum)
{
	bool ret = true;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract unsigned short vector, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(vector<unsigned short>)");
		}
		datum.resize(0);
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		strstream iostream;
#else
		stringstream iostream;
#endif /* STRSTREAM */
		datum.resize(value_string.size());
		for (unsigned int i=0; i<value_string.size(); i++)
		{
			iostream.seekp (0); iostream.seekg(0); iostream.clear(); 
			iostream << value_string[i] << ends;
			iostream >> datum[i];
			if (!iostream)
			{
				if (exceptions_flags.test(wrongtype_flag))
				{
					TangoSys_OMemStream desc;
					desc << "Cannot extract unsigned short vector, elt number ";
					desc << i+1 << " is not an unsigned short" << ends;

					ApiDataExcept::throw_exception((const char*)"API_IncompatibleArgumentType",
					     desc.str(),
					     (const char*)"DbDatum::operator >>(vector<unsigned short>)");
				}
				ret = false;
				break;
			}
		}
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<DevLong> &) - insert a vector<DevLong> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<DevLong>& datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif
		value_string[i] = ostream.str();
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
		ostream.seekp(0);
#else
		ostream.str("");
#endif /* STRSTREAM */
	}
	value_type = DEVVAR_LONGARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<DevLong> &) - extract a vector<DevLong> from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (vector<DevLong>& datum)
{
	bool ret = true;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract long vector, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(vector<long>)");
		}
		datum.resize(0);
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		strstream iostream;
#else
		stringstream iostream;
#endif /* STRSTREAM */
		datum.resize(value_string.size());
		for (unsigned int i=0; i<value_string.size(); i++)
		{
			iostream.seekp (0); iostream.seekg(0); iostream.clear(); 
			iostream << value_string[i] << ends;
			iostream >> datum[i];
			if (!iostream)
			{
				if (exceptions_flags.test(wrongtype_flag))
				{
					TangoSys_OMemStream desc;
					desc << "Cannot extract long vector, elt number ";
					desc << i+1 << " is not a DevLong (long 32 bits)" << ends;

					ApiDataExcept::throw_exception((const char*)"API_IncompatibleArgumentType",
					     desc.str(),
					     (const char*)"DbDatum::operator >>(vector<DevLong>)");
				}
				ret = false;
				break;
			}
		}
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<DevULong> &) - insert a vector<DevULong> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<DevULong>& datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif
		value_string[i] = ostream.str();
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
		ostream.seekp(0);
#else
		ostream.str("");
#endif /* STRSTREAM */
	}
	value_type = DEVVAR_ULONGARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<DevULong> &) - extract a vector<DevULong> from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (vector<DevULong>& datum)
{
	bool ret = true;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract unsigned long vector, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(vector<unsigned long>)");
		}
		datum.resize(0);
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		strstream iostream;
#else
		stringstream iostream;
#endif /* STRSTREAM */
		datum.resize(value_string.size());
		for (unsigned int i=0; i<value_string.size(); i++)
		{
			iostream.seekp (0); iostream.seekg(0); iostream.clear(); 
			iostream << value_string[i] << ends;
			iostream >> datum[i];
			if (!iostream)
			{
				if (exceptions_flags.test(wrongtype_flag))
				{
					TangoSys_OMemStream desc;
					desc << "Cannot extract unsigned long vector, elt number ";
					desc << i+1 << " is not a DevULong (unsigned long 32 bits)" << ends;

					ApiDataExcept::throw_exception((const char*)"API_IncompatibleArgumentType",
					     desc.str(),
					     (const char*)"DbDatum::operator >>(vector<DevULong>)");
				}
				ret = false;
				break;
			}
		}
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<DevLong64> &) - insert a vector<DevLong64> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<DevLong64>& datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif
		value_string[i] = ostream.str();
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
		ostream.seekp(0);
#else
		ostream.str("");
#endif /* STRSTREAM */
	}
	value_type = DEVVAR_LONG64ARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<DevLong64> &) - extract a vector<DevLong64> from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (vector<DevLong64>& datum)
{
	bool ret = true;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract unsigned long vector, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(vector<unsigned long>)");
		}
		datum.resize(0);
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		strstream iostream;
#else
		stringstream iostream;
#endif /* STRSTREAM */
		datum.resize(value_string.size());
		for (unsigned int i=0; i<value_string.size(); i++)
		{
			iostream.seekp (0); iostream.seekg(0); iostream.clear(); 
			iostream << value_string[i] << ends;
			iostream >> datum[i];
			if (!iostream)
			{
				if (exceptions_flags.test(wrongtype_flag))
				{
					TangoSys_OMemStream desc;
					desc << "Cannot extract unsigned long vector, elt number ";
					desc << i+1 << " is not a DevLong64 (long 64 bits)" << ends;

					ApiDataExcept::throw_exception((const char*)"API_IncompatibleArgumentType",
					     desc.str(),
					     (const char*)"DbDatum::operator >>(vector<DevLong64>)");
				}
				ret = false;
				break;
			}
		}
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<DevULong64> &) - insert a vector<DevULong64> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<DevULong64>& datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif
		value_string[i] = ostream.str();
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
		ostream.seekp(0);
#else
		ostream.str("");
#endif /* STRSTREAM */
	}
	value_type = DEVVAR_ULONG64ARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<DevULong64> &) - extract a vector<DevULong64> from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (vector<DevULong64>& datum)
{
	bool ret = true;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract unsigned long vector, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(vector<unsigned long>)");
		}
		datum.resize(0);
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		strstream iostream;
#else
		stringstream iostream;
#endif /* STRSTREAM */
		datum.resize(value_string.size());
		for (unsigned int i=0; i<value_string.size(); i++)
		{
			iostream.seekp (0); iostream.seekg(0); iostream.clear(); 
			iostream << value_string[i] << ends;
			iostream >> datum[i];
			if (!iostream)
			{
				if (exceptions_flags.test(wrongtype_flag))
				{
					TangoSys_OMemStream desc;
					desc << "Cannot extract unsigned long vector, elt number ";
					desc << i+1 << " is not a DevULong64 (unsigned long 64 bits)" << ends;

					ApiDataExcept::throw_exception((const char*)"API_IncompatibleArgumentType",
					     desc.str(),
					     (const char*)"DbDatum::operator >>(vector<DevULong64>)");
				}
				ret = false;
				break;
			}
		}
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<float> &) - insert a vector<float> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<float>& datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif
		value_string[i] = ostream.str();
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
		ostream.seekp(0);
#else
		ostream.str("");
#endif /* STRSTREAM */
	}
	value_type = DEVVAR_FLOATARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<float> &) - extract a vector<float> from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (vector<float>& datum)
{
	bool ret = true;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract float vector, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(vector<float>)");
		}
		datum.resize(0);
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		strstream iostream;
#else
		stringstream iostream;
#endif /* STRSTREAM */
		datum.resize(value_string.size());
		for (unsigned int i=0; i<value_string.size(); i++)
		{
			iostream.seekp (0); iostream.seekg(0); iostream.clear(); 
			iostream << value_string[i] << ends;
			iostream >> datum[i];
			if (!iostream)
			{
				if (exceptions_flags.test(wrongtype_flag))
				{
					TangoSys_OMemStream desc;
					desc << "Cannot extract float vector, elt number ";
					desc << i+1 << " is not a float" << ends;

					ApiDataExcept::throw_exception((const char*)"API_IncompatibleArgumentType",
					     desc.str(),
					     (const char*)"DbDatum::operator >>(vector<float>)");
				}
				ret = false;
				break;
			}
		}
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<double> &) - insert a vector<double> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<double>& datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif
		
		value_string[i] = ostream.str();
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
		ostream.seekp(0);
#else
		ostream.str("");
#endif /* STRSTREAM */
	}
	value_type = DEVVAR_DOUBLEARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<double> &) - extract a vector<double> from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (vector<double>& datum)
{
	bool ret = true;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract double vector, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(vector<float>)");
		}
		datum.resize(0);
		ret = false;
	}
	else
	{
#ifdef STRSTREAM
		strstream iostream;
#else
		stringstream iostream;
#endif /* STRSTREAM */
		datum.resize(value_string.size());
		for (unsigned int i=0; i<value_string.size(); i++)
		{
			iostream.seekp (0); iostream.seekg(0); iostream.clear(); 
			iostream << value_string[i] << ends;
			iostream >> std::setprecision(15) >> datum[i];
			if (!iostream)
			{
				if (exceptions_flags.test(wrongtype_flag))
				{
					TangoSys_OMemStream desc;
					desc << "Cannot extract double vector, elt number ";
					desc << i+1 << " is not a double" << ends;

					ApiDataExcept::throw_exception((const char*)"API_IncompatibleArgumentType",
					     desc.str(),
					     (const char*)"DbDatum::operator >>(vector<double>)");
				}
				ret = false;
				break;
			}
		}
	}
	
	return ret;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<string> &) - insert a vector<string> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<string>& datum)
{
	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		value_string[i] = datum[i];
	}
	value_type = DEVVAR_STRINGARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<string> &) - extract a vector<string> from DbDatum
//
//-----------------------------------------------------------------------------

bool DbDatum::operator >> (vector<string>& datum)
{
	bool ret = true;
	if (value_string.size() == 0)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_EmptyDbDatum",
					     (const char*)"cannot extract string vector, no data in DbDatum object ",
					     (const char*)"DbDatum::operator >>(vector<string>)");
		}
		datum.resize(0);
		ret = false;
	}
	else
	{
		datum.resize(value_string.size());
		for (unsigned int i=0; i<value_string.size(); i++)
		{
			datum[i] = value_string[i];
		}
	}
	
	return ret;
}

} // End of Tango namespace
