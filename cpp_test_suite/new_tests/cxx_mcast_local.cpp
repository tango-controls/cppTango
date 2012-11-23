#ifndef McastLocalTestSuite_h
#define McastLocalTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define coutv	if (verbose == true) cout << "\t"
#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME McastLocalTestSuite

bool verbose = false;

class EventCallBack : public Tango::CallBack
{
	void push_event(Tango::EventData*);
	
public:
	int 	cb_executed;
	int 	cb_err;
	int 	old_sec,old_usec;
	int 	delta_msec;
	long 	val;
	long 	val_size;
};

void EventCallBack::push_event(Tango::EventData* event_data)
{
	vector<DevLong> value;
	struct timeval now_timeval;

#ifdef WIN32
	struct _timeb before_win;
	_ftime(&before_win);
	now_timeval.tv_sec = (unsigned long)before_win.time;
	now_timeval.tv_usec = (long)before_win.millitm * 1000;
#else
	gettimeofday(&now_timeval,NULL);
#endif
//	coutv << "date : tv_sec = " << now_timeval.tv_sec;
//	coutv << ", tv_usec = " << now_timeval.tv_usec << endl;

	delta_msec = ((now_timeval.tv_sec - old_sec) * 1000) + ((now_timeval.tv_usec - old_usec) / 1000);	

	old_sec = now_timeval.tv_sec;
	old_usec = now_timeval.tv_usec;
	
//	coutv << "delta_msec = " << delta_msec << endl;

	cb_executed++;

	try
	{
//		coutv << "EventCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << "\n";
		if (!event_data->err)
		{
			*(event_data->attr_value) >> value;
//			coutv << "CallBack value size " << value.size() << endl;
			val = value[2];
			val_size = value.size();
//			coutv << "Callback value " << val << endl;
		}
		else
		{
//			coutv << "Error send to callback" << endl;
//			Tango::Except::print_error_stack(event_data->errors);
			if (strcmp(event_data->errors[0].reason.in(),"bbb") == 0)
				cb_err++;
		}
	}
	catch (...)
	{
		coutv << "EventCallBack::push_event(): could not extract data !\n";
	}

}


class McastLocalTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy 	*device;
	string 			att_name;
	int 			eve_id;
	EventCallBack 	cb;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string device_name;

		// user arguments, obtained from the command line sequentially
		device_name = CxxTest::TangoPrinter::get_uarg("device","device name"); // get_uarg("device1") will also work

		string str = CxxTest::TangoPrinter::get_param_opt("verbose");
//		cout << "str = " << str << endl;

		// always add this line, otherwise arguments will not be parsed correctly
		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

		try
		{
			device = new DeviceProxy(device_name);

			att_name = "Event_change_tst";
				
//
// Test set up (stop polling and clear abs_change and rel_change attribute
// properties but restart device to take this into account)
// Set the abs_change to 1
//

			if (device->is_attribute_polled(att_name))
				device->stop_poll_attribute(att_name);

			DbAttribute dba(att_name,device_name);
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
		
			DeviceProxy adm_dev(device->adm_name().c_str());
			DeviceData di;
			di << device_name;
			adm_dev.command_inout("DevRestart",di);
		
			delete device;

			device = new DeviceProxy(device_name);
			Tango_sleep(1);

			cb.cb_executed = 0;
			cb.cb_err = 0;
			cb.old_sec = cb.old_usec = 0;
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{
		device->stop_poll_attribute(att_name);

		delete device;
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

		device->poll_attribute(att_name,1000);
		eve_id = device->subscribe_event(att_name,Tango::CHANGE_EVENT,&cb);

// Check that the attribute is now polled at 1000 mS

		bool po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		TS_ASSERT ( po == true);
		
		int poll_period = device->get_attribute_poll_period(att_name);
		coutv << "att polling period : " << poll_period << endl;
		TS_ASSERT( poll_period == 1000);		
	}

// Check that first point has been received

	void test_first_point_received(void)
	{
		TS_ASSERT (cb.cb_executed == 1);
		TS_ASSERT (cb.val == 30);
		TS_ASSERT (cb.val_size == 4);
	}

	void test_Callback_executed_after_a_change(void)
	{
#ifndef WIN32		
		int rest = sleep(1);
		if (rest != 0)
			sleep(1);
#else
		Sleep(1000);
#endif
			
		device->command_inout("IOIncValue");

#ifndef WIN32
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif
						
		coutv << "cb excuted = " << cb.cb_executed << endl;

		TS_ASSERT (cb.cb_executed == 2);
		TS_ASSERT (cb.val == 31);
		TS_ASSERT (cb.val_size == 4);
	}

// unsubscribe to the event

	void test_unsubscribe_event(void)
	{
		device->unsubscribe_event(eve_id);
	}

};
#undef cout
#endif // McastLocalTestSuite_h

