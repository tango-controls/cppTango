#ifndef TemplateTestSuite_h
#define TemplateTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME TemplateTestSuite

class TemplateTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string localparam, device1_name, dserver_name;
		bool verbose;

		// locally defined (test suite scope) mandatory parameters
		localparam = CxxTest::TangoPrinter::get_param_loc("localparam","description of what localparam is");

		// predefined mandatory parameters
		device1_name = CxxTest::TangoPrinter::get_param("device1");
		// or
		dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");

		// predefined optional parameters
		CxxTest::TangoPrinter::get_param_opt("loop"); // loop parameter is then managed by the CXX framework itself
		// or
		verbose = CxxTest::TangoPrinter::is_param_opt_set("verbose");

		// always add this line, otherwise arguments will not be parsed correctly
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

//
// Clean up --------------------------------------------------------
//

		// clean up in case test suite terminates before my_restore_point is restored to defaults
		if(CxxTest::TangoPrinter::is_restore_set("my_restore_point"))
		{
			try
			{
				// execute some instructions
			}
			catch(DevFailed &e)
			{
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
		}

		// delete dynamically allocated objects
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

// Test TestName

	void test_TestName()
	{
		// if your code modifies the default (device) configuration, append the following line straight after
		CxxTest::TangoPrinter::restore_set("my_restore_point");
		TS_ASSERT(true);
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("UndefinedCommand"), Tango::DevFailed &e,
								TS_ASSERT(string(e.errors[0].reason.in()) == API_CommandNotFound
										&& e.errors[0].severity == Tango::ERR));
		// if the test suite fails here, thanks to the restore point, the test suite TearDown method will restore the defaults
		// after you set back the default configuration, append the following line
		CxxTest::TangoPrinter::restore_unset("my_restore_point");
	}
};
#undef cout
#endif // TemplateTestSuite_h
