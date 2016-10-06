/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#define	coutv	if (verbose == true) cout

bool verbose = false;

using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if ((argc == 1) || (argc > 3))
	{
		cout << "usage: %s device [-v] " << endl;
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

	cout << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;

	try
	{	
		device->command_inout("State");				
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	return 0;
	
}
