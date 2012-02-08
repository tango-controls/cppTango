#ifndef AttrMiscTestSuite_h
#define AttrMiscTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME AttrMiscTestSuite

class AttrMiscTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device, *dserver;
	string device_name;
	AttributeInfoListEx *init_attr_conf;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		vector<string> uargs; // user arguments
		uargs.push_back("device");

		vector<string> params; // parameters
		params.push_back("fulldsname");

		vector<string> params_opt; // optional parameters
		params_opt.push_back("loop");

		string dserver_name;

		bool params_ok = true;
		for(size_t i = 0; i < params.size() && params_ok; i++)
			params_ok = CxxTest::TangoPrinter::is_param_set(params[i]);

		if(CxxTest::TangoPrinter::get_uargc() >= uargs.size() && params_ok)
		{
			device_name = CxxTest::TangoPrinter::get_uargv()[0];
			dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param_val(params[0]);
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
			device = new DeviceProxy(device_name);
			dserver = new DeviceProxy(dserver_name);
			device->ping();
			dserver->ping();

			dserver->command_inout("RestartServer");
			Tango_sleep(3);

			vector<string> attr_list;
			attr_list.push_back("Double_attr");
			attr_list.push_back("Float_attr");
			attr_list.push_back("Long_attr");
			attr_list.push_back("Long64_attr");
			attr_list.push_back("Short_attr");
			attr_list.push_back("UChar_attr");
			attr_list.push_back("ULong_attr");
			attr_list.push_back("ULong64_attr");
			attr_list.push_back("UShort_attr");
			init_attr_conf = device->get_attribute_config_ex(attr_list);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{
		DevLong lg;
		DeviceData din;
		lg = 1246;
		din << lg;
		try
		{
			device->set_attribute_config(*init_attr_conf);

			device->command_inout("IOSetAttr", din);
			din << device_name;
			dserver->command_inout("DevRestart", din);
		}
		catch (CORBA::Exception &e)
		{
			cout << endl << "Exception in suite tearDown():" << endl;
			Except::print_exception(e);
			exit(-1);
		}

		delete device;
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

//
// Test set/get min/max alarm/warning functions
//

	void test_set_get_ranges(void)
	{
		const DevVarStringArray *ranges;
		DeviceData dout;

		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("SetGetRanges"));
		TS_ASSERT_THROWS_NOTHING(dout >> ranges);

		TS_ASSERT((*ranges).length() == 45);
		TS_ASSERT(string((*ranges)[0].in()) == "Double_attr");
		TS_ASSERT(string((*ranges)[1].in()) == "-999.99");
		TS_ASSERT(string((*ranges)[2].in()) == "-888.88");
		TS_ASSERT(string((*ranges)[3].in()) == "888.88");
		TS_ASSERT(string((*ranges)[4].in()) == "999.99");
		TS_ASSERT(string((*ranges)[5].in()) == "Float_attr");
		TS_ASSERT(string((*ranges)[6].in()) == "-777.77");
		TS_ASSERT(string((*ranges)[7].in()) == "-666.66");
		TS_ASSERT(string((*ranges)[8].in()) == "666.66");
		TS_ASSERT(string((*ranges)[9].in()) == "777.77");
		TS_ASSERT(string((*ranges)[10].in()) == "Long_attr");
		TS_ASSERT(string((*ranges)[11].in()) == "1000");
		TS_ASSERT(string((*ranges)[12].in()) == "1100");
		TS_ASSERT(string((*ranges)[13].in()) == "1400");
		TS_ASSERT(string((*ranges)[14].in()) == "1500");
		TS_ASSERT(string((*ranges)[15].in()) == "Long64_attr");
		TS_ASSERT(string((*ranges)[16].in()) == "-90000");
		TS_ASSERT(string((*ranges)[17].in()) == "-80000");
		TS_ASSERT(string((*ranges)[18].in()) == "80000");
		TS_ASSERT(string((*ranges)[19].in()) == "90000");
		TS_ASSERT(string((*ranges)[20].in()) == "Short_attr");
		TS_ASSERT(string((*ranges)[21].in()) == "-5000");
		TS_ASSERT(string((*ranges)[22].in()) == "-4000");
		TS_ASSERT(string((*ranges)[23].in()) == "4000");
		TS_ASSERT(string((*ranges)[24].in()) == "5000");
		TS_ASSERT(string((*ranges)[25].in()) == "UChar_attr");
		TS_ASSERT(string((*ranges)[26].in()) == "0");
		TS_ASSERT(string((*ranges)[27].in()) == "1");
		TS_ASSERT(string((*ranges)[28].in()) == "240");
		TS_ASSERT(string((*ranges)[29].in()) == "250");
		TS_ASSERT(string((*ranges)[30].in()) == "ULong_attr");
		TS_ASSERT(string((*ranges)[31].in()) == "0");
		TS_ASSERT(string((*ranges)[32].in()) == "1");
		TS_ASSERT(string((*ranges)[33].in()) == "666666");
		TS_ASSERT(string((*ranges)[34].in()) == "777777");
		TS_ASSERT(string((*ranges)[35].in()) == "ULong64_attr");
		TS_ASSERT(string((*ranges)[36].in()) == "0");
		TS_ASSERT(string((*ranges)[37].in()) == "1");
		TS_ASSERT(string((*ranges)[38].in()) == "88888888");
		TS_ASSERT(string((*ranges)[39].in()) == "99999999");
		TS_ASSERT(string((*ranges)[40].in()) == "UShort_attr");
		TS_ASSERT(string((*ranges)[41].in()) == "0");
		TS_ASSERT(string((*ranges)[42].in()) == "1");
		TS_ASSERT(string((*ranges)[43].in()) == "20000");
		TS_ASSERT(string((*ranges)[44].in()) == "30000");


//		// display ranges
//		for(unsigned int i = 0; i < ranges->length(); i++)
//		{
//			cout << "\t" << (*ranges)[i].in() << endl;
//		}
	}

// Test read attribute exceptions

	void test_read_attribute_exceptions(void)
	{
		DeviceAttribute attr;
		DevShort sh;
		DevLong lg;

		TS_ASSERT_THROWS_NOTHING(attr = device->read_attribute("Toto"));
		TS_ASSERT_THROWS_ASSERT(attr >> sh, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrNotFound"
								&& e.errors[0].severity == Tango::ERR));

		TS_ASSERT_THROWS_NOTHING(attr = device->read_attribute("attr_no_data"));
		TS_ASSERT_THROWS_ASSERT(attr >> sh, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrValueNotSet"
								&& e.errors[0].severity == Tango::ERR));

		TS_ASSERT_THROWS_NOTHING(attr = device->read_attribute("attr_wrong_type"));
		TS_ASSERT_THROWS_ASSERT(attr >> sh, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrOptProp"
								&& e.errors[0].severity == Tango::ERR));

		TS_ASSERT_THROWS_NOTHING(attr = device->read_attribute("attr_wrong_size"));
		TS_ASSERT_THROWS_ASSERT(attr >> lg, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrOptProp"
								&& e.errors[0].severity == Tango::ERR));

		TS_ASSERT_THROWS_NOTHING(attr = device->read_attribute("attr_no_alarm"));
		TS_ASSERT_THROWS_ASSERT(attr >> lg, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrNoAlarm"
								&& e.errors[0].severity == Tango::ERR));
	}

//
// Testing SCALAR attribute of type different than READ
// As we have never written any attribute (yet), the write values should all
// be initialised to 0
//

	void test_SCALAR_attribute_of_type_different_than_READ(void)
	{
		DeviceAttribute long_attr_with_w, long_attr_w, short_attr_rw, float_attr_w, ushort_attr_w, uchar_attr_w;
		DevVarLongArray *lg_array;
		DevVarShortArray *sh_array;
		DevVarFloatArray *fl_array;
		DevVarUShortArray *ush_array;
		DevVarUCharArray *uch_array;

		TS_ASSERT_THROWS_NOTHING(long_attr_with_w = device->read_attribute("Long_attr_with_w"));
		TS_ASSERT(long_attr_with_w.get_name() == "Long_attr_with_w");
		TS_ASSERT(long_attr_with_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr_with_w.get_dim_x() == 1);
		TS_ASSERT(long_attr_with_w.get_dim_y() == 0);
		long_attr_with_w >> lg_array;
		TS_ASSERT((*lg_array)[0] == 1246);
		TS_ASSERT((*lg_array)[1] == 0);

		TS_ASSERT_THROWS_NOTHING(long_attr_w = device->read_attribute("Long_attr_w"));
		TS_ASSERT(long_attr_w.get_name() == "Long_attr_w");
		TS_ASSERT(long_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr_w.get_dim_x() == 1);
		TS_ASSERT(long_attr_w.get_dim_y() == 0);
		long_attr_w >> lg_array ;
		TS_ASSERT((*lg_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(short_attr_rw = device->read_attribute("Short_attr_rw"));
		TS_ASSERT(short_attr_rw.get_name() == "Short_attr_rw");
		TS_ASSERT(short_attr_rw.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr_rw.get_dim_x() == 1);
		TS_ASSERT(short_attr_rw.get_dim_y() == 0);
		short_attr_rw >> sh_array;
		TS_ASSERT((*sh_array)[0] == 66);
		TS_ASSERT((*sh_array)[1] == 0);

		TS_ASSERT_THROWS_NOTHING(float_attr_w = device->read_attribute("Float_attr_w"));
		TS_ASSERT(float_attr_w.get_name() == "Float_attr_w");
		TS_ASSERT(float_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(float_attr_w.get_dim_x() == 1);
		TS_ASSERT(float_attr_w.get_dim_y() == 0);
		float_attr_w >> fl_array;
		TS_ASSERT((*fl_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(ushort_attr_w = device->read_attribute("UShort_attr_w"));
		TS_ASSERT(ushort_attr_w.get_name() == "UShort_attr_w");
		TS_ASSERT(ushort_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(ushort_attr_w.get_dim_x() == 1);
		TS_ASSERT(ushort_attr_w.get_dim_y() == 0);
		ushort_attr_w >> ush_array;
		TS_ASSERT((*ush_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(uchar_attr_w = device->read_attribute("UChar_attr_w"));
		TS_ASSERT(uchar_attr_w.get_name() == "UChar_attr_w");
		TS_ASSERT(uchar_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(uchar_attr_w.get_dim_x() == 1);
		TS_ASSERT(uchar_attr_w.get_dim_y() == 0);
		uchar_attr_w >> uch_array;
		TS_ASSERT((*uch_array)[0] == 0);
	}

// Test read attribute on write type attribute

	void test_read_attribute_on_write_type_attribute(void)
	{
		DeviceAttribute short_attr_w2, long_attr_w, double_attr_w, string_attr_w2;
		DevVarShortArray *sh_array;
		DevVarLongArray *lg_array;
		DevVarDoubleArray *db_array;
		DevVarStringArray *str_array;

		TS_ASSERT_THROWS_NOTHING(short_attr_w2 = device->read_attribute("Short_attr_w2"));
		TS_ASSERT(short_attr_w2.get_name() == "Short_attr_w2");
		TS_ASSERT(short_attr_w2.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr_w2.get_dim_x() == 1);
		TS_ASSERT(short_attr_w2.get_dim_y() == 0);
		short_attr_w2 >> sh_array;
		TS_ASSERT((*sh_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(long_attr_w = device->read_attribute("Long_attr_w"));
		TS_ASSERT(long_attr_w.get_name() == "Long_attr_w");
		TS_ASSERT(long_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr_w.get_dim_x() == 1);
		TS_ASSERT(long_attr_w.get_dim_y() == 0);
		long_attr_w >> lg_array;
		TS_ASSERT((*lg_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(double_attr_w = device->read_attribute("Double_attr_w"));
		TS_ASSERT(double_attr_w.get_name() == "Double_attr_w");
		TS_ASSERT(double_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr_w.get_dim_x() == 1);
		TS_ASSERT(double_attr_w.get_dim_y() == 0);
		double_attr_w >> db_array;
		TS_ASSERT((*db_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(string_attr_w2 = device->read_attribute("String_attr_w2"));
		TS_ASSERT(string_attr_w2.get_name() == "String_attr_w2");
		TS_ASSERT(string_attr_w2.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(string_attr_w2.get_dim_x() == 1);
		TS_ASSERT(string_attr_w2.get_dim_y() == 0);
		string_attr_w2 >> str_array;
		TS_ASSERT(string((*str_array)[0].in()) == "Not initialised");
	}

// Test read attribute on initialised write type attribute

	void test_read_attribute_on_initialised_write_type_attribute(void)
	{
		DeviceAttribute short_attr_w, long_attr_w, double_attr_w, string_attr_w;
		DevVarShortArray *sh_array;
		DevVarLongArray *lg_array;
		DevVarDoubleArray *db_array;
		DevVarStringArray *str_array;

		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOInitWAttr"));

		TS_ASSERT_THROWS_NOTHING(short_attr_w = device->read_attribute("Short_attr_w"));
		TS_ASSERT(short_attr_w.get_name() == "Short_attr_w");
		TS_ASSERT(short_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr_w.get_dim_x() == 1);
		TS_ASSERT(short_attr_w.get_dim_y() == 0);
		short_attr_w >> sh_array;
		TS_ASSERT((*sh_array)[0] == 10);

		TS_ASSERT_THROWS_NOTHING(long_attr_w = device->read_attribute("Long_attr_w"));
		TS_ASSERT(long_attr_w.get_name() == "Long_attr_w");
		TS_ASSERT(long_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr_w.get_dim_x() == 1);
		TS_ASSERT(long_attr_w.get_dim_y() == 0);
		long_attr_w >> lg_array;
		TS_ASSERT((*lg_array)[0] == 100);

		TS_ASSERT_THROWS_NOTHING(double_attr_w = device->read_attribute("Double_attr_w"));
		TS_ASSERT(double_attr_w.get_name() == "Double_attr_w");
		TS_ASSERT(double_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr_w.get_dim_x() == 1);
		TS_ASSERT(double_attr_w.get_dim_y() == 0);
		double_attr_w >> db_array;
		TS_ASSERT((*db_array)[0] == 1.1);

		TS_ASSERT_THROWS_NOTHING(string_attr_w = device->read_attribute("String_attr_w"));
		TS_ASSERT(string_attr_w.get_name() == "String_attr_w");
		TS_ASSERT(string_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(string_attr_w.get_dim_x() == 1);
		TS_ASSERT(string_attr_w.get_dim_y() == 0);
		string_attr_w >> str_array;
		TS_ASSERT(string((*str_array)[0].in()) == "Init");
	}

//
// Test alarm on attribute. An alarm is defined for the Long_attr attribute
// is < 1000 and > 1500.
//

	void test_alarm_on_attribute(void)
	{
		DeviceAttribute long_attr;
		DevVarLongArray *lg_array;
		DevLong lg;
		DeviceData din, dout;
		DevState state;
		const char *status;

		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);

		TS_ASSERT_THROWS_NOTHING(long_attr = device->read_attribute("Long_attr"));
		TS_ASSERT(long_attr.get_name() == "Long_attr");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr.get_dim_x() == 1);
		TS_ASSERT(long_attr.get_dim_y() == 0);
		long_attr >> lg_array;
		TS_ASSERT((*lg_array)[0] == 1246);

		lg = 900;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOSetAttr", din));
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ALARM);
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("Status"));
		dout >> status;
		TS_ASSERT(strcmp(status,"The device is in ALARM state.\nAlarm : Value too low for Long_attr") == 0);

		lg = 1200;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOSetAttr", din));
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("Status"));
		dout >> status;
		TS_ASSERT(strcmp(status,"The device is in ON state.") == 0);

		state = Tango::ON;
		din << state;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("Status"));
		dout >> status;
		TS_ASSERT(strcmp(status,"The device is in ON state.") == 0);

		lg = 2000;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOSetAttr", din));
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ALARM);
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("Status"));
		dout >> status;
		TS_ASSERT(strcmp(status,"The device is in ALARM state.\nAlarm : Value too high for Long_attr") == 0);

		lg = 1200;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOSetAttr", din));
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("Status"));
		dout >> status;
		TS_ASSERT(strcmp(status,"The device is in ON state.") == 0);

		state = Tango::ON;
		din << state;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
		TS_ASSERT_THROWS_NOTHING(dout = device->command_inout("Status"));
		dout >> status;
		TS_ASSERT(strcmp(status,"The device is in ON state.") == 0);
	}
};
#undef cout
#endif // AttrMiscTestSuite_h
