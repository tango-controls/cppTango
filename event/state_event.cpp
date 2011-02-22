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
	DevState sta;
	string status;
};

void EventCallBack::push_event(Tango::EventData* event_data)
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
		coutv << "StateEventCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << "\n";
		if (!event_data->err)
		{
			string::size_type pos = event_data->attr_name.find_last_of('/');
			pos++;
			string att_name = event_data->attr_name.substr(pos);
			
			if (att_name == "state")
			{
				*(event_data->attr_value) >> sta;
				coutv << "State in callback: " << DevStateName[sta] << endl;
			}
			else if (att_name == "status")
			{
				*(event_data->attr_value) >> status;
				coutv << "Status in callback: " << status << endl;
			}
			else
				cout << "Received callback for an unknown attribute : " << event_data->attr_name << endl;
		}
		else
		{
			coutv << "Error send to callback" << endl;
		}
	}
	catch (...)
	{
		cb_err++;
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
		string att_name("State");
				
//
// Test set up (stop polling)
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
		
		DeviceProxy adm_dev(device->adm_name().c_str());
		DeviceData di;
		di << device_name;
		adm_dev.command_inout("DevRestart",di);
		
		delete device;
		device = new DeviceProxy(device_name);
		Tango_sleep(1);

//
// subscribe to a state change event
//

		int eve_id;
		vector<string> filters;
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
		// start the polling first!
		device->poll_attribute(att_name,1000);
		eve_id = device->subscribe_event(att_name,Tango::CHANGE_EVENT,&cb,filters);

//
// Check that the attribute is now polled at 1000 mS
//

		bool po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == true);

		po = device->is_command_polled(att_name);
		assert( po == true);
				
		int poll_period = device->get_attribute_poll_period(att_name);
		coutv << "att polling period : " << poll_period << endl;
		assert( poll_period == 1000);		
				
		cout << "   subscribe_event on state attribute --> OK" << endl;

//
// Check that first point has been received
//

		assert (cb.cb_executed == 1);
		assert (cb.sta == Tango::ON);
		assert (cb.cb_err == 0);
		cout << "   first point received --> OK" << endl;

		usleep(1000000);
//
// Change the device state and check that an event has been received
//

		DeviceData dd;
		DevState d_state = Tango::OFF;
		dd << d_state;
		device->command_inout("IOState",dd);
		
		usleep(1000000);
		
		d_state = Tango::ON;
		dd << d_state;
		device->command_inout("IOState",dd);
		
		usleep(1500000);
		
		assert (cb.cb_executed == 3);
		assert (cb.cb_err == 0);
		assert (cb.sta == Tango::ON);
		
		cout << "   Event on state change --> OK" << endl;
				
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
//
// Stop polling
//		
		device->stop_poll_attribute(att_name);
		
		cout << "   unsubscribe_event --> OK" << endl;
		
//
// Clear data in callback object and subscribe to status event
//

		cb.cb_executed = 0;		
		att_name = "status";
		
		// start the polling first!
		device->poll_attribute(att_name,1000);
		eve_id = device->subscribe_event(att_name,Tango::CHANGE_EVENT,&cb,filters);
		cout << "   subscribe_event on status attribute --> OK" << endl;

//
// Check that first point has been received
//

		assert (cb.cb_executed == 1);
		assert (cb.cb_err == 0);
		cout << "   first point received --> OK" << endl;

		usleep(1000000);

//
// Change the device state and check that an event has been received
// Changing the device state obviously change the device status
//

		d_state = Tango::OFF;
		dd << d_state;
		device->command_inout("IOState",dd);
		
		usleep(1000000);
		
		d_state = Tango::ON;
		dd << d_state;
		device->command_inout("IOState",dd);
		
		usleep(1500000);

		string::size_type pos = cb.status.find("ON");		
		assert (cb.cb_executed == 3);
		assert (pos != string::npos);
		assert (cb.cb_err == 0);
		
		cout << "   Event on status change --> OK" << endl;
		
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
	
Tango::ApiUtil::cleanup();
	
	return 0;
}
