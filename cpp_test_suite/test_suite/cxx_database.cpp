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
	Database *db;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		device_name = CxxTest::TangoPrinter::get_uarg("device");

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


// Miscellaneous inserters and extracters for DeviceData object

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

};
#undef cout
#endif // DatabaseTestSuite_h
