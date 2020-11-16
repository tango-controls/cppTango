#ifndef DServerMiscTestSuite_h
#define DServerMiscTestSuite_h

#include "cxx_common.h"
#include <thread>
#include <chrono>

#undef SUITE_NAME
#define SUITE_NAME DServerMiscTestSuite

using namespace std::chrono_literals;

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

class DServerMiscTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;
	string device1_name, dserver_name, full_ds_name, server_host, doc_url;
	DevLong server_version;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		device1_name = CxxTest::TangoPrinter::get_param("device1");
		full_ds_name = CxxTest::TangoPrinter::get_param("fulldsname");
		dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");
		server_host = CxxTest::TangoPrinter::get_param("serverhost");
		server_version = atoi(CxxTest::TangoPrinter::get_param("serverversion").c_str());
		doc_url = CxxTest::TangoPrinter::get_param("docurl");

		CxxTest::TangoPrinter::validate_args();

// For Windows / Linux compatibility
		string::size_type pos = server_host.find('.');
		pos--;
		if (server_host[pos] == 0x0d)
		{
			server_host.erase(pos,1);
		}


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
		// set the Tango::ON state on suite tearDown() in case a test fails leaving Tango::OFF status
		DeviceData din;
		din << device1_name;
		try
		{
			dserver->command_inout("DevRestart", din);
		}
		catch(CORBA::Exception &e)
		{
			cout << endl << "Exception in suite tearDown():" << endl;
			Except::print_exception(e);
			exit(-1);
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

// Test State and Status commands

	void test_State_and_Status_commands(void)
	{
		DeviceData dout;
		DevState state;
		string str;

		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("Status"));
		dout >> str;
		TS_ASSERT(str == "The device is ON\nThe polling is ON");

		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
	}

// Test DevRestart command on the dserver device

	void test_DevRestart_command_on_the_dserver_device(void)
	{
		DeviceData din, dout;
		DevState state_in, state_out;
		string str;

		state_in = Tango::OFF;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::OFF);

		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RestartServer"));
		Tango_sleep(3);

		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::ON);
	}

// Test DevRestart command on classical device

	void test_DevRestart_command_on_classical_device(void)
	{
		DeviceData din, dout;
		DevState state_in, state_out;
		string str;

		str = "a/b/c";
		din << str;
		TS_ASSERT_THROWS_ASSERT(dserver->command_inout("DevRestart", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_DeviceNotFound"
						&& e.errors[0].severity == Tango::ERR));

		state_in = Tango::OFF;
		din << state_in;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::OFF);

		str = device1_name;
		din << str;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RestartServer", din));
		Tango_sleep(6);

		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state_out;
		TS_ASSERT(state_out == Tango::ON);
	}

// Test name, description, state ans status CORBA attributes

	void test_name_description_state_ans_status_CORBA_attributes(void)
	{
		DeviceData dout;
		DevState state_out;
		string str;

		TS_ASSERT_THROWS_NOTHING(str = dserver->name());
		TS_ASSERT(str == dserver_name);

		TS_ASSERT_THROWS_NOTHING(str = dserver->description());
		TS_ASSERT(str == "A device server device !!");

		TS_ASSERT_THROWS_NOTHING(str = dserver->status());
cout << "str = " << str << endl;
		TS_ASSERT(str == "The device is ON\nThe polling is ON");

		TS_ASSERT_THROWS_NOTHING(state_out = dserver->state());
		TS_ASSERT(state_out == Tango::ON);
	}

// Ping the device

	void test_ping_the_device(void)
	{
		TS_ASSERT_THROWS_NOTHING(dserver->ping());
	}

// Test info call

	void test_info_call(void)
	{
		TS_ASSERT(dserver->info().dev_class == "DServer");
		// TODO: uncomment the following if needed
//		TS_ASSERT(dserver->info().dev_type == "Uninitialised");
		TS_ASSERT(dserver->info().doc_url == "Doc URL = " + doc_url);
		TS_ASSERT(dserver->info().server_host == server_host);
		TS_ASSERT(dserver->info().server_id == full_ds_name);
		TS_ASSERT(dserver->info().server_version == server_version);
	}

    /* Tests that subscriber can receive events immediately after
     * a device restart without a need to wait for re-subscription.
     */
    void test_event_subscription_recovery_after_device_restart()
    {
        EventCallback<Tango::EventData> callback{};

        std::string attribute_name = "event_change_tst";

        TS_ASSERT_THROWS_NOTHING(device1->subscribe_event(
            attribute_name,
            Tango::USER_EVENT,
            &callback));

        TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOPushEvent"));
        Tango_sleep(2);
        TS_ASSERT_EQUALS(2, callback.num_of_all_events);
        TS_ASSERT_EQUALS(0, callback.num_of_error_events);

        {
            Tango::DeviceData input{};
            input << device1_name;
            TS_ASSERT_THROWS_NOTHING(dserver->command_inout("DevRestart", input));
        }

        TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOPushEvent"));
        Tango_sleep(2);
        TS_ASSERT_EQUALS(3, callback.num_of_all_events);
        TS_ASSERT_EQUALS(0, callback.num_of_error_events);
    }

    /* Tests that attribute configuration change event
     * is sent to all subscribers after device restart.
     */
    void test_attr_conf_change_event_after_device_restart()
    {
        EventCallback<Tango::AttrConfEventData> callback{};

        const std::string attribute_name = "event_change_tst";

        int subscription = 0;
        TS_ASSERT_THROWS_NOTHING(subscription = device1->subscribe_event(
            attribute_name,
            Tango::ATTR_CONF_EVENT,
            &callback));

        Tango_sleep(1);
        TS_ASSERT_EQUALS(1, callback.num_of_all_events);
        TS_ASSERT_EQUALS(0, callback.num_of_error_events);

        {
            Tango::DeviceData input{};
            input << device1_name;
            TS_ASSERT_THROWS_NOTHING(dserver->command_inout("DevRestart", input));
        }

        Tango_sleep(1);
        TS_ASSERT_EQUALS(2, callback.num_of_all_events);
        TS_ASSERT_EQUALS(0, callback.num_of_error_events);

        TS_ASSERT_THROWS_NOTHING(device1->unsubscribe_event(subscription));
    }

/*
 * Test to check that client still receives periodic archive events after
 * polling is stopped and started again for particular device. Such scenario
 * used to fail as described in #675.
 */
    void test_archive_periodic_events_after_polling_restart()
    {
        constexpr auto poll_period = 1000ms;
        constexpr auto time_buffer = 100ms;
        constexpr auto poll_period_ms = std::chrono::milliseconds(poll_period).count();

        std::string attribute_name = "PollLong_attr";

        auto config = device1->get_attribute_config(attribute_name);
        config.events.arch_event.archive_period = std::to_string(poll_period_ms);
        AttributeInfoListEx config_in = { config };
        TS_ASSERT_THROWS_NOTHING(device1->set_attribute_config(config_in));

        TS_ASSERT_THROWS_NOTHING(device1->poll_attribute(attribute_name, poll_period_ms));

        EventCallback<Tango::EventData> callback{};

        int subscription = 0;
        TS_ASSERT_THROWS_NOTHING(subscription = device1->subscribe_event(
            attribute_name,
            Tango::ARCHIVE_EVENT,
            &callback));

        std::this_thread::sleep_for(poll_period + time_buffer);
        TS_ASSERT_EQUALS(0, callback.num_of_error_events);
        TS_ASSERT_EQUALS(2, callback.num_of_all_events);

        TS_ASSERT_THROWS_NOTHING(device1->stop_poll_attribute(attribute_name));

        std::this_thread::sleep_for(time_buffer);
        TS_ASSERT_EQUALS(1, callback.num_of_error_events);
        TS_ASSERT_EQUALS(3, callback.num_of_all_events);

        TS_ASSERT_THROWS_NOTHING(device1->poll_attribute(attribute_name, poll_period_ms));

        std::this_thread::sleep_for(poll_period + time_buffer);
        TS_ASSERT_EQUALS(1, callback.num_of_error_events);
        TS_ASSERT_EQUALS(4, callback.num_of_all_events);

        std::this_thread::sleep_for(poll_period);
        TS_ASSERT_EQUALS(1, callback.num_of_error_events);
        TS_ASSERT_EQUALS(5, callback.num_of_all_events);

        TS_ASSERT_THROWS_NOTHING(device1->unsubscribe_event(subscription));
    }

/*
 * Tests that subscriber can receive pipe events immediately after
 * server is restarted due to RestartServer admin device command
 * without a need to wait for re-subscription.
 */
    void test_pipe_event_subscription_recovery_after_restart_server_command()
    {
        EventCallback<Tango::PipeEventData> callback{};

        TS_ASSERT_THROWS_NOTHING(device1->subscribe_event(
            "RWPipe",
            Tango::PIPE_EVENT,
            &callback));

        TS_ASSERT_THROWS_NOTHING(push_pipe_event());
        Tango_sleep(1);
        TS_ASSERT_EQUALS(2, callback.num_of_all_events);
        TS_ASSERT_EQUALS(0, callback.num_of_error_events);

        TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RestartServer"));
        Tango_sleep(5);

        TS_ASSERT_THROWS_NOTHING(push_pipe_event());
        Tango_sleep(1);
        TS_ASSERT_EQUALS(3, callback.num_of_all_events);
        TS_ASSERT_EQUALS(0, callback.num_of_error_events);

        {
            Tango::DeviceData input{};
            input << device1_name;
            TS_ASSERT_THROWS_NOTHING(dserver->command_inout("DevRestart", input));
        }

        TS_ASSERT_THROWS_NOTHING(push_pipe_event());
        Tango_sleep(1);
        TS_ASSERT_EQUALS(4, callback.num_of_all_events);
        TS_ASSERT_EQUALS(0, callback.num_of_error_events);
    }

    void push_pipe_event()
    {
        Tango::DevShort event_type = 0;
        Tango::DeviceData data;
        data << event_type;
        device1->command_inout("PushPipeEvent", data);
    }
};
#undef cout
#endif // DServerMiscTestSuite_h
