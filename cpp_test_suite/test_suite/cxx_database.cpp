#ifndef DatabaseTestSuite_h
#define DatabaseTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>
#include <ctime>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME DatabaseTestSuite

class DatabaseTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device;
	string device_name;
	string dev_alias;
	string att_alias;
	Database *db;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		device_name = CxxTest::TangoPrinter::get_uarg("device");

		dev_alias = CxxTest::TangoPrinter::get_param("devicealias");
		att_alias = CxxTest::TangoPrinter::get_param("attributealias");

		CxxTest::TangoPrinter::validate_args();


// Initialization --------------------------------------------------


		try
		{
			device = new DeviceProxy(device_name);
			db = new Database();
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

		delete device;
		delete db;
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


// The get_device_info call

	void test_get_device_info()
	{
		DbDevFullInfo dbfi = db->get_device_info(device_name);

		DeviceData dd,dd_in;
		dd_in << device_name;
		dd = db->command_inout("DbImportDevice",dd_in);

		const DevVarLongStringArray *dvlsa;
		dd >> dvlsa;

		TS_ASSERT(dbfi.name == device_name);
		TS_ASSERT(dbfi.ds_full_name == string(dvlsa->svalue[3]));
		TS_ASSERT(dbfi.exported == 1);
		TS_ASSERT(dbfi.class_name == string(dvlsa->svalue[5]));
		TS_ASSERT(dbfi.host == string(dvlsa->svalue[4]));
		TS_ASSERT(dbfi.pid == dvlsa->lvalue[1]);
	}

// The device alias

	void test_device_alias_calls()
	{
		string d_alias,d_name;

		db->get_alias_from_device(device_name,d_alias);
		TS_ASSERT(d_alias == dev_alias);

		db->get_device_from_alias(dev_alias,d_name);
		TS_ASSERT(d_name == device_name);
	}

// The attribute alias

	void test_attribute_alias_calls()
	{
		string a_alias,a_name;

		string full_att_name(device_name);
		full_att_name = full_att_name + "/Short_attr";

		db->get_alias_from_attribute(full_att_name,a_alias);
		TS_ASSERT(a_alias == att_alias);

		db->get_attribute_from_alias(att_alias,a_name);
		transform(a_name.begin(),a_name.end(),a_name.begin(),::tolower);
		transform(full_att_name.begin(),full_att_name.end(),full_att_name.begin(),::tolower);
		TS_ASSERT(a_name == full_att_name);
	}
};
#undef cout
#endif // DatabaseTestSuite_h
