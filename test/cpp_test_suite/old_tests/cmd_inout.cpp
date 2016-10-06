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
	}
        catch (CORBA::Exception &e)
        {
              	Except::print_exception(e);
		exit(1);
        }

	cout << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;

for (int i = 0;i < 5;i++)
{
	DeviceData da;
	try
	{	
		da = device->command_inout("Status");
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	cout << "Command successfull" << endl;
	string str;
	da >> str;
	cout << "Command result = " << str << endl;
}
		
	delete device;

//	sleep(1000);

		

}
