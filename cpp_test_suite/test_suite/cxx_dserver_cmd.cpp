#ifndef DServerCmdTestSuite_h
#define DServerCmdTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>
#include "compare_test.h"

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME DServerCmdTestSuite

class DServerCmdTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *dserver;
	string device1_name, device2_name, device3_name, dserver_name, refpath, outpath, file_name, ref_file, out_file;

public:
	SUITE_NAME()
	{

		file_name = "dserver_cmd.out";

//
// Arguments check -------------------------------------------------
//

		vector<string> uargs; // user arguments
		uargs.push_back("device1");
		uargs.push_back("device2");
		uargs.push_back("device3");

		vector<string> params; // parameters
		params.push_back("fulldsname");
		params.push_back("outpath");
		params.push_back("refpath");

		vector<string> params_opt; // optional parameters
		params_opt.push_back("loop");

		bool params_ok = true;
		for(size_t i = 0; i < params.size() && params_ok; i++)
			params_ok = CxxTest::TangoPrinter::is_param_set(params[i]);

		if(CxxTest::TangoPrinter::get_uargc() > 0 && params_ok)
		{
			device1_name = CxxTest::TangoPrinter::get_uargv()[0];
			device2_name = CxxTest::TangoPrinter::get_uargv()[1];
			device3_name = CxxTest::TangoPrinter::get_uargv()[2];

			dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param_val(params[0]);
			outpath = CxxTest::TangoPrinter::get_param_val(params[1]);
			refpath = CxxTest::TangoPrinter::get_param_val(params[2]);
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
			dserver = new DeviceProxy(dserver_name);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

		cout << endl;
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

// Test dserver class and instances names

	void test_dserver_class_and_instances_names(void)
	{
		DeviceData dout;
		const DevVarStringArray *str_arr;

		dout = dserver->command_inout("QueryClass");
		dout >> str_arr;
		TS_ASSERT(string((*str_arr)[0].in()) == "DevTest");

		dout = dserver->command_inout("QueryDevice");
		dout >> str_arr;

		cout << "List of devices: " << endl;
		for(unsigned int i = 0; i < (*str_arr).length(); i++)
		{
			cout << "---> " << (*str_arr)[i].in() << endl;
		}
		cout << endl;

//		TS_ASSERT(string((*str_arr)[0].in()) == device1_name);
//		TS_ASSERT(string((*str_arr)[1].in()) == device2_name);
//		TS_ASSERT(string((*str_arr)[2].in()) == device3_name);

	}
//
//// Test changing logging level
//
//	void test_changing_logging_level(void)
//	{
//		DeviceData din, dout;
//		DevVarLongStringArray device_level, dserver_level;
//		device_level.lvalue.length(1);
//		device_level.lvalue[0] = 5;
//		device_level.svalue.length(1);
//		device_level.svalue[0] = device_name.c_str();
//		din << device_level;
//		dserver->command_inout("SetLoggingLevel", din);
//		dserver_level.lvalue.length(1);
//		dserver_level.lvalue[0] = 5;
//		dserver_level.svalue.length(1);
//		dserver_level.svalue[0] = dserver_name.c_str();
//		din << dserver_level;
//		dserver->command_inout("SetLoggingLevel", din);
//
//		DevVarStringArray logging_targets;
//		logging_targets.length(4);
//		logging_targets[0] = device_name.c_str();
//		logging_targets[1] = string("file::" + out_file).c_str();
//		logging_targets[2] = dserver_name.c_str();
//		logging_targets[3] = string("file::" + out_file).c_str();
//
//		din << logging_targets;
//		dserver->command_inout("AddLoggingTarget", din);
//
//		DevLong lg_in = 10, lg_out;
//		din << lg_in;
//		dout = device->command_inout("IOLong", din);
//		dout >> lg_out;
//		TS_ASSERT(lg_out == 20);
//
//		DevVarLongStringArray reset_device_level;
//		reset_device_level.lvalue.length(1);
//		reset_device_level.lvalue[0] = 0;
//		reset_device_level.svalue.length(1);
//		reset_device_level.svalue[0] = "*";
//		din << reset_device_level;
//		dserver->command_inout("SetLoggingLevel", din);
//
//		DevVarStringArray remove_logging_targets;
//		remove_logging_targets.length(4);
//		remove_logging_targets[0] = device_name.c_str();
//		remove_logging_targets[1] = string("file::" + out_file).c_str();
//		remove_logging_targets[2] = dserver_name.c_str();
//		remove_logging_targets[3] = string("file::" + out_file).c_str();
//
//		din << logging_targets;
//		dserver->command_inout("RemoveLoggingTarget", din);
//	}
//
//// Test comparing input with output
//
//	void test_comparing_input_with_output(void)
//	{
//		try
//		{
//			map<string,string> prop_val_map;
//			prop_val_map["timestamp"] = "10";
//			prop_val_map["thread"] = "1";
//			CmpTst::CompareTest::out_set_event_properties(out_file, prop_val_map);
//
//
//			map<string,string> key_val_map;
//			key_val_map["DSERVER"] = dserver_name;
//			key_val_map["DEVICE1"] = device_name;
//			CmpTst::CompareTest::ref_replace_keywords(ref_file, key_val_map);
//
//			map<string,string> prefix_num_map;
//			prefix_num_map["thread = "] = "2";
//			CmpTst::CompareTest::out_set_replace_numbers(out_file, prefix_num_map);
//
//			CmpTst::CompareTest::compare(ref_file, out_file);
//			CmpTst::CompareTest::clean_up(ref_file, out_file);
//		}
//		catch(CmpTst::CompareTestException &e)
//		{
//			try
//			{
//				CmpTst::CompareTest::clean_up(ref_file, out_file);
//			}
//			catch(CmpTst::CompareTestException &in_e)
//			{
//				cout << in_e.what() << endl;
//			}
//			TS_FAIL(e.what());
//		}
//	}
};
#undef cout
#endif // DServerCmdTestSuite_h
