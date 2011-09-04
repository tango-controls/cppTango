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
	DeviceProxy *device, *dserver;
	string device_name, dserver_name, full_ds_name, server_host, doc_url;
	DevLong server_version;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		vector<string> uargs; // user arguments
		uargs.push_back("device");

		vector<string> params; // parameters
		params.push_back("fulldsname");
		params.push_back("serverhost");
		params.push_back("serverversion");
		params.push_back("docurl");

		vector<string> params_opt; // optional parameters
		params_opt.push_back("loop");

		bool params_ok = true;
		for(size_t i = 0; i < params.size() && params_ok; i++)
			params_ok = CxxTest::TangoPrinter::is_param_set(params[i]);

		if(CxxTest::TangoPrinter::get_uargc() >= uargs.size() && params_ok)
		{
			device_name = CxxTest::TangoPrinter::get_uargv()[0];
			dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param_val(params[0]);

			full_ds_name = CxxTest::TangoPrinter::get_param_val(params[0]);
			server_host = CxxTest::TangoPrinter::get_param_val(params[1]);
cout << "Server_host = " << server_host << ", string size = " << server_host.size() << endl;
string::size_type pos = server_host.find('.');
pos--;
if (server_host[pos] == 0x0a)
	cout << "Line feed" << endl;
else if (server_host[pos] == 0x0d)
	cout << "Carriage return" << endl;
else
	cout << "Another character" << endl;
			server_version = atoi(CxxTest::TangoPrinter::get_param_val(params[2]).c_str());
			doc_url = CxxTest::TangoPrinter::get_param_val(params[3]);
		}
		else
		{
			cout << "usage: " << CxxTest::TangoPrinter::get_executable_name();

			for(size_t i = 0; i < uargs.size(); i++)
				cout << " " << uargs[i];

			for(size_t i = 0; i < params.size(); i++)
				cout << " " << CxxTest::TangoPrinter::get_param_def(params[i]);

			for(size_t i = 0; i < params_opt.size(); i++)
				cout << " [" << CxxTest::TangoPrinter::get_param_def(params_opt[i]) << "]";

			cout  << endl;
			exit(-1);
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
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::OFF);

		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RestartServer"));
		Tango_sleep(3);

		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
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
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::OFF);

		str = device_name;
		din << str;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RestartServer", din));
		Tango_sleep(3);

		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
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
cout << "dserver->info().server_host: " << dserver->info().server_host << ", server_host = " << server_host << endl;
		TS_ASSERT(dserver->info().server_host == server_host);
		TS_ASSERT(dserver->info().server_id == full_ds_name);
		TS_ASSERT(dserver->info().server_version == server_version);
	}
};
#undef cout
#endif // DServerMiscTestSuite_h
