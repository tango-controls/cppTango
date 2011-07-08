#ifndef BlackboxTestSuite_h
#define BlackboxTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#undef SUITE_NAME
#define SUITE_NAME BlackboxTestSuite

class BlackboxTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *device2, *device3, *dserver;
	string device1_name, device2_name, device3_name, full_ds_name, server_host, doc_url, dev_type;
	DevLong server_version;

public:
	SUITE_NAME()
	{

		cout << endl;

//
// Arguments check -------------------------------------------------
//

		vector<string> uargs; // user arguments
		uargs.push_back("device1");
		uargs.push_back("device2");
		uargs.push_back("device3");

		vector<string> params; // parameters
		params.push_back("fulldsname");
		params.push_back("serverhost");
		params.push_back("serverversion");
		params.push_back("docurl");
		params.push_back("devtype");

		vector<string> params_opt; // optional parameters
		params_opt.push_back("loop");

		string dserver_name;

		bool params_ok = true;
		for(size_t i = 0; i < params.size() && params_ok; i++)
			params_ok = CxxTest::TangoPrinter::is_param_set(params[i]);

		if(CxxTest::TangoPrinter::get_uargc() > 0 && params_ok)
		{
			device1_name = CxxTest::TangoPrinter::get_uargv()[0];
			device2_name = CxxTest::TangoPrinter::get_uargv()[1];
			device3_name = CxxTest::TangoPrinter::get_uargv()[2];
			dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param_val(params[0]);

			full_ds_name = CxxTest::TangoPrinter::get_param_val(params[0]);
			server_host = CxxTest::TangoPrinter::get_param_val(params[1]);
			server_version = atoi(CxxTest::TangoPrinter::get_param_val(params[2]).c_str());
			doc_url = CxxTest::TangoPrinter::get_param_val(params[3]);
			dev_type = CxxTest::TangoPrinter::get_param_val(params[4]);
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
			device1 = new DeviceProxy(device1_name);
			device2 = new DeviceProxy(device2_name);
			dserver = new DeviceProxy(dserver_name);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

		cout << endl << "new DeviceProxy(" << device1->name() << ") returned" << endl;
		cout << "new DeviceProxy(" << device2->name() << ") returned" << endl;
		cout << "new DeviceProxy(" << dserver->name() << ") returned" << endl << endl;
	}

	virtual ~SUITE_NAME()
	{
		// set the Tango::ON state on suite tearDown() in case a test fails leaving Tango::OFF status
		DeviceData din;
		din << device1_name;
		try
		{
			dserver->command_inout("DevRestart", din);
		}
		catch(CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

		cout << endl;
		delete device1;
		delete device2;
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

// Test black-box device feature

	void test_blackbox_device_feature(void)
	{
		TS_ASSERT_THROWS_ASSERT(device1->black_box(0), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_BlackBoxArgument"
						&& e.errors[0].severity == Tango::ERR));

		DeviceData din, dout;
		DevLong lg_in = 10, lg_out;

		din << lg_in;
		dout = device1->command_inout("IOLong", din);
		dout >> lg_out;
		TS_ASSERT(lg_out == 20);
		dout = device1->command_inout("IOLong", din);
		dout >> lg_out;
		TS_ASSERT(lg_out == 20);
		dout = device1->command_inout("IOLong", din);
		dout >> lg_out;
		TS_ASSERT(lg_out == 20);
		dout = device1->command_inout("IOLong", din);
		dout >> lg_out;
		TS_ASSERT(lg_out == 20);
		dout = device1->command_inout("IOLong", din);
		dout >> lg_out;
		TS_ASSERT(lg_out == 20);

		vector<string> *blackbox_out;
		string out_str, version_str, reference_str;

		// sets the "command_ionout" suffix (refer to blackbox.cpp build_info_as_str())
		switch(server_version)
		{
		case 2:
			version_str = "_2";
			break;
		case 3:
			version_str = "_2";
			break;
		case 4:
			version_str = "_4";
			break;
		default:
			version_str = "";
		}
		reference_str = "Operation command_inout" + version_str + " (cmd = IOLong) from cache_device requested from " + server_host;

		blackbox_out = device1->black_box(3);
		for(vector<string>::iterator it = (*blackbox_out).begin(); it != (*blackbox_out).end(); ++it)
		{
			out_str = *it;
			out_str.erase(0,out_str.rfind(": ") + 2); // removes time stamp from the output
			TS_ASSERT(out_str == reference_str);
		}

		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RestartServer"));
		sleep(3);

		TS_ASSERT_THROWS_NOTHING(device3 = new DeviceProxy(device3_name));

		TS_ASSERT_THROWS_ASSERT(device3->black_box(2), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_BlackBoxEmpty"
						&& e.errors[0].severity == Tango::ERR));

		reference_str = "Operation ping requested from " + server_host;
		blackbox_out = device2->black_box(1);
		out_str = (*blackbox_out)[0];
		out_str.erase(0,out_str.rfind(": ") + 2);
		TS_ASSERT(out_str == reference_str);
	}
};
#endif // BlackboxTestSuite_h
