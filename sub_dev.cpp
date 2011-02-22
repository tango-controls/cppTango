/* 
 * Test module for sub device diagnostics in the Tango API.
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
	DeviceProxy *admin;
	
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
		// be sure that the device name are lower case letters
		std::transform(device_name.begin(), device_name.end(), 
                       device_name.begin(), ::tolower);
		
		// connect to device
		device = new DeviceProxy(device_name);
		
		// Connect to admin device
		string adm_name = device->adm_name();
		admin = new DeviceProxy(adm_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(1);
	}

	cout << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;

	try
	{
	
// restart server to clean all sub device lists
		
		bool except = false;
		try
		{
			admin->command_inout("RestartServer");
		}
		catch (Tango::DevFailed &e)
		{
			Except::print_exception(e);
			except = true;
		}

		assert ( except == false );
		cout << "  Server restart to clean sub device lists --> OK" << endl;
		sleep(3);

// read attribute to have a sub device in the list

		{
			DeviceAttribute da = device->read_attribute("Sub_device_tst");
			coutv << da << endl;
			bool att_value;
			da >> att_value;
			assert (att_value == true);
		}
		
// check the list of sub devices on the administration device

		{
			DeviceData dd = admin->command_inout("QuerySubDevice");
			vector<string> sub_dev_list;
			dd >> sub_dev_list;

			string result = device_name + " dev/test/11";
			assert( sub_dev_list.size() == 1);
			assert( sub_dev_list[0]     == result);

			cout << "  Add sub device in attribute method --> OK" << endl;
		}

// execute command to add sub devices in the list
		
		{
			DeviceData dd = device->command_inout("SubDeviceTst");
			coutv << dd << endl;
			bool cmd_value;
			dd >> cmd_value;
			assert (cmd_value == true);
			
			// let the external thread some time to do its work!
			sleep(1);
		}
		
// check the list of sub devices on the administration device

		{
			DeviceData dd = admin->command_inout("QuerySubDevice");
			vector<string> sub_dev_list;
			dd >> sub_dev_list;

			assert( sub_dev_list.size() == 3);
			
			string result = "dev/test/11";
			assert( sub_dev_list[0]     == result);
			result = device_name + " dev/test/11";
			assert( sub_dev_list[1]     == result);
			result = device_name + " dev/test/12";
			assert( sub_dev_list[2]     == result);

			cout << "  Add sub devices in command method and external thread --> OK" << endl;
		}
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	delete admin;
	delete device;
	return 0;
	
}
