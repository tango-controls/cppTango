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
	
	if (argc != 3)
	{
		cout << "usage: %s device loop" << endl;
		exit(-1);
	}

	string device_name = argv[1];
	int loop = atoi(argv[2]);

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

	int i;


// Write a short

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute din,dout;
		short in = 2;
		din << in;
		din.set_name("Short_attr_w");
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Short_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		short received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Short --> OK" << endl;

// Write a short (with DeviceAttribute constructor)

	for (i = 0;i < loop;i++)
	{
		short in = 3;
		string att_name("Short_attr_w");
		DeviceAttribute din(att_name,in);
		DeviceAttribute dout;
		
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Short_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		short received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Short (with DeviceAttribute constructor) --> OK" << endl;
		
// Write a long

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute din,dout;
		long in = 3;
		din << in;
		din.set_name("Long_attr_w");
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Long_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		long received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Long --> OK" << endl;
	
// Write a long (with DeviceAttribute constructor)

	for (i = 0;i < loop;i++)
	{
		long in = 2;
		string att_name("Long_attr_w");
		DeviceAttribute din(att_name,in);
		DeviceAttribute dout;
		
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Long_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		long received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Long (with DeviceAttribute constructor) --> OK" << endl;
	
// Write a double

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute din,dout;
		double in = 3.2;
		din << in;
		din.set_name("Double_attr_w");
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Double_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		double received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Double --> OK" << endl;
	
// Write a double (with DeviceAttribute constructor)

	for (i = 0;i < loop;i++)
	{
		double in = 1.234;
		string att_name("Double_attr_w");
		DeviceAttribute din(att_name,in);
		DeviceAttribute dout;
		
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Double_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		double received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Double (with DeviceAttribute constructor) --> OK" << endl;
	
// Write a string

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute din,dout;
		string in("OK ?");
		din << in;
		din.set_name("String_attr_w");
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("String_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		string received;
		dout >> received;
		assert( received == in );
	}
	cout << "   String --> OK" << endl;
	
// Write a string (with DeviceAttribute constructor)

	for (i = 0;i < loop;i++)
	{
		string in("Burp");
		string att_name("String_attr_w");
		DeviceAttribute din(att_name,in);
		DeviceAttribute dout;
		
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("String_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		string received;
		dout >> received;
		assert( received == in );
	}
	cout << "   String (with DeviceAttribute constructor) --> OK" << endl;

// Write a float

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute din,dout;
		float in = 4.5;
		din << in;
		din.set_name("Float_attr_w");
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Float_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		float received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Float --> OK" << endl;

// Write a float (with DeviceAttribute constructor)

	for (i = 0;i < loop;i++)
	{
		float in = 16.5;
		string att_name("Float_attr_w");
		DeviceAttribute din(att_name,in);
		DeviceAttribute dout;
		
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Float_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		float received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Float (with DeviceAttribute constructor) --> OK" << endl;

// Write a boolean

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute din,dout;
		bool in = true;
		din << in;
		din.set_name("Boolean_attr_w");
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Boolean_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		bool received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Boolean --> OK" << endl;

// Write a boolean (with DeviceAttribute constructor)

	for (i = 0;i < loop;i++)
	{
		bool in = false;
		string att_name("Boolean_attr_w");
		DeviceAttribute din(att_name,in);
		DeviceAttribute dout;
		
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Boolean_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		bool received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Boolean (with DeviceAttribute constructor) --> OK" << endl;

// Write an unsigned short

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute din,dout;
		unsigned short in = 222;
		din << in;
		din.set_name("UShort_attr_w");
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("UShort_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		unsigned short received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Unsigned Short --> OK" << endl;

// Write an unsigned short (with DeviceAttribute constructor)

	for (i = 0;i < loop;i++)
	{
		unsigned short in = 33;
		string att_name("UShort_attr_w");
		DeviceAttribute din(att_name,in);
		DeviceAttribute dout;
		
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("UShort_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		unsigned short received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Unsigned Short (with DeviceAttribute constructor) --> OK" << endl;

// Write an unsigned char

	for (i = 0;i < loop;i++)
	{
		DeviceAttribute din,dout;
		unsigned char in = 250;
		din << in;
		din.set_name("UChar_attr_w");
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("UChar_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		unsigned char received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Unsigned Char --> OK" << endl;

// Write an unsigned char (with DeviceAttribute constructor)

	for (i = 0;i < loop;i++)
	{
		unsigned char in = 3;
		string att_name("UChar_attr_w");
		DeviceAttribute din(att_name,in);
		DeviceAttribute dout;
		
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("UChar_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		unsigned char received;
		dout >> received;
		assert( received == in );
	}
	cout << "   Unsigned Char (with DeviceAttribute constructor) --> OK" << endl;

//
// Write one non-writable attribute
//

	double db_in = 0.0;
	bool devfailed = false;
	string except_reason;
	DeviceAttribute din("Double_attr",db_in);
	try
	{
		device->write_attribute(din);
	}
	catch (Tango::DevFailed &e)
	{
		devfailed = true;
		except_reason = e.errors[0].reason;
	}
	catch (CORBA::Exception &)
	{
	}
	
	assert (devfailed == true);
	assert (except_reason == "API_AttrNotWritable");
	
	cout << "   write_attribute() method with exception --> OK" << endl;
	
//
// One test with several attributes within the same call
//

	for (i = 0;i < loop;i++)
	{
		string in("Splash");
		double in_d = 1.234;
		string att_name_1("String_attr_w");
		string att_name_2("Double_attr_w");
		vector<DeviceAttribute> attr_in;
		attr_in.push_back(DeviceAttribute(att_name_1,in));
		attr_in.push_back(DeviceAttribute(att_name_2,in_d));
		DeviceAttribute dout_str;
		DeviceAttribute dout_d;
		
		try
		{
			device->write_attributes(attr_in);
			dout_str = device->read_attribute("String_attr_w");
			dout_d = device->read_attribute("Double_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		string received_str;
		dout_str >> received_str;
		assert( received_str == in );
		
		double received_d;		
		dout_d >> received_d;
		assert( received_d == in_d );
	}
	cout << "   write_attributes() method with several attributes --> OK" << endl;


/****************************************************************************
*
*		Testing exception cases
*
*****************************************************************************/

	short in_sh = 234;
	double in_d = 1.234;
	bool multi_except = false;
	vector<string> att_in_fault;
	vector<long> idx_att_in_fault;
	
	vector<DeviceAttribute> attr_in;
	attr_in.push_back(DeviceAttribute("Short_attr_w",in_sh));
	attr_in.push_back(DeviceAttribute("Double_attr",in_d));
	attr_in.push_back(DeviceAttribute("Double_attr_w",in_d));
	attr_in.push_back(DeviceAttribute("Double_attr",in_d));
		
	try
	{
		device->write_attributes(attr_in);
	}
	catch (NamedDevFailedList &e)
	{
		multi_except = true;
		for (i = 0;i < e.err_list.size();i++)
		{
			att_in_fault.push_back(e.err_list[i].name);
			idx_att_in_fault.push_back(e.err_list[i].idx_in_call);
		}
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	assert (multi_except == true);
	assert (att_in_fault.size() == 2);
	assert (att_in_fault[0] == "Double_attr");		
	assert (att_in_fault[1] == "Double_attr");		
	assert (idx_att_in_fault.size() == 2);
	assert (idx_att_in_fault[0] == 1);		
	assert (idx_att_in_fault[1] == 3);
	
	cout << "   write_attributes() method with several attributes and exception --> OK" << endl;
	
	delete device;
	return 0;	
}
