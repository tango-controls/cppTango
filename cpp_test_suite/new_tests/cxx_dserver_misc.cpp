#ifndef DServerMiscTestSuite_h
#define DServerMiscTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME DServerMiscTestSuite

class DServerMiscTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;
	string device1_name, dserver_name, full_ds_name, server_host, doc_url;
	DevLong server_version;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		device1_name = CxxTest::TangoPrinter::get_param("device1");
		full_ds_name = CxxTest::TangoPrinter::get_param("fulldsname");
		dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");
		server_host = CxxTest::TangoPrinter::get_param("serverhost");
		server_version = atoi(CxxTest::TangoPrinter::get_param("serverversion").c_str());
		doc_url = CxxTest::TangoPrinter::get_param("docurl");

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
			device1 = new DeviceProxy(device1_name);
			dserver = new DeviceProxy(dserver_name);
			device1->ping();
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
		din << device1_name;
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

		delete device1;
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

// Test State and Status commands

	void test_State_and_Status_commands(void)
	{
		DeviceData dout;
		DevState state;
		string str;

		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("Status"));
		dout >> str;
		TS_ASSERT(str == "The device is ON\nThe polling is ON");

		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
	}

// Test DevRestart command on the dserver device

	void test_DevRestart_command_on_the_dserver_device(void)
	{
		DeviceData din, dout;
		DevState state_in, state_out;
		string str;

		state_in = Tango::OFF;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::OFF);

		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RestartServer"));
		Tango_sleep(3);

		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::ON);
	}

// Test DevRestart command on classical device

	void test_DevRestart_command_on_classical_device(void)
	{
		DeviceData din, dout;
		DevState state_in, state_out;
		string str;

		str = "a/b/c";
		din << str;
		TS_ASSERT_THROWS_ASSERT(dserver->command_inout("DevRestart", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_DeviceNotFound"
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::OFF;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::OFF);

		str = device1_name;
		din << str;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RestartServer", din));
		Tango_sleep(6);

		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::ON);
	}

// Test name, description, state ans status CORBA attributes

	void test_name_description_state_ans_status_CORBA_attributes(void)
	{
		DeviceData dout;
		DevState state_out;
		string str;

		TS_ASSERT_THROWS_NOTHING(str = dserver->name());
		TS_ASSERT(str == dserver_name);

		TS_ASSERT_THROWS_NOTHING(str = dserver->description());
		TS_ASSERT(str == "A device server device !!");

		TS_ASSERT_THROWS_NOTHING(str = dserver->status());
cout << "str = " << str << endl;
		TS_ASSERT(str == "The device is ON\nThe polling is ON");

		TS_ASSERT_THROWS_NOTHING(state_out = dserver->state());
		TS_ASSERT(state_out == Tango::ON);
	}

// Ping the device

	void test_ping_the_device(void)
	{
		TS_ASSERT_THROWS_NOTHING(dserver->ping());
	}

// Test info call

	void test_info_call(void)
	{
		TS_ASSERT(dserver->info().dev_class == "DServer");
		// TODO: uncomment the following if needed
//		TS_ASSERT(dserver->info().dev_type == "Uninitialised");
		TS_ASSERT(dserver->info().doc_url == "Doc URL = " + doc_url);
		TS_ASSERT(dserver->info().server_host == server_host);
		TS_ASSERT(dserver->info().server_id == full_ds_name);
		TS_ASSERT(dserver->info().server_version == server_version);
	}
};
#undef cout
#endif // DServerMiscTestSuite_h
