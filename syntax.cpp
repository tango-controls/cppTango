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

//
// First check device name syntax when creating DeviceProxy instance
//
	
	val = check_proxy("corvus:10000/a#dbase=no");
	assert (val == 0);
	
	val = check_proxy("corvus:10000/a/b#dbase=no");
	assert (val == 0);
	
	val = check_proxy("corvus:10000/a//b#dbase=no");
	assert (val == 0);
	
	val = check_proxy("corvus:10000/a/b//#dbase=no");
	assert (val == 0);
	
	val = check_proxy("corvus:10000/a/b/c/#dbase=no");
	assert (val == 0);
	
	val = check_proxy("corvus:10000/a/b/c#dbase=no");
	assert (val == 1);
	
	val = check_proxy("corvus:10000/a/b");
	assert (val == 0);
	
	val = check_proxy("corvus:10000/a//b");
	assert (val == 0);
	
	val = check_proxy("corvus:10000/a/b//");
	assert (val == 0);
	
	val = check_proxy("corvus:10000/a/b/c/");
	assert (val == 0);
	
	val = check_proxy("corvus:10000/a/b/c");
	assert (val == 1);

	val = check_proxy("//corvus:10000/a/b/c");
	assert (val == 1);
		
	val = check_proxy("//corvus:10000/a/b/c#dbase=no");
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

	val = check_proxy("corvus:10000/my_alias:");
	assert (val == 0);
	
	val = check_proxy("corvus:10000/my_al->ias");
	assert (val == 0);
	
	val = check_proxy("corvus:10000/my_alias");
	assert (val == 1);

	val = check_proxy("//corvus:10000/my_alias");
	assert (val == 1);
		
//
// Finally, connect to a device via its alias
//

	val = check_proxy("et_alias");
	assert (val == 2);
	DeviceProxy dev("et_alias");
	string na = dev.name();
	assert (na == "dev/test/10");
	DbData db_dat;
	db_dat.push_back(DbDatum("tst_property"));
	dev.get_property(db_dat);
	long prop_val;
	db_dat[0] >> prop_val;
	assert (prop_val == 25);
	cout << "   Connecting to device via alias like my_alias --> OK" << endl;
	
	val = check_proxy("corvus:10000/et_alias");
	assert (val == 2);
	DeviceProxy dev1("corvus:10000/et_alias");
	na = dev1.name();
	assert (na == "dev/test/10");
	dev1.get_property(db_dat);
	long prop_val_1;
	db_dat[0] >> prop_val_1;
	assert (prop_val_1 == 25);
	cout << "   Connecting to device via alias like host:port/my_alias --> OK" << endl;

//
// Connect to device via its classical name
//

	val = check_proxy("dev/test/10");
	assert (val == 2);
	cout << "   Connecting to device via it classical name --> OK" << endl;

//
// Then, check attribute name syntax when creating AttributeProxy instance
//
	
	val = attr_check_proxy("corvus:10000/a#dbase=no");
	assert (val == 0);
	
	val = attr_check_proxy("corvus:10000/a/b#dbase=no");
	assert (val == 0);
	
	val = attr_check_proxy("corvus:10000/a//b#dbase=no");
	assert (val == 0);
	
	val = attr_check_proxy("corvus:10000/a/b//#dbase=no");
	assert (val == 0);
	
	val = attr_check_proxy("corvus:10000/a/b/c/#dbase=no");
	assert (val == 0);
	
	val = attr_check_proxy("corvus:10000/a/b/c/d#dbase=no");
	assert (val == 2);
	
	val = attr_check_proxy("corvus:10000/a/b");
	assert (val == 2);
	
	val = attr_check_proxy("corvus:10000/a//b");
	assert (val == 0);
	
	val = attr_check_proxy("corvus:10000/a/b//");
	assert (val == 0);
	
	val = attr_check_proxy("corvus:10000/a/b/c/");
	assert (val == 0);
	
	val = attr_check_proxy("corvus:10000/a/b/c/d");
	assert (val == 2);
	
	val = attr_check_proxy("//corvus:10000/a/b/c/d");
	assert (val == 2);
		
	val = attr_check_proxy("//corvus:10000/a/b/c/d#dbase=no");
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

	val = attr_check_proxy("dev/test/10/aaaa");
	assert (val == 1);
		
	val = attr_check_proxy("my_alias:");
	assert (val == 0);
	
	val = attr_check_proxy("my_al->ias");
	assert (val == 0);
	
	val =  attr_check_proxy("my_alias");
	assert (val == 2);

	val = attr_check_proxy("corvus:10000/my_alias:");
	assert (val == 0);
	
	val = attr_check_proxy("corvus:10000/my_al->ias");
	assert (val == 0);
	
	val = attr_check_proxy("corvus:10000/my_alias");
	assert (val == 2);

	val = attr_check_proxy("//corvus:10000/my_alias");
	assert (val == 2);
	
	val = attr_check_proxy("et_alias/Short_attr");
	assert (val == 3);
	AttributeProxy att2("et_alias/Short_attr");
	string na_dev = att2.get_device_proxy()->name();
	assert (na_dev == "dev/test/10");
	
	cout << "   Connecting to attribute via device_alias/attribute_name --> OK" << endl;
			
//
// Finally, connect to an attribute via its alias
//

	val = attr_check_proxy("et_attr_alias");
	assert (val == 3);
	
	AttributeProxy att("et_attr_alias");
	na_dev = att.get_device_proxy()->name();
	assert (na_dev == "dev/test/10");
	
	cout << "   Connecting to attribute via alias like et_attr_alias --> OK" << endl;
	
	val = attr_check_proxy("corvus:10000/et_attr_alias");
	assert (val == 3);
	
	AttributeProxy att1("corvus:10000/et_attr_alias");
	na_dev = att1.get_device_proxy()->name();
	assert (na_dev == "dev/test/10");

	cout << "   Connecting to attribute via alias like host:port/et_attr_alias --> OK" << endl;

//
// Check one of the alias call
//

	DeviceProxy dev_al("dev/test/10");
	string al = dev_al.alias();
	assert (al == "et_alias");
	
	try
	{	
		DeviceProxy dev2("dev/test/11");
		al = dev2.alias();
	}
	catch (Tango::DevFailed &e)
	{
		string rea(e.errors[0].reason.in());
		assert(rea == "DB_AlaiasNotDefined");
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
