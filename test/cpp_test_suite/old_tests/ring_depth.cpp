/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#ifdef WIN32
#include <process.h>
#endif


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " <device>" << endl;
		exit(-1);
	}

	string device_name = argv[1];

	try 
	{
		device = new DeviceProxy(device_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(1);
	}

	cout << '\n' << "new DeviceProxy(" << device->name() << ") returned" << '\n' << endl;

	try
	{
	
// Change poll ring depth for one command and one attribute

		DbData db_data;
		DbDatum cmd("cmd_poll_ring_depth");
		DbDatum attr("attr_poll_ring_depth");

		vector<string> vs;
		vs.push_back("State");
		vs.push_back("5");
		cmd << vs;
		
		vs.clear();
		vs.push_back("Double_attr");
		vs.push_back("15");
		attr << vs;
				
		db_data.push_back(cmd);
		db_data.push_back(attr);
		
		device->put_property(db_data);

// Connect to adm_device

		string ad = device->adm_name();
		DeviceProxy adm(ad);
		
// Restart the device

		DeviceData in;
		in << device_name;
#ifdef VALGRIND
		adm.set_timeout_millis(15000);
#endif
		adm.command_inout("DevRestart",in);
		
// Reconnect to device

		Tango_sleep(1);
		delete device;
		device = new DeviceProxy(device_name);

// Start polling 

		device->poll_command("State",300);
		device->poll_attribute("Double_attr",300);
		
		Tango_sleep(2);
		
// Get polling status

		vector<string> *poll_sta = device->polling_status();
		
// Stop polling

		device->stop_poll_command("State");
		device->stop_poll_attribute("Double_attr");

// Check polling status

		assert (poll_sta->size() == 3);

		unsigned int i;
		int index;
		for (i = 0;i < poll_sta->size();i++)
		{
			if ((*poll_sta)[i].find("name = State") != string::npos)
			{
				index = i;
				break;
			}
		}

		string poll_cmd = (*poll_sta)[index];
		string::size_type pos,end;
		pos = poll_cmd.find("depth");
		pos = pos + 8;
		end = poll_cmd.find('\n',pos);
		string dep = poll_cmd.substr(pos, end - pos);
		assert (dep == "5");

		for (i = 0;i < poll_sta->size();i++)
		{
			if ((*poll_sta)[i].find("name = Double_attr") != string::npos)
			{
				index = i;
				break;
			}
		}
		string poll_attr = (*poll_sta)[index];
		pos = poll_attr.find("depth");
		pos = pos + 8;
		end = poll_attr.find('\n',pos);
		dep = poll_attr.substr(pos, end - pos);
		assert (dep == "15");
		
		cout << "   Command and attribute with their own polling buffer depth --> OK" << endl;
					
		delete poll_sta;

		
// Remove these property from db

		DbData db_data1;
		DbDatum cmd1("cmd_poll_ring_depth");
		DbDatum attr1("attr_poll_ring_depth");
		
		db_data1.push_back(cmd1);
		db_data1.push_back(attr1);		
		device->delete_property(db_data1);

// Call command which execute polling methods in DeviceImpl

		device->set_timeout_millis(8000);
		DeviceData dd;
		dd = device->command_inout("IOPollingInDevice");

		vector<string> vs_poll;
		dd >> vs_poll;

		assert (vs_poll.size() == 12);
		assert (vs_poll[0] == "Attribute Double_spec_attr polled = false");
		assert (vs_poll[1] == "Command OULong polled = false");
		assert (vs_poll[2] == "Attribute Double_spec_attr polling period = 0");
		assert (vs_poll[3] == "Command OULong polling period = 0");
		assert (vs_poll[4] == "Attribute Double_spec_attr polled = true");
		assert (vs_poll[5] == "Command OULong polled = true");
		assert (vs_poll[6] == "Attribute Double_spec_attr polling period = 250");
		assert (vs_poll[7] == "Command OULong polling period = 250");
		assert (vs_poll[8] == "Attribute Double_spec_attr polled = false");
		assert (vs_poll[9] == "Command OULong polled = false");
		assert (vs_poll[10] == "Attribute Double_spec_attr polling period = 0");
		assert (vs_poll[11] == "Command OULong polling period = 0");

		cout << "   Polling related methods in DeviceImpl --> OK" << endl;

// Change polling period in polled attribute !!!

		device->command_inout("IOsophisticatedPollInDevice");
		Tango_sleep(4);
		dd = device->command_inout("IOGetPollMess");

		vs_poll.clear();
		dd >> vs_poll;

		assert (vs_poll.size() == 9);
		assert (vs_poll[0] == "Attribute Double_spec_attr polling period = 500");
		assert (vs_poll[1] == "Attribute Short_attr polling period = 250");
		assert (vs_poll[2] == "Attribute ReynaldPollAttr polling period = 250");
		assert (vs_poll[3] == "Attribute Double_spec_attr polling period = 500");
		assert (vs_poll[4] == "Attribute Short_attr polling period = 500");
		assert (vs_poll[5] == "Attribute ReynaldPollAttr polling period = 500");
		assert (vs_poll[6] == "Attribute Double_spec_attr polling period = 500");
		assert (vs_poll[7] == "Attribute Short_attr polling period = 250");
		assert (vs_poll[8] == "Attribute ReynaldPollAttr polling period = 250");

		cout << "   Polled attributes changing their own polling period --> OK" << endl;

	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
		
	delete device;
	
	return 0;
}
