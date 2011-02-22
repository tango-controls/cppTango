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

	
	if (device->is_attribute_polled("state"))
		device->stop_poll_attribute("state");
	if (device->is_attribute_polled("status"))
		device->stop_poll_attribute("status");

//**************************************************************************
//
//			Check that state and status are defined as attr.
//
//**************************************************************************


// First check that state and status are the last two attributes

	AttributeInfoList *att_info;
	try
	{
		att_info = device->attribute_list_query();
	}
	catch (Tango::DevFailed &e)
	{
              	Except::print_exception(e);
		exit(-1);
        }
	
	long state_idx = att_info->size() - 2;
	
	coutv << (*att_info)[state_idx] << endl;
	coutv << (*att_info)[state_idx + 1] << endl;
	
	assert( (*att_info)[state_idx].name == "State" );
	assert( (*att_info)[state_idx + 1].name == "Status" );	

	try
	{
		string state_str("state");
		AttributeInfo sta_ai = device->get_attribute_config(state_str);		
		coutv << sta_ai << endl;
		
		string status_str ("status");
		AttributeInfo status_ai = device->get_attribute_config(status_str);
		coutv << status_ai << endl;
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	cout << "   State and Status defined as attribute --> OK" << endl;

// Check that it is not possible to set the state/status attribute
// config.

	AttributeInfoList att_conf;
	att_conf.push_back((*att_info)[state_idx]);
	att_conf[0].min_alarm = "28";
	bool failed = false;
	
	try
	{
		device->set_attribute_config(att_conf);
	}
	catch (Tango::DevFailed &)
	{
		failed = true;		
        }
	
	assert ( failed == true );
	
	att_conf.clear();
	att_conf.push_back((*att_info)[state_idx + 1]);
	att_conf[0].min_alarm = "28";
	failed = false;
	
	try
	{
		device->set_attribute_config(att_conf);
	}
	catch (Tango::DevFailed &)
	{
		failed = true;		
        }
	
	assert ( failed == true );

	delete att_info;
	cout << "   State and Status attributes config not settable --> OK" << endl;

//**************************************************************************
//
//	Getting state as attribute from device
//
//**************************************************************************
						
// Test reading state as an attribute without polling first

	device->set_source(Tango::DEV);
	string state_name("State");
	DevState sta;

	DeviceAttribute d;
	try
	{
		d = device->read_attribute(state_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	d >> sta;
	coutv << "Device state = " << DevStateName[sta] << endl;
	
	assert( sta == Tango::ON );
	
// Read several attribute in on go including state
	
	vector<string> names;
	names.push_back("state");
	names.push_back("power");
	names.push_back("Position");
		
	vector<DeviceAttribute> *da;
	try
	{	
		da = device->read_attributes(names);
		coutv << (*da)[0] << endl;
		(*da)[0] >> sta;
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	delete da;	
	coutv << "Device state = " << DevStateName[sta] << endl;
	
	assert ( sta == Tango::ON );
	
// Test reading status as an attribute without polling first

	string status_name("Status");
	string status;

	try
	{
		d = device->read_attribute(status_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	d >> status;
	coutv << status << endl;
	
	string::size_type pos;
	pos = status.find("ON state");
	
	assert (pos != string::npos);
	
// Test reading state and status as attributes 

	status = "Not init";
	sta = Tango::UNKNOWN;
	
	names.clear();
	names.push_back("status");
	names.push_back("state");

	try
	{
		da = device->read_attributes(names);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	(*da)[0] >> status;	
	(*da)[1] >> sta;
	
	coutv << status << endl;
	coutv << "Device state = " << DevStateName[sta] << endl;
	
	pos = status.find("ON state");
	
	assert (pos != string::npos);
	assert (sta == Tango::ON );
	delete da;
	
	cout << "   Reading State/Status as attributes from device --> OK" << endl;
	
		
//**************************************************************************
//
//	Getting state as attribute from cache
//
//**************************************************************************

	device->poll_attribute(state_name,1000);
	device->poll_attribute("Status",1200);
	device->set_source(Tango::CACHE);

#ifdef VALGRIND
	usleep(3000000);
#else
#ifdef WIN32
	Tango_sleep(2);
#else
	usleep(1200000);
#endif
#endif

	try
	{
		sta = Tango::UNKNOWN;
		d = device->read_attribute(state_name);
		d >> sta;
		
		coutv << "Device state (from cache) = " << DevStateName[sta] << endl;

		assert (sta == Tango::ON );
		
		sta = Tango::UNKNOWN;
		status = "Not init";
		da = device->read_attributes(names);

		(*da)[0] >> status;	
		(*da)[1] >> sta;
	
		coutv << "Device status (from cache) = " << status << endl;
		coutv << "Device state (from cache) = " << DevStateName[sta] << endl;

		pos = status.find("ON state");
		assert (sta == Tango::ON);
		assert (pos != string::npos);
		
	}
	catch (CORBA::Exception &e)
	{
		device->stop_poll_attribute(state_name);
		device->stop_poll_attribute("Status");
		Except::print_exception(e);
		exit(-1);
	}
	
	device->stop_poll_attribute("State");
	delete da;

	cout << "   Reading State/Status as attributes from cache --> OK" << endl;
	
//**************************************************************************
//
//	Getting state as attribute from cache_device
//
//**************************************************************************

	device->set_source(Tango::CACHE_DEV);

	try
	{
		sta = Tango::UNKNOWN;
		d = device->read_attribute(state_name);
		d >> sta;
		
		coutv << "Device state (from cache_dev) = " << DevStateName[sta] << endl;
		assert (sta == Tango::ON);
		
		sta = Tango::UNKNOWN;
		status = "not init";
		da = device->read_attributes(names);

		(*da)[0] >> status;	
		(*da)[1] >> sta;
	
		coutv << "Device status (from cache_dev) = " << status << endl;
		coutv << "Device state (from cache_dev) = " << DevStateName[sta] << endl;
		pos = status.find("ON state");
		
		assert (sta == Tango::ON);
		assert (pos != string::npos);
	}
	catch (CORBA::Exception &e)
	{
		device->stop_poll_attribute("Status");
		Except::print_exception(e);
		exit(-1);
	}

	delete da;
	cout << "   Reading State/Status as attributes from cache_device --> OK" << endl;
		
//**************************************************************************
//
//	Getting state as command from cache
//
//**************************************************************************

	device->poll_attribute(state_name,1000);
	device->set_source(Tango::CACHE);

#ifdef WIN32
	Tango_sleep(2);
#else
	usleep(1200000);
#endif

	DeviceData dd;	
	try
	{
		sta = Tango::UNKNOWN;
		dd = device->command_inout("state");
		dd >> sta;
		
		coutv << "Device state (as command from cache) = " << sta << endl;
		assert (sta == Tango::ON);
		
		device->set_source(Tango::DEV);
		
		sta = Tango::UNKNOWN;
		dd = device->command_inout("state");
		dd >> sta;
		
		coutv << "Device state (as command from device) = " << sta << endl;
		assert (sta == Tango::ON);

		sta = Tango::UNKNOWN;
		device->set_source(Tango::CACHE_DEV);
		
		dd = device->command_inout("state");
		dd >> sta;
		
		coutv << "Device state (as command from cache-dev) = " << sta << endl;				
		assert (sta == Tango::ON);
		
		dd = device->command_inout("status");
		status = "Not set";
		dd >> status;
		
		coutv << "Device status (as command from cache-dev) = " << status << endl;
		pos = status.find("ON state");
		assert (pos != string::npos);
	}
	
	catch (CORBA::Exception &e)
	{
		device->stop_poll_attribute("Status");
		device->stop_poll_attribute("State");
		Except::print_exception(e);
		exit(-1);
	}

	cout << "   Reading State/Status as commands --> OK" << endl;
	
	device->stop_poll_attribute("Status");		
	device->stop_poll_attribute("State");
	
	delete device;

//**************************************************************************
//
//	Try to create an AttributeProxy for state and status
//
//**************************************************************************

	try
	{
		string att_name = device_name;
		att_name = att_name + "/state";
		
		AttributeProxy att_proxy(att_name);
		
		DeviceAttribute da;
		DevState sta = Tango::UNKNOWN;
		da = att_proxy.read();
		da >> sta;
		
		assert (sta == Tango::ON);
		
		cout << "   Reading State attribute with AttributeProxy object --> OK" << endl;

		att_name = device_name;
		att_name = att_name + "/StaTus";
		
		AttributeProxy att_proxy_status(att_name);
		
		string status = "Burp";
		da = att_proxy_status.read();
		da >> status;

		string::size_type pos;
		pos = status.find("ON state");
				
		assert (pos != string::npos);				

		cout << "   Reading Status attribute with AttributeProxy object --> OK" << endl;
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
		
	return 0;
	
}
