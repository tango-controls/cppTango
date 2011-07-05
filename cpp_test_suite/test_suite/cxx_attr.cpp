#ifndef AttrTestSuite_h
#define AttrTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#undef SUITE_NAME
#define SUITE_NAME AttrTestSuite

class AttrTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device;

public:
	SUITE_NAME()
	{

		cout << endl;

//
// Arguments check -------------------------------------------------
//

		string device_name = "";

		if(CxxTest::TangoPrinter::get_uargc() > 0)
		{
			device_name = CxxTest::TangoPrinter::get_uargv()[0];
		}
		else
		{
			string param = "loop"; // parameter name found in TangoPrinter::params map
			cout << "usage: %s device";
			// checks if the param was defined in TangoPrinter::params map
			if(CxxTest::TangoPrinter::is_param_defined(param))
			{
				// prints out the definition of the param (in this case "--loop=")
				cout << " [" << CxxTest::TangoPrinter::get_param_def(param) <<"]";
			}
			cout  << endl;
			exit(-1);
		}

//
// Initialization --------------------------------------------------
//

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

	}

	virtual ~SUITE_NAME()
	{
		cout << endl;
		delete device;
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

// Test one attribute at a time for all SCALAR types

	void test_one_attribute_at_a_time_for_all_SCALAR_types(void)
	{
		DeviceAttribute short_attr, long_attr, double_attr, string_attr;
		short sh;
		long lg;
		double db;
		string str;

		short_attr = device->read_attribute("Short_attr");
		TS_ASSERT(short_attr.get_name() == "Short_attr");
		TS_ASSERT(short_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr.get_dim_x() == 1);
		TS_ASSERT(short_attr.get_dim_y() == 0);
		short_attr >> sh;
		TS_ASSERT(sh == 12);

		long_attr = device->read_attribute("Long_attr");
		TS_ASSERT(long_attr.get_name() == "Long_attr");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr.get_dim_x() == 1);
		TS_ASSERT(long_attr.get_dim_y() == 0);
		long_attr >> lg;
		TS_ASSERT(lg == 1246);

		double_attr = device->read_attribute("Double_attr");
		TS_ASSERT(double_attr.get_name() == "Double_attr");
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 1);
		TS_ASSERT(double_attr.get_dim_y() == 0);
		double_attr >> db;
		TS_ASSERT(db == 3.2);

		string_attr = device->read_attribute("String_attr");
		TS_ASSERT(string_attr.get_name() == "String_attr");
		TS_ASSERT(string_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(string_attr.get_dim_x() == 1);
		TS_ASSERT(string_attr.get_dim_y() == 0);
		string_attr >> str;
		TS_ASSERT(str == "test_string");
	}

// Test several SCALAR attributes in one call

	void test_several_SCALAR_attributes_in_one_call(void)
	{
		vector<DeviceAttribute> *attr_vec;
		DeviceAttribute short_attr, long_attr, double_attr, string_attr;
		vector<string> name_vec;
		short sh;
		long lg;
		double db;
		string str;

		name_vec.push_back("String_attr");
		name_vec.push_back("Double_attr");

		attr_vec = device->read_attributes(name_vec);

		string_attr = (*attr_vec)[0];
		TS_ASSERT(string_attr.get_name() == "String_attr");
		TS_ASSERT(string_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(string_attr.get_dim_x() == 1);
		TS_ASSERT(string_attr.get_dim_y() == 0);
		string_attr >> str;
		TS_ASSERT(str == "test_string");

		double_attr = (*attr_vec)[1];
		TS_ASSERT(double_attr.get_name() == "Double_attr");
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 1);
		TS_ASSERT(double_attr.get_dim_y() == 0);
		double_attr >> db;
		TS_ASSERT(db == 3.2);

		name_vec.push_back("Long_attr");
		name_vec.push_back("Short_attr");

		attr_vec = device->read_attributes(name_vec);

		string_attr = (*attr_vec)[0];
		TS_ASSERT(string_attr.get_name() == "String_attr");
		TS_ASSERT(string_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(string_attr.get_dim_x() == 1);
		TS_ASSERT(string_attr.get_dim_y() == 0);
		string_attr >> str;
		TS_ASSERT(str == "test_string");

		double_attr = (*attr_vec)[1];
		TS_ASSERT(double_attr.get_name() == "Double_attr");
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 1);
		TS_ASSERT(double_attr.get_dim_y() == 0);
		double_attr >> db;
		TS_ASSERT(db == 3.2);

		long_attr = (*attr_vec)[2];
		TS_ASSERT(long_attr.get_name() == "Long_attr");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr.get_dim_x() == 1);
		TS_ASSERT(long_attr.get_dim_y() == 0);
		long_attr >> lg;
		TS_ASSERT(lg == 1246);

		short_attr = (*attr_vec)[3];
		TS_ASSERT(short_attr.get_name() == "Short_attr");
		TS_ASSERT(short_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr.get_dim_x() == 1);
		TS_ASSERT(short_attr.get_dim_y() == 0);
		short_attr >> sh;
		TS_ASSERT(sh == 12);
	}

// Test one attribute at a time for all SPECTRUM types

	void test_one_attribute_at_a_time_for_all_SPECTRUM_types(void)
	{
		DeviceAttribute short_attr, long_attr, double_attr, string_attr;
		vector<short> sh;
		vector<long> lg;
		vector<double> db;
		vector<string> str;

		short_attr = device->read_attribute("Short_spec_attr");
		TS_ASSERT(short_attr.get_name() == "Short_spec_attr");
		TS_ASSERT(short_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr.get_dim_x() == 4);
		TS_ASSERT(short_attr.get_dim_y() == 0);
		short_attr >> sh;
		TS_ASSERT(sh[0] == 10 && sh[1] == 20 && sh[2] == 30 && sh[3] == 40);

		long_attr = device->read_attribute("Long_spec_attr");
		TS_ASSERT(long_attr.get_name() == "Long_spec_attr");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr.get_dim_x() == 10);
		TS_ASSERT(long_attr.get_dim_y() == 0);
		long_attr >> lg;
		TS_ASSERT(lg[0] == 0 && lg[1] == 1 && lg[2] == 2 && lg[3] == 3
				&& lg[4] == 4 && lg[5] == 5 && lg[6] == 6 && lg[7] == 7
				&& lg[8] == 8 && lg[9] == 9);

		double_attr = device->read_attribute("Double_spec_attr");
		TS_ASSERT(double_attr.get_name() == "Double_spec_attr");
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 2);
		TS_ASSERT(double_attr.get_dim_y() == 0);
		double_attr >> db;
		TS_ASSERT(db[0] == 1.11 && db[1] == 2.22);

		string_attr = device->read_attribute("String_spec_attr");
		TS_ASSERT(string_attr.get_name() == "String_spec_attr");
		TS_ASSERT(string_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(string_attr.get_dim_x() == 2);
		TS_ASSERT(string_attr.get_dim_y() == 0);
		string_attr >> str;
		TS_ASSERT(str[0] == "Hello world" && str[1] == "Hello universe");
	}

// Test several SPECTRUM attributes in one call

	void test_several_SPECTRUM_attributes_in_one_call(void)
	{
		vector<DeviceAttribute> *attr_vec;
		DeviceAttribute double_attr, string_attr;
		vector<string> name_vec;
		vector<double> db;
		vector<string> str;

		name_vec.push_back("String_spec_attr");
		name_vec.push_back("Double_spec_attr");

		attr_vec = device->read_attributes(name_vec);

		string_attr = (*attr_vec)[0];
		TS_ASSERT(string_attr.get_name() == "String_spec_attr");
		TS_ASSERT(string_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(string_attr.get_dim_x() == 2);
		TS_ASSERT(string_attr.get_dim_y() == 0);
		string_attr >> str;
		TS_ASSERT(str[0] == "Hello world" && str[1] == "Hello universe");

		double_attr = (*attr_vec)[1];
		TS_ASSERT(double_attr.get_name() == "Double_spec_attr");
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 2);
		TS_ASSERT(double_attr.get_dim_y() == 0);
		double_attr >> db;
		TS_ASSERT(db[0] == 1.11 && db[1] == 2.22);
	}

// Test one attribute at a time for all IMAGE types

	void test_one_attribute_at_a_time_for_all_IMAGE_types(void)
	{
		DeviceAttribute short_attr, long_attr, double_attr, string_attr;
		vector<short> sh;
		vector<long> lg;
		vector<double> db;
		vector<string> str;

		short_attr = device->read_attribute("Short_ima_attr");
		TS_ASSERT(short_attr.get_name() == "Short_ima_attr");
		TS_ASSERT(short_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr.get_dim_x() == 2);
		TS_ASSERT(short_attr.get_dim_y() == 2);
		short_attr >> sh;
		TS_ASSERT(sh[0] == 40 && sh[1] == 60 && sh[2] == 80 && sh[3] == 100);

		long_attr = device->read_attribute("Long_ima_attr");
		TS_ASSERT(long_attr.get_name() == "Long_ima_attr");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr.get_dim_x() == 3);
		TS_ASSERT(long_attr.get_dim_y() == 2);
		long_attr >> lg;
		TS_ASSERT(lg[0] == 0 && lg[1] == 1 && lg[2] == 2 && lg[3] == 3 && lg[4] == 4 && lg[5] == 5);

		double_attr = device->read_attribute("Double_ima_attr");
		TS_ASSERT(double_attr.get_name() == "Double_ima_attr");
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 2);
		TS_ASSERT(double_attr.get_dim_y() == 1);
		double_attr >> db;
		TS_ASSERT(db[0] == 5.55 && db[1] == 6.66);

		string_attr = device->read_attribute("String_ima_attr");
		TS_ASSERT(string_attr.get_name() == "String_ima_attr");
		TS_ASSERT(string_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(string_attr.get_dim_x() == 1);
		TS_ASSERT(string_attr.get_dim_y() == 2);
		string_attr >> str;
		TS_ASSERT(str[0] == "Hello milky way" && str[1] == "Hello moon");
	}

// Test one call with all three types

	void test_one_call_with_all_three_types(void)
	{
		vector<DeviceAttribute> *attr_vec;
		DeviceAttribute long_attr, double_attr, string_attr;
		vector<string> name_vec;
		vector<long> lg;
		double db;
		vector<string> str;

		name_vec.push_back("String_spec_attr");
		name_vec.push_back("Double_attr");
		name_vec.push_back("Long_ima_attr");

		attr_vec = device->read_attributes(name_vec);

		string_attr = (*attr_vec)[0];
		TS_ASSERT(string_attr.get_name() == "String_spec_attr");
		TS_ASSERT(string_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(string_attr.get_dim_x() == 2);
		TS_ASSERT(string_attr.get_dim_y() == 0);
		string_attr >> str;
		TS_ASSERT(str[0] == "Hello world" && str[1] == "Hello universe");

		double_attr = (*attr_vec)[1];
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 1);
		TS_ASSERT(double_attr.get_dim_y() == 0);
		double_attr >> db;
		TS_ASSERT(db == 3.2);

		long_attr = (*attr_vec)[2];
		TS_ASSERT(long_attr.get_name() == "Long_ima_attr");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr.get_dim_x() == 3);
		TS_ASSERT(long_attr.get_dim_y() == 2);
		long_attr >> lg;
		TS_ASSERT(lg[0] == 0 && lg[1] == 1 && lg[2] == 2 && lg[3] == 3 && lg[4] == 4 && lg[5] == 5);
	}

// Test attributes written using the set value date and quality method

	void test_attributes_written_using_the_set_value_date_and_quality_method(void)
	{
		DeviceAttribute short_attr, long_attr, double_attr, string_attr;
		short sh;
		long lg;
		double db;
		string str;

		short_attr = device->read_attribute("attr_dq_sh");
		TS_ASSERT(short_attr.get_name() == "attr_dq_sh");
		TS_ASSERT(short_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr.get_dim_x() == 1);
		TS_ASSERT(short_attr.get_dim_y() == 0);
		short_attr >> sh;
		TS_ASSERT(sh == 77);

		long_attr = device->read_attribute("attr_dq_lo");
		TS_ASSERT(long_attr.get_name() == "attr_dq_lo");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_ALARM);
		TS_ASSERT(long_attr.get_dim_x() == 1);
		TS_ASSERT(long_attr.get_dim_y() == 0);
		long_attr >> lg;
		TS_ASSERT(lg == 7777);

		double_attr = device->read_attribute("attr_dq_db");
		TS_ASSERT(double_attr.get_name() == "attr_dq_db");
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 1);
		TS_ASSERT(double_attr.get_dim_y() == 0);
		double_attr >> db;
		TS_ASSERT(db == 8.888);

		string_attr = device->read_attribute("attr_dq_str");
		TS_ASSERT(string_attr.get_name() == "attr_dq_str");
		TS_ASSERT(string_attr.get_quality() == Tango::ATTR_ALARM);
		TS_ASSERT(string_attr.get_dim_x() == 1);
		TS_ASSERT(string_attr.get_dim_y() == 0);
		string_attr >> str;
		TS_ASSERT(str == "Setting value date and quality");
	}
};
#endif // AttrTestSuite_h
