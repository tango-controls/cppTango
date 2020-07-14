#ifndef MiscUtilTestSuite_h
#define MiscUtilTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME MiscUtilTestSuite

class MiscUtilTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1;
	string device1_name, dserver_name;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		device1_name = CxxTest::TangoPrinter::get_param("device1");
		dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");

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

// Test get_device_list_by_class

	void test_get_device_list_by_class(void)
	{
		// Depending on OS, the result could be dev/test/10 or dev/test/11
		// thus just the domain/family (dev/test) equality is tested
		DeviceData din, dout;
		const char *class_name = "DevTest", *dev_name;
		din << class_name;

		string name;
		TS_ASSERT_THROWS_NOTHING(name = device1->name());
		size_t rf = name.rfind('/');
		if(rf != string::npos)
			name.erase(rf);

		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IODevListByClass", din));
		dout >> dev_name;
		TS_ASSERT(dev_name == name);
	}

// Test get_device_list_by_class exception

	void test_get_device_list_by_class_exception(void)
	{
		DeviceData din;
		const char *class_name = "Toto";
		din << class_name;

		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IODevListByClass", din), Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_ClassNotFound
								&& e.errors[0].severity == Tango::ERR));
	}

// Test get_dserver_device

	void test_get_dserver_device(void)
	{
		DeviceData dout;
		const char *dserver_name_tmp;
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IODServDevice"));
		dout >> dserver_name_tmp;
		string dserver_name_tmp_str(dserver_name_tmp);
//		transform(dserver_name_tmp_str.begin(), dserver_name_tmp_str.end(), dserver_name_tmp_str.begin(), ::tolower);
		TS_ASSERT(dserver_name_tmp_str == dserver_name);
	}

// Test get_device_by_name

	void test_get_device_by_name(void)
	{
		DeviceData din, dout;
		const char *device1_name_tmp;
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IODevByName", din));
		dout >> device1_name_tmp;
		TS_ASSERT(device1_name_tmp == device1_name);
	}

// Test get_device_by_name exception

	void test_get_device_by_name_exception(void)
	{
		DeviceData din;
		const char *fake_name = "dev/test/1000";
		din << fake_name;
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IODevByName", din), Tango::DevFailed &e,
								TS_ASSERT(string(e.errors[0].reason.in()) == API_DeviceNotFound
										&& e.errors[0].severity == Tango::ERR));
	}
};
#undef cout
#endif // MiscUtilTestSuite_h
