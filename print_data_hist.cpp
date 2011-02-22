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
	
	if (argc < 2)
	{
		cout << "usage: %s device [-v]" << endl;
		exit(-1);
	}

	string device_name = argv[1];
	bool print = false;
	
	if (argc == 3)
	{
		string verb = argv[2];
		if (verb != "-v")
		{
			cout << "Usage: %s device [-v]" << endl;
			exit(-1);
		}
		else
			print = true;
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

	int i;
	vector<DeviceDataHistory> *d_hist;
	
	try
	{
	
// Test command_history (for strings)

		int hist_depth = 4;
		d_hist = device->command_history("IOPollStr1",hist_depth);

		for (i = 0;i < hist_depth;i++)
		{
			cout << (*d_hist)[i] << endl;
		}
		delete d_hist;	
				
		cout << "   Read command history (string) --> OK" << endl;
		
// Test command_history with exception

		d_hist = device->command_history("IOExcept",hist_depth);
		
		for (i = 0;i < hist_depth;i++)
		{
			cout << (*d_hist)[i] << endl;
		}
		delete d_hist;	
				
		cout << "   Read command history with exception --> OK" << endl;
		
// Test attribute_history (for strings spectrum)
		
		vector<DeviceAttributeHistory> *a_hist;
		a_hist = device->attribute_history("PollString_spec_attr",hist_depth);

		for (i = 0;i < hist_depth;i++)
		{
			cout << (*a_hist)[i] << endl;
		}
		delete a_hist;	
				
		cout << "   Read attribute history (string spectrum) --> OK" << endl;

// Test attribute_history with exception

		a_hist = device->attribute_history("attr_wrong_type",hist_depth);
		
		for (i = 0;i < hist_depth;i++)
		{
			cout << (*a_hist)[i] << endl;
		}
		delete a_hist;	
				
		cout << "   Read attribute history with exception --> OK" << endl;
		
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

			
	delete device;
	return 0;
}
