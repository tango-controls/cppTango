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
#ifndef COMPAT
		assert (da.get_data_format() == Tango::FMT_UNKNOWN);
#endif
		
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
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		assert ( data_format == Tango::SCALAR );
#endif
	}
	cout << "   Scalar short --> OK" << endl;
		
// Test SCALAR long

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
#ifndef COMPAT
		assert (da.get_data_format() == Tango::FMT_UNKNOWN);
#endif
		try
		{	
			da = device->read_attribute("Long_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevLong lo;
		da >> lo;
		int data_type = da.get_type();
		assert ( lo == 1246 );
		assert ( data_type == Tango::DEV_LONG );
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		assert ( data_format == Tango::SCALAR );
#endif
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
			float db;
			da >> db;
			assert ( db == 4.5 );
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
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

// Test SCALAR long 64 bits

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Long64_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevLong64 lo;
		da >> lo;
		int data_type = da.get_type();
		
		assert ( lo == 0x800000000LL );
		assert ( data_type == Tango::DEV_LONG64 );
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		assert ( data_format == Tango::SCALAR );
#endif
	}
	cout << "   Scalar long 64 bits --> OK" << endl;
	
// Test SCALAR unsigned long 

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("ULong_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevULong lo;
		da >> lo;
		int data_type = da.get_type();
		assert ( lo == 0xC0000000L );
		assert ( data_type == Tango::DEV_ULONG );
	}
	cout << "   Scalar unsigned long --> OK" << endl;
	
// Test SCALAR unsigned long 64 bits

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("ULong64_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevULong64 lo;
		da >> lo;
		int data_type = da.get_type();
		assert ( lo == 0xC000000000000000LL );
		assert ( data_type == Tango::DEV_ULONG64 );
	}
	cout << "   Scalar unsigned long 64 bits --> OK" << endl;
	
// Test SCALAR state

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
#ifndef COMPAT
		assert (da.get_data_format() == Tango::FMT_UNKNOWN);
#endif
		try
		{	
			da = device->read_attribute("State_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevState lo;
		da >> lo;
		int data_type = da.get_type();
		
		assert ( lo == Tango::FAULT );
		assert ( data_type == Tango::DEV_STATE );
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		assert ( data_format == Tango::SCALAR );
#endif
	}
	cout << "   Scalar state --> OK" << endl;

// Test SCALAR DevEncoded

#ifndef COMPAT
	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Encoded_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevEncoded lo;
		da >> lo;
		int data_type = da.get_type();
		
		assert ( ::strcmp(lo.encoded_format.in(),"Which format?") == 0 );
		assert ( data_type == Tango::DEV_ENCODED );
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		assert ( data_format == Tango::SCALAR );
#endif
		assert ( lo.encoded_data.length() == 4 );
		assert ( lo.encoded_data[0] == 97 );
		assert ( lo.encoded_data[1] == 98 );
		assert ( lo.encoded_data[2] == 99 );
		assert ( lo.encoded_data[3] == 100 );
	}
	cout << "   Scalar DevEncoded --> OK" << endl;
	
	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		EncodedAttribute att;
		int width,height;
		unsigned char *gray8;
		
		try
		{	
			da = device->read_attribute("Encoded_image");
			att.decode_gray8( &da, &width, &height, &gray8 );
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		
		assert ( width == 256 );
		assert ( height == 256 );
		// Check a pixel (margin of 4 levels for jpeg loss)
		assert ( gray8[128+128*256] >= 124 );
		assert ( gray8[128+128*256] <= 132 );
		delete gray8;
		
	}
	cout << "   Scalar DevEncoded (JPEG) --> OK" << endl;
#endif
					
// Thirteen in one go

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
		names.push_back("Long64_attr_rw");
		names.push_back("ULong_attr_rw");
		names.push_back("ULong64_attr_rw");
		names.push_back("State_attr_rw");
#ifndef COMPAT
		names.push_back("Encoded_attr");

		DevEncoded enc;
#endif
						
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
		DevLong lo;
		short sh;
		double db;
		float fl;
		bool bo;
		unsigned short ush;
		unsigned char uch;
		DevLong64 lo64;
		DevULong ulo;
		DevULong64 ulo64;
		DevState sta;
		
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
		(*received)[8] >> lo64;
		assert ( lo64 == 0x800000000LL );
		(*received)[9] >> ulo;
		assert ( ulo == 0xC0000000L );
		(*received)[10] >> ulo64;
		assert ( ulo64 == 0xC000000000000000LL );
		(*received)[11] >> sta;
		assert ( sta == Tango::FAULT );
#ifndef COMPAT
		(*received)[12] >> enc;
		assert ( enc.encoded_data.length() == 4 );
		assert ( ::strcmp(enc.encoded_format.in(),"Which format?") == 0 );
#endif
						
		delete received;
	}
	
	cout << "   Thirteen in one call --> OK" << endl;
	
//
//---------------------------------------------------------------------------------------------
//

	
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
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		assert ( data_format == Tango::SPECTRUM);
#endif
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
		vector<DevLong> lo;
		bool ret = (da >> lo);
		
		assert (ret == true);
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		assert ( data_format == Tango::SPECTRUM);
#endif
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
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		assert ( data_format == Tango::SPECTRUM );
#endif
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
	
// Test SPECTRUM long 64 bits

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Long64_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<DevLong64> lo;
		bool ret = (da >> lo);
		
		assert (ret == true);
		assert ( lo[0] == 1000 );
		assert ( lo[1] == 10000 );
		assert ( lo[2] == 100000 );
		assert ( lo[3] == 0);
	}
	cout << "   Spectrum long 64 bits (C++ vector) --> OK" << endl;
	
// Test SPECTRUM unsigned long

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("ULong_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<DevULong> lo;
		bool ret = (da >> lo);
		
		assert (ret == true);
		assert ( lo[0] == 2222 );
		assert ( lo[1] == 22222 );
		assert ( lo[2] == 222222 );
		assert ( lo[3] == 0);
	}
	cout << "   Spectrum unsigned long (C++ vector) --> OK" << endl;
	
// Test SPECTRUM unsigned long 64 bits

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("ULong64_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<DevULong64> lo;
		bool ret = (da >> lo);
		
		assert (ret == true);
		assert ( lo[0] == 8888 );
		assert ( lo[1] == 88888 );
		assert ( lo[2] == 888888 );
		assert ( lo[3] == 0);
	}
	cout << "   Spectrum unsigned long 64 bits (C++ vector) --> OK" << endl;
	
// Test SPECTRUM state

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("State_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<DevState> lo;
		bool ret = (da >> lo);
		
		assert (ret == true);
		assert ( lo[0] == Tango::ON );
		assert ( lo[1] == Tango::OFF );
		assert ( lo[2] == Tango::UNKNOWN );
	}
	cout << "   Spectrum state (C++ vector) --> OK" << endl;
	
//
//-----------------------------------------------------------------------------------------
//

		
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
	
// Test SPECTRUM long 64 bits

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Long64_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarLong64Array *lo;
		bool ret = (da >> lo);
		
		assert (ret == true);
		assert ( (*lo)[0] == 1000 );
		assert ( (*lo)[1] == 10000 );
		assert ( (*lo)[2] == 100000 );
		assert ( (*lo)[3] == 0 );
		
		delete lo;
	}
	cout << "   Spectrum long 64 bits (DevVarLong64Array) --> OK" << endl;
	
// Test SPECTRUM unsigned long

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("ULong_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarULongArray *lo;
		bool ret = (da >> lo);
		
		assert (ret == true);
		assert ( (*lo)[0] == 2222 );
		assert ( (*lo)[1] == 22222 );
		assert ( (*lo)[2] == 222222 );
		assert ( (*lo)[3] == 0 );
		
		delete lo;
	}
	cout << "   Spectrum unsigned long (DevVarULongArray) --> OK" << endl;
	
// Test SPECTRUM unsigned long 64 bits

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("ULong64_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarULong64Array *lo;
		bool ret = (da >> lo);
		
		assert (ret == true);
		assert ( (*lo)[0] == 8888 );
		assert ( (*lo)[1] == 88888 );
		assert ( (*lo)[2] == 888888 );
		assert ( (*lo)[3] == 0 );
		
		delete lo;
	}
	cout << "   Spectrum unsigned long 64 bits (DevVarULong64Array) --> OK" << endl;
	
// Test SPECTRUM state

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("State_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarStateArray *lo;	
		bool ret = (da >> lo);
		assert (ret == true);
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		assert ( data_format == Tango::SPECTRUM );
#endif
		assert ( (*lo)[0] == Tango::ON );
		assert ( (*lo)[1] == Tango::OFF );
		assert ( (*lo)[2] == Tango::UNKNOWN );
		
		delete lo;
	}
	cout << "   Spectrum state (DevVarStateArray) --> OK" << endl;
	
// Test IMAGE short

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Short_ima_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarShortArray *lo;	
		bool ret = (da >> lo);

		assert (ret == true);
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		assert ( data_format == Tango::IMAGE );
#endif
		
		delete lo;
	}
	cout << "   Image short (DevVarShortArray) --> OK" << endl;

// Test exception on attribute data format unknown

	bool except = false;
	DeviceAttribute db;
#ifndef COMPAT	
	db.set_exceptions(DeviceAttribute::unknown_format_flag);

	try
	{
		AttrDataFormat df = db.get_data_format();
	}
	catch (Tango::DevFailed &e)
	{
		except = true;
	}	
	assert (except == true );

	db.reset_exceptions(DeviceAttribute::unknown_format_flag);	

	AttrDataFormat df = db.get_data_format();
	assert ( df == Tango::FMT_UNKNOWN );
	
	cout << "   Exception/Error for unknown attribute data format --> OK" << endl;
#endif
			
	delete device;
	
	return 0;
	
}
