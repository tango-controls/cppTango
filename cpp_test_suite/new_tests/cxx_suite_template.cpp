#ifndef TemplateTestSuite_h
#define TemplateTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

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

		string device1_name, dserver_name;

		// user arguments, obtained from the command line sequentially
		device1_name = CxxTest::TangoPrinter::get_uarg("device1");

		// predefined mandatory parameters
		dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");

		// predefined optional parameters
		CxxTest::TangoPrinter::get_param_opt("loop");

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

	void test_TestName(void)
	{
		TS_ASSERT(true);
	}
};
#undef cout
#endif // TemplateTestSuite_h
