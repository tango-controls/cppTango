#ifndef TemplateCmdTestSuite_h
#define TemplateCmdTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME TemplateCmdTestSuite

class TemplateCmdTestSuite: public CxxTest::TestSuite
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

// Test IOTempl

	void test_IOTempl(void)
	{
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOTempl"));
	}

// Test IOTemplState

	void test_IOTemplState(void)
	{
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOTemplState"));
	}

// Test IOTemplState exception

	void test_IOTemplState_exception(void)
	{
		DevState state_in;
		DeviceData din;

		state_in = Tango::OFF;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));

		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOTemplState"), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_CommandNotAllowed
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::ON;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
	}

// Test IOTemplIn

	void test_IOTemplIn(void)
	{
		DevLong lg = 1L;
		DeviceData din;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOTemplIn", din));
	}

// Test IOTemplInState

	void test_IOTemplInState(void)
	{
		DevLong lg = 1L;
		DeviceData din;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOTemplInState", din));
	}

// Test IOTemplInState exception

	void test_IOTemplInState_exception(void)
	{
		DevState state_in;
		DeviceData din;
		long lg = 1L;

		state_in = Tango::OFF;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));

		din << lg;
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOTemplInState", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_CommandNotAllowed
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::ON;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
	}

// Test IOTemplOut

	void test_IOTemplOut(void)
	{
		DeviceData dout;
		const DevVarLongArray *out;
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IOTemplOut"));
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
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IOTemplOutState"));
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
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));

		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOTemplOutState"), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_CommandNotAllowed
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::ON;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
	}

// Test IOTemplInOut

	void test_IOTemplInOut(void)
	{
		DeviceData din, dout;
		const DevVarDoubleArray *out;
		DevDouble db = 3.4;
		din << db;
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IOTemplInOut", din));
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
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IOTemplInOutState", din));
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
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));

		din << db;
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOTemplInOutState", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_CommandNotAllowed
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::ON;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
	}
};
#undef cout
#endif // TemplateCmdTestSuite_h
