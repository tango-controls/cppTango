static const char *RcsId = "$Header$";
//
// dbdatum.cpp 	- C++ source code file for TANGO dbapi class DbDatum
//
// programmer 	- Andy Gotz (goetz@esrf.fr)
//
// original 	- October 2000
//
// last changed	- 17/10/2000 
//
// version 	- 1.0
//
// $Log$
// Revision 1.12  2001/06/08 12:55:06  goetz
// fixed bug with extracting vectors under Windows
//
//

#ifdef _HPUX_SOURCE
#include <iostream.h>
#else
#include <iostream>
#endif /* _HPUX_SOURCE */
#ifdef STRSTREAM
#ifdef _HPUX_SOURCE
#include <strstream.h>
#else
#include <strstream>
#endif /* _HPUX_SOURCE */
#else
#include <sstream>
#endif /* STRSTREAM */
#ifdef _HPUX_SOURCE
#include <iomanip.h>
#else
#include <iomanip>
#endif
#include <tango.h>
#include <devapi.h>
#include <dbapi.h>
#ifdef _HPUX_SOURCE
namespace std{}
#endif /* _HPUX_SOURCE */                                                       
using namespace std;
using namespace CORBA;
using namespace Tango;

//-----------------------------------------------------------------------------
//
// DbDatum::DbDatum() - constructor to create DbDatum specifying name
//
//-----------------------------------------------------------------------------

DbDatum::DbDatum(string p_name)
{
	name = p_name;
	value_size = 0;
	value_string.resize(0);
	exceptions_flags = no_flags;
}

//-----------------------------------------------------------------------------
//
// DbDatum::DbDatum() - constructor to create DbDatum without arguments
//
//-----------------------------------------------------------------------------

DbDatum::DbDatum()
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
// DbDatum::size() - returns number of elements in DbDatum
//
//-----------------------------------------------------------------------------

int DbDatum::size()
{
	return value_size;
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
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------
//
// DbDatum::exceptions() - switch on flags to throw exceptions
//
//-----------------------------------------------------------------------------

void DbDatum::exceptions(except_flags flags_to_throw)
{
	exceptions_flags = flags_to_throw;
	
	return;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(short &) - insert a short into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (short& datum)
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
	delete ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_SHORT;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(short &) - extract a short from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (short& datum)
{
	if (value_string.size() == 0) 
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract short, no data in DbDatum object ",
					     "DbDatum::operator >>(short)");
		}
		datum = 0;
		return;
	}

#ifdef STRSTREAM
	istrstream istream(value_string[0].c_str());
#else
	istringstream istream(value_string[0]);
#endif /* STRSTREAM */
	istream >> datum;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(unsigned short &) - insert a unsigned short into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (unsigned short& datum)
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
	delete ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_USHORT;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(unsigned short &) - extract a unsigned short from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (unsigned short& datum)
{
	if (value_string.size() == 0) 
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract unsigned short, no data in DbDatum object ",
					     "DbDatum::operator >>(unsigned short)");
		}
		datum = 0;
		return;
	}
#ifdef STRSTREAM
	istrstream istream(value_string[0].c_str());
#else
	istringstream istream(value_string[0]);
#endif /* STRSTREAM */
	istream >> datum;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(long &) - insert a long into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (long& datum)
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
	delete ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_LONG;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(long &) - extract a long from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (long& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract long, no data in DbDatum object ",
					     "DbDatum::operator >>(long)");
		}
		datum = 0;
		return;
	}
#ifdef STRSTREAM
	istrstream istream(value_string[0].c_str());
#else
	istringstream istream(value_string[0]);
#endif /* STRSTREAM */
	istream >> datum;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(unsigned long &) - insert a unsigned short into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (unsigned long& datum)
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
	delete ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_ULONG;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(unsigned long &) - extract a unsigned short from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (unsigned long& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract unsigned long, no data in DbDatum object ",
					     "DbDatum::operator >>(unsigned long)");
		}
		datum = 0;
		return;
	}
#ifdef STRSTREAM
	istrstream istream(value_string[0].c_str());
#else
	istringstream istream(value_string[0]);
#endif /* STRSTREAM */
	istream >> datum;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(float &) - insert a float into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (float& datum)
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
	delete ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_FLOAT;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(float &) - extract a float from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (float& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract float, no data in DbDatum object ",
					     "DbDatum::operator >>(float)");
		}
		datum = 0.0;
		return;
	}
#ifdef STRSTREAM
	istrstream istream(value_string[0].c_str());
#else
	istringstream istream(value_string[0]);
#endif /* STRSTREAM */
	istream >> datum;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(double &) - insert a double into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (double& datum)
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
	delete ostream.str();
#endif /* STRSTREAM */
	value_type = DEV_DOUBLE;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(double &) - extract a double from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (double& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract double, no data in DbDatum object ",
					     "DbDatum::operator >>(double)");
		}
		datum = 0.0;
		return;
	}
#ifdef STRSTREAM
	istrstream istream(value_string[0].c_str());
#else
	istringstream istream(value_string[0]);
#endif /* STRSTREAM */
#ifndef _HPUX_SOURCE
	istream >> std::setprecision(15) >> datum;
#else
	istream >> setprecision(15) >> datum;
#endif /* _HPUX_SOURCE */
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

void DbDatum::operator >> (string& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract string, no data in DbDatum object ",
					     "DbDatum::operator >>(string)");
		}
		datum = " ";
	}
	else
	{
		datum = value_string[0];
	}
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(char* &) - insert a char* into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (char*& datum)
{
	value_string.resize(1);
	value_string[0] = datum;
	value_type = DEV_STRING;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(const char* &) - insert a const char* into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (const char*& datum)
{
	value_string.resize(1);
	value_string[0] = datum;
	value_type = DEV_STRING;
	value_size = 1;
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(const char* &) - extract a const char* from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (const char*& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract string, no data in DbDatum object ",
					     "DbDatum::operator >>(string)");
		}
		datum = " ";
	}
	else
	{
		datum = value_string[0].c_str();
	}
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
	for (int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif /* STRSTREAM */
		value_string[i] = string(ostream.str());
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
#endif /* STRSTREAM */
		ostream.seekp(0);
	}
	value_type = DEVVAR_SHORTARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<short> &) - extract a vector<short> from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (vector<short>& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract short vector, no data in DbDatum object ",
					     "DbDatum::operator >>(vector<short>)");
		}
		datum.resize(0);
		return;
	}
#ifdef STRSTREAM
	strstream iostream;
#else
	stringstream iostream;
#endif /* STRSTREAM */
	datum.resize(value_string.size());
	for (int i=0; i<value_string.size(); i++)
	{
		iostream.clear(); iostream.seekp (0); iostream.seekg(0);
		iostream << value_string[i] << ends;
		iostream >> datum[i];
	}
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
	for (int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif /* STRSTREAm */
		value_string[i] = string(ostream.str());
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
#endif /* STRSTREAM */
		ostream.seekp(0);
	}
	value_type = DEVVAR_USHORTARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<unsigned short> &) - extract a vector<unsigned short> from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (vector<unsigned short>& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract unsigned short vector, no data in DbDatum object ",
					     "DbDatum::operator >>(vector<unsigned short>)");
		}
		datum.resize(0);
		return;
	}
#ifdef STRSTREAM
	strstream iostream;
#else
	stringstream iostream;
#endif /* STRSTREAM */
	datum.resize(value_string.size());
	for (int i=0; i<value_string.size(); i++)
	{
		iostream.clear(); iostream.seekp (0); iostream.seekg(0);
		iostream << value_string[i] << ends;
		iostream >> datum[i];
	}
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<long> &) - insert a vector<long> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<long>& datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	value_string.resize(datum.size());
	for (int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif /* STRSTREAM */
		value_string[i] = string(ostream.str());
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
#endif /* STRSTREAM */
		ostream.seekp(0);
	}
	value_type = DEVVAR_LONGARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<long> &) - extract a vector<long> from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (vector<long>& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract long vector, no data in DbDatum object ",
					     "DbDatum::operator >>(vector<long>)");
		}
		datum.resize(0);
		return;
	}
#ifdef STRSTREAM
	strstream iostream;
#else
	stringstream iostream;
#endif /* STRSTREAM */
	datum.resize(value_string.size());
	for (int i=0; i<value_string.size(); i++)
	{
		iostream.clear(); iostream.seekp (0); iostream.seekg(0);
		iostream << value_string[i] << ends;
		iostream >> datum[i];
	}
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<unsigned long> &) - insert a vector<unsigned long> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<unsigned long>& datum)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	value_string.resize(datum.size());
	for (int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif /* STRSTREAM */
		value_string[i] = string(ostream.str());
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
#endif /* STRSTREAM */
		ostream.seekp(0);
	}
	value_type = DEVVAR_ULONGARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<unsigned long> &) - extract a vector<unsigned long> from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (vector<unsigned long>& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract unsigned long vector, no data in DbDatum object ",
					     "DbDatum::operator >>(vector<unsigned long>)");
		}
		datum.resize(0);
		return;
	}
#ifdef STRSTREAM
	strstream iostream;
#else
	stringstream iostream;
#endif /* STRSTREAM */
	datum.resize(value_string.size());
	for (int i=0; i<value_string.size(); i++)
	{
		iostream << value_string[i];
		iostream >> datum[i];
	}
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
	for (int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif /* STRSTREAM */
		value_string[i] = string(ostream.str());
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
#endif /* STRSTREAM */
		ostream.seekp(0);
	}
	value_type = DEVVAR_FLOATARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<float> &) - extract a vector<float> from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (vector<float>& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract float vector, no data in DbDatum object ",
					     "DbDatum::operator >>(vector<float>)");
		}
		datum.resize(0);
		return;
	}
#ifdef STRSTREAM
	strstream iostream;
#else
	stringstream iostream;
#endif /* STRSTREAM */
	datum.resize(value_string.size());
	for (int i=0; i<value_string.size(); i++)
	{
		iostream.clear(); iostream.seekp (0); iostream.seekg(0);
		iostream << value_string[i] << ends;
		iostream >> datum[i];
	}
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
	for (int i=0; i<datum.size(); i++)
	{
		ostream << datum[i];
#ifdef STRSTREAM
		ostream << ends;
#endif /* STRSTREAM */
		value_string[i] = string(ostream.str());
#ifdef STRSTREAM
		ostream.rdbuf()->freeze(false);
#endif /* STRSTREAM */
		ostream.seekp(0);
	}
	value_type = DEVVAR_DOUBLEARRAY;
	value_size = datum.size();
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator >>(vector<double> &) - extract a vector<double> from DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator >> (vector<double>& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract double vector, no data in DbDatum object ",
					     "DbDatum::operator >>(vector<float>)");
		}
		datum.resize(0);
		return;
	}
#ifdef STRSTREAM
	strstream iostream;
#else
	stringstream iostream;
#endif /* STRSTREAM */
	datum.resize(value_string.size());
	for (int i=0; i<value_string.size(); i++)
	{
		iostream << value_string[i];
		iostream >> datum[i];
	}
}

//-----------------------------------------------------------------------------
//
// DbDatum::operator <<(vector<string> &) - insert a vector<string> into DbDatum
//
//-----------------------------------------------------------------------------

void DbDatum::operator << (vector<string>& datum)
{
	value_string.resize(datum.size());
	for (int i=0; i<datum.size(); i++)
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

void DbDatum::operator >> (vector<string>& datum)
{
	if (value_string.size() == 0)
	{
		if (exceptions_flags & isempty_flag)
		{
			Except::throw_exception("DbDatum is empty",
					     "cannot extract string vector, no data in DbDatum object ",
					     "DbDatum::operator >>(vector<string>)");
		}
		datum.resize(0);
		return;
	}
	datum.resize(value_string.size());
	for (int i=0; i<value_string.size(); i++)
	{
		datum[i] = value_string[i];
	}
}

