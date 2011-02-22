/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc != 2)
	{
		cout << "usage: %s device" << endl;
		exit(-1);
	}

	string device_name = argv[1];

	try 
	{
		device = new DeviceProxy(device_name);
		cout << "new DeviceProxy() returned" << endl;

		device->set_timeout_millis(1000);
	}
        catch (CORBA::Exception &e)
        {
              	Except::print_exception(e);
		exit(1);
        }


	while (1)
	{
		cout << "Hit any key :" << endl;
		char bid;
		cin >> bid;

		try
		{	
//			device->ping();
//			cout << "Ping successfull, device IDL version = ";
//			cout << device->get_idl_version() << endl;
			device->command_inout("Status");
			cout << "command_inout successfull" << endl;
		}
		catch (Tango::CommunicationFailed &e)
		{
			cerr << "Commnunication Failed exception" << endl;
			Except::print_exception(e);
		}
		catch (Tango::ConnectionFailed &e)
		{
			cerr << "Connection Failed exception" << endl;
			Except::print_exception(e);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
		}
		catch (...)
		{
			cout << "Unknown exception" << endl;
		}
		
	}
		
	return 0;

}
