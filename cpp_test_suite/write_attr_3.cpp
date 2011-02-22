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

/******************************************************************************
*
*		First check some exception cases
*
*******************************************************************************/

// Try to write a non writable attribute

	bool devfailed_except = false;
	string except_reason;
	try
	{
		DeviceAttribute din("Short_spec_attr",(short)10);
		device->write_attribute(din);
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
	assert (except_reason == "API_AttrNotWritable");
		
// Send too many data

	devfailed_except = false;
	try
	{
		vector<short> v1(10,2);
		DeviceAttribute din("Short_spec_attr_w",v1);
		device->write_attribute(din);
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
	assert (except_reason == "API_WAttrOutsideLimit");

// Send data above the max_value

	devfailed_except = false;
	try
	{
		vector<short> v1;
		v1.push_back(10);
		v1.push_back(1200);
		DeviceAttribute din("Short_spec_attr_w",v1);
		device->write_attribute(din);
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
	assert (except_reason == "API_WAttrOutsideLimit");

// Send incorrect data number (mainly for image)

	devfailed_except = false;
	try
	{
		vector<short> v1;
		v1.push_back(10);
		v1.push_back(1200);
		DeviceAttribute din("Short_ima_attr_w",v1,2,2);
		device->write_attribute(din);
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
	assert (except_reason == "API_AttrIncorrectDataNumber");
	
	cout << "   Exception cases --> OK" << endl;

/******************************************************************************
*
*		Write and read spectrum attribute for WRITE attributes
*
*******************************************************************************/

// A short spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<short> vin,vout;
		vin.push_back(10);
		vin.push_back(22);
		vin.push_back(33);
		
		DeviceAttribute dout;
		DeviceAttribute din("Short_spec_attr_w",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Short_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum short (WRITE attribute) --> OK" << endl;
	
	for (i = 0;i < loop;i++)
	{
		vector<short> vin,vout;
		vin.push_back(11);
		vin.push_back(24);
		vin.push_back(46);
		
		DeviceAttribute dout;
		DeviceAttribute din;
		din.set_name("Short_spec_attr_w");
		din << vin;
		
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Short_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum short by vector (WRITE attribute) --> OK" << endl;

// A long spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<DevLong> vin,vout;
		vin.push_back(1000);
		vin.push_back(2222);
		
		DeviceAttribute dout;
		DeviceAttribute din("Long_spec_attr_w",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Long_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum long (WRITE attribute) --> OK" << endl;
	
	for (i = 0;i < loop;i++)
	{
		vector<DevLong> vin,vout;
		vin.push_back(1111);
		vin.push_back(3333);
		
		DeviceAttribute dout;
		DeviceAttribute din;
		din.set_name("Long_spec_attr_w");
		din << vin;
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Long_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum long by vector (WRITE attribute) --> OK" << endl;

// A double spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<double> vin,vout;
		vin.push_back(2.34);
		vin.push_back(5.66);
		
		DeviceAttribute dout;
		DeviceAttribute din("Double_spec_attr_w",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Double_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum double (WRITE attribute) --> OK" << endl;
	
	for (i = 0;i < loop;i++)
	{
		vector<double> vin,vout;
		vin.push_back(5.66);
		vin.push_back(8.99);
		
		DeviceAttribute dout;
		DeviceAttribute din;
		din.set_name("Double_spec_attr_w");
		din << vin;
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Double_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum double by vector (WRITE attribute) --> OK" << endl;

// A string spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<string> vin,vout;
		vin.push_back("Hello girl");
		vin.push_back("You look nice, don't you");
		
		DeviceAttribute dout;
		DeviceAttribute din("String_spec_attr_w",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("String_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum string (WRITE attribute) --> OK" << endl;
	
	for (i = 0;i < loop;i++)
	{
		vector<string> vin,vout;
		vin.push_back("Hello girl");
		vin.push_back("You look nice, don't you");
		
		DeviceAttribute dout;
		DeviceAttribute din;
		din.set_name("String_spec_attr_w");
		din << vin;
		
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("String_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum string by vector (WRITE attribute) --> OK" << endl;

// A float spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<float> vin,vout;
		vin.push_back(16.5);
		vin.push_back(2.75);
		vin.push_back(4.5);
		
		DeviceAttribute dout;
		DeviceAttribute din("Float_spec_attr_w",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Float_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum float (WRITE attribute) --> OK" << endl;
	
// A boolean spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<bool> vin,vout;
		vin.push_back(true);
		vin.push_back(true);
		vin.push_back(false);
		
		DeviceAttribute dout;
		DeviceAttribute din("Boolean_spec_attr_w",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Boolean_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum boolean (WRITE attribute) --> OK" << endl;
	
	for (i = 0;i < loop;i++)
	{
		vector<bool> vin,vout;
		vin.push_back(true);
		vin.push_back(true);
		vin.push_back(false);
		
		DeviceAttribute dout;
		DeviceAttribute din;
		din.set_name("Boolean_spec_attr_w");
		din << vin;
		
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Boolean_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum boolean by vector (WRITE attribute) --> OK" << endl;

// A unsigned short spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<unsigned short> vin,vout;
		vin.push_back(100);
		vin.push_back(220);
		
		DeviceAttribute dout;
		DeviceAttribute din("UShort_spec_attr_w",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("UShort_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum unsigned short (WRITE attribute) --> OK" << endl;
	
	for (i = 0;i < loop;i++)
	{
		vector<unsigned short> vin,vout;
		vin.push_back(100);
		vin.push_back(220);
		
		DeviceAttribute dout;
		DeviceAttribute din;
		din.set_name("UShort_spec_attr_w");
		din << vin;
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("UShort_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum unsigned short by vector (WRITE attribute) --> OK" << endl;		

// A unsigned char spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<unsigned char> vin,vout;
		vin.push_back(100);
		vin.push_back(220);
		vin.push_back(44);
		vin.push_back(66);
		
		DeviceAttribute dout;
		DeviceAttribute din("UChar_spec_attr_w",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("UChar_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum unsigned char (WRITE attribute) --> OK" << endl;
	
	for (i = 0;i < loop;i++)
	{
		vector<unsigned char> vin,vout;
		vin.push_back(100);
		vin.push_back(220);
		vin.push_back(44);
		vin.push_back(66);
		
		DeviceAttribute dout;
		DeviceAttribute din;
		din.set_name("UChar_spec_attr_w");
		din << vin;
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("UChar_spec_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write spectrum unsigned char by vector (WRITE attribute) --> OK" << endl;
	
// A long 64 bits spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<DevLong64> vin,vout;
		vin.push_back(10000);
		vin.push_back(22222);
		
		DeviceAttribute dout;
		DeviceAttribute din("Long64_spec_attr_rw",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Long64_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout[0] == 1000 );
		assert( vout[1] == 10000 );
		assert( vout[2] == 100000 );
		assert( vout[3] == 10000 );
		assert( vout[4] == 22222 );
	}
	cout << "   Write spectrum long 64 bits (READ/WRITE attribute) --> OK" << endl;
	
// A unsigned long spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<DevULong> vin,vout;
		vin.push_back(111);
		vin.push_back(7777);
		
		DeviceAttribute dout;
		DeviceAttribute din("ULong_spec_attr_rw",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("ULong_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout[0] == 2222 );
		assert( vout[1] == 22222 );
		assert( vout[2] == 222222 );
		assert( vout[3] == 111 );
		assert( vout[4] == 7777 );
	}
	cout << "   Write spectrum unsigned long (READ/WRITE attribute) --> OK" << endl;
	
// A unsigned long 64 bits spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<DevULong64> vin,vout;
		vin.push_back(111111);
		vin.push_back(777777);
		
		DeviceAttribute dout;
		DeviceAttribute din("ULong64_spec_attr_rw",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("ULong64_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout[0] == 8888 );
		assert( vout[1] == 88888 );
		assert( vout[2] == 888888 );
		assert( vout[3] == 111111 );
		assert( vout[4] == 777777 );
	}
	cout << "   Write spectrum unsigned long 64 bits (READ/WRITE attribute) --> OK" << endl;
	
// A state spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<DevState> vin,vout;
		vin.push_back(Tango::STANDBY);
		vin.push_back(Tango::MOVING);
		
		DeviceAttribute dout;
		DeviceAttribute din("State_spec_attr_rw",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("State_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout[0] == Tango::ON );
		assert( vout[1] == Tango::OFF );
		assert( vout[2] == Tango::STANDBY );
		assert( vout[3] == Tango::MOVING );
	}
	cout << "   Write spectrum state (READ/WRITE attribute) --> OK" << endl;
	

	
/******************************************************************************
*
*		Write and read image attribute for WRITE attributes
*
*******************************************************************************/

// A short image 

	for (i = 0;i < loop;i++)
	{
		vector<short> vin,vout;
		vin.push_back(10);
		vin.push_back(22);
		vin.push_back(33);
		vin.push_back(44);
		vin.push_back(55);
		vin.push_back(66);
		
		DeviceAttribute dout;
		DeviceAttribute din("Short_ima_attr_w",vin,3,2);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Short_ima_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write image short (WRITE attribute) --> OK" << endl;

// A string image 

	for (i = 0;i < loop;i++)
	{
		vector<string> vin,vout;
		vin.push_back("The sky is blue");
		vin.push_back("The sun is shinning");
		vin.push_back("The sea is not too cold");
		vin.push_back("Girls on the beach are pretty");
		
		DeviceAttribute dout;
		DeviceAttribute din("String_ima_attr_w",vin,2,2);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("String_ima_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write image string (WRITE attribute) --> OK" << endl;

// A float image 

	for (i = 0;i < loop;i++)
	{
		vector<float> vin,vout;
		vin.push_back(10.5);
		vin.push_back(22.5);
		vin.push_back(33.5);
		vin.push_back(44.5);
		vin.push_back(55.5);
		vin.push_back(66.5);
		
		DeviceAttribute dout;
		DeviceAttribute din("Float_ima_attr_w",vin,3,2);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Float_ima_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write image float (WRITE attribute) --> OK" << endl;

// A unsigned short image 

	for (i = 0;i < loop;i++)
	{
		vector<unsigned short> vin,vout;
		vin.push_back(105);
		vin.push_back(225);
		vin.push_back(335);
		vin.push_back(445);
		vin.push_back(555);
		vin.push_back(665);
		vin.push_back(775);
		vin.push_back(995);
		
		DeviceAttribute dout;
		DeviceAttribute din("UShort_ima_attr_w",vin,4,2);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("UShort_ima_attr_w");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		assert( vout == vin );
	}
	cout << "   Write image unsigned short (WRITE attribute) --> OK" << endl;
				
/******************************************************************************
*
*		Write and read spectrum attribute for READ_WRITE attributes
*
*******************************************************************************/

// A short spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<short> vin,vout;
		vin.push_back(66);
		vin.push_back(77);
		vin.push_back(88);
		
		DeviceAttribute dout;
		DeviceAttribute din("Short_spec_attr_rw",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Short_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		vin.insert(vin.begin(),9);
		vin.insert(vin.begin(),8);
		assert( vout == vin );
	}
	cout << "   Write spectrum short (READ_WRITE attribute) --> OK" << endl;

// A string spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<string> vin,vout;
		vin.push_back("Neron is burning");
		vin.push_back("The weather is warm");
		
		DeviceAttribute dout;
		DeviceAttribute din("String_spec_attr_rw",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("String_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		vin.insert(vin.begin(),"It's friday");
		vin.insert(vin.begin(),"Thank's god");

		assert( vout == vin );
	}
	cout << "   Write spectrum string (READ_WRITE attribute) --> OK" << endl;

// A float spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<float> vin,vout;
		vin.push_back(66.0);
		vin.push_back(77.0);
		vin.push_back(88.0);
		
		DeviceAttribute dout;
		DeviceAttribute din("Float_spec_attr_rw",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Float_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		vin.insert(vin.begin(),11.5);
		vin.insert(vin.begin(),5.5);
		assert( vout == vin );
	}
	cout << "   Write spectrum float (READ_WRITE attribute) --> OK" << endl;
	
// A unsigned char spectrum 

	for (i = 0;i < loop;i++)
	{
		vector<unsigned char> vin,vout;
		vin.push_back(63);
		vin.push_back(73);
		vin.push_back(83);
		
		DeviceAttribute dout;
		DeviceAttribute din("UChar_spec_attr_rw",vin);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("UChar_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		vin.insert(vin.begin(),66);
		vin.insert(vin.begin(),44);
		vin.insert(vin.begin(),22);
		assert( vout == vin );
	}
	cout << "   Write spectrum unsigned char (READ_WRITE attribute) --> OK" << endl;
		
/******************************************************************************
*
*		Write and read image attribute for READ_WRITE attributes
*
*******************************************************************************/

// A short image 

	for (i = 0;i < loop;i++)
	{
		vector<short> vin,vout;
		vin.push_back(10);
		vin.push_back(22);
		vin.push_back(33);
		vin.push_back(44);
		vin.push_back(55);
		vin.push_back(66);
		
		DeviceAttribute dout;
		DeviceAttribute din("Short_ima_attr_rw",vin,3,2);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Short_ima_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		vin.insert(vin.begin(),9);
		vin.insert(vin.begin(),8);
		vin.insert(vin.begin(),7);
		vin.insert(vin.begin(),6);
		
		AttributeDimension r_d = dout.get_r_dimension();
		AttributeDimension w_d = dout.get_w_dimension();
		
		assert(r_d.dim_x == 2);
		assert(r_d.dim_y == 2);
		assert(w_d.dim_x == 3);
		assert(w_d.dim_y == 2);
		
		assert( vout == vin );
	}
	cout << "   Write image short (READ_WRITE attribute) --> OK" << endl;

// A string image 

	for (i = 0;i < loop;i++)
	{
		vector<string> vin,vout;
		vin.push_back("Alors quoi");
		vin.push_back("ca va finir par marcher");
		vin.push_back("cette saloperie de soft");
		vin.push_back("Non mais");
		
		DeviceAttribute dout;
		DeviceAttribute din("String_ima_attr_rw",vin,2,2);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("String_ima_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		vin.insert(vin.begin(),"c'est une surprise");
		vin.insert(vin.begin(),"Alors la, pour une surprise");

		assert( vout == vin );
	}
	cout << "   Write image string (READ_WRITE attribute) --> OK" << endl;
			
// A boolean image 

	for (i = 0;i < loop;i++)
	{
		vector<bool> vin,vout;
		vin.push_back(true);
		vin.push_back(true);
		vin.push_back(true);
		vin.push_back(false);
		vin.push_back(false);
		vin.push_back(false);
		
		DeviceAttribute dout;
		DeviceAttribute din("Boolean_ima_attr_rw",vin,3,2);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("Boolean_ima_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		vin.insert(vin.begin(),false);
		vin.insert(vin.begin(),true);
		
		AttributeDimension r_d = dout.get_r_dimension();
		AttributeDimension w_d = dout.get_w_dimension();
		
		assert(r_d.dim_x == 2);
		assert(r_d.dim_y == 1);
		assert(w_d.dim_x == 3);
		assert(w_d.dim_y == 2);
		
// Strangi behaviour with CC 5.5 oin kidiboo. This loop works well if you run it
// 3 times  but fails if you rin it forth. Replace the vector equality test by a
// vector element equality test (20/07/05)
//		assert( vout == vin );
		assert(vin[0] == vout[0]);
		assert(vin[1] == vout[1]);
		assert(vin[2] == vout[2]);
		assert(vin[3] == vout[3]);
		assert(vin[4] == vout[4]);
		assert(vin[5] == vout[5]);
	}
	cout << "   Write image boolean (READ_WRITE attribute) --> OK" << endl;
	
// A insigned short image 

	for (i = 0;i < loop;i++)
	{
		vector<unsigned short> vin,vout;
		vin.push_back(111);
		vin.push_back(222);
		vin.push_back(333);
		vin.push_back(555);
		vin.push_back(666);
		vin.push_back(777);
		
		DeviceAttribute dout;
		DeviceAttribute din("UShort_ima_attr_rw",vin,2,3);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("UShort_ima_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> vout;
		vin.insert(vin.begin(),3);
		vin.insert(vin.begin(),2);
		
		AttributeDimension r_d = dout.get_r_dimension();
		AttributeDimension w_d = dout.get_w_dimension();
		
		assert(r_d.dim_x == 2);
		assert(r_d.dim_y == 1);
		assert(w_d.dim_x == 2);
		assert(w_d.dim_y == 3);
		
		assert( vout == vin );
	}
	cout << "   Write image unsigned short (READ_WRITE attribute) --> OK" << endl;

// The user code a set_write_value in its write_xxx method

	{
		DeviceAttribute dout;
		DeviceAttribute din("ULong_attr_rw",(DevULong)12);
		try
		{	
			device->write_attribute(din);
			dout = device->read_attribute("ULong_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		
		DevVarULongArray *ula;
		dout >> ula;
	
		assert(ula->length() == 2);
		assert((*ula)[1] == 12);
		
		DeviceAttribute din2("ULong_attr_rw",(DevULong)1200);
		try
		{	
			device->write_attribute(din2);
			dout = device->read_attribute("ULong_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		
		dout >> ula;
		
		assert(ula->length() == 2);
		assert((*ula)[1] == 1111);
	}
	cout << "   User code a set_write_value in its write_xxx method --> OK" << endl;
	
		
	delete device;
	return 0;	
}
