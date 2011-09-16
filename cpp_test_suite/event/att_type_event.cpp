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
void check_attribute_data_type(DeviceProxy *,string &, const char *);

class EventCallBack : public Tango::CallBack
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
	short value;
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
			coutv << "CallBack value " << value << endl;
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


//*************** DevShort data type ***********************

		string att_name("Short_attr");
		check_attribute_data_type(device,att_name, "DevShort");
		
//*************** DevLong data type ***********************

		att_name = ("Long_attr");
		check_attribute_data_type(device,att_name, "DevLong");

//*************** DevFloat data type ***********************

		att_name = ("Float_attr");
		check_attribute_data_type(device,att_name, "DevFloat");

//*************** DevDouble data type ***********************

		att_name = ("Double_attr");
		check_attribute_data_type(device,att_name, "DevDouble");

//*************** DevString data type ***********************

		att_name = ("String_attr");
		check_attribute_data_type(device,att_name, "DevString");

//*************** DevLong64 data type ***********************

		att_name = ("Long64_attr_rw");
		check_attribute_data_type(device,att_name, "DevLong64");

//*************** DevULong data type ***********************

		att_name = ("ULong_attr_rw");
		check_attribute_data_type(device,att_name, "DevULong");

//*************** DevULong64 data type ***********************

		att_name = ("ULong64_attr_rw");
		check_attribute_data_type(device,att_name, "DevULong64");

//*************** DevState data type ***********************

		att_name = ("State_attr_rw");
		check_attribute_data_type(device,att_name, "DevState");

//*************** DevBoolean data type ***********************

		att_name = ("Boolean_attr");
		check_attribute_data_type(device,att_name, "DevBoolean");

//*************** DevUShort data type ***********************

		att_name = ("UShort_attr");
		check_attribute_data_type(device,att_name, "DevUShort");

//*************** DevUChar data type ***********************

		att_name = ("UChar_attr");
		check_attribute_data_type(device,att_name, "DevUChar");

//*************** Exception data type ***********************

		att_name = ("attr_wrong_type");
		check_attribute_data_type(device,att_name, "Error");							
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

void check_attribute_data_type(DeviceProxy *device,string &att_name, const char *data_type)
{

//
// subscribe to a periodic event
//

	int eve_id;
	vector<string> filters;
	EventCallBack cb;
	cb.cb_executed = 0;
	cb.cb_err = 0;
	cb.old_sec = cb.old_usec = 0;
		
// start the polling first!

	device->poll_attribute(att_name,500);
		
	eve_id = device->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb,filters);

//
// Check that the attribute is now polled at 500 mS
//

	bool po = device->is_attribute_polled(att_name);
	coutv << "attribute polled : " << po << endl;
	assert( po == true);
		
	int poll_period = device->get_attribute_poll_period(att_name);
	coutv << "att polling period : " << poll_period << endl;
	assert( poll_period == 500);		
		
//
// Check that callback was called
//

// A trick for gdb. The thread created by omniORB for the callback execution
// is just started during the sleep. Gdb has a breakpoint reached at each thread
// creation to display message on the console. This breakpoint is a software
// signal which interrupts the sleep.....
//

#ifndef WIN32
	int rest = sleep(2);
	if (rest != 0)
	sleep(2);
#else
	Sleep(2000);
#endif
			
	coutv << "cb excuted = " << cb.cb_executed << endl;
	assert (cb.cb_executed != 0);
				
	cout << "   Event received for " << data_type << " attribute --> OK" << endl;

//
// unsubscribe to the event and stop polling
//

	device->unsubscribe_event(eve_id);
	device->stop_poll_attribute(att_name);
}

