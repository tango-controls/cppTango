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

		device->poll_command("State",100);
		device->poll_attribute("Double_attr",100);
		
		Tango_sleep(2);
		
// Get polling status

		vector<string> *poll_sta = device->polling_status();
		
// Stop polling

		device->stop_poll_command("State");
		device->stop_poll_attribute("Double_attr");

// Check polling status

		assert (poll_sta->size() == 4);

		string poll_cmd = (*poll_sta)[0];
		string::size_type pos,end;
		pos = poll_cmd.find("depth");
		pos = pos + 8;
		end = poll_cmd.find('\n',pos);
		string dep = poll_cmd.substr(pos, end - pos);
		assert (dep == "5");

		string poll_attr = (*poll_sta)[3];
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
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
		
	delete device;
	
	return 0;
}
