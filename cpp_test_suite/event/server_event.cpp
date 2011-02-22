/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#define	coutv	if (verbose == true) cout

using namespace Tango;

bool verbose = false;

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

//
// Ask the device server to subscribe to an event
//

		device->command_inout("IOSubscribeEvent");
		cout << "   Server subscribe to event --> OK" << endl;

//
// Wait for event to be executed
//

		sleep(3);
		
		DeviceData da;
		da = device->command_inout("IOGetCbExecuted");
		long cb;
		da >> cb;

		coutv << "cb executed = " << cb << endl;		
		assert (cb >= 2);
		assert (cb <= 4);
		
		cout << "   Callback executed --> OK" << endl;
		
//
// Ask server to unsubsribe from event
//

		device->command_inout("IOUnSubscribeEvent");
		da = device->command_inout("IOGetCbExecuted");
		da >> cb;
		
		sleep(2);
		da = device->command_inout("IOGetCbExecuted");
		long cb2;
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
