#ifndef DynCmdSuite_h
#define DynCmdSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME DynCmdTestSuite

class DynCmdTestSuite: public CxxTest::TestSuite
{
protected:

	DeviceProxy 			*device1,*device2;
	string 					device1_name;
	string					device2_name;

public:
	SUITE_NAME()
	{
//
// Arguments check -------------------------------------------------
//

		string full_ds_name;

		// user arguments, obtained from the command line sequentially
		device1_name = CxxTest::TangoPrinter::get_param("device1");
		device2_name = CxxTest::TangoPrinter::get_param("device2");

		// always add this line, otherwise arguments will not be parsed correctly
		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

		try
		{
			device1 = new DeviceProxy(device1_name);
			device1->ping();

			device2 = new DeviceProxy(device2_name);
			device2->ping();
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
		delete device2;
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

// Test dynamic command installed at class level

	void test_dynamic_command_at_class_level()
	{
		// Add a dynamic command

		Tango::DevLong device_level = 0;
		DeviceData din,dout;
		din << device_level;

		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOAddCommand",din));

		// Execute command

		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("Added_cmd"));
		float receiv;
		dout >> receiv;
		TS_ASSERT(receiv == 4.0);

		// Command in command list?

		CommandInfo ci = device1->command_query("Added_cmd");
		TS_ASSERT(ci.cmd_name == "Added_cmd");
		TS_ASSERT(ci.in_type == DEV_VOID);
		TS_ASSERT(ci.out_type == DEV_FLOAT);

		CommandInfoList *cil;
		TS_ASSERT_THROWS_NOTHING(cil = device1->command_list_query());
		bool found = false;
		size_t nb_cmd = cil->size();
		for (size_t loop = 0;loop < nb_cmd;loop++)
		{
			if ((*cil)[loop].cmd_name == "Added_cmd")
				found = true;
		}

		TS_ASSERT(found == true);

		// Command also in command list for device 2

		CommandInfoList *cil2;
		TS_ASSERT_THROWS_NOTHING(cil2 = device2->command_list_query());
		found = false;
		nb_cmd = cil2->size();
		for (size_t loop = 0;loop < nb_cmd;loop++)
		{
			if ((*cil2)[loop].cmd_name == "Added_cmd")
				found = true;
		}

		TS_ASSERT(found == true);

		// Remove command

		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IORemoveCommand"));
	}

// Test dynamic command installed at device level

	void test_dynamic_command_at_device_level()
	{
		// Add a dynamic command

		Tango::DevLong device_level = 1;
		DeviceData din,dout;
		din << device_level;

		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOAddCommand",din));

		// Execute command

		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("Added_cmd"));
		float receiv;
		dout >> receiv;
		TS_ASSERT(receiv == 4.0);

		// Command in command list?

		CommandInfo ci = device1->command_query("Added_cmd");
		TS_ASSERT(ci.cmd_name == "Added_cmd");
		TS_ASSERT(ci.in_type == DEV_VOID);
		TS_ASSERT(ci.out_type == DEV_FLOAT);

		CommandInfoList *cil;
		TS_ASSERT_THROWS_NOTHING(cil = device1->command_list_query());
		bool found = false;
		size_t nb_cmd = cil->size();
		for (size_t loop = 0;loop < nb_cmd;loop++)
		{
			if ((*cil)[loop].cmd_name == "Added_cmd")
				found = true;
		}

		TS_ASSERT(found == true);

		// Command should not be in command list for device 2

		CommandInfoList *cil2;
		TS_ASSERT_THROWS_NOTHING(cil2 = device2->command_list_query());
		found = false;
		nb_cmd = cil2->size();
		for (size_t loop = 0;loop < nb_cmd;loop++)
		{
			if ((*cil2)[loop].cmd_name == "Added_cmd")
				found = true;
		}

		TS_ASSERT(found == false);

		// Remove command

		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IORemoveCommand"));
	}
};

#undef cout
#endif // DynCmdTestSuite_h
