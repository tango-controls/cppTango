/* 
 * example of a client using the TANGO device api.
 */

#include <tango/tango.h>
#include <assert.h>

#ifndef WIN32
#include <unistd.h>
#endif

#define	coutv	if (verbose == true) cout

using namespace Tango;

bool verbose = false;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc < 3 || argc > 4)
	{
		cout << "usage: %s local_device remote_device [-v]" << endl;
		exit(-1);
	}

	string device_name = argv[1];

	if (argc == 4)
	{
		if (strcmp(argv[3],"-v") == 0)
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
// Ask the device server to subscribe to an event
//

		vector<string> vs;
		vs.push_back(argv[2]);
		vs.push_back("Short_attr");
		vs.push_back("periodic");

		DeviceData dd_in,dd_out;
		dd_in << vs;
		dd_out = device->command_inout("IOSubscribeEvent",dd_in);
		DevLong eve_id;
		dd_out >> eve_id;

		cout << "   Server subscribe to event --> OK" << endl;

//
// Wait for event to be executed
//

		Tango_sleep(3);
		
		DeviceData da;
		da = device->command_inout("IOGetCbExecuted");
		Tango::DevLong cb;
		da >> cb;

		coutv << "cb executed = " << cb << endl;		
		assert (cb >= 2);
		assert (cb <= 4);
		
		cout << "   Callback executed --> OK" << endl;
		
//
// Ask server to unsubsribe from event
//

		DeviceData dd_un;
		dd_un << eve_id;

		device->command_inout("IOUnSubscribeEvent",dd_un);
		da = device->command_inout("IOGetCbExecuted");
		da >> cb;
		
		Tango_sleep(2);
		da = device->command_inout("IOGetCbExecuted");
		Tango::DevLong cb2;
		da >> cb2;
		
		assert (cb2 == cb);
		cout << "   Server unsubscribe to event --> OK" << endl;						
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
