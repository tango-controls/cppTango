//
// Created by ingvord on 12/14/16.
//
#ifndef RecoZmqTestSuite_h
#define RecoZmqTestSuite_h


#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>
#include <thread>

using namespace Tango;
using namespace std;

#define cout cout << "\t"
#define    coutv    if (verbose == true) cout

#undef SUITE_NAME
#define SUITE_NAME RecoZmqTestSuite

class EventCallback : public Tango::CallBack
{
public:
    EventCallback()  { };
    ~EventCallback() { };
    void push_event( Tango::EventData *ed ){
        cout << "In callback with error flag = " << std::boolalpha << ed->err << endl;
        if(ed->err) {
            cb_err++;
            cout << "Error: " << ed->errors[0].reason << endl;
        } else {
            cb_executed++;
        }
    }


    int cb_executed;
    int cb_err;
};

class RecoZmqTestSuite : public CxxTest::TestSuite {
protected:
    DeviceProxy *device1, *device2;
    string device1_name, device2_name, device1_instance_name, device2_instance_name;
    bool verbose;
    EventCallback eventCallback;

public:
    SUITE_NAME() :
            device1_instance_name{"test"},//TODO pass via cl
            device2_instance_name{"test2"},
            eventCallback{}
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

            //sleep 18 &&  start_server "@INST_NAME@" &
            thread([this]() {
                Tango_sleep(18);
                CxxTest::TangoPrinter::start_server(device1_instance_name);
            }).detach();

            //sleep 62 &&  start_server "@INST_NAME@" &
            thread([this]() {
                Tango_sleep(62);
                CxxTest::TangoPrinter::start_server(device1_instance_name);
            }).detach();
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
// Subscribe to a user event
//
    void test_subscribe_to_user_event(void) {
        string att_name("event_change_tst");

        const vector<string> filters;
        eventCallback.cb_executed = 0;
        eventCallback.cb_err = 0;

        TS_ASSERT_THROWS_NOTHING(device1->subscribe_event(att_name, Tango::USER_EVENT, &eventCallback, filters));

//
// Fire one event
//

        TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOPushEvent"));
        TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOPushEvent"));

        Tango_sleep(1);

        coutv << "Callback execution before re-connection = " << eventCallback.cb_executed << endl;
        coutv << "Callback error before re-connection = " << eventCallback.cb_err << endl;

        TS_ASSERT_EQUALS (eventCallback.cb_executed, 3);
        TS_ASSERT_EQUALS (eventCallback.cb_err, 0);

//
// Kill device server (using its admin device)
//

        string adm_name = device1->adm_name();
        DeviceProxy admin_dev(adm_name);
        TS_ASSERT_THROWS_NOTHING(admin_dev.command_inout("kill"));

//
// Wait for some error and re-connection
//

        Tango_sleep(40);

//
// Check error and re-connection
//

        coutv << "Callback execution after re-connection = " << eventCallback.cb_executed << endl;
        coutv << "Callback error after re-connection = " << eventCallback.cb_err << endl;

        TS_ASSERT_LESS_THAN_EQUALS (1, eventCallback.cb_err);
        TS_ASSERT_EQUALS (eventCallback.cb_executed, 4);

//
// Fire another event
//

        TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOPushEvent"));
        TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOPushEvent"));

        Tango_sleep(1);

        coutv << "Callback execution after re-connection and event = " << eventCallback.cb_executed << endl;
        coutv << "Callback error after re-connection and event = " << eventCallback.cb_err << endl;

        TS_ASSERT_EQUALS (eventCallback.cb_executed, 6);
        TS_ASSERT_LESS_THAN_EQUALS (1, eventCallback.cb_err);
    }

//
// Clear call back counters and kill device server once more
//
    void test_clear_cb_kill_ds(void) {
        eventCallback.cb_executed = 0;
        eventCallback.cb_err = 0;

        string adm_name = device1->adm_name();
        DeviceProxy admin_dev(adm_name);
        TS_ASSERT_THROWS_NOTHING(admin_dev.command_inout("kill"));

//
// Wait for some error and re-connection
//

        Tango_sleep(40);

//
// Check error and re-connection
//

        coutv << "Callback execution after second re-connection = " << eventCallback.cb_executed << endl;
        coutv << "Callback error after second re-connection = " << eventCallback.cb_err << endl;

        TS_ASSERT_LESS_THAN_EQUALS (1, eventCallback.cb_err);
        TS_ASSERT_EQUALS (eventCallback.cb_executed, 1);

//
// Fire yet another event
//

        TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOPushEvent"));

        Tango_sleep(2);

        coutv << "Callback execution after second re-connection and event = " << eventCallback.cb_executed << endl;
        coutv << "Callback error after second re-connection and event = " << eventCallback.cb_err << endl;

        TS_ASSERT_EQUALS (eventCallback.cb_executed, 2);
        TS_ASSERT_LESS_THAN_EQUALS (1, eventCallback.cb_err);
    }
};

#undef cout
#endif // RecoZmqTestSuite_h

