#ifndef DServerCmdTestSuite_h
#define DServerCmdTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>
#include <algorithm>
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
	int loglevel, dsloglevel;
	bool logging_level_restored, logging_target_restored;

public:
	SUITE_NAME()
	{
		// output/reference file name
		file_name = "dserver_cmd.out";

		logging_level_restored = false;
		logging_target_restored = false;

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
		params.push_back("loglevel");
		params.push_back("dsloglevel");

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

		// clean up in case test suite terminates before logging level is restored to defaults
		if(!logging_level_restored)
		{
			DeviceData din;
			DevVarLongStringArray reset_dserver_level;
			reset_dserver_level.lvalue.length(1);
			reset_dserver_level.lvalue[0] = dsloglevel;
			reset_dserver_level.svalue.length(1);
			reset_dserver_level.svalue[0] = dserver_name.c_str();
			din << reset_dserver_level;
			dserver->command_inout("SetLoggingLevel", din);
		}

		// clean up in case test suite terminates before logging targets are restored to defaults
		if(!logging_target_restored)
		{
			DeviceData din;
			DevVarStringArray remove_logging_target;
			remove_logging_target.length(2);
			remove_logging_target[0] = dserver_name.c_str();
			remove_logging_target[1] = string("file::" + out_file).c_str();
			din << remove_logging_target;
			dserver->command_inout("RemoveLoggingTarget", din);
		}

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

		// execute QueryClass command
		dout = dserver->command_inout("QueryClass");
		dout >> str_arr;
		TS_ASSERT(string((*str_arr)[0].in()) == "DevTest");

		// execute QueryDevice command
		vector<string> serv_dev_vec, usr_dev_vec;
		dout = dserver->command_inout("QueryDevice");
		dout >> str_arr;
		for(size_t i = 0; i < (*str_arr).length(); i++)
			serv_dev_vec.push_back(string((*str_arr)[i].in()));

		usr_dev_vec.push_back("DevTest::" + device1_name);
		usr_dev_vec.push_back("DevTest::" + device2_name);
		usr_dev_vec.push_back("DevTest::" + device3_name);

		cout << "\tOriginal server device vector:" << endl;
		for(size_t i = 0; i < serv_dev_vec.size(); i++)
			cout << "\t---> " << serv_dev_vec[i] << endl;

		sort(serv_dev_vec.begin(), serv_dev_vec.end()); 	// sort expected and returned device names
		sort(usr_dev_vec.begin(), usr_dev_vec.end());		// in alphabetical order to compare the vectors

		cout << "\tSorted server device vector:" << endl;
		for(size_t i = 0; i < serv_dev_vec.size(); i++)
			cout << "\t---> " << serv_dev_vec[i] << endl;

		cout << "\tSorted user device vector:" << endl;
		for(size_t i = 0; i < usr_dev_vec.size(); i++)
			cout << "\t---> " << usr_dev_vec[i] << endl;

		TS_ASSERT(serv_dev_vec[0] == usr_dev_vec[0]);
		TS_ASSERT(serv_dev_vec[1] == usr_dev_vec[1]);
		TS_ASSERT(serv_dev_vec[2] == usr_dev_vec[2]);
	}

// Test trace levels commands

	void test_trace_levels_commands(void)
	{
		DeviceData din, dout;

		// get logging level and check if defaults
		const DevVarLongStringArray *dserver_level_out;
		DevVarStringArray dserver_name_in;
		dserver_name_in.length(1);
		dserver_name_in[0] = dserver_name.c_str();
		din << dserver_name_in;
		dout = dserver->command_inout("GetLoggingLevel", din);
		dout >> dserver_level_out;
		TS_ASSERT((*dserver_level_out).lvalue[0] == dsloglevel);
		TS_ASSERT((*dserver_level_out).svalue[0].in() == dserver_name);

		// set logging level to 5
		DevVarLongStringArray dserver_level_in;
		dserver_level_in.lvalue.length(1);
		dserver_level_in.lvalue[0] = 5;
		dserver_level_in.svalue.length(1);
		dserver_level_in.svalue[0] = dserver_name.c_str();
		din << dserver_level_in;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("SetLoggingLevel", din));

		// query device server class
		const DevVarStringArray *query_class_out;
		dout = dserver->command_inout("QueryClass");
		dout >> query_class_out;
		TS_ASSERT(string((*query_class_out)[0].in()) == "DevTest");

		// get logging level again and check if 5
		din << dserver_name_in;
		dout = dserver->command_inout("GetLoggingLevel", din);
		dout >> dserver_level_out;
		TS_ASSERT((*dserver_level_out).lvalue[0] == 5);
		TS_ASSERT((*dserver_level_out).svalue[0].in() == dserver_name);

		// restore logging level to defaluts
		dserver_level_in.lvalue[0] = dsloglevel;
		din << dserver_level_in;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("SetLoggingLevel", din));
		logging_level_restored = true; // flag indicating that logging level has been reset to defaults (see destructor)

		// once more get logging level and check if set to defaults
		din << dserver_name_in;
		dout = dserver->command_inout("GetLoggingLevel", din);
		dout >> dserver_level_out;
		TS_ASSERT((*dserver_level_out).lvalue[0] == dsloglevel);
		TS_ASSERT((*dserver_level_out).svalue[0].in() == dserver_name);
	}

// Test set output file commands

	void test_set_output_file_commands(void)
	{
		DeviceData din, dout;

		// check if logging target is not set
		const DevVarStringArray *logging_target_out;
		din << dserver_name;
		dout = dserver->command_inout("GetLoggingTarget", din);
		dout >> logging_target_out;
		TS_ASSERT((*logging_target_out).length() == 0);

		// set logging level to 5
		DevVarLongStringArray dserver_level_in;
		dserver_level_in.lvalue.length(1);
		dserver_level_in.lvalue[0] = 5;
		dserver_level_in.svalue.length(1);
		dserver_level_in.svalue[0] = dserver_name.c_str();
		din << dserver_level_in;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("SetLoggingLevel", din));
		logging_level_restored = false; // flag indicating that logging level has been modified

		// try to add logging target as a file which cannot be opened
		DevVarStringArray fake_logging_target;
		fake_logging_target.length(2);
		fake_logging_target[0] = dserver_name.c_str();
		fake_logging_target[1] = string("file::/usr/lib/cxx_dserver_cmd.out").c_str();
		din << fake_logging_target;
		TS_ASSERT_THROWS_ASSERT(dserver->command_inout("AddLoggingTarget", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_CannotOpenFile"
						&& e.errors[0].severity == Tango::ERR));

		// add logging target
		DevVarStringArray logging_target;
		logging_target.length(2);
		logging_target[0] = dserver_name.c_str();
		logging_target[1] = string("file::" + out_file).c_str();
		din << logging_target;
		dserver->command_inout("AddLoggingTarget", din);

		// query device server class
		const DevVarStringArray *query_class_out;
		dout = dserver->command_inout("QueryClass");
		dout >> query_class_out;
		TS_ASSERT(string((*query_class_out)[0].in()) == "DevTest");

		// remove logging target
		DevVarStringArray remove_logging_target;
		remove_logging_target.length(2);
		remove_logging_target[0] = dserver_name.c_str();
		remove_logging_target[1] = string("file::" + out_file).c_str();
		din << remove_logging_target;
		dserver->command_inout("RemoveLoggingTarget", din);
		logging_target_restored = true; // flag indicating that logging targets have been removed (see destructor)

		// set logging level back to defaults
		DevVarLongStringArray reset_dserver_level;
		reset_dserver_level.lvalue.length(1);
		reset_dserver_level.lvalue[0] = dsloglevel;
		reset_dserver_level.svalue.length(1);
		reset_dserver_level.svalue[0] = dserver_name.c_str();
		din << reset_dserver_level;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("SetLoggingLevel", din));
		logging_level_restored = true; // flag indicating that logging level has been restored to defaults (see destructor)

		// check if logging target was removed
		const DevVarStringArray *check_logging_target;
		din << dserver_name;
		dout = dserver->command_inout("GetLoggingTarget", din);
		dout >> check_logging_target;
		TS_ASSERT((*check_logging_target).length() == 0);
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
			key_val_map["FILE"] = out_file;
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
#endif // DServerCmdTestSuite_h
