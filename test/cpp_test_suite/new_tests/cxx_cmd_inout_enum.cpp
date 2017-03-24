//
// Created by ingvord on 3/13/17.
//
#ifndef CmdInOutEnumTestSuite_h
#define CmdInOutEnumTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

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

    void test_cmd_DevEnum_query(void){
        CommandInfo cmd_info;

        cmd_info = device1->command_query("IODevEnum");


        TS_ASSERT_EQUALS("IODevEnum", cmd_info.cmd_name);
        TS_ASSERT_EQUALS("Some label", cmd_info.in_enum_labels[0]);
    }


void test_cmd_DevEnum_in(void){
    DeviceData in, out;
    in << (DevEnum)1;

    out = device1->command_inout("IODevEnum", in);


    short result;
    out >> result;

    TS_ASSERT_EQUALS(1, result);
}

};
#undef cout
#endif // CmdInOutEnumTestSuite_h


