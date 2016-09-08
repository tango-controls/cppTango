/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc < 2)
	{
		cout << "Usage: %s device_name" << endl;
		exit(-1);
	}

	string device_name(argv[1]);
			

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
	
	while (true)
	{	
		try
		{
	
// Test polling_status

			vector<string> *poll_str;
			poll_str = device->polling_status();

			cout << poll_str->size() << " object(s) polled for device" << endl;
//			cout << endl;
//			for (int i = 0;i < poll_str->size();i++)
//				cout << "Polling status = " << (*poll_str)[i] << endl;
			cout << endl;

			delete poll_str;
		}
		catch (Tango::DevFailed &e)
		{
			Except::print_exception(e);
		}
		
		cout << "Hit any key :" << endl;
		char bid;
		cin >> bid;		

	}
			
	delete device;
}
