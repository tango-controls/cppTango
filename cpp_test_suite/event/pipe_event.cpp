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
	void push_event(Tango::PipeEventData*);

public:
	int cb_executed;
	int cb_err;
	std::string root_blob_name;
	size_t	nb_data;
};

void EventCallBack::push_event(Tango::PipeEventData* event_data)
{
	cb_executed++;

	try
	{
		coutv << "EventCallBack::push_event(): called pipe " << event_data->pipe_name << " event " << event_data->event << "\n";
		if (!event_data->err)
		{
			coutv << "Received pipe event for pipe " << event_data->pipe_name << std::endl;
//			coutv << *(event_data->pipe_value) << std::endl;
			root_blob_name = event_data->pipe_value->get_root_blob_name();

			if (root_blob_name == "PipeEventCase4")
			{
				std::vector<Tango::DevLong> v_dl;
				(*(event_data->pipe_value))["Martes"] >> v_dl;
				nb_data = v_dl.size();
			}
		}
		else
		{
			coutv << "Error sent to callback" << std::endl;
//			Tango::Except::print_error_stack(event_data->errors);
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
		cout << "usage: %s device [-v]" << std::endl;
		exit(-1);
	}

	std::string device_name = argv[1];

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

	coutv << std::endl << "new DeviceProxy(" << device->name() << ") returned" << std::endl << std::endl;

	try
	{
		EventCallBack cb;
		cb.cb_executed = 0;
		cb.cb_err = 0;

//
// subscribe to a pipe event
//

		int eve_id1 = device->subscribe_event("RWPipe",Tango::PIPE_EVENT,&cb);

//
// The callback should have been executed once
//

		assert (cb.cb_executed == 1);

		cout << "   subscribe_event --> OK" << std::endl;

//
// Ask device to push a pipe event
//

		Tango::DevShort ev_type = 0;
		Tango::DeviceData dd;
		dd << ev_type;

		device->command_inout("PushPipeEvent",dd);

//
// The callback should have been executed
//

		assert (cb.cb_executed == 2);
		assert (cb.root_blob_name == "PipeEventCase0");

//
// Ask device to push a pipe event with another data
//

		ev_type = 1;
		dd << ev_type;

		device->command_inout("PushPipeEvent",dd);

//
// The callback should have been executed
//


		assert (cb.cb_executed == 3);
		assert (cb.root_blob_name == "PipeEventCase1");

		cout << "   received event --> OK" << std::endl;

//
// Ask device to push a pipe event when date is specified
//

		ev_type = 2;
		dd << ev_type;

		device->command_inout("PushPipeEvent",dd);

//
// The callback should have been executed
//

		assert (cb.cb_executed == 4);
		assert (cb.root_blob_name == "PipeEventCase2");

		cout << "   received event (with specified date) --> OK" << std::endl;

//
// Ask device to push a pipe event with error
//

		ev_type = 3;
		dd << ev_type;

		device->command_inout("PushPipeEvent",dd);

//
// The callback should have been executed
//

		assert (cb.cb_executed == 5);

		cout << "   received event (with error) --> OK" << std::endl;

//
// Ask device to push a pipe event with enough data to trigger a no copy event sending
//

		ev_type = 4;
		dd << ev_type;

		device->command_inout("PushPipeEvent",dd);

//
// The callback should have been executed
//

		assert (cb.cb_executed == 6);
		assert (cb.root_blob_name == "PipeEventCase4");
		assert (cb.nb_data == 3000);

		cout << "   received event (no copy sending) --> OK" << std::endl;


//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id1);

		cout << "   unsubscribe_event --> OK" << std::endl;

//
// subscribe to a another pipe
//

		cb.cb_executed = 0;
		cb.cb_err = 0;

		DeviceData d_in;
		d_in << (short)9;
		device->command_inout("SetPipeOutput",d_in);

		eve_id1 = device->subscribe_event("RPipe",Tango::PIPE_EVENT,&cb);

		assert (cb.cb_executed == 2);

		DevicePipe pipe_data = device->read_pipe("rPipe");

		assert (cb.cb_executed == 3);

		device->unsubscribe_event(eve_id1);

		cout << "   read_pipe which trigger a push_pipe_event --> OK" << std::endl;
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
