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

class SlowCallBack : public Tango::CallBack
{
	void push_event(Tango::EventData*);
	
public:
	int cb_executed;
	int cb_err;
	int old_sec,old_usec;
	int delta_msec;
	short val;
	AttrQuality qua;
};

class FastCallBack : public Tango::CallBack
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

void SlowCallBack::push_event(Tango::EventData* event_data)
{
	vector<short> value;
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
		coutv << "SlowCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << "\n";
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
//			Tango::Except::print_error_stack(event_data->errors);
		}
	}
	catch (...)
	{
		coutv << "SlowCallBack::push_event(): could not extract data !\n";
	}

}

void FastCallBack::push_event(Tango::EventData* event_data)
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
		coutv << "FastCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << "\n";
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
//			Tango::Except::print_error_stack(event_data->errors);
		}
	}
	catch (...)
	{
		coutv << "FastCallBack::push_event(): could not extract data !\n";
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

		string att_name("slow_actuator");
				
//
// Test set up (stop polling)
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
					
//
// subscribe to a quality event
//

		int eve_id;
		vector<string> filters;
		SlowCallBack cb;
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
				
		cout << "   (Slow actuator) subscribe_event --> OK" << endl;

//
// Check that first point has been received
//

		assert (cb.cb_executed == 1);
		assert (cb.qua == ATTR_VALID);
		cout << "   (Slow actuator) first point received --> OK" << endl;


//
// Write a value into the actuator
//

		short new_val = 10;
		DeviceAttribute da(att_name,new_val);
		
		device->write_attribute(da);

#ifndef WIN32		
		int rest = sleep(1);
		if (rest != 0)
			sleep(1);
#else
		Sleep(1000);
#endif

//
// Check that the event has been received
//

		assert (cb.cb_executed == 2);
		assert (cb.qua == ATTR_CHANGING);
		cout << "   (Slow actuator) Event quality from VALID->CHANGING --> OK" << endl;

//
// Read value from device which should have quality factor still set to
// CHANGING
//

		DeviceAttribute da_r;
		device->set_source(DEV);
		da_r = device->read_attribute(att_name);
		device->set_source(CACHE_DEV);
		
		assert (da_r.get_quality() == ATTR_CHANGING);
		cout << "   (Slow actuator) Attribute quality still CHANGING after read --> OK" << endl;
		
//
// Wait for end of movement
//

#ifndef WIN32		
		rest = sleep(3);
		if (rest != 0)
			sleep(3);
#else
		Sleep(3000);
#endif

//
// Check that the event has been received
//

		assert (cb.cb_executed == 3);
		assert (cb.qua == ATTR_VALID);
		cout << "   (Slow actuator) event quality from CHANGING->VALID --> OK" << endl;
												
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
		cout << "   (Slow actuator) unsubscribe_event --> OK" << endl;
		
//---------------------------------------------------------------------------------
//
//			Now, check a fast actuator scan
//
//---------------------------------------------------------------------------------

		att_name = "fast_actuator";
						
//
// Test set up (stop polling)
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
					
//
// subscribe to a quality event
//

		FastCallBack fcb;
		fcb.cb_executed = 0;
		fcb.cb_err = 0;
		fcb.old_sec = cb.old_usec = 0;
		
		eve_id = device->subscribe_event(att_name,Tango::QUALITY_EVENT,&fcb,filters);

//
// Check that the attribute is now polled at 1000 mS
//

		po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == true);
		
		poll_period = device->get_attribute_poll_period(att_name);
		coutv << "att polling period : " << poll_period << endl;
		assert( poll_period == 1000);		
				
		cout << "   (Fast actuator) subscribe_event --> OK" << endl;

//
// Check that first point has been received
//

		assert (fcb.cb_executed == 1);
		assert (fcb.qua == ATTR_VALID);
		cout << "   (Fast actuator) first point received --> OK" << endl;


//
// Write a value into the actuator
//

		double val = 100.333;
		DeviceAttribute fda(att_name,val);
		
		device->write_attribute(fda);

#ifndef WIN32		
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif

//
// Check that the events has been received
//

		assert (cb.cb_executed == 3);
		assert (cb.qua == ATTR_VALID);
		cout << "   (Fast actuator) Two quality events received --> OK" << endl;
												
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
		cout << "   (Fast actuator) Unsubscribe_event --> OK" << endl;
		
		device->stop_poll_attribute(att_name);
		device->stop_poll_attribute("slow_actuator");
	
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
