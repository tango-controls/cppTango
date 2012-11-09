#ifndef TestStateOnTestSuite_h
#define TestStateOnTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME TestStateOnTestSuite

class TestStateOnTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string device_name;

		device_name = CxxTest::TangoPrinter::get_uarg("device");

		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

		try
		{
			device = new DeviceProxy(device_name);
			device->ping();
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{
		delete device;
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

// Test Test State ON

	void test_test_state_on(void)
	{
		TS_ASSERT(device->state() == Tango::ON);
	}
};
#undef cout
#endif // TestStateOnTestSuite_h
