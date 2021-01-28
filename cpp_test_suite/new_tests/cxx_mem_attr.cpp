#ifndef MemAttrTestSuite_h
#define MemAttrTestSuite_h

#include "cxx_common.h"

#define STATUS_MEM_FAILED "Memorized attribute Short_attr_w has failed during device startup sequence"
#define STATUS_ON "The device is in ON state."

#undef SUITE_NAME
#define SUITE_NAME MemAttrTestSuite

class MemAttrTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1;
	string device1_name;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		device1_name = CxxTest::TangoPrinter::get_param("device1");

		CxxTest::TangoPrinter::validate_args();

//
// Initialization --------------------------------------------------
//

		try
		{
			device1 = new DeviceProxy(device1_name);
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
		vector<short> v_s;
		v_s.push_back(4);
		v_s.push_back(0);

		din << v_s;
		try
		{
			device1->command_inout("IOAttrThrowEx", din);
		}
		catch(CORBA::Exception &e)
		{
			cout << endl << "Exception in suite tearDown():" << endl;
			Except::print_exception(e);
			exit(-1);
		}
		delete device1;
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

// Test one memorized attribute which fails during init command

	void test_One_memorized_attribute_failing_during_init_cmd(void)
	{
		DeviceAttribute short_attr_w("Short_attr_w", static_cast<DevShort>(10));
		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(short_attr_w));
//
// Ask the attribute to throw exception during any write call
//

		DeviceData din;
		vector<short> v_s;
		v_s.push_back(4);
		v_s.push_back(1);

		din << v_s;

		device1->command_inout("IOAttrThrowEx",din);

//
// Execute init command
//
		device1->command_inout("Init");

//
// Get device state and status
//

		Tango::DevState ds = device1->state();
		TS_ASSERT(ds == Tango::ALARM);

		string d_status = device1->status();
		string::size_type pos = d_status.find('.');
		pos = pos + 2;
		string sub_status = d_status.substr(pos);
		TS_ASSERT(sub_status == STATUS_MEM_FAILED);

//
// Try to read the attribute
//

		DeviceAttribute read_da = device1->read_attribute("Short_attr_w");
		short s_val;
		TS_ASSERT_THROWS_ASSERT(read_da >> s_val,Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "Aaaa" && e.errors[0].severity == Tango::ERR &&
						  string(e.errors[1].reason.in()) == API_MemAttFailedDuringInit && e.errors[1].severity == Tango::ERR));

//
// Ask attribute not to throw exception any more
//

		v_s.clear();
		v_s.push_back(4);
		v_s.push_back(0);

		din << v_s;

		device1->command_inout("IOAttrThrowEx",din);

//
// Write the attribute
//

		short val = 10;
		Tango::DeviceAttribute da("Short_attr_w",val);

		device1->write_attribute(da);

//
// Get device state and status
//

		ds = device1->state();
		TS_ASSERT(ds == Tango::ON);

		d_status = device1->status();
		TS_ASSERT(d_status == STATUS_ON);

//
// Read the attribute
//

		DeviceAttribute read_da_2 = device1->read_attribute("Short_attr_w");
		short s_val_2;
		read_da_2 >> s_val_2;
		TS_ASSERT(s_val_2 == 10);
	}

/*
 * Test for changing min and max alarm threshold of a  memorized attribute
 * with neither min_ nor max_value specified. Such scenario used to fail
 * (#401), with misleading error message claiming that memorized value
 * is above new limit.
 */

    void test_change_max_alarm_threshold_of_memorized_attribute()
    {
        const char* attr_name = "Short_attr_w";
        const DevShort attr_value = 20;

        unset_attribute_min_max_value(attr_name);

        DeviceAttribute value(attr_name, attr_value);
        TS_ASSERT_THROWS_NOTHING(device1->write_attribute(value));

        auto config = device1->get_attribute_config(attr_name);
        config.alarms.max_alarm = std::to_string(attr_value + 1);
        AttributeInfoListEx config_in = { config };
        TS_ASSERT_THROWS_NOTHING(device1->set_attribute_config(config_in));
    }

    void test_change_min_alarm_threshold_of_memorized_attribute()
    {
        const char* attr_name = "Short_attr_w";
        const DevShort attr_value = -20;

        unset_attribute_min_max_value(attr_name);

        DeviceAttribute value(attr_name, attr_value);
        TS_ASSERT_THROWS_NOTHING(device1->write_attribute(value));

        auto config = device1->get_attribute_config(attr_name);
        config.alarms.min_alarm = std::to_string(attr_value - 1);
        AttributeInfoListEx config_in = { config };
        TS_ASSERT_THROWS_NOTHING(device1->set_attribute_config(config_in));
    }

    void unset_attribute_min_max_value(const char* attr_name)
    {
        auto config = device1->get_attribute_config(attr_name);
        config.min_value = AlrmValueNotSpec;
        config.max_value = AlrmValueNotSpec;
        AttributeInfoListEx config_in = { config };
        TS_ASSERT_THROWS_NOTHING(device1->set_attribute_config(config_in));
    }
};

#undef cout
#endif // MemAttrTestSuite_h
