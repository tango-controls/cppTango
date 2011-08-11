#ifndef PollAdminTestSuite_h
#define PollAdminTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME PollAdminTestSuite

class PollAdminTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;
	string device1_name, device2_name, dserver_name;
	bool polling_restored, dev1_IOStr1_polling_restored, dev1_double_attr_polling_restored, dev2_IOStr1_polling_restored, dev2_double_attr_polling_restored, dev1_attr_wrong_size_polling_restored, dev1_IOPollStr1_polling_restored;

public:
	SUITE_NAME()
	{

		// clean up flags
		polling_restored = true;
		dev1_IOStr1_polling_restored = true;
		dev1_double_attr_polling_restored = true;
		dev2_IOStr1_polling_restored = true;
		dev2_double_attr_polling_restored = true;
		dev1_attr_wrong_size_polling_restored = true;
		dev1_IOPollStr1_polling_restored = true;

//
// Arguments check -------------------------------------------------
//

		vector<string> uargs; // user arguments
		uargs.push_back("device1");
		uargs.push_back("device2");

		vector<string> params; // parameters
		params.push_back("fulldsname");

		vector<string> params_opt; // optional parameters
		params_opt.push_back("loop");

		bool params_ok = true;
		for(size_t i = 0; i < params.size() && params_ok; i++)
			params_ok = CxxTest::TangoPrinter::is_param_set(params[i]);

		if(CxxTest::TangoPrinter::get_uargc() >= 3 && params_ok)
		{
			device1_name = CxxTest::TangoPrinter::get_uargv()[0];
			device2_name = CxxTest::TangoPrinter::get_uargv()[1];

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
			device1 = new DeviceProxy(device1_name);
			dserver = new DeviceProxy(dserver_name);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

		cout << endl;
		cout << "new DeviceProxy(" << device1->name() << ") returned" << endl;
		cout << "new DeviceProxy(" << dserver->name() << ") returned" << endl << endl;

	}

	virtual ~SUITE_NAME()
	{
		cout << endl;

//
// Clean up --------------------------------------------------------
//

		// clean up in case test suite terminates before polling state is restored to defaults
		if(!polling_restored)
		{
			try
			{
				dserver->command_inout("StartPolling");
			}
			catch(DevFailed &e)
			{
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before 'IOStr1'
		// command polling state is restored to defaults for device1
//		if(!dev1_IOStr1_polling_restored)
//		{
//			DeviceData din;
//			DevVarStringArray rem_cmd_poll;
//			rem_cmd_poll.length(3);
//			rem_cmd_poll[0] = device1_name.c_str();
//			rem_cmd_poll[1] = "command";
//			rem_cmd_poll[2] = "IOStr1";
//			din << rem_cmd_poll;
//			try
//			{
//				dserver->command_inout("RemObjPolling", din);
//			}
//			catch(DevFailed &e)
//			{
//				Except::print_exception(e);
//			}
//		}

		// clean up in case test suite terminates before 'Double_attr'
		// attribute polling state is restored to defaults for device1
//		if(!dev1_double_attr_polling_restored)
//		{
//			DeviceData din;
//			DevVarStringArray rem_attr_poll;
//			rem_attr_poll.length(3);
//			rem_attr_poll[0] = device1_name.c_str();
//			rem_attr_poll[1] = "attribute";
//			rem_attr_poll[2] = "Double_attr";
//			din << rem_attr_poll;
//			try
//			{
//				dserver->command_inout("RemObjPolling", din);
//			}
//			catch(DevFailed &e)
//			{
//				Except::print_exception(e);
//			}
//		}

		// clean up in case test suite terminates before 'IOStr1'
		// command polling state is restored to defaults for device2
		if(!dev2_IOStr1_polling_restored)
		{
			DeviceData din;
			DevVarStringArray rem_cmd_poll;
			rem_cmd_poll.length(3);
			rem_cmd_poll[0] = device2_name.c_str();
			rem_cmd_poll[1] = "command";
			rem_cmd_poll[2] = "IOStr1";
			din << rem_cmd_poll;
			try
			{
				dserver->command_inout("RemObjPolling", din);
			}
			catch(DevFailed &e)
			{
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before 'Double_attr'
		// attribute polling state is restored to defaults for device2
		if(!dev2_double_attr_polling_restored)
		{
			DeviceData din;
			DevVarStringArray rem_attr_poll;
			rem_attr_poll.length(3);
			rem_attr_poll[0] = device2_name.c_str();
			rem_attr_poll[1] = "attribute";
			rem_attr_poll[2] = "Double_attr";
			din << rem_attr_poll;
			try
			{
				dserver->command_inout("RemObjPolling", din);
			}
			catch(DevFailed &e)
			{
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before 'attr_wrong_size'
		// attribute polling state is restored to defaults for device1
		if(!dev1_attr_wrong_size_polling_restored)
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
				Except::print_exception(e);
			}
		}

		// clean up in case test suite terminates before 'IOPollStr1'
		// command polling state is restored to defaults for device1
		if(!dev1_IOPollStr1_polling_restored)
		{
			DeviceData din;
			DevVarStringArray rem_cmd_poll;
			rem_cmd_poll.length(3);
			rem_cmd_poll[0] = device1_name.c_str();
			rem_cmd_poll[1] = "command";
			rem_cmd_poll[2] = "IOPollStr1";
			din << rem_cmd_poll;
			try
			{
				dserver->command_inout("RemObjPolling", din);
			}
			catch(DevFailed &e)
			{
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

// Test Start, Stop polling

	void test_Start_Stop_polling(void)
	{
		DeviceData din, dout;
		string status;

		const DevVarStringArray *status_arr;
				din << device1_name;
				TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
				dout >> status_arr;
				cout << "status length : " << (*status_arr).length() << endl;
				cout << "Polling status on startup: " << endl << string((*status_arr)[0].in()) << endl << endl << string((*status_arr)[1].in()) << endl;

		// check if the devices are not polled
		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices).length() == 0);

		// check if polling is available
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("Status"));
		dout >> status;
		TS_ASSERT(status == "The device is ON\nThe polling is ON");

		// stop polling and check status
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("StopPolling"));
		polling_restored = false;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("Status"));
		dout >> status;
		TS_ASSERT(status == "The device is ON\nThe polling is OFF");

		// start polling and check status
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("StartPolling"));
		polling_restored = true;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("Status"));
		dout >> status;
		TS_ASSERT(status == "The device is ON\nThe polling is ON");
	}

// Test polling status for a non polled device

	void test_get_polling_status_for_a_non_polled_device(void)
	{
		DeviceData din, dout;

		// get polling status for a non existing device
		string mock_device = "toto";
		din << mock_device;
		TS_ASSERT_THROWS_ASSERT(dserver->command_inout("DevPollStatus", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_DeviceNotFound"
						&& e.errors[0].severity == Tango::ERR));

		// get polling status for a non polled device
		string status;
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status;
		TS_ASSERT(status == "");
	}

// Start polling a command

	void test_start_polling_a_command(void)
	{
		DeviceData din, dout;
		DevVarLongStringArray cmd_poll;

		cmd_poll.lvalue.length(1);
		cmd_poll.lvalue[0] = 500;
		cmd_poll.svalue.length(3);
		cmd_poll.svalue[0] = device1_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOStr1";
		din << cmd_poll;

		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_IOStr1_polling_restored = false;
	}

// Check if command polling is started

	void test_check_if_command_polling_is_started(void)
	{
		DeviceData din, dout;

		// check if the device is polled
		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices)[0].in() == device1_name);

		Tango_sleep(3);

		// get polling status for the device
		const DevVarStringArray *status_arr;
		string status, status_ref = "Polled command name = IOStr1\nPolling period (mS) = 500\nPolling ring buffer depth = 10";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared
		status = string((*status_arr)[0].in()).substr(0,status_ref.length());
		TS_ASSERT(status == status_ref);
	}

// Update command polling period

	void test_update_command_polling_period(void)
	{
		DeviceData din, dout;

		// update polling period
		DevVarLongStringArray cmd_poll;
		cmd_poll.lvalue.length(1);
		cmd_poll.lvalue[0] = 200;
		cmd_poll.svalue.length(3);
		cmd_poll.svalue[0] = device1_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOStr1";
		din << cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("UpdObjPollingPeriod", din));

		Tango_sleep(3);

		// check if polling status has been updated
		const DevVarStringArray *status_arr;
		string status, status_ref = "Polled command name = IOStr1\nPolling period (mS) = 200\nPolling ring buffer depth = 10";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared
		status = string((*status_arr)[0].in()).substr(0,status_ref.length());
		TS_ASSERT(status == status_ref);
	}

// Stop polling the command

	void test_stop_polling_the_command(void)
	{
		DeviceData din, dout;

		// stop command polling
		DevVarStringArray rem_cmd_poll;
		rem_cmd_poll.length(3);
		rem_cmd_poll[0] = device1_name.c_str();
		rem_cmd_poll[1] = "command";
		rem_cmd_poll[2] = "IOStr1";
		din << rem_cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_IOStr1_polling_restored = true;

		// confirm that the devices are not polled
		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices).length() == 0);
	}

// Start polling an attribute

	void test_start_polling_an_attribute(void)
	{
		DeviceData din, dout;
		DevVarLongStringArray attr_poll;

		attr_poll.lvalue.length(1);
		attr_poll.lvalue[0] = 200;
		attr_poll.svalue.length(3);
		attr_poll.svalue[0] = device1_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "Double_attr";
		din << attr_poll;

		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_double_attr_polling_restored = false;
	}

// Check if attribute polling is started

	void test_check_if_attribute_polling_is_started(void)
	{
		DeviceData din, dout;

		// check if the device is polled
		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices)[0].in() == device1_name);

		Tango_sleep(3);

		// get polling status for the device
		const DevVarStringArray *status_arr;
		string status, status_ref = "Polled attribute name = Double_attr\nPolling period (mS) = 200\nPolling ring buffer depth = 10";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared
		status = string((*status_arr)[0].in()).substr(0,status_ref.length());
		TS_ASSERT(status == status_ref);
	}

// Update attribute polling period

	void test_update_attribute_polling_period(void)
	{
		DeviceData din, dout;

		// update polling period
		DevVarLongStringArray attr_poll;
		attr_poll.lvalue.length(1);
		attr_poll.lvalue[0] = 500;
		attr_poll.svalue.length(3);
		attr_poll.svalue[0] = device1_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "Double_attr";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("UpdObjPollingPeriod", din));

		Tango_sleep(3);

		// check if polling status has been updated
		const DevVarStringArray *status_arr;
		string status, status_ref = "Polled attribute name = Double_attr\nPolling period (mS) = 500\nPolling ring buffer depth = 10";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared
		status = string((*status_arr)[0].in()).substr(0,status_ref.length());
		TS_ASSERT(status == status_ref);
	}

// Stop polling the attribute

	void test_stop_polling_the_attribute(void)
	{
		DeviceData din, dout;

		// stop attribute polling
		DevVarStringArray rem_attr_poll;
		rem_attr_poll.length(3);
		rem_attr_poll[0] = device1_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "Double_attr";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_double_attr_polling_restored = true;

		// confirm that the devices are not polled
		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices).length() == 0);
	}

// Start polling an attribute and a command

	void test_start_polling_an_attribute_and_a_command(void)
	{
		DeviceData din, dout;
		DevVarLongStringArray attr_poll, cmd_poll;

		// start polling an attribute
		attr_poll.lvalue.length(1);
		attr_poll.lvalue[0] = 200;
		attr_poll.svalue.length(3);
		attr_poll.svalue[0] = device1_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "Double_attr";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_double_attr_polling_restored = false;

		// start polling a command
		cmd_poll.lvalue.length(1);
		cmd_poll.lvalue[0] = 500;
		cmd_poll.svalue.length(3);
		cmd_poll.svalue[0] = device1_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOStr1";
		din << cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_IOStr1_polling_restored = false;
	}

// Check if polling is started

	void test_check_if_polling_is_started(void)
	{
		DeviceData din, dout;

		// check if the device is polled
		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices)[0].in() == device1_name);

		Tango_sleep(3);

		// get polling status for the device
		const DevVarStringArray *status_arr;
		string status[2], status_ref[2];
		status_ref[0] = "Polled command name = IOStr1\nPolling period (mS) = 500\nPolling ring buffer depth = 10";
		status_ref[1] = "Polled attribute name = Double_attr\nPolling period (mS) = 200\nPolling ring buffer depth = 10";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared
		status[0] = string((*status_arr)[0].in()).substr(0,status_ref[0].length());
		TS_ASSERT(status[0] == status_ref[0]);
		status[1] = string((*status_arr)[1].in()).substr(0,status_ref[1].length());
		TS_ASSERT(status[1] == status_ref[1]);
	}

// Stop polling the attribute and the command

	void test_stop_polling_the_attribute_and_the_command(void)
	{
		DeviceData din, dout;

		// stop attribute polling
		DevVarStringArray rem_attr_poll;
		rem_attr_poll.length(3);
		rem_attr_poll[0] = device1_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "Double_attr";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_double_attr_polling_restored = true;

		// stop command polling
		DevVarStringArray rem_cmd_poll;
		rem_cmd_poll.length(3);
		rem_cmd_poll[0] = device1_name.c_str();
		rem_cmd_poll[1] = "command";
		rem_cmd_poll[2] = "IOStr1";
		din << rem_cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_IOStr1_polling_restored = true;

		// confirm that the devices are not polled
		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices).length() == 0);
	}

// Start polling for several devices

	void test_start_polling_for_several_devices(void)
	{
		DeviceData din, dout;
		DevVarLongStringArray attr_poll, cmd_poll;

		// start polling an attribute for device1
		attr_poll.lvalue.length(1);
		attr_poll.lvalue[0] = 200;
		attr_poll.svalue.length(3);
		attr_poll.svalue[0] = device1_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "Double_attr";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_double_attr_polling_restored = false;

		// start polling a command for device1
		cmd_poll.lvalue.length(1);
		cmd_poll.lvalue[0] = 500;
		cmd_poll.svalue.length(3);
		cmd_poll.svalue[0] = device1_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOStr1";
		din << cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_IOStr1_polling_restored = false;

		// start polling an attribute for device2
		attr_poll.lvalue[0] = 200;
		attr_poll.svalue[0] = device2_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "Double_attr";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev2_double_attr_polling_restored = false;

		// start polling a command for device2
		cmd_poll.lvalue[0] = 500;
		cmd_poll.svalue[0] = device2_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOStr1";
		din << cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev2_IOStr1_polling_restored = false;
	}

// Check if polling for several devices is started

	void test_check_if_polling_for_serveral_devices_is_started(void)
	{
		DeviceData din, dout;

		// check if the devices are polled
		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices)[0].in() == device1_name);
		TS_ASSERT((*polled_devices)[1].in() == device2_name);

		Tango_sleep(3);

		const DevVarStringArray *status_arr;
		string status[2], status_ref[2];

		// get polling status for the device1
		status_ref[0] = "Polled command name = IOStr1\nPolling period (mS) = 500\nPolling ring buffer depth = 10";
		status_ref[1] = "Polled attribute name = Double_attr\nPolling period (mS) = 200\nPolling ring buffer depth = 10";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared
		status[0] = string((*status_arr)[0].in()).substr(0,status_ref[0].length());
		TS_ASSERT(status[0] == status_ref[0]);
		status[1] = string((*status_arr)[1].in()).substr(0,status_ref[1].length());
		TS_ASSERT(status[1] == status_ref[1]);

		// get polling status for the device2
		din << device2_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared
		status[0] = string((*status_arr)[0].in()).substr(0,status_ref[0].length());
		TS_ASSERT(status[0] == status_ref[0]);
		status[1] = string((*status_arr)[1].in()).substr(0,status_ref[1].length());
		TS_ASSERT(status[1] == status_ref[1]);
	}

// Stop polling for several devices

	void test_stop_polling_for_several_devices(void)
	{
		DeviceData din, dout;
		DevVarStringArray rem_attr_poll, rem_cmd_poll;

		// stop attribute polling for device1
		rem_attr_poll.length(3);
		rem_attr_poll[0] = device1_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "Double_attr";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_double_attr_polling_restored = true;

		// stop command polling for device1
		rem_cmd_poll.length(3);
		rem_cmd_poll[0] = device1_name.c_str();
		rem_cmd_poll[1] = "command";
		rem_cmd_poll[2] = "IOStr1";
		din << rem_cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_IOStr1_polling_restored = true;

		// stop attribute polling for device2
		rem_attr_poll[0] = device2_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "Double_attr";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev2_double_attr_polling_restored = true;

		// stop command polling for device2
		rem_cmd_poll[0] = device2_name.c_str();
		rem_cmd_poll[1] = "command";
		rem_cmd_poll[2] = "IOStr1";
		din << rem_cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev2_IOStr1_polling_restored = true;

		// confirm that the devices are not polled
		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices).length() == 0);
	}

// Test device polling after a restart

	void test_device_polling_after_a_restart(void)
	{
		DeviceData din, dout;
		DevVarLongStringArray attr_poll, cmd_poll;
		const DevVarStringArray *polled_devices;

		// start polling an attribute
		attr_poll.lvalue.length(1);
		attr_poll.lvalue[0] = 200;
		attr_poll.svalue.length(3);
		attr_poll.svalue[0] = device1_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "Double_attr";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_double_attr_polling_restored = false;

		// start polling a command
		cmd_poll.lvalue.length(1);
		cmd_poll.lvalue[0] = 500;
		cmd_poll.svalue.length(3);
		cmd_poll.svalue[0] = device1_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOStr1";
		din << cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_IOStr1_polling_restored = false;

		// restart the device
		Tango_sleep(3);
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("DevRestart", din));
		Tango_sleep(3);

		// check device polling after a restart
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices)[0].in() == device1_name);

		Tango_sleep(3);

		// get polling status for the device
		const DevVarStringArray *status_arr;
		string status[2], status_ref[2];
		status_ref[0] = "Polled command name = IOStr1\nPolling period (mS) = 500\nPolling ring buffer depth = 10";
		status_ref[1] = "Polled attribute name = Double_attr\nPolling period (mS) = 200\nPolling ring buffer depth = 10";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared
		status[0] = string((*status_arr)[0].in()).substr(0,status_ref[0].length());
		TS_ASSERT(status[0] == status_ref[0]);
		status[1] = string((*status_arr)[1].in()).substr(0,status_ref[1].length());
		TS_ASSERT(status[1] == status_ref[1]);

		// stop attribute polling
		DevVarStringArray rem_attr_poll;
		rem_attr_poll.length(3);
		rem_attr_poll[0] = device1_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "Double_attr";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_double_attr_polling_restored = true;

		// stop command polling
		DevVarStringArray rem_cmd_poll;
		rem_cmd_poll.length(3);
		rem_cmd_poll[0] = device1_name.c_str();
		rem_cmd_poll[1] = "command";
		rem_cmd_poll[2] = "IOStr1";
		din << rem_cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_IOStr1_polling_restored = true;

		// confirm that the device is not polled
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices).length() == 0);
	}

// Poll object which returns an exception

	void test_poll_object_which_returns_an_exception(void)
	{
		DeviceData din, dout;
		DevVarLongStringArray attr_poll, cmd_poll;
		const DevVarStringArray *polled_devices, *status_arr;
		string status[2], status_ref[2];

		// start polling an attribute
		attr_poll.lvalue.length(1);
		attr_poll.lvalue[0] = 200;
		attr_poll.svalue.length(3);
		attr_poll.svalue[0] = device1_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "attr_wrong_size";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_attr_wrong_size_polling_restored = false;

		Tango_sleep(3);

		// get polling status for the device
		status_ref[0] = "Polled attribute name = attr_wrong_size\nPolling period (mS) = 200\nPolling ring buffer depth = 10";
		status_ref[1] = "Last attribute read FAILED :\n\tReason = API_AttrOptProp\n\tDesc = Data size for attribute attr_wrong_size exceeds given limit\n\tOrigin = Attribute::set_value";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first and the last 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other, middle 3 lines could also be compared
		status[0] = string((*status_arr)[0].in()).substr(0,status_ref[0].length()); // first 3 lines
		TS_ASSERT(status[0] == status_ref[0]);
		status[1] = string((*status_arr)[0].in()).substr(
				string((*status_arr)[0].in()).length() - status_ref[1].length(),
				status_ref[1].length()); // last 3 lines
		TS_ASSERT(status[1] == status_ref[1]);

		// restart the device
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("DevRestart", din));
		Tango_sleep(3);

		// again get polling status for the device
		status_ref[0] = "Polled attribute name = attr_wrong_size\nPolling period (mS) = 200\nPolling ring buffer depth = 10";
		status_ref[1] = "Last attribute read FAILED :\n\tReason = API_AttrOptProp\n\tDesc = Data size for attribute attr_wrong_size exceeds given limit\n\tOrigin = Attribute::set_value";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first and the last 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other, middle 3 lines could also be compared
		status[0] = string((*status_arr)[0].in()).substr(0,status_ref[0].length()); // first 3 lines
		TS_ASSERT(status[0] == status_ref[0]);
		status[1] = string((*status_arr)[0].in()).substr(
				string((*status_arr)[0].in()).length() - status_ref[1].length(),
				status_ref[1].length()); // last 3 lines
		TS_ASSERT(status[1] == status_ref[1]);

		// stop attribute polling
		DevVarStringArray rem_attr_poll;
		rem_attr_poll.length(3);
		rem_attr_poll[0] = device1_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "attr_wrong_size";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_attr_wrong_size_polling_restored = true;

		// confirm that the device is not polled
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices).length() == 0);
	}

// Start polling for several devices (test the ServerRestart command)

	void test_polling_for_several_devices_after_a_ServerRestart(void)
	{
		DeviceData din, dout;
		DevVarLongStringArray attr_poll, cmd_poll;
		const DevVarStringArray *status_arr, *polled_devices;
		string status[2], status_ref[2];

		cout << "---> Poll period 1: " << device1->get_attribute_poll_period("Double_attr") << endl;
		// start polling an attribute for device1
		attr_poll.lvalue.length(1);
		attr_poll.lvalue[0] = 200;
		attr_poll.svalue.length(3);
		attr_poll.svalue[0] = device1_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "Double_attr";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_double_attr_polling_restored = false;
		cout << "---> Poll period 2: " << device1->get_attribute_poll_period("Double_attr") << endl;

		// start polling a command for device1
		cmd_poll.lvalue.length(1);
		cmd_poll.lvalue[0] = 500;
		cmd_poll.svalue.length(3);
		cmd_poll.svalue[0] = device1_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOStr1";
		din << cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_IOStr1_polling_restored = false;
		cout << "---> Poll period 3: " << device1->get_attribute_poll_period("Double_attr") << endl;

		// start polling an attribute for device2
		attr_poll.lvalue[0] = 200;
		attr_poll.svalue[0] = device2_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "Double_attr";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev2_double_attr_polling_restored = false;
		cout << "---> Poll period 4: " << device1->get_attribute_poll_period("Double_attr") << endl;

		// start polling a command for device2
		cmd_poll.lvalue[0] = 500;
		cmd_poll.svalue[0] = device2_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOStr1";
		din << cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev2_IOStr1_polling_restored = false;
		cout << "---> Poll period 5: " << device1->get_attribute_poll_period("Double_attr") << endl;

		// check if the devices are polled
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices)[0].in() == device1_name);
		TS_ASSERT((*polled_devices)[1].in() == device2_name);
		cout << "---> Poll period 6: " << device1->get_attribute_poll_period("Double_attr") << endl;

		Tango_sleep(3);

		// get polling status for the device1
		status_ref[0] = "Polled command name = IOStr1\nPolling period (mS) = 500\nPolling ring buffer depth = 10";
		status_ref[1] = "Polled attribute name = Double_attr\nPolling period (mS) = 200\nPolling ring buffer depth = 10";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared
		status[0] = string((*status_arr)[0].in()).substr(0,status_ref[0].length());
		TS_ASSERT(status[0] == status_ref[0]);
		status[1] = string((*status_arr)[1].in()).substr(0,status_ref[1].length());
		TS_ASSERT(status[1] == status_ref[1]);
		cout << "---> Poll period 7: " << device1->get_attribute_poll_period("Double_attr") << endl;

		// get polling status for the device2
		din << device2_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared
		status[0] = string((*status_arr)[0].in()).substr(0,status_ref[0].length());
		TS_ASSERT(status[0] == status_ref[0]);
		status[1] = string((*status_arr)[1].in()).substr(0,status_ref[1].length());
		TS_ASSERT(status[1] == status_ref[1]);
		cout << "---> Poll period 8: " << device1->get_attribute_poll_period("Double_attr") << endl;



	// tmp update object polling
	cmd_poll.lvalue[0] = 300;
	cmd_poll.svalue[0] = device1_name.c_str();
	cmd_poll.svalue[1] = "attribute";
	cmd_poll.svalue[2] = "Double_attr";
	din << cmd_poll;
	TS_ASSERT_THROWS_NOTHING(dserver->command_inout("UpdObjPollingPeriod", din));
	dev1_double_attr_polling_restored = false;
	cout << "---> Poll period 8.1: " << device1->get_attribute_poll_period("Double_attr") << endl;

		// restart server
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RestartServer"));
		Tango_sleep(5);
		cout << "---> Poll period 9: " << device1->get_attribute_poll_period("Double_attr") << endl;

		// check if the devices are still polled
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices)[0].in() == device1_name);
		TS_ASSERT((*polled_devices)[1].in() == device2_name);
		cout << "---> Poll period 10: " << device1->get_attribute_poll_period("Double_attr") << endl;

		Tango_sleep(3);

		// get polling status for the device1
		status_ref[0] = "Polled command name = IOStr1\nPolling period (mS) = 500\nPolling ring buffer depth = 10";
		status_ref[1] = "Polled attribute name = Double_attr\nPolling period (mS) = 200\nPolling ring buffer depth = 10";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared


		status[0] = string((*status_arr)[0].in()).substr(0,status_ref[0].length());
		TS_ASSERT(status[0] == status_ref[0]);
		status[1] = string((*status_arr)[1].in()).substr(0,status_ref[1].length());

		cout << "---> Poll period 11: " << device1->get_attribute_poll_period("Double_attr") << endl;

//		cout << endl;
//		cout << "~~~~~ (*status_arr)[0].in() ~~~~~" << endl;
//		cout << (*status_arr)[0].in() << endl;
//		cout << "~~~~~ (*status_arr)[1].in() ~~~~~" << endl;
//		cout << (*status_arr)[1].in() << endl;
//		cout << "~~~~~ status[0] ~~~~~" << endl;
//		cout << status[0] << endl;
//		cout << "~~~~~ status_ref[0] ~~~~~" << endl;
//		cout << status_ref[0] << endl;
//		cout << "~~~~~ status[1] ~~~~~" << endl;
//		cout << status[1] << endl;
//		cout << "~~~~~ status_ref[1] ~~~~~" << endl;
//		cout << status_ref[1] << endl;
//		cout << "~~~~~ end of status ~~~~~" << endl << endl;


		TS_ASSERT(status[1] == status_ref[1]);

		// get polling status for the device2
		din << device2_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other 3 lines could also be compared
		status[0] = string((*status_arr)[0].in()).substr(0,status_ref[0].length());
		TS_ASSERT(status[0] == status_ref[0]);
		status[1] = string((*status_arr)[1].in()).substr(0,status_ref[1].length());
		TS_ASSERT(status[1] == status_ref[1]);

		DevVarStringArray rem_attr_poll, rem_cmd_poll;

		// stop attribute polling for device1
		rem_attr_poll.length(3);
		rem_attr_poll[0] = device1_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "Double_attr";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_double_attr_polling_restored = true;

		// stop command polling for device1
		rem_cmd_poll.length(3);
		rem_cmd_poll[0] = device1_name.c_str();
		rem_cmd_poll[1] = "command";
		rem_cmd_poll[2] = "IOStr1";
		din << rem_cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_IOStr1_polling_restored = true;

		// stop attribute polling for device2
		rem_attr_poll[0] = device2_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "Double_attr";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev2_double_attr_polling_restored = true;

		// stop command polling for device2
		rem_cmd_poll[0] = device2_name.c_str();
		rem_cmd_poll[1] = "command";
		rem_cmd_poll[2] = "IOStr1";
		din << rem_cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev2_IOStr1_polling_restored = true;

		// confirm that the devices are not polled
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices).length() == 0);
	}

// Start a command externally triggered

	void test_start_a_command_externally_triggered(void)
	{
		DeviceData din, dout;
		const DevVarStringArray *status_arr;
		DevVarLongStringArray cmd_poll;
		DevVarStringArray rem_cmd_poll;
		string status, status_ref;

		// start externally triggered command
		cmd_poll.lvalue.length(1);
		cmd_poll.lvalue[0] = 0;
		cmd_poll.svalue.length(3);
		cmd_poll.svalue[0] = device1_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOStr1";
		din << cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_IOStr1_polling_restored = false;

		// check if the device is polled
		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices)[0].in() == device1_name);

		// get polling status for the device
		status_ref = "Polled command name = IOStr1\nPolling externally triggered\nPolling ring buffer depth = 10\nNo data recorded yet";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		status = (*status_arr)[0].in();
		TS_ASSERT(status == status_ref);

		// trigger polling for a non-existing command
		string mock_command = "toto";
		din << mock_command;
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOTrigPoll", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_PollObjNotFound"
						&& e.errors[0].severity == Tango::ERR));

		// trigger polling for a non-polled command
		string non_polled_command = "IOPollStr1";
		din << non_polled_command;
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOTrigPoll", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_PollObjNotFound"
						&& e.errors[0].severity == Tango::ERR));

		// add polling for a non externally triggered command
		cmd_poll.lvalue[0] = 500;
		cmd_poll.svalue.length(3);
		cmd_poll.svalue[0] = device1_name.c_str();
		cmd_poll.svalue[1] = "command";
		cmd_poll.svalue[2] = "IOPollStr1";
		din << cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("AddObjPolling", din));
		dev1_IOPollStr1_polling_restored = false;

		// trigger polling for the not externally triggered command
		string non_ext_trig_command = "IOPollStr1";
		din << non_ext_trig_command;
		TS_ASSERT_THROWS_ASSERT(device1->command_inout("IOTrigPoll", din), Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == "API_NotSupported"
						&& e.errors[0].severity == Tango::ERR));

		// stop polling for the non externally triggered command
		rem_cmd_poll.length(3);
		rem_cmd_poll[0] = device1_name.c_str();
		rem_cmd_poll[1] = "command";
		rem_cmd_poll[2] = "IOPollStr1";
		din << rem_cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_IOPollStr1_polling_restored = true;

		// trigger polling for the externally triggered command twice
		string ext_trig_command = "IOStr1";
		din << ext_trig_command;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOTrigPoll", din));
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOTrigPoll", din));

		// get polling status
		status_ref = "Polled command name = IOStr1\nPolling externally triggered\nPolling ring buffer depth = 10";
		din << device1_name;
		TS_ASSERT_THROWS_NOTHING(dout = dserver->command_inout("DevPollStatus", din));
		dout >> status_arr;
		// extract the first 3 lines of the status and compare with the reference string
		// TODO: although device/time dependent, the other line could also be compared
		status = string((*status_arr)[0].in()).substr(0,status_ref.length());
		TS_ASSERT(status == status_ref);

		// stop polling for the externally triggered command
		rem_cmd_poll[0] = device1_name.c_str();
		rem_cmd_poll[1] = "command";
		rem_cmd_poll[2] = "IOStr1";
		din << rem_cmd_poll;
		TS_ASSERT_THROWS_NOTHING(dserver->command_inout("RemObjPolling", din));
		dev1_IOStr1_polling_restored = true;
	}
};
#undef cout
#endif // PollAdminTestSuite_h
