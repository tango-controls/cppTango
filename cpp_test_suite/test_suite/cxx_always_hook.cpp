#ifndef AlwaysHookTestSuite_h
#define AlwaysHookTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>
#include "compare_test.h"

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME AlwaysHookTestSuite

class AlwaysHookTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device, *dserver;
	string device_name, dserver_name, refpath, outpath, file_name, ref_file, out_file;
	int loglevel, dsloglevel;
	bool logging_level_restored, logging_target_restored;

public:
	SUITE_NAME()
	{
		// output/reference file name
		file_name = "always_hook.out";

		logging_level_restored = false;
		logging_target_restored = false;

//
// Arguments check -------------------------------------------------
//

		vector<string> uargs; // user arguments
		uargs.push_back("device");

		vector<string> params; // parameters
		params.push_back("fulldsname");
		params.push_back("outpath");
		params.push_back("refpath");
		params.push_back("loglevel");
		params.push_back("dsloglevel");

		vector<string> params_opt; // optional parameters
		params_opt.push_back("loop");

		bool params_ok = true;
		for(size_t i = 0; i < params.size() && params_ok; i++)
			params_ok = CxxTest::TangoPrinter::is_param_set(params[i]);

		if(CxxTest::TangoPrinter::get_uargc() > 0 && params_ok)
		{
			device_name = CxxTest::TangoPrinter::get_uargv()[0];
			dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param_val(params[0]);
			outpath = CxxTest::TangoPrinter::get_param_val(params[1]);
			refpath = CxxTest::TangoPrinter::get_param_val(params[2]);
			loglevel = atoi(CxxTest::TangoPrinter::get_param_val(params[3]).c_str());
			dsloglevel = atoi(CxxTest::TangoPrinter::get_param_val(params[4]).c_str());
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
			dserver = new DeviceProxy(dserver_name);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

		cout << endl;
		cout << "new DeviceProxy(" << device->name() << ") returned" << endl;
		cout << "new DeviceProxy(" << dserver->name() << ") returned" << endl << endl;

//
// File names ------------------------------------------------------
//

		ref_file = refpath + file_name;
		out_file = outpath + file_name;

	}

	virtual ~SUITE_NAME()
	{
		cout << endl;

		// clean up in case test suite terminates before logging level is restored to defaults
		if(!logging_level_restored)
		{
			DeviceData din;
			DevVarLongStringArray reset_device_level;
			reset_device_level.lvalue.length(2);
			reset_device_level.lvalue[0] = loglevel;
			reset_device_level.lvalue[1] = dsloglevel;
			reset_device_level.svalue.length(2);
			reset_device_level.svalue[0] = "*";
			reset_device_level.svalue[1] = dserver_name.c_str();
			din << reset_device_level;
			dserver->command_inout("SetLoggingLevel", din);
		}

		// clean up in case test suite terminates before logging targets are restored to defaults
		if(!logging_target_restored)
		{
			DeviceData din;
			DevVarStringArray remove_logging_targets;
			remove_logging_targets.length(4);
			remove_logging_targets[0] = device_name.c_str();
			remove_logging_targets[1] = string("file::" + out_file).c_str();
			remove_logging_targets[2] = dserver_name.c_str();
			remove_logging_targets[3] = string("file::" + out_file).c_str();
			din << remove_logging_targets;
			dserver->command_inout("RemoveLoggingTarget", din);
		}

		delete device;
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

// Test Tango device server signal features

	void test_checking_OILong_command_preceding_the_test(void)
	{
		DeviceData din, dout;
		DevLong lg_in = 10, lg_out;
		din << lg_in;
		dout = device->command_inout("IOLong", din);
		dout >> lg_out;
		TS_ASSERT(lg_out == 20);
	}

// Test changing logging level

	void test_changing_logging_level(void)
	{
		DeviceData din, dout;

		// set logging level to 5 for the device and device server
		DevVarLongStringArray device_level, dserver_level;
		device_level.lvalue.length(1);
		device_level.lvalue[0] = 5;
		device_level.svalue.length(1);
		device_level.svalue[0] = device_name.c_str();
		din << device_level;
		dserver->command_inout("SetLoggingLevel", din);
		dserver_level.lvalue.length(1);
		dserver_level.lvalue[0] = 5;
		dserver_level.svalue.length(1);
		dserver_level.svalue[0] = dserver_name.c_str();
		din << dserver_level;
		dserver->command_inout("SetLoggingLevel", din);

		// set logging targets for the device and device server
		DevVarStringArray logging_targets;
		logging_targets.length(4);
		logging_targets[0] = device_name.c_str();
		logging_targets[1] = string("file::" + out_file).c_str();
		logging_targets[2] = dserver_name.c_str();
		logging_targets[3] = string("file::" + out_file).c_str();
		din << logging_targets;
		dserver->command_inout("AddLoggingTarget", din);

		// execute IOLong command
		DevLong lg_in = 10, lg_out;
		din << lg_in;
		dout = device->command_inout("IOLong", din);
		dout >> lg_out;
		TS_ASSERT(lg_out == 20);

		// restore device and device server logging level to default
		DevVarLongStringArray reset_device_level;
		reset_device_level.lvalue.length(2);
		reset_device_level.lvalue[0] = loglevel;
		reset_device_level.lvalue[1] = dsloglevel;
		reset_device_level.svalue.length(2);
		reset_device_level.svalue[0] = "*";
		reset_device_level.svalue[1] = dserver_name.c_str();
		din << reset_device_level;
		dserver->command_inout("SetLoggingLevel", din);
		logging_level_restored = true; // flag indicating that logging level has been restored to defaults (see destructor)

		// remove logging targets for device and device server
		din << logging_targets;
		dserver->command_inout("RemoveLoggingTarget", din);
		logging_target_restored = true; // flag indicating that logging targets have been removed (see destructor)
	}

// Test comparing input with output

	void test_comparing_input_with_output(void)
	{
		try
		{
			map<string,string> prop_val_map;
			prop_val_map["timestamp"] = "10";
			prop_val_map["thread"] = "1";
			CmpTst::CompareTest::out_set_event_properties(out_file, prop_val_map);

			map<string,string> key_val_map;
			key_val_map["DSERVER"] = dserver_name;
			key_val_map["DEVICE1"] = device_name;
			CmpTst::CompareTest::ref_replace_keywords(ref_file, key_val_map);

			map<string,string> prefix_num_map;
			prefix_num_map["thread = "] = "2";
			CmpTst::CompareTest::out_set_replace_numbers(out_file, prefix_num_map);

			CmpTst::CompareTest::compare(ref_file, out_file);
			CmpTst::CompareTest::clean_up(ref_file, out_file);
		}
		catch(CmpTst::CompareTestException &e)
		{
			try
			{
				CmpTst::CompareTest::clean_up(ref_file, out_file);
			}
			catch(CmpTst::CompareTestException &in_e)
			{
				cout << in_e.what() << endl;
			}
			TS_FAIL(e.what());
		}
	}
};
#undef cout
#endif // AlwaysHookTestSuite_h
