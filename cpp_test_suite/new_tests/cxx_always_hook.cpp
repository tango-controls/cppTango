#ifndef AlwaysHookTestSuite_h
#define AlwaysHookTestSuite_h

#include "compare_test.h"
#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME AlwaysHookTestSuite

class AlwaysHookTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;
	string device1_name, dserver_name, refpath, outpath, file_name, ref_file, out_file;
	int loglevel, dsloglevel;

public:
	SUITE_NAME()
	{
		// output/reference file name
		file_name = "always_hook.out";


//
// Arguments check -------------------------------------------------
//

		device1_name = CxxTest::TangoPrinter::get_param("device1");
		dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");
		outpath = CxxTest::TangoPrinter::get_param("outpath");
		refpath = CxxTest::TangoPrinter::get_param("refpath");
		loglevel = atoi(CxxTest::TangoPrinter::get_param("loglevel").c_str());
		dsloglevel = atoi(CxxTest::TangoPrinter::get_param("dsloglevel").c_str());

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
			DevVarLongStringArray reset_device_level;
			reset_device_level.lvalue.length(2);
			reset_device_level.lvalue[0] = loglevel;
			reset_device_level.lvalue[1] = dsloglevel;
			reset_device_level.svalue.length(2);
			reset_device_level.svalue[0] = "*";
			reset_device_level.svalue[1] = dserver_name.c_str();
			din << reset_device_level;
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
			DevVarStringArray remove_logging_targets;
			remove_logging_targets.length(4);
			remove_logging_targets[0] = device1_name.c_str();
			remove_logging_targets[1] = string("file::" + out_file).c_str();
			remove_logging_targets[2] = dserver_name.c_str();
			remove_logging_targets[3] = string("file::" + out_file).c_str();
			din << remove_logging_targets;
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

// Test Tango device server signal features

	void test_checking_OILong_command_preceding_the_test(void)
	{
		DeviceData din, dout;
		DevLong lg_in = 10, lg_out;
		din << lg_in;
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IOLong", din));
		dout >> lg_out;
		TS_ASSERT(lg_out == 20);
	}

// Test changing logging level

	void test_changing_logging_level(void)
	{
		DeviceData din, dout;

		// set logging level to 5 for the device and device server
		DevVarLongStringArray device_level, dserver_level;
		device_level.lvalue.length(2);
		device_level.lvalue[0] = 5;
		device_level.lvalue[1] = 5;
		device_level.svalue.length(2);
		device_level.svalue[0] = device1_name.c_str();
		device_level.svalue[1] = dserver_name.c_str();
		din << device_level;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("SetLoggingLevel", din));
		CxxTest::TangoPrinter::restore_set("logging_level");

		// set logging targets for the device and device server
		DevVarStringArray logging_targets;
		logging_targets.length(4);
		logging_targets[0] = device1_name.c_str();
		logging_targets[1] = string("file::" + out_file).c_str();
		logging_targets[2] = dserver_name.c_str();
		logging_targets[3] = string("file::" + out_file).c_str();
		din << logging_targets;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddLoggingTarget", din));
		CxxTest::TangoPrinter::restore_set("logging_target");

		// execute IOLong command
		DevLong lg_in = 10, lg_out;
		din << lg_in;
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IOLong", din));
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
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("SetLoggingLevel", din));
		CxxTest::TangoPrinter::restore_unset("logging_level");

		// remove logging targets for device and device server
		din << logging_targets;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemoveLoggingTarget", din));
		CxxTest::TangoPrinter::restore_unset("logging_target");
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
			key_val_map["DSERVER"] = dserver_name;
			key_val_map["DEVICE1"] = device1_name;
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
#endif // AlwaysHookTestSuite_h
