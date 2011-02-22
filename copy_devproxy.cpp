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

	string device_name("dev/test/10");

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

// Test copy constructor

		DeviceProxy dev2(*device);
		
		assert (dev2.name() == "dev/test/10");
#ifndef COMPAT
		assert (dev2.get_idl_version() == 4);
#endif
						
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	cout << "   Copy constructor --> OK" << endl;
	
// Test assignement operator

	DeviceProxy dev2("dev/test/11");
	DeviceProxy dev3("dev/test/12");

	dev3 = *device;
	
	assert (dev3.name() == "dev/test/10");
#ifndef COMPAT
	assert (dev3.get_idl_version() == 4);
#endif
	
	cout << "   Assignement operator --> OK" << endl;
				
	delete device;
	return 0;
}
