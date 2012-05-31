static const char *RcsId = "$Id$\n$Name$";
//
// dbdatum.cpp 	- C++ source code file for TANGO dbapi class DbDatum
//
// programmer 	- Andy Gotz (goetz@esrf.fr)
//
// original 	- October 2000
//
// Copyright (C) :      2000,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012
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

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#include <iomanip>

using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// DbDatum::DbDatum() - constructor to create DbDatum specifying name
//
//-----------------------------------------------------------------------------

DbDatum::DbDatum(string p_name):ext(Tango_NullPtr)
{
	name = p_name;
	value_size = 0;
	value_string.resize(0);
}

DbDatum::DbDatum(const char *p_name):name(p_name),ext(Tango_NullPtr)
{
	value_size = 0;
	value_string.resize(0);
}

//-----------------------------------------------------------------------------
//
// DbDatum::DbDatum() - constructor to create DbDatum without arguments
//
//-----------------------------------------------------------------------------

DbDatum::DbDatum():ext(Tango_NullPtr)
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

DbDatum::DbDatum(const DbDatum &source):ext(new DbDatumExt)
{
	name = source.name;
	value_string = source.value_string;
	value_type = source.value_type;
	value_size = source.value_size;
	exceptions_flags = source.exceptions_flags;
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
#ifdef HAS_UNIQUE_PTR
	ext.reset(new DbDatumExt);
#else
    ext = rval.ext;
#endif

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
	ostringstream ostream;
	ostream << boolalpha << datum;

	value_string.resize(1);
	value_string[0] = string(ostream.str());

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
	ostringstream ostream;
	ostream << datum;

	value_string.resize(1);
	value_string[0] = string(ostream.str());

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
		istringstream istream(value_string[0]);

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
	ostringstream ostream;
	ostream << (short)datum; // to accept only numbers

	value_string.resize(1);
	value_string[0] = string(ostream.str());

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
					     (const char*)"DbDatum::operator >>(unsigned char)");
		}
		ret = false;
	}
	else
	{
		istringstream istream(value_string[0]);
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
	ostringstream ostream;
	ostream << datum;

	value_string.resize(1);
	value_string[0] = string(ostream.str());

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
		istringstream istream(value_string[0]);
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
	ostringstream ostream;
	ostream << datum;

	value_string.resize(1);
	value_string[0] = string(ostream.str());

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
		istringstream istream(value_string[0]);
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
	ostringstream ostream;
	ostream << datum;

	value_string.resize(1);
	value_string[0] = string(ostream.str());

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
		istringstream istream(value_string[0]);
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
	ostringstream ostream;
	ostream << datum;

	value_string.resize(1);
	value_string[0] = string(ostream.str());

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
		istringstream istream(value_string[0]);
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
	ostringstream ostream;
	ostream << datum;

	value_string.resize(1);
	value_string[0] = string(ostream.str());

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
		istringstream istream(value_string[0]);
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
	ostringstream ostream;
	ostream << std::setprecision(TANGO_FLOAT_PRECISION) << datum;

	value_string.resize(1);
	value_string[0] = string(ostream.str());

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
		istringstream istream(value_string[0]);
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
	ostringstream ostream;
	ostream << setprecision(TANGO_FLOAT_PRECISION) << datum;

	value_string.resize(1);
	value_string[0] = string(ostream.str());

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
		istringstream istream(value_string[0]);
		istream >> std::setprecision(TANGO_FLOAT_PRECISION) >> datum;
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
	ostringstream ostream;
	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
		value_string[i] = ostream.str();

		ostream.str("");
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
		stringstream iostream;

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
	ostringstream ostream;
	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
		value_string[i] = ostream.str();
		ostream.str("");
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
		stringstream iostream;

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
	ostringstream ostream;

	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
		value_string[i] = ostream.str();
		ostream.str("");
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
		stringstream iostream;

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
	ostringstream ostream;

	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
		value_string[i] = ostream.str();
		ostream.str("");
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
		stringstream iostream;
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
	ostringstream ostream;

	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
		value_string[i] = ostream.str();
		ostream.str("");
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
		stringstream iostream;

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
	ostringstream ostream;

	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
		value_string[i] = ostream.str();
		ostream.str("");
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
		stringstream iostream;

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
	ostringstream ostream;

	value_string.resize(datum.size());
	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << std::setprecision(TANGO_FLOAT_PRECISION) << datum[i];
		value_string[i] = ostream.str();
		ostream.str("");
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
		stringstream iostream;

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
	ostringstream ostream;
	value_string.resize(datum.size());

	for (unsigned int i=0; i<datum.size(); i++)
	{
		ostream << std::setprecision(TANGO_FLOAT_PRECISION) << datum[i];
		value_string[i] = ostream.str();
		ostream.str("");
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
		stringstream iostream;

		datum.resize(value_string.size());
		for (unsigned int i=0; i<value_string.size(); i++)
		{
			iostream.seekp (0); iostream.seekg(0); iostream.clear();
			iostream << value_string[i] << ends;
			iostream >> std::setprecision(TANGO_FLOAT_PRECISION) >> datum[i];
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
