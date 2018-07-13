/* 
 * example of a client using the TANGO device api.
 * This client can be used to test the reconnection
 * when a client subscribed to several events
 */

#include <tango.h>
#include <assert.h>

#ifndef WIN32
#include <unistd.h>
#endif

#define	coutv	if (verbose == true) cout

using namespace Tango;

bool verbose = false;

class EventCallback : public Tango::CallBack
{
public:
    EventCallback(string cb_name): cb_executed(0), cb_err(0), name(cb_name) { };
    ~EventCallback() { };
    void push_event( Tango::EventData *ed );

	int cb_executed;
	int cb_err;
	string name;
};

void EventCallback::push_event( Tango::EventData *ed )
{
	coutv << "In callback " << name << " with error flag = " << std::boolalpha << ed->err << endl;
    if(ed->err == false)
	{
		cb_executed++;
	}
    else
	{
		cb_err++;
		coutv << "Error: " << ed->errors[0].reason << endl;
	}
}

int main(int argc, char **argv)
{
	DeviceProxy *device1;
    DeviceProxy *device2;
	
	if (argc < 3)
	{
		cout << "usage: %s device1 device2 [-v]" << endl;
		exit(-1);
	}

	string device_name1 = argv[1];
    string device_name2 = argv[2];

	if (argc == 4)
	{
		if (strcmp(argv[3],"-v") == 0)
			verbose = true;
	}
	
	try 
	{
		device1 = new DeviceProxy(device_name1);
        device2 = new DeviceProxy(device_name2);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(1);
	}

	string att_name ("event_change_tst");
	
	try
	{

//
// Subscribe to a user event on device 1
//

    	const vector<string>  filters;
    	EventCallback *eventCallback1 = new EventCallback("1");
		eventCallback1->cb_executed = 0;
		eventCallback1->cb_err = 0;

        device1->subscribe_event(att_name,Tango::USER_EVENT,eventCallback1,filters);

//
// Fire one event on device1
//
		device1->command_inout("IOPushEvent");
		sleep(1);

//
// Subscribe to a user event on device 2
//
        EventCallback *eventCallback2 = new EventCallback("2");
        eventCallback2->cb_executed = 0;
        eventCallback2->cb_err = 0;

        device2->subscribe_event(att_name,Tango::USER_EVENT,eventCallback2,filters);


// Fire events on device 1 and 2
		device1->command_inout("IOPushEvent");
        device2->command_inout("IOPushEvent");
		
		Tango_sleep(1);

		coutv << "Callback1 execution before re-connection = " << eventCallback1->cb_executed << endl;
		coutv << "Callback1 error before re-connection = " << eventCallback1->cb_err << endl;
		
		assert (eventCallback1->cb_executed == 3);
		assert (eventCallback1->cb_err == 0);

        coutv << "Callback2 execution before re-connection = " << eventCallback2->cb_executed << endl;
        coutv << "Callback2 error before re-connection = " << eventCallback2->cb_err << endl;

        assert (eventCallback2->cb_executed == 2);
        assert (eventCallback2->cb_err == 0);

//
// Kill device servers (using their admin device)
//

		string adm_name1 = device1->adm_name();
		DeviceProxy admin_dev1(adm_name1);
        string adm_name2 = device2->adm_name();
        DeviceProxy admin_dev2(adm_name2);
		admin_dev1.command_inout("kill");
        admin_dev2.command_inout("kill");

//
// Wait for some error and re-connection - A script or the user will restart the servers
//

		Tango_sleep(40);
	
//
// Check error and re-connection
//

		coutv << "Callback1 execution after re-connection = " << eventCallback1->cb_executed << endl;
		coutv << "Callback1 error after re-connection = " << eventCallback1->cb_err << endl;

		assert (eventCallback1->cb_err >= 1);
		assert (eventCallback1->cb_executed == 4);

        coutv << "Callback2 execution after re-connection = " << eventCallback2->cb_executed << endl;
        coutv << "Callback2 error after re-connection = " << eventCallback2->cb_err << endl;

        assert (eventCallback2->cb_err >= 1);
        assert (eventCallback2->cb_executed == 3);

//
// Fire other events
//

		device1->command_inout("IOPushEvent");
		device2->command_inout("IOPushEvent");
        device2->command_inout("IOPushEvent");


        Tango_sleep(1);

		coutv << "Callback1 execution after re-connection and event = " << eventCallback1->cb_executed << endl;
		coutv << "Callback1 error after re-connection and event = " << eventCallback1->cb_err << endl;
	
		assert (eventCallback1->cb_executed == 5);
		assert (eventCallback1->cb_err >= 1);

        coutv << "Callback2 execution after re-connection and event = " << eventCallback2->cb_executed << endl;
        coutv << "Callback2 error after re-connection and event = " << eventCallback2->cb_err << endl;

        assert (eventCallback1->cb_executed == 5);
        assert (eventCallback1->cb_err >= 1);

		cout << "   Event re-connection (client subscribing to 2 attributes from 2 different devices - different ports) --> OK" << endl;

//
// Clear call back counters and kill device servers once more
//

        eventCallback1->cb_executed = 0;
        eventCallback1->cb_err = 0;
        eventCallback2->cb_executed = 0;
        eventCallback2->cb_err = 0;

		admin_dev1.command_inout("kill");
        admin_dev2.command_inout("kill");

//
// Wait for some error and re-connectiongit d
//

		Tango_sleep(40);
	
//
// Check error and re-connection
//

		coutv << "Callback1 execution after second re-connection = " << eventCallback1->cb_executed << endl;
		coutv << "Callback1 error after second re-connection = " << eventCallback1->cb_err << endl;

		assert (eventCallback1->cb_err >= 1);
		assert (eventCallback1->cb_executed == 1);

        coutv << "Callback2 execution after second re-connection = " << eventCallback2->cb_executed << endl;
        coutv << "Callback2 error after second re-connection = " << eventCallback2->cb_err << endl;

        assert (eventCallback2->cb_err >= 1);
        assert (eventCallback2->cb_executed == 1);

//
// Fire yet another event
//

		device1->command_inout("IOPushEvent");
        device2->command_inout("IOPushEvent");
        device1->command_inout("IOPushEvent");
        device2->command_inout("IOPushEvent");
        device2->command_inout("IOPushEvent");
		
		Tango_sleep(2);

		coutv << "Callback1 execution after second re-connection and event = " << eventCallback1->cb_executed << endl;
		coutv << "Callback1 error after second re-connection and event = " << eventCallback1->cb_err << endl;
	
		assert (eventCallback1->cb_executed == 3);
		assert (eventCallback1->cb_err >= 1);

        coutv << "Callback2 execution after second re-connection and event = " << eventCallback2->cb_executed << endl;
        coutv << "Callback2 error after second re-connection and event = " << eventCallback2->cb_err << endl;

        assert (eventCallback2->cb_executed == 4);
        assert (eventCallback2->cb_err >= 1);


		cout << "   Event re-connection (client subscribing to 2 attributes from 2 different devices - same ports) --> OK" << endl;
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	delete device1;
	delete device2;
	
	return 0;
}
