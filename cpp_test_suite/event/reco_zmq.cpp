/* 
 * example of a client using the TANGO device api.
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
    EventCallback()  { };
    ~EventCallback() { };
    void push_event( Tango::EventData *ed );

	int cb_executed;
	int cb_err;
};

void EventCallback::push_event( Tango::EventData *ed )
{
	coutv << "In callback with error flag = " << std::boolalpha << ed->err << endl;
    //TODO is it thread safe?
    if(ed->err == false)
		cb_executed++;
    else {
		cb_err++;
		coutv << "Error: " << ed->errors[0].reason << endl;
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

	string att_name ("event_change_tst");
	
	try
	{

//
// Subscribe to a user event
//

    	const vector<string>  filters;
    	EventCallback *eventCallback = new EventCallback();
		eventCallback->cb_executed = 0;
		eventCallback->cb_err = 0;

        device->subscribe_event(att_name,Tango::USER_EVENT,eventCallback,filters);

//
// Fire one event
//

		device->command_inout("IOPushEvent");
		device->command_inout("IOPushEvent");
		
		Tango_sleep(1);

		coutv << "Callback execution before re-connection = " << eventCallback->cb_executed << endl;
		coutv << "Callback error before re-connection = " << eventCallback->cb_err << endl;
		
		assert (eventCallback->cb_executed == 3);
		assert (eventCallback->cb_err == 0);

//
// Kill device server (using its admin device)
//

		string adm_name = device->adm_name();
		DeviceProxy admin_dev(adm_name);
		admin_dev.command_inout("kill");

//
// Wait for some error and re-connection
//

		Tango_sleep(40);
	
//
// Check error and re-connection
//

		coutv << "Callback execution after re-connection = " << eventCallback->cb_executed << endl;
		coutv << "Callback error after re-connection = " << eventCallback->cb_err << endl;

		assert (eventCallback->cb_err >= 1);
		assert (eventCallback->cb_executed == 4);

//
// Fire another event
//

		device->command_inout("IOPushEvent");
		device->command_inout("IOPushEvent");
		
		Tango_sleep(1);

		coutv << "Callback execution after re-connection and event = " << eventCallback->cb_executed << endl;
		coutv << "Callback error after re-connection and event = " << eventCallback->cb_err << endl;
	
		assert (eventCallback->cb_executed == 6);
		assert (eventCallback->cb_err >= 1);

		cout << "   Event re-connection (differents ports) --> OK" << endl;

//
// Clear call back counters and kill device server once more
//

        eventCallback->cb_executed = 0;
        eventCallback->cb_err = 0;

		admin_dev.command_inout("kill");

//
// Wait for some error and re-connection
//

		Tango_sleep(40);
	
//
// Check error and re-connection
//

		coutv << "Callback execution after second re-connection = " << eventCallback->cb_executed << endl;
		coutv << "Callback error after second re-connection = " << eventCallback->cb_err << endl;

		assert (eventCallback->cb_err >= 1);
		assert (eventCallback->cb_executed == 1);

//
// Fire yet another event
//

		device->command_inout("IOPushEvent");
		
		Tango_sleep(2);

		coutv << "Callback execution after second re-connection and event = " << eventCallback->cb_executed << endl;
		coutv << "Callback error after second re-connection and event = " << eventCallback->cb_err << endl;
	
		assert (eventCallback->cb_executed == 2);
		assert (eventCallback->cb_err >= 1);

		cout << "   Event re-connection (same ports) --> OK" << endl;			
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	delete device;
	
	return 0;
}
