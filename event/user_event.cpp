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
};

class EventEncodedCallBack : public Tango::CallBack
{
	void push_event(Tango::EventData*);
	
public:
	int cb_executed;
	int cb_err;
	int old_sec,old_usec;
	int delta_msec;
};

void EventCallBack::push_event(Tango::EventData* event_data)
{
	vector<long> value;
//	Tango::DevState sta;
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
//			*(event_data->attr_value) >> sta;
			coutv << "CallBack vector value size : " << value.size() << endl;
//			coutv << "CallBack vector value size : " << sta << endl;
		}
		else
		{
			coutv << "Error send to callback" << endl;
//			Tango::Except::print_error_stack(event_data->errors);
			if (strcmp(event_data->errors[0].reason.in(),"aaa") == 0)
				cb_err++;
		}
	}
	catch (...)
	{
		coutv << "EventCallBack::push_event(): could not extract data !\n";
	}

}

void EventEncodedCallBack::push_event(Tango::EventData* event_data)
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

	cb_executed++;
	try
	{
		coutv << "EventEncodedCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << "\n";
		if (!event_data->err)
		{
			coutv << "Valid data received" << endl;
		}
		else
		{
			coutv << "Error send to callback" << endl;
			if (strcmp(event_data->errors[0].reason.in(),"aaa") == 0)
				cb_err++;
		}
	}
	catch (...)
	{
		coutv << "EventEncodedCallBack::push_event(): could not extract data !\n";
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
// Test set up (stop polling of this attribute but start to poll another
// attribute)
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
		DeviceAttribute dap = device->read_attribute("String_attr");

//
// subscribe to a user event
//

		int eve_id;
		vector<string> filters;
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
		eve_id = device->subscribe_event(att_name,Tango::USER_EVENT,&cb,filters);

//
// Check that the attribute is still not polled
//

		bool po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == false);	

//
// The callback should have been executed once
//

		assert (cb.cb_executed == 1);
						
		cout << "   subscribe_event --> OK" << endl;
		
//
// Fire the event
//

		device->command_inout("IOPushEvent");
		device->command_inout("IOPushEvent");
		device->command_inout("IOPushEvent");
		
		Tango_sleep(1);
		
		assert (cb.cb_executed == 4);
		cout << "   user_event --> OK" << endl;
				
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
		cout << "   unsubscribe_event --> OK" << endl;

#ifndef COMPAT		
//
// subscribe to a user event for an attribute of the DevEncoded data type
//

		att_name = "encoded_attr";
		EventEncodedCallBack enc_cb;
		enc_cb.cb_executed = 0;
		enc_cb.cb_err = 0;
		enc_cb.old_sec = cb.old_usec = 0;
		
		eve_id = device->subscribe_event(att_name,Tango::USER_EVENT,&enc_cb,filters);	

//
// The callback should have been executed once
//

		assert (enc_cb.cb_executed == 1);
						
		cout << "   subscribe_event (DevEncoded data type) --> OK" << endl;
		
//
// Fire the event
//

		device->command_inout("IOPushDevEncodedEvent");
		device->command_inout("IOPushDevEncodedEvent");
		device->command_inout("IOPushDevEncodedEvent");
		
		Tango_sleep(1);
		
		assert (enc_cb.cb_executed == 4);
		cout << "   user_event (DevEncoded data type) --> OK" << endl;
				
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
		cout << "   unsubscribe_event (DevEncoded data type) --> OK" << endl;
#endif						
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
