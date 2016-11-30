#ifndef McastSimpleTestSuite_h
#define McastSimpleTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define coutv	if (verbose == true) cout << "\t"
#define coutv_cb 	if (parent->verbose == true) cout << "\t"
#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME McastSimpleTestSuite

//
// In this test, the client subscribes to two multicast events
// One event comes from a device running in the same host than the client
// The other event comes from a device running in a different host (therefore using PGM to communicate)
// These two events are using different callback objects
//

class McastSimpleTestSuite: public CxxTest::TestSuite
{
public:
	class EventCallBack : public Tango::CallBack
	{
	public:
		EventCallBack(McastSimpleTestSuite *ptr):parent(ptr) {}
		void push_event(Tango::EventData*);

		int 	cb_executed;
		int 	cb_err;
		long 	val;
		long 	val_size;

	private:
		McastSimpleTestSuite	*parent;
	};

protected:
	DeviceProxy 	*local_device;
	DeviceProxy		*remote_device;
	string 			att_name;
	int 			local_eve_id;
	int				remote_eve_id;
	EventCallBack 	*cb_local;
	EventCallBack   *cb_remote;

	bool			verbose;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string local_device_name;
		string remote_device_name;

		cb_local = new EventCallBack(this);
		cb_remote = new EventCallBack(this);

		// local parameters, obtained from the command line
		local_device_name = CxxTest::TangoPrinter::get_param_loc("local_device","local device name");
		remote_device_name = CxxTest::TangoPrinter::get_param_loc("remote_device","remote device name");

		verbose = CxxTest::TangoPrinter::is_param_set("verbose");

		// always add this line, otherwise arguments will not be parsed correctly
		CxxTest::TangoPrinter::validate_args();

//
// Initialization --------------------------------------------------
//

		att_name = "Event_change_tst";

		local_init(local_device_name,local_device);
		local_init(remote_device_name,remote_device);

		cb_local->cb_executed = 0;
		cb_local->cb_err = 0;

		cb_remote->cb_executed = 0;
		cb_remote->cb_err = 0;
	}

	virtual ~SUITE_NAME()
	{
		local_device->stop_poll_attribute(att_name);
		remote_device->stop_poll_attribute(att_name);

		delete local_device;
		delete remote_device;

		delete cb_local;
		delete cb_remote;
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

	void test_Subscribe_multicast_event_locally(void)
	{
		
// switch on the polling first!

		local_device->poll_attribute(att_name,1000);
		local_eve_id = local_device->subscribe_event(att_name,Tango::CHANGE_EVENT,cb_local);

// Check that the attribute is now polled at 1000 mS

		bool po = local_device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		TS_ASSERT ( po == true);
		
		int poll_period = local_device->get_attribute_poll_period(att_name);
		coutv << "att polling period : " << poll_period << endl;
		TS_ASSERT( poll_period == 1000);	
	}

	void test_Subscribe_multicast_event_remotely(void)
	{

// For the remote device

		remote_device->poll_attribute(att_name,1000);
		remote_eve_id = remote_device->subscribe_event(att_name,Tango::CHANGE_EVENT,cb_remote);

// Check that the attribute is now polled at 1000 mS

		bool po = remote_device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		TS_ASSERT ( po == true);
		
		int poll_period = remote_device->get_attribute_poll_period(att_name);
		coutv << "att polling period : " << poll_period << endl;
		TS_ASSERT( poll_period == 1000);		
	}

// Check that first point has been received

	void test_first_point_received_locally_and_remotely(void)
	{
		TS_ASSERT (cb_local->cb_executed == 1);
		TS_ASSERT (cb_local->val == 30);
		TS_ASSERT (cb_local->val_size == 4);

		TS_ASSERT (cb_remote->cb_executed == 1);
		TS_ASSERT (cb_remote->val == 30);
		TS_ASSERT (cb_remote->val_size == 4);
	}

	void test_Callback_executed_after_a_change_localy_and_remotely(void)
	{
#ifndef WIN32		
		int rest = sleep(1);
		if (rest != 0)
			sleep(1);
#else
		Sleep(1000);
#endif
			
		local_device->command_inout("IOIncValue");
		remote_device->command_inout("IOIncValue");

#ifndef WIN32
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif
						
		coutv << "local cb excuted = " << cb_local->cb_executed << endl;
		coutv << "remote cb executed = " << cb_remote->cb_executed << endl;

		TS_ASSERT (cb_local->cb_executed == 2);
		TS_ASSERT (cb_local->val == 31);
		TS_ASSERT (cb_local->val_size == 4);

		TS_ASSERT (cb_remote->cb_executed == 2);
		TS_ASSERT (cb_remote->val == 31);
		TS_ASSERT (cb_remote->val_size == 4);
	}

// unsubscribe to the event

	void test_unsubscribe_event_localy_and_remotely(void)
	{
		local_device->unsubscribe_event(local_eve_id);
		remote_device->unsubscribe_event(remote_eve_id);
	}

//---------------------------------------------------------------------

	void local_init(string &dev_name,Tango::DeviceProxy *&dev_ptr)
	{
		try
		{
			dev_ptr = new DeviceProxy(dev_name);
				
//
// Test set up (stop polling and clear abs_change and rel_change attribute
// properties but restart device to take this into account)
// Set the abs_change to 1
//

			if (dev_ptr->is_attribute_polled(att_name))
				dev_ptr->stop_poll_attribute(att_name);

			DbAttribute dba(att_name,dev_name);
			DbData dbd;
			DbDatum a(att_name);
			a << (short)2;
			dbd.push_back(a);
			dbd.push_back(DbDatum("abs_change"));
			dbd.push_back(DbDatum("rel_change"));
			dba.delete_property(dbd);
		
			dbd.clear();
			a << (short)1;
			dbd.push_back(a);
			DbDatum ch("abs_change");
			ch << (short)1;
			dbd.push_back(ch);
			dba.put_property(dbd);
		
			DeviceProxy adm_dev(dev_ptr->adm_name().c_str());
			DeviceData di;
			di << dev_name;
			adm_dev.command_inout("DevRestart",di);
		
			delete dev_ptr;

			dev_ptr = new DeviceProxy(dev_name);
			Tango_sleep(1);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
	}
};

void McastSimpleTestSuite::EventCallBack::push_event(Tango::EventData* event_data)
{
	vector<DevLong> value;

	cb_executed++;

	try
	{
		coutv_cb << "EventCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << endl;
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
#endif // McastSimpleTestSuite_h

