/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc != 3)
	{
		cout << "usage: %s device loop" << endl;
		exit(-1);
	}

	string device_name = argv[1];


	try 
	{
		device = new DeviceProxy(device_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(1);
	}

	int ctr = atoi(argv[2]);

	while (ctr > 0)
	{
		try
		{
			device->state();
			ctr = 0;
		}
		catch (Tango::DevFailed &e)
		{
			string reason(e.errors[0].reason.in());
			if (reason != API_DeviceNotExported && reason != API_CorbaException)
			{
				Except::print_exception(e);
				exit(-1);
			}
			else
			{
				ctr--;
				if (ctr == 0)
					exit(-1);
				cout << "Multicast device not ready, waiting for 30 sec" << endl;
				Tango_sleep(30);
			}
		}
	}
		
	delete device;	
	return 0;
}
