#ifndef FwdTestSuite_h
#define FwdTestSuite_h

#include "cxx_common.h"

#define coutv_cb 	if (parent->verbose == true) cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME FwdAttTestSuite

class FwdAttTestSuite: public CxxTest::TestSuite
{
protected:
	class EventCallBack : public Tango::CallBack
	{
	public:
		EventCallBack(FwdAttTestSuite *ptr):parent(ptr) {}
		void push_event(Tango::EventData*);

		int 		cb_executed;
		int 		cb_err;
		DevShort 	val;
		string		ev_name;

	private:
		FwdAttTestSuite	*parent;
	};

	DeviceProxy 			*device1,*device2,*fwd_device;
	string 					device1_name,device2_name;
	AttributeInfoListEx 	*confs_root_init;
	AttributeInfoListEx 	*confs_init;
	AttributeInfoListEx 	*confs_root;
	AttributeInfoListEx 	*confs;
	string 					fwd_device_name;
	DeviceProxy 			*root_admin,*ad;

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
		device2_name = CxxTest::TangoPrinter::get_param("device2");
		full_ds_name = CxxTest::TangoPrinter::get_param("fulldsname");
		fwd_device_name = CxxTest::TangoPrinter::get_param_loc("fwd_device");

		verbose = CxxTest::TangoPrinter::is_param_set("verbose");

		// always add this line, otherwise arguments will not be parsed correctly
		CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

		confs_init = NULL;
		confs_root_init = NULL;

		try
		{
			device1 = new DeviceProxy(device1_name);
			device1->ping();

			device2 = new DeviceProxy(device2_name);
			device2->ping();

			fwd_device = new DeviceProxy(fwd_device_name);
			fwd_device->ping();

			string adm_name = fwd_device->adm_name();
			ad = new DeviceProxy(adm_name);

			string root_adm_name("dserver/");
			root_adm_name = root_adm_name + full_ds_name;
			root_admin = new DeviceProxy(root_adm_name);
			root_admin->command_inout("RestartServer");

			Tango_sleep(15);

			vector<string> att;
			att.push_back("fwd_short_rw");
			confs_init = fwd_device->get_attribute_config_ex(att);

			vector<string> att_r;
			att_r.push_back("short_attr_rw");
			confs_root_init = device1->get_attribute_config_ex(att_r);
		}
		catch (Tango::DevFailed &e)
		{
			cerr << "cxx_fwd_att.cpp test suite - DevFailed exception" << endl;
			Except::print_exception(e);
			string reason(e.errors[0].reason);
			if (reason == API_AttrNotFound)
			{
				string status = fwd_device->status();
				cout << "Forward device status = " << status << endl;
			}
			exit(-1);
		}
		catch (CORBA::Exception &e)
		{
			cerr << "cxx_fwd_att.cpp test suite - CORBA exception" << endl;
			Except::print_exception(e);
			exit(-1);
		}
	}

	virtual ~SUITE_NAME()
	{
		if(CxxTest::TangoPrinter::is_restore_set("fwd_att_conf"))
		{
			Tango::Database db;

			Tango::DbDatum fwd_att("fwd_short_rw");
			Tango::DbDatum root_att("__root_att");
			fwd_att << (short)1;
			string r_name = device1_name + "/short_attr_rw";;
			root_att << r_name;
			Tango::DbData dd;
			dd.push_back(fwd_att);
			dd.push_back(root_att);

			db.put_device_attribute_property(fwd_device_name,dd);
		}

		if(CxxTest::TangoPrinter::is_restore_set("poll_root"))
		{
			DevVarStringArray rem_attr_poll;
			DeviceData din;
			rem_attr_poll.length(3);

			rem_attr_poll[0] = device1_name.c_str();
			rem_attr_poll[1] = "attribute";
			rem_attr_poll[2] = "short_attr_rw";
			din << rem_attr_poll;
			try
			{
				root_admin->command_inout("RemObjPolling", din);
			}
			catch (Tango::DevFailed &e) {}
		}

		(*confs_init)[0].label = "";
		(*confs_init)[0].description = "";
		(*confs_root_init)[0].label = "";
		(*confs_root_init)[0].min_value = "";

		device1->set_attribute_config(*confs_root_init);
		fwd_device->set_attribute_config(*confs_init);

		delete device1;
		delete device2;
		delete fwd_device;
		delete root_admin;
		delete ad;

		delete confs_init;
		delete confs_root_init;
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

// Test some error case in forwarded attribute configuration

	void test_error_case_in_forwarded_attribute_configuration()
	{
		Tango::Database db;

		Tango::DbDatum fwd_att("fwd_short_rw");
		Tango::DbDatum root_att("__root_att");
		fwd_att << (short)1;
		root_att << "devtest/10/short_attr_rw";
		Tango::DbData dd;
		dd.push_back(fwd_att);
		dd.push_back(root_att);

		// A wrong root attribute configuration in db (wrong name syntax)

		db.put_device_attribute_property(fwd_device_name,dd);
		CxxTest::TangoPrinter::restore_set("fwd_att_conf");

		ad->command_inout("RestartServer");
		Tango_sleep(4);

		Tango::DevState ds = fwd_device->state();
		string dev_status = fwd_device->status();

		TS_ASSERT(ds == Tango::ALARM);
		string::size_type pos = dev_status.find("Error: Wrong syntax in root attribute definition");
		TS_ASSERT(pos != string::npos);

		// A wrong root attribute configuration in db (root device is local device)

		string r_name(fwd_device_name);
		r_name = r_name + "/short_attr_rw";
		dd[1] << r_name;
		db.put_device_attribute_property(fwd_device_name,dd);

		ad->command_inout("RestartServer");
		Tango_sleep(4);

		ds = fwd_device->state();
		dev_status = fwd_device->status();

		TS_ASSERT(ds == Tango::ALARM);
		pos = dev_status.find("Error: Root device is local device");
		TS_ASSERT(pos != string::npos);

		// A wrong root attribute configuration in db (unknown root device)

		dd[1] << "a/b/c/short_attr_rw";
		db.put_device_attribute_property(fwd_device_name,dd);

		ad->command_inout("RestartServer");
		Tango_sleep(4);

		ds = fwd_device->state();
		dev_status = fwd_device->status();

		TS_ASSERT(ds == Tango::ALARM);
		pos = dev_status.find("Error: Wrong root device");
		TS_ASSERT(pos != string::npos);

		// Reset root attribute configuration to a correct value

		r_name = device1_name;
		r_name = r_name + "/short_attr_rw";
		dd[1] << r_name;
		db.put_device_attribute_property(fwd_device_name,dd);

		ad->command_inout("RestartServer");
		Tango_sleep(4);

		ds = fwd_device->state();
		dev_status = fwd_device->status();

		TS_ASSERT(ds == Tango::ON);
		pos = dev_status.find("The device is in ON state");
		TS_ASSERT(pos != string::npos);
		CxxTest::TangoPrinter::restore_unset("fwd_att_conf");
	}


// Test attribute reading

	void test_reading_forwarded_attribute(void)
	{
		DeviceAttribute short_attr, string_attr, sp_db, ima_str;

		// read attributes

		TS_ASSERT_THROWS_NOTHING(short_attr = fwd_device->read_attribute("fwd_short_rw"));
		DevShort sh;
		short_attr >> sh;
		TS_ASSERT(short_attr.name == "fwd_short_rw");
		TS_ASSERT(short_attr.quality == Tango::ATTR_VALID);
		TS_ASSERT(short_attr.dim_x == 1);
		TS_ASSERT(short_attr.dim_y == 0);
		TS_ASSERT(sh == 66);

		TS_ASSERT_THROWS_NOTHING(string_attr = fwd_device->read_attribute("fwd_string_w"));
		string str;
		string_attr >> str;
		TS_ASSERT(string_attr.name == "fwd_string_w");
		TS_ASSERT(string_attr.quality == Tango::ATTR_VALID);
		TS_ASSERT(string_attr.dim_x == 1);
		TS_ASSERT(string_attr.dim_y == 0);
		TS_ASSERT(str == "Not initialised");

		TS_ASSERT_THROWS_NOTHING(sp_db = fwd_device->read_attribute("fwd_spec_double"));
		vector<double> v_db;
		sp_db >> v_db;
		TS_ASSERT(sp_db.name == "fwd_spec_double");
		TS_ASSERT(sp_db.quality == Tango::ATTR_VALID);
		TS_ASSERT(sp_db.dim_x == 2);
		TS_ASSERT(sp_db.dim_y == 0);
		TS_ASSERT(v_db.size() == 2);
		TS_ASSERT(v_db[0] == 1.11);
		TS_ASSERT(v_db[1] == 2.22);

		TS_ASSERT_THROWS_NOTHING(ima_str = fwd_device->read_attribute("fwd_ima_string_rw"));
		vector<string> v_str;
		ima_str >> v_str;
		TS_ASSERT(ima_str.name == "fwd_ima_string_rw");
		TS_ASSERT(ima_str.quality == Tango::ATTR_VALID);
		TS_ASSERT(ima_str.dim_x == 2);
		TS_ASSERT(ima_str.dim_y == 1);
		TS_ASSERT(v_str.size() == 3);
		TS_ASSERT(v_str[0] == "Alors la, pour une surprise");
		TS_ASSERT(v_str[1] == "c'est une surprise");
		TS_ASSERT(v_str[2] == "Not initialised");
	}

// Test attribute writing

	void test_writing_forwarded_attribute(void)
	{

// First writable attribute

		Tango::DevShort sh=33;
		DeviceAttribute da_sh("fwd_short_rw",sh);
		fwd_device->write_attribute(da_sh);

		Tango::DeviceAttribute da_sh_read = fwd_device->read_attribute("fwd_short_rw");
		Tango::DevShort sh_read;
		da_sh_read >> sh_read;

		TS_ASSERT(sh_read == 33);

		Tango::DeviceAttribute da_sh_read_root = device1->read_attribute("short_attr_rw");
		vector<Tango::DevShort> v_sh_read;
		da_sh_read_root >> v_sh_read;

		TS_ASSERT(v_sh_read.size() == 2);
		TS_ASSERT(v_sh_read[0] == 33);
		TS_ASSERT(v_sh_read[1] == 33);

// Second writable attribute

		string str("Hello");
		DeviceAttribute da_str("fwd_string_w",str);
		fwd_device->write_attribute(da_str);

		Tango::DeviceAttribute da_str_read = fwd_device->read_attribute("fwd_string_w");
		string str_read;
		da_str_read >> str_read;

		TS_ASSERT(str_read == "Hello");

		Tango::DeviceAttribute da_str_read_root = device1->read_attribute("string_attr_w2");
		string str_read_root;
		da_str_read_root >> str_read_root;

		TS_ASSERT(str_read_root == "Hello");

// Third writable attribute

		vector<string> v_str;
		v_str.push_back("Tango");
		DeviceAttribute da_v_str("fwd_ima_string_rw",v_str,1,1);
		fwd_device->write_attribute(da_v_str);

		Tango::DeviceAttribute da_v_str_read = fwd_device->read_attribute("fwd_ima_string_rw");
		vector<string> v_str_read;
		da_v_str_read >> v_str_read;

		TS_ASSERT(v_str_read.size() == 3);
		TS_ASSERT(v_str_read[0] == "Alors la, pour une surprise");
		TS_ASSERT(v_str_read[1] == "c'est une surprise");
		TS_ASSERT(v_str_read[2] == "Tango");

		Tango::DeviceAttribute da_v_str_read_root = device2->read_attribute("string_ima_attr_rw");
		vector<string> v_str_read_root;
		da_v_str_read_root >> v_str_read_root;

		TS_ASSERT(v_str_read_root.size() == 3);
		TS_ASSERT(v_str_read_root[0] == "Alors la, pour une surprise");
		TS_ASSERT(v_str_read_root[1] == "c'est une surprise");
		TS_ASSERT(v_str_read_root[2] == "Tango");
	}

// Test data propagation

	void test_data_propagation(void)
	{

// First writable attribute

		Tango::DevShort sh=44;
		DeviceAttribute da_sh("short_attr_rw",sh);
		device1->write_attribute(da_sh);

		Tango::DeviceAttribute da_sh_read = fwd_device->read_attribute("fwd_short_rw");
		Tango::DevShort sh_read;
		da_sh_read >> sh_read;

		TS_ASSERT(sh_read == 44);

// Second writable attribute

		string str("Hola");
		DeviceAttribute da_str("string_attr_w2",str);
		device1->write_attribute(da_str);

		Tango::DeviceAttribute da_str_read = fwd_device->read_attribute("fwd_string_w");
		string str_read;
		da_str_read >> str_read;

		TS_ASSERT(str_read == "Hola");

// Third writable attribute

		vector<string> v_str;
		v_str.push_back("Samba");
		DeviceAttribute da_v_str("string_ima_attr_rw",v_str,1,1);
		device2->write_attribute(da_v_str);

		Tango::DeviceAttribute da_v_str_read = fwd_device->read_attribute("fwd_ima_string_rw");
		vector<string> v_str_read;
		da_v_str_read >> v_str_read;

		TS_ASSERT(v_str_read.size() == 3);
		TS_ASSERT(v_str_read[0] == "Alors la, pour une surprise");
		TS_ASSERT(v_str_read[1] == "c'est une surprise");
		TS_ASSERT(v_str_read[2] == "Samba");
	}

// Test attribute configuration

	void test_attribute_configuration()
	{
		vector<string> att_names;
		att_names.push_back("fwd_short_rw");
		att_names.push_back("fwd_string_w");
		att_names.push_back("fwd_spec_double");
		att_names.push_back("fwd_ima_string_rw");

		AttributeInfoListEx *confs = fwd_device->get_attribute_config_ex(att_names);

		TS_ASSERT((*confs)[0].name == "fwd_short_rw");
		string tango_host(getenv("TANGO_HOST"));
		string local_root_base("tango://");
		local_root_base = local_root_base + tango_host + "/" +device1_name;
		string local_root = local_root_base + "/short_attr_rw";
		cout << (*confs)[0].root_attr_name << "==" << local_root << endl;
		TS_ASSERT((*confs)[0].root_attr_name == local_root);

		TS_ASSERT((*confs)[0].writable == Tango::READ_WRITE);
		TS_ASSERT((*confs)[0].data_format == Tango::SCALAR);
		TS_ASSERT((*confs)[0].data_type == Tango::DEV_SHORT);
		TS_ASSERT((*confs)[0].max_dim_x == 1);
		TS_ASSERT((*confs)[0].max_dim_y == 0);
		TS_ASSERT((*confs)[0].description == "No description");
		TS_ASSERT((*confs)[0].label == "Gasp a fwd attribute");
		TS_ASSERT((*confs)[0].unit == "Kg");
		TS_ASSERT((*confs)[0].standard_unit == "1.0");
		TS_ASSERT((*confs)[0].display_unit == "No display unit");
		TS_ASSERT((*confs)[0].format == "%d");
		TS_ASSERT((*confs)[0].min_value == "Not specified");
		TS_ASSERT((*confs)[0].max_value == "Not specified");
		TS_ASSERT((*confs)[0].min_alarm == "Not specified");
		TS_ASSERT((*confs)[0].max_alarm == "Not specified");
		TS_ASSERT((*confs)[0].writable_attr_name == "fwd_short_rw");

		TS_ASSERT((*confs)[1].name == "fwd_string_w");
		local_root = local_root_base + "/string_attr_w2";
		TS_ASSERT((*confs)[1].root_attr_name == local_root);

		TS_ASSERT((*confs)[1].writable == Tango::WRITE);
		TS_ASSERT((*confs)[1].data_format == Tango::SCALAR);
		TS_ASSERT((*confs)[1].data_type == Tango::DEV_STRING);
		TS_ASSERT((*confs)[1].max_dim_x == 1);
		TS_ASSERT((*confs)[1].max_dim_y == 0);
		TS_ASSERT((*confs)[1].description == "No description");
		TS_ASSERT((*confs)[1].label == "fwd_string_w");
		TS_ASSERT((*confs)[1].unit == "");
		TS_ASSERT((*confs)[1].standard_unit == "No standard unit");
		TS_ASSERT((*confs)[1].display_unit == "No display unit");
		TS_ASSERT((*confs)[1].format == "%s");
		TS_ASSERT((*confs)[1].min_value == "Not specified");
		TS_ASSERT((*confs)[1].max_value == "Not specified");
		TS_ASSERT((*confs)[1].min_alarm == "Not specified");
		TS_ASSERT((*confs)[1].max_alarm == "Not specified");
		TS_ASSERT((*confs)[1].writable_attr_name == "None");

		TS_ASSERT((*confs)[2].name == "fwd_spec_double");
		local_root = local_root_base + "/double_spec_attr";
		TS_ASSERT((*confs)[2].root_attr_name == local_root);

		TS_ASSERT((*confs)[2].writable == Tango::READ);
		TS_ASSERT((*confs)[2].data_format == Tango::SPECTRUM);
		TS_ASSERT((*confs)[2].data_type == Tango::DEV_DOUBLE);
		TS_ASSERT((*confs)[2].max_dim_x == 3);
		TS_ASSERT((*confs)[2].max_dim_y == 0);
		TS_ASSERT((*confs)[2].description == "No description");
		TS_ASSERT((*confs)[2].label == "fwd_spec_double");
		TS_ASSERT((*confs)[2].unit == "");
		TS_ASSERT((*confs)[2].standard_unit == "No standard unit");
		TS_ASSERT((*confs)[2].display_unit == "No display unit");
		TS_ASSERT((*confs)[2].format == "%6.2f");
		TS_ASSERT((*confs)[2].min_value == "Not specified");
		TS_ASSERT((*confs)[2].max_value == "Not specified");
		TS_ASSERT((*confs)[2].min_alarm == "Not specified");
		TS_ASSERT((*confs)[2].max_alarm == "Not specified");
		TS_ASSERT((*confs)[2].writable_attr_name == "None");

		TS_ASSERT((*confs)[3].name == "fwd_ima_string_rw");
		local_root = "tango://" + tango_host + "/" + device2_name + "/string_ima_attr_rw";
		TS_ASSERT((*confs)[3].root_attr_name == local_root);

		TS_ASSERT((*confs)[3].writable == Tango::READ_WRITE);
		TS_ASSERT((*confs)[3].data_format == Tango::IMAGE);
		TS_ASSERT((*confs)[3].data_type == Tango::DEV_STRING);
		TS_ASSERT((*confs)[3].max_dim_x == 2);
		TS_ASSERT((*confs)[3].max_dim_y == 2);
		TS_ASSERT((*confs)[3].description == "No description");
		TS_ASSERT((*confs)[3].label == "fwd_ima_string_rw");
		TS_ASSERT((*confs)[3].unit == "");
		TS_ASSERT((*confs)[3].standard_unit == "No standard unit");
		TS_ASSERT((*confs)[3].display_unit == "No display unit");
		TS_ASSERT((*confs)[3].format == "%s");
		TS_ASSERT((*confs)[3].min_value == "Not specified");
		TS_ASSERT((*confs)[3].max_value == "Not specified");
		TS_ASSERT((*confs)[3].min_alarm == "Not specified");
		TS_ASSERT((*confs)[3].max_alarm == "Not specified");
		TS_ASSERT((*confs)[3].writable_attr_name == "fwd_ima_string_rw");

		delete confs;
	}

// Test attribute configuration propagation

	void test_attribute_configuration_propagation()
	{
		vector<string> att_names_root;
		att_names_root.push_back("short_attr_rw");
		vector<string> att_names;
		att_names.push_back("fwd_short_rw");
		confs_root = device1->get_attribute_config_ex(att_names_root);

		(*confs_root)[0].description = "houla houla";
		device1->set_attribute_config(*confs_root);

		Tango_sleep(1);

		confs = fwd_device->get_attribute_config_ex(att_names);
		TS_ASSERT((*confs)[0].description == "houla houla");

		(*confs)[0].min_value = "0";
		fwd_device->set_attribute_config(*confs);

		Tango_sleep(1);

		confs_root = device1->get_attribute_config_ex(att_names_root);
		TS_ASSERT((*confs_root)[0].min_value == "0");

		delete confs;
		delete confs_root;
	}

// Label is local

	void test_label_is_local()
	{
		vector<string> att_names_root;
		att_names_root.push_back("short_attr_rw");
		vector<string> att_names;
		att_names.push_back("fwd_short_rw");
		confs_root = device1->get_attribute_config_ex(att_names_root);

		(*confs_root)[0].label = "Ca marche?";
		device1->set_attribute_config(*confs_root);

		confs = fwd_device->get_attribute_config_ex(att_names);
		TS_ASSERT((*confs)[0].label == "Gasp a fwd attribute");

		(*confs)[0].label = "Hai Hai Hai";
		fwd_device->set_attribute_config(*confs);

		confs = fwd_device->get_attribute_config_ex(att_names);
		TS_ASSERT((*confs)[0].label == "Hai Hai Hai");

		confs_root = device1->get_attribute_config_ex(att_names_root);
		TS_ASSERT((*confs_root)[0].label == "Ca marche?");

		delete confs;
		delete confs_root;
	}

// Try a write_read on a fwd attribute

	void test_write_read_on_forwarded_attribute()
	{
		Tango::DevShort sh=22;
		DeviceAttribute da_sh("fwd_short_rw",sh);
		DeviceAttribute da_read = fwd_device->write_read_attribute(da_sh);
		Tango::DevShort read_sh;
		da_read >> read_sh;

		TS_ASSERT(read_sh == 22);

		DeviceAttribute da_root_read = device1->read_attribute("short_attr_rw");
		Tango::DevShort sh_root;
		da_root_read >> sh_root;

		TS_ASSERT(sh_root == 22);
	}

// Polling and forwarded attribute

	void test_polling_on_a_forwarded_attribute()
	{
		DeviceData din, dout;
		DevVarLongStringArray attr_poll;
		attr_poll.lvalue.length(1);
		attr_poll.svalue.length(3);

		const DevVarStringArray *polled_devices;
		TS_ASSERT_THROWS_NOTHING(dout = ad->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices).length() == 0);

		TS_ASSERT_THROWS_NOTHING(dout = root_admin->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices).length() == 0);

		// Impossible to start polling on a forwarded attribute

		attr_poll.lvalue[0] = 300;
		attr_poll.svalue[0] = fwd_device_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "fwd_short_rw";
		din << attr_poll;
		TS_ASSERT_THROWS_ASSERT(ad->command_inout("AddObjPolling",din), Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_NotSupportedFeature
								&& e.errors[0].severity == Tango::ERR));

		// Start polling on root device

		attr_poll.lvalue[0] = 300;
		attr_poll.svalue[0] = device1_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "short_attr_rw";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(root_admin->command_inout("AddObjPolling", din));
		CxxTest::TangoPrinter::restore_set("poll_root");

		Tango_sleep(2);

		// Read data from CACHE

		fwd_device->set_source(CACHE);
		DeviceAttribute da_cache = fwd_device->read_attribute("fwd_short_rw");
		DevShort ds;
		da_cache >> ds;
		TS_ASSERT(ds == 22);

		// Read data history

		vector<DeviceAttributeHistory> *hist;

 		TS_ASSERT_THROWS_NOTHING(hist = fwd_device->attribute_history("fwd_short_rw",5));

 		for (int i = 0;i < 5;i++)
 		{
    		bool fail = (*hist)[i].has_failed();
			TS_ASSERT(fail == false);

			DevShort hist_val;
			(*hist)[i] >> hist_val;
			TS_ASSERT(hist_val == 22);
 		}
 		delete hist;

		// Stop polling

		DevVarStringArray rem_attr_poll;
		rem_attr_poll.length(3);

		rem_attr_poll[0] = device1_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "short_attr_rw";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(root_admin->command_inout("RemObjPolling", din));

		TS_ASSERT_THROWS_NOTHING(dout = root_admin->command_inout("PolledDevice"));
		dout >> polled_devices;
		TS_ASSERT((*polled_devices).length() == 0);

		// Read data from cache when polling on root attribute is stopped fails

		DeviceAttribute da_fail = fwd_device->read_attribute("fwd_short_rw");
		TS_ASSERT_THROWS_ASSERT(da_fail >> ds, Tango::DevFailed &e,
						TS_ASSERT(string(e.errors[0].reason.in()) == API_AttrNotPolled
								&& e.errors[0].severity == Tango::ERR));

	}

// Events and forwarded attributes

	void test_event_on_a_forwarded_attribute()
	{
		int eve_id = 0;
		vector<string> filters;
		EventCallBack cb(this);
		cb.cb_executed = 0;
		cb.cb_err = 0;
		fwd_device->set_source(CACHE_DEV);
		try
		{
			fwd_device->subscribe_event("fwd_short_rw",Tango::PERIODIC_EVENT,&cb);
		}
		catch (Tango::DevFailed &e)
		{
			Tango::Except::print_exception(e);
		}

		TS_ASSERT_THROWS_ASSERT(fwd_device->subscribe_event("fwd_short_rw",Tango::PERIODIC_EVENT,&cb),Tango::DevFailed &e,
							TS_ASSERT(string(e.errors[0].reason.in()) == API_AttributePollingNotStarted
										&& e.errors[0].severity == Tango::ERR));

		// Start polling on root device and subscribe

		DeviceData din, dout;
		DevVarLongStringArray attr_poll;
		attr_poll.lvalue.length(1);
		attr_poll.svalue.length(3);

		attr_poll.lvalue[0] = 300;
		attr_poll.svalue[0] = device1_name.c_str();
		attr_poll.svalue[1] = "attribute";
		attr_poll.svalue[2] = "short_attr_rw";
		din << attr_poll;
		TS_ASSERT_THROWS_NOTHING(root_admin->command_inout("AddObjPolling", din));
		TS_ASSERT_THROWS_NOTHING(eve_id = fwd_device->subscribe_event("fwd_short_rw",Tango::PERIODIC_EVENT,&cb));

		Tango_sleep(3);

		TS_ASSERT(cb.cb_err == 0);
		TS_ASSERT(cb.cb_executed >= 3);

		string at_name = fwd_device_name + "/fwd_short_rw";
		string::size_type pos = cb.ev_name.find(at_name);
		TS_ASSERT(pos != string::npos);

		// unsubscribe from event

		fwd_device->unsubscribe_event(eve_id);

		// Stop polling

		DevVarStringArray rem_attr_poll;
		rem_attr_poll.length(3);

		rem_attr_poll[0] = device1_name.c_str();
		rem_attr_poll[1] = "attribute";
		rem_attr_poll[2] = "short_attr_rw";
		din << rem_attr_poll;
		TS_ASSERT_THROWS_NOTHING(root_admin->command_inout("RemObjPolling", din));
		CxxTest::TangoPrinter::restore_unset("poll_root");
	}

// Locking and forwarded attributes

	void test_locking_device_with_forwarded_attribute()
	{
		fwd_device->lock();

		bool lo = device1->is_locked();
		TS_ASSERT(lo == true);

		lo = device2->is_locked();
		TS_ASSERT(lo == true);

		Tango::Database db;
		DbDevFullInfo fi = db.get_device_info(fwd_device_name);

		LockerInfo li;
		device1->get_locker(li);
		TS_ASSERT(li.ll == Tango::CPP);
		TS_ASSERT(li.li.LockerPid == fi.pid);

		device2->get_locker(li);
		TS_ASSERT(li.ll == Tango::CPP);
		TS_ASSERT(li.li.LockerPid == fi.pid);

		fwd_device->unlock();
		lo = fwd_device->is_locked();
		TS_ASSERT(lo == false);

		lo = device1->is_locked();
		TS_ASSERT(lo == false);

		lo = device2->is_locked();
		TS_ASSERT(lo == false);
	}

	void test_reading_state_forwarded_attribute(void)
	{
		DeviceAttribute state_attr;

		// read attributes
		TS_ASSERT_THROWS_NOTHING(state_attr = fwd_device->read_attribute("fwd_state"));
		DevState the_state;
		if(state_attr >> the_state)
		{
			TS_ASSERT(state_attr.name == "fwd_state");
			TS_ASSERT(state_attr.quality == Tango::ATTR_VALID);
			TS_ASSERT(state_attr.dim_x == 1);
			TS_ASSERT(state_attr.dim_y == 0);
			cout << "the_state = " << the_state << endl;
			TS_ASSERT(the_state == Tango::ON);
		}
		else
		{
			TS_FAIL("Could not extract attribute value to DevState");
		}
	}
};

void FwdAttTestSuite::EventCallBack::push_event(Tango::EventData* event_data)
{
	cb_executed++;
	ev_name = event_data->attr_name;
	try
	{
		coutv_cb << "EventCallBack::push_event(): called attribute " << event_data->attr_name << " event " << event_data->event << endl;
		if (!event_data->err)
		{
			*(event_data->attr_value) >> val;
		}
		else
		{
			cb_err++;
		}
	}
	catch (...)
	{
		coutv_cb << "EventCallBack::push_event(): could not extract data !\n";
	}
}

#undef cout
#endif // FwdTestSuite_h
