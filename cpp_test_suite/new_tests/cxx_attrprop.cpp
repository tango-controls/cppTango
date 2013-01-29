#ifndef AttrProp_h
#define AttrProp_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME AttrPropTestSuite

class AttrPropTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy *device1;

public:
	SUITE_NAME()
	{

//
// Arguments check -------------------------------------------------
//

		string device1_name;

		// user arguments, obtained from the command line sequentially
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

//
// Tests -------------------------------------------------------
//

// Test AttrProp template specialization for all Tango types

	void test_AttrProp_template_specialization_for_all_Tango_types(void)
	{
//		AttrProp<string> aps;

		AttrProp<DevBoolean> apbool;
		AttrProp<DevDouble> apdb;
		AttrProp<DevEncoded> apenc;
		AttrProp<DevFloat> apfl;
		AttrProp<DevLong> aplg;
		AttrProp<DevLong64> aplg64;
		AttrProp<DevShort> apsh;
		AttrProp<DevState> apst;
		AttrProp<DevString> apstr;
		AttrProp<DevUChar> apych;
		AttrProp<DevULong> apulg;
		AttrProp<DevULong64> apulg64;
		AttrProp<DevUShort> apush;

//		char * abc = "abc";
//		DevString def;// = "def";
//		AttrProp<DevString> aps(DevString("aaa"));
//		AttrProp<string> aps;
//		aps = def;//"Hello world";
//		aps = "Hello world";
//		AttrProp<DevString> aps2(string("Hai"));
//		MultiAttrProp<DevString> map;
//		AttrProp<short> apsh(2);
	}

// Test AttrProp template specialization for all Tango types

	void test_DoubleAttrProp_template_specialization_for_all_Tango_types(void)
	{
		DoubleAttrProp<DevBoolean> dapbool;
		DoubleAttrProp<DevDouble> dapdb;
		DoubleAttrProp<DevEncoded> dapenc;
		DoubleAttrProp<DevFloat> dapfl;
		DoubleAttrProp<DevLong> daplg;
		DoubleAttrProp<DevLong64> daplg64;
		DoubleAttrProp<DevShort> dapsh;
		DoubleAttrProp<DevState> dapst;
		DoubleAttrProp<DevString> dapstr;
		DoubleAttrProp<DevUChar> dapych;
		DoubleAttrProp<DevULong> dapulg;
		DoubleAttrProp<DevULong64> dapulg64;
		DoubleAttrProp<DevUShort> dapush;
	}

// Test MultiAttrProp template specialization for all Tango types

	void test_MultiAttrProp_template_specialization_for_all_Tango_types(void)
	{
		MultiAttrProp<DevBoolean> mapbool;
		MultiAttrProp<DevDouble> mapdb;
		MultiAttrProp<DevEncoded> mapenc;
		MultiAttrProp<DevFloat> mapfl;
		MultiAttrProp<DevLong> maplg;
		MultiAttrProp<DevLong64> maplg64;
		MultiAttrProp<DevShort> mapsh;
		MultiAttrProp<DevState> mapst;
		MultiAttrProp<DevString> mapstr;
		MultiAttrProp<DevUChar> mapych;
		MultiAttrProp<DevULong> mapulg;
		MultiAttrProp<DevULong64> mapulg64;
		MultiAttrProp<DevUShort> mapush;
	}
};
#undef cout
#endif // AttrProp_h
