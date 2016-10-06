#ifndef CmdTypesTestSuite_h
#define CmdTypesTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME CmdTypesTestSuite

class CmdTypesTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string device1_name;

		device1_name = CxxTest::TangoPrinter::get_param("device1");

		CxxTest::TangoPrinter::get_param_opt("loop");

		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

		try
		{
			device1 = new DeviceProxy(device1_name);
			device1->ping();
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{
		delete device1;
	}

	static SUITE_NAME *createSuite()
	{
		return new SUITE_NAME();
	}

	static void destroySuite(SUITE_NAME *suite)
	{
		delete suite;
	}


	void setUp(void)
	{
	}

	void tearDown(void)
	{
	}

//
// Tests -------------------------------------------------------
//

// Test Scalar Short

	void test_Scalar_Short(void) {
		DeviceAttribute da;
#ifndef COMPAT
		TS_ASSERT (da.get_data_format() == Tango::FMT_UNKNOWN);
#endif

		try
		{
			da = device1->read_attribute("Short_attr");
			TS_ASSERT(1==1);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		short sh;
		int data_type;
		try
		{
			data_type = da.get_type();
		}
		catch(CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		da >> sh;
		TS_ASSERT ( sh == 12 );
		TS_ASSERT ( data_type == Tango::DEV_SHORT );
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		TS_ASSERT ( data_format == Tango::SCALAR );
#endif
//		cout << "   Scalar short --> OK" << endl;
	}

// Test Scalar Short 2

	void test_Scalar_Short_DevEncoded_2__loop(void)
	{
		DeviceAttribute da;
#ifndef COMPAT
		TS_ASSERT (da.get_data_format() == Tango::FMT_UNKNOWN);
#endif

		try
		{
			da = device1->read_attribute("Short_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		short sh;
		int data_type = da.get_type();
		da >> sh;
		TS_ASSERT ( sh == 12 );
		TS_ASSERT ( data_type == Tango::DEV_SHORT );
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		TS_ASSERT ( data_format == Tango::SCALAR );
#endif
//		cout << "   Scalar short --> OK" << endl;
	}

// Test SCALAR long

	void test_Scalar_Long__loop(void)
	{
		DeviceAttribute da;
#ifndef COMPAT
		TS_ASSERT (da.get_data_format() == Tango::FMT_UNKNOWN);
#endif
		try
		{
			da = device1->read_attribute("Long_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevLong lo;
		da >> lo;
		int data_type = da.get_type();
		TS_ASSERT ( lo == 1246 );
		TS_ASSERT ( data_type == Tango::DEV_LONG );
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		TS_ASSERT ( data_format == Tango::SCALAR );
#endif
	}


// Test SCALAR double

	void test_Scalar_Double__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Double_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		double db;
		da >> db;
		TS_ASSERT ( db == 3.2 );
	}

// Test SCALAR string

	void test_Scalar_String__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("String_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		string str;
		da >> str;
		TS_ASSERT ( str == "test_string" );
	}

// Test SCALAR float

	void test_Scalar_Float__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Float_attr");
			float db;
			da >> db;
			TS_ASSERT ( db == 4.5 );
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
	}

// Test SCALAR boolean

	void test_Scalar_Boolean__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Boolean_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		bool db;
		da >> db;
		TS_ASSERT ( db == true );
	}

// Test SCALAR unsigned short

	void test_Scalar_Unsigned_Short__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("UShort_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		unsigned short db;
		da >> db;
		TS_ASSERT ( db == 111 );
	}

// Test SCALAR unsigned char

	void test_Scalar_Unsigned_Char__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("UChar_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		unsigned char db;
		da >> db;
		TS_ASSERT ( db == 88 );
	}

// Test SCALAR long 64 bits

	void test_Scalar_Long64__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Long64_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevLong64 lo;
		da >> lo;
		int data_type = da.get_type();

		TS_ASSERT ( lo == 0x800000000LL );
		TS_ASSERT ( data_type == Tango::DEV_LONG64 );
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		TS_ASSERT ( data_format == Tango::SCALAR );
#endif
	}

// Test SCALAR unsigned long

	void test_Scalar_Unsigned_Long__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("ULong_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevULong lo;
		da >> lo;
		int data_type = da.get_type();
		TS_ASSERT ( lo == 0xC0000000L );
		TS_ASSERT ( data_type == Tango::DEV_ULONG );
	}

// Test SCALAR unsigned long 64 bits

	void test_Scalar_Unsigned_Long64__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("ULong64_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevULong64 lo;
		da >> lo;
		int data_type = da.get_type();
		TS_ASSERT ( lo == 0xC000000000000000LL );
		TS_ASSERT ( data_type == Tango::DEV_ULONG64 );
	}

// Test SCALAR state

	void test_Scalar_State__loop(void)
	{
		DeviceAttribute da;
#ifndef COMPAT
		TS_ASSERT (da.get_data_format() == Tango::FMT_UNKNOWN);
#endif
		try
		{
			da = device1->read_attribute("State_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevState lo;
		da >> lo;
		int data_type = da.get_type();

		TS_ASSERT ( lo == Tango::FAULT );
		TS_ASSERT ( data_type == Tango::DEV_STATE );
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		TS_ASSERT ( data_format == Tango::SCALAR );
#endif
	}

// Test SCALAR DevEncoded

#ifndef COMPAT
	void test_Scalar_DevEncoded__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Encoded_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevEncoded lo;
		da >> lo;
		int data_type = da.get_type();

		TS_ASSERT ( ::strcmp(lo.encoded_format.in(),"Which format?") == 0 );
		TS_ASSERT ( data_type == Tango::DEV_ENCODED );
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		TS_ASSERT ( data_format == Tango::SCALAR );
#endif
		TS_ASSERT ( lo.encoded_data.length() == 4 );
		TS_ASSERT ( lo.encoded_data[0] == 97 );
		TS_ASSERT ( lo.encoded_data[1] == 98 );
		TS_ASSERT ( lo.encoded_data[2] == 99 );
		TS_ASSERT ( lo.encoded_data[3] == 100 );
	}

// Test SCALAR DevEncoded (JPEG)

	void test_Scalar_DevEncoded_JPEG__loop(void)
	{
		DeviceAttribute da;
		EncodedAttribute att;
		int width,height;
		unsigned char *gray8;

		try
		{
			da = device1->read_attribute("Encoded_image");
			att.decode_gray8( &da, &width, &height, &gray8 );
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

		TS_ASSERT ( width == 256 );
		TS_ASSERT ( height == 256 );
		// Check a pixel (margin of 4 levels for jpeg loss)
		TS_ASSERT ( gray8[128+128*256] >= 124 );
		TS_ASSERT ( gray8[128+128*256] <= 132 );
		delete [] gray8;

	}
#endif

// Thirteen in one go

	void test_Thirteen_in_one_call__loop(void)
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
			received = device1->read_attributes(names);
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
		TS_ASSERT ( sh == 12 );
		(*received)[1] >> lo;
		TS_ASSERT ( lo == 1246 );
		(*received)[2] >> db;
		TS_ASSERT ( db == 3.2 );
		(*received)[3] >> str;
		TS_ASSERT ( str == "test_string" );
		(*received)[4] >> fl;
		TS_ASSERT ( fl == 4.5 );
		(*received)[5] >> bo;
		TS_ASSERT ( bo == true );
		(*received)[6] >> ush;
		TS_ASSERT ( ush == 111 );
		(*received)[7] >> uch;
		TS_ASSERT ( uch == 88 );
		(*received)[8] >> lo64;
		TS_ASSERT ( lo64 == 0x800000000LL );
		(*received)[9] >> ulo;
		TS_ASSERT ( ulo == 0xC0000000L );
		(*received)[10] >> ulo64;
		TS_ASSERT ( ulo64 == 0xC000000000000000LL );
		(*received)[11] >> sta;
		TS_ASSERT ( sta == Tango::FAULT );
#ifndef COMPAT
		(*received)[12] >> enc;
		TS_ASSERT ( enc.encoded_data.length() == 4 );
		TS_ASSERT ( ::strcmp(enc.encoded_format.in(),"Which format?") == 0 );
#endif

		delete received;
	}


//
//---------------------------------------------------------------------------------------------
//


// Test SPECTRUM short

	void test_Spectrum_Short__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Short_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<short> sh;
		bool ret = (da >> sh);

		TS_ASSERT (ret == true);
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		TS_ASSERT ( data_format == Tango::SPECTRUM);
#endif
		TS_ASSERT ( sh[0] == 10 );
		TS_ASSERT ( sh[1] == 20 );
		TS_ASSERT ( sh[2] == 30 );
		TS_ASSERT ( sh[3] == 40 );
	}

// Test SPECTRUM long

	void test_Spectrum_Long__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Long_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<DevLong> lo;
		bool ret = (da >> lo);

		TS_ASSERT (ret == true);
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		TS_ASSERT ( data_format == Tango::SPECTRUM);
#endif
		TS_ASSERT ( lo[0] == 0 );
		TS_ASSERT ( lo[1] == 1 );
		TS_ASSERT ( lo[2] == 2 );
		TS_ASSERT ( lo[3] == 3 );
		TS_ASSERT ( lo[9] == 9 );
	}

// Test SPECTRUM double

	void test_Spectrum_Double__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Double_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<double> db;
		bool ret = (da >> db);
		int data_type = da.get_type();

		TS_ASSERT ( ret == true);
		TS_ASSERT ( db[0] == 1.11 );
		TS_ASSERT ( db[1] == 2.22 );
		TS_ASSERT ( data_type == Tango::DEV_DOUBLE );
	}

// Test SPECTRUM string

	void test_Spectrum_String__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("String_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<string> str;
		bool ret = (da >> str);

		TS_ASSERT ( ret == true );
		TS_ASSERT ( str[0] == "Hello world" );
		TS_ASSERT ( str[1] == "Hello universe" );
	}

// Test SPECTRUM float

	void test_Spectrum_Float__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Float_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<float> sh;
		bool ret = (da >> sh);

		TS_ASSERT (ret == true);
		TS_ASSERT ( sh[0] == 4.5 );
		TS_ASSERT ( sh[1] == 8.5 );
		TS_ASSERT ( sh[2] == 16.5 );
	}

// Test SPECTRUM boolean

	void test_Spectrum_Boolean__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Boolean_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<bool> sh;
		bool ret = (da >> sh);

		TS_ASSERT (ret == true);
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		TS_ASSERT ( data_format == Tango::SPECTRUM );
#endif
		TS_ASSERT ( sh[0] == true );
		TS_ASSERT ( sh[1] == true );
		TS_ASSERT ( sh[2] == false );
		TS_ASSERT ( sh[3] == true );
		TS_ASSERT ( sh[4] == true );
	}

// Test SPECTRUM unsigned short

	void test_Spectrum_Unsigned_Short__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("UShort_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<unsigned short> sh;
		bool ret = (da >> sh);

		TS_ASSERT (ret == true);
		TS_ASSERT ( sh[0] == 333 );
		TS_ASSERT ( sh[1] == 444 );
	}

// Test SPECTRUM unsigned char

	void test_Spectrum_Unsigned_Char__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("UChar_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<unsigned char> sh;
		bool ret = (da >> sh);

		TS_ASSERT (ret == true);
		TS_ASSERT ( sh[0] == 28 );
		TS_ASSERT ( sh[1] == 45 );
		TS_ASSERT ( sh[2] == 156 );
		TS_ASSERT ( sh[3] == 34 );
		TS_ASSERT ( sh[4] == 200 );
		TS_ASSERT ( sh[5] == 12 );
	}

// Test SPECTRUM long 64 bits

	void test_Spectrum_Long64__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Long64_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<DevLong64> lo;
		bool ret = (da >> lo);

		TS_ASSERT (ret == true);
		TS_ASSERT ( lo[0] == 1000 );
		TS_ASSERT ( lo[1] == 10000 );
		TS_ASSERT ( lo[2] == 100000 );
		TS_ASSERT ( lo[3] == 0);
	}

// Test SPECTRUM unsigned long

	void test_Spectrum_Unsigned_Long__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("ULong_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<DevULong> lo;
		bool ret = (da >> lo);

		TS_ASSERT (ret == true);
		TS_ASSERT ( lo[0] == 2222 );
		TS_ASSERT ( lo[1] == 22222 );
		TS_ASSERT ( lo[2] == 222222 );
		TS_ASSERT ( lo[3] == 0);
	}

// Test SPECTRUM unsigned long 64 bits

	void test_Spectrum_Unsigned_Long64__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("ULong64_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<DevULong64> lo;
		bool ret = (da >> lo);

		TS_ASSERT (ret == true);
		TS_ASSERT ( lo[0] == 8888 );
		TS_ASSERT ( lo[1] == 88888 );
		TS_ASSERT ( lo[2] == 888888 );
		TS_ASSERT ( lo[3] == 0);
	}

// Test SPECTRUM state

	void test_Spectrum_State__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("State_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<DevState> lo;
		bool ret = (da >> lo);

		TS_ASSERT (ret == true);
		TS_ASSERT ( lo[0] == Tango::ON );
		TS_ASSERT ( lo[1] == Tango::OFF );
		TS_ASSERT ( lo[2] == Tango::UNKNOWN );
	}

//
//-----------------------------------------------------------------------------------------
//


// Test SPECTRUM short (DevVarShortArray)

	void test_Spectrum_Short_DevVarShortArray__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Short_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarShortArray *sh;
		da >> sh;
		TS_ASSERT ( (*sh)[0] == 10 );
		TS_ASSERT ( (*sh)[1] == 20 );
		TS_ASSERT ( (*sh)[2] == 30 );
		TS_ASSERT ( (*sh)[3] == 40 );

		delete sh;
	}

// Test SPECTRUM long

	void test_Spectrum_Long_DevVarLongArray__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Long_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarLongArray *lo;
		da >> lo;
		TS_ASSERT ( (*lo)[0] == 0 );
		TS_ASSERT ( (*lo)[3] == 3 );
		TS_ASSERT ( (*lo)[6] == 6 );
		TS_ASSERT ( (*lo)[9] == 9 );

		delete lo;
	}

// Test SPECTRUM double

	void test_Spectrum_Double_DevVarDoubleArray__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Double_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarDoubleArray *db;
		da >> db;
		TS_ASSERT ( (*db)[0] == 1.11 );
		TS_ASSERT ( (*db)[1] == 2.22 );

		delete db;
	}

// Test SPECTRUM string

	void test_Spectrum_String_DevVarStringArray__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("String_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarStringArray *str;
		da >> str;
		TS_ASSERT ( !strcmp((*str)[0],"Hello world") );
		TS_ASSERT ( !strcmp((*str)[1],"Hello universe") );

		delete str;
	}

// Test SPECTRUM float

	void test_Spectrum_Float_DevVarFloatArray__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Float_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarFloatArray *lo;
		da >> lo;
		TS_ASSERT ( (*lo)[0] == 4.5 );
		TS_ASSERT ( (*lo)[1] == 8.5 );
		TS_ASSERT ( (*lo)[2] == 16.5 );

		delete lo;
	}

// Test SPECTRUM boolean

	void test_Spectrum_Boolean_DevVarBooleanArray__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Boolean_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarBooleanArray *lo;
		int data_type = da.get_type();
		da >> lo;

		TS_ASSERT ( (*lo)[0] == true );
		TS_ASSERT ( (*lo)[1] == true );
		TS_ASSERT ( (*lo)[2] == false );
		TS_ASSERT ( (*lo)[3] == true );
		TS_ASSERT ( (*lo)[4] == true );
		TS_ASSERT ( data_type == Tango::DEV_BOOLEAN );

		delete lo;
	}

// Test SPECTRUM unsigned short

	void test_Spectrum_Unsigned_Short_DevVarUShortArray__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("UShort_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarUShortArray *lo;
		da >> lo;
		TS_ASSERT ( (*lo)[0] == 333 );
		TS_ASSERT ( (*lo)[1] == 444 );

		delete lo;
	}

// Test SPECTRUM unsigned char

	void test_Spectrum_Unsigned_Char_DevVarUCharArray__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("UChar_spec_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarCharArray *lo;
		da >> lo;
		TS_ASSERT ( (*lo)[0] == 28 );
		TS_ASSERT ( (*lo)[1] == 45 );
		TS_ASSERT ( (*lo)[2] == 156 );
		TS_ASSERT ( (*lo)[3] == 34 );
		TS_ASSERT ( (*lo)[4] == 200 );
		TS_ASSERT ( (*lo)[5] == 12 );

		delete lo;
	}

// Test SPECTRUM long 64 bits

	void test_Spectrum_Long64_DevVarLong64Array__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Long64_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarLong64Array *lo;
		bool ret = (da >> lo);

		TS_ASSERT (ret == true);
		TS_ASSERT ( (*lo)[0] == 1000 );
		TS_ASSERT ( (*lo)[1] == 10000 );
		TS_ASSERT ( (*lo)[2] == 100000 );
		TS_ASSERT ( (*lo)[3] == 0 );

		delete lo;
	}

// Test SPECTRUM unsigned long

	void test_Spectrum_Unsigned_Long_DevVarULongArray__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("ULong_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarULongArray *lo;
		bool ret = (da >> lo);

		TS_ASSERT (ret == true);
		TS_ASSERT ( (*lo)[0] == 2222 );
		TS_ASSERT ( (*lo)[1] == 22222 );
		TS_ASSERT ( (*lo)[2] == 222222 );
		TS_ASSERT ( (*lo)[3] == 0 );

		delete lo;
	}

// Test SPECTRUM unsigned long 64 bits

	void test_Spectrum_Unsigned_Long64_DevVarULong64Array__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("ULong64_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarULong64Array *lo;
		bool ret = (da >> lo);

		TS_ASSERT (ret == true);
		TS_ASSERT ( (*lo)[0] == 8888 );
		TS_ASSERT ( (*lo)[1] == 88888 );
		TS_ASSERT ( (*lo)[2] == 888888 );
		TS_ASSERT ( (*lo)[3] == 0 );

		delete lo;
	}

// Test SPECTRUM state

	void test_Spectrum_State_DevVarStateArray__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("State_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarStateArray *lo;
		bool ret = (da >> lo);
		TS_ASSERT (ret == true);
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		TS_ASSERT ( data_format == Tango::SPECTRUM );
#endif
		TS_ASSERT ( (*lo)[0] == Tango::ON );
		TS_ASSERT ( (*lo)[1] == Tango::OFF );
		TS_ASSERT ( (*lo)[2] == Tango::UNKNOWN );

		delete lo;
	}

// Test IMAGE short

	void test_Image_Short_DevVarShortArray__loop(void)
	{
		DeviceAttribute da;
		try
		{
			da = device1->read_attribute("Short_ima_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevVarShortArray *lo;
		bool ret = (da >> lo);

		TS_ASSERT (ret == true);
#ifndef COMPAT
		AttrDataFormat data_format = da.get_data_format();
		TS_ASSERT ( data_format == Tango::IMAGE );
#endif

		delete lo;
	}

// Test exception on attribute data format unknown

	void test_Exception_Error_for_unknown_attribute_data_format(void)
	{
		bool except = false;
		DeviceAttribute db;
#ifndef COMPAT
		db.set_exceptions(DeviceAttribute::unknown_format_flag);

		try
		{
			db.get_data_format();
		}
		catch (Tango::DevFailed &e)
		{
			except = true;
		}
		TS_ASSERT (except == true );

		db.reset_exceptions(DeviceAttribute::unknown_format_flag);

		AttrDataFormat df = db.get_data_format();
		TS_ASSERT ( df == Tango::FMT_UNKNOWN );

#endif
	}

};
#undef cout
#endif // CmdTypesTestSuite_h
