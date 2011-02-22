/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc != 3)
	{
		cout << "usage: %s device loop" << endl;
		exit(-1);
	}

	string device_name = argv[1];
	int loop = atoi(argv[2]);

	try 
	{
		device = new DeviceProxy(device_name);
	}
        catch (CORBA::Exception &e)
        {
              	Except::print_exception(e);
		exit(1);
        }

	cout << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;
	int i;

// Test SCALAR short

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Short_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		short sh;
		int data_type = da.get_type();
		da >> sh;
		assert ( sh == 12 );
		assert ( data_type == Tango::DEV_SHORT );
	}
	cout << "   Scalar short --> OK" << endl;
		
// Test SCALAR long

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Long_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		long lo;
		da >> lo;
		int data_type = da.get_type();
		assert ( lo == 1246 );
		assert ( data_type == Tango::DEV_LONG );
	}
	cout << "   Scalar long --> OK" << endl;

// Test SCALAR double

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Double_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		double db;
		da >> db;
		assert ( db == 3.2 );
	}
	cout << "   Scalar double --> OK" << endl;

// Test SCALAR string

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("String_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		string str;
		da >> str;
		assert ( str == "test_string" );
	}
	cout << "   Scalar C++ string --> OK" << endl;

// Test SCALAR float

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Float_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		float db;
		da >> db;
		assert ( db == 4.5 );
	}
	cout << "   Scalar float --> OK" << endl;

// Test SCALAR boolean

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Boolean_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		bool db;
		da >> db;
		assert ( db == true );
	}
	cout << "   Scalar boolean --> OK" << endl;

// Test SCALAR unsigned short

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("UShort_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		unsigned short db;
		da >> db;
		assert ( db == 111 );
	}
	cout << "   Scalar unsigned short --> OK" << endl;

// Test SCALAR unsigned char

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("UChar_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		unsigned char db;
		da >> db;
		assert ( db == 88 );
	}
	cout << "   Scalar unsigned char --> OK" << endl;
					
// Eight in one go

	for (i = 0;i < loop;i++)
	{
		vector<string> names;
		names.push_back("Short_attr");
		names.push_back("Long_attr");
		names.push_back("Double_attr");
		names.push_back("String_attr");
		names.push_back("Float_attr");
		names.push_back("Boolean_attr");
		names.push_back("UShort_attr");
		names.push_back("UChar_attr");
				
		vector<DeviceAttribute> *received;
				
		try
		{
			received = device->read_attributes(names);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		string str;
		long lo;
		short sh;
		double db;
		float fl;
		bool bo;
		unsigned short ush;
		unsigned char uch;
		
		(*received)[0] >> sh;
		assert ( sh == 12 );
		(*received)[1] >> lo;
		assert ( lo == 1246 );
		(*received)[2] >> db;
		assert ( db == 3.2 );		
		(*received)[3] >> str;
		assert ( str == "test_string" );
		(*received)[4] >> fl;
		assert ( fl == 4.5 );
		(*received)[5] >> bo;
		assert ( bo == true );
		(*received)[6] >> ush;
		assert ( ush == 111 );		
		(*received)[7] >> uch;
		assert ( uch == 88 );
				
		delete received;
	}
	
	cout << "   Eight in one call --> OK" << endl;
	
// Test SPECTRUM short

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Short_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<short> sh;
		bool ret = (da >> sh);
		
		assert (ret == true);
		assert ( sh[0] == 10 );
		assert ( sh[1] == 20 );
		assert ( sh[2] == 30 );
		assert ( sh[3] == 40 );
	}
	cout << "   Spectrum short (C++ vector) --> OK" << endl;

// Test SPECTRUM long

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Long_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<long> lo;
		bool ret = (da >> lo);
		
		assert (ret == true);
		assert ( lo[0] == 0 );
		assert ( lo[1] == 1 );
		assert ( lo[2] == 2 );
		assert ( lo[3] == 3 );
		assert ( lo[9] == 9 );
	}
	cout << "   Spectrum long (C++ vector) --> OK" << endl;

// Test SPECTRUM double

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Double_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<double> db;
		bool ret = (da >> db);
		int data_type = da.get_type();
		
		assert ( ret == true);
		assert ( db[0] == 1.11 );
		assert ( db[1] == 2.22 );
		assert ( data_type == Tango::DEV_DOUBLE );
	}
	cout << "   Spectrum double (C++ vector) --> OK" << endl;
	
// Test SPECTRUM string

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("String_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<string> str;
		bool ret = (da >> str);
		
		assert ( ret == true );
		assert ( str[0] == "Hello world" );
		assert ( str[1] == "Hello universe" );
	}
	cout << "   Spectrum string (C++ vector) --> OK" << endl;

// Test SPECTRUM float

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Float_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<float> sh;
		bool ret = (da >> sh);
		
		assert (ret == true);
		assert ( sh[0] == 4.5 );
		assert ( sh[1] == 8.5 );
		assert ( sh[2] == 16.5 );
	}
	cout << "   Spectrum float (C++ vector) --> OK" << endl;

// Test SPECTRUM boolean

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Boolean_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<bool> sh;
		bool ret = (da >> sh);
		
		assert (ret == true);
		assert ( sh[0] == true );
		assert ( sh[1] == true );
		assert ( sh[2] == false );
		assert ( sh[3] == true );
		assert ( sh[4] == true );
	}
	cout << "   Spectrum boolean (C++ vector) --> OK" << endl;

// Test SPECTRUM unsigned short

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("UShort_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<unsigned short> sh;
		bool ret = (da >> sh);
		
		assert (ret == true);
		assert ( sh[0] == 333 );
		assert ( sh[1] == 444 );
	}
	cout << "   Spectrum unsigned short (C++ vector) --> OK" << endl;
	
// Test SPECTRUM unsigned char

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("UChar_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<unsigned char> sh;
		bool ret = (da >> sh);
		
		assert (ret == true);
		assert ( sh[0] == 28 );
		assert ( sh[1] == 45 );
		assert ( sh[2] == 156 );
		assert ( sh[3] == 34 );
		assert ( sh[4] == 200 );
		assert ( sh[5] == 12 );
	}
	cout << "   Spectrum unsigned char (C++ vector) --> OK" << endl;
		
// Test SPECTRUM short (DevVarShortArray)

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Short_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarShortArray *sh;
		da >> sh;
		assert ( (*sh)[0] == 10 );
		assert ( (*sh)[1] == 20 );
		assert ( (*sh)[2] == 30 );
		assert ( (*sh)[3] == 40 );
		
		delete sh;
	}
	cout << "   Spectrum short (DevVarShortArray) --> OK" << endl;

// Test SPECTRUM long

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Long_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarLongArray *lo;
		da >> lo;
		assert ( (*lo)[0] == 0 );
		assert ( (*lo)[3] == 3 );
		assert ( (*lo)[6] == 6 );
		assert ( (*lo)[9] == 9 );
		
		delete lo;
	}
	cout << "   Spectrum long (DevVarLongArray) --> OK" << endl;

// Test SPECTRUM double

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Double_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarDoubleArray *db;
		da >> db;
		assert ( (*db)[0] == 1.11 );
		assert ( (*db)[1] == 2.22 );
		
		delete db;
	}
	cout << "   Spectrum double (DevVarDoubleArray) --> OK" << endl;

// Test SPECTRUM string

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("String_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarStringArray *str;
		da >> str;
		assert ( !strcmp((*str)[0],"Hello world") );
		assert ( !strcmp((*str)[1],"Hello universe") );
		
		delete str;
	}
	cout << "   Spectrum string (DevVarStringArray) --> OK" << endl;

// Test SPECTRUM float

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Float_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarFloatArray *lo;
		da >> lo;
		assert ( (*lo)[0] == 4.5 );
		assert ( (*lo)[1] == 8.5 );
		assert ( (*lo)[2] == 16.5 );
		
		delete lo;
	}
	cout << "   Spectrum float (DevVarFloatArray) --> OK" << endl;
	
// Test SPECTRUM boolean

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Boolean_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarBooleanArray *lo;
		int data_type = da.get_type();
		da >> lo;
		
		assert ( (*lo)[0] == true );
		assert ( (*lo)[1] == true );
		assert ( (*lo)[2] == false );
		assert ( (*lo)[3] == true );
		assert ( (*lo)[4] == true );
		assert ( data_type == Tango::DEV_BOOLEAN );
				
		delete lo;
	}
	cout << "   Spectrum boolean (DevVarBooleanArray) --> OK" << endl;
				
// Test SPECTRUM unsigned short

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("UShort_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarUShortArray *lo;
		da >> lo;
		assert ( (*lo)[0] == 333 );
		assert ( (*lo)[1] == 444 );
				
		delete lo;
	}
	cout << "   Spectrum unsigned short (DevVarUShortArray) --> OK" << endl;
	
// Test SPECTRUM unsigned char

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("UChar_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarCharArray *lo;
		da >> lo;
		assert ( (*lo)[0] == 28 );
		assert ( (*lo)[1] == 45 );
		assert ( (*lo)[2] == 156 );
		assert ( (*lo)[3] == 34 );
		assert ( (*lo)[4] == 200 );
		assert ( (*lo)[5] == 12 );
				
		delete lo;
	}
	cout << "   Spectrum unsigned char (DevVarCharArray) --> OK" << endl;
	
	delete device;
	
	return 0;
	
}
