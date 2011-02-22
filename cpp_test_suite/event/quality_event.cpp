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
	double val;
	AttrQuality qua;
};

void EventCallBack::push_event(Tango::EventData* event_data)
{
	vector<double> value;
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
		qua = event_data->attr_value->get_quality();
		if ((!event_data->err) && (qua != ATTR_INVALID))
		{

			*(event_data->attr_value) >> value;
			val = value[0];
			coutv << "Callback value " << val << endl;
		}
		else
		{
			coutv << "Error send to callback" << endl;
			coutv << "Attribute quality = " << qua << endl;
//			Tango::Except::print_error_stack(event_data->errors);
			if (event_data->err == true)
			{
				if (strcmp(event_data->errors[0].reason.in(),"ccc") == 0)
					cb_err++;
			}
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

		string att_name("event_quality_tst");
				
//
// Test set up (stop polling and clear abs_change and rel_change attribute
// properties but restart device to take this into account)
// Set attribute quality to VALID
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
		DeviceData di;
		short in = 0;
		di << in;		
		device->command_inout("IOChangeQuality",di);

		vector<short> data_in(2);
		data_in[0] = 2;
		data_in[1] = 0;
		di << data_in;
		device->command_inout("IOAttrThrowEx",di);					

//
// subscribe to a quality event
//

		int eve_id;
		vector<string> filters;
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
		eve_id = device->subscribe_event(att_name,Tango::QUALITY_EVENT,&cb,filters);

//
// Check that the attribute is now polled at 1000 mS
//

		bool po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == true);
		
		int poll_period = device->get_attribute_poll_period(att_name);
		coutv << "att polling period : " << poll_period << endl;
		assert( poll_period == 1000);		
				
		cout << "   subscribe_event --> OK" << endl;

//
// Check that first point has been received
//

		assert (cb.cb_executed == 1);
		assert (cb.val == 1.2);
		assert (cb.qua == ATTR_VALID);
		cout << "   first point received --> OK" << endl;
		
//
// Check that callback was called after a positive value change 
//

// A trick for gdb. The thread created by omniORB for the callback execution
// is just started during the sleep. Gdb has a breakpoint reached at each thread
// creation to display message on the console. This breakpoint is a software
// signal which interrupts the sleep.....
//

#ifndef WIN32		
		int rest = sleep(1);
		if (rest != 0)
			sleep(1);
#else
		Sleep(1000);
#endif

		in = 1;
		di << in;		
		device->command_inout("IOChangeQuality",di);

#ifndef WIN32
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif
						
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 2);
		assert (cb.val == 1.2);
		assert (cb.qua == ATTR_INVALID);
				
		cout << "   CallBack executed for quality change (VALID->INVALID) --> OK" << endl;

//
// Check that callback was called after a negative value change 
//

		in = 0;
		di << in;
		device->command_inout("IOChangeQuality",di);

#ifndef WIN32		
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif
			
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 3);
		assert (cb.val == 1.2);
		assert (cb.qua == ATTR_VALID);
				
		cout << "   CallBack executed for quality change (INVALID->VALID) --> OK" << endl;
								
//
// Force the attribute to throw exception
//

		data_in[0] = 2;
		data_in[1] = 1;
		di << data_in;
		
		device->command_inout("IOAttrThrowEx",di);
		
//
// Check that callback was called
//
		
		Tango_sleep(4);
		coutv << "Callback cb_err = " << cb.cb_err << endl;
		assert (cb.cb_err > 2);
		assert (cb.cb_err < 5);
		assert (cb.delta_msec > 800);
		assert (cb.delta_msec < 1200);
				
		cout << "   CallBack executed when attribute throw exception every 1000 mS --> OK" << endl;		

//
// Attribute does not send exception any more
//

		data_in[1] = 0;
		di << data_in;
		
		device->command_inout("IOAttrThrowEx",di);
						
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
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
