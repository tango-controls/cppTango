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
// Try to write_read different attribute in one go
//

	try
	{
		vector<DeviceAttribute> v_da;
		DeviceAttribute din("Short_attr_rw",(short)22);
		v_da.push_back(din);

		vector<DeviceAttribute> *dout;
		vector<string> r_name;
		r_name.push_back(string("Short_spec_attr_rw"));

		dout = device->write_read_attributes(v_da,r_name);

		vector<short> vs;
		(*dout)[0] >> vs;

		assert (vs.size() == 4);
		assert (vs[0] == 8);
		assert (vs[1] == 9);
		assert (vs[2] == 11);
		assert (vs[3] == 22);

		DeviceAttribute d_check = device->read_attribute("Short_attr_rw");
		vs.clear();
		d_check >> vs;

		assert (vs.size() == 2);
		assert (vs[0] == 22);
		assert (vs[1] == 22);

		delete dout;
	}
	catch(DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	cout << "  Simple write_read_attributes (1 W / 1 R) --> OK" << endl;

//
// Try to write_read different attribute (more elaborated case)
//

	try
	{
		vector<DeviceAttribute> v_da;
		DeviceAttribute din("Short_attr_rw",(short)55);
		DeviceAttribute din2("Long64_attr_rw",(DevLong64)555);
		v_da.push_back(din2);
		v_da.push_back(din);

		vector<DeviceAttribute> *dout;
		vector<string> r_name;
		r_name.push_back(string("String_attr"));
		r_name.push_back(string("Double_attr"));
		r_name.push_back(string("Short_spec_attr_rw"));

		dout = device->write_read_attributes(v_da,r_name);

		string str;
		double db;
		vector<short> v_s;

		(*dout)[0] >> str;
		(*dout)[1] >> db;
		(*dout)[2] >> v_s;

		assert (str == "test_string");
		assert (db == 3.2);
		assert (v_s.size() == 4);
		assert (v_s[0] == 8);
		assert (v_s[1] == 9);
		assert (v_s[2] == 11);
		assert (v_s[3] == 22);

		DeviceAttribute d_check = device->read_attribute("Short_attr_rw");
		v_s.clear();
		d_check >> v_s;

		assert (v_s.size() == 2);
		assert (v_s[0] == 55);
		assert (v_s[1] == 55);

		d_check = device->read_attribute("Long64_attr_rw");
		vector<DevLong64> v_dl;
		d_check >> v_dl;

		assert (v_dl.size() == 2);
		assert (v_dl[0] == 555);
		assert (v_dl[1] == 555);

		delete dout;
	}
	catch(DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	cout << "  Elaborated write_read_attributes (2 W / 3 R) --> OK" << endl;

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
	assert (except_reason == API_AttrNotWritable);

	cout << "  Some basic exception cases --> OK" << endl;
	
	delete device;
	return 0;	
}
