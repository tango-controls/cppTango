/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#ifdef WIN32
#include <sys/timeb.h>
#include <process.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#define	coutv	if (verbose == true) cout

using namespace Tango;

bool verbose = false;

class EventCallBack : public Tango::CallBack
{
	void push_event(Tango::EventData*);
	
public:
	int cb_executed;
	int cb_err;
	int old_sec,old_usec;
	int delta_msec;
	long val;
	long val_size;
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
	coutv << "date : tv_sec = " << now_timeval.tv_sec;
	coutv << ", tv_usec = " << now_timeval.tv_usec << endl;
	
	int delta_s = now_timeval.tv_sec - old_sec;
	if (delta_s == 0)
		delta_msec = (now_timeval.tv_usec - old_usec) / 1000;
	else
	{
		delta_msec = (now_timeval.tv_usec + (1000000 - old_usec)) / 1000;
		if (delta_s > 1)
			delta_msec = delta_msec + ((delta_s - 1)* 1000);
	}
	old_sec = now_timeval.tv_sec;
	old_usec = now_timeval.tv_usec;
	
	coutv << "delta_msec = " << delta_msec << endl;

	cb_executed++;
	try
	{
		coutv << "StateEventCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << "\n";
		if (!event_data->err)
		{

			*(event_data->attr_value) >> value;
			coutv << "CallBack value size " << value.size() << endl;
			val = value[2];
			val_size = value.size();
			coutv << "Callback value " << val << endl;
		}
		else
		{
			coutv << "Error send to callback" << endl;
			if (strcmp(event_data->errors[0].reason.in(),"bbb") == 0)
				cb_err++;
		}
	}
	catch (...)
	{
		coutv << "EventCallBack::push_event(): could not extract data !\n";
	}

}

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc == 1)
	{
		cout << "usage: %s device [-v]" << endl;
		exit(-1);
	}

	string device_name = argv[1];

	if (argc == 3)
	{
		if (strcmp(argv[2],"-v") == 0)
			verbose = true;
	}
	
	try 
	{
		device = new DeviceProxy(device_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(1);
	}

	coutv << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;

	
	try
	{
		string att_name("event_change_tst");
		
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

//
// switch on the polling first!
//
		device->poll_attribute(att_name,250);

//
// Check that the attribute is now polled at 250 mS
//

		bool po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == true);
		
		int poll_period = device->get_attribute_poll_period(att_name);
		coutv << "att polling period : " << poll_period << endl;
		assert( poll_period == 250);
		

//
// Subscribe for change events.
// Set-up the event buffers to keep only the last received event
//
		int eve_id;
		vector<string> filters;
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;


		eve_id = device->subscribe_event(att_name,Tango::CHANGE_EVENT,1,filters);
		cout << "   subscribe_event --> OK" << endl;
//
// Send 10 change events
//
		for (int i=0; i<10; i++)
		{
			device->command_inout("IOIncValue");
#ifdef _TG_WINDOWS_
			Sleep((DWORD)400);
#else
			struct timespec to_wait,inter;
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 400000000;
			nanosleep(&to_wait,&inter);
#endif
		}
//
// Check that only the last event was kept
//
		device->get_events(eve_id, &cb);
		
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 1);
		assert (cb.val == 40);
		cout << "   CallBack executed only for the last arrived event --> OK" << endl;
//
// unsubscribe the event
//
		device->unsubscribe_event(eve_id);
		cout << "   unsubscribe_event --> OK" << endl;
				

//
// Subscribe for change events.
// Set-up the event buffers to keep only the last 5 received events
//
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
		eve_id = device->subscribe_event(att_name,Tango::CHANGE_EVENT,5,filters);
//
// Send 10 change events
//
		for (int i=0; i<10; i++)
		{
			device->command_inout("IOIncValue");
#ifdef _TG_WINDOWS_
			Sleep((DWORD)400);
#else
			struct timespec to_wait,inter;
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 400000000;
			nanosleep(&to_wait,&inter);
#endif
		}
//
// Check that only the last 5 events were kept
//
		device->get_events(eve_id, &cb);
		
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 5);
		assert (cb.val == 50);
		cout << "   CallBack executed only for the last 5 arrived events --> OK" << endl;
//
// unsubscribe the event
//
		device->unsubscribe_event(eve_id);
		cout << "   unsubscribe_event --> OK" << endl;


//
// Subscribe for change events.
// Set-up the event buffers to keep all received events
//
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
		eve_id = device->subscribe_event(att_name,Tango::CHANGE_EVENT,ALL_EVENTS,filters);
//
// Send 10 change events
//
		for (int i=0; i<10; i++)
		{
			device->command_inout("IOIncValue");
#ifdef _TG_WINDOWS_
			Sleep((DWORD)400);
#else
			struct timespec to_wait,inter;
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 400000000;
			nanosleep(&to_wait,&inter);
#endif
		}
//
// Check that all events were kept
//
		device->get_events(eve_id, &cb);
		
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 11); // 1 synchrounous event at subscription +
		                               // 10 value changes
		assert (cb.val == 60);
		cout << "   CallBack executed for 11 arrived events --> OK" << endl;



//
// Check the event data reading as a vector
//
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
//
// Send 15 change events
//
		for (int i=0; i<15; i++)
		{
			device->command_inout("IOIncValue");
#ifdef _TG_WINDOWS_
			Sleep((DWORD)400);
#else
			struct timespec to_wait,inter;
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 400000000;
			nanosleep(&to_wait,&inter);
#endif
		}
//
// Check that all events were kept and can be read as a vector
//
		coutv << "event queue size = " << device->event_queue_size(eve_id) << endl;
		assert (device->event_queue_size(eve_id) == 15);
		
		EventDataList event_list;
		device->get_events(eve_id, event_list);
		coutv << "number of events read = " << event_list.size() << endl;
		assert (event_list.size() == 15);
		
		long ref_val = 61;
		EventDataList::iterator vpos;
		for (vpos=event_list.begin(); vpos!=event_list.end(); vpos++)
		{
			vector<DevLong> value;
			*((*vpos)->attr_value) >> value;
			
			coutv << "event value = " << value[2] << endl;
			assert (value[2] == ref_val);
			ref_val++;
		}
		cout << "   Data received for 15 arrived events --> OK" << endl;

//
// unsubscribe the event
//
		device->unsubscribe_event(eve_id);
		cout << "   unsubscribe_event --> OK" << endl;


//
// Subscribe for several change events.
// Set-up the event buffers to keep only the last 5 received events
//

		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
		eve_id = device->subscribe_event(att_name,Tango::CHANGE_EVENT,5,filters);
		int eve_id1 = device->subscribe_event(att_name,Tango::CHANGE_EVENT,5,filters);
		int eve_id2 = device->subscribe_event(att_name,Tango::CHANGE_EVENT,5,filters);
//
// Send 10 change events
//
		for (int i=0; i<10; i++)
		{
			device->command_inout("IOIncValue");
#ifdef _TG_WINDOWS_
			Sleep((DWORD)400);
#else
			struct timespec to_wait,inter;
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 400000000;
			nanosleep(&to_wait,&inter);
#endif
		}
//
// Check that only the last 5 events were kept
//
		device->get_events(eve_id, &cb);
		
		assert (cb.cb_executed == 5);
		assert (cb.val == 85);

		device->get_events(eve_id1, &cb);
		
		assert (cb.cb_executed == 10);
		assert (cb.val == 85);

		device->get_events(eve_id2, &cb);
		
		assert (cb.cb_executed == 15);
		assert (cb.val == 85);

		cout << "   Three CallBacks executed only for the last 5 arrived events --> OK" << endl;
//
// unsubscribe the event
//
		device->unsubscribe_event(eve_id);
		device->unsubscribe_event(eve_id1);
		device->unsubscribe_event(eve_id2);

		cout << "   unsubscribe_events --> OK" << endl;
//
// Stop polling
//

		device->stop_poll_attribute(att_name);
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	catch (CORBA::Exception &ex)
	{
		Except::print_exception(ex);
		exit(-1);
	}

	delete device;

	Tango::ApiUtil::cleanup();
	
	return 0;

}
