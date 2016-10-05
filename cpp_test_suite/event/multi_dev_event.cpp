/* 
 * example of a client using the TANGO device api.
 */

#include <tango/tango.h>
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
	int 					cb_executed;
	int 					cb_err;
	Tango::AttrDataFormat 	d_format;
};

void EventCallBack::push_event(Tango::EventData* event_data)
{
	cb_executed++;

	try
	{
		coutv << "EventCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << "\n";
		if (!event_data->err)
		{
			d_format = event_data->attr_value->get_data_format();
			coutv << "Valid data sent to callBack" << endl;
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
	DeviceProxy *device1,*device2,*device3;
	
	if (argc < 4 || argc > 5)
	{
		cout << "usage: multi_dev_event device1 device2 device3 [-v]" << endl;
		exit(-1);
	}

	string device1_name = argv[1];
	string device2_name = argv[2];
	string device3_name = argv[3];

	if (argc == 5)
	{
		if (strcmp(argv[4],"-v") == 0)
			verbose = true;
	}
	
	try 
	{
		device1 = new DeviceProxy(device1_name);
		device2 = new DeviceProxy(device2_name);
		device3 = new DeviceProxy(device3_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(1);
	}
	
	string att_name("Short_attr");
	string another_att("Event_change_tst");

	try
	{

//
// subscribe to periodic events from three devices
//

		int eve_id1,eve_id2,eve_id3,eve_id4;

		EventCallBack cb_evid1;
		cb_evid1.cb_executed = 0;
		cb_evid1.cb_err = 0;
		cb_evid1.d_format = Tango::FMT_UNKNOWN;

		EventCallBack cb_evid2;
		cb_evid2.cb_executed = 0;
		cb_evid2.cb_err = 0;
		cb_evid2.d_format = Tango::FMT_UNKNOWN;

		EventCallBack cb_evid3;
		cb_evid3.cb_executed = 0;
		cb_evid3.cb_err = 0;
		cb_evid3.d_format = Tango::FMT_UNKNOWN;

		EventCallBack cb_evid4;
		cb_evid4.cb_executed = 0;
		cb_evid4.cb_err = 0;
		cb_evid4.d_format = Tango::FMT_UNKNOWN;
		
		// start the polling first!
		device1->poll_attribute(att_name,500);
		device1->poll_attribute(another_att,500);
		device2->poll_attribute(att_name,500);
		device3->poll_attribute(att_name,500);

		eve_id1 = device1->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb_evid1);
		eve_id2 = device1->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb_evid2);
		eve_id3 = device2->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb_evid3);
		eve_id4 = device3->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb_evid4);
				
		cout << "   subscribe to all events --> OK" << endl;
		
//
// Check that callback was called
//

// A trick for gdb. The thread created by omniORB for the callback execution
// is just started during the sleep. Gdb has a breakpoint reached at each thread
// creation to display message on the console. This breakpoint is a software
// signal which interrupts the sleep.....
//

#ifndef WIN32
		int rest = sleep(5);
		if (rest != 0)
			sleep(5);
#else
		Sleep(5000);
#endif

		int old_cb_evid1,old_cb_evid2,old_cb_evid3;
			
		coutv << "cb_evid1 executed = " << cb_evid1.cb_executed << endl;
		assert (cb_evid1.cb_executed > 2);
		assert (cb_evid1.cb_executed < 8);
		assert (cb_evid1.cb_err == 0);
		assert (cb_evid1.d_format == SCALAR);
		old_cb_evid1 = cb_evid1.cb_executed;

		coutv << "cb_evid2 executed = " << cb_evid2.cb_executed << endl;
		assert (cb_evid2.cb_executed > 2);
		assert (cb_evid2.cb_executed < 8);
		assert (cb_evid2.cb_err == 0);
		assert (cb_evid2.d_format == SCALAR);
		old_cb_evid2 = cb_evid2.cb_executed;

		coutv << "cb_evid3 executed = " << cb_evid3.cb_executed << endl;
		assert (cb_evid3.cb_executed > 2);
		assert (cb_evid3.cb_executed < 8);
		assert (cb_evid3.cb_err == 0);
		assert (cb_evid3.d_format == SCALAR);
		old_cb_evid3 = cb_evid3.cb_executed;

		coutv << "cb_evid4 executed = " << cb_evid4.cb_executed << endl;
		assert (cb_evid4.cb_executed > 2);
		assert (cb_evid4.cb_executed < 8);
		assert (cb_evid4.cb_err == 0);
		assert (cb_evid4.d_format == SCALAR);
		
		cout << "   CallBack executed every 1000 mS for all events --> OK" << endl;

//
// unsubscribe one event
//

		device3->unsubscribe_event(eve_id4);


#ifndef WIN32
		rest = sleep(5);
		if (rest != 0)
			sleep(5);
#else
		Sleep(5000);
#endif

		coutv << "cb_evid1 executed = " << cb_evid1.cb_executed << endl;
		assert (cb_evid1.cb_executed > old_cb_evid1 + 3);
		old_cb_evid1 = cb_evid1.cb_executed;

		coutv << "cb_evid2 executed = " << cb_evid2.cb_executed << endl;
		assert (cb_evid2.cb_executed > old_cb_evid2 + 3);
		old_cb_evid1 = cb_evid1.cb_executed;

		coutv << "cb_evid3 executed = " << cb_evid3.cb_executed << endl;
		assert (cb_evid3.cb_executed > old_cb_evid3 + 3);

		cout << "   Event still received after first unsubscribe_event --> OK" << endl;
						
//
// unsubscribe another event
//

		device2->unsubscribe_event(eve_id3);

#ifndef WIN32
		rest = sleep(5);
		if (rest != 0)
			sleep(5);
#else
		Sleep(5000);
#endif

		coutv << "cb_evid1 executed = " << cb_evid1.cb_executed << endl;
		assert (cb_evid1.cb_executed > old_cb_evid1 + 3);
		old_cb_evid1 = cb_evid1.cb_executed;

		coutv << "cb_evid2 executed = " << cb_evid2.cb_executed << endl;
		assert (cb_evid2.cb_executed > old_cb_evid2 + 3);
		old_cb_evid1 = cb_evid1.cb_executed;

		cout << "   Event still received after second unsubscribe_event --> OK" << endl;

		device1->unsubscribe_event(eve_id2);

#ifndef WIN32
		rest = sleep(5);
		if (rest != 0)
			sleep(5);
#else
		Sleep(5000);
#endif

		coutv << "cb_evid1 executed = " << cb_evid1.cb_executed << endl;
		assert (cb_evid1.cb_executed > old_cb_evid1 + 3);

		cout << "   Event still received after third unsubscribe_event --> OK" << endl;

		device1->unsubscribe_event(eve_id1);
		
		cout << "   unsubscribe_event --> OK" << endl;

//
// Check automatic unsubscription
//

		DeviceProxy *dev = new DeviceProxy(device2_name);

		eve_id1 = device1->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb_evid1);
		eve_id2 = dev->subscribe_event(att_name,Tango::PERIODIC_EVENT,&cb_evid2);
		eve_id3 = dev->subscribe_event("Float_attr",Tango::PERIODIC_EVENT,&cb_evid2,true);

#ifndef WIN32
		rest = sleep(5);
		if (rest != 0)
			sleep(5);
#else
		Sleep(5000);
#endif

		delete dev;

#ifndef WIN32
		rest = sleep(15);
		if (rest != 0)
			sleep(15);
#else
		Sleep(15000);
#endif

		cout << "   DeviceProxy dtor automatically unsubscribe --> OK" << endl;		

		device1->unsubscribe_event(eve_id1);							
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
	}
	catch (CORBA::Exception &ex)
	{
		Except::print_exception(ex);
	}

//
// Stop polling
//

	device1->stop_poll_attribute(att_name);
	device1->stop_poll_attribute(another_att);
	device2->stop_poll_attribute(att_name);
	device3->stop_poll_attribute(att_name);

	delete device1;
	delete device2;
	delete device3;
	
	return 0;
}
