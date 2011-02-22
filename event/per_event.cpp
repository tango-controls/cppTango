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

		string att_name("short_attr");
				
//
// Test set up (stop polling and clear event_period attribute property but
// restart device to take this into account)
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
		DbAttribute dba(att_name,device_name);
		DbData dbd;
		DbDatum a(att_name);
		a << (short)1;
		dbd.push_back(a);
		dbd.push_back(DbDatum("event_period"));
		
		dba.delete_property(dbd);
		DeviceProxy adm_dev(device->adm_name().c_str());
		DeviceData di;
		di << device_name;
		adm_dev.command_inout("DevRestart",di);
		
		delete device;
		device = new DeviceProxy(device_name);
		Tango_sleep(1);

//
// subscribe to a periodic event
//

		int eve_id;
		vector<string> filters;
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		eve_id = device->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb,filters);

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
// Check that callback was called
//

// A trick for gdb. The thread created by omniORB for the callback execution
// is just started during the sleep. Gdb has a breakpoint reached at each thread
// creation to display message on the console. This breakpoint is a software
// signal which interrupts the sleep.....
//

#ifndef WIN32
		int rest = sleep(4);
		if (rest != 0)
			sleep(4);
#else
		Sleep(4000);
#endif
			
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed > 2);
		assert (cb.cb_executed < 6);
		assert (cb.delta_msec > 900);
		assert (cb.delta_msec < 1100);
				
		cout << "   CallBack executed every 1000 mS --> OK" << endl;
		
//
// Force the attribute to throw exception
//

		vector<short> data_in(2);
		data_in[0] = 0;
		data_in[1] = 1;
		di << data_in;
		
		device->command_inout("IOAttrThrowEx",di);
		
//
// Check that callback was called
//
		
		Tango_sleep(3);
		coutv << "Callback cb_err = " << cb.cb_err << endl;
		assert (cb.cb_err > 2);
		assert (cb.cb_err < 5);
		assert (cb.delta_msec > 950);
		assert (cb.delta_msec < 1050);
				
		cout << "   CallBack executed when attribute throw exception every 1000 mS --> OK" << endl;
						
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
		cout << "   unsubscribe_event --> OK" << endl;
		
//
// Attribute does not send exception any more
//

		data_in[1] = 0;
		di << data_in;
		
		device->command_inout("IOAttrThrowEx",di);

//
// subscribe to a periodic event with a filter
//

		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		filters.push_back("$counter % 2 == 0");
		
		eve_id = device->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb,filters);
		cout << "   subscribe_event (with modulo filter) --> OK" << endl;

//
// Check that callback was called
//
		
		Tango_sleep(6);
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed > 2);
		assert (cb.cb_executed < 5);
		assert (cb.delta_msec > 1900);
		assert (cb.delta_msec < 2100);
				
		cout << "   CallBack executed every 2000 mS --> OK" << endl;
		
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
		cout << "   unsubscribe_event (with filter) --> OK" << endl;

//
// Change polling period to 200mS
//

		device->poll_attribute(att_name,200);
		
//
// subscribe to a periodic event
//

		cb.cb_executed = 0;
		cb.cb_err = 0;
		filters.clear();
				
		eve_id = device->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb,filters);

//
// Check that the attribute is still polled at 200 mS
//

		po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == true);
		
		poll_period = device->get_attribute_poll_period(att_name);
		coutv << "att polling period : " << poll_period << endl;
		assert( poll_period == 200);		
				
		cout << "   subscribe_event (with attribute already polled) --> OK" << endl;

//
// Check that callback was called
//
		
		Tango_sleep(3);
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed > 2);
		assert (cb.cb_executed < 5);
		assert (cb.delta_msec > 900);
		assert (cb.delta_msec < 1100);
				
		cout << "   CallBack executed every 1000 mS --> OK" << endl;
		
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
		cout << "   unsubscribe_event --> OK" << endl;
		
//
// Stop polling
//

		device->stop_poll_attribute(att_name);

//
// Change the periodic event "event_period" attribute property to
// 600 mS
//

		DbAttribute db_att(att_name,device_name);
		DbData db_dat;
		DbDatum dat(att_name);
		dat << (long)1;
		db_dat.push_back(dat);
		DbDatum dat1("event_period");
		dat1 << (long)600;
		db_dat.push_back(dat1);
		
		db_att.put_property(db_dat);

		
//
// Restart device
//

		adm_dev = device->adm_name().c_str();
		di << device_name;
		adm_dev.command_inout("DevRestart",di);
		
		delete device;
		device = new DeviceProxy(device_name);
		
		Tango_sleep(2);
		
//
// Poll device at 200 mS
//

		device->poll_attribute(att_name,200);

//
// subscribe to a periodic event
//

		cb.cb_executed = 0;
		cb.cb_err = 0;
		
		eve_id = device->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb,filters);

//
// Check that the attribute is  polled at 200 mS
//

		po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == true);
		
		poll_period = device->get_attribute_poll_period(att_name);
		coutv << "att polling period : " << poll_period << endl;
		assert( poll_period == 200);		
				
		cout << "   subscribe_event (with event_period property set) --> OK" << endl;

//
// Check that callback was called
//
		
		Tango_sleep(3);
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed >= 3);
		assert (cb.cb_executed < 8);
		assert (cb.delta_msec > 500);
		assert (cb.delta_msec < 700);
		
		cout << "   CallBack executed every 600 mS --> OK" << endl;
		
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
		cout << "   unsubscribe_event --> OK" << endl;	
		
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
	
	return 0;
}
