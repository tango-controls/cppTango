/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

long check_proxy(const char *);
long attr_check_proxy(const char *);

using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	long val;

	if (argc < 5)
	{
		cout << "usage: syntax <device1> <device2> <device1_alias> <attr_alias>" << endl;
		exit(-1);
	}

	string device1_name = argv[1];
	string device2_name = argv[2];
	string device1_alias = argv[3];
	string attr_alias = argv[4];

//	try
//	{
//		// connect to device
//		device = new DeviceProxy(device1_name);
//
//		// Connect to admin device
//		string adm_name = device->adm_name();
//		admin = new DeviceProxy(adm_name);
//	}
//	catch (CORBA::Exception &e)
//	{
//		Except::print_exception(e);
//		exit(1);
//	}
//
//	admin-> // command in out - info - to get host and port

//
// First check device name syntax when creating DeviceProxy instance
//
	
	val = check_proxy("kidiboo:10000/a#dbase=no");
	assert (val == 0);
	
	val = check_proxy("kidiboo:10000/a/b#dbase=no");
	assert (val == 0);
	
	val = check_proxy("kidiboo:10000/a//b#dbase=no");
	assert (val == 0);
	
	val = check_proxy("kidiboo:10000/a/b//#dbase=no");
	assert (val == 0);
	
	val = check_proxy("kidiboo:10000/a/b/c/#dbase=no");
	assert (val == 0);
	
	val = check_proxy("kidiboo:10000/a/b/c#dbase=no");
	assert (val == 1);
	
	val = check_proxy("kidiboo:10000/a/b");
	assert (val == 0);
	
	val = check_proxy("kidiboo:10000/a//b");
	assert (val == 0);
	
	val = check_proxy("kidiboo:10000/a/b//");
	assert (val == 0);
	
	val = check_proxy("kidiboo:10000/a/b/c/");
	assert (val == 0);
	
	val = check_proxy("kidiboo:10000/a/b/c");
	assert (val == 1);

	val = check_proxy("//kidiboo:10000/a/b/c");
	assert (val == 1);
		
	val = check_proxy("//kidiboo:10000/a/b/c#dbase=no");
	assert (val == 1);
	
	val = check_proxy("/a/b");
	assert (val == 0);

	val = check_proxy("a/b");
	assert (val == 0);
		
	val = check_proxy("a//b");
	assert (val == 0);
	
	val = check_proxy("a/b//");
	assert (val == 0);
	
	val = check_proxy("a/b/c/");
	assert (val == 0);
	
	val = check_proxy("a/b/c");
	assert (val == 1);
	
	val = check_proxy("my_alias:");
	assert (val == 0);
	
	val = check_proxy("my_al->ias");
	assert (val == 0);
	
	val =  check_proxy("my_alias");
	assert (val == 1);

	val = check_proxy("kidiboo:10000/my_alias:");
	assert (val == 0);
	
	val = check_proxy("kidiboo:10000/my_al->ias");
	assert (val == 0);
	
	val = check_proxy("kidiboo:10000/my_alias");
	assert (val == 1);

	val = check_proxy("//kidiboo:10000/my_alias");
	assert (val == 1);
		
//
// Finally, connect to a device via its alias
//

	val = check_proxy(device1_alias.c_str());
	assert (val == 2);
	DeviceProxy dev(device1_alias);
	string na = dev.name();
	assert (na == device1_name);
	DbData db_dat;
	db_dat.push_back(DbDatum("tst_property"));
	dev.get_property(db_dat);
	long prop_val;
	db_dat[0] >> prop_val;
	assert (prop_val == 25);
	cout << "   Connecting to device via alias like my_alias --> OK" << endl;

	string tmp_name("kidiboo:10000/");
	tmp_name = tmp_name + device1_alias;
	
	val = check_proxy(tmp_name.c_str());
	assert (val == 2);
	DeviceProxy dev1(tmp_name);
	na = dev1.name();
	assert (na == device1_name);
	dev1.get_property(db_dat);
	long prop_val_1;
	db_dat[0] >> prop_val_1;
	assert (prop_val_1 == 25);
	cout << "   Connecting to device via alias like host:port/my_alias --> OK" << endl;

//
// Connect to device via its classical name
//

	val = check_proxy(device1_name.c_str());
	assert (val == 2);
	cout << "   Connecting to device via it classical name --> OK" << endl;

//
// Then, check attribute name syntax when creating AttributeProxy instance
//
	
	val = attr_check_proxy("kidiboo:10000/a#dbase=no");
	assert (val == 0);
	
	val = attr_check_proxy("kidiboo:10000/a/b#dbase=no");
	assert (val == 0);
	
	val = attr_check_proxy("kidiboo:10000/a//b#dbase=no");
	assert (val == 0);
	
	val = attr_check_proxy("kidiboo:10000/a/b//#dbase=no");
	assert (val == 0);
	
	val = attr_check_proxy("kidiboo:10000/a/b/c/#dbase=no");
	assert (val == 0);
	
	val = attr_check_proxy("kidiboo:10000/a/b/c/d#dbase=no");
	assert (val == 2);
	
	val = attr_check_proxy("kidiboo:10000/a/b");
	assert (val == 2);
	
	val = attr_check_proxy("kidiboo:10000/a//b");
	assert (val == 0);
	
	val = attr_check_proxy("kidiboo:10000/a/b//");
	assert (val == 0);
	
	val = attr_check_proxy("kidiboo:10000/a/b/c/");
	assert (val == 0);
	
	val = attr_check_proxy("kidiboo:10000/a/b/c/d");
	assert (val == 2);
	
	val = attr_check_proxy("//kidiboo:10000/a/b/c/d");
	assert (val == 2);
		
	val = attr_check_proxy("//kidiboo:10000/a/b/c/d#dbase=no");
	assert (val == 2);
	
	val = attr_check_proxy("/a/b");
	assert (val == 0);

	val = attr_check_proxy("a/b");
	assert (val == 2);
		
	val = attr_check_proxy("a//b");
	assert (val == 0);
	
	val = attr_check_proxy("a/b//");
	assert (val == 0);
	
	val = attr_check_proxy("a/b/c/");
	assert (val == 0);
	
	val = attr_check_proxy("a/b/c/d");
	assert (val == 2);

	string att_name = device1_name;
	att_name = att_name + "/aaaa";
	val = attr_check_proxy(att_name.c_str());
	assert (val == 1);
		
	val = attr_check_proxy("my_alias:");
	assert (val == 0);
	
	val = attr_check_proxy("my_al->ias");
	assert (val == 0);
	
	val =  attr_check_proxy("my_alias");
	assert (val == 2);

	val = attr_check_proxy("kidiboo:10000/my_alias:");
	assert (val == 0);
	
	val = attr_check_proxy("kidiboo:10000/my_al->ias");
	assert (val == 0);
	
	val = attr_check_proxy("kidiboo:10000/my_alias");
	assert (val == 2);

	val = attr_check_proxy("//kidiboo:10000/my_alias");
	assert (val == 2);

	tmp_name = device1_alias;
	tmp_name = tmp_name + "/Short_attr";
	
	val = attr_check_proxy(tmp_name.c_str());
	assert (val == 3);
	AttributeProxy att2(tmp_name);
	string na_dev = att2.get_device_proxy()->name();
	assert (na_dev == device1_name);
	
	cout << "   Connecting to attribute via device_alias/attribute_name --> OK" << endl;
			
//
// Finally, connect to an attribute via its alias
//

	val = attr_check_proxy(attr_alias.c_str());
	assert (val == 3);

	AttributeProxy att(attr_alias);
	na_dev = att.get_device_proxy()->name();
	assert (na_dev == device1_name);

	cout << "   Connecting to attribute via alias like et_attr_alias --> OK" << endl;

	tmp_name = "Kidiboo:10000/";
	tmp_name = tmp_name + attr_alias;
	
	val = attr_check_proxy(tmp_name.c_str());
	assert (val == 3);
	
	AttributeProxy att1(tmp_name);
	na_dev = att1.get_device_proxy()->name();
	assert (na_dev == device1_name);

	cout << "   Connecting to attribute via alias like host:port/et_attr_alias --> OK" << endl;

//
// Check one of the alias call
//

	DeviceProxy dev_al(device1_name);
	string al = dev_al.alias();
	assert (al == device1_alias);
	
	try
	{	
		DeviceProxy dev2(device2_name);
		al = dev2.alias();
	}
	catch (Tango::DevFailed &e)
	{
		string rea(e.errors[0].reason.in());
		assert(rea == "DB_AliasNotDefined");
	}
		
	cout << "   Get device alias from its name --> OK" << endl;
	
	return 0;
}


long check_proxy(const char *name)
{
	DeviceProxy *dev;
	long ret;
	
	try
	{
		dev = new DeviceProxy(name);
		ret = 2;
	}
	catch (DevFailed &e)
	{
		if (strcmp(e.errors[0].reason.in(),"API_WrongDeviceNameSyntax") == 0)
			ret = 0;
		else
			ret = 1;
	}
	
	return ret;
}

long attr_check_proxy(const char *name)
{
	AttributeProxy *attr;
	long ret;
	
	try
	{
		attr = new AttributeProxy(name);
		ret = 3;
	}
	catch (DevFailed &e)
	{
		if (strcmp(e.errors[0].reason.in(),"API_WrongAttributeNameSyntax") == 0)
			ret = 0;
		else if (strcmp(e.errors[0].reason.in(),"API_UnsupportedAttribute") == 0)
			ret = 1;
		else
			ret = 2;
	}
	
	return ret;
}
