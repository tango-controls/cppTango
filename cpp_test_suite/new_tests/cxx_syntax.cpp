#ifndef SyntaxTestSuite_h
#define SyntaxTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME SyntaxTestSuite

class SyntaxTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *device2;
	string device1_name, device2_name, tango_host, device_alias, attribute_alias;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		device1_name = CxxTest::TangoPrinter::get_param("device1");
		device2_name = CxxTest::TangoPrinter::get_param("device2");
		device_alias = CxxTest::TangoPrinter::get_param("devicealias");
		attribute_alias = CxxTest::TangoPrinter::get_param("attributealias");

		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

		try
		{
			device1 = new DeviceProxy(device1_name);
			device2 = new DeviceProxy(device2_name);

			ApiUtil *api = ApiUtil::instance();
			api->get_env_var("TANGO_HOST", tango_host);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{
		delete device1;
		delete device2;
	}

	static SUITE_NAME *createSuite()
	{
		return new SUITE_NAME();
	}

	static void destroySuite(SUITE_NAME *suite)
	{
		delete suite;
	}

//
// Tests -------------------------------------------------------
//

// Helper functions

	unsigned int check_proxy(string name)
	{
		unsigned int ret;

		try
		{
			DeviceProxy dev(name);
			ret = 2;
		}
		catch (DevFailed &e)
		{
			if (strcmp(e.errors[0].reason.in(),API_WrongDeviceNameSyntax) == 0)
				ret = 0;
			else
				ret = 1;
		}

		return ret;
	}

	unsigned int attr_check_proxy(string name)
	{
		unsigned int ret;

		try
		{
			AttributeProxy attr(name);
			ret = 3;
		}
		catch (DevFailed &e)
		{
			if (strcmp(e.errors[0].reason.in(),API_WrongAttributeNameSyntax) == 0)
				ret = 0;
			else if (strcmp(e.errors[0].reason.in(),API_UnsupportedAttribute) == 0)
				ret = 1;
			else
				ret = 2;
		}

		return ret;
	}

// Check device name syntax when creating DeviceProxy instance

	void test_check_device_name_syntax_when_creating_DeviceProxy_instance(void)
	{
		TS_ASSERT(check_proxy(tango_host + "/a#dbase=no") == 0);
		TS_ASSERT(check_proxy(tango_host + "/a/b#dbase=no") == 0);
		TS_ASSERT(check_proxy(tango_host + "/a//b#dbase=no") == 0);
		TS_ASSERT(check_proxy(tango_host + "/a/b//#dbase=no") == 0);
		TS_ASSERT(check_proxy(tango_host + "/a/b/c/#dbase=no") == 0);
		TS_ASSERT(check_proxy(tango_host + "/a/b/c#dbase=no") == 1);
		TS_ASSERT(check_proxy(tango_host + "/a/b") == 0);
		TS_ASSERT(check_proxy(tango_host + "/a//b") == 0);
		TS_ASSERT(check_proxy(tango_host + "/a/b//") == 0);
		TS_ASSERT(check_proxy(tango_host + "/a/b/c/") == 0);
		TS_ASSERT(check_proxy(tango_host + "/a/b/c") == 1);
		TS_ASSERT(check_proxy("//" + tango_host + "/a/b/c") == 1);
		TS_ASSERT(check_proxy("//" + tango_host + "/a/b/c#dbase=no") == 1);
		TS_ASSERT(check_proxy("/a/b") == 0);
		TS_ASSERT(check_proxy("a/b") == 0);
		TS_ASSERT(check_proxy("a//b") == 0);
		TS_ASSERT(check_proxy("a/b//") == 0);
		TS_ASSERT(check_proxy("a/b/c/") == 0);
		TS_ASSERT(check_proxy("a/b/c") == 1);
		TS_ASSERT(check_proxy("my_alias:") == 0);
		TS_ASSERT(check_proxy("my_al->ias") == 0);
		TS_ASSERT(check_proxy("my_alias") == 1);
		TS_ASSERT(check_proxy(tango_host + "/my_alias:") == 0);
		TS_ASSERT(check_proxy(tango_host + "/my_al->ias") == 0);
		TS_ASSERT(check_proxy(tango_host + "/my_alias") == 1);
		TS_ASSERT(check_proxy("//" + tango_host + "/my_alias") == 1);
	}

// Connect to a device via its alias as my_alias

	void test_connect_to_a_device_via_its_alias_as_my_alias(void)
	{
		TS_ASSERT(check_proxy(device_alias) == 2);
		DeviceProxy *device = nullptr;
		TS_ASSERT_THROWS_NOTHING(device = new DeviceProxy(device_alias));
		TS_ASSERT(device->name() == device1_name);

		DbData db_data;
		long prop_val;
		db_data.push_back(DbDatum("tst_property"));
		TS_ASSERT_THROWS_NOTHING(device->get_property(db_data));
		delete device;
		db_data[0] >> prop_val;
		TS_ASSERT(prop_val == 25);
	}

// Connect to a device via its alias as host:port/my_alias

	void test_connect_to_a_device_via_its_alias_as_host_port_my_alias(void)
	{
		string device_name = tango_host + "/" + device_alias;
		TS_ASSERT(check_proxy(device_name) == 2);
		DeviceProxy *device = nullptr;
		TS_ASSERT_THROWS_NOTHING(device = new DeviceProxy(device_name));
		TS_ASSERT(device->name() == device1_name);

		DbData db_data;
		long prop_val;
		db_data.push_back(DbDatum("tst_property"));
		TS_ASSERT_THROWS_NOTHING(device->get_property(db_data));
		delete device;
		db_data[0] >> prop_val;
		TS_ASSERT(prop_val == 25);
	}

// Connect to a device via its classic name

	void test_connect_to_a_device_via_its_classic_name(void)
	{
		TS_ASSERT(check_proxy(device1_name) == 2);
	}

// Check attribute name syntax when creating AttributeProxy instance

	void test_check_attribute_name_syntax_when_creating_AttributeProxy_instance(void)
	{
		TS_ASSERT(attr_check_proxy(tango_host + "/a#dbase=no") == 0);
		TS_ASSERT(attr_check_proxy(tango_host + "/a/b#dbase=no") == 0);
		TS_ASSERT(attr_check_proxy(tango_host + "/a//b#dbase=no") == 0);
		TS_ASSERT(attr_check_proxy(tango_host + "/a/b//#dbase=no") == 0);
		TS_ASSERT(attr_check_proxy(tango_host + "/a/b/c/#dbase=no") == 0);
		TS_ASSERT(attr_check_proxy(tango_host + "/a/b/c/d#dbase=no") == 2);
		TS_ASSERT(attr_check_proxy(tango_host + "/a/b") == 2);
		TS_ASSERT(attr_check_proxy(tango_host + "/a//b") == 0);
		TS_ASSERT(attr_check_proxy(tango_host + "/a/b//") == 0);
		TS_ASSERT(attr_check_proxy(tango_host + "/a/b/c/") == 0);
		TS_ASSERT(attr_check_proxy(tango_host + "/a/b/c/d") == 2);
		TS_ASSERT(attr_check_proxy("//" + tango_host + "/a/b/c/d") == 2);
		TS_ASSERT(attr_check_proxy("//" + tango_host + "/a/b/c/d#dbase=no") == 2);
		TS_ASSERT(attr_check_proxy("/a/b") == 0);
		TS_ASSERT(attr_check_proxy("a/b") == 2);
		TS_ASSERT(attr_check_proxy("a//b") == 0);
		TS_ASSERT(attr_check_proxy("a/b//") == 0);
		TS_ASSERT(attr_check_proxy("a/b/c/") == 0);
		TS_ASSERT(attr_check_proxy("a/b/c/d") == 2);
		TS_ASSERT(attr_check_proxy(device1_name + "/aaaa") == 1);
		TS_ASSERT(attr_check_proxy("my_alias:") == 0);
		TS_ASSERT(attr_check_proxy("my_al->ias") == 0);
		TS_ASSERT(attr_check_proxy("my_alias") == 2);
		TS_ASSERT(attr_check_proxy(tango_host + "/my_alias:") == 0);
		TS_ASSERT(attr_check_proxy(tango_host + "/my_al->ias") == 0);
		TS_ASSERT(attr_check_proxy(tango_host + "/my_alias") == 2);
		TS_ASSERT(attr_check_proxy("//" + tango_host + "/my_alias") == 2);
	}

// Connect to an attribute via its alias as attribute_alias

	void test_connect_to_an_attribute_via_its_alias_as_attribute_alias(void)
	{
		TS_ASSERT(attr_check_proxy(attribute_alias) == 3);
		AttributeProxy *attribute = nullptr;
		TS_ASSERT_THROWS_NOTHING(attribute = new AttributeProxy(attribute_alias));
		TS_ASSERT(attribute->get_device_proxy()->name() == device1_name);
		delete attribute;
	}

// Connect to an attribute via its alias as host:port/attribute_alias

	void test_connect_to_an_attribute_via_its_alias_as_host_port_attribute_alias(void)
	{
		string attribute_name = tango_host + "/" + attribute_alias;
		TS_ASSERT(attr_check_proxy(attribute_name) == 3);
		AttributeProxy *attribute = nullptr;
		TS_ASSERT_THROWS_NOTHING(attribute = new AttributeProxy(attribute_name));
		TS_ASSERT(attribute->get_device_proxy()->name() == device1_name);
		delete attribute;
	}

// Check alias call

	void test_check_alias_call(void)
	{
		DeviceProxy *device1 = nullptr;
		TS_ASSERT_THROWS_NOTHING(device1 = new DeviceProxy(device1_name));
		TS_ASSERT(device1->alias() == device_alias);
		delete device1;

		DeviceProxy *device2 = nullptr;
		TS_ASSERT_THROWS_NOTHING(device2 = new DeviceProxy(device2_name));
		TS_ASSERT_THROWS_ASSERT(device2->alias(), Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == DB_AliasNotDefined
								&& e.errors[0].severity == Tango::ERR));
		delete device2;
	}
};
#undef cout
#endif // SyntaxTestSuite_h
