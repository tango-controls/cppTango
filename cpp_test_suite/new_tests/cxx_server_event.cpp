//
// Created by ingvord on 12/14/16.
//
#ifndef ServerEventTestSuite_h
#define ServerEventTestSuite_h


#include "cxx_common.h"

#define    coutv    if (verbose == true) cout

#undef SUITE_NAME
#define SUITE_NAME ServerEventTestSuite

template <typename TEvent>
struct EventCallback : public Tango::CallBack
{
    EventCallback()
        : num_of_all_events(0)
        , num_of_error_events(0)
    {}

    void push_event(TEvent* event)
    {
        num_of_all_events++;
        if (event->err)
        {
            num_of_error_events++;
        }
    }

    int num_of_all_events;
    int num_of_error_events;
};

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

    /**
     * Tests that the client can still receive events after the device server is
     * shut down, renamed in the database and then restarted. This scenario used
     * to fail as reported in #679.
     */
    void test_reconnection_after_ds_instance_rename()
    {
        const std::string new_instance_name = "renamed_ds";
        const std::string new_ds_name = "DevTest/" + new_instance_name;
        const std::string old_ds_name = "DevTest/" + device1_instance_name;

        const std::string attribute_name = "event_change_tst";

        EventCallback<Tango::EventData> callback{};

        int subscription{};
        TS_ASSERT_THROWS_NOTHING(subscription = device1->subscribe_event(
            attribute_name,
            Tango::USER_EVENT,
            &callback));

        TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOPushEvent"));
        Tango_sleep(1);
        TS_ASSERT_EQUALS(2, callback.num_of_all_events);
        TS_ASSERT_EQUALS(0, callback.num_of_error_events);

        CxxTest::TangoPrinter::kill_server();

        Database db{};
        db.rename_server(old_ds_name, new_ds_name);

        CxxTest::TangoPrinter::start_server(new_instance_name);
        Tango_sleep(EVENT_HEARTBEAT_PERIOD); // Wait for reconnection

        Tango_sleep(1);
        TS_ASSERT_EQUALS(4, callback.num_of_all_events);
        TS_ASSERT_EQUALS(1, callback.num_of_error_events);

        TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOPushEvent"));
        Tango_sleep(1);
        TS_ASSERT_EQUALS(5, callback.num_of_all_events);
        TS_ASSERT_EQUALS(1, callback.num_of_error_events);

        TS_ASSERT_THROWS_NOTHING(device1->unsubscribe_event(subscription));

        db.rename_server(new_ds_name, old_ds_name);
    }
};

#undef cout
#endif // ServerEventTestSuite_h




