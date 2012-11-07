#ifndef MiscTestSuite_h
#define MiscTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME MiscTestSuite

class MiscTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device, *dserver;
	string device_name, full_ds_name, server_host, doc_url, dev_type;
	DevLong server_version;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string dserver_name;

		device_name = CxxTest::TangoPrinter::get_uarg("device");

		full_ds_name = CxxTest::TangoPrinter::get_param("fulldsname");
		dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");
		server_host = CxxTest::TangoPrinter::get_param("serverhost");
		server_version = atoi(CxxTest::TangoPrinter::get_param("serverversion").c_str());
		doc_url = CxxTest::TangoPrinter::get_param("docurl");
		dev_type = CxxTest::TangoPrinter::get_param("devtype");

		CxxTest::TangoPrinter::validate_args();

// For Windows / Linux compatibility
		string::size_type pos = server_host.find('.');
		pos--;
		if (server_host[pos] == 0x0d)
		{
			server_host.erase(pos,1);
		}


//
// Initialization --------------------------------------------------
//

		try
		{
			device = new DeviceProxy(device_name);
			dserver = new DeviceProxy(dserver_name);
			device->ping();
			dserver->ping();
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{
		// set the Tango::ON state on suite tearDown() in case a test fails leaving Tango::OFF status
		DeviceData din;
		din << device_name;
		try
		{
			dserver->command_inout("DevRestart", din);
		}
		catch(CORBA::Exception &e)
		{
			cout << endl << "Exception in suite tearDown():" << endl;
			Except::print_exception(e);
			exit(-1);
		}

		delete device;
		delete dserver;
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

// Test DevState and DevStatus commnds

	void test_DevState_and_DevStatus_commands(void)
	{
		DeviceData dout;
		string str;
		DevState state;
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("Status"));
		dout >> str;
		TS_ASSERT(str == "The device is in ON state.");

		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
	}

// Test DevRestart commnd

	void test_DevRestart_command(void)
	{
		DeviceData din, dout;
		DevState state_in = Tango::OFF, state_out;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::OFF);

		din << device_name;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("DevRestart", din));
		Tango_sleep(3);
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::ON);
	}

// Test name, description, state and status CORBA attributes

	void test_name_description_state_and_status_read_as_CORBA_attributes(void)
	{
		DeviceData dout;
		string str;
		DevState state;

		TS_ASSERT(device->name() == device_name);
		TS_ASSERT(device->description() == "A TANGO device");

		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("Status"));
		dout >> str;
		TS_ASSERT(str == "The device is in ON state.");

		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
	}

// Ping the device

	void test_ping_the_device(void)
	{
		TS_ASSERT_THROWS_NOTHING(device->ping());
	}

// Test info call

	void test_info_call(void)
	{
		TS_ASSERT(device->info().dev_class == "DevTest");
		TS_ASSERT(device->info().dev_type == dev_type);
		TS_ASSERT(device->info().doc_url == "Doc URL = " + doc_url);
		TS_ASSERT(device->info().server_host == server_host);
		TS_ASSERT(device->info().server_id == full_ds_name);
		TS_ASSERT(device->info().server_version == server_version);
	}
};
#undef cout
#endif // MiscTestSuite_h
