#ifndef AttrTestSuite_h
#define AttrTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME AttrTestSuite

class AttrTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;
	string device1_name;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string dserver_name;

		device1_name = CxxTest::TangoPrinter::get_param("device1");
		dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");

		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

		try
		{
			device1 = new DeviceProxy(device1_name);
			dserver = new DeviceProxy(dserver_name);
			device1->ping();
			dserver->ping();
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{
		DeviceData din;
		din << device1_name;
		try
		{
			dserver->command_inout("DevRestart", din);
		}
		catch(CORBA::Exception &e)
		{
			cout << endl << "Exception in suite tearDown():" << endl;
			Except::print_exception(e);
			exit(-1);
		}
		delete device1;
		delete dserver;
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
		DevShort sh;
		DevLong lg;
		DevDouble db;
		string str;

		TS_ASSERT_THROWS_NOTHING(short_attr = device1->read_attribute("Short_attr"));
		TS_ASSERT(short_attr.get_name() == "Short_attr");
		TS_ASSERT(short_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr.get_dim_x() == 1);
		TS_ASSERT(short_attr.get_dim_y() == 0);
		short_attr >> sh;
		TS_ASSERT(sh == 12);

		TS_ASSERT_THROWS_NOTHING(long_attr = device1->read_attribute("Long_attr"));
		TS_ASSERT(long_attr.get_name() == "Long_attr");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr.get_dim_x() == 1);
		TS_ASSERT(long_attr.get_dim_y() == 0);
		long_attr >> lg;
		TS_ASSERT(lg == 1246);

		TS_ASSERT_THROWS_NOTHING(double_attr = device1->read_attribute("Double_attr"));
		TS_ASSERT(double_attr.get_name() == "Double_attr");
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 1);
		TS_ASSERT(double_attr.get_dim_y() == 0);
		double_attr >> db;
		TS_ASSERT(db == 3.2);

		TS_ASSERT_THROWS_NOTHING(string_attr = device1->read_attribute("String_attr"));
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
		DevShort sh;
		DevLong lg;
		DevDouble db;
		string str;

		name_vec.push_back("String_attr");
		name_vec.push_back("Double_attr");

		TS_ASSERT_THROWS_NOTHING(attr_vec = device1->read_attributes(name_vec));

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

		TS_ASSERT_THROWS_NOTHING(attr_vec = device1->read_attributes(name_vec));

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
		vector<DevShort> sh;
		vector<DevLong> lg;
		vector<DevDouble> db;
		vector<string> str;

		TS_ASSERT_THROWS_NOTHING(short_attr = device1->read_attribute("Short_spec_attr"));
		TS_ASSERT(short_attr.get_name() == "Short_spec_attr");
		TS_ASSERT(short_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr.get_dim_x() == 4);
		TS_ASSERT(short_attr.get_dim_y() == 0);
		short_attr >> sh;
		TS_ASSERT(sh[0] == 10 && sh[1] == 20 && sh[2] == 30 && sh[3] == 40);

		TS_ASSERT_THROWS_NOTHING(long_attr = device1->read_attribute("Long_spec_attr"));
		TS_ASSERT(long_attr.get_name() == "Long_spec_attr");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr.get_dim_x() == 10);
		TS_ASSERT(long_attr.get_dim_y() == 0);
		long_attr >> lg;
		TS_ASSERT(lg[0] == 0 && lg[1] == 1 && lg[2] == 2 && lg[3] == 3
				&& lg[4] == 4 && lg[5] == 5 && lg[6] == 6 && lg[7] == 7
				&& lg[8] == 8 && lg[9] == 9);

		TS_ASSERT_THROWS_NOTHING(double_attr = device1->read_attribute("Double_spec_attr"));
		TS_ASSERT(double_attr.get_name() == "Double_spec_attr");
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 2);
		TS_ASSERT(double_attr.get_dim_y() == 0);
		double_attr >> db;
		TS_ASSERT(db[0] == 1.11 && db[1] == 2.22);

		TS_ASSERT_THROWS_NOTHING(string_attr = device1->read_attribute("String_spec_attr"));
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
		vector<DevDouble> db;
		vector<string> str;

		name_vec.push_back("String_spec_attr");
		name_vec.push_back("Double_spec_attr");

		TS_ASSERT_THROWS_NOTHING(attr_vec = device1->read_attributes(name_vec));

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
		vector<DevShort> sh;
		vector<DevLong> lg;
		vector<DevDouble> db;
		vector<string> str;

		TS_ASSERT_THROWS_NOTHING(short_attr = device1->read_attribute("Short_ima_attr"));
		TS_ASSERT(short_attr.get_name() == "Short_ima_attr");
		TS_ASSERT(short_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr.get_dim_x() == 2);
		TS_ASSERT(short_attr.get_dim_y() == 2);
		short_attr >> sh;
		TS_ASSERT(sh[0] == 40 && sh[1] == 60 && sh[2] == 80 && sh[3] == 100);

		TS_ASSERT_THROWS_NOTHING(long_attr = device1->read_attribute("Long_ima_attr"));
		TS_ASSERT(long_attr.get_name() == "Long_ima_attr");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr.get_dim_x() == 3);
		TS_ASSERT(long_attr.get_dim_y() == 2);
		long_attr >> lg;
		TS_ASSERT(lg[0] == 0 && lg[1] == 1 && lg[2] == 2 && lg[3] == 3 && lg[4] == 4 && lg[5] == 5);

		TS_ASSERT_THROWS_NOTHING(double_attr = device1->read_attribute("Double_ima_attr"));
		TS_ASSERT(double_attr.get_name() == "Double_ima_attr");
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 2);
		TS_ASSERT(double_attr.get_dim_y() == 1);
		double_attr >> db;
		TS_ASSERT(db[0] == 5.55 && db[1] == 6.66);

		TS_ASSERT_THROWS_NOTHING(string_attr = device1->read_attribute("String_ima_attr"));
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
		vector<DevLong> lg;
		DevDouble db;
		vector<string> str;

		name_vec.push_back("String_spec_attr");
		name_vec.push_back("Double_attr");
		name_vec.push_back("Long_ima_attr");

		TS_ASSERT_THROWS_NOTHING(attr_vec = device1->read_attributes(name_vec));

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
		DevShort sh;
		DevLong lg;
		DevDouble db;
		string str;

		TS_ASSERT_THROWS_NOTHING(short_attr = device1->read_attribute("attr_dq_sh"));
		TS_ASSERT(short_attr.get_name() == "attr_dq_sh");
		TS_ASSERT(short_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr.get_dim_x() == 1);
		TS_ASSERT(short_attr.get_dim_y() == 0);
		short_attr >> sh;
		TS_ASSERT(sh == 77);

		TS_ASSERT_THROWS_NOTHING(long_attr = device1->read_attribute("attr_dq_lo"));
		TS_ASSERT(long_attr.get_name() == "attr_dq_lo");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_ALARM);
		TS_ASSERT(long_attr.get_dim_x() == 1);
		TS_ASSERT(long_attr.get_dim_y() == 0);
		long_attr >> lg;
		TS_ASSERT(lg == 7777);

		TS_ASSERT_THROWS_NOTHING(double_attr = device1->read_attribute("attr_dq_db"));
		TS_ASSERT(double_attr.get_name() == "attr_dq_db");
		TS_ASSERT(double_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr.get_dim_x() == 1);
		TS_ASSERT(double_attr.get_dim_y() == 0);
		double_attr >> db;
		TS_ASSERT(db == 8.888);

		TS_ASSERT_THROWS_NOTHING(string_attr = device1->read_attribute("attr_dq_str"));
		TS_ASSERT(string_attr.get_name() == "attr_dq_str");
		TS_ASSERT(string_attr.get_quality() == Tango::ATTR_ALARM);
		TS_ASSERT(string_attr.get_dim_x() == 1);
		TS_ASSERT(string_attr.get_dim_y() == 0);
		string_attr >> str;
		TS_ASSERT(str == "Setting value date and quality");
	}
};
#undef cout
#endif // AttrTestSuite_h
