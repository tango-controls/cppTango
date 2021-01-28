#ifndef ClassSignalTestSuite_h
#define ClassSignalTestSuite_h

#include <signal.h>
#include "compare_test.h"
#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME ClassSignalTestSuite

class ClassSignalTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver, *dbserver;
	string device1_name, device2_name, dserver_name, dbserver_name, refpath, outpath, file_name, ref_file, out_file;
	int loglevel, dsloglevel;

public:
	SUITE_NAME()
	{
		// output/reference file name
		file_name = "class_signal.out";


//
// Arguments check -------------------------------------------------
//

		device1_name = CxxTest::TangoPrinter::get_param("device1");
		device2_name = CxxTest::TangoPrinter::get_param("device2");
		dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");
		outpath = CxxTest::TangoPrinter::get_param("outpath");
		refpath = CxxTest::TangoPrinter::get_param("refpath");
		loglevel = atoi(CxxTest::TangoPrinter::get_param("loglevel").c_str());
		dsloglevel = atoi(CxxTest::TangoPrinter::get_param("dsloglevel").c_str());
		dbserver_name = CxxTest::TangoPrinter::get_param("dbserver");

		CxxTest::TangoPrinter::validate_args();


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
		CmpTst::CompareTest::clean_on_startup(ref_file, out_file);

	}

	virtual ~SUITE_NAME()
	{

//
// Clean up --------------------------------------------------------
//

		// clean up in case test suite terminates before logging level is restored to defaults
		if(CxxTest::TangoPrinter::is_restore_set("logging_level"))
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
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before logging targets are restored to defaults
		if(CxxTest::TangoPrinter::is_restore_set("logging_target"))
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
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before newly registered signals are unregistered
		if(CxxTest::TangoPrinter::is_restore_set("class_signal_unregistered"))
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
				cout << endl << "Exception in suite tearDown():" << endl;
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
				TS_ASSERT(string(e.errors[0].reason.in()) == API_SignalOutOfRange
						&& e.errors[0].severity == Tango::ERR));

		// try to unregister class signal of out of range value
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOUnregClassSig", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_SignalOutOfRange
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
		int sig_num_int = 14;

		// register class signal
		din << sig_num;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IORegClassSig", din));
		CxxTest::TangoPrinter::restore_set("class_signal_unregistered");
		Tango_sleep(2);

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
		CxxTest::TangoPrinter::restore_set("logging_level");

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
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddLoggingTarget", din));
		CxxTest::TangoPrinter::restore_set("logging_target");

		// get device server PID and send signal
		TangoSys_Pid pid;
		const DevVarLongStringArray *result;
		string query = "select pid from device where name='" + dserver_name + "'";
		din << query;
		TS_ASSERT_THROWS_NOTHING(dout = dbserver->command_inout("DbMySqlSelect", din));
		dout >> result;
		pid = atoi((*result).svalue[0].in());
		if(pid > 0)
			kill(pid, sig_num_int);
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
		CxxTest::TangoPrinter::restore_unset("logging_level");

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
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemoveLoggingTarget", din));
		CxxTest::TangoPrinter::restore_unset("logging_target");

		// unregister class signal
		din << sig_num;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOUnregClassSig", din));
		CxxTest::TangoPrinter::restore_unset("class_signal_unregistered");
	}

// Test comparing input with output

	void test_comparing_input_with_output(void)
	{
		try
		{
			vector<string> v_s;
			v_s.push_back("push_heartbeat_event()");
			v_s.push_back("Sending event heartbeat");
			v_s.push_back("Event heartbeat");
			v_s.push_back("Sub device property");
			v_s.push_back("Sleep for :");
			CmpTst::CompareTest::out_remove_entries(out_file,v_s);

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
				CmpTst::CompareTest::leave_output(ref_file);
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
