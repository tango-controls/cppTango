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
	vector<long> value;
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
		a << (short)3;
		dbd.push_back(a);
		dbd.push_back(DbDatum("archive_abs_change"));
		dbd.push_back(DbDatum("archive_rel_change"));
		dbd.push_back(DbDatum("archive_period"));
		dba.delete_property(dbd);
		
		dbd.clear();
		a << (short)1;
		dbd.push_back(a);
		DbDatum ch("archive_abs_change");
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
// subscribe to a archive event
//

		int eve_id;
		vector<string> filters;
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;
		cb.old_sec = cb.old_usec = 0;
		
		eve_id = device->subscribe_event(att_name,Tango::ARCHIVE_EVENT,&cb,filters);

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
		assert (cb.cb_err > 2);
		assert (cb.cb_err < 5);
//		assert (cb.delta_msec > 900);
		assert (cb.delta_msec < 1100);
				
		cout << "   CallBack executed when attribute throw exception every 1000 mS --> OK" << endl;		

//
// Attribute does not send exception any more
//

		data_in[1] = 0;
		di << data_in;
		
		device->command_inout("IOAttrThrowEx",di);
		
//
// Wait for the periodic part of the event (default = 10 sec)
// Wait one second for the callback sent because exception ot thrown any more
//

		Tango_sleep(1);
		long nb_cb = cb.cb_executed;
		Tango_sleep(12);

		coutv << "theorical nb = " << nb_cb + 1 << ", real nb = " << cb.cb_executed << endl;		
		assert (cb.cb_executed == (nb_cb + 1));
		cout << "   CallBack executed for the periodic part of the event --> OK" << endl;
		
								
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
		cout << "   unsubscribe_event --> OK" << endl;
//
// Change the event parameters. This means restart the device to take
// this into account
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
		dbd.clear();
		a << (short)3;
		dbd.push_back(a);
		dbd.push_back(DbDatum("archive_abs_change"));
		dbd.push_back(DbDatum("archive_rel_change"));
		dbd.push_back(DbDatum("archive_period"));
		dba.delete_property(dbd);
		
		dbd.clear();
		a << (short)2;
		dbd.push_back(a);
		DbDatum drel("archive_rel_change");
		drel << (short)10;
		dbd.push_back(drel);
		DbDatum drel1("archive_period");
		drel1 << (short)5000;
		dbd.push_back(drel1);
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
		
		eve_id = device->subscribe_event(att_name,Tango::ARCHIVE_EVENT,&cb,filters);

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
// Note that like archive_period is set to 5 seconds, we should have received
// an archive periodic callback
//
			
		device->command_inout("IODecValue");

#ifndef WIN32
		rest = sleep(3);
		if (rest != 0)
			sleep(3);
#else
		Sleep(3000);
#endif
						
		coutv << "cb excuted = " << cb.cb_executed << endl;
		
// 3 instead of 2 due to the archive_period property

		assert (cb.cb_executed == 3);
		
		device->command_inout("IODecValue");
		device->command_inout("IODecValue");
		
		Tango_sleep(1);
		
		assert (cb.cb_executed == 3);
		
		device->command_inout("IODecValue");
		
		Tango_sleep(1);
		
		assert (cb.cb_executed == 4);
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
		assert (cb.cb_err > 3);
		assert (cb.cb_err < 6);
		assert (cb.delta_msec > 400);
		assert (cb.delta_msec < 600);
				
		cout << "   CallBack executed when attribute throw exception every 500 mS --> OK" << endl;		

//
// Attribute does not send exception any more
//

		data_in[1] = 0;
		di << data_in;
		
		device->command_inout("IOAttrThrowEx",di);

//
// Wait for the periodic part of the event (default = 10 sec)
//

		Tango_sleep(1);
		nb_cb = cb.cb_executed;
		Tango_sleep(7);
		
		assert (cb.cb_executed == (nb_cb + 1));
		cout << "   CallBack executed for the periodic part of the event (archive_period set) --> OK" << endl;
		
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id);
		
		cout << "   unsubscribe_event --> OK" << endl;

//
// Change the event parameters. This means restart the device to take
// this into account
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
		dbd.clear();
		a << (short)3;
		dbd.push_back(a);
		dbd.push_back(DbDatum("archive_abs_change"));
		dbd.push_back(DbDatum("archive_rel_change"));
		dbd.push_back(DbDatum("archive_period"));
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
		
		eve_id = device->subscribe_event(att_name,Tango::ARCHIVE_EVENT,&cb,filters);

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
		
		Tango_sleep(2);
		
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
		assert (cb.cb_err > 2);
		assert (cb.cb_err < 5);
		assert (cb.delta_msec > 950);
		assert (cb.delta_msec < 1050);
				
		cout << "   CallBack executed when attribute throw exception every 1000 mS (with filter) --> OK" << endl;		

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
