#ifndef TemplateCmdTestSuite_h
#define TemplateCmdTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME TemplateCmdTestSuite

class TemplateCmdTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device, *dserver;
	string device_name;

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

		cout << endl;
		cout << "new DeviceProxy(" << device->name() << ") returned" << endl;
		cout << "new DeviceProxy(" << dserver->name() << ") returned" << endl << endl;

	}

	virtual ~SUITE_NAME()
	{
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

// Test IOTempl

	void test_IOTempl(void)
	{
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOTempl"));
	}

// Test IOTemplState

	void test_IOTemplState(void)
	{
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOTemplState"));
	}

// Test IOTemplState exception

	void test_IOTemplState_exception(void)
	{
		DevState state_in;
		DeviceData din;

		state_in = Tango::OFF;
		din << state_in;
		device->command_inout("IOState", din);

		TS_ASSERT_THROWS_ASSERT(device->command_inout("IOTemplState"), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_CommandNotAllowed"
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::ON;
		din << state_in;
		device->command_inout("IOState", din);
	}

// Test IOTemplIn

	void test_IOTemplIn(void)
	{
		DevLong lg = 1L;
		DeviceData din;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOTemplIn", din));
	}

// Test IOTemplInState

	void test_IOTemplInState(void)
	{
		DevLong lg = 1L;
		DeviceData din;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOTemplInState", din));
	}

// Test IOTemplInState exception

	void test_IOTemplInState_exception(void)
	{
		DevState state_in;
		DeviceData din;
		long lg = 1L;

		state_in = Tango::OFF;
		din << state_in;
		device->command_inout("IOState", din);

		din << lg;
		TS_ASSERT_THROWS_ASSERT(device->command_inout("IOTemplInState", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_CommandNotAllowed"
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::ON;
		din << state_in;
		device->command_inout("IOState", din);
	}

// Test IOTemplOut

	void test_IOTemplOut(void)
	{
		DeviceData dout;
		const DevVarLongArray *out;
		dout = device->command_inout("IOTemplOut");
		dout >> out;
		TS_ASSERT((*out)[0] == 10
				&& (*out)[1] == 20
				&& (*out)[2] == 30
				&& (*out)[3] == 40);
	}

// Test IOTemplOutState

	void test_IOTemplOutState(void)
	{
		DeviceData dout;
		const DevVarLongArray *out;
		dout = device->command_inout("IOTemplOutState");
		dout >> out;
		TS_ASSERT((*out)[0] == 10
				&& (*out)[1] == 20
				&& (*out)[2] == 30
				&& (*out)[3] == 40);
	}

// Test IOTemplOutState exception

	void test_IOTemplOutState_exception(void)
	{
		DevState state_in;
		DeviceData din;

		state_in = Tango::OFF;
		din << state_in;
		device->command_inout("IOState", din);

		TS_ASSERT_THROWS_ASSERT(device->command_inout("IOTemplOutState"), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_CommandNotAllowed"
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::ON;
		din << state_in;
		device->command_inout("IOState", din);
	}

// Test IOTemplInOut

	void test_IOTemplInOut(void)
	{
		DeviceData din, dout;
		const DevVarDoubleArray *out;
		DevDouble db = 3.4;
		din << db;
		dout = device->command_inout("IOTemplInOut", din);
		dout >> out;
		TS_ASSERT((*out)[0] == 3.4
				&& (*out)[1] == 6.8);
	}

// Test IOTemplInOutState

	void test_IOTemplInOutState(void)
	{
		DeviceData din, dout;
		const DevVarDoubleArray *out;
		DevDouble db = 4.2;
		din << db;
		dout = device->command_inout("IOTemplInOutState", din);
		dout >> out;
		TS_ASSERT((*out)[0] == 4.2
				&& (*out)[1] == 8.4);
	}

// Test IOTemplInOutState exception

	void test_IOTemplInOutState_exception(void)
	{
		DevState state_in;
		DeviceData din;
		DevDouble db = 4.2;

		state_in = Tango::OFF;
		din << state_in;
		device->command_inout("IOState", din);

		din << db;
		TS_ASSERT_THROWS_ASSERT(device->command_inout("IOTemplInOutState", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_CommandNotAllowed"
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::ON;
		din << state_in;
		device->command_inout("IOState", din);
	}
};
#undef cout
#endif // TemplateCmdTestSuite_h
