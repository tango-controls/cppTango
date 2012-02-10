static const char *RcsId = "$Id$";

//
// devapi_data.cpp 	- C++ source code file for TANGO devapi class DeviceData
//
// programmer(s) 	- Andy Gotz (goetz@esrf.fr)
//
// original 		- March 2001
//
// Copyright (C) :      2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// version 		- $Revision$
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

DeviceData::DeviceData():ext(Tango_NullPtr)
{
//
// For omniORB, it is necessary to do the ORB::init before creating the Any.
// Otherwise, string insertion into the Any will not be possible
//

	ApiUtil *au = ApiUtil::instance();
	if (CORBA::is_nil(au->get_orb()) == true)
		au->create_orb();

	any = new CORBA::Any();
	exceptions_flags.set(isempty_flag);
}

//-----------------------------------------------------------------------------
//
// DeviceData::DeviceData() - copy constructor to create DeviceData
//
//-----------------------------------------------------------------------------

DeviceData::DeviceData(const DeviceData & source):ext(Tango_NullPtr)
{
	exceptions_flags = source.exceptions_flags;
#ifdef HAS_RVALUE
	any = source.any;
#else
	any = const_cast<DeviceData &>(source).any._retn();
#endif

#ifdef HAS_UNIQUE_PTR
    if (source.ext.get() != NULL)
    {
        ext.reset(new DeviceDataExt);
        *(ext.get()) = *(source.ext.get());
    }
#else
	if (source.ext != NULL)
	{
		ext = new DeviceDataExt();
		*ext = *(source.ext);
	}
	else
		ext = NULL;
#endif
}

//-----------------------------------------------------------------------------
//
// DeviceData::DeviceData() - move constructor to create DeviceData
//
//-----------------------------------------------------------------------------

#ifdef HAS_RVALUE
DeviceData::DeviceData(DeviceData &&source):ext(Tango_NullPtr)
{
	exceptions_flags = source.exceptions_flags;
	any = source.any._retn();

    if (source.ext.get() != NULL)
        ext = move(source.ext);
}
#endif

//-----------------------------------------------------------------------------
//
// DeviceData::operator=() - assignement operator
//
//-----------------------------------------------------------------------------

DeviceData & DeviceData::operator=(const DeviceData &rval)
{
    if (this != &rval)
    {
        exceptions_flags = rval.exceptions_flags;
#ifdef HAS_RVALUE
        any = rval.any;
#else
        any = const_cast<DeviceData &>(rval).any._retn();
#endif

#ifdef HAS_UNIQUE_PTR
        if (rval.ext.get() != NULL)
        {
            ext.reset(new DeviceDataExt);
            *(ext.get()) = *(rval.ext.get());
        }
        else
            ext.reset();
#else
        delete ext;

        if (rval.ext != NULL)
        {
            ext = new DeviceDataExt();
            *ext = *(rval.ext);
        }
        else
            ext = NULL;
#endif
    }
	return *this;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator=() - move assignement operator
//
//-----------------------------------------------------------------------------

#ifdef HAS_RVALUE
DeviceData & DeviceData::operator=(DeviceData &&rval)
{
	exceptions_flags = rval.exceptions_flags;
	any = rval.any._retn();

    if (rval.ext.get() != NULL)
        ext = move(rval.ext);
    else
        ext.reset();

	return *this;
}
#endif

//-----------------------------------------------------------------------------
//
// DeviceData::~DeviceData() - destructor to destroy DeviceData
//
//-----------------------------------------------------------------------------

DeviceData::~DeviceData()
{
#ifndef HAS_UNIQUE_PTR
    delete ext;
#endif
}

//-----------------------------------------------------------------------------
//
// DeviceData::any_is_null() - test any type for equality to null
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
// DeviceData::get_type() - return DeviceData data type
//
//-----------------------------------------------------------------------------

int DeviceData::get_type()
{
	int data_type = 0;

	if (any_is_null() == true)
		return -1;
	else
	{
		CORBA::TypeCode_ptr tc;
		CORBA::TypeCode_var tc_al;
		CORBA::TypeCode_var tc_seq;
		CORBA::TypeCode_var tc_field;

		tc = any->type();
		switch(tc->kind())
		{
		case CORBA::tk_boolean:
			data_type = Tango::DEV_BOOLEAN;
			break;

		case CORBA::tk_short:
			data_type = Tango::DEV_SHORT;
			break;

		case CORBA::tk_long:
			data_type = Tango::DEV_LONG;
			break;

		case CORBA::tk_longlong:
			data_type = Tango::DEV_LONG64;
			break;

		case CORBA::tk_float:
			data_type = Tango::DEV_FLOAT;
			break;

		case CORBA::tk_double:
			data_type = Tango::DEV_DOUBLE;
			break;

		case CORBA::tk_ushort:
			data_type = Tango::DEV_USHORT;
			break;

		case CORBA::tk_ulong:
			data_type = Tango::DEV_ULONG;
			break;

		case CORBA::tk_ulonglong:
			data_type = Tango::DEV_ULONG64;
			break;

		case CORBA::tk_string:
			data_type = Tango::DEV_STRING;
			break;

		case CORBA::tk_alias:
			tc_al = tc->content_type();
			tc_seq = tc_al->content_type();
			switch (tc_seq->kind())
			{
			case CORBA::tk_octet:
				data_type = Tango::DEVVAR_CHARARRAY;
				break;

			case CORBA::tk_short:
				data_type = Tango::DEVVAR_SHORTARRAY;
				break;

			case CORBA::tk_long:
				data_type = Tango::DEVVAR_LONGARRAY;
				break;

			case CORBA::tk_longlong:
				data_type = Tango::DEVVAR_LONG64ARRAY;
				break;

			case CORBA::tk_float:
				data_type = Tango::DEVVAR_FLOATARRAY;
				break;

			case CORBA::tk_double:
				data_type = Tango::DEVVAR_DOUBLEARRAY;
				break;

			case CORBA::tk_ushort:
				data_type = Tango::DEVVAR_USHORTARRAY;
				break;

			case CORBA::tk_ulong:
				data_type = Tango::DEVVAR_ULONGARRAY;
				break;

			case CORBA::tk_ulonglong:
				data_type = Tango::DEVVAR_ULONG64ARRAY;
				break;

			case CORBA::tk_string:
				data_type = Tango::DEVVAR_STRINGARRAY;
				break;

			default:
				break;
			}
			break;

		case CORBA::tk_struct:
			tc_field = tc->member_type(0);
			tc_al = tc_field->content_type();
            switch (tc_al->kind())
            {
                case CORBA::tk_sequence:
                    tc_seq = tc_al->content_type();
                    switch (tc_seq->kind())
                    {
                    case CORBA::tk_long:
                        data_type = Tango::DEVVAR_LONGSTRINGARRAY;
                        break;

                    case CORBA::tk_double:
                        data_type = Tango::DEVVAR_DOUBLESTRINGARRAY;
                        break;

                    default:
                        break;
                    }
                    break;

                case CORBA::tk_string:
                    data_type = Tango::DEV_ENCODED;
                    break;

                default:
                    break;
            }
			break;

		case CORBA::tk_enum:
			data_type = Tango::DEV_STATE;
			break;

		default:
			break;

		}

		CORBA::release(tc);
	}

	return data_type;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(bool &) - extract a boolean from DeviceData
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
// DeviceData::operator >>(DevLong &) - extract a DevLong from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (DevLong& datum)
{
	bool ret;

	ret = (any >>= datum);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
					       	(const char*)"Cannot extract, data in DeviceData object is not a DevLong (long 32 bits)",
					        (const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevULong &) - extract a DevULong from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (DevULong& datum)
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
					       	(const char*)"Cannot extract, data in DeviceData object is not an DevULong (unsigned long 32 bits)",
					        (const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevLong64) - extract a DevLong64 from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (DevLong64 & datum)
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
					       	(const char*)"Cannot extract, data in DeviceData object is not a DevLong64 (64 bits long)",
					        (const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevULong64 &) - extract a DevULong64 from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (DevULong64 & datum)
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
					       	(const char*)"Cannot extract, data in DeviceData object is not a DevULong64 (unsigned 64 bits long)",
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

	const char *c_string = NULL;
	ret = (any >>= c_string);
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
	else
	{
		datum = c_string;
	}
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
// DeviceData::operator >>(vector<unsigned char> &) - extract a vector<unsigned char> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<unsigned char>& datum)
{
	bool ret;

	const DevVarCharArray *char_array = NULL;
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
	    if (char_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
	    }
        else
        {
            datum.resize(char_array->length());
            for (unsigned int i=0; i<char_array->length(); i++)
            {
                datum[i] = (*char_array)[i];
            }
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
// DeviceData::operator >>(vector<short> &) - extract a vector<short> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<short>& datum)
{
	bool ret;

	const DevVarShortArray *short_array = NULL;
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
	    if (short_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
	    }
        else
        {
            datum.resize(short_array->length());
            for (unsigned int i=0; i<short_array->length(); i++)
            {
                datum[i] = (*short_array)[i];
            }
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
// DeviceData::operator >>(vector<unsigned short> &) - extract a vector<unsigned short> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<unsigned short>& datum)
{
	bool ret = true;

	const DevVarUShortArray *ushort_array = NULL;
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
	    if (ushort_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
	    }
        else
        {
            datum.resize(ushort_array->length());
            for (unsigned int i=0; i<ushort_array->length(); i++)
            {
                datum[i] = (*ushort_array)[i];
            }
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
// DeviceData::operator >>(vector<DevLong> &) - extract a vector<DevLong> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<DevLong>& datum)
{
	bool ret = true;

	const DevVarLongArray *long_array = NULL;

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
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of DevLong (long 32 bits)",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
	    if (long_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
	    }
        else
        {
            datum.resize(long_array->length());
            for (unsigned int i=0; i<long_array->length(); i++)
            {
                datum[i] = (*long_array)[i];
            }
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
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of long (32 bits)",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<DevULong> &) - extract a vector<DevULong> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<DevULong>& datum)
{
	bool ret = true;

	const DevVarULongArray *ulong_array = NULL;

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
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of DevULong (unsigned long 32 bits)",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
	    if (ulong_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
	    }
        else
        {
            datum.resize(ulong_array->length());
            for (unsigned int i=0; i<ulong_array->length(); i++)
            {
                datum[i] = (*ulong_array)[i];
            }
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
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of unsigned long (32 bits)",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarLong64Array *) - extract a DevVarLong64Array from DeviceData
//
//-----------------------------------------------------------------------------


bool DeviceData::operator >> (const DevVarLong64Array* &datum)
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
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of long (64 bits)",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarULong64Array *) - extract a DevVarULong64Array from DeviceData
//
//-----------------------------------------------------------------------------


bool DeviceData::operator >> (const DevVarULong64Array* &datum)
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
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of unsigned long (64 bits)",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<DevLong64> &) - extract a vector<DevLong64> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<DevLong64>& datum)
{
	bool ret = true;

	const DevVarLong64Array *ll_array = NULL;
	ret = (any.inout() >>= ll_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of DevLong64 (64 bits long)",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
	    if (ll_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
	    }
        else
        {
            datum.resize(ll_array->length());
            for (unsigned int i=0; i<ll_array->length(); i++)
            {
                datum[i] = (*ll_array)[i];
            }
        }
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<DevULong64> &) - extract a vector<DevULong64> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<DevULong64>& datum)
{
	bool ret = true;

	const DevVarULong64Array *ull_array = NULL;
	ret = (any.inout() >>= ull_array);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not an array of DevULong64 (unsigned 64 bits long)",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
	    if (ull_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
	    }
        else
        {
            datum.resize(ull_array->length());
            for (unsigned int i=0; i<ull_array->length(); i++)
            {
                datum[i] = (*ull_array)[i];
            }
        }
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<float> &) - extract a vector<float> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<float>& datum)
{
	bool ret = true;

	const DevVarFloatArray *float_array = NULL;
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
	    if (float_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
	    }
        else
        {
            datum.resize(float_array->length());
            for (unsigned int i=0; i<float_array->length(); i++)
            {
                datum[i] = (*float_array)[i];
            }
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
// DeviceData::operator >>(vector<double> &) - extract a vector<double> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<double>& datum)
{
	bool ret = true;
	const DevVarDoubleArray *double_array = NULL;

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
	    if (double_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
	    }
        else
        {
            datum.resize(double_array->length());
            for (unsigned int i=0; i<double_array->length(); i++)
            {
                datum[i] = (*double_array)[i];
            }
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
// DeviceData::operator >>(vector<string> &) - extract a vector<string> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (vector<string>& datum)
{
	bool ret;
	const DevVarStringArray *string_array = NULL;

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
	    if (string_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
	    }
        else
        {
            datum.resize(string_array->length());
            for (unsigned int i=0; i<string_array->length(); i++)
            {
                datum[i] = (*string_array)[i];
            }
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
// DeviceData::operator >>(DevEncoded *) - extract a DevEncoded from DeviceData
// by pointer
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (const DevEncoded* &datum)
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
				       		(const char*)"Cannot extract, data in DeviceData object is not a DevEncoded",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevEncoded &) - extract a DevEncoded from DeviceData
// by reference
//
//-----------------------------------------------------------------------------

bool DeviceData::operator >> (DevEncoded &datum)
{
    bool ret = true;
    const DevEncoded *tmp_enc = NULL;
	ret = (any.inout() >>= tmp_enc);
	if (ret == false)
	{
		if (exceptions_flags.test(isempty_flag))
		{
			any_is_null();
		}
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleCmdArgumentType",
				       		(const char*)"Cannot extract, data in DeviceData object is not a DevEncoded",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
	    if (tmp_enc == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
	    }
        else
        {
            datum.encoded_data.length(tmp_enc->encoded_data.length());
            for (unsigned int i=0; i<tmp_enc->encoded_data.length(); i++)
            {
                datum.encoded_data[i] = tmp_enc->encoded_data[i];
            }
            datum.encoded_format = CORBA::string_dup(tmp_enc->encoded_format);
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
// DeviceData::operator <<(vector<DevLong> &) - insert a vector<DevLong> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<DevLong>& datum)
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
// DeviceData::operator <<(vector<DevULong> &) - insert a vector<DevULong> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<DevULong>& datum)
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
// DeviceData::operator <<(vector<DevLong64> &) - insert a vector<DevLong64> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<DevLong64>& datum)
{
	DevVarLong64Array *ll_array = new DevVarLong64Array();
	ll_array->length(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		(*ll_array)[i] = datum[i];
	}
	any.inout() <<= ll_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<DevULong64> &) - insert a vector<DevULong64> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<DevULong64>& datum)
{
	DevVarULong64Array *ull_array = new DevVarULong64Array();
	ull_array->length(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		(*ull_array)[i] = datum[i];
	}
	any.inout() <<= ull_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::insert (vector<DevLong>, vector<string> &) - insert a pair of
//             vector<DevLong>,vector<string> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::insert (vector<DevLong> &long_datum, vector<string>& string_datum)
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
// DeviceData::extract (vector<DevLong>, vector<string> &) - extract a pair of
//             vector<Devlong>,vector<string> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::extract(vector<DevLong> &long_datum, vector<string>& string_datum)
{
	bool ret;
	unsigned int i;
	const DevVarLongStringArray *long_string_array = NULL;
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
				       		(const char*)"Cannot extract, data in DeviceData object is not a structure with sequences of string(s) and long(s) (32 bits)",
				        	(const char*)"DeviceData::operator>>");
		}
	}
	else
	{
	    if (long_string_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
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
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarLongStringArray *) - insert a DevVarLongStringArray into DeviceData
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
				       		(const char*)"Cannot extract, data in DeviceData object is not a structure with sequences of string(s) and long(s) (32 bits) ",
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
//             vector<double>,vector<string> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceData::extract (vector<double> &double_datum, vector<string>& string_datum)
{
	bool ret;
	unsigned int i;
	const DevVarDoubleStringArray *double_string_array = NULL;
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
	    if (double_string_array == NULL)
	    {
            ApiDataExcept::throw_exception((const char *)"API_IncoherentDevData",
                                       (const char *)"Incoherent data received from server",
                                       (const char *)"DeviceData::operator>>");
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
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(DevVarDoubleStringArray *) - insert a DevVarDoubleStringArray into DeviceData
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

//-----------------------------------------------------------------------------
//
// DeviceData::insert (string, vector<unsigned char> &) - insert a pair of
//             string,vector<unsigned char> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::insert (const string &str_datum, vector<unsigned char>& char_datum)
{
	DevEncoded *the_enc = new DevEncoded();
	the_enc->encoded_format = CORBA::string_dup(str_datum.c_str());

	the_enc->encoded_data.replace(char_datum.size(),char_datum.size(),&(char_datum[0]),false);
	any.inout() <<= the_enc;
}

//-----------------------------------------------------------------------------
//
// DeviceData::insert (const char *, DevVarCharArray *) - insert a pair of
//             char *,DevVarCharArray into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::insert (const char *str_datum, DevVarCharArray *char_datum)
{
	DevEncoded *the_enc = new DevEncoded();
	the_enc->encoded_format = CORBA::string_dup(str_datum);

	the_enc->encoded_data.replace(char_datum->length(),char_datum->length(),char_datum->get_buffer(),false);
	any.inout() <<= the_enc;
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
			Tango::DevLong l_tmp;
			dd.any >>= l_tmp;
			o_str << l_tmp;
			break;

		case CORBA::tk_longlong:
#ifdef TANGO_LONG32
			long long ll_tmp;
#else
			long ll_tmp;
#endif
			dd.any >>= ll_tmp;
			o_str << ll_tmp;
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
			Tango::DevULong ul_tmp;
			dd.any >>= ul_tmp;
			o_str << ul_tmp;
			break;

		case CORBA::tk_ulonglong:
			unsigned long ull_tmp;
			dd.any >>= ull_tmp;
			o_str << ull_tmp;
			break;

		case CORBA::tk_string:
			const char *str_tmp;
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

			case CORBA::tk_longlong:
				Tango::DevVarLong64Array *llg_arr;
				dd.any.inout() >>= llg_arr;
				o_str << *llg_arr;
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

			case CORBA::tk_ulonglong:
				Tango::DevVarULong64Array *ull_arr;
				dd.any.inout() >>= ull_arr;
				o_str << *ull_arr;
				break;

			case CORBA::tk_string:
				Tango::DevVarStringArray *str_arr;
				dd.any.inout() >>= str_arr;
				o_str << *str_arr;
				break;

			default:
				break;
			}
			break;

		case CORBA::tk_struct:
            tc_field = tc->member_type(0);
            tc_al = tc_field->content_type();
            switch (tc_al->kind())
			{
                case CORBA::tk_sequence:
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

                        default:
                            break;
                    }
                    break;

                case CORBA::tk_string:
                    Tango::DevEncoded *enc;
                    dd.any.inout() >>= enc;
                    o_str << "Encoding string: " << enc->encoded_format << endl;
                    {
                        long nb_data_elt = enc->encoded_data.length();
                        for (long i = 0;i < nb_data_elt;i++)
                        {
                            o_str << "Data element number [" << i << "] = " << (int)enc->encoded_data[i];
                            if (i < (nb_data_elt - 1))
                                o_str << '\n';
                        }
                    }
                    break;

                default:
                    break;
			}
			break;

		case CORBA::tk_enum:
			Tango::DevState tmp_state;
			dd.any.inout() >>= tmp_state;
			o_str << Tango::DevStateName[tmp_state];
			break;

		default:
			break;

		}

		CORBA::release(tc);
	}

	return o_str;
}


} // End of Tango namepsace
