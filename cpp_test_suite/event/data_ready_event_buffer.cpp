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
	void push_event(Tango::DataReadyEventData*);
	
public:
	int cb_executed;
	int cb_err;
	int old_sec,old_usec;
	int delta_msec;
	
	int ctr;
};

void EventCallBack::push_event(Tango::DataReadyEventData* event_data)
{
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
		coutv << "EventCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << "\n";
		if (!event_data->err)
		{
			coutv << "Event data: Attribute data type = " << event_data->attr_data_type << ", User ctr = " << event_data->ctr << endl;
			ctr = event_data->ctr;
		}
		else
		{
			coutv << "Error send to callback" << endl;
			Tango::Except::print_error_stack(event_data->errors);
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
		string att_name("Long_attr");
		
//
// Check that the attribute is still not polled
//

		bool po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == false);

//
// subscribe to an attribute data ready event
//

		int eve_id;
		vector<string> filters;
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		cb.ctr = -1;
		
		eve_id = device->subscribe_event(att_name,Tango::DATA_READY_EVENT,1,filters);

//
// Send 10 attribute data ready events
//

		Tango::DevVarLongStringArray dvlsa;
		dvlsa.svalue.length(1);
		dvlsa.lvalue.length(1);
		dvlsa.lvalue[0] = 9;
		dvlsa.svalue[0] = CORBA::string_dup(att_name.c_str());
		DeviceData d_in;

		for (int i=0; i<10; i++)
		{
			dvlsa.lvalue[0] = dvlsa.lvalue[0] + 1;
			d_in << dvlsa;
			
			device->command_inout("PushDataReady",d_in);
			
#ifdef _TG_WINDOWS_
			Sleep((DWORD)200);
#else
			struct timespec to_wait,inter;
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 200000000;
			nanosleep(&to_wait,&inter);
#endif
		}
		
//
// Check that only the last event was kept
//

		device->get_events(eve_id, &cb);
		
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 1);
		assert (cb.ctr == 19);
		
		cout << "   CallBack executed only for the last arrived event --> OK" << endl;
		
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		cout << "   unsubscribe_event --> OK" << endl;

//
// Subscribe for attibute configuration events.
// Set-up the event buffers to keep only the last 5 received events
//

		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		cb.ctr = 0;		

		eve_id = device->subscribe_event(att_name,Tango::DATA_READY_EVENT,5,filters);

//
// Send 10 attribute configuration events
//

		for (int i=0; i<10; i++)
		{
			dvlsa.lvalue[0] = dvlsa.lvalue[0] + 1;
			d_in << dvlsa;
			
			device->command_inout("PushDataReady",d_in);
			
#ifdef _TG_WINDOWS_
			Sleep((DWORD)200);
#else
			struct timespec to_wait,inter;
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 200000000;
			nanosleep(&to_wait,&inter);
#endif
		}
		
//
// Check that only the last event was kept
//

		device->get_events(eve_id, &cb);
		
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 5);
		assert (cb.ctr == 29);
		
		cout << "   CallBack executed only for the last 5 arrived events --> OK" << endl;
		
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		cout << "   unsubscribe_event --> OK" << endl;

//
// Subscribe for attribute data ready events.
// Set-up the event buffers to keep all received events
//

		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		cb.ctr = 0;		

		eve_id = device->subscribe_event(att_name,Tango::DATA_READY_EVENT,ALL_EVENTS,filters);

//
// Send 10 attribute data ready events
//

		for (int i=0; i<10; i++)
		{
			dvlsa.lvalue[0] = dvlsa.lvalue[0] + 1;
			d_in << dvlsa;
			
			device->command_inout("PushDataReady",d_in);
			
#ifdef _TG_WINDOWS_
			Sleep((DWORD)200);
#else
			struct timespec to_wait,inter;
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 200000000;
			nanosleep(&to_wait,&inter);
#endif
		}
		
//
// Check that only the last event was kept
//

		device->get_events(eve_id, &cb);
		
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 10);
		assert (cb.ctr == 39);
		
		cout << "   CallBack executed for 10 arrived events --> OK" << endl;

//
// Check the data ready event data reading as a vector
//

		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
//
// Send 15 attribute data ready events
//

		for (int i=0; i<15; i++)
		{
			dvlsa.lvalue[0] = dvlsa.lvalue[0] + 1;
			d_in << dvlsa;
			
			device->command_inout("PushDataReady",d_in);
			
#ifdef _TG_WINDOWS_
			Sleep((DWORD)200);
#else
			struct timespec to_wait,inter;
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 200000000;
			nanosleep(&to_wait,&inter);
#endif
		}
		
//
// Check that all events were kept and can be read as a vector
//

		coutv << "event queue size = " << device->event_queue_size(eve_id) << endl;
		assert (device->event_queue_size(eve_id) == 15);
		
		DataReadyEventDataList event_list;
		device->get_events(eve_id, event_list);
		coutv << "number of events read = " << event_list.size() << endl;
		assert (event_list.size() == 15);
		
		int ref_val = 40;
		DataReadyEventDataList::iterator vpos;
		for (vpos=event_list.begin(); vpos!=event_list.end(); vpos++)
		{
			double local_ctr = (*vpos)->ctr;
			
			coutv << "local_ctr = " << local_ctr << endl;
			assert (local_ctr == ref_val);
			ref_val++;
		}
		cout << "   Data received for 15 arrived events --> OK" << endl;

//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		cout << "   unsubscribe_event --> OK" << endl;

//
// subscribe several times to an attribute data ready event
//

		int eve_id1,eve_id2;

		EventCallBack cb1;
		cb1.cb_executed = 0;
		cb1.cb_err = 0;
		cb1.old_sec = cb.old_usec = 0;
		cb1.ctr = -1;

		EventCallBack cb2;
		cb2.cb_executed = 0;
		cb2.cb_err = 0;
		cb2.old_sec = cb.old_usec = 0;
		cb2.ctr = -1;
		
		eve_id1 = device->subscribe_event(att_name,Tango::DATA_READY_EVENT,1,filters);
		eve_id2 = device->subscribe_event(att_name,Tango::DATA_READY_EVENT,1,filters);

//
// Send 10 attribute data ready events
//

		Tango::DevVarLongStringArray dvlsa1;
		dvlsa1.svalue.length(1);
		dvlsa1.lvalue.length(1);
		dvlsa1.lvalue[0] = 9;
		dvlsa1.svalue[0] = CORBA::string_dup(att_name.c_str());
		DeviceData d_in1;

		for (int i=0; i<10; i++)
		{
			dvlsa1.lvalue[0] = dvlsa1.lvalue[0] + 1;
			d_in1 << dvlsa1;
			
			device->command_inout("PushDataReady",d_in);
			
#ifdef _TG_WINDOWS_
			Sleep((DWORD)200);
#else
			struct timespec to_wait,inter;
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 200000000;
			nanosleep(&to_wait,&inter);
#endif
		}
		
//
// Check that only the last event was kept
//

		device->get_events(eve_id1, &cb1);
		device->get_events(eve_id2, &cb2);
		
		coutv << "cb excuted = " << cb1.cb_executed << endl;
		assert (cb1.cb_executed == 1);
		assert (cb1.ctr == 54);
		assert (cb2.cb_executed == 1);
		assert (cb2.ctr == 54);
		
		cout << "   Several CallBack executed only for the last arrived event --> OK" << endl;
		
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id1);
		device->unsubscribe_event(eve_id2);
		cout << "   unsubscribe_events --> OK" << endl;
	
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
	
	return 0;
}
