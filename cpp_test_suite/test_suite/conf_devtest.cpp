#include <tango.h>

using namespace  Tango;

/*
 * This is a utility that configures properties of test device servers provided as
 * command line parameters. Attribute values, their ranges and other properties are
 * set to defaults used in the test suite.
 */
int main(int argc, char **argv)
{
	if(argc < 5)
	{
		cout << "usage: " << argv[0] << " dserver device1 device2 device3" << endl;
		exit(-1);
	}

	string dserver_name = argv[1], device1_name = argv[2], device2_name = argv[3], device3_name = argv[4];

	Database *db = new Database();
	DbData db_data;

	long num_prop;
	string str;
	DevLong lg;
	DeviceData din;

//
// dserver properties
//

	DbDatum logging_level("logging_level");
	string logging_str = "WARNING";
	logging_level << logging_str;
	db_data.push_back(logging_level);

	db->put_device_property(dserver_name, db_data);

	db_data.clear();

//
// Properties common to all devices
//

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

	db_data.clear();

//
// Device specific properties
//

// device1

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

// device2

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
