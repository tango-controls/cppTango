#ifndef SvrMcastLocalRemoteTestSuite_h
#define SvrMcastLocalRemoteTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define coutv		if (verbose == true) cout << "\t"
#define coutv_cb 	if (parent->verbose == true) cout << "\t"
#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME SvrMcastLocalRemoteTestSuite


//
// In this test, the local server is used to send mcast event locally (to this client) but
// also to another client running on a remote host. This another client is in fact a device server
// as well with the commands IOSubscribeEvent, IOGetCbExecuted and IOUnsubscribeEvent.
// This allows us to test the case where we have a server firing multicast event to local and
// remote clients
//

class SvrMcastLocalRemoteTestSuite: public CxxTest::TestSuite
{
public:
	class EventCallBack : public Tango::CallBack
	{
	public:
		EventCallBack(SvrMcastLocalRemoteTestSuite *ptr):parent(ptr) {}
		void push_event(Tango::EventData*);

		int 	cb_executed;
		int 	cb_err;
		long 	val;
		long 	val_size;

	private:
		SvrMcastLocalRemoteTestSuite	*parent;
	};

protected:
	DeviceProxy 	*device_local;
	DeviceProxy		*device_remote;
	string 			att_name;
	string			local_device_name;
	int 			eve_id_local;
	int 			eve_id_remote;
	EventCallBack 	*cb;

	bool 			verbose;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string remote_device_name;

		// user arguments, obtained from the command line sequentially

		local_device_name = CxxTest::TangoPrinter::get_param_loc("local_device","local device name");
		remote_device_name = CxxTest::TangoPrinter::get_param_loc("remote_device","remote device name");

		verbose = CxxTest::TangoPrinter::is_param_set("verbose");

		// always add this line, otherwise arguments will not be parsed correctly
		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

		cb = new EventCallBack(this);

		try
		{
			device_local = new DeviceProxy(local_device_name);
			device_remote = new DeviceProxy(remote_device_name);

			att_name = "Event_change_tst";
				
//
// Test set up (stop polling and clear abs_change and rel_change attribute
// properties but restart device to take this into account)
// Set the abs_change to 1
//

			if (device_local->is_attribute_polled(att_name))
				device_local->stop_poll_attribute(att_name);

			DbAttribute db_local(att_name,local_device_name);

			DbData dbd;
			DbDatum a(att_name);
			a << (short)2;
			dbd.push_back(a);
			dbd.push_back(DbDatum("abs_change"));
			dbd.push_back(DbDatum("rel_change"));
			db_local.delete_property(dbd);
		
			dbd.clear();
			a << (short)1;
			dbd.push_back(a);
			DbDatum ch("abs_change");
			ch << (short)1;
			dbd.push_back(ch);
			db_local.put_property(dbd);
		
			DeviceProxy adm_dev_local(device_local->adm_name().c_str());
			DeviceData di;
			di << local_device_name;
			adm_dev_local.command_inout("DevRestart",di);
		
			delete device_local;

			device_local = new DeviceProxy(local_device_name);
			Tango_sleep(1);

			cb->cb_executed = 0;
			cb->cb_err = 0;
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{
		if (CxxTest::TangoPrinter::is_restore_set("remote_subscribe"))
		{
			DeviceData dd;
			dd << eve_id_remote;
			device_remote->command_inout("IOUnsubscribeEvent",dd);
		}

		delete device_local;
		delete device_remote;

		delete cb;
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

// Test subscribe_event call

	void test_Subscribe_multicast_events(void)
	{
		
// switch on the polling first!

		device_local->poll_attribute(att_name,1000);
		eve_id_local = device_local->subscribe_event(att_name,Tango::CHANGE_EVENT,cb);

// Check that the attribute is now polled at 1000 mS

		bool po = device_local->is_attribute_polled(att_name);
		coutv << "Local device: attribute polled : " << po << endl;
		TS_ASSERT ( po == true);
		
		int poll_period = device_local->get_attribute_poll_period(att_name);
		coutv << "Local device: att polling period : " << poll_period << endl;
		TS_ASSERT( poll_period == 1000);

// Ask remote device to subscribe to the same event

		vector<string> vs;
		vs.push_back(local_device_name);
		vs.push_back(att_name);
		vs.push_back("change");

		DeviceData dd_in,dd_out;
		dd_in << vs;
		CxxTest::TangoPrinter::restore_set("remote_subscribe");
		dd_out = device_remote->command_inout("IOSubscribeEvent",dd_in);
		dd_out >> eve_id_remote;
		coutv << "Remote event id = " << eve_id_remote << endl;
	}

// Check that first point has been received

	void test_first_point_received(void)
	{
		TS_ASSERT (cb->cb_executed == 1);
		TS_ASSERT (cb->val_size == 4);

		DeviceData da;
		da = device_remote->command_inout("IOGetCbExecuted");
		long cb = 0;
		da >> cb;
		coutv << "Remote event ctr = " << cb << endl;
		TS_ASSERT (cb == 1);
	}

	void test_Callback_executed_after_a_change_for_both_events(void)
	{
#ifndef WIN32		
		int rest = sleep(1);
		if (rest != 0)
			sleep(1);
#else
		Sleep(1000);
#endif
			
		device_local->command_inout("IOIncValue");

#ifndef WIN32
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif
						
		coutv << "local cb excuted = " << cb->cb_executed << endl;

		TS_ASSERT (cb->cb_executed == 2);
		TS_ASSERT (cb->val_size == 4);

		DeviceData da;
		da = device_remote->command_inout("IOGetCbExecuted");
		long cb = 0;
		da >> cb;
		coutv << "remote cb excuted = " << cb << endl;
		TS_ASSERT (cb == 2);
	}

// unsubscribe to the event. No need to stop polling on local device. The IOUnsubscribeEvent on
// remote device will take care of this

	void test_unsubscribe_event(void)
	{
		DeviceData dd;
		dd << eve_id_remote;
		device_remote->command_inout("IOUnsubscribeEvent",dd);
		CxxTest::TangoPrinter::restore_unset("remote_subscribe");

		device_local->unsubscribe_event(eve_id_local);
	}

};

void SvrMcastLocalRemoteTestSuite::EventCallBack::push_event(Tango::EventData* event_data)
{
	vector<DevLong> value;

	cb_executed++;

	try
	{
		coutv_cb << "EventCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << "\n";
		if (!event_data->err)
		{
			*(event_data->attr_value) >> value;
			val = value[2];
			val_size = value.size();
		}
		else
		{
			if (strcmp(event_data->errors[0].reason.in(),"bbb") == 0)
				cb_err++;
		}
	}
	catch (...)
	{
		coutv_cb << "EventCallBack::push_event(): could not extract data !\n";
	}

}

#undef cout
#endif // SvrMcastLocalRemoteTestSuite_h

