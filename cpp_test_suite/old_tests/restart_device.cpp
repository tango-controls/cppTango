/* 
 * Send the DevRestart command to the admin device of the
 * server in charg eof the device with name given as parameter.
 *
 * Possible return code:
 *  -1 : major error
 *   0 : success
 *   1 : Exception "API_DeviceLocked"
 *   2 : All other exceptions
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
	
	if ((argc < 3) || (argc > 4))
	{
		cout << "usage: restart_device device cmd [-v] " << endl;
		exit(-1);
	}

	string device_name = argv[1];
	string cmd_name = argv[2];
	transform(cmd_name.begin(),cmd_name.end(),cmd_name.begin(),::tolower);
	
	if (argc == 4)
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
		return -1;
	}

	try
	{
		DeviceProxy *admin;
		string adm_name = device->adm_name();

		admin = new DeviceProxy(adm_name);

		DeviceData din;
		if (cmd_name == "devrestart")
			din << device_name;
		else if (cmd_name == "addobjpolling")
		{
			vector<Tango::DevLong> vl;
			vector<string> vs;

			vl.push_back(2000);
			vs.push_back(device_name);
			vs.push_back("command");
			vs.push_back("state");

			din.insert(vl,vs);
		}
		else if (cmd_name == "remobjpolling")
		{
			vector<string> vs;

			vs.push_back(device_name);
			vs.push_back("command");
			vs.push_back("state");

			din << vs;
		}
		else if (cmd_name == "updobjpollingperiod")
		{
			vector<Tango::DevLong> vl;
			vector<string> vs;

			vl.push_back(4000);
			vs.push_back(device_name);
			vs.push_back("command");
			vs.push_back("state");

			din.insert(vl,vs);
		}
		else if (cmd_name == "addloggingtarget")
		{
			vector<string> vs;

			vs.push_back(device_name);
			vs.push_back("file::/tmp/bid.log");

			din << vs;
		}
		else if (cmd_name == "removeloggingtarget")
		{
			vector<string> vs;

			vs.push_back(device_name);
			vs.push_back("file:/tmp/bid.log");

			din << vs;
		}
		else if (cmd_name == "setlogginglevel")
		{
			vector<Tango::DevLong> vl;
			vector<string> vs;

			vl.push_back(5);
			vs.push_back(device_name);

			din.insert(vl,vs);
		}
		else
			return -1;

		admin->command_inout(cmd_name,din);
	}
	catch (Tango::DevFailed &e)
	{
		if (::strcmp(e.errors[0].reason.in(),"API_DeviceLocked") == 0)
			return 1;
		else
			return 2;
	}

	delete device;		
	return 0;
	
}
