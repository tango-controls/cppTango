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
	void push_event(Tango::AttrConfEventData*);
	
public:
	int cb_executed;
	int cb_err;
	int old_sec,old_usec;
	int delta_msec;
	
	string min_value;
	string max_value;
};

void EventCallBack::push_event(Tango::AttrConfEventData* event_data)
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
			coutv << *(event_data->attr_conf) << endl;
			min_value = event_data->attr_conf->min_value;
			max_value = event_data->attr_conf->max_value;
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
		string att_name("Double_attr_w");
		
//
// Check that the attribute is still not polled
//

		bool po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == false);


//
// subscribe to an attribute config event
//
		int eve_id;
		vector<string> filters;
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
		eve_id = device->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,1,filters);

//
// Send 10 attribute configuration events
//
		Tango::DevVarDoubleArray dvda(2);
		dvda.length(2);
		dvda[0] = 1.0;
		dvda[1] = 10.0;

		for (int i=0; i<10; i++)
		{
			dvda[1] = dvda[1] + 1;
			DeviceData d_in;
			d_in << dvda;
			
			device->command_inout("IOSetWAttrLimit",d_in);
			
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
		assert (cb.min_value == "0");
		assert (cb.max_value == "20");
		
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

		eve_id = device->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,5,filters);

//
// Send 10 attribute configuration events
//
		for (int i=0; i<10; i++)
		{
			dvda[1] = dvda[1] + 1;
			DeviceData d_in;
			d_in << dvda;
			
			device->command_inout("IOSetWAttrLimit",d_in);
			
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
		assert (cb.min_value == "0");
		assert (cb.max_value == "30");
		
		cout << "   CallBack executed only for the last 5 arrived events --> OK" << endl;
//
// unsubscribe to the event
//
		device->unsubscribe_event(eve_id);
		cout << "   unsubscribe_event --> OK" << endl;

//
// Subscribe for attibute configuration events.
// Set-up the event buffers to keep all received events
//
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;		

		eve_id = device->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,ALL_EVENTS,filters);

//
// Send 10 attribute configuration events
//
		for (int i=0; i<10; i++)
		{
			dvda[1] = dvda[1] + 1;
			DeviceData d_in;
			d_in << dvda;
			
			device->command_inout("IOSetWAttrLimit",d_in);
			
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
		assert (cb.cb_executed == 11);
		assert (cb.min_value == "0");
		assert (cb.max_value == "40");
		
		cout << "   CallBack executed for 11 arrived events --> OK" << endl;

//
// Check the attribute config event data reading as a vector
//
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
//
// Send 15 attribute configuration events
//
		for (int i=0; i<15; i++)
		{
			dvda[1] = dvda[1] + 1;
			DeviceData d_in;
			d_in << dvda;
			
			device->command_inout("IOSetWAttrLimit",d_in);
			
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
		
		AttrConfEventDataList event_list;
		device->get_events(eve_id, event_list);
		coutv << "number of events read = " << event_list.size() << endl;
		assert (event_list.size() == 15);
		
		double ref_val = 41;
		AttrConfEventDataList::iterator vpos;
		for (vpos=event_list.begin(); vpos!=event_list.end(); vpos++)
		{
			double min_value = atof ( ((*vpos)->attr_conf->min_value).c_str());
			double max_value = atof ( ((*vpos)->attr_conf->max_value).c_str());
			
			coutv << "min value = " << min_value << endl;
			coutv << "max value = " << max_value << endl;
			assert (max_value == ref_val);
			ref_val++;
		}
		cout << "   Data received for 15 arrived events --> OK" << endl;

//
// unsubscribe to the event
//
		device->unsubscribe_event(eve_id);
		cout << "   unsubscribe_event --> OK" << endl;

//
// subscribe to an attribute config event several times
//

		int eve_id1,eve_id2,eve_id3;

		EventCallBack cb1;
		cb1.cb_executed = 0;
		cb1.cb_err = 0;
		cb1.old_sec = cb.old_usec = 0;

		EventCallBack cb2;
		cb2.cb_executed = 0;
		cb2.cb_err = 0;
		cb2.old_sec = cb.old_usec = 0;

		EventCallBack cb3;
		cb3.cb_executed = 0;
		cb3.cb_err = 0;
		cb3.old_sec = cb.old_usec = 0;
		
		eve_id1 = device->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,ALL_EVENTS,filters);
		eve_id2 = device->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,ALL_EVENTS,filters);
		eve_id3 = device->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,ALL_EVENTS,filters);

//
// Send 15 attribute configuration events
//

		for (int i=0; i<15; i++)
		{
			dvda[1] = dvda[1] + 1;
			DeviceData d_in;
			d_in << dvda;
			
			device->command_inout("IOSetWAttrLimit",d_in);
			
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
		coutv << "event queue size = " << device->event_queue_size(eve_id1) << endl;
		assert (device->event_queue_size(eve_id1) == 16);
		assert (device->event_queue_size(eve_id2) == 16);
		assert (device->event_queue_size(eve_id3) == 16);
		
		AttrConfEventDataList event_list1;
		device->get_events(eve_id1, event_list1);
		coutv << "number of events read = " << event_list1.size() << endl;
		assert (event_list1.size() == 16);

		AttrConfEventDataList event_list2;
		device->get_events(eve_id2, event_list2);
		coutv << "number of events read = " << event_list2.size() << endl;
		assert (event_list2.size() == 16);

		AttrConfEventDataList event_list3;
		device->get_events(eve_id3, event_list3);
		coutv << "number of events read = " << event_list3.size() << endl;
		assert (event_list3.size() == 16);
		
		ref_val = 55;
		for (vpos=event_list1.begin(); vpos!=event_list1.end(); vpos++)
		{
			double min_value = atof ( ((*vpos)->attr_conf->min_value).c_str());
			double max_value = atof ( ((*vpos)->attr_conf->max_value).c_str());
			
			coutv << "min value = " << min_value << endl;
			coutv << "max value = " << max_value << endl;
			assert (max_value == ref_val);
			ref_val++;
		}

		ref_val = 55;
		for (vpos=event_list2.begin(); vpos!=event_list2.end(); vpos++)
		{
			double min_value = atof ( ((*vpos)->attr_conf->min_value).c_str());
			double max_value = atof ( ((*vpos)->attr_conf->max_value).c_str());
			
			coutv << "min value = " << min_value << endl;
			coutv << "max value = " << max_value << endl;
			assert (max_value == ref_val);
			ref_val++;
		}

		ref_val = 55;
		for (vpos=event_list3.begin(); vpos!=event_list3.end(); vpos++)
		{
			double min_value = atof ( ((*vpos)->attr_conf->min_value).c_str());
			double max_value = atof ( ((*vpos)->attr_conf->max_value).c_str());
			
			coutv << "min value = " << min_value << endl;
			coutv << "max value = " << max_value << endl;
			assert (max_value == ref_val);
			ref_val++;
		}

		cout << "   Data received for 15 arrived events (3 subscribe) --> OK" << endl;

//
// unsubscribe to the events
//

		device->unsubscribe_event(eve_id1);
		device->unsubscribe_event(eve_id2);
		device->unsubscribe_event(eve_id3);

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
