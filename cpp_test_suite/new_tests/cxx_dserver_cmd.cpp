#ifndef DServerCmdTestSuite_h
#define DServerCmdTestSuite_h

#include "compare_test.h"
#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME DServerCmdTestSuite

class DServerCmdTestSuite : public CxxTest::TestSuite {
protected:
    DeviceProxy *dserver;
    string device1_name, device2_name, device3_name, dserver_name, refpath, outpath, file_name, ref_file, out_file;
    int loglevel, dsloglevel;

public:
    SUITE_NAME() {
        // output/reference file name
        file_name = "dserver_cmd.out";


//
// Arguments check -------------------------------------------------
//

        device1_name = CxxTest::TangoPrinter::get_param("device1");
        device2_name = CxxTest::TangoPrinter::get_param("device2");
        device3_name = CxxTest::TangoPrinter::get_param("device3");
        dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");
        outpath = CxxTest::TangoPrinter::get_param("outpath");
        refpath = CxxTest::TangoPrinter::get_param("refpath");
        loglevel = atoi(CxxTest::TangoPrinter::get_param("loglevel").c_str());
        dsloglevel = atoi(CxxTest::TangoPrinter::get_param("dsloglevel").c_str());

        CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

        try {
            dserver = new DeviceProxy(dserver_name);
            dserver->ping();
        }
        catch (CORBA::Exception &e) {
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

    virtual ~SUITE_NAME() {

//
// Clean up --------------------------------------------------------
//

        // clean up in case test suite terminates before logging level is restored to defaults
        if (CxxTest::TangoPrinter::is_restore_set("logging_level")) {
            DeviceData din;
            DevVarLongStringArray reset_dserver_level;
            reset_dserver_level.lvalue.length(1);
            reset_dserver_level.lvalue[0] = dsloglevel;
            reset_dserver_level.svalue.length(1);
            reset_dserver_level.svalue[0] = dserver_name.c_str();
            din << reset_dserver_level;
            try {
                dserver->command_inout("SetLoggingLevel", din);
            }
            catch (DevFailed &e) {
                cout << endl << "Exception in suite tearDown():" << endl;
                Except::print_exception(e);
            }
        }

        // clean up in case test suite terminates before logging targets are restored to defaults
        if (CxxTest::TangoPrinter::is_restore_set("logging_target")) {
            DeviceData din;
            DevVarStringArray remove_logging_target;
            remove_logging_target.length(2);
            remove_logging_target[0] = dserver_name.c_str();
            remove_logging_target[1] = string("file::" + out_file).c_str();
            din << remove_logging_target;
            try {
                dserver->command_inout("RemoveLoggingTarget", din);
            }
            catch (DevFailed &e) {
                cout << endl << "Exception in suite tearDown():" << endl;
                Except::print_exception(e);
            }
        }

        delete dserver;
    }

    static SUITE_NAME *createSuite() {
        return new SUITE_NAME();
    }

    static void destroySuite(SUITE_NAME *suite) {
        delete suite;
    }

//
// Tests -------------------------------------------------------
//

// Test dserver class and instances names

    void test_dserver_class_and_instances_names(void) {
        DeviceData dout;
        const DevVarStringArray *str_arr;

        // execute QueryClass command
        TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("QueryClass"));
        dout >> str_arr;
        TS_ASSERT(string((*str_arr)[0].in()) == "DevTest");

        // execute QueryDevice command
        vector <string> serv_dev_vec, usr_dev_vec;
        TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("QueryDevice"));
        dout >> str_arr;
        for (size_t i = 0; i < (*str_arr).length(); i++)
            serv_dev_vec.push_back(string((*str_arr)[i].in()));

        usr_dev_vec.push_back("DevTest::" + device1_name);
        usr_dev_vec.push_back("DevTest::" + device2_name);
        usr_dev_vec.push_back("DevTest::" + device3_name);

        sort(serv_dev_vec.begin(), serv_dev_vec.end());    // sort expected and returned device names
        sort(usr_dev_vec.begin(), usr_dev_vec.end());        // in alphabetical order to compare the vectors

        TS_ASSERT(serv_dev_vec[0] == usr_dev_vec[0]);
        TS_ASSERT(serv_dev_vec[1] == usr_dev_vec[1]);
        TS_ASSERT(serv_dev_vec[2] == usr_dev_vec[2]);
    }

// Test trace levels commands

    void test_trace_levels_commands(void) {
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
        CxxTest::TangoPrinter::restore_set("logging_level");

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
        CxxTest::TangoPrinter::restore_unset("logging_level");

        // once more get logging level and check if set to defaults
        din << dserver_name_in;
        TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("GetLoggingLevel", din));
        dout >> dserver_level_out;
        TS_ASSERT((*dserver_level_out).lvalue[0] == dsloglevel);
        TS_ASSERT((*dserver_level_out).svalue[0].in() == dserver_name);
    }

// Test set output file commands

    void test_set_output_file_commands(void) {
        DeviceData din, dout;

        // check if logging target is not set
        const DevVarStringArray *logging_target_out;
        din << dserver_name;
        TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("GetLoggingTarget", din));
        dout >> logging_target_out;
        TS_ASSERT((*logging_target_out).length() == 1);//console::cout

        // set logging level to 5
        DevVarLongStringArray dserver_level_in;
        dserver_level_in.lvalue.length(1);
        dserver_level_in.lvalue[0] = 5;
        dserver_level_in.svalue.length(1);
        dserver_level_in.svalue[0] = dserver_name.c_str();
        din << dserver_level_in;
        TS_ASSERT_THROWS_NOTHING(dserver->command_inout("SetLoggingLevel", din));
        CxxTest::TangoPrinter::restore_set("logging_level");

        // try to add logging target as a file which cannot be opened
        // note: we do not want to depend on current user permissions
        // so that the testcase is portable. We try to create the log
        // file in a location that no one (even root) can write to.
        DevVarStringArray fake_logging_target;
        fake_logging_target.length(2);
        fake_logging_target[0] = dserver_name.c_str();
        fake_logging_target[1] = string("file::/sys/cxx_dserver_cmd.out").c_str();
        din << fake_logging_target;

        TS_ASSERT_THROWS_ASSERT(dserver->command_inout("AddLoggingTarget", din), Tango::DevFailed & e,
                                TS_ASSERT(string(e.errors[0].reason.in()) == API_CannotOpenFile
                                          && e.errors[0].severity == Tango::ERR));

        // add logging target
        DevVarStringArray logging_target;
        logging_target.length(2);
        logging_target[0] = dserver_name.c_str();
        logging_target[1] = string("file::" + out_file).c_str();
        din << logging_target;
        TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddLoggingTarget", din));
        CxxTest::TangoPrinter::restore_set("logging_target");

        // query device server class
        const DevVarStringArray *query_class_out;
        TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("QueryClass"));
        dout >> query_class_out;
        TS_ASSERT(string((*query_class_out)[0].in()) == "DevTest");

        // remove logging target
        DevVarStringArray remove_logging_target;
        remove_logging_target.length(2);
        remove_logging_target[0] = dserver_name.c_str();
        remove_logging_target[1] = string("file::" + out_file).c_str();
        din << remove_logging_target;
        TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemoveLoggingTarget", din));
        CxxTest::TangoPrinter::restore_unset("logging_target");

        // set logging level back to defaults
        DevVarLongStringArray reset_dserver_level;
        reset_dserver_level.lvalue.length(1);
        reset_dserver_level.lvalue[0] = dsloglevel;
        reset_dserver_level.svalue.length(1);
        reset_dserver_level.svalue[0] = dserver_name.c_str();
        din << reset_dserver_level;
        TS_ASSERT_THROWS_NOTHING(dserver->command_inout("SetLoggingLevel", din));
        CxxTest::TangoPrinter::restore_unset("logging_level");

        // check if logging target was removed
        const DevVarStringArray *check_logging_target;
        din << dserver_name;
        TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("GetLoggingTarget", din));
        dout >> check_logging_target;
        TS_ASSERT((*check_logging_target).length() == 1);//console::cout
    }

// Test comparing input with output

    void test_comparing_input_with_output(void) {
        try {
            vector <string> v_s;
            v_s.push_back("push_heartbeat_event()");
            v_s.push_back("Sending event heartbeat");
            v_s.push_back("Event heartbeat");
            v_s.push_back("Sub device property");
            v_s.push_back("Sleep for :");
            CmpTst::CompareTest::out_remove_entries(out_file, v_s);

            map <string, string> prop_val_map;
            prop_val_map["timestamp"] = "10";
            prop_val_map["thread"] = "1";
            CmpTst::CompareTest::out_set_event_properties(out_file, prop_val_map);


            map <string, string> key_val_map;
            key_val_map["DSERVER"] = dserver_name;
            key_val_map["FILE"] = out_file;
            CmpTst::CompareTest::ref_replace_keywords(ref_file, key_val_map);

            map <string, string> prefix_num_map;
            prefix_num_map["thread = "] = "2";
            CmpTst::CompareTest::out_set_replace_numbers(out_file, prefix_num_map);

            CmpTst::CompareTest::compare(ref_file, out_file);
            CmpTst::CompareTest::clean_up(ref_file, out_file);
        }
        catch (CmpTst::CompareTestException &e) {
            try {
                CmpTst::CompareTest::leave_output(ref_file);
            }
            catch (CmpTst::CompareTestException &in_e) {
                cout << in_e.what() << endl;
            }
            TS_FAIL(e.what());
        }
    }
};

#undef cout
#endif // DServerCmdTestSuite_h
