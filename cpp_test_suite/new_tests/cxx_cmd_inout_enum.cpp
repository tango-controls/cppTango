//
// Created by ingvord on 3/13/17.
//
#ifndef CmdInOutEnumTestSuite_h
#define CmdInOutEnumTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>
#include <thread>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME CmdInOutEnumTestSuite

class CmdInOutEnumTestSuite: public CxxTest::TestSuite
{
protected:
    DeviceProxy *device1, *dserver;

public:
    SUITE_NAME()
    {

//
// Arguments check -------------------------------------------------
//

        string device1_name, dserver_name;

        device1_name = CxxTest::TangoPrinter::get_param("device1");
        dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");

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

    }

    virtual ~SUITE_NAME()
    {
        if (CxxTest::TangoPrinter::is_restore_set("dev1_EnumLabelsDynCommandPolled_polling"))
        {
            DeviceData din;
            DevVarStringArray rem_attr_poll;
            rem_attr_poll.length(3);
            rem_attr_poll[0] = device1->name().c_str();
            rem_attr_poll[1] = "command";
            rem_attr_poll[2] = "EnumLabelsDynCommandPolled";
            din << rem_attr_poll;
            try
            {
                dserver->command_inout("RemObjPolling", din);
            }
            catch (DevFailed &e)
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

    void test_cmd_DevEnum_in(void)
    {
        DeviceData in, out;
        in << (DevEnum) 1;

        out = device1->command_inout("IODevEnum", in);


        short result;
        out >> result;

        TS_ASSERT_EQUALS(1, result);
    }

    void test_cmd_DevEnum_query(void)
    {
        CommandInfo cmd_info;

        cmd_info = device1->command_query("IODevEnum");


        TS_ASSERT_EQUALS("IODevEnum", cmd_info.cmd_name);
        TS_ASSERT_EQUALS(3, cmd_info.in_enum_labels.size());
        TS_ASSERT_EQUALS("IN Label 1", cmd_info.in_enum_labels[0]);
        TS_ASSERT_EQUALS("IN Label 2", cmd_info.in_enum_labels[1]);
        TS_ASSERT_EQUALS("IN Label 3", cmd_info.in_enum_labels[2]);
        TS_ASSERT_EQUALS(3, cmd_info.out_enum_labels.size());
        TS_ASSERT_EQUALS("OUT Label 1", cmd_info.out_enum_labels[0]);
        TS_ASSERT_EQUALS("OUT Label 2", cmd_info.out_enum_labels[1]);
        TS_ASSERT_EQUALS("OUT Label 3", cmd_info.out_enum_labels[2]);
    }

    void test_cmd_DevEnum_query_list(void)
    {
        auto cmd_info_list = device1->command_list_query();

        CommandInfo cmd_info;
        auto found = find_if(cmd_info_list->begin(), cmd_info_list->end(), [](CommandInfo item)
        {
            return item.cmd_name == "IODevEnum";
        });
        cmd_info = *found;

        TS_ASSERT_EQUALS("IODevEnum", cmd_info.cmd_name);
        TS_ASSERT_EQUALS(3, cmd_info.in_enum_labels.size());
        TS_ASSERT_EQUALS("IN Label 1", cmd_info.in_enum_labels[0]);
        TS_ASSERT_EQUALS("IN Label 2", cmd_info.in_enum_labels[1]);
        TS_ASSERT_EQUALS("IN Label 3", cmd_info.in_enum_labels[2]);
        TS_ASSERT_EQUALS(3, cmd_info.out_enum_labels.size());
        TS_ASSERT_EQUALS("OUT Label 1", cmd_info.out_enum_labels[0]);
        TS_ASSERT_EQUALS("OUT Label 2", cmd_info.out_enum_labels[1]);
        TS_ASSERT_EQUALS("OUT Label 3", cmd_info.out_enum_labels[2]);
    }

    void test_dyn_cmd_DevEnum_query(void)
    {
        auto cmd_info = device1->command_query("EnumLabelsDynCommand");

        TS_ASSERT_EQUALS("EnumLabelsDynCommand", cmd_info.cmd_name);
        TS_ASSERT_EQUALS(3, cmd_info.in_enum_labels.size());
        TS_ASSERT_EQUALS("IN Dyn Label 1", cmd_info.in_enum_labels[0]);
        TS_ASSERT_EQUALS("IN Dyn Label 2", cmd_info.in_enum_labels[1]);
        TS_ASSERT_EQUALS("IN Dyn Label 3", cmd_info.in_enum_labels[2]);
        TS_ASSERT_EQUALS(3, cmd_info.out_enum_labels.size());
        TS_ASSERT_EQUALS("OUT Dyn Label 1", cmd_info.out_enum_labels[0]);
        TS_ASSERT_EQUALS("OUT Dyn Label 2", cmd_info.out_enum_labels[1]);
        TS_ASSERT_EQUALS("OUT Dyn Label 3", cmd_info.out_enum_labels[2]);
    }

    void test_poll_cmd_DevEnum(void)
    {
        DevVarLongStringArray cmd_poll;
        DeviceData din;

        cmd_poll.lvalue.length(1);
        cmd_poll.lvalue[0] = 200;
        cmd_poll.svalue.length(3);
        cmd_poll.svalue[0] = device1->name().c_str();
        cmd_poll.svalue[1] = "command";
        cmd_poll.svalue[2] = "EnumLabelsDynCommandPolled";
        din << cmd_poll;
        TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
        CxxTest::TangoPrinter::restore_set("dev1_EnumLabelsDynCommandPolled_polling");

        TS_ASSERT_THROWS_NOTHING(device1->set_source(Tango::CACHE));

        std::this_thread::sleep_for(
            std::chrono::milliseconds(1000));

        DeviceData dout;
        DevEnum result;
        TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("EnumLabelsDynCommandPolled"));
        dout >> result;
        TS_ASSERT_EQUALS(result, 1);

    }
};
#undef cout
#endif // CmdInOutEnumTestSuite_h


