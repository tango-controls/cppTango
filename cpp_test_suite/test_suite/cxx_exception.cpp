#ifndef ExceptionTestSuite_h
#define ExceptionTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME ExceptionTestSuite

class ExceptionTestSuite: public CxxTest::TestSuite
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

		if(CxxTest::TangoPrinter::get_uargc() >= uargs.size() && params_ok)
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

// Test throw exception command

	void test_throw_exception_command(void)
	{
		Tango::DevVarLongStringArray in_except;
		in_except.lvalue.length(1);
		in_except.lvalue[0] = Tango::PANIC;
		in_except.svalue.length(1);
		in_except.svalue[0] = "Test Exception";
		DeviceData din;
		din << in_except;
		TS_ASSERT_THROWS_ASSERT(device->command_inout("IOThrow", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "Test Exception"
						&& e.errors[0].severity == Tango::PANIC));
	}

// Test throw exception with several levels

	void test_throw_exception_with_several_levels(void)
	{
		Tango::DevVarLongStringArray in_except;
		in_except.lvalue.length(3);
		in_except.lvalue[0] = Tango::PANIC;
		in_except.lvalue[1] = Tango::ERR;
		in_except.lvalue[2] = Tango::PANIC;
		in_except.svalue.length(3);
		in_except.svalue[0] = "Test exception level 1";
		in_except.svalue[1] = "Test exception level 2";
		in_except.svalue[2] = "Test exception level 3";
		DeviceData din;
		din << in_except;
		TS_ASSERT_THROWS_ASSERT(device->command_inout("IOReThrow", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "Test exception level 1"
						&& string(e.errors[1].reason.in()) == "Test exception level 2"
						&& string(e.errors[2].reason.in()) == "Test exception level 3"
						&& e.errors[0].severity == Tango::PANIC
						&& e.errors[1].severity == Tango::ERR
						&& e.errors[2].severity == Tango::PANIC));
	}

// Test command not found exception

	void test_command_not_found_exception(void)
	{
		TS_ASSERT_THROWS_ASSERT(device->command_inout("DevToto"), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_CommandNotFound"
						&& e.errors[0].severity == Tango::ERR));
	}

// Test command not allowed in this state exception

	void test_command_not_allowed_in_this_state_exception(void)
	{
		DevState state_in, state_out;
		DeviceData din, dout;

		state_in = Tango::OFF;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state_out;
//		cout << "----> STATE: " << state_out << endl;
		TS_ASSERT(state_out == Tango::OFF);

		DevLong num = 1L;
		din << num;
		TS_ASSERT_THROWS_ASSERT(device->command_inout("IOLong", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_CommandNotAllowed"
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::ON;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state_out;
//		cout << "----> STATE: " << state_out << endl;
		TS_ASSERT(state_out == Tango::ON);
	}
};
#undef cout
#endif // ExceptionTestSuite_h
