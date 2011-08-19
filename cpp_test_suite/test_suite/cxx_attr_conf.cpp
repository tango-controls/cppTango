#ifndef AttrConfTestSuite_h
#define AttrConfTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME AttrConfTestSuite

class AttrConfTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *device2, *device3;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		vector<string> uargs; // user arguments
		uargs.push_back("device1");
		uargs.push_back("device2");
		uargs.push_back("device3");

		vector<string> params; // parameters

		vector<string> params_opt; // optional parameters
		params_opt.push_back("loop");

		string device_name1, device_name2, device_name3;

		bool params_ok = true;
		for(size_t i = 0; i < params.size() && params_ok; i++)
			params_ok = CxxTest::TangoPrinter::is_param_set(params[i]);

		if(CxxTest::TangoPrinter::get_uargc() >= uargs.size() && params_ok)
		{
			device_name1 = CxxTest::TangoPrinter::get_uargv()[0];
			device_name2 = CxxTest::TangoPrinter::get_uargv()[1];
			device_name3 = CxxTest::TangoPrinter::get_uargv()[2];
		}
		else
		{
			cout << "usage: " << CxxTest::TangoPrinter::get_executable_name();

			for(size_t i = 0; i < uargs.size(); i++)
				cout << " " << uargs[i];

			for(size_t i = 0; i < params.size(); i++)
				cout << " " << CxxTest::TangoPrinter::get_param_def(params[i]);

			for(size_t i = 0; i < params_opt.size(); i++)
				cout << " [" << CxxTest::TangoPrinter::get_param_def(params_opt[i]) << "]";

			cout  << endl;
			exit(-1);
		}

//
// Initialization --------------------------------------------------
//

		try
		{
			device1 = new DeviceProxy(device_name1);
			device2 = new DeviceProxy(device_name2);
			device3 = new DeviceProxy(device_name3);
			device1->ping();
			device2->ping();
			device3->ping();
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
		delete device3;
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

// Test some basic exception cases

	void test_some_basic_exception_cases(void)
	{
		TS_ASSERT_THROWS_ASSERT(device1->get_attribute_config("toto"), Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrNotFound"
								&& e.errors[0].severity == Tango::ERR));

	}

// Get a full lib default conf

	void test_get_a_full_lib_default_conf(void)
	{
		AttributeInfoEx att_inf;
		att_inf = device1->get_attribute_config("Double_attr");
		TS_ASSERT(att_inf.name == "Double_attr");
		TS_ASSERT(att_inf.writable == Tango::READ);
		TS_ASSERT(att_inf.data_format == Tango::SCALAR);
		TS_ASSERT(att_inf.data_type == Tango::DEV_DOUBLE);
		TS_ASSERT(att_inf.max_dim_x == 1);
		TS_ASSERT(att_inf.max_dim_y == 0);
		TS_ASSERT(att_inf.description == "No description");
		TS_ASSERT(att_inf.label == "Double_attr");
		TS_ASSERT(att_inf.unit == "No unit");
		TS_ASSERT(att_inf.standard_unit == "No standard unit");
		TS_ASSERT(att_inf.display_unit == "No display unit");
		TS_ASSERT(att_inf.format == "%6.2f");
		TS_ASSERT(att_inf.min_value == "Not specified");
		TS_ASSERT(att_inf.max_value == "Not specified");
		TS_ASSERT(att_inf.min_alarm == "Not specified");
		TS_ASSERT(att_inf.max_alarm == "Not specified");
		TS_ASSERT(att_inf.writable_attr_name == "None"); // ?
	}

// Get a full user default conf

	void test_get_a_full_user_default_conf(void)
	{
		AttributeInfoEx att_inf;
		att_inf = device1->get_attribute_config("Long_attr_w");
		TS_ASSERT(att_inf.name == "Long_attr_w");
		TS_ASSERT(att_inf.writable == Tango::WRITE);
		TS_ASSERT(att_inf.data_format == Tango::SCALAR);
		TS_ASSERT(att_inf.data_type == Tango::DEV_LONG);
		TS_ASSERT(att_inf.max_dim_x == 1);
		TS_ASSERT(att_inf.max_dim_y == 0);
		TS_ASSERT(att_inf.description == "Test description");
		TS_ASSERT(att_inf.label == "Test label");
		TS_ASSERT(att_inf.unit == "Kilogramme");
		TS_ASSERT(att_inf.standard_unit == "100");
		TS_ASSERT(att_inf.display_unit == "Et ta soeur");
		TS_ASSERT(att_inf.format == "Tres long");
		TS_ASSERT(att_inf.min_value == "0");
		TS_ASSERT(att_inf.max_value == "100000");
		TS_ASSERT(att_inf.min_alarm == "1");
		TS_ASSERT(att_inf.max_alarm == "99999");
		TS_ASSERT(att_inf.writable_attr_name == "None");
	}

// Get a db conf with some conf defined at class level and device level

	void test_get_a_db_conf_with_some_conf_defined_at_class_level_and_device_level(void)
	{
		AttributeInfoEx att_inf;
		att_inf = device1->get_attribute_config("String_spec_attr");
		TS_ASSERT(att_inf.name == "String_spec_attr");
		TS_ASSERT(att_inf.writable == Tango::READ);
		TS_ASSERT(att_inf.data_format == Tango::SPECTRUM);
		TS_ASSERT(att_inf.data_type == Tango::DEV_STRING);
		TS_ASSERT(att_inf.max_dim_x == 2);
		TS_ASSERT(att_inf.max_dim_y == 0);
		TS_ASSERT(att_inf.description == "No description");
		TS_ASSERT(att_inf.label == "Class label");
		TS_ASSERT(att_inf.unit == "Class unit");
		TS_ASSERT(att_inf.standard_unit == "No standard unit");
		TS_ASSERT(att_inf.display_unit == "No display unit");
		TS_ASSERT(att_inf.format == "Class format");
		TS_ASSERT(att_inf.min_value == "Not specified");
		TS_ASSERT(att_inf.max_value == "Not specified");
		TS_ASSERT(att_inf.min_alarm == "Not specified");
		TS_ASSERT(att_inf.max_alarm == "Not specified");
		TS_ASSERT(att_inf.writable_attr_name == "None");

		att_inf = device2->get_attribute_config("String_spec_attr");
		TS_ASSERT(att_inf.name == "String_spec_attr");
		TS_ASSERT(att_inf.writable == Tango::READ);
		TS_ASSERT(att_inf.data_format == Tango::SPECTRUM);
		TS_ASSERT(att_inf.data_type == Tango::DEV_STRING);
		TS_ASSERT(att_inf.max_dim_x == 2);
		TS_ASSERT(att_inf.max_dim_y == 0);
		TS_ASSERT(att_inf.description == "No description");
		TS_ASSERT(att_inf.label == "Device label");
		TS_ASSERT(att_inf.unit == "Device unit");
		TS_ASSERT(att_inf.standard_unit == "No standard unit");
		TS_ASSERT(att_inf.display_unit == "No display unit");
		TS_ASSERT(att_inf.format == "%s");
		TS_ASSERT(att_inf.min_value == "Not specified");
		TS_ASSERT(att_inf.max_value == "Not specified");
		TS_ASSERT(att_inf.min_alarm == "Not specified");
		TS_ASSERT(att_inf.max_alarm == "Not specified");
		TS_ASSERT(att_inf.writable_attr_name == "None");

		att_inf = device3->get_attribute_config("String_spec_attr");
		TS_ASSERT(att_inf.name == "String_spec_attr");
		TS_ASSERT(att_inf.writable == Tango::READ);
		TS_ASSERT(att_inf.data_format == Tango::SPECTRUM);
		TS_ASSERT(att_inf.data_type == Tango::DEV_STRING);
		TS_ASSERT(att_inf.max_dim_x == 2);
		TS_ASSERT(att_inf.max_dim_y == 0);
		TS_ASSERT(att_inf.description == "No description");
		TS_ASSERT(att_inf.label == "Class label");
		TS_ASSERT(att_inf.unit == "Class unit");
		TS_ASSERT(att_inf.standard_unit == "No standard unit");
		TS_ASSERT(att_inf.display_unit == "No display unit");
		TS_ASSERT(att_inf.format == "Class format");
		TS_ASSERT(att_inf.min_value == "Not specified");
		TS_ASSERT(att_inf.max_value == "Not specified");
		TS_ASSERT(att_inf.min_alarm == "Not specified");
		TS_ASSERT(att_inf.max_alarm == "Not specified");
		TS_ASSERT(att_inf.writable_attr_name == "None");
	}

// Test several attributes in one call

	void test_several_attributes_in_one_call(void)
	{
		AttributeInfoListEx *att_inf_list;
		AttributeInfoEx att_inf;
		vector<string> attributes;
		attributes.push_back("String_spec_attr");
		attributes.push_back("Long_attr_w");
		attributes.push_back("Double_attr");

		att_inf_list = device1->get_attribute_config_ex(attributes);

		att_inf = (*att_inf_list)[0];
		TS_ASSERT(att_inf.name == "String_spec_attr");
		TS_ASSERT(att_inf.writable == Tango::READ);
		TS_ASSERT(att_inf.data_format == Tango::SPECTRUM);
		TS_ASSERT(att_inf.data_type == Tango::DEV_STRING);
		TS_ASSERT(att_inf.max_dim_x == 2);
		TS_ASSERT(att_inf.max_dim_y == 0);
		TS_ASSERT(att_inf.description == "No description");
		TS_ASSERT(att_inf.label == "Class label");
		TS_ASSERT(att_inf.unit == "Class unit");
		TS_ASSERT(att_inf.standard_unit == "No standard unit");
		TS_ASSERT(att_inf.display_unit == "No display unit");
		TS_ASSERT(att_inf.format == "Class format");
		TS_ASSERT(att_inf.min_value == "Not specified");
		TS_ASSERT(att_inf.max_value == "Not specified");
		TS_ASSERT(att_inf.min_alarm == "Not specified");
		TS_ASSERT(att_inf.max_alarm == "Not specified");
		TS_ASSERT(att_inf.writable_attr_name == "None");

		att_inf = (*att_inf_list)[1];
		TS_ASSERT(att_inf.name == "Long_attr_w");
		TS_ASSERT(att_inf.writable == Tango::WRITE);
		TS_ASSERT(att_inf.data_format == Tango::SCALAR);
		TS_ASSERT(att_inf.data_type == Tango::DEV_LONG);
		TS_ASSERT(att_inf.max_dim_x == 1);
		TS_ASSERT(att_inf.max_dim_y == 0);
		TS_ASSERT(att_inf.description == "Test description");
		TS_ASSERT(att_inf.label == "Test label");
		TS_ASSERT(att_inf.unit == "Kilogramme");
		TS_ASSERT(att_inf.standard_unit == "100");
		TS_ASSERT(att_inf.display_unit == "Et ta soeur");
		TS_ASSERT(att_inf.format == "Tres long");
		TS_ASSERT(att_inf.min_value == "0");
		TS_ASSERT(att_inf.max_value == "100000");
		TS_ASSERT(att_inf.min_alarm == "1");
		TS_ASSERT(att_inf.max_alarm == "99999");
		TS_ASSERT(att_inf.writable_attr_name == "None");

		att_inf = (*att_inf_list)[2];
		TS_ASSERT(att_inf.name == "Double_attr");
		TS_ASSERT(att_inf.writable == Tango::READ);
		TS_ASSERT(att_inf.data_format == Tango::SCALAR);
		TS_ASSERT(att_inf.data_type == Tango::DEV_DOUBLE);
		TS_ASSERT(att_inf.max_dim_x == 1);
		TS_ASSERT(att_inf.max_dim_y == 0);
		TS_ASSERT(att_inf.description == "No description");
		TS_ASSERT(att_inf.label == "Double_attr");
		TS_ASSERT(att_inf.unit == "No unit");
		TS_ASSERT(att_inf.standard_unit == "No standard unit");
		TS_ASSERT(att_inf.display_unit == "No display unit");
		TS_ASSERT(att_inf.format == "%6.2f");
		TS_ASSERT(att_inf.min_value == "Not specified");
		TS_ASSERT(att_inf.max_value == "Not specified");
		TS_ASSERT(att_inf.min_alarm == "Not specified");
		TS_ASSERT(att_inf.max_alarm == "Not specified");
		TS_ASSERT(att_inf.writable_attr_name == "None");
	}

// Add one attribute on the fly and get its config

	void test_add_one_attribute_on_the_fly_and_get_its_config(void)
	{
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOAddAttribute"));

		AttributeInfoEx att_inf;
		att_inf = device1->get_attribute_config("Added_short_attr");
		TS_ASSERT(att_inf.name == "Added_short_attr");
		TS_ASSERT(att_inf.writable == Tango::READ);
		TS_ASSERT(att_inf.data_format == Tango::SCALAR);
		TS_ASSERT(att_inf.data_type == Tango::DEV_SHORT);
		TS_ASSERT(att_inf.max_dim_x == 1);
		TS_ASSERT(att_inf.max_dim_y == 0);
		TS_ASSERT(att_inf.description == "Test description");
		TS_ASSERT(att_inf.label == "From db (class)");
		TS_ASSERT(att_inf.unit == "No unit");
		TS_ASSERT(att_inf.standard_unit == "No standard unit");
		TS_ASSERT(att_inf.display_unit == "No display unit");
		TS_ASSERT(att_inf.format == "From db (device)");
		TS_ASSERT(att_inf.min_value == "Not specified");
		TS_ASSERT(att_inf.max_value == "Not specified");
		TS_ASSERT(att_inf.min_alarm == "Not specified");
		TS_ASSERT(att_inf.max_alarm == "Not specified");
		TS_ASSERT(att_inf.writable_attr_name == "None");

		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOAddAttribute")); // ?
	}
};
#undef cout
#endif // AttrConfTestSuite_h
