#ifndef WriteAttrHardware_h
#define WriteAttrHardware_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME WriteAttrHardware

class WriteAttrHardware: public CxxTest::TestSuite
{
protected:
	DeviceProxy 	*device;
	string			att1_name;
	string			att2_name;
	string 			att3_name;
	vector<string>	vs;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string device1_name;

		device1_name = CxxTest::TangoPrinter::get_param("device1");

		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

		try
		{
			device = new DeviceProxy(device1_name);
			device->ping();

			att1_name = "Long64_attr_rw";
			att2_name = "State_attr_rw";
			att3_name = "Long64_spec_attr_rw";

			vs.push_back(att1_name);
			vs.push_back(att2_name);
			vs.push_back(att3_name);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{
		if(CxxTest::TangoPrinter::is_restore_set("restore_except"))
		{
			Tango::DeviceData din;
			din << (short)0;
			device->command_inout("WriteAttrHardwareThrow",din);

			vector<Tango::DeviceAttribute> attr_in;
			Tango::DevLong64 lg64_attr = 0x800000000LL;
			Tango::DevState sta_attr = Tango::FAULT;
			vector<Tango::DevLong64> lg64_arr_attr;
			lg64_arr_attr.push_back(0);

			attr_in.push_back(Tango::DeviceAttribute(att1_name,lg64_attr));
			attr_in.push_back(Tango::DeviceAttribute(att2_name,sta_attr));
			attr_in.push_back(Tango::DeviceAttribute(att3_name,lg64_arr_attr));

			device->write_attributes(attr_in);
		}

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

// Test write_attr_hardware throwing DevFailed exception

	void test_DevFailed_from_write_attr_hardware(void)
	{
		Tango::DeviceData din;
		din << (short)1;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("WriteAttrHardwareThrow",din));

		CxxTest::TangoPrinter::restore_set("restore_except");

		vector<Tango::DeviceAttribute> *read_bef;
		vector<Tango::DeviceAttribute> *read_after;

		TS_ASSERT_THROWS_NOTHING(read_bef = device->read_attributes(vs));

		vector<Tango::DevLong64> read_bef_att1;
		vector<Tango::DevState> read_bef_att2;
		vector<Tango::DevLong64> read_bef_att3;

		(*read_bef)[0].extract_set(read_bef_att1);
		(*read_bef)[1].extract_set(read_bef_att2);
		(*read_bef)[2].extract_set(read_bef_att3);

		vector<Tango::DeviceAttribute> attr_in;

		Tango::DevLong64 lg64_attr = (Tango::DevLong64)111;
		Tango::DevState sta_attr = Tango::STANDBY;
		vector<Tango::DevLong64> lg64_arr_attr;
		lg64_arr_attr.push_back(4444);


		attr_in.push_back(Tango::DeviceAttribute(att1_name,lg64_attr));
		attr_in.push_back(Tango::DeviceAttribute(att2_name,sta_attr));
		attr_in.push_back(Tango::DeviceAttribute(att3_name,lg64_arr_attr));

		TS_ASSERT_THROWS_ASSERT(device->write_attributes(attr_in),Tango::NamedDevFailedList &e,
				TS_ASSERT(string(e.err_list[0].name) == att1_name
						&& e.err_list[0].idx_in_call == 0
						&& string(e.err_list[0].err_stack[0].reason.in()) == "DevTest_WriteAttrHardware"
						&& string(e.err_list[0].err_stack[0].origin.in()).find("DevTest::write_attr_hardware") != std::string::npos
						&& e.err_list[0].err_stack[0].severity == Tango::ERR
						&& string(e.err_list[1].name) == att2_name
						&& e.err_list[1].idx_in_call == 1
						&& string(e.err_list[1].err_stack[0].reason.in()) == "DevTest_WriteAttrHardware"
						&& string(e.err_list[1].err_stack[0].origin.in()).find("DevTest::write_attr_hardware") != std::string::npos
						&& e.err_list[1].err_stack[0].severity == Tango::ERR
						&& string(e.err_list[2].name) == att3_name
						&& e.err_list[2].idx_in_call == 2
						&& string(e.err_list[2].err_stack[0].reason.in()) == "DevTest_WriteAttrHardware"
						&& string(e.err_list[2].err_stack[0].origin.in()).find("DevTest::write_attr_hardware") != std::string::npos
						&& e.err_list[2].err_stack[0].severity == Tango::ERR));

		TS_ASSERT_THROWS_NOTHING(read_after = device->read_attributes(vs));

		vector<Tango::DevLong64> read_after_att1;
		vector<Tango::DevState> read_after_att2;
		vector<Tango::DevLong64> read_after_att3;

		(*read_after)[0].extract_set(read_after_att1);
		(*read_after)[1].extract_set(read_after_att2);
		(*read_after)[2].extract_set(read_after_att3);

		TS_ASSERT(read_bef_att1 == read_after_att1);
		TS_ASSERT(read_bef_att2 == read_after_att2);
		TS_ASSERT(read_bef_att3 == read_after_att3);

		delete read_bef;
		delete read_after;
	}

// Test write_attr_hardware throwing MultiDevFailed exception from att name

	void test_MultiDevFailed_from_name_thrown_by_write_attr_hardware(void)
	{
		Tango::DeviceData din;
		din << (short)2;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("WriteAttrHardwareThrow",din));

		vector<Tango::DeviceAttribute> *read_bef;
		vector<Tango::DeviceAttribute> *read_after;

		TS_ASSERT_THROWS_NOTHING(read_bef = device->read_attributes(vs));

		vector<Tango::DevLong64> read_bef_att1;

		(*read_bef)[0].extract_set(read_bef_att1);

		vector<Tango::DeviceAttribute> attr_in;

		Tango::DevLong64 lg64_attr = (Tango::DevLong64)111;
		Tango::DevState sta_attr = Tango::STANDBY;
		vector<Tango::DevLong64> lg64_arr_attr;
		lg64_arr_attr.push_back(4444);


		attr_in.push_back(Tango::DeviceAttribute(att1_name,lg64_attr));
		attr_in.push_back(Tango::DeviceAttribute(att2_name,sta_attr));
		attr_in.push_back(Tango::DeviceAttribute(att3_name,lg64_arr_attr));

		TS_ASSERT_THROWS_ASSERT(device->write_attributes(attr_in),Tango::NamedDevFailedList &e,
				TS_ASSERT(string(e.err_list[0].name) == att1_name
						&& e.err_list[0].idx_in_call == 0
						&& string(e.err_list[0].err_stack[0].reason.in()) == "DevTest_WriteAttrHardware"
						&& string(e.err_list[0].err_stack[0].origin.in()) == "DevTest::write_attr_hardware"
						&& string(e.err_list[0].err_stack[0].desc.in()) == "aaa"
						&& e.err_list[0].err_stack[0].severity == Tango::ERR));

		TS_ASSERT_THROWS_NOTHING(read_after = device->read_attributes(vs));

		vector<Tango::DevLong64> read_after_att1;
		vector<Tango::DevState> read_after_att2;
		vector<Tango::DevLong64> read_after_att3;

		(*read_after)[0].extract_set(read_after_att1);
		(*read_after)[1].extract_set(read_after_att2);
		(*read_after)[2].extract_set(read_after_att3);

		TS_ASSERT(read_bef_att1 == read_after_att1);
		TS_ASSERT(read_after_att2.size() == 1 && read_after_att2[0] == Tango::STANDBY);
		TS_ASSERT(read_after_att3.size() == 1 && read_after_att3[0] == 4444);

		delete read_bef;
		delete read_after;
	}

// Test write_attr_hardware throwing MultiDevFailed exception from att index

	void test_MultiDevFailed_from_ind_thrown_by_write_attr_hardware(void)
	{
		Tango::DeviceData din;
		din << (short)3;
		TS_ASSERT_THROWS_NOTHING(device->command_inout("WriteAttrHardwareThrow",din));

		vector<Tango::DeviceAttribute> *read_bef;
		vector<Tango::DeviceAttribute> *read_after;

		TS_ASSERT_THROWS_NOTHING(read_bef = device->read_attributes(vs));

		vector<Tango::DevLong64> read_bef_att3;

		(*read_bef)[2].extract_set(read_bef_att3);

		vector<Tango::DeviceAttribute> attr_in;

		Tango::DevLong64 lg64_attr = (Tango::DevLong64)111;
		Tango::DevState sta_attr = Tango::STANDBY;
		vector<Tango::DevLong64> lg64_arr_attr;
		lg64_arr_attr.push_back(4444);


		attr_in.push_back(Tango::DeviceAttribute(att1_name,lg64_attr));
		attr_in.push_back(Tango::DeviceAttribute(att2_name,sta_attr));
		attr_in.push_back(Tango::DeviceAttribute(att3_name,lg64_arr_attr));

		TS_ASSERT_THROWS_ASSERT(device->write_attributes(attr_in),Tango::NamedDevFailedList &e,
				TS_ASSERT(string(e.err_list[0].name) == att3_name
						&& e.err_list[0].idx_in_call == 2
						&& string(e.err_list[0].err_stack[0].reason.in()) == "DevTest_WriteAttrHardware"
						&& string(e.err_list[0].err_stack[0].origin.in()) == "DevTest::write_attr_hardware"
						&& string(e.err_list[0].err_stack[0].desc.in()) == "bbb"
						&& e.err_list[0].err_stack[0].severity == Tango::ERR));

		TS_ASSERT_THROWS_NOTHING(read_after = device->read_attributes(vs));

		vector<Tango::DevLong64> read_after_att1;
		vector<Tango::DevState> read_after_att2;
		vector<Tango::DevLong64> read_after_att3;

		(*read_after)[0].extract_set(read_after_att1);
		(*read_after)[1].extract_set(read_after_att2);
		(*read_after)[2].extract_set(read_after_att3);

		TS_ASSERT(read_bef_att3 == read_after_att3);
		TS_ASSERT(read_after_att2.size() == 1 && read_after_att2[0] == Tango::STANDBY);
		TS_ASSERT(read_after_att1.size() == 1 && read_after_att1[0] == 111);

	}
};
#undef cout
#endif // WriteAttrHardware_h
