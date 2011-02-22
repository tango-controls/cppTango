/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#define	coutv	if (verbose == true) cout

bool verbose = false;

using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if ((argc == 1) || (argc > 3))
	{
		cout << "usage: %s device [-v] " << endl;
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

	cout << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;


//**************************************************************************
//
//		Test for a short attribute
//
//**************************************************************************



	try
	{
	
		string short_att_name("Short_attr_w");
		string string_att_name("String_attr_w");
		string bool_att_name("Boolean_attr_w");
		
// Write  these attributes

		short sh = 345;
		string str("Do you want to dance?");
		bool bo = true;
		
		vector<DeviceAttribute> v_da;
		v_da.push_back(DeviceAttribute(short_att_name,sh));
		v_da.push_back(DeviceAttribute(string_att_name,str));
		v_da.push_back(DeviceAttribute(bool_att_name,bo));
		
		device->write_attributes(v_da);
		
// Restart the server

		string adm_name = device->adm_name();
		DeviceProxy adm_dev(adm_name);

#ifdef VALGRIND
		adm_dev.set_timeout_millis(15000);
#endif		
		adm_dev.command_inout("RestartServer");
		
		struct timespec sle;
		sle.tv_sec = 3;
		sle.tv_nsec = 500000000;
		nanosleep(&sle,NULL);
		
		delete device;
		device = new DeviceProxy(device_name);
				
// Read attributes value

		vector<string> ra;
		ra.push_back(short_att_name);
		ra.push_back(string_att_name);
		ra.push_back(bool_att_name);
		
		vector<DeviceAttribute> *r_att;
		
		r_att = device->read_attributes(ra);
		
		short read_sh;
		string read_str;
		bool read_bo;
		
		(*r_att)[0] >> read_sh;
		(*r_att)[1] >> read_str;
		(*r_att)[2] >> read_bo;
		
		coutv << "Read value for Short_attr_w = " << read_sh << endl;
		coutv << "Read value for String_attr_w = " << read_str << endl;
		coutv << "Read value for Boolean_attr_w = " << read_bo << endl;

		assert ( read_sh == sh );
		assert ( read_str == str );
		assert ( read_bo == bo );
		
		cout << "   Memorized attributes --> OK" << endl;
							
// Reset the boolean attribute which is part of the device server
// output message taken into account in the automatic sequence


		DeviceAttribute  da_bool("Boolean_attr_w",false);

		device->write_attribute(da_bool);
	}
	catch (Tango::DevFailed &e)
	{
              	Except::print_exception(e);
		exit(1);
        }
	
	delete device;		
	return 0;
	
}
