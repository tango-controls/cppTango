#ifndef EnumTestSuite_h
#define EnumTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define coutv_cb 	if (parent->verbose == true) cout << "\t"
#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME EnumAttTestSuite

class EnumAttTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy 			*device1,*adm_dev;
	string 					device1_name;

	bool					verbose;

public:
	SUITE_NAME()
	{
//
// Arguments check -------------------------------------------------
//


		// user arguments, obtained from the command line sequentially
		device1_name = CxxTest::TangoPrinter::get_param("device1");

		verbose = CxxTest::TangoPrinter::is_param_set("verbose");

		// always add this line, otherwise arguments will not be parsed correctly
		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//


		try
		{
			device1 = new DeviceProxy(device1_name);
			device1->ping();

			string adm_name = device1->adm_name();
			adm_dev = new DeviceProxy(adm_name);
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
		delete adm_dev;
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

// Test enum labels management in attribute config

	void test_enum_attribute_configuration()
	{
		Tango::AttributeInfoEx aie;
		TS_ASSERT_THROWS_NOTHING(aie = device1->get_attribute_config("Enum_attr_rw"));

		TS_ASSERT(aie.name == "Enum_attr_rw");
		TS_ASSERT(aie.writable == Tango::READ_WRITE);
		TS_ASSERT(aie.data_format == Tango::SCALAR);
		TS_ASSERT(aie.data_type == Tango::DEV_ENUM);

		TS_ASSERT(aie.enum_labels.size() == 4);
		TS_ASSERT(aie.enum_labels[0] == "North");
		TS_ASSERT(aie.enum_labels[1] == "South");
		TS_ASSERT(aie.enum_labels[2] == "East");
		TS_ASSERT(aie.enum_labels[3] == "West");

// Change enum labels

		aie.enum_labels[0] = "Nord";
		aie.enum_labels[1] = "Sud";
		aie.enum_labels[2] = "Est";
		aie.enum_labels[3] = "Ouest";

		Tango::AttributeInfoListEx aile;
		aile.push_back(aie);

		TS_ASSERT_THROWS_NOTHING(device1->set_attribute_config(aile));

		Tango::AttributeInfoEx aie2;
		TS_ASSERT_THROWS_NOTHING(aie2 = device1->get_attribute_config("Enum_attr_rw"));

		TS_ASSERT(aie2.enum_labels.size() == 4);
		TS_ASSERT(aie2.enum_labels[0] == "Nord");
		TS_ASSERT(aie2.enum_labels[1] == "Sud");
		TS_ASSERT(aie2.enum_labels[2] == "Est");
		TS_ASSERT(aie2.enum_labels[3] == "Ouest");

// Restart device and get att conf


		adm_dev->command_inout("RestartServer");
		Tango_sleep(3);

		TS_ASSERT_THROWS_NOTHING(aie2 = device1->get_attribute_config("Enum_attr_rw"));

		TS_ASSERT(aie2.enum_labels.size() == 4);
		TS_ASSERT(aie2.enum_labels[0] == "Nord");
		TS_ASSERT(aie2.enum_labels[1] == "Sud");
		TS_ASSERT(aie2.enum_labels[2] == "Est");
		TS_ASSERT(aie2.enum_labels[3] == "Ouest");

// Reset to user default

		aile[0].enum_labels.clear();
		aile[0].enum_labels.push_back("");

		TS_ASSERT_THROWS_NOTHING(device1->set_attribute_config(aile));

		TS_ASSERT_THROWS_NOTHING(aie2 = device1->get_attribute_config("Enum_attr_rw"));

		TS_ASSERT(aie2.enum_labels.size() == 4);
		TS_ASSERT(aie2.enum_labels[0] == "North");
		TS_ASSERT(aie2.enum_labels[1] == "South");
		TS_ASSERT(aie2.enum_labels[2] == "East");
		TS_ASSERT(aie2.enum_labels[3] == "West");

// Two times the same label is invalid

		aile[0].enum_labels.clear();
		aile[0].enum_labels.push_back("North");
		aile[0].enum_labels.push_back("South");
		aile[0].enum_labels.push_back("East");
		aile[0].enum_labels.push_back("North");

		TS_ASSERT_THROWS_ASSERT(device1->set_attribute_config(aile),Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrOptProp"
								&& e.errors[0].severity == Tango::ERR));

// Reset to lib default is invalid

		aile[0].enum_labels.clear();
		aile[0].enum_labels.push_back("Not specified");

		TS_ASSERT_THROWS_ASSERT(device1->set_attribute_config(aile),Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrOptProp"
								&& e.errors[0].severity == Tango::ERR));

// Change enum labels number is not authorized from outside the Tango class

		aile[0].enum_labels.clear();
		aile[0].enum_labels.push_back("North");
		aile[0].enum_labels.push_back("South");

		TS_ASSERT_THROWS_ASSERT(device1->set_attribute_config(aile),Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_NotSupportedFeature"
								&& e.errors[0].severity == Tango::ERR));
	}

// Test read/write enumerated attribute

	void test_enum_attribute_reading()
	{
		DeviceAttribute da;
		TS_ASSERT_THROWS_NOTHING(da = device1->read_attribute("Enum_attr_rw"));

		short sh;
		TS_ASSERT_THROWS_NOTHING(da >> sh);
		TS_ASSERT(sh == 1);

		TS_ASSERT_THROWS_NOTHING(da = device1->read_attribute("Enum_spec_attr_rw"));

		vector<short> v_sh_read;
		vector<short> v_sh_write;
		TS_ASSERT_THROWS_NOTHING(da.extract_read(v_sh_read));
		TS_ASSERT_THROWS_NOTHING(da.extract_set(v_sh_write));

		TS_ASSERT(v_sh_read.size() == 3);
		TS_ASSERT(v_sh_read[0] == 1);
		TS_ASSERT(v_sh_read[1] == 0);
		TS_ASSERT(v_sh_read[2] == 3);

		TS_ASSERT(v_sh_write.size() == 1);
		TS_ASSERT(v_sh_write[0] == 0);
	}

	void test_enum_attribute_writing()
	{

// Scalar att

		short sh_wr = 2;
		DeviceAttribute da_wr("Enum_attr_rw",sh_wr);	
		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(da_wr));

		DeviceAttribute da_read;
		TS_ASSERT_THROWS_NOTHING(da_read = device1->read_attribute("Enum_attr_rw"));
		vector<short> sh_rd;
		TS_ASSERT_THROWS_NOTHING(da_read >> sh_rd);
		TS_ASSERT(sh_rd.size() == 2);
		TS_ASSERT(sh_rd[0] == 1);
		TS_ASSERT(sh_rd[1] == 2);

// Spectrum att

		vector<short> v_sh_wr;
		v_sh_wr.push_back(1);
		v_sh_wr.push_back(1);
		DeviceAttribute da_wr2("Enum_spec_attr_rw",v_sh_wr);
		TS_ASSERT_THROWS_NOTHING(device1->write_attribute(da_wr2));

		DeviceAttribute da_read2;
		TS_ASSERT_THROWS_NOTHING(da_read2 = device1->read_attribute("Enum_spec_attr_rw"));

		vector<short> v_sh_read;
		vector<short> v_sh_write;
		TS_ASSERT_THROWS_NOTHING(da_read2.extract_read(v_sh_read));
		TS_ASSERT_THROWS_NOTHING(da_read2.extract_set(v_sh_write));

		TS_ASSERT(v_sh_read.size() == 3);
		TS_ASSERT(v_sh_read[0] == 1);
		TS_ASSERT(v_sh_read[1] == 0);
		TS_ASSERT(v_sh_read[2] == 3);

		TS_ASSERT(v_sh_write.size() == 2);
		TS_ASSERT(v_sh_write[0] == 1);
		TS_ASSERT(v_sh_write[1] == 1);
	}
};

#undef cout
#endif // EnumTestSuite_h
