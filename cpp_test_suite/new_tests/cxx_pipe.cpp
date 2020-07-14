#ifndef PipeTestSuite_h
#define PipeTestSuite_h

#include "cxx_common.h"

#undef SUITE_NAME
#define SUITE_NAME PipeTestSuite

class PipeTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy 			*device1;
	string 					device1_name;
	DeviceProxy 			*root_admin;

	bool					verbose;

public:
	SUITE_NAME()
	{
//
// Arguments check -------------------------------------------------
//

		string full_ds_name;

		// user arguments, obtained from the command line sequentially
		device1_name = CxxTest::TangoPrinter::get_param("device1");
		full_ds_name = CxxTest::TangoPrinter::get_param("fulldsname");

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

			string root_adm_name("dserver/");
			root_adm_name = root_adm_name + full_ds_name;
			root_admin = new DeviceProxy(root_adm_name);

			DeviceData d_in;
			d_in << (short)0;
			device1->command_inout("SetPipeOutput",d_in);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
	}

	virtual ~SUITE_NAME()
	{
		DeviceData d_in;
		d_in << Tango::ON;
		device1->command_inout("IOState",d_in);

		delete device1;
		delete root_admin;
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


// Test pipe reading

	void test_reading_pipe_with_extraction_using_DataElement(void)
	{
		DevicePipe pipe_data = device1->read_pipe("rPipe");

		DataElement<DevLong> de_dl;
		DataElement<vector<double> > de_v_db;
		DataElement<DevVarUShortArray *> de_dvush(new DevVarUShortArray());
		DataElement<vector<DevState> > de_v_sta;

		size_t de_nb = pipe_data.get_data_elt_nb();
		TS_ASSERT(de_nb == 4);

    	pipe_data >> de_dl >> de_v_db >> de_dvush >> de_v_sta;

		TS_ASSERT(pipe_data.get_root_blob_name() == "BlobCase0");

		TS_ASSERT(de_dl.name == "FirstDE");
		TS_ASSERT(de_dl.value == 666);

		TS_ASSERT(de_v_db.name == "SecondDE");
		TS_ASSERT(de_v_db.value.size() == 2);
		TS_ASSERT(de_v_db.value[0] == 1.11);
		TS_ASSERT(de_v_db.value[1] == 2.22);

		TS_ASSERT(de_dvush.name == "ThirdDE");
		TS_ASSERT(de_dvush.value->length() == 100);
		TS_ASSERT((*de_dvush.value)[0] == 0);
		TS_ASSERT((*de_dvush.value)[1] == 1);
		TS_ASSERT((*de_dvush.value)[99] == 99);

		TS_ASSERT(de_v_sta.name == "ForthDE");
		TS_ASSERT(de_v_sta.value.size() == 2);
		TS_ASSERT(de_v_sta.value[0] == ON);
		TS_ASSERT(de_v_sta.value[1] == OFF);

		delete de_dvush.value;
	}

	void test_reading_pipe_with_simple_extraction(void)
	{
		DevicePipe pipe_data = device1->read_pipe("rPipe");

		DevLong dl;
		vector<double> v_db;
		DevVarUShortArray *dvush = new DevVarUShortArray();
		vector<DevState> v_sta;

		size_t de_nb = pipe_data.get_data_elt_nb();
		vector<string> de_names = pipe_data.get_data_elt_names();

		TS_ASSERT(pipe_data.get_root_blob_name() == "BlobCase0");
		TS_ASSERT(de_nb == 4);

		TS_ASSERT(de_names.size() == 4);
		TS_ASSERT(de_names[0] == "FirstDE");
		TS_ASSERT(de_names[1] == "SecondDE");
		TS_ASSERT(de_names[2] == "ThirdDE");
		TS_ASSERT(de_names[3] == "ForthDE");

		int type = pipe_data.get_data_elt_type(0);
		TS_ASSERT(type == DEV_LONG);

		type = pipe_data.get_data_elt_type(1);
		TS_ASSERT(type == DEVVAR_DOUBLEARRAY);

		type = pipe_data.get_data_elt_type(2);
		TS_ASSERT(type == DEVVAR_USHORTARRAY);

		type = pipe_data.get_data_elt_type(3);
		TS_ASSERT(type == DEVVAR_STATEARRAY);

    	pipe_data >> dl >> v_db >> dvush >> v_sta;

		TS_ASSERT(dl == 666);

		TS_ASSERT(v_db.size() == 2);
		TS_ASSERT(v_db[0] == 1.11);
		TS_ASSERT(v_db[1] == 2.22);

		TS_ASSERT(dvush->length() == 100);
		TS_ASSERT((*dvush)[0] == 0);
		TS_ASSERT((*dvush)[1] == 1);
		TS_ASSERT((*dvush)[99] == 99);

		TS_ASSERT(v_sta.size() == 2);
		TS_ASSERT(v_sta[0] == ON);
		TS_ASSERT(v_sta[1] == OFF);

		delete dvush;
	}

	void test_reading_pipe_with_ulong64_and_boolean_array(void)
	{
		DeviceData d_in;
		d_in << (short)11;
		device1->command_inout("SetPipeOutput",d_in);

		DevicePipe pipe_data = device1->read_pipe("rPipe");

		DevULong64 udl64;
		vector<Tango::DevBoolean> v_bool;

		size_t de_nb = pipe_data.get_data_elt_nb();
		vector<string> de_names = pipe_data.get_data_elt_names();

		TS_ASSERT(pipe_data.get_root_blob_name() == "BlobCase11");
		TS_ASSERT(de_nb == 2);

		TS_ASSERT(de_names.size() == 2);
		TS_ASSERT(de_names[0] == "1DE");
		TS_ASSERT(de_names[1] == "2DE");

		int type = pipe_data.get_data_elt_type(0);
		TS_ASSERT(type == DEV_ULONG64);

		type = pipe_data.get_data_elt_type(1);
		TS_ASSERT(type == DEVVAR_BOOLEANARRAY);

    	pipe_data >> udl64 >> v_bool;

		TS_ASSERT(udl64 == 123456);

		TS_ASSERT(v_bool.size() == 3);
		TS_ASSERT(v_bool[0] == true);
		TS_ASSERT(v_bool[1] == true);
		TS_ASSERT(v_bool[2] == false);
	}

	void test_reading_pipe_with_arrays_length_1(void)
	{
		DeviceData d_in;
		d_in << (short)12;
		device1->command_inout("SetPipeOutput",d_in);

		DevicePipe pipe_data = device1->read_pipe("rPipe");

		vector<Tango::DevBoolean> v_bool;
		vector<Tango::DevLong> v_dl;

		size_t de_nb = pipe_data.get_data_elt_nb();
		vector<string> de_names = pipe_data.get_data_elt_names();

		TS_ASSERT(pipe_data.get_root_blob_name() == "BlobCase12");
		TS_ASSERT(de_nb == 2);

		TS_ASSERT(de_names.size() == 2);
		TS_ASSERT(de_names[0] == "1DE_Array");
		TS_ASSERT(de_names[1] == "2DE_Array");

		int type = pipe_data.get_data_elt_type(0);
		TS_ASSERT(type == DEVVAR_LONGARRAY);

		type = pipe_data.get_data_elt_type(1);
		TS_ASSERT(type == DEVVAR_BOOLEANARRAY);

    	pipe_data >> v_dl >> v_bool;

		TS_ASSERT(v_dl.size() == 1);
		TS_ASSERT(v_dl[0] == 9);

		TS_ASSERT(v_bool.size() == 1);
		TS_ASSERT(v_bool[0] == false);
	}

	void test_reading_pipe_with_string_types(void)
	{
		DeviceData d_in;
		d_in << (short)1;
		device1->command_inout("SetPipeOutput",d_in);

		DevicePipe pipe_data = device1->read_pipe("rPipe");

		size_t de_nb = pipe_data.get_data_elt_nb();
		vector<string> de_names = pipe_data.get_data_elt_names();

		TS_ASSERT(pipe_data.get_root_blob_name() == "BlobCase1");
		TS_ASSERT(de_nb == 5);

		TS_ASSERT(de_names.size() == 5);
		TS_ASSERT(de_names[0] == "stringDE");
		TS_ASSERT(de_names[1] == "DevStringDE");
		TS_ASSERT(de_names[2] == "DevEncodedDE");
		TS_ASSERT(de_names[3] == "VectorStringDE");
		TS_ASSERT(de_names[4] == "DevVarStringArrayDE");

		string str;
		DevString ds = nullptr;
		DevEncoded enc;
		vector<string> v_str;
		DevVarStringArray *dvsa = new DevVarStringArray();

		pipe_data >> str >> ds >> enc >> v_str >> dvsa;

		TS_ASSERT(str == "Hello");

		string tmp("Hola");
		TS_ASSERT(strcmp(ds,tmp.c_str()) == 0);

		string tmp_enc("Format");
		TS_ASSERT(strcmp(enc.encoded_format,tmp_enc.c_str()) == 0);
		TS_ASSERT(enc.encoded_data.length() == 2);
		TS_ASSERT(enc.encoded_data[0] == 0);
		TS_ASSERT(enc.encoded_data[1] == 1);

		TS_ASSERT(v_str.size() == 3);
		TS_ASSERT(v_str[0] == "Bonjour");
		TS_ASSERT(v_str[1] == "le");
		TS_ASSERT(v_str[2] == "monde");

		TS_ASSERT(dvsa->length() == 1);
		string tmp_dvsa("Why not?");
		TS_ASSERT(strcmp((*dvsa)[0],tmp_dvsa.c_str()) == 0);

		delete dvsa;
		delete [] ds;
	}

	void test_reading_pipe_with_operator_bracket(void)
	{
		DeviceData d_in;
		d_in << (short)2;
		device1->command_inout("SetPipeOutput",d_in);

		DevicePipe pipe_data = device1->read_pipe("rPipe");

		size_t de_nb = pipe_data.get_data_elt_nb();
		vector<string> de_names = pipe_data.get_data_elt_names();

		TS_ASSERT(pipe_data.get_root_blob_name() == "BlobCase2");
		TS_ASSERT(de_nb == 2);

		TS_ASSERT(de_names.size() == 2);
		TS_ASSERT(de_names[0] == "FirstDE");
		TS_ASSERT(de_names[1] == "SecondDE");

		vector<double> v_db;
		DevLong dl;

		pipe_data["SecondDE"] >> v_db;
		pipe_data["FirstDE"] >> dl;

		TS_ASSERT(v_db.size() == 3);
		TS_ASSERT(v_db[0] == 3.33);
		TS_ASSERT(v_db[1] == 4.44);
		TS_ASSERT(v_db[2] == 5.55);

		TS_ASSERT(dl == 999);
	}

	void test_reading_pipe_initialized_with_data_elt(void)
	{
		DevicePipe pipe_data = device1->read_pipe("rPipeDE");

		size_t de_nb = pipe_data.get_data_elt_nb();
		vector<string> de_names = pipe_data.get_data_elt_names();

		TS_ASSERT(pipe_data.get_root_blob_name() == "BlobDE");
		TS_ASSERT(de_nb == 4);

		TS_ASSERT(de_names.size() == 4);
		TS_ASSERT(de_names[0] == "FirstDE");
		TS_ASSERT(de_names[1] == "SecondDE");

		vector<double> v_db;
		DevLong dl;

		pipe_data["SecondDE"] >> v_db;
		pipe_data["FirstDE"] >> dl;

		TS_ASSERT(v_db.size() == 2);
		TS_ASSERT(v_db[0] == 1.11);
		TS_ASSERT(v_db[1] == 2.22);

		TS_ASSERT(dl == 666);
	}

	void test_reading_pipe_with_blob_inside_blob(void)
	{
		DeviceData d_in;
		d_in << (short)3;
		device1->command_inout("SetPipeOutput",d_in);

		DevicePipe pipe_data = device1->read_pipe("rPipe");

		size_t de_nb = pipe_data.get_data_elt_nb();
		vector<string> de_names = pipe_data.get_data_elt_names();

		TS_ASSERT(pipe_data.get_root_blob_name() == "BlobCase3");
		TS_ASSERT(de_nb == 2);

		TS_ASSERT(de_names.size() == 2);
		TS_ASSERT(de_names[0] == "1DE");
		TS_ASSERT(de_names[1] == "2DE");

		TS_ASSERT(pipe_data.get_data_elt_type(0) == DEV_PIPE_BLOB);
		TS_ASSERT(pipe_data.get_data_elt_type(1) == DEVVAR_LONGARRAY);

		vector<DevLong> v_dl;
		pipe_data["2DE"] >> v_dl;

		TS_ASSERT(v_dl.size() == 4);
		TS_ASSERT(v_dl[0] == 3);
		TS_ASSERT(v_dl[1] == 4);
		TS_ASSERT(v_dl[2] == 5);
		TS_ASSERT(v_dl[3] == 6);

		DevicePipeBlob inner_dpb;
		pipe_data["1DE"] >> inner_dpb;

		TS_ASSERT(inner_dpb.get_name() == "Inner");
		TS_ASSERT(inner_dpb.get_data_elt_nb() == 3);
		TS_ASSERT(inner_dpb.get_data_elt_name(0) == "InnerFirstDE");
		TS_ASSERT(inner_dpb.get_data_elt_name(1) == "InnerSecondDE");
		TS_ASSERT(inner_dpb.get_data_elt_name(2) == "InnerThirdDE");

		DevicePipeBlob inner_inner_dpb;
		string inner_str;
		bool inner_bool;

		inner_dpb >> inner_str >> inner_inner_dpb >> inner_bool;

		TS_ASSERT (inner_str == "Grenoble");
		TS_ASSERT (inner_bool = true);

		TS_ASSERT(inner_inner_dpb.get_name() == "InnerInner");
		TS_ASSERT(inner_inner_dpb.get_data_elt_nb() == 2);
		TS_ASSERT(inner_inner_dpb.get_data_elt_name(0) == "InnerInnerFirstDE");
		TS_ASSERT(inner_inner_dpb.get_data_elt_name(1) == "InnerInnerSecondDE");

		DataElement<DevLong> de_dl;
		DataElement<vector<double> > de_v_db;

		inner_inner_dpb >> de_dl >> de_v_db;

		TS_ASSERT(de_dl.name == "InnerInnerFirstDE");
		TS_ASSERT(de_dl.value == 111);

		TS_ASSERT(de_v_db.value.size() == 1);
		TS_ASSERT(de_v_db.value[0] == 3.33);
	}

	void test_reading_pipe_with_error_cases(void)
	{

// Error in extract data

		DeviceData d_in;
		d_in << (short)0;
		device1->command_inout("SetPipeOutput",d_in);

		DevicePipe pipe_data = device1->read_pipe("rPipe");

		DevLong dl, dl_extra;
		vector<double> v_db;
		vector<unsigned short> v_us;
		vector<DevState> v_sta;

		TS_ASSERT_THROWS_ASSERT(pipe_data >> dl >> v_db >> v_us >> v_sta >> dl_extra;, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_PipeWrongArg
								&& e.errors[0].severity == Tango::ERR));

		pipe_data = device1->read_pipe("rPipe");

		TS_ASSERT_THROWS_ASSERT(pipe_data >> dl >> dl_extra;, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_IncompatibleArgumentType
								&& e.errors[0].severity == Tango::ERR));

		d_in << (short)4;
		device1->command_inout("SetPipeOutput",d_in);

		pipe_data = device1->read_pipe("rPipe");

		TS_ASSERT(pipe_data.get_data_elt_nb() == 2);

		TS_ASSERT_THROWS_ASSERT(pipe_data >> dl >> dl;, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_EmptyDataElement
								&& e.errors[0].severity == Tango::ERR));

// Same error with bit test instead of exceptions

		pipe_data = device1->read_pipe("rPipe");

		bitset<DevicePipeBlob::numFlags> bs;
		bs.reset();

		pipe_data.exceptions(bs);

		pipe_data >> dl;
		TS_ASSERT(pipe_data.has_failed() == false);

		pipe_data >> dl;
		TS_ASSERT(pipe_data.has_failed() == true);

		bitset<DevicePipeBlob::numFlags> bs_err = pipe_data.state();
		TS_ASSERT(bs_err.test(DevicePipeBlob::isempty_flag) == true);
		TS_ASSERT(bs_err.test(DevicePipeBlob::wrongtype_flag) == false);
		TS_ASSERT(bs_err.test(DevicePipeBlob::notenoughde_flag) == false);
		TS_ASSERT(bs_err.test(DevicePipeBlob::blobdenamenotset_flag) == false);

		bs.set();
		pipe_data.exceptions(bs);

// Error when device write pipe

		d_in << (short)5;
		device1->command_inout("SetPipeOutput",d_in);

		TS_ASSERT_THROWS_ASSERT(pipe_data = device1->read_pipe("rpipe");, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_PipeValueNotSet
								&& e.errors[0].severity == Tango::ERR));

// Error pipe not found

		TS_ASSERT_THROWS_ASSERT(pipe_data = device1->read_pipe("pi");, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_PipeNotFound
								&& e.errors[0].severity == Tango::ERR));

// Not allowed error

		d_in << Tango::OFF;
		device1->command_inout("IOState",d_in);

		TS_ASSERT_THROWS_ASSERT(pipe_data = device1->read_pipe("rpipe");, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_PipeNotAllowed
								&& e.errors[0].severity == Tango::ERR));

		d_in << Tango::ON;
		device1->command_inout("IOState",d_in);

// Duplicate DE name

		d_in << (short)6;
		device1->command_inout("SetPipeOutput",d_in);

		TS_ASSERT_THROWS_ASSERT(pipe_data = device1->read_pipe("rpipe");, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_PipeDuplicateDEName
								&& e.errors[0].severity == Tango::ERR));

// Duplicate DE name

		d_in << (short)7;
		device1->command_inout("SetPipeOutput",d_in);

		TS_ASSERT_THROWS_ASSERT(pipe_data = device1->read_pipe("rpipe");, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_PipeNoDataElement
								&& e.errors[0].severity == Tango::ERR));

// Mixing insertion method type

		d_in << (short)8;
		device1->command_inout("SetPipeOutput",d_in);

		TS_ASSERT_THROWS_ASSERT(pipe_data = device1->read_pipe("rpipe");, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_NotSupportedFeature
								&& e.errors[0].severity == Tango::ERR));

// Not enough data element

		d_in << (short)10;
		device1->command_inout("SetPipeOutput",d_in);

		TS_ASSERT_THROWS_ASSERT(pipe_data = device1->read_pipe("rpipe");, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_PipeWrongArg
								&& e.errors[0].severity == Tango::ERR));

// Mixing extraction method type

		d_in << (short)0;
		device1->command_inout("SetPipeOutput",d_in);

		pipe_data = device1->read_pipe("rpipe");

		DevLong de_dl;
		vector<double> de_v_db;

    	pipe_data >> de_dl;

		TS_ASSERT_THROWS_ASSERT(pipe_data["SecondDE"] >> de_v_db;, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_NotSupportedFeature
								&& e.errors[0].severity == Tango::ERR));
	}

	void test_writing_pipe(void)
	{
		DevicePipe dp("RwPiPe","WritenBlob");

		vector<string> de_names;
		de_names.push_back(string("aaa"));
		de_names.push_back(string("bbb"));
		string str("Writing pipe");
		vector<float> v_fl;
		v_fl.push_back(1.11);
		v_fl.push_back(8.88);

		dp.set_data_elt_names(de_names);
		dp << str << v_fl;

		TS_ASSERT_THROWS_NOTHING(device1->write_pipe(dp));
	}

	void test_writing_pipe_with_error(void)
	{

// Empty device pipe

		DevicePipe dp("RwPiPe","WritenBlob");

		vector<string> de_names;
		de_names.push_back(string("aaa"));
		de_names.push_back(string("bbb"));
		string str("Writing pipe");
		vector<float> v_fl;
		v_fl.push_back(1.11);
		v_fl.push_back(8.88);

		TS_ASSERT_THROWS_ASSERT(device1->write_pipe(dp);, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_PipeNoDataElement
								&& e.errors[0].severity == Tango::ERR));

// Empty data element

		dp.set_data_elt_names(de_names);

		TS_ASSERT_THROWS_ASSERT(device1->write_pipe(dp);, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_EmptyDataElement
								&& e.errors[0].severity == Tango::ERR));

// Pipe not found

		dp.set_name("XXX");
		dp.set_data_elt_names(de_names);
		dp << str << v_fl;

		TS_ASSERT_THROWS_ASSERT(device1->write_pipe(dp);, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_PipeNotFound
								&& e.errors[0].severity == Tango::ERR));

// Pipe not writable

		dp.set_name("Rpipe");
		dp.set_data_elt_names(de_names);
		dp << str << v_fl;

		TS_ASSERT_THROWS_ASSERT(device1->write_pipe(dp);, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_PipeNotWritable
								&& e.errors[0].severity == Tango::ERR));

// Wrong data sent to pipe

		dp.set_name("RWpipe");
		de_names.clear();
		de_names.push_back("111");
		dp.set_data_elt_names(de_names);
		dp << v_fl;

		TS_ASSERT_THROWS_ASSERT(device1->write_pipe(dp);, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_IncompatibleArgumentType
								&& e.errors[0].severity == Tango::ERR));

		de_names.clear();
		de_names.push_back("111");
		de_names.push_back("222");
		de_names.push_back("333");
		dp.set_data_elt_names(de_names);

		dp << str << str;

		TS_ASSERT_THROWS_ASSERT(device1->write_pipe(dp);, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_IncompatibleArgumentType
								&& e.errors[0].severity == Tango::ERR));
	}

	void test_write_read_pipe(void)
	{
		DevicePipe dp("RwPiPe","WritenBlob");

		vector<string> de_names;
		de_names.push_back("aaaWR");
		de_names.push_back("bbbWR");
		string str("Writing Reading pipe");
		vector<float> v_fl;
		v_fl.push_back(1.11);
		v_fl.push_back(8.88);

		dp.set_data_elt_names(de_names);
		dp << str << v_fl;

		DevicePipe r_dp;
		TS_ASSERT_THROWS_NOTHING(r_dp = device1->write_read_pipe(dp));

		DataElement<DevLong> de_dl;
		DataElement<vector<double> > de_v_db;

		size_t de_nb = r_dp.get_data_elt_nb();
		TS_ASSERT(de_nb == 2);

    	r_dp >> de_dl >> de_v_db;

		TS_ASSERT(r_dp.get_root_blob_name() == "RWPipeBlob");

		TS_ASSERT(de_dl.name == "RW_1DE");
		TS_ASSERT(de_dl.value == 666);

		TS_ASSERT(de_v_db.name == "RW_2DE");
		TS_ASSERT(de_v_db.value.size() == 2);
		TS_ASSERT(de_v_db.value[0] == 1.11);
		TS_ASSERT(de_v_db.value[1] == 2.22);
	}
};


#undef cout
#endif // PipeTestSuite_h
