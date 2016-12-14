//
// Created by ingvord on 12/14/16.
//
#ifndef ServerEventTestSuite_h
#define ServerEventTestSuite_h


#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"
#define    coutv    if (verbose == true) cout

#undef SUITE_NAME
#define SUITE_NAME ServerEventTestSuite

class ServerEventTestSuite : public CxxTest::TestSuite {
protected:
    DeviceProxy *device1, *device2;
    string device1_name, device2_name, device1_instance_name, device2_instance_name;
    bool verbose;
    DevLong eve_id;

public:
    SUITE_NAME():
            device1_instance_name{"test"},//TODO pass via cl
            device2_instance_name{"test2"}
            {

//
// Arguments check -------------------------------------------------
//

        device1_name = CxxTest::TangoPrinter::get_param("device1");
        device2_name = CxxTest::TangoPrinter::get_param("device20");

        verbose = CxxTest::TangoPrinter::is_param_defined("verbose");

        CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

        try {
            device1 = new DeviceProxy(device1_name);
            device2 = new DeviceProxy(device2_name);

            //TODO start server 2 and set fallback point
            CxxTest::TangoPrinter::start_server(device2_instance_name);
            CxxTest::TangoPrinter::restore_set("test2/debian8/20 started.");
        }
        catch (CORBA::Exception &e) {
            Except::print_exception(e);
            exit(-1);
        }

    }

    virtual ~SUITE_NAME() {
        if (CxxTest::TangoPrinter::is_restore_set("test2/debian8/20 started."))
            CxxTest::TangoPrinter::kill_server();

        CxxTest::TangoPrinter::start_server(device1_instance_name);

        delete device1;
        delete device2;
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

//
// Ask the device server to subscribe to an event
//
    void test_device_server_subscribe_to_event(void) {
        coutv << endl << "new DeviceProxy(" << device1->name() << ") returned" << endl << endl;


        vector<string> vs{device2_name, "Short_attr", "periodic"};

        DeviceData dd_in, dd_out;
        dd_in << vs;
        TS_ASSERT_THROWS_NOTHING(dd_out = device1->command_inout("IOSubscribeEvent", dd_in));
        dd_out >> eve_id;
    }



//
// Wait for event to be executed
//

    void test_wait_event(void) {
        Tango_sleep(3);

        DeviceData da;
        TS_ASSERT_THROWS_NOTHING(da = device1->command_inout("IOGetCbExecuted"));
        Tango::DevLong cb;
        da >> cb;

        coutv << "cb executed = " << cb << endl;
        TS_ASSERT_LESS_THAN_EQUALS(2, cb);
        TS_ASSERT_LESS_THAN_EQUALS(cb, 4);
    }

//
// Ask server to unsubsribe from event
//
    void test_server_unsubscribes_from_event(void) {
        DeviceData dd_un;
        dd_un << eve_id;

        DeviceData da;
        TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOUnSubscribeEvent", dd_un));
        TS_ASSERT_THROWS_NOTHING(da = device1->command_inout("IOGetCbExecuted"));

        Tango::DevLong cb;
        da >> cb;

        Tango_sleep(2);
        TS_ASSERT_THROWS_NOTHING(da = device1->command_inout("IOGetCbExecuted"));
        Tango::DevLong cb2;
        da >> cb2;

        TS_ASSERT_EQUALS(cb2, cb);
    }
};

#undef cout
#endif // ServerEventTestSuite_h




