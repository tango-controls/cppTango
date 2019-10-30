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

#include <TangoCallbackMock.h>

#define	coutv	if (verbose == true) cout

using namespace Tango;

bool verbose = false;

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
		Tango::CallbackMock<Tango::DevIntrChangeEventData> cb{};

//
// subscribe to a data ready event
//

		int eve_id1 = device->subscribe_event(Tango::INTERFACE_CHANGE_EVENT,&cb);

//
// The callback should have been executed once
//

		size_t old_cmd_nb = 0;
		size_t old_att_nb = 0;

		cb.assert_called(0, [&](const decltype(cb)::Event& event) {
			assert(not event.err);
			assert(event.dev_started);
			old_cmd_nb = event.cmd_list.size();
			old_att_nb = event.att_list.size();
		});

		cout << "   subscribe_event --> OK" << std::endl;

//
// Add a dynamic command -> Should generate a device interface change event
//

		Tango::DevLong in = 0;
		DeviceData d_in;
		d_in << in;
		device->command_inout("IOAddCommand",d_in);

		cb.assert_called(1000, [&](const decltype(cb)::Event& event) {
			assert(event.cmd_list.size() == old_cmd_nb + 1);
			assert(event.att_list.size() == old_att_nb);
		});

//
// Remove the dynamic command -> Should generate a device interface change event
//

		device->command_inout("IORemoveCommand",d_in);

		cb.assert_called(1000, [&](const decltype(cb)::Event& event) {
			assert(event.cmd_list.size() == old_cmd_nb);
			assert(event.att_list.size() == old_att_nb);
		});

//
// Add dynamic commands in loop -> 1 event
//

		in = 2;
		d_in << in;
		device->command_inout("IOAddCommand",d_in);

		cb.assert_called(1000, [&](const decltype(cb)::Event& event) {
			assert(event.cmd_list.size() == old_cmd_nb + 3);
			assert(event.att_list.size() == old_att_nb);
		});

		cout << "   Dev. interface change event after add/remove dynamic command --> OK" << std::endl;

//
// Init and DevRestart command without any dev. interface change -> No event
//

		device->command_inout("Init");

		Tango_sleep(1);
		cb.assert_not_called();

		std::string adm_name = device->adm_name();
		DeviceProxy adm(adm_name);

		Tango::DeviceData d_in_restart;
		d_in_restart << device_name;
		adm.command_inout("DevRestart",d_in_restart);

		cb.assert_called(1000, [&](const decltype(cb)::Event& event) {
			assert(event.cmd_list.size() == old_cmd_nb);
			assert(event.att_list.size() == old_att_nb);
		});

		cout << "   Dev. interface change event after Init or DevRestart command --> OK" << std::endl;

//
// Restart server cmd -> event generated
//

		in = 0;
		d_in << in;
		device->command_inout("IOAddCommand",d_in);

		cb.assert_called(1000, [&](const decltype(cb)::Event& event) {
			assert(event.cmd_list.size() == old_cmd_nb + 1);
			assert(event.att_list.size() == old_att_nb);
		});

		adm.command_inout("RestartServer");

		cb.assert_called(5000, [&](const decltype(cb)::Event& event) {
			assert(event.cmd_list.size() == old_cmd_nb);
			assert(event.att_list.size() == old_att_nb);
			assert(not event.dev_started);
		});

		cb.assert_not_called();

		cout << "   Event sent after RestartServer command --> OK" << std::endl;

//
// unsubscribe to the event
//

		device->unsubscribe_event(eve_id1);

		cout << "   unsubscribe_event --> OK" << std::endl;
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
