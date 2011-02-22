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
	long val;
	long val_size;
};

void EventCallBack::push_event(Tango::EventData* event_data)
{
	vector<DevLong> value;
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
			coutv << "CallBack value size " << value.size() << endl;
			val = value[2];
			val_size = value.size();
			coutv << "Callback value " << val << endl;
		}
		else
		{
			coutv << "Error send to callback" << endl;
//			Tango::Except::print_error_stack(event_data->errors);
			if (strcmp(event_data->errors[0].reason.in(),"bbb") == 0)
				cb_err++;
		}
	}
	catch (...)
	{
		coutv << "EventCallBack::push_event(): could not extract data !\n";
	}

}


class EventUnsubCallBack : public Tango::CallBack
{
public:
	EventUnsubCallBack(DeviceProxy *d):cb_executed(0),dev(d),Tango::CallBack() {}

	void push_event(Tango::EventData*);
	void set_ev_id(int e) {ev_id = e;}
	
protected:
	int 				cb_executed;
	int 				ev_id;
	Tango::DeviceProxy 	*dev;
};

void EventUnsubCallBack::push_event(Tango::EventData* event_data)
{
	cb_executed++;
	if (cb_executed == 2)
	{
		dev->unsubscribe_event(ev_id);
	}
}

//---------------------------------------------------------------------------------------------

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
// Test set up (stop polling and clear abs_change and rel_change attribute
// properties but restart device to take this into account)
// Set the abs_change to 1
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
		DbAttribute dba(att_name,device_name);
		DbData dbd;
		DbDatum a(att_name);
		a << (short)2;
		dbd.push_back(a);
		dbd.push_back(DbDatum("abs_change"));
		dbd.push_back(DbDatum("rel_change"));
		dba.delete_property(dbd);
		
		dbd.clear();
		a << (short)1;
		dbd.push_back(a);
		DbDatum ch("abs_change");
		ch << (short)1;
		dbd.push_back(ch);
		dba.put_property(dbd);
		
		DeviceProxy adm_dev(device->adm_name().c_str());
		DeviceData di;
		di << device_name;
		adm_dev.command_inout("DevRestart",di);
		
		delete device;
		device = new DeviceProxy(device_name);
		Tango_sleep(1);

//
// subscribe to a change event
//

		int eve_id;
		vector<string> filters;
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
		// switch on the polling first!
		device->poll_attribute(att_name,1000);
		eve_id = device->subscribe_event(att_name,Tango::CHANGE_EVENT,&cb,filters);

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
		assert (cb.val == 30);
		assert (cb.val_size == 4);
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
			
		device->command_inout("IOIncValue");

#ifndef WIN32
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif
						
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 2);
		assert (cb.val == 31);
		assert (cb.val_size == 4);
				
		cout << "   CallBack executed for positive absolute delta --> OK" << endl;

//
// Check that callback was called after a negative value change 
//

		device->command_inout("IODecValue");

#ifndef WIN32		
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif
			
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 3);
		assert (cb.val == 30);
		assert (cb.val_size == 4);
				
		cout << "   CallBack executed for negative absolute delta --> OK" << endl;

//
// Check that callback was called after adding one elt to the spectrum
//

		device->command_inout("IOAddOneElt");
		
		Tango_sleep(2);
			
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 4);
		assert (cb.val == 30);
		assert (cb.val_size == 5);
				
		cout << "   CallBack executed when spectrum size increases --> OK" << endl;

//
// Check that callback was called after removing one elt to the spectrum
//

		device->command_inout("IORemoveOneElt");
		
		Tango_sleep(2);
			
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 5);
		assert (cb.val == 30);
		assert (cb.val_size == 4);
				
		cout << "   CallBack executed when spectrum size decreases --> OK" << endl;
								
//
// Force the attribute to throw exception
//

		vector<short> data_in(2);
		data_in[0] = 1;
		data_in[1] = 1;
		di << data_in;
		
		device->command_inout("IOAttrThrowEx",di);
		
//
// Check that callback was called
//
		
		Tango_sleep(3);
		coutv << "Callback cb_err = " << cb.cb_err << endl;
		assert (cb.cb_err == 1);
				
		cout << "   CallBack executed when attribute throw exception (only once) --> OK" << endl;		

//
// Attribute does not send exception any more
//

		data_in[1] = 0;
		di << data_in;
		
		device->command_inout("IOAttrThrowEx",di);

//
// Check that the event is still received event after a try to subscribe with a null callback
//

		EventCallBack cb2;
		cb2.cb_executed = 0;
		cb2.cb_err = 0;
		cb2.old_sec = cb2.old_usec = 0;	

		bool ex = false;
		bool already_conn = false;
		try
		{
			int eve_id2 = device->subscribe_event(att_name,Tango::CHANGE_EVENT,(CallBack *)NULL,filters);
		}
		catch (Tango::DevFailed &e)
		{
//			Tango::Except::print_exception(e);
			ex = true;
			string de(e.errors[0].desc);
			if (de.find("Already") != string::npos)
				already_conn = true;
		}

		device->command_inout("IOIncValue");

#ifndef WIN32
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif
						
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 7);
		assert (ex == true);
				
		cout << "   CallBack executed after a try to subscribe to one attribute with a NULL callback --> OK" << endl;
					
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
		cout << "   unsubscribe_event --> OK" << endl;

//
// Subscribe to the event using the full device name
//

		string full_device_name("tango://kidiboo:10000/");
		full_device_name = full_device_name + device_name;
		Tango::DeviceProxy full_dp(full_device_name);

		cb.cb_executed = 0;
		int ev_id_full = full_dp.subscribe_event(att_name,Tango::CHANGE_EVENT,&cb,filters);

		assert (cb.cb_executed == 1);

		cout << "   Subscription using Tango full device name --> OK" << endl;

		full_dp.unsubscribe_event(ev_id_full);

//
// Subscribe to the event using the full device name with fqdn
//

		string full_device_name_fqdn("tango://kidiboo.esrf.fr:10000/");
		full_device_name_fqdn = full_device_name_fqdn + device_name;
		Tango::DeviceProxy full_dp_fq(full_device_name_fqdn);

		cb.cb_executed = 0;
		int ev_id_full_fq = full_dp_fq.subscribe_event(att_name,Tango::CHANGE_EVENT,&cb,filters);

		assert (cb.cb_executed == 1);

		cout << "   Subscription using Tango full device name with FQDN --> OK" << endl;

		full_dp_fq.unsubscribe_event(ev_id_full_fq);

//sleep(1000);
delete device;
Tango::ApiUtil::cleanup();
device = new DeviceProxy(device_name);

//
// Change the event parameters. This means restart the device to take
// this into account
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
		dbd.clear();
		a << (short)2;
		dbd.push_back(a);
		dbd.push_back(DbDatum("abs_change"));
		dbd.push_back(DbDatum("rel_change"));
		dba.delete_property(dbd);
		
		dbd.clear();
		a << (short)1;
		dbd.push_back(a);
		DbDatum drel("rel_change");
		drel << (short)10;
		dbd.push_back(drel);
		dba.put_property(dbd);
		
		DeviceProxy adm_dev_1 = device->adm_name().c_str();
		di << device_name;
		adm_dev_1.command_inout("DevRestart",di);
		
		delete device;
		device = new DeviceProxy(device_name);
		Tango_sleep(1);

//
// Poll attribute at 500 mS
//

		device->poll_attribute(att_name,500);
		
//
// subscribe to a change event
//

		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
	
		eve_id = device->subscribe_event(att_name,Tango::CHANGE_EVENT,&cb,filters);

//
// Check that the attribute is still polled at 500 mS
//

		po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == true);
		
		poll_period = device->get_attribute_poll_period(att_name);
		coutv << "att polling period : " << poll_period << endl;
		assert( poll_period == 500);		
				
		cout << "   subscribe_event (with relative change) --> OK" << endl;

//
// Check that first point has been received
//

		assert (cb.cb_executed == 1);
		assert (cb.val == 30);
		assert (cb.val_size == 4);
		cout << "   first point received (with relative change) --> OK" << endl;

//
// Generates a positive change
//

#ifndef WIN32
		rest = sleep(1);
		if (rest != 0)
			sleep(1);
#else
		Sleep(1000);
#endif
			
		device->command_inout("IOIncValue");

#ifndef WIN32
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif
						
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 1);
		
		device->command_inout("IOIncValue");
		device->command_inout("IOIncValue");
		
		Tango_sleep(1);
		
		assert (cb.cb_executed == 2);
		assert (cb.val == 33);
		assert (cb.val_size == 4);
				
		cout << "   CallBack executed for positive relative delta --> OK" << endl;

//
// Generates a negative change (10 % of 33 is 3.3)
//
			
		device->command_inout("IODecValue");

#ifndef WIN32
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif
						
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 2);
		
		device->command_inout("IODecValue");
		device->command_inout("IODecValue");
		
		Tango_sleep(1);
		
		assert (cb.cb_executed == 2);
		
		device->command_inout("IODecValue");
		
		Tango_sleep(1);
		
		assert (cb.cb_executed == 3);
		assert (cb.val == 29);
		assert (cb.val_size == 4);
				
		cout << "   CallBack executed for negative relative delta --> OK" << endl;		

//
// Force the attribute to throw exception
//

		data_in[0] = 1;
		data_in[1] = 1;
		di << data_in;
		
		device->command_inout("IOAttrThrowEx",di);
		
//
// Check that callback was called
//
		
		Tango_sleep(2);
		coutv << "Callback cb_err = " << cb.cb_err << endl;
		assert (cb.cb_err == 1);
				
		cout << "   CallBack executed when attribute throw exception only once --> OK" << endl;		

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
	
delete device;
Tango::ApiUtil::cleanup();
device = new DeviceProxy(device_name);

//
// Change the event parameters. This means restart the device to take
// this into account
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
		dbd.clear();
		a << (short)2;
		dbd.push_back(a);
		dbd.push_back(DbDatum("abs_change"));
		dbd.push_back(DbDatum("rel_change"));
		dba.delete_property(dbd);
		
		dbd.clear();
		a << (short)1;
		dbd.push_back(a);
		dbd.push_back(ch);
		dba.put_property(dbd);
		
		DeviceProxy adm_dev_2 = device->adm_name().c_str();
		di << device_name;
		adm_dev_2.command_inout("DevRestart",di);
		
		delete device;
		device = new DeviceProxy(device_name);
		Tango_sleep(1);
		
//
// subscribe to a change event with a filter
//

		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		filters.push_back("$delta_change_abs >= 2 or $delta_change_abs <= -2");
		// start the polling first!
		device->poll_attribute(att_name,1000);
		eve_id = device->subscribe_event(att_name,Tango::CHANGE_EVENT,&cb,filters);

//
// Check that the attribute is still polled at 1000 mS
//

		po = device->is_attribute_polled(att_name);
		coutv << "attribute polled : " << po << endl;
		assert( po == true);
		
		poll_period = device->get_attribute_poll_period(att_name);
		coutv << "att polling period : " << poll_period << endl;
		assert( poll_period == 1000);		
				
		cout << "   subscribe_event (with filter) --> OK" << endl;

//
// Check that first point has been received
//

		assert (cb.cb_executed == 1);
		assert (cb.val == 30);
		assert (cb.val_size == 4);
		cout << "   first point received (with filter) --> OK" << endl;

//
// Generates a positive change
//

#ifndef WIN32
	 	rest = sleep(1);
		if (rest != 0)
			sleep(1);
#else
		Sleep(1000);
#endif
			
		device->command_inout("IOIncValue");

#ifndef WIN32
		rest = sleep(1);
		if (rest != 0)
			sleep(1);
#else
		Sleep(1000);
#endif
						
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 1);

// Let's hope the polling thread will not start just between the two command_inout
		
		device->command_inout("IOIncValue");
		device->command_inout("IOIncValue");
		
		Tango_sleep(3);
		
		assert (cb.cb_executed == 2);
		assert (cb.val == 33);
		assert (cb.val_size == 4);
				
		cout << "   CallBack executed for positive delta (with filter) --> OK" << endl;

//
// Generates a negative change
//
			
		device->command_inout("IODecValue");

#ifndef WIN32
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif
						
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 2);
		
		device->command_inout("IODecValue");
		device->command_inout("IODecValue");
		device->command_inout("IODecValue");
		
		Tango_sleep(2);
		
		assert (cb.cb_executed == 3);
		assert (cb.val == 29);
		assert (cb.val_size == 4);
				
		cout << "   CallBack executed for negative delta (with filter) --> OK" << endl;		

//
// Check that callback was called after adding one elt to the spectrum
//

		device->command_inout("IOAddOneElt");
		
		Tango_sleep(2);
			
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 4);
		assert (cb.val == 29);
		assert (cb.val_size == 5);
				
		cout << "   CallBack executed when spectrum size increases (with filter) --> OK" << endl;

//
// Check that callback was called after removing one elt to the spectrum
//

		device->command_inout("IORemoveOneElt");
		
		Tango_sleep(2);
			
		coutv << "cb excuted = " << cb.cb_executed << endl;
		assert (cb.cb_executed == 5);
		assert (cb.val == 29);
		assert (cb.val_size == 4);
				
		cout << "   CallBack executed when spectrum size decreases (with filter) --> OK" << endl;

//
// Force the attribute to throw exception
//

		data_in[0] = 1;
		data_in[1] = 1;
		di << data_in;
		
		device->command_inout("IOAttrThrowEx",di);
		
//
// Check that callback was called
//
		
		Tango_sleep(3);
		coutv << "Callback cb_err = " << cb.cb_err << endl;
		assert (cb.cb_err == 1);
				
		cout << "   CallBack executed when attribute throw exception only once (with filter) --> OK" << endl;		

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

//
// Try to unsubscribe within the callback
//

		EventUnsubCallBack cb_unsub(device);
		
		eve_id = device->subscribe_event(att_name,Tango::CHANGE_EVENT,&cb_unsub,filters);
		cb_unsub.set_ev_id(eve_id);

		device->command_inout("IOIncValue");

#ifndef WIN32
		rest = sleep(2);
		if (rest != 0)
			sleep(2);
#else
		Sleep(2000);
#endif

		bool unsub = false;
		try
		{
			device->unsubscribe_event(eve_id);
		}
		catch(Tango::DevFailed &e)
		{
//			Tango::Except::print_exception(e);
			string reason(e.errors[0].reason);
			if (reason == "API_EventNotFound")
				unsub = true;
		}

		assert (unsub == true);

		cout << "   Event unsubscription within the callback --> OK" << endl;
			
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
