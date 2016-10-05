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
	void push_event(Tango::DevIntrChangeEventData*);
	
public:
	int cb_executed;
	int cb_err;
	size_t nb_cmd;
	size_t nb_att;
	bool dev_start;
};

void EventCallBack::push_event(Tango::DevIntrChangeEventData* event_data)
{
	cb_executed++;

	try
	{
		coutv << "EventCallBack::push_event(): called device " << event_data->device_name << " event " << event_data->event << "\n";
		if (!event_data->err)
		{
			coutv << "Received device interface change event for device " << event_data->device_name << endl;
			nb_cmd = event_data->cmd_list.size();
			nb_att = event_data->att_list.size();
			dev_start = event_data->dev_started;
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
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;

//
// subscribe to a data ready event
//
		
		int eve_id1 = device->subscribe_event(Tango::INTERFACE_CHANGE_EVENT,&cb);

//
// The callback should have been executed once
//

		assert (cb.cb_executed == 1);
		assert (cb.dev_start == true);

		size_t old_cmd_nb = cb.nb_cmd;
		size_t old_att_nb = cb.nb_att;
		cb.dev_start = false;
						
		cout << "   subscribe_event --> OK" << endl;
		
//
// Add a dynamic command -> Should generate a device interface change event
//

		Tango::DevLong in = 0;
		DeviceData d_in;
		d_in << in;
		device->command_inout("IOAddCommand",d_in);
				
		Tango_sleep(1);
		
		assert (cb.cb_executed == 2);
		assert (cb.nb_cmd == old_cmd_nb + 1);
		assert (cb.nb_att == old_att_nb);

//
// Remove the dynamic command -> Should generate a device interface change event
//

		device->command_inout("IORemoveCommand",d_in);
				
		Tango_sleep(1);
		
		assert (cb.cb_executed == 3);
		assert (cb.nb_cmd == old_cmd_nb);
		assert (cb.nb_att == old_att_nb);

//
// Add dynamic commands in loop -> 1 event
//

		in = 2;
		d_in << in;
		device->command_inout("IOAddCommand",d_in);

		Tango_sleep(1);

		assert (cb.cb_executed == 4);
		assert (cb.nb_cmd == old_cmd_nb + 3);
		assert (cb.nb_att == old_att_nb);

		cout << "   Dev. interface change event after add/remove dynamic command --> OK" << endl;

//
// Init and DevRestart command without any dev. interface change -> No event
//

		device->command_inout("Init");
		Tango_sleep(1);

		assert (cb.cb_executed == 4);
		string adm_name = device->adm_name();
		DeviceProxy adm(adm_name);

		Tango::DeviceData d_in_restart;
		d_in_restart << device_name;
		adm.command_inout("DevRestart",d_in_restart);
		Tango_sleep(1);

		assert (cb.cb_executed == 5);
		assert (cb.nb_cmd == old_cmd_nb);
		assert (cb.nb_att == old_att_nb);

		cout << "   Dev. interface change event after Init or DevRestart command --> OK" << endl;

//
// Restart server cmd -> event generated
//

		in = 0;
		d_in << in;
		device->command_inout("IOAddCommand",d_in);
				
		Tango_sleep(1);

		assert (cb.cb_executed == 6);
		assert (cb.nb_cmd == old_cmd_nb + 1);
		assert (cb.nb_att == old_att_nb);

		adm.command_inout("RestartServer");
		Tango_sleep(5);

		assert (cb.cb_executed == 7);
		assert (cb.nb_cmd == old_cmd_nb);
		assert (cb.nb_att == old_att_nb);
		assert (cb.dev_start == false);

		cout << "   Event sent after RestartServer command --> OK" << endl;
	
//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id1);
		
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
