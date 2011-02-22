/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device,*device1;
	
	string device_name("dev/test/10");
	bool print = false;
	
	if (argc == 2)
	{
		string verb = argv[1];
		if (verb != "-v")
		{
			cout << "Usage: %s [-v]" << endl;
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
	
	try
	{
	
// Test polling_status

		bool string_attr = false;
		vector<string> *poll_str;
		poll_str = device->polling_status();

		for (int i = 0;i < poll_str->size();i++)
		{
			if ((*poll_str)[i].find("String_attr") != string::npos)
			{
				string_attr = true;
				break;
			}
		}
		
		if (print == true)
		{
			cout << poll_str->size() << " object(s) polled for device" << endl;
			cout << endl;
			for (int i = 0;i < poll_str->size();i++)
				cout << "Polling status = " << (*poll_str)[i] << endl;
			cout << endl;
		}

		if (string_attr == true)
			assert ( poll_str->size() == 7);
		else				
			assert ( poll_str->size() == 6);

		delete poll_str;
		
		device1 = new DeviceProxy("dev/test/11");
		poll_str = device1->polling_status();
		
		assert (poll_str->size() == 0);
		delete poll_str;
		
		delete device1;	
				
		cout << "   Polling_status --> OK" << endl;

// Test get_command_poll_period

		string cmd("IOExcept");		
		int per = device->get_command_poll_period(cmd);	
		
		if (print == true)
		{
			cout << "Cmd " << cmd << " polling period = " << per << endl;
		}
		
		assert( per == 2000 );
			
		string attr("PollLong_attr");
		per = device->get_attribute_poll_period(attr);
		
		if (print == true)
			cout << "Attr " << attr << " polling period = " << per << endl;
			
		assert ( per == 1000 );
		
		cout << "   Get polling period (cmd and attr) --> OK" << endl;
		
// Test poll_command

		cmd = ("IOArray1");
		bool poll = device->is_command_polled(cmd);
		
		assert( poll == false );
		
		device->poll_command(cmd,3000);
		poll = device->is_command_polled(cmd);
		
		assert ( poll == true );
		
		per = device->get_command_poll_period(cmd);
		
		assert (per == 3000 );

#ifdef WIN32
		Sleep(3000);
#else		
		sleep(3);
#endif
		
		device->poll_command(cmd,5000);
		per = device->get_command_poll_period(cmd);
		
		assert ( per == 5000 );

#ifdef WIN32
		Sleep(3000);
#else		
		sleep(3);
#endif
		poll_str = device->polling_status();

		if (print == true)
		{
			cout << poll_str->size() << " object(s) polled for device" << endl;
			cout << endl;
			for (int i = 0;i < poll_str->size();i++)
				cout << "Polling status = " << (*poll_str)[i] << endl;
			cout << endl;
		}

		if (string_attr == true)				
			assert ( poll_str->size() == 8);
		else
			assert ( poll_str->size() == 7);
			
		delete poll_str;
				
		cout << "   Poll command method --> OK" << endl;
		
// Test stop_poll_command

		device->stop_poll_command(cmd);
		
		poll = device->is_command_polled(cmd);
		
		assert ( poll == false );

		poll_str = device->polling_status();

		if (print == true)
		{
			cout << poll_str->size() << " object(s) polled for device" << endl;
			cout << endl;
			for (int i = 0;i < poll_str->size();i++)
				cout << "Polling status = " << (*poll_str)[i] << endl;
			cout << endl;
		}

		if (string_attr == true)				
			assert ( poll_str->size() == 7);
		else
			assert ( poll_str->size() == 6);
			
		delete poll_str;
				
		cout << "   Stop poll command method --> OK" << endl;

// Test poll_attribute

		attr = ("Double_attr");
		poll = device->is_attribute_polled(attr);
		
		assert( poll == false );
		
		device->poll_attribute(attr,3000);
		poll = device->is_attribute_polled(attr);
		
		assert ( poll == true );
		
		per = device->get_attribute_poll_period(attr);
		
		assert (per == 3000 );

#ifdef WIN32
		Sleep(3000);
#else		
		sleep(3);
#endif
		
		device->poll_attribute(attr,5000);
		per = device->get_attribute_poll_period(attr);
		
		assert ( per == 5000 );

#ifdef WIN32		
		Sleep(3000);
#else
		sleep(3);
#endif

		poll_str = device->polling_status();

		if (print == true)
		{
			cout << poll_str->size() << " object(s) polled for device" << endl;
			cout << endl;
			for (int i = 0;i < poll_str->size();i++)
				cout << "Polling status = " << (*poll_str)[i] << endl;
			cout << endl;
		}

		if (string_attr == true)				
			assert ( poll_str->size() == 8);
		else
			assert ( poll_str->size() == 7);
		delete poll_str;
				
		cout << "   Poll attribute method --> OK" << endl;
		
// Test stop_poll_command

		device->stop_poll_attribute(attr);
		
		poll = device->is_attribute_polled(attr);
		
		assert ( poll == false );

		poll_str = device->polling_status();

		if (print == true)
		{
			cout << poll_str->size() << " object(s) polled for device" << endl;
			cout << endl;
			for (int i = 0;i < poll_str->size();i++)
				cout << "Polling status = " << (*poll_str)[i] << endl;
			cout << endl;
		}

		if (string_attr == true)				
			assert ( poll_str->size() == 7);
		else
			assert ( poll_str->size() == 6);
		delete poll_str;
				
		cout << "   Stop poll attribute method --> OK" << endl;
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

			
	delete device;
	return 0;
}
