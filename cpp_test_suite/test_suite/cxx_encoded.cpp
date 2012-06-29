#ifndef EncodedTestSuite_h
#define EncodedTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>
#include <ctime>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME EncodedTestSuite

class EncodedTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device;
	string device_name;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		device_name = CxxTest::TangoPrinter::get_uarg("device");

		CxxTest::TangoPrinter::validate_args();


// Initialization --------------------------------------------------


		try
		{
			device = new DeviceProxy(device_name);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{

//
// Clean up --------------------------------------------------------
//

		delete device;
	}

	static SUITE_NAME *createSuite()
	{
		return new SUITE_NAME();
	}

	static void destroySuite(SUITE_NAME *suite)
	{
		delete suite;
	}

//
// Tests -------------------------------------------------------
//


// Miscellaneous inserters and extracters for DeviceData object

	void test_Miscellaneous_inserters_and_extracters_for_DeviceData()
	{

		Tango::DeviceData dd_in,dd_out;

		Tango::DevEncoded de,de_out;
		de.encoded_format = "Hola";
		de.encoded_data.length(2);
		de.encoded_data[0] = 0x10;
		de.encoded_data[1] = 0x11;

		dd_in << de;
		dd_out = device->command_inout("IOEncoded",dd_in);
		dd_out >> de_out;

		string tmp_str(de_out.encoded_format);
		TS_ASSERT(tmp_str == "Returned string");
		TS_ASSERT(de_out.encoded_data.length() == 2)
		TS_ASSERT(de_out.encoded_data[0] == 0x20);
		TS_ASSERT(de_out.encoded_data[1] == 0x22);

//-----------------

		string sent_str("Bonjour");
		vector<unsigned char> sent_data;
		sent_data.push_back(0x20);
		sent_data.push_back(0x21);
		sent_data.push_back(0x22);

		dd_in.insert(sent_str,sent_data);
		dd_out = device->command_inout("IOEncoded",dd_in);

		string received_str;
		vector<unsigned char> received_data;
		dd_out.extract(received_str,received_data);

		TS_ASSERT(received_str == "Returned string");
		TS_ASSERT(received_data.size() == 3)
		TS_ASSERT(received_data[0] == 0x40);
		TS_ASSERT(received_data[1] == 0x42);
		TS_ASSERT(received_data[2] == 0x44);

//---------------

		Tango::DevVarCharArray dvca;
		dvca.length(1);
		dvca[0] = 0x10;

		dd_in.insert("Hi",&dvca);
		dd_out = device->command_inout("IOEncoded",dd_in);

		const char *received_str_ptr;
		const unsigned char *received_data_ptr;
		unsigned int received_data_length;

		dd_out.extract(received_str_ptr,received_data_ptr,received_data_length);

		string t_str(received_str_ptr);

		TS_ASSERT(t_str == "Returned string");
		TS_ASSERT(received_data_length == 1)
		TS_ASSERT(received_data_ptr[0] == 0x20);

//-----------

		const char *sent_ptr = "Salut";
		unsigned char *sent_data_ptr = new unsigned char[4];
		sent_data_ptr[0] = 0x0;
		sent_data_ptr[1] = 0x1;
		sent_data_ptr[2] = 0x2;
		sent_data_ptr[3] = 0x3;
		unsigned int sent_length = 4;

		dd_in.insert(sent_ptr,sent_data_ptr,sent_length);
		dd_out = device->command_inout("IOEncoded",dd_in);

		delete [] sent_data_ptr;
	}

// Miscellaneous inserters and extracters for DeviceData object

	void test_Miscellaneous_inserters_and_extracters_for_DeviceAttribute()
	{
		Tango::DeviceAttribute da_in,da_out;
		Tango::DevEncoded att_de,att_de_out;

		da_out = device->read_attribute("Encoded_attr");

		da_out >> att_de_out;

//-------

		da_out = device->read_attribute("Encoded_attr");

		string att_read_str;
		vector<unsigned char> att_read_data;

		da_out.extract(att_read_str,att_read_data);

//-------

		char *att_read_ptr;
		unsigned char *att_read_data_ptr;
		unsigned int att_data_length;

		da_out = device->read_attribute("Encoded_attr");
		da_out.extract(att_read_ptr,att_read_data_ptr,att_data_length);

		CORBA::string_free(att_read_ptr);
		delete [] att_read_data_ptr;

//-------
	
		att_de.encoded_format = "Hola";
		att_de.encoded_data.length(2);
		att_de.encoded_data[0] = 0x10;
		att_de.encoded_data[1] = 0x11;

		da_in << att_de;
		da_in.set_name("Encoded_attr");

		device->write_attribute(da_in);

		string written_str("Adios");
		vector<unsigned char> written_data;
		written_data.push_back(0x0);
		written_data.push_back(0x1);
		written_data.push_back(0x2);

		da_in.insert(written_str,written_data);

		device->write_attribute(da_in);

//-------

		const char *written_str_ptr = "Bye";
		unsigned char *written_ptr = new unsigned char [2];
		written_ptr[0] = 0x10;
		written_ptr[1] = 0x11;
		unsigned int written_data_length = 2;

		da_in.insert(written_str_ptr,written_ptr,written_data_length);

		device->write_attribute(da_in);

		delete [] written_ptr;

		const char *written_p = "Au revoir";
		Tango::DevVarCharArray dvc;
		dvc.length(1);
		dvc[0] = 0x15;

		da_in.insert(written_p,&dvc);

		device->write_attribute(da_in);
	}
};
#undef cout
#endif // EncodedTestSuite_h
