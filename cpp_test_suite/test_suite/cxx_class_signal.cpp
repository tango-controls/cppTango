#ifndef ClassSignalTestSuite_h
#define ClassSignalTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>
#include <signal.h>
#include "compare_test.h"

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME ClassSignalTestSuite

class ClassSignalTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver, *dbserver;
	string device1_name, device2_name, dserver_name, dbserver_name, refpath, outpath, file_name, ref_file, out_file;
	int loglevel, dsloglevel;
	bool logging_level_restored, logging_target_restored, class_signal_unregistered;

public:
	SUITE_NAME()
	{
		// output/reference file name
		file_name = "class_signal.out";

		// clean up flags
		logging_level_restored = true;
		logging_target_restored = true;
		class_signal_unregistered = true;

//
// Arguments check -------------------------------------------------
//

		vector<string> uargs; // user arguments
		uargs.push_back("device1");
		uargs.push_back("device2");

		vector<string> params; // parameters
		params.push_back("fulldsname");
		params.push_back("outpath");
		params.push_back("refpath");
		params.push_back("loglevel");
		params.push_back("dsloglevel");
		params.push_back("dbserver");

		vector<string> params_opt; // optional parameters
		params_opt.push_back("loop");

		bool params_ok = true;
		for(size_t i = 0; i < params.size() && params_ok; i++)
			params_ok = CxxTest::TangoPrinter::is_param_set(params[i]);

		if(CxxTest::TangoPrinter::get_uargc() >= uargs.size() && params_ok)
		{
			device1_name = CxxTest::TangoPrinter::get_uargv()[0];
			device2_name = CxxTest::TangoPrinter::get_uargv()[1];

			dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param_val(params[0]);
			outpath = CxxTest::TangoPrinter::get_param_val(params[1]);
			refpath = CxxTest::TangoPrinter::get_param_val(params[2]);
			loglevel = atoi(CxxTest::TangoPrinter::get_param_val(params[3]).c_str());
			dsloglevel = atoi(CxxTest::TangoPrinter::get_param_val(params[4]).c_str());
			dbserver_name = CxxTest::TangoPrinter::get_param_val(params[5]);
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
			dserver = new DeviceProxy(dserver_name);
			dbserver = new DeviceProxy(dbserver_name);
			device1->ping();
			dserver->ping();
			dbserver->ping();
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

	}

	virtual ~SUITE_NAME()
	{
		cout << endl;

//
// Clean up --------------------------------------------------------
//

		// clean up in case test suite terminates before logging level is restored to defaults
		if(!logging_level_restored)
		{
			DeviceData din;
			DevVarLongStringArray reset_dserver_level;
			reset_dserver_level.lvalue.length(3);
			reset_dserver_level.lvalue[0] = loglevel;
			reset_dserver_level.lvalue[1] = loglevel;
			reset_dserver_level.lvalue[2] = dsloglevel;
			reset_dserver_level.svalue.length(3);
			reset_dserver_level.svalue[0] = device1_name.c_str();
			reset_dserver_level.svalue[1] = device2_name.c_str();
			reset_dserver_level.svalue[2] = dserver_name.c_str();
			din << reset_dserver_level;
			try
			{
				dserver->command_inout("SetLoggingLevel", din);
			}
			catch(DevFailed &e)
			{
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before logging targets are restored to defaults
		if(!logging_target_restored)
		{
			DeviceData din;
			DevVarStringArray remove_logging_target;
			remove_logging_target.length(6);
			remove_logging_target[0] = device1_name.c_str();
			remove_logging_target[1] = string("file::" + out_file).c_str();
			remove_logging_target[2] = device2_name.c_str();
			remove_logging_target[3] = string("file::" + out_file).c_str();
			remove_logging_target[4] = dserver_name.c_str();
			remove_logging_target[5] = string("file::" + out_file).c_str();
			din << remove_logging_target;
			try
			{
				dserver->command_inout("RemoveLoggingTarget", din);
			}
			catch(DevFailed &e)
			{
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before newly registered signals are unregistered
		if(!class_signal_unregistered)
		{
			DeviceData din;
			DevLong sig_num = 14;
			din << sig_num;
			try
			{
				device1->command_inout("IOUnregClassSig", din);
			}
			catch(DevFailed &e)
			{
				Except::print_exception(e);
			}
		}

		delete device1;
		delete dserver;
		delete dbserver;
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

// Test class signal out of range exceptions

	void test_class_signal_out_of_range_exceptions(void)
	{
		DeviceData din;
		DevLong sig_num = 1000;
		din << sig_num;

		// try to register class signal of out of range value
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IORegClassSig", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_SignalOutOfRange"
						&& e.errors[0].severity == Tango::ERR));

		// try to unregister class signal of out of range value
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOUnregClassSig", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_SignalOutOfRange"
						&& e.errors[0].severity == Tango::ERR));
	}

// Test registering and unregistering class signals

	void test_registering_and_unregistering_signals(void)
	{
		DeviceData din;
		DevLong sig_num = 14;
		din << sig_num;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IORegClassSig", din));
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOUnregClassSig", din));
	}

// Test class signal handling

	void test_class_signal_handling(void)
	{
		DeviceData din, dout;
		DevLong sig_num = 14;

		// register class signal
		din << sig_num;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IORegClassSig", din));
		class_signal_unregistered = false; // flag indicating that class signals have been registered

		// set logging level to 5
		DevVarLongStringArray dserver_level_in;
		dserver_level_in.lvalue.length(3);
		dserver_level_in.lvalue[0] = 5;
		dserver_level_in.lvalue[1] = 5;
		dserver_level_in.lvalue[2] = 5;
		dserver_level_in.svalue.length(3);
		dserver_level_in.svalue[0] = device1_name.c_str();
		dserver_level_in.svalue[1] = device2_name.c_str();
		dserver_level_in.svalue[2] = dserver_name.c_str();
		din << dserver_level_in;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("SetLoggingLevel", din));
		logging_level_restored = false; // flag indicating that logging level has been modified

		// add logging target
		DevVarStringArray logging_target;
		logging_target.length(6);
		logging_target[0] = device1_name.c_str();
		logging_target[1] = string("file::" + out_file).c_str();
		logging_target[2] = device2_name.c_str();
		logging_target[3] = string("file::" + out_file).c_str();
		logging_target[4] = dserver_name.c_str();
		logging_target[5] = string("file::" + out_file).c_str();
		din << logging_target;
		dserver->command_inout("AddLoggingTarget", din);
		logging_target_restored = false; // flag indicating that logging targets have been added

		// get device server PID and send signal
		TangoSys_Pid pid;
		const DevVarLongStringArray *result;
		string query = "select pid from device where name='" + dserver_name + "'";
		din << query;
		dout = dbserver->command_inout("DbMySqlSelect", din);
		dout >> result;
		pid = atoi((*result).svalue[0].in());
		if(pid > 0)
			kill(pid, sig_num);
		Tango_sleep(2);

		// set logging level back to defaults
		DevVarLongStringArray reset_dserver_level;
		reset_dserver_level.lvalue.length(3);
		reset_dserver_level.lvalue[0] = loglevel;
		reset_dserver_level.lvalue[1] = loglevel;
		reset_dserver_level.lvalue[2] = dsloglevel;
		reset_dserver_level.svalue.length(3);
		reset_dserver_level.svalue[0] = device1_name.c_str();
		reset_dserver_level.svalue[1] = device2_name.c_str();
		reset_dserver_level.svalue[2] = dserver_name.c_str();
		din << reset_dserver_level;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("SetLoggingLevel", din));
		logging_level_restored = true; // flag indicating that logging level has been restored to defaults (see destructor)

		// remove logging target
		DevVarStringArray remove_logging_target;
		remove_logging_target.length(6);
		remove_logging_target[0] = device1_name.c_str();
		remove_logging_target[1] = string("file::" + out_file).c_str();
		remove_logging_target[2] = device2_name.c_str();
		remove_logging_target[3] = string("file::" + out_file).c_str();
		remove_logging_target[4] = dserver_name.c_str();
		remove_logging_target[5] = string("file::" + out_file).c_str();
		din << remove_logging_target;
		dserver->command_inout("RemoveLoggingTarget", din);
		logging_target_restored = true; // flag indicating that logging targets have been removed (see destructor)

		// unregister class signal
		din << sig_num;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOUnregClassSig", din));
		class_signal_unregistered = true; // flag indicating that class signals have been unregistered
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
			key_val_map["DEVICE1"] = device1_name;
			key_val_map["DEVICE2"] = device2_name;
			key_val_map["DSERVER"] = dserver_name;
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
#endif // ClassSignalTestSuite_h
