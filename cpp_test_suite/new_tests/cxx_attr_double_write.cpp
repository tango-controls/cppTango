#ifndef AttrWriteDoubleTestSuite_h
#define AttrWriteDoubleTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME AttrWriteDoubleTestSuite

class AttrWriteDoubleTestSuite : public CxxTest::TestSuite
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

	void test_read_and_write_double_attribute()
	{
		DeviceAttribute double_attr;
		DevDouble db;
		std::string attr_name = "Double_attr_w";

		DeviceAttribute value(attr_name, 3.1);
		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(value));

		TS_ASSERT_THROWS_NOTHING(double_attr = device1->read_attribute(attr_name));
		TS_ASSERT_EQUALS(double_attr.get_name(), "Double_attr_w");
		TS_ASSERT_EQUALS(double_attr.get_dim_x(), 1);
		TS_ASSERT_EQUALS(double_attr.get_dim_y(), 0);
		TS_ASSERT(double_attr >> db);
		TS_ASSERT_EQUALS(db, 3.2);
	}
};

#undef cout
#endif // AttrWriteDoubleTestSuite_h
