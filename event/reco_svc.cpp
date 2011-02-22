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

void wait_and_dot(int);

bool verbose = false;

class EventCallBack : public Tango::CallBack
{
	void push_event(Tango::EventData*);
	
public:
	int cb_executed;
	int cb_err;
	int cb_no_err;
	int old_sec,old_usec;
	int delta_msec;
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
			coutv << "CallBack without error " << endl;
			cb_no_err++;
		}
		else
		{
			coutv << "Error send to callback" << endl;
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
		string att_name_change("event_change_tst");
				
//
// Test set up (stop polling and clear event_period attribute property but
// restart device to take this into account)
//

		if (device->is_attribute_polled(att_name))
			device->stop_poll_attribute(att_name);
		if (device->is_attribute_polled(att_name_change))
			device->stop_poll_attribute(att_name_change);
			
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
// subscribe to a periodic and to a change event
//

		int eve_id1,eve_id2;
		int eve_id_change1,eve_id_change2;
		vector<string> filters;
		
		EventCallBack cb1;
		cb1.cb_executed = 0;
		cb1.cb_err = 0;
		cb1.cb_no_err = 0;
		cb1.old_sec = cb1.old_usec = 0;

		EventCallBack cb2;
		cb2.cb_executed = 0;
		cb2.cb_err = 0;
		cb2.cb_no_err = 0;
		cb2.old_sec = cb2.old_usec = 0;
		
		EventCallBack cb_change1;
		cb_change1.cb_executed = 0;
		cb_change1.cb_err = 0;
		cb_change1.cb_no_err = 0;
		cb_change1.old_sec = cb_change1.old_usec = 0;

		EventCallBack cb_change2;
		cb_change2.cb_executed = 0;
		cb_change2.cb_err = 0;
		cb_change2.cb_no_err = 0;
		cb_change2.old_sec = cb_change2.old_usec = 0;
		
		// start the polling first!
		device->poll_attribute(att_name,1000);
		device->poll_attribute(att_name_change,1000);
		
		eve_id1 = device->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb1,filters);
		eve_id2 = device->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb2,filters);	
		eve_id_change1 = device->subscribe_event(att_name_change,Tango::CHANGE_EVENT,&cb_change1,filters);
		eve_id_change2 = device->subscribe_event(att_name_change,Tango::CHANGE_EVENT,&cb_change2,filters);	
				
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
		int rest = sleep(3);
		if (rest != 0)
			sleep(3);
#else
		Sleep(3000);
#endif
			
		coutv << "cb excuted = " << cb1.cb_executed << endl;
		assert (cb1.cb_executed >= 2);
		assert (cb1.cb_executed < 5);
		assert (cb1.delta_msec > 950);
		assert (cb1.delta_msec < 1050);

		assert (cb2.cb_executed >= 2);
		assert (cb2.cb_executed < 5);
		assert (cb2.delta_msec > 950);
		assert (cb2.delta_msec < 1050);
				
		cout << "   CallBack executed every 1000 mS for periodic event--> OK" << endl;
		
		assert (cb_change1.cb_executed != 0);
		assert (cb_change2.cb_executed != 0);
		
cout << "   CallBack executed once for change event--> OK" << endl;

//
// Now, kill the server
//

		string key;
		cout << "Kill device server and hit \"return\" key when done" << endl;
		cout << "Process will wait for callback with error during 25 sec";
		getline(cin,key);

//
// Checking for error in callback
//

		wait_and_dot(25);
		
		assert (cb1.cb_err != 0);
		assert (cb_change1.cb_err != 0);

		assert (cb2.cb_err != 0);
		assert (cb_change2.cb_err != 0);
		
		cout << "\n   CallBacks executed with error --> OK" << endl;

//
// Now, Restart DS
//

		int old_cb_no_err1,old_cb_no_err2;
		int new_cb_no_err1,new_cb_no_err2;
		
		old_cb_no_err1 = cb_change1.cb_no_err;
		old_cb_no_err2 = cb_change2.cb_no_err;
		cout << "Restart DS and hit \"return\" key when done" << endl;
		cout << "Process will wait for callback without error during 25 sec";
		getline(cin,key);
		
		int old_cb_err1,old_cb_err_ch1;
		int new_cb_err1,new_cb_err_ch1;
		int old_cb_err2,old_cb_err_ch2;
		int new_cb_err2,new_cb_err_ch2;		

		old_cb_err1 = cb1.cb_executed;
		old_cb_err_ch1 = cb_change1.cb_executed;

		old_cb_err2 = cb2.cb_executed;
		old_cb_err_ch2 = cb_change2.cb_executed;

		wait_and_dot(25);
		
		new_cb_err1 = cb1.cb_executed;
		new_cb_err_ch1 = cb_change1.cb_executed;
		new_cb_no_err1 = cb_change1.cb_no_err;

		new_cb_err2 = cb2.cb_executed;
		new_cb_err_ch2 = cb_change2.cb_executed;
		new_cb_no_err2 = cb_change2.cb_no_err;

		assert ((new_cb_err1 - old_cb_err1) > 4);
		assert ((new_cb_no_err1 - old_cb_no_err1) >= 1);

		assert ((new_cb_err2 - old_cb_err2) > 4);
		assert ((new_cb_no_err2 - old_cb_no_err2) >= 1);
		
		cout << "\n   CallBacks executed after reconnection to notifd --> OK" << endl;
												
//
// Now, kill the notifd
//

		cout << "Kill notifd process and hit \"return\" key when done" << endl;
		cout << "Process will wait for callback with error during 30 sec";
		getline(cin,key);
		
//
// Checking for error in callback
//

		wait_and_dot(30);
		
		assert (cb1.cb_err != 0);
		assert (cb_change1.cb_err != 0);

		assert (cb2.cb_err != 0);
		assert (cb_change2.cb_err != 0);
		
		cout << "\n   CallBacks executed with error --> OK" << endl;

//
// Now, Restart notifd
//

#ifdef WIN32
		cout << "Restart notifd (notifd -n -DFactoryIORFileName=C:\\Temp\\evfact.ior)" << endl;
		cout << "and hit \"return\" key when done" << endl;
#else
		cout << "Restart notifd (notifd -n) and hit \"return\" key when done" << endl;
#endif
		cout << "Process will wait for callback with error during 25 sec";
		getline(cin,key);
		
//
// Checking for error in callback
//

		old_cb_err1 = cb1.cb_err;
		old_cb_err_ch1 = cb_change1.cb_err;

		old_cb_err2 = cb2.cb_err;
		old_cb_err_ch2 = cb_change2.cb_err;

		wait_and_dot(25);

		new_cb_err1 = cb1.cb_err;
		new_cb_err_ch1 = cb_change1.cb_err;

		new_cb_err2 = cb2.cb_err;
		new_cb_err_ch2 = cb_change2.cb_err;
		
		assert ((new_cb_err1 - old_cb_err1) >= 2);
		assert ((new_cb_err_ch1 - old_cb_err_ch1) >= 2);

		assert ((new_cb_err2 - old_cb_err2) >= 2);
		assert ((new_cb_err_ch2 - old_cb_err_ch2) >= 2);		
		
		cout << "\n   CallBacks executed with error --> OK" << endl;

//
// Now, export notifd to db
//

		old_cb_no_err1 = cb_change1.cb_no_err;
		old_cb_no_err2 = cb_change2.cb_no_err;
#ifdef WIN32
		cout << "Export notifd to db (notifd2db C:\\Temp\\evfact.ior) and hit \"return\" key when done" << endl;
#else
		cout << "Export notifd to db (notifd2db) and hit \"return\" key when done" << endl;
#endif
		cout << "Process will wait for callback during 20 sec";
		getline(cin,key);
		
//
// Checking for error in callback
//

		old_cb_err1 = cb1.cb_executed;
		old_cb_err_ch1 = cb_change1.cb_executed;

		old_cb_err2 = cb2.cb_executed;
		old_cb_err_ch2 = cb_change2.cb_executed;

		wait_and_dot(20);
		
		new_cb_err1 = cb1.cb_executed;
		new_cb_err_ch1 = cb_change1.cb_executed;
		new_cb_no_err1 = cb_change1.cb_no_err;

		new_cb_err2 = cb2.cb_executed;
		new_cb_err_ch2 = cb_change2.cb_executed;
		new_cb_no_err2 = cb_change2.cb_no_err;

		assert ((new_cb_err1 - old_cb_err1) > 4);
		assert ((new_cb_no_err1 - old_cb_no_err1) >= 1);

		assert ((new_cb_err2 - old_cb_err2) > 4);
		assert ((new_cb_no_err2 - old_cb_no_err2) >= 1);
		
		cout << "\n   CallBacks executed after reconnection to notifd --> OK" << endl;	
		
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id1);
		device->unsubscribe_event(eve_id2);
		device->unsubscribe_event(eve_id_change1);
		device->unsubscribe_event(eve_id_change2);
		
		cout << "   unsubscribe_event --> OK" << endl;		
//
// Stop polling
//
		device->stop_poll_attribute(att_name);
		device->stop_poll_attribute(att_name_change);			
							
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


void wait_and_dot(int max)
{
	int loop = 0;
	for (loop = 0;loop < max;loop++)
	{
#ifndef WIN32
		int rest = sleep(1);
		if (rest != 0)
			sleep(1);
#else
		Sleep(1000);
#endif
		cout << ".";
		cout.flush();
	}
}
