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
	int loglevel, dsloglevel;
	bool logging_level_restored, logging_target_restored;

public:
	SUITE_NAME()
	{
		// output/reference file name
		file_name = "dserver_cmd.out";

		// clean up flags
		logging_level_restored = true;
		logging_target_restored = true;

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

		if(CxxTest::TangoPrinter::get_uargc() >= uargs.size() && params_ok)
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
			dserver->ping();
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

//
// File names ------------------------------------------------------
//

		ref_file = refpath + file_name;
		out_file = outpath + file_name;
		CmpTst::CompareTest::clean_on_startup(ref_file, out_file);

	}

	virtual ~SUITE_NAME()
	{
		cout << "Hello world 19" << endl;

//
// Clean up --------------------------------------------------------
//

		// clean up in case test suite terminates before logging level is restored to defaults
		if(!logging_level_restored)
		{
			cout << "Hello world 20" << endl;
			DeviceData din;
			DevVarLongStringArray reset_dserver_level;
			reset_dserver_level.lvalue.length(1);
			reset_dserver_level.lvalue[0] = dsloglevel;
			reset_dserver_level.svalue.length(1);
			reset_dserver_level.svalue[0] = dserver_name.c_str();
			din << reset_dserver_level;
			try
			{
				dserver->command_inout("SetLoggingLevel", din);
			}
			catch(DevFailed &e)
			{
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before logging targets are restored to defaults
		if(!logging_target_restored)
		{
			cout << "Hello world 21" << endl;
			DeviceData din;
			DevVarStringArray remove_logging_target;
			remove_logging_target.length(2);
			remove_logging_target[0] = dserver_name.c_str();
			remove_logging_target[1] = string("file::" + out_file).c_str();
			din << remove_logging_target;
			try
			{
				dserver->command_inout("RemoveLoggingTarget", din);
			}
			catch(DevFailed &e)
			{
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
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
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("QueryClass"));
		dout >> str_arr;
		TS_ASSERT(string((*str_arr)[0].in()) == "DevTest");

		// execute QueryDevice command
		vector<string> serv_dev_vec, usr_dev_vec;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("QueryDevice"));
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

		// get logging level and check if default
		const DevVarLongStringArray *dserver_level_out;
		DevVarStringArray dserver_name_in;
		dserver_name_in.length(1);
		dserver_name_in[0] = dserver_name.c_str();
		din << dserver_name_in;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("GetLoggingLevel", din));
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
		logging_level_restored = false; // flag indicating that logging level has been modified

		// query device server class
		const DevVarStringArray *query_class_out;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("QueryClass"));
		dout >> query_class_out;
		TS_ASSERT(string((*query_class_out)[0].in()) == "DevTest");

		// get logging level again and check if 5
		din << dserver_name_in;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("GetLoggingLevel", din));
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
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("GetLoggingLevel", din));
		dout >> dserver_level_out;
		TS_ASSERT((*dserver_level_out).lvalue[0] == dsloglevel);
		TS_ASSERT((*dserver_level_out).svalue[0].in() == dserver_name);
	}

// Test set output file commands

	void test_set_output_file_commands(void)
	{
		cout << "Hello world 1" << endl;
		DeviceData din, dout;

		// check if logging target is not set
		const DevVarStringArray *logging_target_out;
		din << dserver_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("GetLoggingTarget", din));
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
		cout << "Hello world 2" << endl;

		// try to add logging target as a file which cannot be opened
		DevVarStringArray fake_logging_target;
		fake_logging_target.length(2);
		fake_logging_target[0] = dserver_name.c_str();
		fake_logging_target[1] = string("file::/usr/lib/cxx_dserver_cmd.out").c_str();
		din << fake_logging_target;
		TS_ASSERT_THROWS_ASSERT(dserver->command_inout("AddLoggingTarget", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_CannotOpenFile"
						&& e.errors[0].severity == Tango::ERR));
		cout << "Hello world 3" << endl;

		// add logging target
		DevVarStringArray logging_target;
		logging_target.length(2);
		logging_target[0] = dserver_name.c_str();
		logging_target[1] = string("file::" + out_file).c_str();
		din << logging_target;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddLoggingTarget", din));
		logging_target_restored = false; // flag indicating that logging targets have been added

		// query device server class
		const DevVarStringArray *query_class_out;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("QueryClass"));
		dout >> query_class_out;
		TS_ASSERT(string((*query_class_out)[0].in()) == "DevTest");
		cout << "Hello world 4" << endl;

		// remove logging target
		DevVarStringArray remove_logging_target;
		remove_logging_target.length(2);
		remove_logging_target[0] = dserver_name.c_str();
		remove_logging_target[1] = string("file::" + out_file).c_str();
		din << remove_logging_target;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemoveLoggingTarget", din));
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
		cout << "Hello world 5" << endl;

		// check if logging target was removed
		const DevVarStringArray *check_logging_target;
		din << dserver_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("GetLoggingTarget", din));
		dout >> check_logging_target;
		TS_ASSERT((*check_logging_target).length() == 0);
		cout << "Hello world 6" << endl;
	}

// Test comparing input with output

	void test_comparing_input_with_output(void)
	{
		cout << "Hello world 7" << endl;
		try
		{
			map<string,string> prop_val_map;
			prop_val_map["timestamp"] = "10";
			prop_val_map["thread"] = "1";
			CmpTst::CompareTest::out_set_event_properties(out_file, prop_val_map);
			cout << "Hello world 8" << endl;


			map<string,string> key_val_map;
			key_val_map["DSERVER"] = dserver_name;
			key_val_map["FILE"] = out_file;
			CmpTst::CompareTest::ref_replace_keywords(ref_file, key_val_map);
			cout << "Hello world 9" << endl;

			map<string,string> prefix_num_map;
			prefix_num_map["thread = "] = "2";
			CmpTst::CompareTest::out_set_replace_numbers(out_file, prefix_num_map);
			cout << "Hello world 10" << endl;

			CmpTst::CompareTest::compare(ref_file, out_file);
			cout << "Hello world 11" << endl;
			CmpTst::CompareTest::clean_up(ref_file, out_file);
			cout << "Hello world 12" << endl;
		}
		catch(CmpTst::CompareTestException &e)
		{
			cout << "Hello world 13" << endl;
			try
			{
				CmpTst::CompareTest::leave_output(ref_file);
				cout << "Hello world 14" << endl;
			}
			catch(CmpTst::CompareTestException &in_e)
			{
				cout << "Hello world 15" << endl;
				cout << in_e.what() << endl;
				cout << "Hello world 16" << endl;
			}
			cout << "Hello world 17" << endl;
			TS_FAIL(e.what());
		}
		cout << "Hello world 18" << endl;
	}
};
#undef cout
#endif // DServerCmdTestSuite_h
