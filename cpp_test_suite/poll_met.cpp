/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#ifndef COMPAT
#define		BASIC_NB_POLL		15
#else
#define		BASIC_NB_POLL		14
#endif

using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device,*device1;
	
	string device_name("dev/test/10");
	string alias_name("et_alias");
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

// Test polling_status from device name

		bool string_attr = false;
		bool startpoll = false;
		vector<string> *poll_str;
		poll_str = device->polling_status();

		long nb_polled = BASIC_NB_POLL;
		for (int i = 0;i < poll_str->size();i++)
		{
			if ((*poll_str)[i].find("String_attr") != string::npos)
			{
				string_attr = true;
				nb_polled++;
			}
			if ((*poll_str)[i].find("IOStartPoll") != string::npos)
			{
				startpoll = true;
				nb_polled++;
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

		assert (poll_str->size() == nb_polled );

		delete poll_str;
		
		device1 = new DeviceProxy("dev/test/11");
		poll_str = device1->polling_status();

		if (print == true)
		{
			cout << poll_str->size() << " object(s) polled for device" << endl;
			cout << endl;
			for (int i = 0;i < poll_str->size();i++)
				cout << "Polling status = " << (*poll_str)[i] << endl;
			cout << endl;
		}
		
		assert (poll_str->size() == 0);
		delete poll_str;
		
		delete device1;			

// Test polling status from alias name

		string adm_name = device->adm_name();
		DeviceProxy *admin_dev = new DeviceProxy(adm_name);
		
		DeviceData d_send,d_received;
		d_send << alias_name;	
		d_received = admin_dev->command_inout("DevPollStatus",d_send);
		vector<string> v_str;
		d_received >> v_str;

		nb_polled = BASIC_NB_POLL;				
		for (int i = 0;i < v_str.size();i++)
		{
			if (v_str[i].find("String_attr") != string::npos)
			{
				string_attr = true;
				nb_polled++;
				continue;
			}
			if (v_str[i].find("IOStartPoll") != string::npos)
			{
				startpoll = true;
				nb_polled++;
			}
		}
		
		if (print == true)
		{
			cout << v_str.size() << " object(s) polled for device" << endl;
			cout << endl;
			for (int i = 0;i < v_str.size();i++)
				cout << "Polling status = " << v_str[i] << endl;
			cout << endl;
		}

		assert ( v_str.size() == nb_polled);		

		cout << "   Polling_status (from device name and alias) --> OK" << endl;
		
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

		nb_polled = BASIC_NB_POLL + 1;
		for (int i = 0;i < poll_str->size();i++)
		{
			if ((*poll_str)[i].find("String_attr") != string::npos)
			{
				string_attr = true;
				nb_polled++;
			}
			if ((*poll_str)[i].find("IOStartPoll") != string::npos)
			{
				startpoll = true;
				nb_polled++;
			}
		}		

		assert ( poll_str->size() == nb_polled);
			
		delete poll_str;
				
		cout << "   Poll command method --> OK" << endl;

// Test min polling period
//
//  WARNING, this test works only if device property min_poll_period set to 200
//  and cmd_min_poll_period set to IOExcept,500
//

		bool except = false;
		try
		{
			device->poll_command("IOExcept",300);
		}
		catch (Tango::DevFailed &e)
		{
			except = true;
		}

		assert (except == true);
		except = false;		
		
		string prop_name("cmd_min_poll_period");
		device->delete_property(prop_name);
	
		try
		{
			device->poll_command("IOExcept",100);
		}
		catch (Tango::DevFailed &e)
		{
			except = true;
		}

		assert (except == true);
		except = false;

		try
		{
			device->poll_command("IOExcept",500);
		}
		catch (Tango::DevFailed &e)
		{
			except = true;
		}

		assert (except = true);
		except = false;

#ifdef WIN32
		Sleep(3000);
#else		
		sleep(3);
#endif

		try
		{
			device->poll_command("IOExcept",2000);
		}
		catch (Tango::DevFailed &e)
		{
			except = true;
		}

		assert (except == false);

		DbData db;
		vector<string> prop_vs;
		prop_vs.push_back("IOExcept");
		prop_vs.push_back("500");
		db.push_back(DbDatum("cmd_min_poll_period"));
		db[0] << prop_vs;
		device->put_property(db);

		cout << "   min polling period management --> OK" << endl;

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

		nb_polled = BASIC_NB_POLL;
		for (int i = 0;i < poll_str->size();i++)
		{
			if ((*poll_str)[i].find("String_attr") != string::npos)
			{
				string_attr = true;
				nb_polled++;
			}
			if ((*poll_str)[i].find("IOStartPoll") != string::npos)
			{
				startpoll = true;
				nb_polled++;
			}
		}		

		assert ( poll_str->size() == nb_polled);
					
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

		nb_polled = BASIC_NB_POLL + 1;
		for (int i = 0;i < poll_str->size();i++)
		{
			if ((*poll_str)[i].find("String_attr") != string::npos)
			{
				string_attr = true;
				nb_polled++;
			}
			if ((*poll_str)[i].find("IOStartPoll") != string::npos)
			{
				startpoll = true;
				nb_polled++;
			}
		}		

		assert ( poll_str->size() == nb_polled);
		
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

		nb_polled = BASIC_NB_POLL;
		for (int i = 0;i < poll_str->size();i++)
		{
			if ((*poll_str)[i].find("String_attr") != string::npos)
			{
				string_attr = true;
				nb_polled++;
			}
			if ((*poll_str)[i].find("IOStartPoll") != string::npos)
			{
				startpoll = true;
				nb_polled++;
			}
		}		

		assert ( poll_str->size() == nb_polled);
		
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
