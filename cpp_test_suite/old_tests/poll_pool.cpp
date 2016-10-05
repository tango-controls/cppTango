/* 
 * example of a client using the TANGO device api.
 */

#include <tango/tango.h>
#include <assert.h>

#define	coutv	if (verbose == true) cout

using namespace Tango;
using namespace std;

bool verbose = false;

#define TEST_CLASS "devTest"

void split_string(string &,char,vector<string> &);
void reset_device_server(Database *,vector<string> &,string &,DeviceProxy *,string &,DeviceProxy *);
void del_device_no_error(Database *,const char *);

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		cout << "usage: %s device1 device2 inst_name [-v]" << endl;
		exit(-1);
	}

	string device_name1 = argv[1];
	string device_name2 = argv[2];
	string inst_name = argv[3];

	vector<string> ref_polling_pool_conf;
	string admin_dev_name("dserver/");
	Database *db = new Database();
	DeviceProxy *admin_dev = NULL;
	DeviceProxy *dev2 = NULL;
	
	try
	{

// First poll device 2

		dev2 = new DeviceProxy(device_name2);
		dev2->poll_attribute("PollLong_attr",1000);

		Tango_sleep(2);

// Get a reference polling thread pool

		DeviceProxy dev(device_name1);
		DeviceData dd;

		dd = dev.command_inout("PollingPoolTst");
		vector<string> polled_devs;

		dd >> ref_polling_pool_conf;
		split_string(ref_polling_pool_conf[0],',',polled_devs);
		int nb_polled_devs = polled_devs.size();

// Add a device into device server and restart it
// Also add property to poll one of the device attribute

		DbDevInfo my_device_info;
		string new_dev("test/");
		new_dev = new_dev + inst_name + "/77";

		my_device_info.name = new_dev.c_str();
		my_device_info._class = TEST_CLASS;
		string serv_name(TEST_CLASS);
		serv_name = serv_name + '/' + inst_name;
		my_device_info.server = serv_name.c_str();

		db->add_device(my_device_info);

		DbDatum poll_prop("polled_attr");
		vector<string> poll_param;
		poll_param.push_back("PollLong_attr");
		poll_param.push_back("1000");
		poll_prop << poll_param;
		DbData db_poll;
		db_poll.push_back(poll_prop);
		db->put_device_property(new_dev.c_str(),db_poll);

		admin_dev_name = admin_dev_name + serv_name;

		admin_dev = new DeviceProxy(admin_dev_name);
		admin_dev->command_inout("RestartServer");

		Tango_sleep(5);

// Read polling threads pool conf once more

		DeviceData da;

		da = dev.command_inout("PollingPoolTst");
		vector<string> new_polling_pool_conf;
		da >> new_polling_pool_conf;

		assert (ref_polling_pool_conf.size() == new_polling_pool_conf.size());
		split_string(new_polling_pool_conf[0],',',polled_devs);
		int new_nb_polled_devs = polled_devs.size();

		assert (new_nb_polled_devs == nb_polled_devs + 1);
		vector<string>::iterator iter;
		iter = find(polled_devs.begin(),polled_devs.end(),new_dev);

		assert (iter != polled_devs.end());

		cout << "   Add one device to a polling thread pool with 1 thread --> OK" << endl;

// Change polling thread number and add 2 more devices

		string new_dev1_th2 = "test/";
		new_dev1_th2 = new_dev1_th2 + inst_name + "/800";

		my_device_info.name = new_dev1_th2.c_str();
		db->add_device(my_device_info);

		string new_dev2_th2 = "test/";
		new_dev2_th2 = new_dev2_th2 + inst_name + "/801";

		my_device_info.name = new_dev2_th2.c_str();
		db->add_device(my_device_info);

		DbDatum pool_size("polling_threads_pool_size");
		DbData db_data;

		pool_size << 2L;
		db_data.push_back(pool_size);
		db->put_device_property(admin_dev_name.c_str(),db_data);

		db->put_device_property(new_dev1_th2.c_str(),db_poll);
		db->put_device_property(new_dev2_th2.c_str(),db_poll);

		admin_dev->command_inout("RestartServer");

		Tango_sleep(5);

// Check new pool conf

		DeviceData dz;

		dz = dev.command_inout("PollingPoolTst");
		dz >> new_polling_pool_conf;

		assert (new_polling_pool_conf.size() == ref_polling_pool_conf.size() + 1);
		split_string(new_polling_pool_conf[1],',',polled_devs);
		new_nb_polled_devs = polled_devs.size();

		assert (new_nb_polled_devs == 2);

		iter = find(polled_devs.begin(),polled_devs.end(),new_dev1_th2);
		assert (iter != polled_devs.end());

		iter = find(polled_devs.begin(),polled_devs.end(),new_dev2_th2);
		assert (iter != polled_devs.end());

		cout << "   Add two other devices to the polling threads pool with now 2 threads --> OK" << endl;

// Change polling thread number to 3 and add 1 more device

		string new_dev1_th3 = "test/";
		new_dev1_th3 = new_dev1_th3 + inst_name + "/9000";

		my_device_info.name = new_dev1_th3.c_str();
		db->add_device(my_device_info);

		DbDatum pool_size3("polling_threads_pool_size");
		DbData db_data3;

		pool_size3 << 3L;
		db_data3.push_back(pool_size3);
		db->put_device_property(admin_dev_name.c_str(),db_data3);

		db->put_device_property(new_dev1_th3.c_str(),db_poll);

		admin_dev->command_inout("RestartServer");

		Tango_sleep(5);

// Check new pool conf

		DeviceData dx;

		dx = dev.command_inout("PollingPoolTst");
		dx >> new_polling_pool_conf;

		assert (new_polling_pool_conf.size() == ref_polling_pool_conf.size() + 2);
		split_string(new_polling_pool_conf[2],',',polled_devs);
		new_nb_polled_devs = polled_devs.size();

		assert (new_nb_polled_devs == 1);

		iter = find(polled_devs.begin(),polled_devs.end(),new_dev1_th3);
		assert (iter != polled_devs.end());

		cout << "   Add one device to the polling threads pool with now 3 threads --> OK" << endl;

// Delete one device to check automatic polling pool re-configuration

		db->delete_device(new_dev1_th3);

		admin_dev->command_inout("RestartServer");

		Tango_sleep(5);

// Check pool conf

		DeviceData dv;

		dv = dev.command_inout("PollingPoolTst");
		dv >> new_polling_pool_conf;

		assert (new_polling_pool_conf.size() == ref_polling_pool_conf.size() + 1);
		split_string(new_polling_pool_conf[1],',',polled_devs);
		new_nb_polled_devs = polled_devs.size();

		assert (new_nb_polled_devs == 2);

		iter = find(polled_devs.begin(),polled_devs.end(),new_dev1_th2);
		assert (iter != polled_devs.end());

		iter = find(polled_devs.begin(),polled_devs.end(),new_dev2_th2);
		assert (iter != polled_devs.end());

		cout << "   Automatic polling threads pool reconfig after device removal --> OK" << endl;

		reset_device_server(db,ref_polling_pool_conf,admin_dev_name,admin_dev,inst_name,dev2);
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		reset_device_server(db,ref_polling_pool_conf,admin_dev_name,admin_dev,inst_name,dev2);
		exit(-1);
	}

	delete db;
	delete admin_dev;
	delete dev2;	
	return 0;
}

//
// Split a string according to a delimiter character
//

void split_string(string &the_str,char delim,vector<string> &splitted_str)
{
	string::size_type pos,start;
	splitted_str.clear();

	start = 0;
	while ((pos = the_str.find(delim,start)) != string::npos)
	{
		splitted_str.push_back(the_str.substr(start,pos - start));
		start = pos + 1;
	}

	splitted_str.push_back(the_str.substr(start));
}


void del_device_no_error(Database *db,const char *d_name)
{
	try
	{
		db->delete_device(d_name);
	}
	catch (DevFailed &) {}
}

//
// Reset device server to default conf
//

void reset_device_server(Database *db,vector<string> &polling_pool_conf,string &admin_dev_name,DeviceProxy *adm_dev,string &inst_name,DeviceProxy *dev2)
{
	string base_name("test/");
	base_name = base_name + inst_name;

	string dev_name = base_name + "/77";
	del_device_no_error(db,dev_name.c_str());

	dev_name = base_name + "/800";
	del_device_no_error(db,dev_name.c_str());

	dev_name = base_name + "/801";
	del_device_no_error(db,dev_name.c_str());

	dev_name = base_name + "/9000";
	del_device_no_error(db,dev_name.c_str());

	DbDatum pool_conf("polling_threads_pool_conf");
	DbData db_data;

	pool_conf << polling_pool_conf;
	db_data.push_back(pool_conf);
	db->put_device_property(admin_dev_name.c_str(),db_data);

	DbDatum del_prop("polling_threads_pool_size");
	db_data.clear();
	db_data.push_back(del_prop);
	db->delete_device_property(admin_dev_name.c_str(),db_data);

	try
	{
		dev2->stop_poll_attribute("PollLong_attr");
	}
	catch(DevFailed &) {}

	adm_dev->command_inout("RestartServer");

	Tango_sleep(3);
}
