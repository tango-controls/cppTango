/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#define	coutv	if (verbose == true) cout

using namespace Tango;
using namespace std;

bool verbose = false;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc == 1)
	{
		cout << "usage: %s device [-v]" << endl;
		exit(-1);
	}

	string device_name = argv[1];

	if (argc == 3)
	{
		if (strcmp(argv[2],"-v") == 0)
			verbose = true;
	}
	
	try 
	{
		device = new DeviceProxy(device_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(1);
	}

	coutv << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;

	try
	{

		device->set_source(Tango::CACHE);
			
// Test getting a long 64 attribute from polling buffer

		DeviceAttribute da;
		try
		{	
			da = device->read_attribute("Long64_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevLong64 lo;
		da >> lo;
		int data_type = da.get_type();
		coutv << "64 bits attribute data = " << hex << lo << dec << endl;
		assert ( lo == 0x800000000LL );
		assert ( data_type == Tango::DEV_LONG64 );
		
		cout << "   Scalar long 64 bits from CACHE buffer --> OK" << endl;

// Test getting a unsigned long 64 bits attribute from polling buffer

		DeviceAttribute da_ulo;
		try
		{	
			da_ulo = device->read_attribute("ULong64_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		DevULong64 ulo;
		da_ulo >> ulo;
		int data_type_ulo = da_ulo.get_type();
		coutv << "unsigned 64 bits attribute data = " << hex << ulo << dec << endl;
		assert ( ulo == 0xC000000000000000LL );
		assert ( data_type_ulo == Tango::DEV_ULONG64 );
		
		cout << "   Scalar unsigned long 64 bits from CACHE buffer --> OK" << endl;
		
// Test getting a unsigned long spectrum attribute from polling buffer

		bool ret;		
		try
		{	
			da = device->read_attribute("ULong_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<DevULong> v_lo;
		ret = (da >> v_lo);
		
		assert (ret == true);
		assert ( v_lo[0] == 2222 );
		assert ( v_lo[1] == 22222 );
		assert ( v_lo[2] == 222222 );
		
		cout << "   Spectrum unsigned long from CACHE buffer --> OK" << endl;
		
// Test getting a state spectrum attribute from polling buffer
		
		try
		{	
			da = device->read_attribute("State_spec_attr_rw");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<DevState> v_sta;
		ret = (da >> v_sta);
		
		assert (ret == true);
		assert ( v_sta[0] == Tango::ON );
		assert ( v_sta[1] == Tango::OFF );
		
		cout << "   Spectrum state from CACHE buffer --> OK" << endl;
		
// Test getting a DevEncoded attribute from polling buffer

#ifndef COMPAT		
		try
		{	
			da = device->read_attribute("Encoded_attr");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		
		DevEncoded enc_lo;
		da >> enc_lo;
		data_type = da.get_type();
		assert ( ::strcmp(enc_lo.encoded_format.in(),"Which format?") == 0 );
		assert ( data_type == Tango::DEV_ENCODED );
		assert ( enc_lo.encoded_data.length() == 4 );
		assert ( enc_lo.encoded_data[0] == 97 );
		assert ( enc_lo.encoded_data[1] == 98 );
		assert ( enc_lo.encoded_data[2] == 99 );
		assert ( enc_lo.encoded_data[3] == 100 );
		
		cout << "   DevEncoded attribute from CACHE buffer --> OK" << endl;
#endif
		
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
			
	delete device;
	return 0;
}
