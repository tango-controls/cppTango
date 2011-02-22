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

//	OBProxy_Tango::Device *d = dynamic_cast<OBProxy_Tango::Device *>(device->get_device().in());
//	long ref1 = d->_OB_getRef();
	
//	assert( ref1 == 1 );
		
	try
	{

// Test copy constructor

		DeviceProxy dev2(*device);
		
//		OBProxy_Tango::Device *d1 = dynamic_cast<OBProxy_Tango::Device *>(dev2.get_device().in());
//		long ref2 = d1->_OB_getRef();
		
//		assert ( ref2 == 2 );
		assert (dev2.name() == "dev/test/10");
		assert (dev2.get_idl_version() == 3);
//		assert (dev2.get_idl_version() == 2);
						
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

//	OBProxy_Tango::Device *d2 = dynamic_cast<OBProxy_Tango::Device *>(device->get_device().in());
//	long ref3 = d2->_OB_getRef();
	
//	assert (ref3 == 1);
	
	cout << "   Copy constructor --> OK" << endl;
	
// Test assignement operator

	DeviceProxy dev2("dev/test/11");
	DeviceProxy dev3("dev/test/12");
	
//	OBProxy_Tango::Device *d3 = dynamic_cast<OBProxy_Tango::Device *>(dev3.get_device().in());
//	long ref4 = d3->_OB_getRef();
//	assert ( ref4 == 1 );

	dev3 = *device;
	
	assert (dev3.name() == "dev/test/10");
	assert (dev3.get_idl_version() == 3);
//	assert (dev3.get_idl_version() == 2);
	
//	OBProxy_Tango::Device *d4 = dynamic_cast<OBProxy_Tango::Device *>(device->get_device().in());
//	long ref5 = d4->_OB_getRef();
//	assert ( ref5 == 2 );

//	OBProxy_Tango::Device *d5 = dynamic_cast<OBProxy_Tango::Device *>(dev3.get_device().in());
//	long ref6 = d5->_OB_getRef();
//	assert ( ref6 == 2 );
	
	cout << "   Assignement operator --> OK" << endl;
				
	delete device;
	return 0;
}
