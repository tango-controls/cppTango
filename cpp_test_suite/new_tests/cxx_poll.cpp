#ifndef PollTestSuite_h
#define PollTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME PollTestSuite__loop

class PollTestSuite__loop: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;
	string device1_name, dserver_name;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		device1_name = CxxTest::TangoPrinter::get_param("device1");
		dserver_name = "dserver/" + CxxTest::TangoPrinter::get_param("fulldsname");

		CxxTest::TangoPrinter::get_param_opt("loop");
		CxxTest::TangoPrinter::get_param_opt("suiteloop");

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

//
// Clean up --------------------------------------------------------
//

		// clean up in case test suite terminates before 'Double_attr'
		// attribute polling state is restored to defaults for device1
		if(CxxTest::TangoPrinter::is_restore_set("dev1_Double_attr_polling"))
		{
			DeviceData din;
			DevVarStringArray rem_attr_poll;
			rem_attr_poll.length(3);
			rem_attr_poll[0] = device1_name.c_str();
			rem_attr_poll[1] = "attribute";
			rem_attr_poll[2] = "Double_attr";
			din << rem_attr_poll;
			try
			{
				dserver->command_inout("RemObjPolling", din);
			}
			catch(DevFailed &e)
			{
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before 'IOStr1'
		// command polling state is restored to defaults for device1
		if(CxxTest::TangoPrinter::is_restore_set("dev1_IOStr1_polling"))
		{
			DeviceData din;
			DevVarStringArray rem_cmd_poll;
			rem_cmd_poll.length(3);
			rem_cmd_poll[0] = device1_name.c_str();
			rem_cmd_poll[1] = "command";
			rem_cmd_poll[2] = "IOStr1";
			din << rem_cmd_poll;
			try
			{
				dserver->command_inout("RemObjPolling", din);
			}
			catch(DevFailed &e)
			{
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before 'attr_wrong_size'
		// attribute polling state is restored to defaults for device1
		if(CxxTest::TangoPrinter::is_restore_set("dev1_attr_wrong_size_polling"))
		{
			DeviceData din;
			DevVarStringArray rem_attr_poll;
			rem_attr_poll.length(3);
			rem_attr_poll[0] = device1_name.c_str();
			rem_attr_poll[1] = "attribute";
			rem_attr_poll[2] = "attr_wrong_size";
			din << rem_attr_poll;
			try
			{
				dserver->command_inout("RemObjPolling", din);
			}
			catch(DevFailed &e)
			{
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before 'IOExcept'
		// command polling state is restored to defaults for device1
		if(CxxTest::TangoPrinter::is_restore_set("dev1_IOExcept_polling"))
		{
			DeviceData din;
			DevVarStringArray rem_cmd_poll;
			rem_cmd_poll.length(3);
			rem_cmd_poll[0] = device1_name.c_str();
			rem_cmd_poll[1] = "command";
			rem_cmd_poll[2] = "IOExcept";
			din << rem_cmd_poll;
			try
			{
				dserver->command_inout("RemObjPolling", din);
			}
			catch(DevFailed &e)
			{
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
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

// Test polling exceptions

	void test_polling_exceptions(void)
	{
		DeviceData dout;
		DeviceAttribute short_attr, mock_attr;
		DevShort sh;
		string str_out;

		// check if the devices are not polled
		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		for (unsigned int i = 0;i < polled_devices->length();i++)
			cout << "Polled device = " << (*polled_devices)[i] << endl;
		TS_ASSERT((*polled_devices).length() == 0);

		// check if the data source is set to polling buffer and than device (CACHE_DEV)
		TS_ASSERT(device1->get_source() == Tango::CACHE_DEV);

		// set the data source to device (DEV)
		TS_ASSERT_THROWS_NOTHING(device1->set_source(Tango::DEV));

		// execute some commands
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOVoid"));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IOStr1"));
		dout >> str_out;
		TS_ASSERT(str_out == "Hello from IOStr1");

		// execute non-existing command
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOxxx"), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_CommandNotFound
						&& e.errors[0].severity == Tango::ERR));

		// now set the data source to polling buffer (CACHE)
		TS_ASSERT_THROWS_NOTHING(device1->set_source(Tango::CACHE));

		// execute non-existing command
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOxxx"), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_CommandNotFound
						&& e.errors[0].severity == Tango::ERR));

		// execute non-polled command
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOStr1"), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_CmdNotPolled
						&& e.errors[0].severity == Tango::ERR));

		// set the data source again to device (DEV)
		TS_ASSERT_THROWS_NOTHING(device1->set_source(Tango::DEV));

		// read an attribute
		TS_ASSERT_THROWS_NOTHING(short_attr = device1->read_attribute("Short_attr"));
		short_attr >> sh;
		TS_ASSERT(short_attr.name == "Short_attr");
		TS_ASSERT(short_attr.quality == Tango::ATTR_VALID);
		TS_ASSERT(short_attr.dim_x == 1);
		TS_ASSERT(short_attr.dim_y == 0);
		TS_ASSERT(sh == 12);

		// read a non-existing attribute
		TS_ASSERT_THROWS_NOTHING(mock_attr = device1->read_attribute("xxx"));
		TS_ASSERT_THROWS_ASSERT(mock_attr >> sh, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_AttrNotFound
						&& e.errors[0].severity == Tango::ERR));

		// set the data source again to polling buffer (CACHE)
		TS_ASSERT_THROWS_NOTHING(device1->set_source(Tango::CACHE));

		// again read a non-existing attribute
		TS_ASSERT_THROWS_NOTHING(mock_attr = device1->read_attribute("xxx"));
		TS_ASSERT_THROWS_ASSERT(mock_attr >> sh, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_AttrNotFound
						&& e.errors[0].severity == Tango::ERR));

		// read a non-polled attribute
		TS_ASSERT_THROWS_NOTHING(short_attr = device1->read_attribute("Short_attr"));
		TS_ASSERT_THROWS_ASSERT(short_attr >> sh, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_AttrNotPolled
						&& e.errors[0].severity == Tango::ERR));
	}

// Test commands and attributes obtained from polling buffer

	void test_commands_and_attributes_obtained_from_polling_buffer(void)
	{
		DeviceData din, dout;
		DevVarLongStringArray attr_poll, cmd_poll;

		// poll Double_attr
		attr_poll.lvalue.length(1);
		attr_poll.lvalue[0] = 200;
		attr_poll.svalue.length(3);
		attr_poll.svalue[0] = device1_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "Double_attr";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		CxxTest::TangoPrinter::restore_set("dev1_Double_attr_polling");

		// poll IOStr1
		cmd_poll.lvalue.length(1);
		cmd_poll.lvalue[0] = 200;
		cmd_poll.svalue.length(3);
		cmd_poll.svalue[0] = device1_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOStr1";
		din << cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		CxxTest::TangoPrinter::restore_set("dev1_IOStr1_polling");

		// poll attr_wrong_size
		attr_poll.lvalue[0] = 300;
		attr_poll.svalue[0] = device1_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "attr_wrong_size";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		CxxTest::TangoPrinter::restore_set("dev1_attr_wrong_size_polling");

		// poll IOExcept
		cmd_poll.lvalue[0] = 550;
		cmd_poll.svalue[0] = device1_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOExcept";
		din << cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		CxxTest::TangoPrinter::restore_set("dev1_IOExcept_polling");
		Tango_sleep(3);

		// set the data source to polling buffer (CACHE)
		TS_ASSERT_THROWS_NOTHING(device1->set_source(Tango::CACHE));

		// execute a command
		string str_out;
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IOStr1"));
		dout >> str_out;
		TS_ASSERT(str_out == "Hello from IOStr1");

		// read an attribute
		DeviceAttribute db_attr;
		DevDouble db;
		TS_ASSERT_THROWS_NOTHING(db_attr = device1->read_attribute("Double_attr"));
		db_attr >> db;
		TS_ASSERT(db_attr.name == "Double_attr");
		TS_ASSERT(db_attr.quality == Tango::ATTR_VALID);
		TS_ASSERT(db_attr.dim_x == 1);
		TS_ASSERT(db_attr.dim_y == 0);
		TS_ASSERT(db == 3.2);

		// read an attribute which throws an exception
		DeviceAttribute mock_attr;
		DevLong lg;
		TS_ASSERT_THROWS_NOTHING(mock_attr = device1->read_attribute("attr_wrong_size"));
		TS_ASSERT_THROWS_ASSERT(mock_attr >> lg, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_AttrOptProp
						&& e.errors[0].severity == Tango::ERR));

		// execute a command which throws an exception
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOExcept"), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_ThrowException
						&& e.errors[0].severity == Tango::ERR));
	}

// Check the CACHE_DEV source parameter

	void test_check_the_CACHE_DEV_source_parameter(void)
	{
		DeviceData din, dout;
		DeviceAttribute db_attr, mock_attr, lg_attr;
		DevVarStringArray rem_cmd_poll, rem_attr_poll;
		DevDouble db;
		DevLong lg;
		string str_out;

		//
		// Read from cache
 		//

		// set the data source to CACHE_DEV
		TS_ASSERT_THROWS_NOTHING(device1->set_source(Tango::CACHE_DEV));

		// read an attribute from cache
		TS_ASSERT_THROWS_NOTHING(db_attr = device1->read_attribute("Double_attr"));
		db_attr >> db;
		TS_ASSERT(db_attr.name == "Double_attr");
		TS_ASSERT(db_attr.quality == Tango::ATTR_VALID);
		TS_ASSERT(db_attr.dim_x == 1);
		TS_ASSERT(db_attr.dim_y == 0);
		TS_ASSERT(db == 3.2);

		// read an attribute from cache, which throws an exception
		TS_ASSERT_THROWS_NOTHING(mock_attr = device1->read_attribute("attr_wrong_size"));
		TS_ASSERT_THROWS_ASSERT(mock_attr >> lg, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_AttrOptProp
						&& e.errors[0].severity == Tango::ERR));

		//
		// If data not in cache, while data source is set to cache, throw exception
		//

		// set the data source to polling buffer
		TS_ASSERT_THROWS_NOTHING(device1->set_source(Tango::CACHE));

		// read a non-polled attribute from cache
		TS_ASSERT_THROWS_NOTHING(lg_attr = device1->read_attribute("Long_attr"));
		TS_ASSERT_THROWS_ASSERT(lg_attr >> lg, Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_AttrNotPolled
						&& e.errors[0].severity == Tango::ERR));

		// execute non-polled command with polling buffer as data source
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOStr2"), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_CmdNotPolled
						&& e.errors[0].severity == Tango::ERR));

		//
		// If data not in cache, read from device
		//

		// set the data source to polling buffer and then device
		TS_ASSERT_THROWS_NOTHING(device1->set_source(Tango::CACHE_DEV));

		// try to read a non-polled attribute from cache and then from device
		TS_ASSERT_THROWS_NOTHING(lg_attr = device1->read_attribute("Long_attr"));
		lg_attr >> lg;
		TS_ASSERT(lg_attr.name == "Long_attr");
		TS_ASSERT(lg_attr.quality == Tango::ATTR_VALID);
		TS_ASSERT(lg_attr.dim_x == 1);
		TS_ASSERT(lg_attr.dim_y == 0);
		TS_ASSERT(lg == 1246);

		// execute non-polled command firstly with polling buffer and then device as the data source
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("IOStr2"));
		dout >> str_out;
		TS_ASSERT(str_out == "Hello from IOStr2");

		//
		// Restore polling to defaults
		//

		rem_attr_poll.length(3);
		rem_cmd_poll.length(3);

		// remove Double_attr polling
		rem_attr_poll[0] = device1_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "Double_attr";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		CxxTest::TangoPrinter::restore_unset("dev1_Double_attr_polling");

		// remove IOStr1 polling
		rem_cmd_poll[0] = device1_name.c_str();
		rem_cmd_poll[1] = "command";
		rem_cmd_poll[2] = "IOStr1";
		din << rem_cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		CxxTest::TangoPrinter::restore_unset("dev1_IOStr1_polling");

		// remove attr_wrong_size polling
		rem_attr_poll[0] = device1_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "attr_wrong_size";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		CxxTest::TangoPrinter::restore_unset("dev1_attr_wrong_size_polling");

		// remove IOExcept polling
		rem_cmd_poll[0] = device1_name.c_str();
		rem_cmd_poll[1] = "command";
		rem_cmd_poll[2] = "IOExcept";
		din << rem_cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		CxxTest::TangoPrinter::restore_unset("dev1_IOExcept_polling");
	}
};
#undef cout
#endif // PollTestSuite_h
