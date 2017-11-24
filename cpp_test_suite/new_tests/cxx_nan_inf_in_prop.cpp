#ifndef NanInfInPropSuite_h
#define NanInfInPropSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME NanInfInPropSuite

class NanInfInPropSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string device1_name;

		// predefined mandatory parameters
		device1_name = CxxTest::TangoPrinter::get_param("device1");

		// always add this line, otherwise arguments will not be parsed correctly
		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

		try
		{
			device1 = new DeviceProxy(device1_name);
			device1->ping();
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

		// clean up in case test suite terminates before my_restore_point is restored to defaults
		if(CxxTest::TangoPrinter::is_restore_set("NanInArrayProp_restore_point"))
		{
			try
			{
                string prop_name = "NaNInfArrayProperty";
				device1->delete_property(prop_name);
			}
			catch(DevFailed &e)
			{
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
		}
		if(CxxTest::TangoPrinter::is_restore_set("NanInScalarProp_restore_point"))
		{
			try
			{
                string prop_name = "NaNInfScalarProperty";
				device1->delete_property(prop_name);
			}
			catch(DevFailed &e)
			{
				cout << endl << "Exception in suite tearDown():" << endl;
				Except::print_exception(e);
			}
		}

		// delete dynamically allocated objects
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

	void set_property(const char * prop_name, const char * str_prop_val)
	{
		Tango::DbData db_data_w;
		Tango::DbDatum db_datum(prop_name);
		TS_ASSERT_THROWS_NOTHING(db_datum << str_prop_val);
		db_data_w.push_back(db_datum);
		TS_ASSERT_THROWS_NOTHING(device1->put_property(db_data_w));
	}

    template<typename T>
    void write_then_read_array_property_containing_nan_and_inf()
    {
        // Write NaNInfArrayProperty device property
        Tango::DbData db_data_w;
        vector<T> array_val;
        array_val.push_back(T(1.2));
        if(std::numeric_limits<T>::has_quiet_NaN)
        {
	        array_val.push_back(std::numeric_limits<T>::quiet_NaN());
	        array_val.push_back(-std::numeric_limits<T>::quiet_NaN());
        }
        array_val.push_back(T(3.4));
        if(std::numeric_limits<T>::has_infinity)
        {
            array_val.push_back(std::numeric_limits<T>::infinity());
            array_val.push_back(-std::numeric_limits<T>::infinity());
        }
        array_val.push_back(T(-9.78));

        Tango::DbDatum db_datum("NaNInfArrayProperty");
        TS_ASSERT_THROWS_NOTHING(db_datum << array_val);
        db_data_w.push_back(db_datum);
        TS_ASSERT_THROWS_NOTHING(device1->put_property(db_data_w));

        // Read NanInfArrayProperty device property
        Tango::DbData db_data_r;
        string prop_name = "NaNInfArrayProperty";
        TS_ASSERT_THROWS_NOTHING(device1->get_property(prop_name,db_data_r));

        // Extract as T array:
        size_t expected_size = 3;
        if(std::numeric_limits<T>::has_quiet_NaN)
            expected_size += 2;
        if(std::numeric_limits<T>::has_infinity)
            expected_size += 2;
        vector<T> read_array_val;
        size_t index = 0;
        TS_ASSERT(db_data_r[0] >> read_array_val);
        TS_ASSERT_EQUALS(read_array_val.size(),expected_size);
        TS_ASSERT_EQUALS(read_array_val[index],T(1.2));
        index++;
        if(std::numeric_limits<T>::has_quiet_NaN)
        {
            TS_ASSERT(std::isnan(read_array_val[index]));
	        index++;
	        // -NaN is extracted as NaN
	        TS_ASSERT(std::isnan(read_array_val[index]));
            index++;
        }
        TS_ASSERT_EQUALS(read_array_val[index],T(3.4));
        index++;
        if(std::numeric_limits<T>::has_infinity)
        {
            TS_ASSERT(std::isinf(read_array_val[index]));
            TS_ASSERT_LESS_THAN(0,read_array_val[index]);
            index++;
            TS_ASSERT(std::isinf(read_array_val[index]));
            TS_ASSERT_LESS_THAN(read_array_val[index],0);
            index++;
        }
        TS_ASSERT_EQUALS(read_array_val[index],T(-9.78));

        // Extract as long array:
        if ((std::numeric_limits<T>::has_infinity) || (std::numeric_limits<T>::has_quiet_NaN))
        {
            vector <Tango::DevLong> long_array_val;
            TS_ASSERT(!(db_data_r[0] >> long_array_val));
        }
    }

	void NaN_in_scalar_property(const string & prop_value)
	{
		//
		// if your code modifies the default (device) configuration, append the following line straight after
		CxxTest::TangoPrinter::restore_set("NanInfScalarProp_restore_point");

		// Write NaNInfArrayProperty device property
		Tango::DbData db_data_r;

		// Write NaNInfScalarProperty device property
		set_property("NaNInfScalarProperty", prop_value.c_str());

		// Read NaNInfScalarProperty device property
		string prop_name = "NaNInfScalarProperty";
		TS_ASSERT_THROWS_NOTHING(device1->get_property(prop_name,db_data_r));
		// Extract as a string:
		string string_scalar_val;
		TS_ASSERT(db_data_r[0] >> string_scalar_val);
		TS_ASSERT_EQUALS(string_scalar_val,prop_value);

		// Extract as float:
		Tango::DevFloat float_val;
		TS_ASSERT(db_data_r[0] >> float_val);
		TS_ASSERT(std::isnan(float_val));

		// Extract as double:
		Tango::DevDouble double_val;
		TS_ASSERT(db_data_r[0] >> double_val);
		TS_ASSERT(std::isnan(double_val));

		// Extract as short
		Tango::DevShort short_val;
		TS_ASSERT(!(db_data_r[0] >> short_val));

		// Restore the default configuration
		TS_ASSERT_THROWS_NOTHING(device1->delete_property(prop_name));
		// if the test suite fails here, thanks to the restore point, the test suite TearDown method will restore the defaults
		// after you set back the default configuration, append the following line
		CxxTest::TangoPrinter::restore_unset("NanInfScalarProp_restore_point");
	}
//
// Tests -------------------------------------------------------
//

    // test NaN and infinity in array property

	void test_NaN_and_infinity_in_array_property()
	{
		//
		// if your code modifies the default (device) configuration, append the following line straight after
		CxxTest::TangoPrinter::restore_set("NanInArrayProp_restore_point");

		// Write NaNInfArrayProperty device property
		Tango::DbData db_data_w;
		vector<string> str_val;
		str_val.push_back("1.2");
		str_val.push_back("NaN");
		str_val.push_back("3.4");
		str_val.push_back("inf");
		str_val.push_back("nan");
		str_val.push_back("-iNf");
		str_val.push_back("-9.78");
		str_val.push_back("-NaN");
		str_val.push_back("-89101112.1314");

		Tango::DbDatum db_datum("NaNInfArrayProperty");
		TS_ASSERT_THROWS_NOTHING(db_datum << str_val);
		db_data_w.push_back(db_datum);
		TS_ASSERT_THROWS_NOTHING(device1->put_property(db_data_w));

		// Read NanInfArrayProperty device property
		Tango::DbData db_data_r;
        string prop_name = "NaNInfArrayProperty";
		TS_ASSERT_THROWS_NOTHING(device1->get_property(prop_name,db_data_r));

		// Extract as a string array:
		vector<string> string_array_val;
		TS_ASSERT(db_data_r[0] >> string_array_val);
		TS_ASSERT_EQUALS(string_array_val.size(),(size_t) 9);
		TS_ASSERT_EQUALS(string_array_val[0],"1.2");
		TS_ASSERT_EQUALS(string_array_val[1],"NaN");
		TS_ASSERT_EQUALS(string_array_val[2],"3.4");
		TS_ASSERT_EQUALS(string_array_val[3],"inf");
		TS_ASSERT_EQUALS(string_array_val[4],"nan");
		TS_ASSERT_EQUALS(string_array_val[5],"-iNf");
		TS_ASSERT_EQUALS(string_array_val[6],"-9.78");
		TS_ASSERT_EQUALS(string_array_val[7],"-NaN");
		TS_ASSERT_EQUALS(string_array_val[8],"-89101112.1314");

		// Extract as a float array:
		vector<Tango::DevFloat> float_array_val;
		TS_ASSERT(db_data_r[0] >> float_array_val);
		TS_ASSERT_EQUALS(float_array_val.size(),(size_t) 9);
		TS_ASSERT_EQUALS(float_array_val[0],Tango::DevFloat(1.2));
		TS_ASSERT(std::isnan(float_array_val[1]));
		TS_ASSERT_EQUALS(float_array_val[2],Tango::DevFloat(3.4));
		TS_ASSERT(std::isinf(float_array_val[3]));
		TS_ASSERT(std::isnan(float_array_val[4]));
		TS_ASSERT(std::isinf(float_array_val[5]));
		TS_ASSERT_LESS_THAN(float_array_val[5],0);
		TS_ASSERT_EQUALS(float_array_val[6],Tango::DevFloat(-9.78));
		TS_ASSERT(std::isnan(float_array_val[7]));
		TS_ASSERT_EQUALS(float_array_val[8],Tango::DevFloat(-89101112.1314));


		// Extract as a double array:
		vector<Tango::DevDouble> double_array_val;
		TS_ASSERT(db_data_r[0] >> double_array_val);
		TS_ASSERT_EQUALS(double_array_val.size(),(size_t) 9);
		TS_ASSERT_EQUALS(double_array_val[0],Tango::DevDouble(1.2));
		TS_ASSERT(std::isnan(double_array_val[1]));
		TS_ASSERT_EQUALS(double_array_val[2],Tango::DevDouble(3.4));
		TS_ASSERT(std::isinf(double_array_val[3]));
		TS_ASSERT(std::isnan(double_array_val[4]));
		TS_ASSERT(std::isinf(double_array_val[5]));
		TS_ASSERT_LESS_THAN(double_array_val[5],0);
		TS_ASSERT_EQUALS(double_array_val[6],Tango::DevDouble(-9.78));
		TS_ASSERT(std::isnan(double_array_val[7]));
		TS_ASSERT_EQUALS(double_array_val[8],Tango::DevDouble(-89101112.1314));

		// Extract as long array:
		vector<Tango::DevLong> long_array_val;
		TS_ASSERT(! (db_data_r[0] >> long_array_val));

		// Restore the default configuration
		TS_ASSERT_THROWS_NOTHING(device1->delete_property(prop_name));
		// if the test suite fails here, thanks to the restore point, the test suite TearDown method will restore the defaults
		// after you set back the default configuration, append the following line
		CxxTest::TangoPrinter::restore_unset("NanInArrayProp_restore_point");
	}

    // test write then read array float property with nan and inf
    void test_write_then_read_array_float_property_with_nan_and_inf()
    {
        //
        // if your code modifies the default (device) configuration, append the following line straight after
        CxxTest::TangoPrinter::restore_set("NanInArrayProp_restore_point");

        write_then_read_array_property_containing_nan_and_inf<Tango::DevFloat>();

        // Restore the default configuration
        string prop_name = "NaNInfArrayProperty";
        TS_ASSERT_THROWS_NOTHING(device1->delete_property(prop_name));
        // if the test suite fails here, thanks to the restore point, the test suite TearDown method will restore the defaults
        // after you set back the default configuration, append the following line
        CxxTest::TangoPrinter::restore_unset("NanInArrayProp_restore_point");
    }

    // test write then read array double property with nan and inf
    void test_write_then_read_array_double_property_with_nan_and_inf()
    {
        //
        // if your code modifies the default (device) configuration, append the following line straight after
        CxxTest::TangoPrinter::restore_set("NanInArrayProp_restore_point");

        write_then_read_array_property_containing_nan_and_inf<Tango::DevDouble>();

        // Restore the default configuration
        string prop_name = "NaNInfArrayProperty";
        TS_ASSERT_THROWS_NOTHING(device1->delete_property(prop_name));
        // if the test suite fails here, thanks to the restore point, the test suite TearDown method will restore the defaults
        // after you set back the default configuration, append the following line
        CxxTest::TangoPrinter::restore_unset("NanInArrayProp_restore_point");
    }

    // test NaN in scalar property
	void test_NaN_in_scalar_property()
	{
		NaN_in_scalar_property("NaN");
		NaN_in_scalar_property("nan");
		NaN_in_scalar_property("Nan");
	}

	// test -NaN in scalar property
	void test_Negative_NaN_in_scalar_property()
	{
		NaN_in_scalar_property("-NaN");
		NaN_in_scalar_property("-nan");
		NaN_in_scalar_property("-naN");
	}

    // test infinity in scalar property
    void test_infinity_in_scalar_property()
    {
        //
        // if your code modifies the default (device) configuration, append the following line straight after
        CxxTest::TangoPrinter::restore_set("NanInfScalarProp_restore_point");

        // Write NaNInfArrayProperty device property
        Tango::DbData db_data_r;

        // Write NaNInfScalarProperty device property
        set_property("NaNInfScalarProperty", "InF");

        // Read NaNInfScalarProperty device property
        string prop_name = "NaNInfScalarProperty";
        TS_ASSERT_THROWS_NOTHING(device1->get_property(prop_name,db_data_r));
        // Extract as a string:
        string string_scalar_val;
        TS_ASSERT(db_data_r[0] >> string_scalar_val);
        TS_ASSERT_EQUALS(string_scalar_val,"InF");

        // Extract as float:
        Tango::DevFloat float_val;
        TS_ASSERT(db_data_r[0] >> float_val);
        TS_ASSERT(std::isinf(float_val));

        // Extract as double:
        Tango::DevDouble double_val;
        TS_ASSERT(db_data_r[0] >> double_val);
        TS_ASSERT(std::isinf(double_val));

        // Extract as short
        Tango::DevShort short_val;
        TS_ASSERT(!(db_data_r[0] >> short_val));

        // Restore the default configuration
        TS_ASSERT_THROWS_NOTHING(device1->delete_property(prop_name));
        // if the test suite fails here, thanks to the restore point, the test suite TearDown method will restore the defaults
        // after you set back the default configuration, append the following line
        CxxTest::TangoPrinter::restore_unset("NanInfScalarProp_restore_point");
    }

    // test -iinfinity in scalar property
    void test_minus_infinity_in_scalar_property()
    {
        //
        // if your code modifies the default (device) configuration, append the following line straight after
        CxxTest::TangoPrinter::restore_set("NanInfScalarProp_restore_point");

        // Write NaNInfArrayProperty device property
        Tango::DbData db_data_r;

        // Write NaNInfScalarProperty device property
        set_property("NaNInfScalarProperty", "-inF");

        // Read NaNInfScalarProperty device property
        string prop_name = "NaNInfScalarProperty";
        TS_ASSERT_THROWS_NOTHING(device1->get_property(prop_name,db_data_r));
        // Extract as a string:
        string string_scalar_val;
        TS_ASSERT(db_data_r[0] >> string_scalar_val);
        TS_ASSERT_EQUALS(string_scalar_val,"-inF");

        // Extract as float:
        Tango::DevFloat float_val;
        TS_ASSERT(db_data_r[0] >> float_val);
        TS_ASSERT(std::isinf(float_val));
        TS_ASSERT_LESS_THAN(float_val,0);

        // Extract as double:
        Tango::DevDouble double_val;
        TS_ASSERT(db_data_r[0] >> double_val);
        TS_ASSERT(std::isinf(double_val));
		TS_ASSERT_LESS_THAN(double_val,0);

        // Extract as short
        Tango::DevShort short_val;
        TS_ASSERT(!(db_data_r[0] >> short_val));

        // Restore the default configuration
        TS_ASSERT_THROWS_NOTHING(device1->delete_property(prop_name));
        // if the test suite fails here, thanks to the restore point, the test suite TearDown method will restore the defaults
        // after you set back the default configuration, append the following line
        CxxTest::TangoPrinter::restore_unset("NanInfScalarProp_restore_point");
    }

};
#undef cout
#endif // NanInfInPropSuite_h
