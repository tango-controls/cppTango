#ifndef CmdQueryTestSuite_h
#define CmdQueryTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME CmdQueryTestSuite

class CmdQueryTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;
	CommandInfoList cmd_inf_list;

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

//// Test AddLoggingTarget command information
//
//	void test_command_AddLoggingTarget_information(void) {
//		CommandInfo cmd_inf = dserver->command_query("AddLoggingTarget");
//		cout << "name: " << cmd_inf.cmd_name << " tag: " << cmd_inf.cmd_tag << " displ_level: " << cmd_inf.disp_level << " in_type: " << cmd_inf.in_type << " type_desc: " << cmd_inf.in_type_desc << " out_type: " << cmd_inf.out_type << " out_type_desc: " << cmd_inf.out_type_desc << endl;
//	}

// Test command list query

	void test_command_list_query(void)
	{
		TS_ASSERT_THROWS_NOTHING(cmd_inf_list = *dserver->command_list_query());
		TS_ASSERT(cmd_inf_list.size() == 32);
	}

// Test Status command

	void test_command_Status(void)
	{
		CommandInfo cmd_inf;
		TS_ASSERT_THROWS_NOTHING(cmd_inf = device1->command_query("Status"));
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"Status");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_STRING);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Device status");
	}

// Test fake command

	void test_fake_command(void)
	{
		TS_ASSERT_THROWS_ASSERT(device1->command_query("DevToto"),Tango::DevFailed &e,
				TS_ASSERT(string(e.errors[0].reason.in()) == API_CommandNotFound
						&& e.errors[0].severity == Tango::ERR));
	}

// Test AddLoggingTarget command_list_query

	void test_command_list_query_AddLoggingTarget(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("AddLoggingTarget");
		CommandInfo cmd_inf = cmd_inf_list[0];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"AddLoggingTarget");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Str[i]=Device-name. Str[i+1]=Target-type::Target-name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test AddObjPolling command_list_query

	void test_command_list_query_AddObjPolling(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("AddObjPolling");
		CommandInfo cmd_inf = cmd_inf_list[1];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"AddObjPolling");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_LONGSTRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Lg[0]=Upd period. Str[0]=Device name. Str[1]=Object type. Str[2]=Object name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test DevLockStatus command_list_query

	void test_command_list_query_DevLockStatus(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("DevLockStatus");
		CommandInfo cmd_inf = cmd_inf_list[2];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"DevLockStatus");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_STRING);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEVVAR_LONGSTRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Device name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Device locking status");
	}

// Test DevPollStatus command_list_query

	void test_command_list_query_DevPollStatus(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("DevPollStatus");
		CommandInfo cmd_inf = cmd_inf_list[3];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"DevPollStatus");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_STRING);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Device name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Device polling status");
	}

// Test DevRestart command_list_query

	void test_command_list_query_DevRestart(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("DevRestart");
		CommandInfo cmd_inf = cmd_inf_list[4];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"DevRestart");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_STRING);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Device name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test EventConfirmSubscription command_list_query

	void test_command_list_query_EventConfirmSubscriptionChange(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("EventConfirmSubscription");
		CommandInfo cmd_inf = cmd_inf_list[5];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"EventConfirmSubscription");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Str[0] = dev1 name, Str[1] = att1 name, Str[2] = event name, Str[3] = dev2 name, Str[4] = att2 name, Str[5] = event name,...");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test EventSubscriptionChange command_list_query

	void test_command_list_query_EventSubscriptionChange(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("EventSubscriptionChange");
		CommandInfo cmd_inf = cmd_inf_list[6];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"EventSubscriptionChange");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_LONG);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Event consumer wants to subscribe to");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Tango lib release");
	}

// Test GetLoggingLevel command_list_query

	void test_command_list_query_GetLoggingLevel(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("GetLoggingLevel");
		CommandInfo cmd_inf = cmd_inf_list[7];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"GetLoggingLevel");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEVVAR_LONGSTRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Device list");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Lg[i]=Logging Level. Str[i]=Device name.");
	}

// Test GetLoggingTarget command_list_query

	void test_command_list_query_GetLoggingTarget(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("GetLoggingTarget");
		CommandInfo cmd_inf = cmd_inf_list[8];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"GetLoggingTarget");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_STRING);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Device name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Logging target list");
	}

// Test Init command_list_query

	void test_command_list_query_Init(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("Init");
		CommandInfo cmd_inf = cmd_inf_list[9];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"Init");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test Kill command_list_query

	void test_command_list_query_Kill(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("Kill");
		CommandInfo cmd_inf = cmd_inf_list[10];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"Kill");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test LockDevice command_list_query

	void test_command_list_query_LockDevice(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("LockDevice");
		CommandInfo cmd_inf = cmd_inf_list[11];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"LockDevice");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_LONGSTRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Str[0] = Device name. Lg[0] = Lock validity");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test PolledDevice command_list_query

	void test_command_list_query_PolledDevice(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("PolledDevice");
		CommandInfo cmd_inf = cmd_inf_list[12];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"PolledDevice");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Polled device name list");
	}

// Test QueryClass command_list_query

	void test_command_list_query_QueryClass(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("QueryClass");
		CommandInfo cmd_inf = cmd_inf_list[13];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"QueryClass");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Device server class(es) list");
	}

// Test QueryDevice command_list_query

	void test_command_list_query_QueryDevice(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("QueryDevice");
		CommandInfo cmd_inf = cmd_inf_list[14];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"QueryDevice");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Device server device(s) list");
	}

// Test QuerySubDevice command_list_query

	void test_command_list_query_QuerySubDevice(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("QuerySubDevice");
		CommandInfo cmd_inf = cmd_inf_list[15];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"QuerySubDevice");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Device server sub device(s) list");
	}

// Test QueryWizardClassProperty command_list_query

	void test_command_list_query_QueryWizardClassProperty(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("QueryWizardClassProperty");
		CommandInfo cmd_inf = cmd_inf_list[16];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"QueryWizardClassProperty");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_STRING);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Class name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Class property list (name - description and default value)");
	}

// Test QueryWizardDevProperty command_list_query

	void test_command_list_query_QueryWizardDevProperty(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("QueryWizardDevProperty");
		CommandInfo cmd_inf = cmd_inf_list[17];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"QueryWizardDevProperty");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_STRING);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Class name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Device property list (name - description and default value)");
	}

// Test ReLockDevices command_list_query

	void test_command_list_query_ReLockDevices(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("ReLockDevices");
		CommandInfo cmd_inf = cmd_inf_list[18];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"ReLockDevices");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Device(s) name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test RemObjPolling command_list_query

	void test_command_list_query_RemObjPolling(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("RemObjPolling");
		CommandInfo cmd_inf = cmd_inf_list[19];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"RemObjPolling");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Str[0]=Device name. Str[1]=Object type. Str[2]=Object name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test RemoveLoggingTarget command_list_query

	void test_command_list_query_RemoveLoggingTarget(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("RemoveLoggingTarget");
		CommandInfo cmd_inf = cmd_inf_list[20];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"RemoveLoggingTarget");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_STRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Str[i]=Device-name. Str[i+1]=Target-type::Target-name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test RestartServer command_list_query

	void test_command_list_query_RestartServer(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("RestartServer");
		CommandInfo cmd_inf = cmd_inf_list[21];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"RestartServer");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test SetLoggingLevel command_list_query

	void test_command_list_query_SetLoggingLevel(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("SetLoggingLevel");
		CommandInfo cmd_inf = cmd_inf_list[22];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"SetLoggingLevel");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_LONGSTRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Lg[i]=Logging Level. Str[i]=Device name.");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test StartLogging command_list_query

	void test_command_list_query_StartLogging(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("StartLogging");
		CommandInfo cmd_inf = cmd_inf_list[23];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"StartLogging");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test StartPolling command_list_query

	void test_command_list_query_StartPolling(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("StartPolling");
		CommandInfo cmd_inf = cmd_inf_list[24];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"StartPolling");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test State command_list_query

	void test_command_list_query_State(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("State");
		CommandInfo cmd_inf = cmd_inf_list[25];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"State");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_STATE);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Device state");
	}

// Test Status command_list_query

	void test_command_list_query_Status(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("Status");
		CommandInfo cmd_inf = cmd_inf_list[26];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"Status");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_STRING);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Device status");
	}

// Test StopLogging command_list_query

	void test_command_list_query_StopLogging(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("StopLogging");
		CommandInfo cmd_inf = cmd_inf_list[27];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"StopLogging");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test StopPolling command_list_query

	void test_command_list_query_StopPolling(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("StopPolling");
		CommandInfo cmd_inf = cmd_inf_list[28];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"StopPolling");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Uninitialised");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test UnLockDevice command_list_query

	void test_command_list_query_UnLockDevice(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("UnLockDevice");
		CommandInfo cmd_inf = cmd_inf_list[29];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"UnLockDevice");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_LONGSTRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_LONG);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Str[x] = Device name(s). Lg[0] = Force flag");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Device global lock counter");
	}

// Test UpdObjPollingPeriod command_list_query

	void test_command_list_query_list_query_UpdObjPollingPeriod(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("UpdObjPollingPeriod");
		CommandInfo cmd_inf = cmd_inf_list[30];
		TS_ASSERT_EQUALS(cmd_inf.cmd_name,"UpdObjPollingPeriod");
		TS_ASSERT_EQUALS(cmd_inf.in_type,Tango::DEVVAR_LONGSTRINGARRAY);
		TS_ASSERT_EQUALS(cmd_inf.out_type,Tango::DEV_VOID);
		TS_ASSERT_EQUALS(cmd_inf.in_type_desc,"Lg[0]=Upd period. Str[0]=Device name. Str[1]=Object type. Str[2]=Object name");
		TS_ASSERT_EQUALS(cmd_inf.out_type_desc,"Uninitialised");
	}

// Test ZmqEventSubscriptionChange command_list_query

	void test_command_list_query_ZMQEventSubscriptionChange(void)
	{
//		CommandInfo cmd_inf = dserver->command_query("ZmqEventSubscriptionChange");
        CommandInfo cmd_inf = cmd_inf_list[31];
        TS_ASSERT_EQUALS(cmd_inf.cmd_name, "ZmqEventSubscriptionChange");
        TS_ASSERT_EQUALS(cmd_inf.in_type, Tango::DEVVAR_STRINGARRAY);
        TS_ASSERT_EQUALS(cmd_inf.out_type, Tango::DEVVAR_LONGSTRINGARRAY);
        TS_ASSERT_EQUALS(cmd_inf.in_type_desc, "Event consumer wants to subscribe to.\n"
            "device name, attribute/pipe name, action (\"subscribe\"), event name, <Tango client IDL version>\"\n"
            "event name can take the following values:\n"
            "    \"change\",\n"
            "    \"quality\",\n"
            "    \"periodic\",\n"
            "    \"archive\",\n"
            "    \"user_event\",\n"
            "    \"attr_conf\",\n"
            "    \"data_ready\",\n"
            "    \"intr_change\",\n"
            "    \"pipe\"\n"
            "\"info\" can also be used as single parameter to retrieve information about the heartbeat and event pub endpoints.");
        TS_ASSERT_EQUALS(cmd_inf.out_type_desc, "Str[0] = Heartbeat pub endpoint - Str[1] = Event pub endpoint\n"
            "...\n"
            "Str[n] = Alternate Heartbeat pub endpoint - Str[n+1] = Alternate Event pub endpoint\n"
            "Str[n+1] = event name used by this server as zmq topic to send events\n"
            "Str[n+2] = channel name used by this server to send heartbeat events\n"
            "Lg[0] = Tango lib release - Lg[1] = Device IDL release\n"
            "Lg[2] = Subscriber HWM - Lg[3] = Multicast rate\n"
            "Lg[4] = Multicast IVL - Lg[5] = ZMQ release");
	}

};
#undef cout
#endif // CmdQueryTestSuite_h
