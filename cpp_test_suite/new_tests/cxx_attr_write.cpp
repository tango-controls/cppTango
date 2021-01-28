#ifndef AttrWriteTestSuite_h
#define AttrWriteTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME AttrWriteTestSuite

class AttrWriteTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string device1_name, dserver_name;

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

// Test some exception cases

	void test_some_exception_cases(void)
	{
		DevShort sh = 10;
		DevLong lg = 1000;
		DeviceAttribute toto("toto", "test_string"),
				short_attr_w("Short_attr_w", sh), short_attr_w2("Short_attr_w", sh),
				short_attr("Short_attr", sh),
				long_attr_w("Long_attr_w", lg);
		vector<DeviceAttribute> attributes;

		TS_ASSERT_THROWS_ASSERT(device1->write_attribute(toto), Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_AttrNotFound
								&& e.errors[0].severity == Tango::ERR));

		// WARNING:
		// the copy constructor used by push_back() removes memory from the copied object;
		// unless fixed, the original object cannot be used in further tests
		attributes.push_back(short_attr_w);
		attributes.push_back(toto);
		TS_ASSERT_THROWS_ASSERT(device1->write_attributes(attributes), Tango::NamedDevFailedList &e,
						TS_ASSERT(string(e.err_list[0].err_stack[0].reason.in()) == API_AttrNotFound
								&& e.errors[0].severity == Tango::ERR));

		attributes.clear();
		attributes.push_back(short_attr_w2);
		attributes.push_back(short_attr);
		attributes.push_back(long_attr_w);
		TS_ASSERT_THROWS_ASSERT(device1->write_attributes(attributes), Tango::NamedDevFailedList &e,
						TS_ASSERT(string(e.err_list[0].err_stack[0].reason.in()) == API_AttrNotWritable
								&& e.errors[0].severity == Tango::ERR));

	}

// Write some scalar attributes

	void test_write_some_scalar_attributes(void)
	{
		DevShort sh = 10;
		DevLong lg = 1000;
		DevDouble db = 1.2345;
		string str = "Hello world";
		DevFloat fl = 10.22f;
		DevBoolean bl = 0;
		DevUShort ush = 1234;
		DevUChar uch = 233;
		DeviceAttribute short_attr_w("Short_attr_w", sh),
				long_attr_w("Long_attr_w", lg),
				double_attr_w("Double_attr_w", db),
				string_attr_w("String_attr_w", str),
				float_attr_w("Float_attr_w", fl),
				boolean_attr_w("Boolean_attr_w", bl),
				ushort_attr_w("UShort_attr_w", ush),
				uchar_attr_w("UChar_attr_w", uch);
		vector<string> attributes_str;
		attributes_str.push_back("Short_attr_w");
		attributes_str.push_back("Long_attr_w");
		attributes_str.push_back("Double_attr_w");
		attributes_str.push_back("String_attr_w");
		attributes_str.push_back("Float_attr_w");
		attributes_str.push_back("Boolean_attr_w");
		attributes_str.push_back("UShort_attr_w");
		attributes_str.push_back("UChar_attr_w");

		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(short_attr_w));
		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(long_attr_w));
		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(double_attr_w));
		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(string_attr_w));
		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(float_attr_w));
		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(boolean_attr_w));
		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(ushort_attr_w));
		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(uchar_attr_w));

		vector<DeviceAttribute> *attributes_vec;
		TS_ASSERT_THROWS_NOTHING(attributes_vec = device1->read_attributes(attributes_str));

		vector<DeviceAttribute> &attributes = *attributes_vec;
		attributes[0] >> sh;
		TS_ASSERT(attributes[0].name == "Short_attr_w" && sh == 10);
		attributes[1] >> lg;
		TS_ASSERT(attributes[1].name == "Long_attr_w" && lg == 1000);
		attributes[2] >> db;
		TS_ASSERT(attributes[2].name == "Double_attr_w" && db == 1.2345);
		attributes[3] >> str;
		TS_ASSERT(attributes[3].name == "String_attr_w" && str == "Hello world");
		attributes[4] >> fl;
		TS_ASSERT(attributes[4].name == "Float_attr_w");
		TS_ASSERT_DELTA(fl,10.22,0.001); // floating point comparison
		attributes[5] >> bl;
		TS_ASSERT(attributes[5].name == "Boolean_attr_w" && bl == 0);
		attributes[6] >> ush;
		TS_ASSERT(attributes[6].name == "UShort_attr_w" && ush == 1234);
		attributes[7] >> uch;
		TS_ASSERT(attributes[7].name == "UChar_attr_w" && uch == 233);

		attributes.clear();
		attributes.push_back(short_attr_w);
		attributes.push_back(long_attr_w);
		attributes.push_back(double_attr_w);
		attributes.push_back(string_attr_w);
		TS_ASSERT_THROWS_NOTHING(device1->write_attributes(attributes));
	}

// Short_attr_w, String_attr_w and Boolean_attr_w are memorized attributes

	void test_memorized_attributes(void)
	{
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RestartServer"));
		Tango_sleep(6);

		DevShort sh;
		string str;
		DevBoolean bl;
		vector<string> attributes_str;
		attributes_str.push_back("Short_attr_w");
		attributes_str.push_back("String_attr_w");
		attributes_str.push_back("Boolean_attr_w");

		vector<DeviceAttribute> *attributes_vec;
		TS_ASSERT_THROWS_NOTHING(attributes_vec = device1->read_attributes(attributes_str));

		vector<DeviceAttribute> &attributes = *attributes_vec;
		attributes[0] >> sh;
		TS_ASSERT(attributes[0].name == "Short_attr_w" && sh == 10);
		attributes[1] >> str;
		TS_ASSERT(attributes[1].name == "String_attr_w" && str == "Hello world");
		attributes[2] >> bl;
		TS_ASSERT(attributes[2].name == "Boolean_attr_w" && bl == 0);
	}
};
#undef cout
#endif // AttrWriteTestSuite_h
