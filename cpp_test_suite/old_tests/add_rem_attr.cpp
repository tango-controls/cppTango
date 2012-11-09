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

	try
	{
	
//
// Get attribute number
//

		vector<string> *att_list;
		att_list = device->get_attribute_list();
		long nb_att = att_list->size();
		
		coutv << "Device attribute number before add = " << nb_att << endl;

//
// Add one attribute to the device
//

		device->command_inout("IOAddAttribute");
		
//
// Get new attribute number
//

		vector<string> *att_list_add;
		att_list_add = device->get_attribute_list();
		long nb_att_add = att_list_add->size();
		
		coutv << "Device attribute number after add = " << nb_att_add << endl;
		assert (nb_att_add == (nb_att + 1));
		
//
// Redo it once, this should not change att number (attribute already added)
//

		device->command_inout("IOAddAttribute");
		
		vector<string> *att_list_add2;		
		att_list_add2 = device->get_attribute_list();
		long nb_att_add2 = att_list_add2->size();
		
		coutv << "Device attribute number after second add = " << nb_att_add2 << endl;
		assert (nb_att_add2 == (nb_att + 1));
		
		cout << "   Adding attribute --> OK" << endl;
		
//
// Remove the attribute
//

		device->command_inout("IORemoveAttribute");
		
		vector<string> *att_list_rem;		
		att_list_rem = device->get_attribute_list();
		long nb_att_rem = att_list_rem->size();
		
		coutv << "Device attribute number after removing add = " << nb_att_rem << endl;
		assert (nb_att_rem == nb_att);
		
//
// Do it once more. This time, it should send an exception
//

		bool except = false;
		try
		{
			device->command_inout("IORemoveAttribute");
		}
		catch (Tango::DevFailed &e)
		{
			coutv << "Received exception" << endl;
			except = true;
		}
		
		assert (except == true);
		
		cout << "   Removing attribute --> OK" << endl;		
						
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
		
	return 0;
	
}
