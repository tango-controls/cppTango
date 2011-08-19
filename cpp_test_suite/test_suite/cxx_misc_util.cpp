#ifndef MiscUtilTestSuite_h
#define MiscUtilTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME MiscUtilTestSuite

class MiscUtilTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device;
	string device_name, dserver_name;

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

// Test get_device_list_by_class

	void test_get_device_list_by_class(void)
	{
		// Depending on OS, the result could be dev/test/10 or dev/test/11
		// thus just the domain/family (dev/test) equality is tested
		DeviceData din, dout;
		const char *class_name = "DevTest", *dev_name;
		din << class_name;

		string name = device->name();
		size_t rf = name.rfind('/');
		if(rf != string::npos)
			name.erase(rf);

		dout = device->command_inout("IODevListByClass", din);
		dout >> dev_name;
		TS_ASSERT(dev_name == name);
	}

// Test get_device_list_by_class exception

	void test_get_device_list_by_class_exception(void)
	{
		DeviceData din;
		const char *class_name = "Toto";
		din << class_name;

		TS_ASSERT_THROWS_ASSERT(device->command_inout("IODevListByClass", din), Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_ClassNotFound"
								&& e.errors[0].severity == Tango::ERR));
	}

// Test get_dserver_device

	void test_get_dserver_device(void)
	{
		DeviceData dout;
		const char *dserver_name_tmp;
		dout = device->command_inout("IODServDevice");
		dout >> dserver_name_tmp;
		string dserver_name_tmp_str(dserver_name_tmp);
//		transform(dserver_name_tmp_str.begin(), dserver_name_tmp_str.end(), dserver_name_tmp_str.begin(), ::tolower);
		TS_ASSERT(dserver_name_tmp_str == dserver_name);
	}

// Test get_device_by_name

	void test_get_device_by_name(void)
	{
		DeviceData din, dout;
		const char *device_name_tmp;
		din << device_name;
		dout = device->command_inout("IODevByName", din);
		dout >> device_name_tmp;
		TS_ASSERT(device_name_tmp == device_name);
	}

// Test get_device_by_name exception

	void test_get_device_by_name_exception(void)
	{
		DeviceData din;
		const char *fake_name = "dev/test/1000";
		din << fake_name;
		TS_ASSERT_THROWS_ASSERT(device->command_inout("IODevByName", din), Tango::DevFailed &e,
								TS_ASSERT(string(e.errors[0].reason.in()) == "API_DeviceNotFound"
										&& e.errors[0].severity == Tango::ERR));
	}
};
#undef cout
#endif // MiscUtilTestSuite_h
