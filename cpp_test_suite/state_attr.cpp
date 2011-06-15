/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#ifdef WIN32
#include <process.h>
#endif

#define	coutv	if (verbose == true) cout

#define STATE_LOG_MESSAGE	"State: Number of attribute(s) to read: "

bool verbose = false;

void start_logging(string &,string &);
void stop_logging(string &,string &);
int message_in_file(string &,string &,vector<string> &);
void build_f_name(string &);

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
		sta = Tango::MOVING;
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

//**************************************************************************
//
//	Check state for alarmed and polled attribute(s)
//
//**************************************************************************

	if (device->is_attribute_polled("SlowAttr"))
		device->stop_poll_attribute("SlowAttr");

	string adm_name = device->adm_name();
	string file_name;
	build_f_name(file_name);
	start_logging(adm_name,file_name);

	AttributeInfoListEx *att_conf2;

	try
	{
		DeviceAttribute da;

// Set an alarm in the SlowAttr attribute
		
		vector<string> att_conf_list;
		att_conf_list.push_back("SlowAttr");
	
		att_conf2 = device->get_attribute_config_ex(att_conf_list);

		(*att_conf2)[0].alarms.min_alarm = "6.6";
		device->set_attribute_config(*att_conf2);

// Get device state with SlowAttr non-polled and polled

		da = device->read_attribute("State");
		Tango::DevState sta1 = device->state();
		coutv << "State = " << Tango::DevStateName[sta1] << endl;

		device->poll_attribute("SlowAttr",1000);
		device->poll_attribute("Long_attr",1000);

		Tango_sleep(2);

		da = device->read_attribute("State");
		Tango::DevState sta2 = device->state();
		coutv << "State = " << Tango::DevStateName[sta2] << endl;

		device->stop_poll_attribute("Long_attr");

		Tango_sleep(2);

		da = device->read_attribute("State");
		Tango::DevState sta3 = device->state();
		coutv << "State = " << Tango::DevStateName[sta3] << endl;

// Stop lib logging

		stop_logging(adm_name,file_name);

		file_name.erase(0,6);
		vector<string> mess_in_file;
		string base_message(STATE_LOG_MESSAGE);
		int res = message_in_file(file_name,base_message,mess_in_file);

		for (unsigned long loop = 0;loop < mess_in_file.size();loop++)
			coutv << "Message in file = " << mess_in_file[loop] << endl;

// Reset device server to its normal state

		(*att_conf2)[0].alarms.min_alarm = "NaN";
		device->set_attribute_config(*att_conf2);
		device->stop_poll_attribute("SlowAttr");

// Check everything fine

		assert (res == 0);
		assert (mess_in_file.size() == 6);
		string mess = base_message + "2";
		assert (mess_in_file[0].find(mess) != string::npos);
		assert (mess_in_file[1].find(mess) != string::npos);
		mess = base_message + "0";
		assert (mess_in_file[2].find(mess) != string::npos);
		assert (mess_in_file[3].find(mess) != string::npos);
		mess = base_message + "1";
		assert (mess_in_file[4].find(mess) != string::npos);
		assert (mess_in_file[5].find(mess) != string::npos);

		assert (sta1 == Tango::ALARM);
		assert (sta2 == Tango::ALARM);
		assert (sta3 == Tango::ALARM);

		cout << "   Reading State with alarmed and polled attribute(s) --> OK" << endl;

	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);

		stop_logging(adm_name,file_name);
		(*att_conf2)[0].alarms.min_alarm = "NaN";
		device->set_attribute_config(*att_conf2);
		device->stop_poll_attribute("SlowAttr");
		device->stop_poll_attribute("Long_attr");

		exit(-1);
	}

	delete device;	
	return 0;
	
}

//**************************************************************************
//
//	Create logging file name from PID (in /tmp directory)
//
//**************************************************************************

void build_f_name(string &f_name)
{
	stringstream str;
#ifdef WIN32
	int pid = _getpid();
#else
	pid_t pid = getpid();
#endif
	str << pid;

	string tmp_name("file::/tmp/ds_");
	tmp_name = tmp_name + str.str();
//	cout << "file_name = " << tmp_name << endl;
	f_name = tmp_name;
}

//**************************************************************************
//
//	Ask library to log messages
//
//**************************************************************************

void start_logging(string &adm_dev_name,string &f_name)
{
	try
	{
		vector<string> log_target;
		log_target.push_back(adm_dev_name);
		log_target.push_back(f_name);
	
		DeviceProxy adm_dev(adm_dev_name);
		DeviceData dd;
		dd << log_target;
		adm_dev.command_inout("AddLoggingTarget",dd);

		vector<string> log_level;
		log_level.push_back(adm_dev_name);
		vector<Tango::DevLong> log_level_long;
		log_level_long.push_back(LOG_DEBUG);

		dd.insert(log_level_long,log_level);
		adm_dev.command_inout("SetLoggingLevel",dd);
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
	}
}

//**************************************************************************
//
//	Ask library to stop logging messages
//
//**************************************************************************

void stop_logging(string &adm_dev_name,string &f_name)
{
	try
	{
		vector<string> log_target;
		log_target.push_back(adm_dev_name);
		log_target.push_back(f_name);
	
		DeviceProxy adm_dev(adm_dev_name);
		DeviceData dd;
		dd << log_target;
		adm_dev.command_inout("RemoveLoggingTarget",dd);

		vector<string> log_level;
		log_level.push_back(adm_dev_name);
		vector<Tango::DevLong> log_level_long;
		log_level_long.push_back(LOG_WARN);

		dd.insert(log_level_long,log_level);
		adm_dev.command_inout("SetLoggingLevel",dd);
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
	}
}

//**************************************************************************
//
//	Find a message in the logging file
//
//**************************************************************************

int message_in_file(string &f_name,string &mess,vector<string> &mess_occur)
{
	ifstream inFile;
	string file_line;
	int ret = 0;
    
    inFile.open(f_name.c_str());
    if (!inFile)
	{
		ret = -1;
        return ret;
    }
 
	string::size_type pos_env;
		   
    while (!inFile.eof())
	{
		getline(inFile,file_line);
		
		if ((pos_env = file_line.find(mess)) != string::npos)
		{
			mess_occur.push_back(file_line);
		}
    }
   
    inFile.close();
    return ret;
}
