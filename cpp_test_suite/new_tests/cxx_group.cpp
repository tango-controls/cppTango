#ifndef GroupTestSuite_h
#define GroupTestSuite_h

#include <ctime>
#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME GroupTestSuite

class GroupTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *device2, *device3;
	Group *group, *sub_group;
	string device1_name, device2_name, device3_name;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		device1_name = CxxTest::TangoPrinter::get_param("device1");
		device2_name = CxxTest::TangoPrinter::get_param("device2");
		device3_name = CxxTest::TangoPrinter::get_param("device3");

		CxxTest::TangoPrinter::validate_args();


// Initialization --------------------------------------------------


		try
		{
			device1 = new DeviceProxy(device1_name);
			device2 = new DeviceProxy(device2_name);
			device3 = new DeviceProxy(device3_name);

			group = new Group("group");
			sub_group = new Group("sub_group");
			sub_group->add(device1_name);
			group->add(sub_group);
			group->add(device2_name);
			group->add(device3_name);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{

//
// Clean up --------------------------------------------------------
//

		if(CxxTest::TangoPrinter::is_restore_set("double_attr_value"))
		{
			DeviceAttribute value("Double_attr_w",0.0);
			device1->write_attribute(value);
			device2->write_attribute(value);
			device2->write_attribute(value);
		}

		delete device1;
		delete device2;
		delete device3;

		// TODO: check how to delete group
		delete group;
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

// Test get group and device names

	void test_miscellaneous()
	{
		// TODO : test somehow getting sub groups names
		vector<string> groups, devices, device_names(2);
		Group *group_tmp, *sub_group_tmp;
		DeviceProxy *device_tmp;

		// group name
		TS_ASSERT(group->get_name() == "group");

		// group size
		TS_ASSERT(group->get_size() == 3);

		// get sub group
		sub_group_tmp = group->get_group("sub_group");
		TS_ASSERT(sub_group_tmp != 0);
		TS_ASSERT(sub_group_tmp->get_name() == "sub_group");

		// get sub group parent
		group_tmp = sub_group_tmp->get_parent();
		TS_ASSERT(group_tmp->get_name() == "group");

		// sub_group devices names
		devices = sub_group_tmp->get_device_list();
		TS_ASSERT(devices.size() == 1);
		TS_ASSERT(devices[0] == device1_name);

		// devices names with forwarding
		devices = group->get_device_list();
		TS_ASSERT(devices.size() == 3);
		TS_ASSERT(devices[0] == device1_name);
		TS_ASSERT(devices[1] == device2_name);
		TS_ASSERT(devices[2] == device3_name);

		// devices names without forwarding
		devices = group->get_device_list(false);
		TS_ASSERT(devices.size() == 2);
		TS_ASSERT(devices[0] == device2_name);
		TS_ASSERT(devices[1] == device3_name);

		// contains() method
		TS_ASSERT(group->contains(device1_name) == true);
		TS_ASSERT(group->contains("nonexistent_name") == false);

		// patterns
		TS_ASSERT(group->name_equals("group") == true);
		TS_ASSERT(sub_group->name_matches("group") == true);

		// root
		TS_ASSERT(group->is_root_group() == true);
		TS_ASSERT(sub_group->is_root_group() == false);

		// add & remove
		device_names[0] = device2_name;
		device_names[1] = device3_name;
		group->remove(device_names);
		TS_ASSERT(group->get_size() == 1)
		group->add(device_names);
		TS_ASSERT(group->get_size() == 3)
		group->remove(device3_name);
		TS_ASSERT(group->get_size() == 2)
		group->add(device3_name);
		TS_ASSERT(group->get_size() == 3)
		devices.clear();
		devices = group->get_device_list();
		TS_ASSERT(devices[0] == device1_name);
		TS_ASSERT(devices[1] == device2_name);
		TS_ASSERT(devices[2] == device3_name);

		// get device
		device_tmp = group->get_device(device2_name);
		TS_ASSERT(device_tmp->name() == device2_name);
		device_tmp = group->get_device(3);
		TS_ASSERT(device_tmp->name() == device3_name);

		// ping
		TS_ASSERT(group->ping() == true);
	}

// Test synchronous command with forwarding (default) and no arguments

	void test_synchronous_command_with_forwarding_and_no_arguments()
	{
		vector<string> device_names;
		device_names.push_back(device1_name);
		device_names.push_back(device2_name);
		device_names.push_back(device3_name);

		GroupCmdReplyList crl = group->command_inout("State");
		TS_ASSERT(crl.has_failed() == false);
		TS_ASSERT(crl.size() == 3);
		for(size_t i = 0; i < crl.size(); i++)
		{
			DevState state;
			crl[i] >> state;
			TS_ASSERT(state == ON);
			TS_ASSERT(crl[i].dev_name() == device_names[i]);
			TS_ASSERT(crl[i].obj_name() == "State");

			DeviceData dd = crl[i].get_data();
			dd >> state;
			TS_ASSERT(state == ON);
		}
	}

// Test asynchronous command with forwarding (default) and no arguments

	void test_asynchronous_command_with_forwarding_and_no_arguments()
	{
		long request_id = group->command_inout_asynch("State");
		GroupCmdReplyList crl = group->command_inout_reply(request_id);
		TS_ASSERT(crl.has_failed() == false);
		TS_ASSERT(crl.size() == 3);
		for(size_t i = 0; i < crl.size(); i++)
		{
			DevState state;
			crl[i] >> state;
			TS_ASSERT(state == ON);
		}
	}

// Test synchronous command with no forwarding and no arguments

	void test_synchronous_command_with_no_forwarding_and_no_arguments()
	{
		GroupCmdReplyList crl = group->command_inout("State",false);
		TS_ASSERT(crl.has_failed() == false);
		TS_ASSERT(crl.size() == 2);
		for(size_t i = 0; i < crl.size(); i++)
		{
			DevState state;
			crl[i] >> state;
			TS_ASSERT(state == ON);
		}
	}

// Test asynchronous command with no forwarding and no arguments

	void test_asynchronous_command_with_no_forwarding_and_no_arguments()
	{
		long request_id = group->command_inout_asynch("State",false,false);
		GroupCmdReplyList crl = group->command_inout_reply(request_id);
		TS_ASSERT(crl.has_failed() == false);
		TS_ASSERT(crl.size() == 2);
		for(size_t i = 0; i < crl.size(); i++)
		{
			DevState state;
			crl[i] >> state;
			TS_ASSERT(state == ON);
		}
	}

// Test synchronous command with forwarding (default) and one argument

	void test_synchronous_command_with_forwarding_and_one_argument()
	{
		DeviceData dd;
		DevDouble db = 5.0;
		dd << db;
		GroupCmdReplyList crl = group->command_inout("IODouble",dd);
		TS_ASSERT(crl.has_failed() == false);
		TS_ASSERT(crl.size() == 3);
		for(size_t i = 0; i < crl.size(); i++)
		{
			crl[i] >> db;
			TS_ASSERT(db == 10.0);
		}
	}

// Test asynchronous command with forwarding (default) and one argument

	void test_asynchronous_command_with_forwarding_and_one_argument()
	{
		DeviceData dd;
		DevDouble db = 15.0;
		dd << db;
		long request_id = group->command_inout_asynch("IODouble",dd);
		GroupCmdReplyList crl = group->command_inout_reply(request_id);
		TS_ASSERT(crl.has_failed() == false);
		TS_ASSERT(crl.size() == 3);
		for(size_t i = 0; i < crl.size(); i++)
		{
			crl[i] >> db;
			TS_ASSERT(db == 30.0);
		}
	}

// Test synchronous command with forwarding (default) and several arguments

	void test_synchronous_command_with_forwarding_and_several_arguments()
	{
		DevDouble db;
		vector<DevDouble> arguments(3);
		arguments[0] = 15.0;
		arguments[1] = 25.0;
		arguments[2] = 35.0;
		GroupCmdReplyList crl = group->command_inout("IODouble",arguments);
		TS_ASSERT(crl.has_failed() == false);
		TS_ASSERT(crl.size() == 3);

		crl[0] >> db;
		TS_ASSERT(db == 30.0);
		crl[1] >> db;
		TS_ASSERT(db == 50.0);
		crl[2] >> db;
		TS_ASSERT(db == 70.0);
	}

// Test asynchronous command with forwarding (default) and several arguments

	void test_asynchronous_command_with_forwarding_and_several_arguments()
	{
		DevDouble db;
		vector<DevDouble> arguments(3);
		arguments[0] = 45.0;
		arguments[1] = 55.0;
		arguments[2] = 65.0;
		long request_id = group->command_inout_asynch("IODouble",arguments);
		GroupCmdReplyList crl = group->command_inout_reply(request_id);
		TS_ASSERT(crl.has_failed() == false);
		TS_ASSERT(crl.size() == 3);

		crl[0] >> db;
		TS_ASSERT(db == 90.0);
		crl[1] >> db;
		TS_ASSERT(db == 110.0);
		crl[2] >> db;
		TS_ASSERT(db == 130.0);
	}

// Test synchronous command with forwarding (default) and several DeviceData arguments

	void test_synchronous_command_with_forwarding_and_several_DeviceData_arguments()
	{
		DevDouble db;
		DeviceData dd1, dd2, dd3;
		vector<DeviceData> arguments;
		dd1 << 15.0;
		dd2 << 25.0;
		dd3 << 35.0;
		arguments.push_back(dd1);
		arguments.push_back(dd2);
		arguments.push_back(dd3);
		GroupCmdReplyList crl = group->command_inout("IODouble",arguments);
		TS_ASSERT(crl.has_failed() == false);
		TS_ASSERT(crl.size() == 3);

		crl[0] >> db;
		TS_ASSERT(db == 30.0);
		crl[1] >> db;
		TS_ASSERT(db == 50.0);
		crl[2] >> db;
		TS_ASSERT(db == 70.0);
	}

// Test asynchronous command with forwarding (default) and several DeviceData arguments

	void test_asynchronous_command_with_forwarding_and_several_DeviceData_arguments()
	{
		DevDouble db;
		DeviceData dd1, dd2, dd3;
		vector<DeviceData> arguments;
		dd1 << 45.0;
		dd2 << 55.0;
		dd3 << 65.0;
		arguments.push_back(dd1);
		arguments.push_back(dd2);
		arguments.push_back(dd3);
		long request_id = group->command_inout_asynch("IODouble",arguments);
		GroupCmdReplyList crl = group->command_inout_reply(request_id);
		TS_ASSERT(crl.has_failed() == false);
		TS_ASSERT(crl.size() == 3);

		crl[0] >> db;
		TS_ASSERT(db == 90.0);
		crl[1] >> db;
		TS_ASSERT(db == 110.0);
		crl[2] >> db;
		TS_ASSERT(db == 130.0);

		// wrong number of arguments
		DeviceData dd;
		dd << 75.0;
		arguments.push_back(dd);
		TS_ASSERT_THROWS_ASSERT(group->command_inout_asynch("IODouble",arguments), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_MethodArgument
						&& e.errors[0].severity == Tango::ERR));
	}

// Test synchronous command with forwarding (default) and wrong number of arguments

	void test_synchronous_command_with_forwarding_and_wrong_number_of_arguments()
	{
		vector<DevDouble> arguments(2);
		arguments[0] = 15.0;
		arguments[1] = 25.0;
		TS_ASSERT_THROWS_ASSERT(group->command_inout("IODouble",arguments), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_MethodArgument
						&& e.errors[0].severity == Tango::ERR));
	}

// Test synchronous command throwing an exception with enable exception mode ON

	void test_synchronous_command_throwing_exception_mode_on()
	{
		DevDouble db;
		bool last_mode = GroupReply::enable_exception(true);
		GroupCmdReplyList crl = group->command_inout("IOExcept");
		TS_ASSERT(crl.has_failed() == true);
		TS_ASSERT(crl.size() == 3);
		TS_ASSERT_THROWS_ASSERT(crl[0] >> db, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_ThrowException
						&& e.errors[0].severity == Tango::ERR));
		TS_ASSERT_THROWS_ASSERT(crl[1] >> db, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_ThrowException
						&& e.errors[0].severity == Tango::ERR));
		TS_ASSERT_THROWS_ASSERT(crl[2] >> db, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_ThrowException
						&& e.errors[0].severity == Tango::ERR));
		GroupReply::enable_exception(last_mode);
	}

// Test synchronous command throwing an exception with enable exception mode OFF

	void test_synchronous_command_throwing_exception_mode_off()
	{
		bool last_mode = GroupReply::enable_exception(false);
		GroupCmdReplyList crl = group->command_inout("IOExcept");
		TS_ASSERT(crl.has_failed() == true);
		TS_ASSERT(crl.size() == 3);
		TS_ASSERT(crl[0].has_failed() == true);
		TS_ASSERT(crl[1].has_failed() == true);
		TS_ASSERT(crl[2].has_failed() == true);
		TS_ASSERT(string(crl[0].get_err_stack()[0].reason.in()) == API_ThrowException);
		TS_ASSERT(string(crl[1].get_err_stack()[0].reason.in()) == API_ThrowException);
		TS_ASSERT(string(crl[2].get_err_stack()[0].reason.in()) == API_ThrowException);
		GroupReply::enable_exception(last_mode);
	}

// Test read attribute synchronously

	void test_read_attribute_synchronously()
	{
		vector<string> device_names;
		device_names.push_back(device1_name);
		device_names.push_back(device2_name);
		device_names.push_back(device3_name);

		GroupAttrReplyList arl = group->read_attribute("Double_attr");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		for(size_t i = 0; i < arl.size(); i++)
		{
			DevDouble db;
			arl[i] >> db;
			TS_ASSERT(db == 3.2);
			TS_ASSERT(arl[i].dev_name() == device_names[i]);
			TS_ASSERT(arl[i].obj_name() == "Double_attr");

			DeviceAttribute da = arl[i].get_data();
			da >> db;
			TS_ASSERT(db == 3.2);
		}
	}

// Test read attribute asynchronously

	void test_read_attribute_asynchronously()
	{
		long request_id = group->read_attribute_asynch("Double_attr");
		GroupAttrReplyList arl = group->read_attribute_reply(request_id);
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		for(size_t i = 0; i < arl.size(); i++)
		{
			DevDouble db;
			arl[i] >> db;
			TS_ASSERT(db == 3.2);
		}
	}

// Test read several attributes synchronously

	void test_read_several_attributes_synchronously()
	{
		vector<string> attributes;
		attributes.push_back("Double_attr");
		attributes.push_back("Float_attr");

		GroupAttrReplyList arl = group->read_attributes(attributes);
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 6);

		DevDouble db;
		DevFloat fl;
		arl[0] >> db;
		TS_ASSERT(db == 3.2);
		TS_ASSERT(arl[0].dev_name() == device1_name);
		TS_ASSERT(arl[0].obj_name() == "Double_attr");
		db = 0;
		arl[1] >> fl;
		TS_ASSERT(fl == 4.5);
		TS_ASSERT(arl[1].dev_name() == device1_name);
		TS_ASSERT(arl[1].obj_name() == "Float_attr");

		arl[2] >> db;
		TS_ASSERT(db == 3.2);
		TS_ASSERT(arl[2].dev_name() == device2_name);
		TS_ASSERT(arl[2].obj_name() == "Double_attr");
		arl[3] >> fl;
		TS_ASSERT(fl == 4.5);
		TS_ASSERT(arl[3].dev_name() == device2_name);
		TS_ASSERT(arl[3].obj_name() == "Float_attr");

		arl[4] >> db;
		TS_ASSERT(db == 3.2);
		TS_ASSERT(arl[4].dev_name() == device3_name);
		TS_ASSERT(arl[4].obj_name() == "Double_attr");
		arl[5] >> fl;
		TS_ASSERT(fl == 4.5);
		TS_ASSERT(arl[5].dev_name() == device3_name);
		TS_ASSERT(arl[5].obj_name() == "Float_attr");
	}

// Test read attribute synchronously with throwing exception mode on

	void test_read_attribute_synchronously_throwing_exception_mode_on()
	{
		DevDouble db;
		bool last_mode = GroupReply::enable_exception(true);
		GroupAttrReplyList arl = group->read_attribute("nonexistent_attr");
		TS_ASSERT(arl.has_failed() == true);
		TS_ASSERT(arl.size() == 3);
		TS_ASSERT_THROWS_ASSERT(arl[0] >> db, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_AttrNotFound
						&& e.errors[0].severity == Tango::ERR));
		TS_ASSERT_THROWS_ASSERT(arl[1] >> db, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_AttrNotFound
						&& e.errors[0].severity == Tango::ERR));
		TS_ASSERT_THROWS_ASSERT(arl[2] >> db, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_AttrNotFound
						&& e.errors[0].severity == Tango::ERR));
		GroupReply::enable_exception(last_mode);
	}

// Test read attribute synchronously with throwing exception mode off

	void test_read_attribute_synchronously_throwing_exception_mode_off()
	{
		bool last_mode = GroupReply::enable_exception(false);
		GroupAttrReplyList arl = group->read_attribute("nonexistent_attr");
		TS_ASSERT(arl.has_failed() == true);
		TS_ASSERT(arl.size() == 3);
		TS_ASSERT(arl[0].has_failed() == true);
		TS_ASSERT(arl[1].has_failed() == true);
		TS_ASSERT(arl[2].has_failed() == true);
		TS_ASSERT(string(arl[0].get_err_stack()[0].reason.in()) == API_AttrNotFound);
		TS_ASSERT(string(arl[1].get_err_stack()[0].reason.in()) == API_AttrNotFound);
		TS_ASSERT(string(arl[2].get_err_stack()[0].reason.in()) == API_AttrNotFound);
		GroupReply::enable_exception(last_mode);
	}

// Test write attribute synchronously one value

	void test_write_attribute_synchronously_one_value()
	{
		CxxTest::TangoPrinter::restore_set("double_attr_value");

		// write attribute
		DeviceAttribute value("Double_attr_w",11.1);
		GroupReplyList rl = group->write_attribute(value);
		TS_ASSERT(rl.has_failed() == false);

		// read attribute to check if new value was properly set
		GroupAttrReplyList arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		for(size_t i = 0; i < arl.size(); i++)
		{
			DevDouble db;
			arl[i] >> db;
			TS_ASSERT(db == 11.1);
		}

		// write old value to restore the defaults and read to check if successful
		rl.reset();
		arl.reset();
		DeviceAttribute old_value("Double_attr_w",0.0);
		rl = group->write_attribute(old_value);
		TS_ASSERT(rl.has_failed() == false);
		arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		for(size_t i = 0; i < arl.size(); i++)
		{
			DevDouble db;
			arl[i] >> db;
			TS_ASSERT(db == 0.0);
		}

		CxxTest::TangoPrinter::restore_unset("double_attr_value");
	}

// Test write attribute asynchronously one value

	void test_write_attribute_asynchronously_one_value()
	{
		CxxTest::TangoPrinter::restore_set("double_attr_value");

		// write attribute
		DeviceAttribute value("Double_attr_w",22.2);
		long request_id = group->write_attribute_asynch(value);
		GroupReplyList rl = group->write_attribute_reply(request_id);
		TS_ASSERT(rl.has_failed() == false);

		// read attribute to check if new value was properly set
		GroupAttrReplyList arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		for(size_t i = 0; i < arl.size(); i++)
		{
			DevDouble db;
			arl[i] >> db;
			TS_ASSERT(db == 22.2);
		}

		// write old value to restore the defaults and read to check if successful
		rl.reset();
		arl.reset();
		DeviceAttribute old_value("Double_attr_w",0.0);
		rl = group->write_attribute(old_value);
		TS_ASSERT(rl.has_failed() == false);
		arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		for(size_t i = 0; i < arl.size(); i++)
		{
			DevDouble db;
			arl[i] >> db;
			TS_ASSERT(db == 0.0);
		}

		CxxTest::TangoPrinter::restore_unset("double_attr_value");
	}

// Test write attribute synchronously several values

	void test_write_attribute_synchronously_several_values()
	{
		CxxTest::TangoPrinter::restore_set("double_attr_value");

		// write attribute
		vector<DevDouble> values(3);
		values[0] = 33.3;
		values[1] = 33.4;
		values[2] = 33.5;
		GroupReplyList rl = group->write_attribute("Double_attr_w",values,true);
		TS_ASSERT(rl.has_failed() == false);

		// read attribute to check if new value was properly set
		GroupAttrReplyList arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		DevDouble db;
		arl[0] >> db;
		TS_ASSERT(db == 33.3);
		arl[1] >> db;
		TS_ASSERT(db == 33.4);
		arl[2] >> db;
		TS_ASSERT(db == 33.5);

		// write old value to restore the defaults and read to check if successful
		rl.reset();
		arl.reset();
		DeviceAttribute old_value("Double_attr_w",0.0);
		rl = group->write_attribute(old_value);
		TS_ASSERT(rl.has_failed() == false);
		arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		for(size_t i = 0; i < arl.size(); i++)
		{
			DevDouble db;
			arl[i] >> db;
			TS_ASSERT(db == 0.0);
		}

		CxxTest::TangoPrinter::restore_unset("double_attr_value");
	}

// Test write attribute asynchronously several values

	void test_write_attribute_asynchronously_several_values()
	{
		CxxTest::TangoPrinter::restore_set("double_attr_value");

		// write attribute
		vector<DevDouble> values(3);
		values[0] = 44.4;
		values[1] = 44.5;
		values[2] = 44.6;
		long request_id = group->write_attribute_asynch("Double_attr_w",values,true);
		GroupReplyList rl = group->write_attribute_reply(request_id);
		TS_ASSERT(rl.has_failed() == false);

		// read attribute to check if new value was properly set
		GroupAttrReplyList arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		DevDouble db;
		arl[0] >> db;
		TS_ASSERT(db == 44.4);
		arl[1] >> db;
		TS_ASSERT(db == 44.5);
		arl[2] >> db;
		TS_ASSERT(db == 44.6);

		// write old value to restore the defaults and read to check if successful
		rl.reset();
		arl.reset();
		DeviceAttribute old_value("Double_attr_w",0.0);
		rl = group->write_attribute(old_value);
		TS_ASSERT(rl.has_failed() == false);
		arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		for(size_t i = 0; i < arl.size(); i++)
		{
			DevDouble db;
			arl[i] >> db;
			TS_ASSERT(db == 0.0);
		}

		CxxTest::TangoPrinter::restore_unset("double_attr_value");
	}

// Test write attribute synchronously several DeviceAttribute values

	void test_write_attribute_synchronously_several_DeviceAttribute_values()
	{
		CxxTest::TangoPrinter::restore_set("double_attr_value");

		// write attribute
		DeviceAttribute da1("Double_attr_w",DevDouble(55.5)), da2("Double_attr_w",DevDouble(55.6)), da3("Double_attr_w",DevDouble(55.7));
		vector<DeviceAttribute> values;
		values.push_back(da1);
		values.push_back(da2);
		values.push_back(da3);
		GroupReplyList rl = group->write_attribute(values,true);
		TS_ASSERT(rl.has_failed() == false);

		// read attribute to check if new value was properly set
		GroupAttrReplyList arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		DevDouble db;
		arl[0] >> db;
		TS_ASSERT(db == 55.5);
		arl[1] >> db;
		TS_ASSERT(db == 55.6);
		arl[2] >> db;
		TS_ASSERT(db == 55.7);

		// write old value to restore the defaults and read to check if successful
		rl.reset();
		arl.reset();
		DeviceAttribute old_value("Double_attr_w",0.0);
		rl = group->write_attribute(old_value);
		TS_ASSERT(rl.has_failed() == false);
		arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		for(size_t i = 0; i < arl.size(); i++)
		{
			DevDouble db;
			arl[i] >> db;
			TS_ASSERT(db == 0.0);
		}

		CxxTest::TangoPrinter::restore_unset("double_attr_value");
	}

// Test write attribute asynchronously several DeviceAttribute values

	void test_write_attribute_asynchronously_several_DeviceAttribute_values()
	{
		CxxTest::TangoPrinter::restore_set("double_attr_value");

		// write attribute
		DeviceAttribute da1("Double_attr_w",DevDouble(66.6)), da2("Double_attr_w",DevDouble(66.7)), da3("Double_attr_w",DevDouble(66.8)), da4("Double_attr_w",DevDouble(66.9));
		vector<DeviceAttribute> values;
		values.push_back(da1);
		values.push_back(da2);
		values.push_back(da3);
		long request_id = group->write_attribute_asynch(values,true);
		GroupReplyList rl = group->write_attribute_reply(request_id);
		TS_ASSERT(rl.has_failed() == false);

		// wrong number of arguments
		values.push_back(da4);
		TS_ASSERT_THROWS_ASSERT(group->write_attribute_asynch(values,true), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_MethodArgument
						&& e.errors[0].severity == Tango::ERR));

		// read attribute to check if new value was properly set
		GroupAttrReplyList arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		DevDouble db;
		arl[0] >> db;
		TS_ASSERT(db == 66.6);
		arl[1] >> db;
		TS_ASSERT(db == 66.7);
		arl[2] >> db;
		TS_ASSERT(db == 66.8);

		// write old value to restore the defaults and read to check if successful
		rl.reset();
		arl.reset();
		DeviceAttribute old_value("Double_attr_w",0.0);
		rl = group->write_attribute(old_value);
		TS_ASSERT(rl.has_failed() == false);
		arl = group->read_attribute("Double_attr_w");
		TS_ASSERT(arl.has_failed() == false);
		TS_ASSERT(arl.size() == 3);
		for(size_t i = 0; i < arl.size(); i++)
		{
			DevDouble db;
			arl[i] >> db;
			TS_ASSERT(db == 0.0);
		}

		CxxTest::TangoPrinter::restore_unset("double_attr_value");
	}

	/* Verifies that a group can contain devices from a remote TANGO_HOST
	 * (a Tango instance different from client's default TANGO_HOST).
	 * An issue was reported when resolving names containing wildcards.
	 * Update: to simplify test setup, the scenario has been changed
	 * to unset client's TANGO_HOST instead of providing different value. */

	void test_use_devices_from_remote_tango_host()
	{
		const std::string original_tango_host = std::getenv("TANGO_HOST");

		TS_ASSERT_EQUALS(0, unsetenv("TANGO_HOST"));
		ApiUtil::instance()->cleanup();

		Group group("group");
		group.add("tango://" + original_tango_host + "/" + device1_name + "*");

		GroupCmdReplyList command_results = group.command_inout("State");
		GroupCmdReply command_result = command_results[0];

		TS_ASSERT(not command_results.has_failed());
		TS_ASSERT_EQUALS(1u, command_results.size());

		TS_ASSERT(not command_result.has_failed());

		DevState state;
		TS_ASSERT(command_result >> state);
		TS_ASSERT_EQUALS(ON, state);

		const bool force_update = true;
		TS_ASSERT_EQUALS(0, setenv("TANGO_HOST", original_tango_host.c_str(), force_update));
		ApiUtil::instance()->cleanup();
	}

};
#undef cout
#endif // GroupTestSuite_h
