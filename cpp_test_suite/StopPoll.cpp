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
		device->stop_poll_command("IOPollStr1");
		device->stop_poll_command("IOPollArray2");
		device->stop_poll_command("IOExcept");
		device->stop_poll_command("State");
		device->stop_poll_command("Status");
		
		device->stop_poll_attribute("PollLong_attr");
		device->stop_poll_attribute("PollString_spec_attr");
		device->stop_poll_attribute("attr_wrong_type");
		
		device->stop_poll_attribute("Long64_attr_rw");
		device->stop_poll_attribute("ULong_spec_attr_rw");
		device->stop_poll_attribute("ULong64_attr_rw");
		device->stop_poll_attribute("State_spec_attr_rw");
		device->stop_poll_attribute("Encoded_attr");
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
		
	delete device;	

}
