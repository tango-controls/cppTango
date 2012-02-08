#include <tango.h>

using namespace  Tango;

#define CLASS_NAME "DevTest"

/*
 * This is a utility that configures properties of test device servers provided as
 * command line parameters. Attribute values, their ranges and other properties are
 * set to defaults used in the test suite.
 */
int main(int argc, char **argv)
{
	if(argc < 7)
	{
		cout << "usage: " << argv[0] << " dserver device1 device2 device3 device1_alias attribute_alias" << endl;
		exit(-1);
	}

	string dserver_name = argv[1], device1_name = argv[2], device2_name = argv[3], device3_name = argv[4], device1_alias = argv[5], attribute_alias = argv[6];

	Database *db = new Database();
	DbData db_data;

	long num_prop;
	string str;
	vector<string> str_vec;
	DevLong lg;
	DeviceData din;
	DbDevInfos db_dev_infos;
	DbDevInfo db_dev_info_1, db_dev_info_2, db_dev_info_3;

//
// DsCache/test pseudo server (creation & properties)
//

	str = "DsCache/test";
	db_dev_info_1.name = "test/cache1/1";
	db_dev_info_1._class = "CacheTest1";
	db_dev_info_2.name = "test/cache1/2";
	db_dev_info_2._class = "CacheTest1";
	db_dev_info_3.name = "test/cache2/1";
	db_dev_info_3._class = "CacheTest2";
	db_dev_infos.push_back(db_dev_info_1);
	db_dev_infos.push_back(db_dev_info_2);
	db_dev_infos.push_back(db_dev_info_3);

	try
	{
		db->add_server(str, db_dev_infos);
	}
	catch(...)
	{
		cout << "Exception: cannot create DsCache/test pseudo server" << endl;
	}

	db_data.clear();
	DbDatum tst_property_1("tst_property");
	lg = 15;
	tst_property_1 << lg;
	db_data.push_back(tst_property_1);

	try
	{
		db->put_device_property(db_dev_info_1.name, db_data);
	}
	catch(...)
	{
		cout << "Exception: cannot set test/cache1/1 properties" << endl;
	}

	db_data.clear();
	DbDatum tst_property_2("tst_property");
	lg = 25;
	tst_property_2 << lg;
	db_data.push_back(tst_property_2);

	try
	{
		db->put_device_property(db_dev_info_2.name, db_data);
	}
	catch(...)
	{
		cout << "Exception: cannot set test/cache1/2 properties" << endl;
	}


//
// dserver properties
//

	db_data.clear();
	DbDatum logging_level("logging_level");
	string logging_str = "WARNING";
	logging_level << logging_str;
	db_data.push_back(logging_level);

	try
	{
		db->put_device_property(dserver_name, db_data);
	}
	catch(...)
	{
		cout << "Exception: cannot set dserver properties" << endl;
	}


//
// Properties common to all devices
//

// class properties

	db_data.clear();
	DbDatum allowed_access_cmd("AllowedAccessCmd");
	str = "IOFloat";
	allowed_access_cmd << str;
	db_data.push_back(allowed_access_cmd);

	try
	{
		db->put_class_property("DevTest", db_data);
	}
	catch(...)
	{
		cout << "Exception: cannot set class properties" << endl;
	}


// attribute properties defined on class level

	db_data.clear();
	DbDatum class_string_spec_attr("String_spec_attr"), class_string_spec_attr_label("label"), class_string_spec_attr_unit("unit"), class_string_spec_attr_format("format");

	num_prop = 3;
	class_string_spec_attr << num_prop;
	db_data.push_back(class_string_spec_attr);
	str = "Class label";
	class_string_spec_attr_label << str;
	db_data.push_back(class_string_spec_attr_label);
	str = "Class unit";
	class_string_spec_attr_unit << str;
	db_data.push_back(class_string_spec_attr_unit);
	str = "Class format";
	class_string_spec_attr_format << str;
	db_data.push_back(class_string_spec_attr_format);

	DbDatum class_added_short_attr("Added_short_attr"), class_added_short_attr_label("label");

	num_prop = 1;
	class_added_short_attr << num_prop;
	db_data.push_back(class_added_short_attr);
	str = "From db (class)";
	class_added_short_attr_label << str;
	db_data.push_back(class_added_short_attr_label);

	try
	{
		db->put_class_attribute_property(CLASS_NAME, db_data);
	}
	catch(...)
	{
		cout << "Exception: cannot set attribute properties defined on class level" << endl;
	}

// attribute properties defined on device level

	db_data.clear();
	DbDatum long_attr("Long_attr"), long_attr_min_alarm("min_alarm"), long_attr_max_alarm("max_alarm");

	num_prop = 2;
	long_attr << num_prop;
	db_data.push_back(long_attr);
	lg = 1000;
	long_attr_min_alarm << lg;
	db_data.push_back(long_attr_min_alarm);
	lg = 1500;
	long_attr_max_alarm << lg;
	db_data.push_back(long_attr_max_alarm);

	for(int i = 2; i < argc; i++)
	{
		try
		{
			db->put_device_attribute_property(argv[i], db_data);
		}
		catch(...)
		{
			cout << "Exception: cannot set common properties for the device: " << argv[i] << endl;
		}
	}

//
// Device specific properties
//

// device1

	db_data.clear();
	DbDatum added_short_attr("Added_short_attr"), added_short_attr_format("format");

	num_prop = 1;

	added_short_attr << num_prop;
	db_data.push_back(added_short_attr);
	str = "From db (device)";
	added_short_attr_format << str;
	db_data.push_back(added_short_attr_format);

	try
	{
		db->put_device_attribute_property(device1_name, db_data);
	}
	catch(...)
	{
		cout << "Exception: cannot set specific properties for the device: " << device1_name << endl;
	}

	db_data.clear();
	DbDatum min_poll_period("min_poll_period");

	lg = 200;
	min_poll_period << lg;
	db_data.push_back(min_poll_period);

	try
	{
		db->put_device_property(device1_name, db_data);
	}
	catch(...)
	{
		cout << "Exception: cannot set specific properties for the device: " << device1_name << endl;
	}

	db_data.clear();
	DbDatum cmd_min_poll_period("cmd_min_poll_period");
	str_vec.push_back("IOExcept");
	str_vec.push_back("500");
	cmd_min_poll_period << str_vec;
	str_vec.clear();
	db_data.push_back(cmd_min_poll_period);

	try
	{
		db->put_device_property(device1_name, db_data);
	}
	catch(...)
	{
		cout << "Exception: cannot set specific properties for the device: " << device1_name << endl;
	}

	try
	{
		db->put_device_alias(device1_name, device1_alias);
	}
	catch(...)
	{
		cout << "Exception: cannot set specific properties for the device: " << device1_name << endl;
	}

	try
	{
		str = device1_name + "/" + "Short_attr";
		db->put_attribute_alias(str, attribute_alias);
	}
	catch(...)
	{
		cout << "Exception: cannot set specific properties for the device: " << device1_name << endl;
	}

	db_data.clear();
	DbDatum tst_property("tst_property");
	lg = 25;
	tst_property << lg;
	db_data.push_back(tst_property);

	try
	{
		db->put_device_property(device1_name, db_data);
	}
	catch(...)
	{
		cout << "Exception: cannot set specific properties for the device: " << device1_name << endl;
	}

// device2

	db_data.clear();
	DbDatum string_spec_attr("String_spec_attr"), string_spec_attr_label("label"), string_spec_attr_unit("unit"), string_spec_attr_format("format");

	num_prop = 3;

	string_spec_attr << num_prop;
	db_data.push_back(string_spec_attr);
	str = "Device label";
	string_spec_attr_label << str;
	db_data.push_back(string_spec_attr_label);
	str = "Device unit";
	string_spec_attr_unit << str;
	db_data.push_back(string_spec_attr_unit);
	str = "%s";
	string_spec_attr_format << str;
	db_data.push_back(string_spec_attr_format);

	try
	{
		db->put_device_attribute_property(device2_name, db_data);
	}
	catch(...)
	{
		cout << "Exception: cannot set specific properties for the device: " << device2_name << endl;
	}

	delete db;

	return 0;
}
