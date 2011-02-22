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
// subscribe to a attribute config event
//

		int eve_id1,eve_id2;
		vector<string> filters;
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
		eve_id1 = device->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,&cb,filters);
		eve_id2 = device->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,&cb,filters);

//
// Check that the attribute is still not polled
//

		bool po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == false);	

//
// The callback should have been executed once
//

		assert (cb.cb_executed == 2);
						
		cout << "   subscribe_event --> OK" << endl;
		
//
// Execute the command which will fire an attribute
// config event
//

		Tango::DevVarDoubleArray dvda(2);
		dvda.length(2);
		dvda[0] = 0.0;
		dvda[1] = 0.0;
		DeviceData d_in;
		d_in << dvda;
		device->command_inout("IOSetWAttrLimit",d_in);
		
		dvda[0] = 1.0;
		dvda[1] = 10.0;
		d_in << dvda;
		device->command_inout("IOSetWAttrLimit",d_in);
				
		Tango_sleep(1);
		
		assert (cb.cb_executed == 6);
		assert (cb.min_value == "0");
		assert (cb.max_value == "10");
		
		cout << "   attr_conf_event --> OK" << endl;
				
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id1);
		device->unsubscribe_event(eve_id2);
		
		cout << "   unsubscribe_event --> OK" << endl;							
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
