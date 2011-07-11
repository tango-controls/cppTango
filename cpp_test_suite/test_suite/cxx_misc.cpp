#ifndef MiscTestSuite_h
#define MiscTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

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

		cout << endl;

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
		params.push_back("devtype");

		vector<string> params_opt; // optional parameters
		params_opt.push_back("loop");

		string dserver_name;

		bool params_ok = true;
		for(size_t i = 0; i < params.size() && params_ok; i++)
			params_ok = CxxTest::TangoPrinter::is_param_set(params[i]);

		if(CxxTest::TangoPrinter::get_uargc() > 0 && params_ok)
		{
			device_name = CxxTest::TangoPrinter::get_uargv()[0];
			dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param_val(params[0]);

			full_ds_name = CxxTest::TangoPrinter::get_param_val(params[0]);
			server_host = CxxTest::TangoPrinter::get_param_val(params[1]);
			server_version = atoi(CxxTest::TangoPrinter::get_param_val(params[2]).c_str());
			doc_url = CxxTest::TangoPrinter::get_param_val(params[3]);
			dev_type = CxxTest::TangoPrinter::get_param_val(params[4]);
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
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

		cout << endl << "new DeviceProxy(" << device->name() << ") returned" << endl;
		cout << "new DeviceProxy(" << dserver->name() << ") returned" << endl << endl;
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
			Except::print_exception(e);
			exit(-1);
		}

		cout << endl;
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

	void test_DevState_and_DevStatus_commnds(void)
	{
		DeviceData dout;
		string str;
		DevState state;
		dout = device->command_inout("Status");
		dout >> str;
		TS_ASSERT(str == "The device is in ON state.");

		dout = device->command_inout("State");
		dout >> state;
		TS_ASSERT(state == Tango::ON);
	}

// Test DevRestart commnd

	void test_DevRestart_commnd(void)
	{
		DeviceData din, dout;
		DevState state_in = Tango::OFF, state_out;
		din << state_in;
		device->command_inout("IOState", din);
		dout = device->command_inout("State");
		dout >> state_out;
		TS_ASSERT(state_out == Tango::OFF);

		din << device_name;
		dserver->command_inout("DevRestart", din);
		Tango_sleep(3);
		dout = device->command_inout("State");
		dout >> state_out;
		TS_ASSERT(state_out == Tango::ON);
	}

// Test name, description, state and status CORBA attributes

	void test_name_description_state_and_status_CORBA_attributes(void)
	{
		DeviceData dout;
		string str;
		DevState state;

		TS_ASSERT(device->name() == device_name);
		TS_ASSERT(device->description() == "A TANGO device");

		dout = device->command_inout("Status");
		dout >> str;
		TS_ASSERT(str == "The device is in ON state.");

		dout = device->command_inout("State");
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
		cout << endl << "---> server_host: " << server_host << " info().server_host: " << device->info().server_host << endl << endl;
		TS_ASSERT(device->info().dev_class == "DevTest");
		TS_ASSERT(device->info().dev_type == dev_type);
		TS_ASSERT(device->info().doc_url == "Doc URL = " + doc_url);
//		TS_ASSERT(device->info().server_host == server_host);
		TS_ASSERT(device->info().server_id == full_ds_name);
		TS_ASSERT(device->info().server_version == server_version);
	}
};
#endif // MiscTestSuite_h
