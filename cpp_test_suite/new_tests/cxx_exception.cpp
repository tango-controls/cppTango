#ifndef ExceptionTestSuite_h
#define ExceptionTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME ExceptionTestSuite

class ExceptionTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;
	string device1_name;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string dserver_name;

		device1_name = CxxTest::TangoPrinter::get_param("device1");
		dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");

		CxxTest::TangoPrinter::validate_args();


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
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOThrow", din), Tango::DevFailed &e,
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
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOReThrow", din), Tango::DevFailed &e,
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
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("DevToto"), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_CommandNotFound
						&& e.errors[0].severity == Tango::ERR));
	}

// Test command not allowed in this state exception

	void test_command_not_allowed_in_this_state_exception(void)
	{
		DevState state_in, state_out;
		DeviceData din, dout;

		state_in = Tango::OFF;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state_out;
//		cout << "----> STATE: " << state_out << endl;
		TS_ASSERT(state_out == Tango::OFF);

		DevLong num = 1L;
		din << num;
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOLong", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_CommandNotAllowed
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::ON;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state_out;
//		cout << "----> STATE: " << state_out << endl;
		TS_ASSERT(state_out == Tango::ON);
	}
};
#undef cout
#endif // ExceptionTestSuite_h
