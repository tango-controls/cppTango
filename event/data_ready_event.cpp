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
	
	int user_ctr;
	
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
			user_ctr = event_data->ctr;
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
//
// Subscribe to an attribute which is not data ready event enabled should fail
//

		string att_name("Short_attr");

		int eve_id1,eve_id2;
		vector<string> filters;
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		cb.user_ctr = 0;

		bool except = true;
						
		try
		{
			eve_id1 = device->subscribe_event(att_name,Tango::DATA_READY_EVENT,&cb,filters);
			except = false;
		}
		catch (Tango::DevFailed &e)
		{
//			Tango::Except::print_exception(e);
		}
		
		assert (except == true);
				
		att_name = "Long_attr";
				

//
// subscribe to a data ready event
//
		
		eve_id1 = device->subscribe_event(att_name,Tango::DATA_READY_EVENT,&cb,filters);
		eve_id2 = device->subscribe_event(att_name,Tango::DATA_READY_EVENT,&cb,filters);

//
// Check that the attribute is still not polled
//

		bool po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == false);	

//
// The callback should not have been executed once
//

		assert (cb.cb_executed == 0);
						
		cout << "   subscribe_event --> OK" << endl;
		
//
// Execute the command which will fire an attribute
// data ready event
//

		Tango::DevVarLongStringArray dvlsa;
		dvlsa.svalue.length(1);
		dvlsa.lvalue.length(1);
		dvlsa.lvalue[0] = 10;
		dvlsa.svalue[0] = CORBA::string_dup(att_name.c_str());
		DeviceData d_in;
		d_in << dvlsa;
		device->command_inout("PushDataReady",d_in);
				
		Tango_sleep(1);
		
		assert (cb.cb_executed == 2);
		assert (cb.user_ctr == 10);
		
//
// Redo the command several times
//

		dvlsa.lvalue[0] = 15;
		d_in << dvlsa;
		device->command_inout("PushDataReady",d_in);
		
		dvlsa.lvalue[0] = 16;
		d_in << dvlsa;
		device->command_inout("PushDataReady",d_in);
		
		dvlsa.lvalue[0] = 17;
		d_in << dvlsa;
		device->command_inout("PushDataReady",d_in);
		
		Tango_sleep(1);
		
		assert (cb.cb_executed == 8);
		assert (cb.user_ctr == 17);		
		
		cout << "   data_ready_event --> OK" << endl;
		
//
// Server push event on a non-exiting attribute
//

		bool received_err = false;
		string err_reason;
		
		dvlsa.svalue[0] = CORBA::string_dup("bidon");
		d_in << dvlsa;
		
		try
		{		
			device->command_inout("PushDataReady",d_in);
		}
		catch (Tango::DevFailed &e)
		{
			received_err = true;
			err_reason = e.errors[0].reason;
		}
		
		assert (received_err == true);
		assert (err_reason == "API_AttrNotFound");
			
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
