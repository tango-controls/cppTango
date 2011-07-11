#include <tango.h>

using namespace  Tango;

/*
 * This is an executable that configures properties of testing device servers provided as
 * command line parameters. Attribute values, their ranges and other properties are set
 * to defaults used in the test suite.
 */
int main(int argc, char **argv)
{
	if(argc < 4)
	{
		cout << "usage: " << argv[0] << " device1 device2 device3" << endl;
		exit(-1);
	}

	string device1 = argv[1], device2 = argv[2], device3 = argv[3];

	Database *db = new Database();
	DbData db_data;

	long num_prop;
	string str;
	DevLong lg;

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

	for(int i = 1; i < argc; i++)
	{
		db->put_device_attribute_property(argv[i], db_data);
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

	db->put_device_attribute_property(device1, db_data);


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

	db->put_device_attribute_property(device2, db_data);


	return 0;
}
