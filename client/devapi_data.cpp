static const char *RcsId = "$Header$";
//
// devapi_data.cpp 	- C++ source code file for TANGO devapi class DeviceData
//
// programmer(s) 	- Andy Gotz (goetz@esrf.fr)
//
// original 		- March 2001
//
// log			- $Log$
// log			- Revision 1.2  2001/05/28 11:50:07  goetz
// log			- intermediate checkin
// log			-
// log			- Revision 1.1.1.1  2001/04/02 12:33:35  goetz
// log			- Imported sources
// log			-
//
// version 		- $Version$
//

#include <tango.h>
#include <devapi.h>
#ifdef _HPUX_SOURCE
namespace std{}
#endif /* _HPUX_SOURCE */                                                       
using namespace std;
using namespace CORBA;
using namespace Tango;

//-----------------------------------------------------------------------------
//
// DeviceData::DeviceData() - constructor to create DeviceData 
//
//-----------------------------------------------------------------------------

DeviceData::DeviceData()
{
	value_size = 0;
	exceptions_flags = no_flags;
	any = new Any;
}

DeviceData::DeviceData(const DeviceData & source) 
{
	value_size = source.value_size;
	exceptions_flags = source.exceptions_flags;
	any = source.any;
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
// DeviceData::is_empty() - returns true or false if datum is empty
//
//-----------------------------------------------------------------------------

bool DeviceData::is_empty()
{
	if (value_size == 0)
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
// DeviceData::exceptions() - switch on flags to throw exceptions
//
//-----------------------------------------------------------------------------

void DeviceData::exceptions(except_flags flags_to_throw)
{
	exceptions_flags = flags_to_throw;
	
	return;
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
		if (exceptions_flags | isempty_flag)
		{
			Except::throw_exception("DeviceData is empty",
					     "cannot extract,  no data in DeviceData object ",
					     "DeviceData::any_is_null");
		}
		return(true);
	}
	CORBA::release(tc);

	return(false);
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(short &) - insert a short into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (short& datum)
{
	value_type = DEV_SHORT;
	value_size = 1;
	any <<= datum;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(short &) - extract a short from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (short& datum)
{
	if (any_is_null())
	{
		datum = 0;
	}
	else
	{
		any >>= datum;
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(unsigned short &) - insert a unsigned short into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (unsigned short& datum)
{
	value_type = DEV_USHORT;
	value_size = 1;
	any <<= datum;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(unsigned short &) - extract a unsigned short from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (unsigned short& datum)
{
	if (any_is_null())
	{
		datum = 0;
	}
	else
	{
		any >>= datum;
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(long &) - insert a long into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (long& datum)
{
	value_type = DEV_LONG;
	value_size = 1;
	any <<= datum;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(long &) - extract a long from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (long& datum)
{
	if (any_is_null())
	{
		datum = 0;
	}
	else
	{
		any >>= datum;
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(unsigned long &) - insert a unsigned short into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (unsigned long& datum)
{
	value_type = DEV_ULONG;
	value_size = 1;
	any <<= datum;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(unsigned long &) - extract a unsigned short from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (unsigned long& datum)
{
	if (any_is_null())
	{
		datum = 0;
	}
	else
	{
		any >>= datum;
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(float &) - insert a float into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (float& datum)
{
	value_type = DEV_FLOAT;
	value_size = 1;
	any <<= datum;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(float &) - extract a float from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (float& datum)
{
	if (any_is_null())
	{
		datum = 0.0;
	}
	else
	{
		any >>= datum;
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(double &) - insert a double into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (double& datum)
{
	value_type = DEV_DOUBLE;
	value_size = 1;
	any <<= datum;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(double &) - extract a double from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (double& datum)
{
	if (any_is_null())
	{
		datum = 0.0;
	}
	else
	{
		any >>= datum;
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(string &) - insert a string into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (string& datum)
{
	value_type = DEV_STRING;
	value_size = 1;
	any <<= datum.c_str();
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(string &) - extract a string from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (string& datum)
{
	if (any_is_null())
	{
		datum = " ";
	}
	else
	{
		const char *c_string;
		any >>= c_string;
		datum = c_string;
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(char* &) - insert a char* into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (char*& datum)
{
	value_type = DEV_STRING;
	value_size = 1;
	any <<= datum;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(const char* &) - insert a const char* into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (const char*& datum)
{
	value_type = DEV_STRING;
	value_size = 1;
	any <<= datum;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(const char* &) - extract a const char* from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (const char*& datum)
{
	if (any_is_null())
	{
		datum = " ";
	}
	else
	{
		any >>= datum;
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<short> &) - insert a vector<short> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<short>& datum)
{
	value_type = DEVVAR_SHORTARRAY;
	value_size = datum.size();

	DevVarShortArray *short_array = new DevVarShortArray();
	short_array->length(datum.size());
	for (int i=0; i<datum.size(); i++)
	{
		(*short_array)[i] = datum[i];
	}
	any <<= short_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<short> &) - extract a vector<short> from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (vector<short>& datum)
{
	if (any_is_null())
	{
		datum.resize(0);
	}
	else
	{
		const DevVarShortArray *short_array;
		any >>= short_array;

		datum.resize(short_array->length());
		for (int i=0; i<short_array->length(); i++)
		{
			datum[i] = (*short_array)[i];
		}
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<unsigned short> &) - insert a vector<unsigned short> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<unsigned short>& datum)
{
	value_type = DEVVAR_USHORTARRAY;
	value_size = datum.size();

	DevVarUShortArray *ushort_array = new DevVarUShortArray();
	ushort_array->length(datum.size());
	for (int i=0; i<datum.size(); i++)
	{
		(*ushort_array)[i] = datum[i];
	}
	any <<= ushort_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<unsigned short> &) - extract a vector<unsigned short> from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (vector<unsigned short>& datum)
{
	if (any_is_null())
	{
		datum.resize(0);
	}
	else
	{
		const DevVarUShortArray *ushort_array;
		any >>= ushort_array;
		datum.resize(ushort_array->length());
		for (int i=0; i<ushort_array->length(); i++)
		{
			datum[i] = (*ushort_array)[i];
		}
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<long> &) - insert a vector<long> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<long>& datum)
{
	value_type = DEVVAR_LONGARRAY;
	value_size = datum.size();

	DevVarLongArray *long_array = new DevVarLongArray();
	long_array->length(datum.size());
	for (int i=0; i<datum.size(); i++)
	{
		(*long_array)[i] = datum[i];
	}
	any <<= long_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<long> &) - extract a vector<long> from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (vector<long>& datum)
{
	if (any_is_null())
	{
		datum.resize(0);
	}
	else
	{
		const DevVarLongArray *long_array;
		any >>= long_array;
		datum.resize(long_array->length());
		for (int i=0; i<long_array->length(); i++)
		{
			datum[i] = (*long_array)[i];
		}
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<unsigned long> &) - insert a vector<unsigned long> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<unsigned long>& datum)
{
	value_type = DEVVAR_ULONGARRAY;
	value_size = datum.size();

	DevVarULongArray *ulong_array = new DevVarULongArray();
	ulong_array->length(datum.size());
	for (int i=0; i<datum.size(); i++)
	{
		(*ulong_array)[i] = datum[i];
	}
	any <<= ulong_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<unsigned long> &) - extract a vector<unsigned long> from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (vector<unsigned long>& datum)
{
	if (any_is_null())
	{
		datum.resize(0);
	}
	else
	{
		const DevVarULongArray *ulong_array;
		any >>= ulong_array;
		datum.resize(ulong_array->length());
		for (int i=0; i<ulong_array->length(); i++)
		{
			datum[i] = (*ulong_array)[i];
		}
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<float> &) - insert a vector<float> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<float>& datum)
{
	value_type = DEVVAR_FLOATARRAY;
	value_size = datum.size();

	DevVarFloatArray *float_array = new DevVarFloatArray();
	float_array->length(datum.size());
	for (int i=0; i<datum.size(); i++)
	{
		(*float_array)[i] = datum[i];
	}
	any <<= float_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<float> &) - extract a vector<float> from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (vector<float>& datum)
{
	if (any_is_null())
	{
		datum.resize(0);
	}
	else
	{
		const DevVarFloatArray *float_array;
		any >>= float_array;
		datum.resize(float_array->length());
		for (int i=0; i<float_array->length(); i++)
		{
			datum[i] = (*float_array)[i];
		}
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<double> &) - insert a vector<double> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<double>& datum)
{
	value_type = DEVVAR_DOUBLEARRAY;
	value_size = datum.size();

	DevVarDoubleArray *double_array = new DevVarDoubleArray();
	double_array->length(datum.size());
	for (int i=0; i<datum.size(); i++)
	{
		(*double_array)[i] = datum[i];
	}
	any <<= double_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<double> &) - extract a vector<double> from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (vector<double>& datum)
{
	if (any_is_null())
	{
		datum.resize(0);
	}
	else
	{
		const DevVarDoubleArray *double_array;
		any >>= double_array;
		datum.resize(double_array->length());
		for (int i=0; i<double_array->length(); i++)
		{
			datum[i] = (*double_array)[i];
		}
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator <<(vector<string> &) - insert a vector<string> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator << (vector<string>& datum)
{
	value_type = DEVVAR_STRINGARRAY;
	value_size = datum.size();

	DevVarStringArray *string_array = new DevVarStringArray();
	string_array->length(datum.size());
	for (int i=0; i<datum.size(); i++)
	{
		(*string_array)[i] = string_dup(datum[i].c_str());
	}
	any <<= string_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::operator >>(vector<string> &) - extract a vector<string> from DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::operator >> (vector<string>& datum)
{
	if (any_is_null())
	{
		datum.resize(0);
	}
	else
	{
		const DevVarStringArray *string_array;
		any >>= string_array;
		datum.resize(string_array->length());
		for (int i=0; i<string_array->length(); i++)
		{
			datum[i] = (*string_array)[i];
		}
	}
}

//-----------------------------------------------------------------------------
//
// DeviceData::insert (vector<long>, vector<string> &) - insert a pair of
//             vector<long>,vector<string> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::insert (vector<long> &long_datum, vector<string>& string_datum)
{

	DevVarLongStringArray *long_string_array = new DevVarLongStringArray();
	long_string_array->lvalue.length(long_datum.size());
	for (int i=0; i<long_datum.size(); i++)
	{
		(long_string_array->lvalue)[i] = long_datum[i];
	}
	long_string_array->svalue.length(string_datum.size());
	for (int i=0; i<string_datum.size(); i++)
	{
		(long_string_array->svalue)[i] = string_dup(string_datum[i].c_str());
	}
	any <<= long_string_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::extract (vector<long>, vector<string> &) - extract a pair of
//             vector<long>,vector<string> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::extract (vector<long> &long_datum, vector<string>& string_datum)
{

	if (any_is_null())
	{
		long_datum.resize(0);
		string_datum.resize(0);
	}
	else
	{
		const DevVarLongStringArray *long_string_array; 
		any >>= long_string_array;
		long_datum.resize(long_string_array->lvalue.length());
		for (int i=0; i<long_datum.size(); i++)
		{
			long_datum[i] = (long_string_array->lvalue)[i];
		}
		string_datum.resize(long_string_array->svalue.length());
		for (int i=0; i<string_datum.size(); i++)
		{
			string_datum[i] = (long_string_array->svalue)[i];
		}
	}
	return;
}

//-----------------------------------------------------------------------------
//
// DeviceData::insert (vector<double>, vector<string> &) - insert a pair of
//             vector<double>,vector<string> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::insert (vector<double> &double_datum, vector<string>& string_datum)
{

	DevVarDoubleStringArray *double_string_array = new DevVarDoubleStringArray();
	double_string_array->dvalue.length(double_datum.size());
	for (int i=0; i<double_datum.size(); i++)
	{
		(double_string_array->dvalue)[i] = double_datum[i];
	}
	double_string_array->svalue.length(string_datum.size());
	for (int i=0; i<string_datum.size(); i++)
	{
		(double_string_array->svalue)[i] = string_dup(string_datum[i].c_str());
	}
	any <<= double_string_array;
}

//-----------------------------------------------------------------------------
//
// DeviceData::extract (vector<double>, vector<string> &) - extract a pair of
//             vector<double>,vector<string> into DeviceData
//
//-----------------------------------------------------------------------------

void DeviceData::extract (vector<double> &double_datum, vector<string>& string_datum)
{

	if (any_is_null())
	{
		double_datum.resize(0);
		string_datum.resize(0);
	}
	else
	{
		const DevVarDoubleStringArray *double_string_array; 
		any >>= double_string_array;
		double_datum.resize(double_string_array->dvalue.length());
		for (int i=0; i<double_datum.size(); i++)
		{
			double_datum[i] = (double_string_array->dvalue)[i];
		}
		string_datum.resize(double_string_array->svalue.length());
		for (int i=0; i<string_datum.size(); i++)
		{
			string_datum[i] = (double_string_array->svalue)[i];
		}
	}
	return;
}

