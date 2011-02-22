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

//
// Try to write_read a scalar attribute
//

	try
	{
		DeviceAttribute din("Short_attr_rw",(short)10);
		DeviceAttribute dout;

		dout = device->write_read_attribute(din);

		vector<short> vs;
		dout >> vs;

		assert (vs.size() == 2);
		assert (vs[0] == 10);
		assert (vs[1] == 10);
	}
	catch(DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	cout << "  Write_Read SCALAR attribute --> OK" << endl;

//
// Try to write_read a spectrum attribute
//

	try
	{
		vector<short> sent;
		sent.push_back(11);
		sent.push_back(22);

		DeviceAttribute din("Short_spec_attr_rw",sent);
		DeviceAttribute dout;

		dout = device->write_read_attribute(din);

		vector<short> vs;
		dout >> vs;

		assert (vs.size() == 4);
		assert (vs[0] == 8);
		assert (vs[1] == 9);
		assert (vs[2] == 11);
		assert (vs[3] == 22);
	}
	catch(DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	cout << "  Write_Read SPECTRUM attribute --> OK" << endl;

//
// Try to write_read an image attribute
//

	try
	{
		vector<short> sent;
		sent.push_back(111);
		sent.push_back(222);
		sent.push_back(333);

		DeviceAttribute din("Short_ima_attr_rw",sent);
		DeviceAttribute dout;

		dout = device->write_read_attribute(din);

		vector<short> vs;
		dout >> vs;

		assert (vs.size() == 7);
		assert (vs[0] == 6);
		assert (vs[1] == 7);
		assert (vs[2] == 8);
		assert (vs[3] == 9);
		assert (vs[4] == 111);
		assert (vs[5] == 222);
		assert (vs[6] == 333);
	}
	catch(DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	cout << "  Write_Read IMAGE attribute --> OK" << endl;

//
// Try to write a non writable attribute
//

	bool devfailed_except = false;
	string except_reason;
	try
	{
		DeviceAttribute din("Short_spec_attr",(short)10);
		DeviceAttribute dout;
		dout = device->write_read_attribute(din);
	}
	catch (DevFailed &e)
	{
		devfailed_except = true;
		except_reason = e.errors[0].reason.in();
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	assert (devfailed_except == true);
	assert (except_reason == "API_IncompatibleAttrDataType");

	cout << "  Some basic exception cases --> OK" << endl;
	
	delete device;
	return 0;	
}
