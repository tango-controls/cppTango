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
	DeviceProxy *device1;
	string device1_name;
	string dev_alias;
	string att_alias;
	Database *db;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		device1_name = CxxTest::TangoPrinter::get_param("device1");
		dev_alias = CxxTest::TangoPrinter::get_param("devicealias");
		att_alias = CxxTest::TangoPrinter::get_param("attributealias");

		CxxTest::TangoPrinter::validate_args();


// Initialization --------------------------------------------------


		try
		{
			device1 = new DeviceProxy(device1_name);
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

		delete device1;
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
		DbDevFullInfo dbfi = db->get_device_info(device1_name);

		DeviceData dd,dd_in;
		dd_in << device1_name;
		dd = db->command_inout("DbImportDevice",dd_in);

		const DevVarLongStringArray *dvlsa;
		dd >> dvlsa;

		TS_ASSERT(dbfi.name == device1_name);
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

		db->get_alias_from_device(device1_name,d_alias);
		TS_ASSERT(d_alias == dev_alias);

		db->get_device_from_alias(dev_alias,d_name);
		TS_ASSERT(d_name == device1_name);
	}

// The attribute alias

	void test_attribute_alias_calls()
	{
		string a_alias,a_name;

		string full_att_name(device1_name);
		full_att_name = full_att_name + "/Short_attr";

		db->get_alias_from_attribute(full_att_name,a_alias);
		TS_ASSERT(a_alias == att_alias);

		db->get_attribute_from_alias(att_alias,a_name);
		transform(a_name.begin(),a_name.end(),a_name.begin(),::tolower);
		transform(full_att_name.begin(),full_att_name.end(),full_att_name.begin(),::tolower);
		TS_ASSERT(a_name == full_att_name);
	}

// Pipe class oriented calls

	void test_class_pipe_oriented_calls()
	{
		DbDatum velocity("_tst_pipe"), vel_min("_tst_pipe_propA"), vel_max("_tst_pipe_propB");
		DbData db_data;

		velocity << (short)2;
		vel_min << 3.0;
		vel_max << 33.0;

		db_data.push_back(velocity);
		db_data.push_back(vel_min);
		db_data.push_back(vel_max);

		TS_ASSERT_THROWS_NOTHING(db->put_class_pipe_property("MyStepperMotor",db_data));

		DbData get_db_data;
		get_db_data.push_back(DbDatum("_tst_pipe"));

		TS_ASSERT_THROWS_NOTHING(db->get_class_pipe_property("MyStepperMotor",get_db_data));

		int nb_prop;
		get_db_data[0] >> nb_prop;
		TS_ASSERT(nb_prop == 2);

		float propA, propB;
		string prop1_name,prop2_name;
		TS_ASSERT(get_db_data[1].name == "_tst_pipe_propA");
		TS_ASSERT(get_db_data[2].name == "_tst_pipe_propB");

		get_db_data[1] >> propA;
		get_db_data[2] >> propB;
		TS_ASSERT(propA == 3.0);
		TS_ASSERT(propB == 33.0);

		DbDatum db_datum;
 		TS_ASSERT_THROWS_NOTHING(db_datum = db->get_class_pipe_list("MyStepperMotor","*"));

		vector<string> pipe_list;
		db_datum >> pipe_list;

		TS_ASSERT(pipe_list.size() == 1);
		TS_ASSERT(pipe_list[0] == "_tst_pipe");


		DbData del_db_data;
		del_db_data.push_back(DbDatum("_tst_pipe"));
		del_db_data.push_back(DbDatum("_tst_pipe_propA"));
		del_db_data.push_back(DbDatum("_tst_pipe_propB"));

		TS_ASSERT_THROWS_NOTHING(db->delete_class_pipe_property("MyStepperMotor", del_db_data));
	}

// Pipe device oriented calls

	void test_device_pipe_oriented_calls()
	{
		DbDatum velocity("_tst_pipe"), vel_min("_tst_pipe_propA"), vel_max("_tst_pipe_propB");
		DbData db_data;

		velocity << (short)2;
		vel_min << 3.0;
		vel_max << 33.0;

		db_data.push_back(velocity);
		db_data.push_back(vel_min);
		db_data.push_back(vel_max);

		TS_ASSERT_THROWS_NOTHING(db->put_device_pipe_property("a/b/c",db_data));

		DbData get_db_data;
		get_db_data.push_back(DbDatum("_tst_pipe"));

		TS_ASSERT_THROWS_NOTHING(db->get_device_pipe_property("a/b/c",get_db_data));

		int nb_prop;
		get_db_data[0] >> nb_prop;
		TS_ASSERT(nb_prop == 2);

		float propA, propB;
		string prop1_name,prop2_name;
		TS_ASSERT(get_db_data[1].name == "_tst_pipe_propA");
		TS_ASSERT(get_db_data[2].name == "_tst_pipe_propB");

		get_db_data[1] >> propA;
		get_db_data[2] >> propB;
		TS_ASSERT(propA == 3.0);
		TS_ASSERT(propB == 33.0);

		vector<string> pipe_list;
 		TS_ASSERT_THROWS_NOTHING(db->get_device_pipe_list("a/b/c",pipe_list));

		TS_ASSERT(pipe_list.size() == 1);
		TS_ASSERT(pipe_list[0] == "_tst_pipe");


		DbData del_db_data;
		del_db_data.push_back(DbDatum("_tst_pipe"));
		del_db_data.push_back(DbDatum("_tst_pipe_propA"));
		del_db_data.push_back(DbDatum("_tst_pipe_propB"));

		TS_ASSERT_THROWS_NOTHING(db->delete_device_pipe_property("a/b/c", del_db_data));

		TS_ASSERT_THROWS_NOTHING(db->put_device_pipe_property("a/b/c",db_data));

		DbData del_all;
		del_all.push_back(DbDatum("_tst_pipe"));
		TS_ASSERT_THROWS_NOTHING(db->delete_all_device_pipe_property("a/b/c",del_all));
	}
};
#undef cout
#endif // DatabaseTestSuite_h
