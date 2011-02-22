/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#define	coutv	if (verbose == true) cout
		    
using namespace Tango;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	bool verbose = false;
	
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

	
	try
	{
	
// Change timeout (with a useless name as a workaround for ORBacus bug)

//		device->set_timeout_millis(6000);
//		device->name();			
//		coutv << "Timeout set to 6 seconds" << endl;
				
// Send a command in fire and forget mode

		long id;

		char key;		
		cout << "Hit any key : ";
		cin >> key;

		id = device->command_inout_asynch("State",true);
		
		assert (id == 0);
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
