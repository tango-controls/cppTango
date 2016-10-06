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

	try
	{	
		device->poll_command("IOPollStr1",500);
		device->poll_command("IOPollArray2",500);
		device->poll_command("IOExcept",2000);
		device->poll_command("OEncoded",500);
		device->poll_command("State",2000);
		device->poll_command("Status",2000);

		device->poll_attribute("PollLong_attr",1000);
		device->poll_attribute("PollString_spec_attr",500);
		device->poll_attribute("attr_wrong_type",500);

		device->poll_attribute("Long64_attr_rw",500);
		device->poll_attribute("ULong_spec_attr_rw",500);
		device->poll_attribute("ULong64_attr_rw",500);
		device->poll_attribute("State_spec_attr_rw",500);
		device->poll_attribute("Encoded_attr",500);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
		
	delete device;	

}
