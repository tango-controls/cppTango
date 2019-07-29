#ifndef TestStateOnTestSuite_h
#define TestStateOnTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME TestStateOnTestSuite

class TestStateOnTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string device1_name;

		device1_name = CxxTest::TangoPrinter::get_param("device1");

		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

		try
		{
			device1 = new DeviceProxy(device1_name);
			device1->ping();
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
		TS_ASSERT(device1->state() == Tango::ON);
	}
};
#undef cout
#endif // TestStateOnTestSuite_h
