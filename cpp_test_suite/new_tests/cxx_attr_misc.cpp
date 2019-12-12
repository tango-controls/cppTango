#ifndef AttrMiscTestSuite_h
#define AttrMiscTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME AttrMiscTestSuite

class AttrMiscTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1, *dserver;
	string device1_name;
	AttributeInfoListEx *init_attr_conf;
	int def_timeout;

public:
	SUITE_NAME()
	{
		// default timeout
		def_timeout = 3000;


//
// Arguments check -------------------------------------------------
//

		string dserver_name;

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

			dserver->command_inout("RestartServer");
			Tango_sleep(10);

			vector<string> attr_list;
			attr_list.push_back("Double_attr");
			attr_list.push_back("Float_attr");
			attr_list.push_back("Long_attr");
			attr_list.push_back("Long64_attr");
			attr_list.push_back("Short_attr");
			attr_list.push_back("UChar_attr");
			attr_list.push_back("ULong_attr");
			attr_list.push_back("ULong64_attr");
			attr_list.push_back("UShort_attr");
			init_attr_conf = device1->get_attribute_config_ex(attr_list);

			def_timeout = device1->get_timeout_millis();
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}

	}

	virtual ~SUITE_NAME()
	{
time_t ti = time(NULL);
cout << "Destroying suite at " << ctime(&ti) << endl;
		DevLong lg;
		DeviceData din;
		lg = 1246;
		din << lg;
		try
		{
            device1->set_timeout_millis(9000);
			device1->set_attribute_config(*init_attr_conf);

			device1->command_inout("IOSetAttr", din);
			din << device1_name;
			dserver->command_inout("DevRestart", din);
            device1->set_timeout_millis(def_timeout);
		}
		catch (CORBA::Exception &e)
		{
			cout << endl << "Exception in suite tearDown():" << endl;
			Except::print_exception(e);
			exit(-1);
		}

//
// Clean up --------------------------------------------------------
//

		// clean up in case test suite terminates before timeout is restored to defaults
		if(CxxTest::TangoPrinter::is_restore_set("timeout"))
		{
			try
			{
				device1->set_timeout_millis(def_timeout);
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

//
// Test set/get min/max alarm/warning functions
//

	void test_set_get_alarms(void)
	{
		const DevVarStringArray *alarms;
		DeviceData dout;

		device1->set_timeout_millis(10*def_timeout);
		CxxTest::TangoPrinter::restore_set("timeout");

		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("SetGetAlarms"));
		TS_ASSERT_THROWS_NOTHING(dout >> alarms);

		TS_ASSERT((*alarms).length() == 45);
		TS_ASSERT(string((*alarms)[0].in()) == "Double_attr");
		TS_ASSERT(string((*alarms)[1].in()) == "-999.99");
		TS_ASSERT(string((*alarms)[2].in()) == "-888.88");
		TS_ASSERT(string((*alarms)[3].in()) == "888.88");
		TS_ASSERT(string((*alarms)[4].in()) == "999.99");
		TS_ASSERT(string((*alarms)[5].in()) == "Float_attr");
		TS_ASSERT(string((*alarms)[6].in()) == "-777.77");
		TS_ASSERT(string((*alarms)[7].in()) == "-666.66");
		TS_ASSERT(string((*alarms)[8].in()) == "666.66");
		TS_ASSERT(string((*alarms)[9].in()) == "777.77");
		TS_ASSERT(string((*alarms)[10].in()) == "Long_attr");
		TS_ASSERT(string((*alarms)[11].in()) == "1000");
		TS_ASSERT(string((*alarms)[12].in()) == "1100");
		TS_ASSERT(string((*alarms)[13].in()) == "1400");
		TS_ASSERT(string((*alarms)[14].in()) == "1500");
		TS_ASSERT(string((*alarms)[15].in()) == "Long64_attr");
		TS_ASSERT(string((*alarms)[16].in()) == "-90000");
		TS_ASSERT(string((*alarms)[17].in()) == "-80000");
		TS_ASSERT(string((*alarms)[18].in()) == "80000");
		TS_ASSERT(string((*alarms)[19].in()) == "90000");
		TS_ASSERT(string((*alarms)[20].in()) == "Short_attr");
		TS_ASSERT(string((*alarms)[21].in()) == "-5000");
		TS_ASSERT(string((*alarms)[22].in()) == "-4000");
		TS_ASSERT(string((*alarms)[23].in()) == "4000");
		TS_ASSERT(string((*alarms)[24].in()) == "5000");
		TS_ASSERT(string((*alarms)[25].in()) == "UChar_attr");
		TS_ASSERT(string((*alarms)[26].in()) == "1");
		TS_ASSERT(string((*alarms)[27].in()) == "2");
		TS_ASSERT(string((*alarms)[28].in()) == "230");
		TS_ASSERT(string((*alarms)[29].in()) == "240");
		TS_ASSERT(string((*alarms)[30].in()) == "ULong_attr");
		TS_ASSERT(string((*alarms)[31].in()) == "1");
		TS_ASSERT(string((*alarms)[32].in()) == "2");
		TS_ASSERT(string((*alarms)[33].in()) == "666666");
		TS_ASSERT(string((*alarms)[34].in()) == "777777");
		TS_ASSERT(string((*alarms)[35].in()) == "ULong64_attr");
		TS_ASSERT(string((*alarms)[36].in()) == "1");
		TS_ASSERT(string((*alarms)[37].in()) == "2");
		TS_ASSERT(string((*alarms)[38].in()) == "77777777");
		TS_ASSERT(string((*alarms)[39].in()) == "88888888");
		TS_ASSERT(string((*alarms)[40].in()) == "UShort_attr");
		TS_ASSERT(string((*alarms)[41].in()) == "1");
		TS_ASSERT(string((*alarms)[42].in()) == "2");
		TS_ASSERT(string((*alarms)[43].in()) == "20000");
		TS_ASSERT(string((*alarms)[44].in()) == "30000");

		device1->set_timeout_millis(def_timeout);
		CxxTest::TangoPrinter::restore_unset("timeout");
	}

//
// Test set/get min/max value functions
//

	void test_set_get_ranges(void)
	{
		const DevVarStringArray *ranges;
		DeviceData dout;

		device1->set_timeout_millis(6*def_timeout);
		CxxTest::TangoPrinter::restore_set("timeout");

		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("SetGetRanges"));
		TS_ASSERT_THROWS_NOTHING(dout >> ranges);

		TS_ASSERT((*ranges).length() == 27);
		TS_ASSERT(string((*ranges)[0].in()) == "Double_attr_w");
		TS_ASSERT(string((*ranges)[1].in()) == "-1111.11");
		TS_ASSERT(string((*ranges)[2].in()) == "1111.11");
		TS_ASSERT(string((*ranges)[3].in()) == "Float_attr_w");
		TS_ASSERT(string((*ranges)[4].in()) == "-888.88");
		TS_ASSERT(string((*ranges)[5].in()) == "888.88");
		TS_ASSERT(string((*ranges)[6].in()) == "Long_attr_w");
		TS_ASSERT(string((*ranges)[7].in()) == "900");
		TS_ASSERT(string((*ranges)[8].in()) == "1600");
		TS_ASSERT(string((*ranges)[9].in()) == "Long64_attr_rw");
		TS_ASSERT(string((*ranges)[10].in()) == "-100000");
		TS_ASSERT(string((*ranges)[11].in()) == "100000");
		TS_ASSERT(string((*ranges)[12].in()) == "Short_attr_w");
		TS_ASSERT(string((*ranges)[13].in()) == "-6000");
		TS_ASSERT(string((*ranges)[14].in()) == "6000");
		TS_ASSERT(string((*ranges)[15].in()) == "UChar_attr_w");
		TS_ASSERT(string((*ranges)[16].in()) == "0");
		TS_ASSERT(string((*ranges)[17].in()) == "250");
		TS_ASSERT(string((*ranges)[18].in()) == "ULong_attr_rw");
		TS_ASSERT(string((*ranges)[19].in()) == "0");
		TS_ASSERT(string((*ranges)[20].in()) == "888888");
		TS_ASSERT(string((*ranges)[21].in()) == "ULong64_attr_rw");
		TS_ASSERT(string((*ranges)[22].in()) == "0");
		TS_ASSERT(string((*ranges)[23].in()) == "99999999");
		TS_ASSERT(string((*ranges)[24].in()) == "UShort_attr_w");
		TS_ASSERT(string((*ranges)[25].in()) == "0");
		TS_ASSERT(string((*ranges)[26].in()) == "40000");

		device1->set_timeout_millis(def_timeout);
		CxxTest::TangoPrinter::restore_unset("timeout");
	}

//
// Test set/get properties functions
//

	void test_set_get_properties(void)
	{
/*		const DevVarStringArray *props;
		DeviceData dout;

		device1->set_timeout_millis(25*def_timeout);
		CxxTest::TangoPrinter::restore_set("timeout");

time_t ti = time(NULL);
cout << "Calling SetGetProperties at " << ctime(&ti) << endl;
struct timeval start,stop;
gettimeofday(&start,NULL);
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("SetGetProperties"));
		TS_ASSERT_THROWS_NOTHING(dout >> props);
gettimeofday(&stop,NULL);
double elapsed = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000.0;
cout << "required time for command SetGetProperties = " << elapsed << endl;

//		cout << "## prop length = " << (*props).length() << endl;

		TS_ASSERT((*props).length() == 420);
		size_t i = 0;
		TS_ASSERT(string((*props)[i].in()) == "Double_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0"); i++;
		TS_ASSERT(string((*props)[i].in()) == "200"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "190"); i++;
		TS_ASSERT(string((*props)[i].in()) == "20"); i++;
		TS_ASSERT(string((*props)[i].in()) == "180"); i++;
		TS_ASSERT(string((*props)[i].in()) == "5"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "300"); i++;
		TS_ASSERT(string((*props)[i].in()) == "400"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.2,0.3"); i++;
		TS_ASSERT(string((*props)[i].in()) == "40,50"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.6,0.7"); i++;
		TS_ASSERT(string((*props)[i].in()) == "80,90"); i++;

		TS_ASSERT(string((*props)[i].in()) == "Double_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "1"); i++;
		TS_ASSERT(string((*props)[i].in()) == "201"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "191"); i++;
		TS_ASSERT(string((*props)[i].in()) == "21"); i++;
		TS_ASSERT(string((*props)[i].in()) == "181"); i++;
		TS_ASSERT(string((*props)[i].in()) == "6"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "301"); i++;
		TS_ASSERT(string((*props)[i].in()) == "401"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.3,0.4"); i++;
		TS_ASSERT(string((*props)[i].in()) == "41,51"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.7,0.8"); i++;
		TS_ASSERT(string((*props)[i].in()) == "81,91"); i++;





		TS_ASSERT(string((*props)[i].in()) == "Float_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0"); i++;
		TS_ASSERT(string((*props)[i].in()) == "200"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "190"); i++;
		TS_ASSERT(string((*props)[i].in()) == "20"); i++;
		TS_ASSERT(string((*props)[i].in()) == "180"); i++;
		TS_ASSERT(string((*props)[i].in()) == "5"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "300"); i++;
		TS_ASSERT(string((*props)[i].in()) == "400"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.2,0.3"); i++;
		TS_ASSERT(string((*props)[i].in()) == "40,50"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.6,0.7"); i++;
		TS_ASSERT(string((*props)[i].in()) == "80,90"); i++;

		TS_ASSERT(string((*props)[i].in()) == "Float_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "1"); i++;
		TS_ASSERT(string((*props)[i].in()) == "201"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "191"); i++;
		TS_ASSERT(string((*props)[i].in()) == "21"); i++;
		TS_ASSERT(string((*props)[i].in()) == "181"); i++;
		TS_ASSERT(string((*props)[i].in()) == "6"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "301"); i++;
		TS_ASSERT(string((*props)[i].in()) == "401"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.3,0.4"); i++;
		TS_ASSERT(string((*props)[i].in()) == "41,51"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.7,0.8"); i++;
		TS_ASSERT(string((*props)[i].in()) == "81,91"); i++;







		TS_ASSERT(string((*props)[i].in()) == "Long_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0"); i++;
		TS_ASSERT(string((*props)[i].in()) == "200"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "190"); i++;
		TS_ASSERT(string((*props)[i].in()) == "20"); i++;
		TS_ASSERT(string((*props)[i].in()) == "180"); i++;
		TS_ASSERT(string((*props)[i].in()) == "5"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "300"); i++;
		TS_ASSERT(string((*props)[i].in()) == "400"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.2,0.3"); i++;
		TS_ASSERT(string((*props)[i].in()) == "40,50"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.6,0.7"); i++;
		TS_ASSERT(string((*props)[i].in()) == "80,90"); i++;

		TS_ASSERT(string((*props)[i].in()) == "Long_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "1"); i++;
		TS_ASSERT(string((*props)[i].in()) == "201"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "191"); i++;
		TS_ASSERT(string((*props)[i].in()) == "21"); i++;
		TS_ASSERT(string((*props)[i].in()) == "181"); i++;
		TS_ASSERT(string((*props)[i].in()) == "6"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "301"); i++;
		TS_ASSERT(string((*props)[i].in()) == "401"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.3,0.4"); i++;
		TS_ASSERT(string((*props)[i].in()) == "41,51"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.7,0.8"); i++;
		TS_ASSERT(string((*props)[i].in()) == "81,91"); i++;




		TS_ASSERT(string((*props)[i].in()) == "Long64_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0"); i++;
		TS_ASSERT(string((*props)[i].in()) == "200"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "190"); i++;
		TS_ASSERT(string((*props)[i].in()) == "20"); i++;
		TS_ASSERT(string((*props)[i].in()) == "180"); i++;
		TS_ASSERT(string((*props)[i].in()) == "5"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "300"); i++;
		TS_ASSERT(string((*props)[i].in()) == "400"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.2,0.3"); i++;
		TS_ASSERT(string((*props)[i].in()) == "40,50"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.6,0.7"); i++;
		TS_ASSERT(string((*props)[i].in()) == "80,90"); i++;

		TS_ASSERT(string((*props)[i].in()) == "Long64_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "1"); i++;
		TS_ASSERT(string((*props)[i].in()) == "201"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "191"); i++;
		TS_ASSERT(string((*props)[i].in()) == "21"); i++;
		TS_ASSERT(string((*props)[i].in()) == "181"); i++;
		TS_ASSERT(string((*props)[i].in()) == "6"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "301"); i++;
		TS_ASSERT(string((*props)[i].in()) == "401"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.3,0.4"); i++;
		TS_ASSERT(string((*props)[i].in()) == "41,51"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.7,0.8"); i++;
		TS_ASSERT(string((*props)[i].in()) == "81,91"); i++;






		TS_ASSERT(string((*props)[i].in()) == "Short_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0"); i++;
		TS_ASSERT(string((*props)[i].in()) == "200"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "190"); i++;
		TS_ASSERT(string((*props)[i].in()) == "20"); i++;
		TS_ASSERT(string((*props)[i].in()) == "180"); i++;
		TS_ASSERT(string((*props)[i].in()) == "5"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "300"); i++;
		TS_ASSERT(string((*props)[i].in()) == "400"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.2,0.3"); i++;
		TS_ASSERT(string((*props)[i].in()) == "40,50"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.6,0.7"); i++;
		TS_ASSERT(string((*props)[i].in()) == "80,90"); i++;

		TS_ASSERT(string((*props)[i].in()) == "Short_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "1"); i++;
		TS_ASSERT(string((*props)[i].in()) == "201"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "191"); i++;
		TS_ASSERT(string((*props)[i].in()) == "21"); i++;
		TS_ASSERT(string((*props)[i].in()) == "181"); i++;
		TS_ASSERT(string((*props)[i].in()) == "6"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "301"); i++;
		TS_ASSERT(string((*props)[i].in()) == "401"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.3,0.4"); i++;
		TS_ASSERT(string((*props)[i].in()) == "41,51"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.7,0.8"); i++;
		TS_ASSERT(string((*props)[i].in()) == "81,91"); i++;





		TS_ASSERT(string((*props)[i].in()) == "UChar_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0"); i++;
		TS_ASSERT(string((*props)[i].in()) == "200"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "190"); i++;
		TS_ASSERT(string((*props)[i].in()) == "20"); i++;
		TS_ASSERT(string((*props)[i].in()) == "180"); i++;
		TS_ASSERT(string((*props)[i].in()) == "5"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "300"); i++;
		TS_ASSERT(string((*props)[i].in()) == "400"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.2,0.3"); i++;
		TS_ASSERT(string((*props)[i].in()) == "40,50"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.6,0.7"); i++;
		TS_ASSERT(string((*props)[i].in()) == "80,90"); i++;

		TS_ASSERT(string((*props)[i].in()) == "UChar_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "1"); i++;
		TS_ASSERT(string((*props)[i].in()) == "201"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "191"); i++;
		TS_ASSERT(string((*props)[i].in()) == "21"); i++;
		TS_ASSERT(string((*props)[i].in()) == "181"); i++;
		TS_ASSERT(string((*props)[i].in()) == "6"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "301"); i++;
		TS_ASSERT(string((*props)[i].in()) == "401"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.3,0.4"); i++;
		TS_ASSERT(string((*props)[i].in()) == "41,51"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.7,0.8"); i++;
		TS_ASSERT(string((*props)[i].in()) == "81,91"); i++;




		TS_ASSERT(string((*props)[i].in()) == "ULong_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0"); i++;
		TS_ASSERT(string((*props)[i].in()) == "200"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "190"); i++;
		TS_ASSERT(string((*props)[i].in()) == "20"); i++;
		TS_ASSERT(string((*props)[i].in()) == "180"); i++;
		TS_ASSERT(string((*props)[i].in()) == "5"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "300"); i++;
		TS_ASSERT(string((*props)[i].in()) == "400"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.2,0.3"); i++;
		TS_ASSERT(string((*props)[i].in()) == "40,50"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.6,0.7"); i++;
		TS_ASSERT(string((*props)[i].in()) == "80,90"); i++;

		TS_ASSERT(string((*props)[i].in()) == "ULong_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "1"); i++;
		TS_ASSERT(string((*props)[i].in()) == "201"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "191"); i++;
		TS_ASSERT(string((*props)[i].in()) == "21"); i++;
		TS_ASSERT(string((*props)[i].in()) == "181"); i++;
		TS_ASSERT(string((*props)[i].in()) == "6"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "301"); i++;
		TS_ASSERT(string((*props)[i].in()) == "401"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.3,0.4"); i++;
		TS_ASSERT(string((*props)[i].in()) == "41,51"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.7,0.8"); i++;
		TS_ASSERT(string((*props)[i].in()) == "81,91"); i++;




		TS_ASSERT(string((*props)[i].in()) == "ULong64_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0"); i++;
		TS_ASSERT(string((*props)[i].in()) == "200"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "190"); i++;
		TS_ASSERT(string((*props)[i].in()) == "20"); i++;
		TS_ASSERT(string((*props)[i].in()) == "180"); i++;
		TS_ASSERT(string((*props)[i].in()) == "5"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "300"); i++;
		TS_ASSERT(string((*props)[i].in()) == "400"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.2,0.3"); i++;
		TS_ASSERT(string((*props)[i].in()) == "40,50"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.6,0.7"); i++;
		TS_ASSERT(string((*props)[i].in()) == "80,90"); i++;

		TS_ASSERT(string((*props)[i].in()) == "ULong64_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "1"); i++;
		TS_ASSERT(string((*props)[i].in()) == "201"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "191"); i++;
		TS_ASSERT(string((*props)[i].in()) == "21"); i++;
		TS_ASSERT(string((*props)[i].in()) == "181"); i++;
		TS_ASSERT(string((*props)[i].in()) == "6"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "301"); i++;
		TS_ASSERT(string((*props)[i].in()) == "401"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.3,0.4"); i++;
		TS_ASSERT(string((*props)[i].in()) == "41,51"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.7,0.8"); i++;
		TS_ASSERT(string((*props)[i].in()) == "81,91"); i++;



		TS_ASSERT(string((*props)[i].in()) == "UShort_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0"); i++;
		TS_ASSERT(string((*props)[i].in()) == "200"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "190"); i++;
		TS_ASSERT(string((*props)[i].in()) == "20"); i++;
		TS_ASSERT(string((*props)[i].in()) == "180"); i++;
		TS_ASSERT(string((*props)[i].in()) == "5"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "300"); i++;
		TS_ASSERT(string((*props)[i].in()) == "400"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.2,0.3"); i++;
		TS_ASSERT(string((*props)[i].in()) == "40,50"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.6,0.7"); i++;
		TS_ASSERT(string((*props)[i].in()) == "80,90");i++;

		TS_ASSERT(string((*props)[i].in()) == "UShort_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "1"); i++;
		TS_ASSERT(string((*props)[i].in()) == "201"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "191"); i++;
		TS_ASSERT(string((*props)[i].in()) == "21"); i++;
		TS_ASSERT(string((*props)[i].in()) == "181"); i++;
		TS_ASSERT(string((*props)[i].in()) == "6"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "301"); i++;
		TS_ASSERT(string((*props)[i].in()) == "401"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.3,0.4"); i++;
		TS_ASSERT(string((*props)[i].in()) == "41,51"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.7,0.8"); i++;
		TS_ASSERT(string((*props)[i].in()) == "81,91"); i++;


		TS_ASSERT(string((*props)[i].in()) == "Encoded_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0"); i++;
		TS_ASSERT(string((*props)[i].in()) == "200"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "190"); i++;
		TS_ASSERT(string((*props)[i].in()) == "20"); i++;
		TS_ASSERT(string((*props)[i].in()) == "180"); i++;
		TS_ASSERT(string((*props)[i].in()) == "5"); i++;
		TS_ASSERT(string((*props)[i].in()) == "10"); i++;
		TS_ASSERT(string((*props)[i].in()) == "300"); i++;
		TS_ASSERT(string((*props)[i].in()) == "400"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.2,0.3"); i++;
		TS_ASSERT(string((*props)[i].in()) == "40,50"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.6,0.7"); i++;
		TS_ASSERT(string((*props)[i].in()) == "80,90");i++;

		TS_ASSERT(string((*props)[i].in()) == "Encoded_attr"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_label"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_description"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_standard_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_display_unit"); i++;
		TS_ASSERT(string((*props)[i].in()) == "Test_format"); i++;
		TS_ASSERT(string((*props)[i].in()) == "1"); i++;
		TS_ASSERT(string((*props)[i].in()) == "201"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "191"); i++;
		TS_ASSERT(string((*props)[i].in()) == "21"); i++;
		TS_ASSERT(string((*props)[i].in()) == "181"); i++;
		TS_ASSERT(string((*props)[i].in()) == "6"); i++;
		TS_ASSERT(string((*props)[i].in()) == "11"); i++;
		TS_ASSERT(string((*props)[i].in()) == "301"); i++;
		TS_ASSERT(string((*props)[i].in()) == "401"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.3,0.4"); i++;
		TS_ASSERT(string((*props)[i].in()) == "41,51"); i++;
		TS_ASSERT(string((*props)[i].in()) == "0.7,0.8"); i++;
		TS_ASSERT(string((*props)[i].in()) == "81,91"); i++;


		device1->set_timeout_millis(def_timeout);
		CxxTest::TangoPrinter::restore_unset("timeout");*/
	}

// Test read attribute exceptions

	void test_read_attribute_exceptions(void)
	{
		DeviceAttribute attr;
		DevShort sh;
		DevLong lg;

		TS_ASSERT_THROWS_NOTHING(attr = device1->read_attribute("Toto"));
		TS_ASSERT_THROWS_ASSERT(attr >> sh, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrNotFound"
								&& e.errors[0].severity == Tango::ERR));

		TS_ASSERT_THROWS_NOTHING(attr = device1->read_attribute("attr_no_data"));
		TS_ASSERT_THROWS_ASSERT(attr >> sh, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrValueNotSet"
								&& e.errors[0].severity == Tango::ERR));

		TS_ASSERT_THROWS_NOTHING(attr = device1->read_attribute("attr_wrong_type"));
		TS_ASSERT_THROWS_ASSERT(attr >> sh, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrOptProp"
								&& e.errors[0].severity == Tango::ERR));

		TS_ASSERT_THROWS_NOTHING(attr = device1->read_attribute("attr_wrong_size"));
		TS_ASSERT_THROWS_ASSERT(attr >> lg, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrOptProp"
								&& e.errors[0].severity == Tango::ERR));

		TS_ASSERT_THROWS_NOTHING(attr = device1->read_attribute("attr_no_alarm"));
		TS_ASSERT_THROWS_ASSERT(attr >> lg, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == "API_AttrNoAlarm"
								&& e.errors[0].severity == Tango::ERR));
	}

//
// Testing SCALAR attribute of type different than READ
// As we have never written any attribute (yet), the write values should all
// be initialised to 0
//

	void test_SCALAR_attribute_of_type_different_than_READ(void)
	{
		DeviceAttribute long_attr_with_w, long_attr_w, short_attr_rw, float_attr_w, ushort_attr_w, uchar_attr_w;
		DevVarLongArray *lg_array;
		DevVarShortArray *sh_array;
		DevVarFloatArray *fl_array;
		DevVarUShortArray *ush_array;
		DevVarUCharArray *uch_array;

		TS_ASSERT_THROWS_NOTHING(long_attr_with_w = device1->read_attribute("Long_attr_with_w"));
		TS_ASSERT(long_attr_with_w.get_name() == "Long_attr_with_w");
		TS_ASSERT(long_attr_with_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr_with_w.get_dim_x() == 1);
		TS_ASSERT(long_attr_with_w.get_dim_y() == 0);
		long_attr_with_w >> lg_array;
		TS_ASSERT((*lg_array)[0] == 1246);
		TS_ASSERT((*lg_array)[1] == 0);

		TS_ASSERT_THROWS_NOTHING(long_attr_w = device1->read_attribute("Long_attr_w"));
		TS_ASSERT(long_attr_w.get_name() == "Long_attr_w");
		TS_ASSERT(long_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr_w.get_dim_x() == 1);
		TS_ASSERT(long_attr_w.get_dim_y() == 0);
		long_attr_w >> lg_array ;
		TS_ASSERT((*lg_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(short_attr_rw = device1->read_attribute("Short_attr_rw"));
		TS_ASSERT(short_attr_rw.get_name() == "Short_attr_rw");
		TS_ASSERT(short_attr_rw.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr_rw.get_dim_x() == 1);
		TS_ASSERT(short_attr_rw.get_dim_y() == 0);
		short_attr_rw >> sh_array;
		TS_ASSERT((*sh_array)[0] == 66);
		TS_ASSERT((*sh_array)[1] == 0);

		TS_ASSERT_THROWS_NOTHING(float_attr_w = device1->read_attribute("Float_attr_w"));
		TS_ASSERT(float_attr_w.get_name() == "Float_attr_w");
		TS_ASSERT(float_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(float_attr_w.get_dim_x() == 1);
		TS_ASSERT(float_attr_w.get_dim_y() == 0);
		float_attr_w >> fl_array;
		TS_ASSERT((*fl_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(ushort_attr_w = device1->read_attribute("UShort_attr_w"));
		TS_ASSERT(ushort_attr_w.get_name() == "UShort_attr_w");
		TS_ASSERT(ushort_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(ushort_attr_w.get_dim_x() == 1);
		TS_ASSERT(ushort_attr_w.get_dim_y() == 0);
		ushort_attr_w >> ush_array;
		TS_ASSERT((*ush_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(uchar_attr_w = device1->read_attribute("UChar_attr_w"));
		TS_ASSERT(uchar_attr_w.get_name() == "UChar_attr_w");
		TS_ASSERT(uchar_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(uchar_attr_w.get_dim_x() == 1);
		TS_ASSERT(uchar_attr_w.get_dim_y() == 0);
		uchar_attr_w >> uch_array;
		TS_ASSERT((*uch_array)[0] == 0);
	}

// Test read attribute on write type attribute

	void test_read_attribute_on_write_type_attribute(void)
	{
		DeviceAttribute short_attr_w2, long_attr_w, double_attr_w, string_attr_w2;
		DevVarShortArray *sh_array;
		DevVarLongArray *lg_array;
		DevVarDoubleArray *db_array;
		DevVarStringArray *str_array;

		TS_ASSERT_THROWS_NOTHING(short_attr_w2 = device1->read_attribute("Short_attr_w2"));
		TS_ASSERT(short_attr_w2.get_name() == "Short_attr_w2");
		TS_ASSERT(short_attr_w2.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr_w2.get_dim_x() == 1);
		TS_ASSERT(short_attr_w2.get_dim_y() == 0);
		short_attr_w2 >> sh_array;
		TS_ASSERT((*sh_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(long_attr_w = device1->read_attribute("Long_attr_w"));
		TS_ASSERT(long_attr_w.get_name() == "Long_attr_w");
		TS_ASSERT(long_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr_w.get_dim_x() == 1);
		TS_ASSERT(long_attr_w.get_dim_y() == 0);
		long_attr_w >> lg_array;
		TS_ASSERT((*lg_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(double_attr_w = device1->read_attribute("Double_attr_w"));
		TS_ASSERT(double_attr_w.get_name() == "Double_attr_w");
		TS_ASSERT(double_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr_w.get_dim_x() == 1);
		TS_ASSERT(double_attr_w.get_dim_y() == 0);
		double_attr_w >> db_array;
		TS_ASSERT((*db_array)[0] == 0);

		TS_ASSERT_THROWS_NOTHING(string_attr_w2 = device1->read_attribute("String_attr_w2"));
		TS_ASSERT(string_attr_w2.get_name() == "String_attr_w2");
		TS_ASSERT(string_attr_w2.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(string_attr_w2.get_dim_x() == 1);
		TS_ASSERT(string_attr_w2.get_dim_y() == 0);
		string_attr_w2 >> str_array;
		TS_ASSERT(string((*str_array)[0].in()) == "Not initialised");
	}

// Test read attribute on initialised write type attribute

	void test_read_attribute_on_initialised_write_type_attribute(void)
	{
		DeviceAttribute short_attr_w, long_attr_w, double_attr_w, string_attr_w;
		DevVarShortArray *sh_array;
		DevVarLongArray *lg_array;
		DevVarDoubleArray *db_array;
		DevVarStringArray *str_array;

		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOInitWAttr"));

		TS_ASSERT_THROWS_NOTHING(short_attr_w = device1->read_attribute("Short_attr_w"));
		TS_ASSERT(short_attr_w.get_name() == "Short_attr_w");
		TS_ASSERT(short_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(short_attr_w.get_dim_x() == 1);
		TS_ASSERT(short_attr_w.get_dim_y() == 0);
		short_attr_w >> sh_array;
		TS_ASSERT((*sh_array)[0] == 10);

		TS_ASSERT_THROWS_NOTHING(long_attr_w = device1->read_attribute("Long_attr_w"));
		TS_ASSERT(long_attr_w.get_name() == "Long_attr_w");
		TS_ASSERT(long_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr_w.get_dim_x() == 1);
		TS_ASSERT(long_attr_w.get_dim_y() == 0);
		long_attr_w >> lg_array;
		TS_ASSERT((*lg_array)[0] == 100);

		TS_ASSERT_THROWS_NOTHING(double_attr_w = device1->read_attribute("Double_attr_w"));
		TS_ASSERT(double_attr_w.get_name() == "Double_attr_w");
		TS_ASSERT(double_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(double_attr_w.get_dim_x() == 1);
		TS_ASSERT(double_attr_w.get_dim_y() == 0);
		double_attr_w >> db_array;
		TS_ASSERT((*db_array)[0] == 1.1);

		TS_ASSERT_THROWS_NOTHING(string_attr_w = device1->read_attribute("String_attr_w"));
		TS_ASSERT(string_attr_w.get_name() == "String_attr_w");
		TS_ASSERT(string_attr_w.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(string_attr_w.get_dim_x() == 1);
		TS_ASSERT(string_attr_w.get_dim_y() == 0);
		string_attr_w >> str_array;
		TS_ASSERT(string((*str_array)[0].in()) == "Init");
	}

//
// Test alarm on attribute. An alarm is defined for the Long_attr attribute
// is < 1000 and > 1500.
//

	void test_alarm_on_attribute(void)
	{
		DeviceAttribute long_attr;
		DevVarLongArray *lg_array;
		DevLong lg;
		DeviceData din, dout;
		DevState state;
		const char *status;

		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);

		TS_ASSERT_THROWS_NOTHING(long_attr = device1->read_attribute("Long_attr"));
		TS_ASSERT(long_attr.get_name() == "Long_attr");
		TS_ASSERT(long_attr.get_quality() == Tango::ATTR_VALID);
		TS_ASSERT(long_attr.get_dim_x() == 1);
		TS_ASSERT(long_attr.get_dim_y() == 0);
		long_attr >> lg_array;
		TS_ASSERT((*lg_array)[0] == 1246);

		lg = 900;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOSetAttr", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ALARM);
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("Status"));
		dout >> status;
cout << "status = " << status << endl;
		TS_ASSERT(strcmp(status,"The device is in ALARM state.\nAlarm : Value too low for Long_attr") == 0);

		lg = 1200;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOSetAttr", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("Status"));
		dout >> status;
		TS_ASSERT(strcmp(status,"The device is in ON state.") == 0);

		state = Tango::ON;
		din << state;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("Status"));
		dout >> status;
		TS_ASSERT(strcmp(status,"The device is in ON state.") == 0);

		lg = 2000;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOSetAttr", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ALARM);
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("Status"));
		dout >> status;
		TS_ASSERT(strcmp(status,"The device is in ALARM state.\nAlarm : Value too high for Long_attr") == 0);

		lg = 1200;
		din << lg;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOSetAttr", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("Status"));
		dout >> status;
		TS_ASSERT(strcmp(status,"The device is in ON state.") == 0);

		state = Tango::ON;
		din << state;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOState", din));
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("State"));
		dout >> state;
		TS_ASSERT(state == Tango::ON);
		TS_ASSERT_THROWS_NOTHING(dout = device1->command_inout("Status"));
		dout >> status;
		TS_ASSERT(strcmp(status,"The device is in ON state.") == 0);
	}

	void set_Long_attr_value(DevLong value)
	{
		DeviceData input;
		input << value;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOSetAttr", input));
	}

	void set_attribute_exception_flag(short attribute_disc, bool enabled)
	{
		std::vector<short> flags(2);
		flags[0] = attribute_disc;
		flags[1] = enabled ? 1 : 0;
		DeviceData data;
		data << flags;
		TS_ASSERT_THROWS_NOTHING(device1->command_inout("IOAttrThrowEx", data));
	}

	void __assert_dev_state(DevState expected, const char* file, const char* line)
	{
		std::string message = std::string("Called from ") + file + ":" + line;

		DevState state;
		DeviceData data;
		TSM_ASSERT_THROWS_NOTHING(message, data = device1->command_inout("State"));
		data >> state;
		TSM_ASSERT_EQUALS(message, expected, state);
	}

#define __QUOTE(x) #x
#define QUOTE(x) __QUOTE(x)
#define assert_dev_state(expected) __assert_dev_state(expected, __FILE__, QUOTE(__LINE__))

// Verifies that device state is set correctly when alarm is configured for an attribute
// but no value is provided for this attribute in user callback (e.g. an exception is thrown).

	void test_alarm_on_attribute_exception_during_read(void)
	{
		const short EXCEPTION_IN_Long_attr = 5;

		set_Long_attr_value(2000);
		assert_dev_state(Tango::ALARM);

		set_attribute_exception_flag(EXCEPTION_IN_Long_attr, true);
		assert_dev_state(Tango::ON);

		set_attribute_exception_flag(EXCEPTION_IN_Long_attr, false);
		assert_dev_state(Tango::ALARM);

		set_Long_attr_value(1200);
		assert_dev_state(Tango::ON);
	}

#undef assert_dev_state
#undef QUOTE
#undef __QUOTE

};
#undef cout
#endif // AttrMiscTestSuite_h
