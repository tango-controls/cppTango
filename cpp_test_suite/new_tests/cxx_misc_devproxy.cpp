//===================================================================================================================
//
// file :		cxx_misc_devproxy.cpp
//
// project :		tango
//
// author(s) :		ingvord
//
// Copyright (C) :      2004-2017
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//===================================================================================================================
#ifndef MiscDevProxyTestSuite_h
#define MiscDevProxyTestSuite_h


#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>

using namespace Tango;
using namespace std;

#define cout cout << "\t"
#define    coutv    if (verbose == true) cout

#undef SUITE_NAME
#define SUITE_NAME MiscDevProxyTestSuite

class MiscDevProxyTestSuite: public CxxTest::TestSuite
{
protected:
    DeviceProxy *device;

    string device_name, ds_name, admin_name;
    bool verbose;

public:
    SUITE_NAME()
    {

//
// Arguments check -------------------------------------------------
//

        device_name = CxxTest::TangoPrinter::get_param("device1");
        ds_name = CxxTest::TangoPrinter::get_param("fulldsname");
        admin_name = "dserver/" + ds_name;

        verbose = CxxTest::TangoPrinter::is_param_defined("verbose");

        CxxTest::TangoPrinter::validate_args();


//
// Initialization --------------------------------------------------
//

        try
        {
            device = new DeviceProxy(device_name);
        }
        catch (CORBA::Exception &e)
        {
            Except::print_exception(e);
            exit(-1);
        }

    }

    virtual ~SUITE_NAME()
    {
        delete device;
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

    void test_get_timeout(void)
    {
        int to;
        TS_ASSERT_THROWS_NOTHING(to = device->get_timeout_millis());

        TS_ASSERT_EQUALS(3000, to);
    }

    // Test set_timeout
    void test_set_get_timeout(void)
    {
        int new_to;
        TS_ASSERT_THROWS_NOTHING(device->set_timeout_millis(2000));
        TS_ASSERT_THROWS_NOTHING(new_to = device->get_timeout_millis());

        TS_ASSERT_EQUALS(2000, new_to);

        TS_ASSERT_THROWS_NOTHING(device->set_timeout_millis(3000));
        TS_ASSERT_THROWS_NOTHING(new_to = device->get_timeout_millis());
        TS_ASSERT_EQUALS(3000, new_to);
    }

    // Test ping
    void test_ping(void)
    {
        int elapsed;
        TS_ASSERT_THROWS_NOTHING(elapsed = device->ping());

        cout << "   Ping ( " << elapsed << " us ) --> OK" << endl;
        ETS_ASSERT_LESS_THAN(0, elapsed);
    }

    // Test state

    void test_state(void)
    {
        DevState sta;
        TS_ASSERT_THROWS_NOTHING(sta = device->state());

        TS_ASSERT_EQUALS(Tango::ON, sta);
    }

    // Test status
    void test_status(void)
    {
        string str;
        TS_ASSERT_THROWS_NOTHING(str = device->status());

        TS_ASSERT_EQUALS("The device is in ON state.", str);
    }

    // Test Tango lib version
    void test_lib_version(void)
    {
        int tg_version;
        TS_ASSERT_THROWS_NOTHING(tg_version = device->get_tango_lib_version());

        TS_ASSERT_LESS_THAN_EQUALS(810, tg_version);
    }

    // Test adm_name
    void test_adm_name(void)
    {
        string str_adm;
        TS_ASSERT_THROWS_NOTHING(str_adm = device->adm_name());

        transform(str_adm.begin(), str_adm.end(), str_adm.begin(), ::tolower);
        transform(admin_name.begin(), admin_name.end(), admin_name.begin(), ::tolower);

        TS_ASSERT_EQUALS(admin_name, str_adm);
    }

    // Test description
    void test_description(void)
    {
        string desc;
        TS_ASSERT_THROWS_NOTHING(desc = device->description());

        TS_ASSERT_EQUALS("A TANGO device", desc);
    }

    // Test name
    void test_name(void)
    {
        string name;
        TS_ASSERT_THROWS_NOTHING(name = device->name());

        TS_ASSERT_EQUALS(device_name, name);
    }

    // Test blackbox
    void test_blackbox(void)
    {
        vector <string> *ptr;
        TS_ASSERT_THROWS_NOTHING(ptr = device->black_box(3));

        TS_ASSERT_EQUALS(3, ptr->size());

        string tmp = (*ptr)[0];
        string::size_type pos, end;
        pos = tmp.find('A');
        end = tmp.find("from");
        string ans = tmp.substr(pos, end - pos);

        TS_ASSERT_EQUALS("Attribute name requested ", ans);

        tmp = (*ptr)[1];
        end = tmp.find("from");
        ans = tmp.substr(pos, end - pos);
        TS_ASSERT_EQUALS("Attribute description requested ", ans);

        tmp = (*ptr)[2];
        end = tmp.find("from");
        ans = tmp.substr(pos, end - pos);
        TS_ASSERT_EQUALS("Attribute adm_name requested ", ans);

        delete ptr;
    }

    // Test info
    void test_info(void)
    {
        DeviceInfo inf;
        TS_ASSERT_THROWS_NOTHING(inf = device->info());
        TS_ASSERT_EQUALS("DevTest", inf.dev_class);

        transform(inf.server_id.begin(), inf.server_id.end(), inf.server_id.begin(), ::tolower);
        transform(ds_name.begin(), ds_name.end(), ds_name.begin(), ::tolower);

        TS_ASSERT_EQUALS(ds_name, inf.server_id);
        TS_ASSERT_EQUALS("Doc URL = http://www.tango-controls.org", inf.doc_url);
        TS_ASSERT_EQUALS("TestDevice", inf.dev_type);
    }

    // Test command_query
    void test_command_query(void)
    {
        DevCommandInfo cmd_info;
        TS_ASSERT_THROWS_NOTHING(cmd_info = device->command_query("IODoubleArray"));

        TS_ASSERT_EQUALS("IODoubleArray", cmd_info.cmd_name);
        TS_ASSERT_EQUALS(DEVVAR_DOUBLEARRAY, cmd_info.in_type);
        TS_ASSERT_EQUALS(DEVVAR_DOUBLEARRAY, cmd_info.out_type);
        TS_ASSERT_EQUALS("Array of double", cmd_info.in_type_desc);
        TS_ASSERT_EQUALS("This array * 2", cmd_info.out_type_desc);
    }

    // Test command_list_query and get_command_list
    void test_command_list_query(void)
    {
        CommandInfoList *cmd_list;
        TS_ASSERT_THROWS_NOTHING(cmd_list = device->command_list_query());
        cout << "cmd list size = " << cmd_list->size() << endl;

        vector <string> *cmd_name_list;
        TS_ASSERT_THROWS_NOTHING(cmd_name_list = device->get_command_list());

        cout << "cmd_name_list size = " << cmd_name_list->size() << endl;

        TS_ASSERT_EQUALS(cmd_name_list->size(), cmd_list->size());
        delete cmd_list;
        delete cmd_name_list;
    }

    void test_get_command_list(void)
    {
        vector <string> *cmd_name_list;
        TS_ASSERT_THROWS_NOTHING(cmd_name_list = device->get_command_list());

        cout << "cmd_name_list size = " << cmd_name_list->size() << endl;
        TS_ASSERT_EQUALS(118, cmd_name_list->size());
//        TS_ASSERT_EQUALS ("FileDb" , (*cmd_list)[0].cmd_name);
//        TS_ASSERT_EQUALS ("Status", (*cmd_list)[87].cmd_name);

        delete cmd_name_list;
    }

    // Test get_attribute_list
    void test_get_attribute_list(void)
    {
        vector <string> *att_list;
        TS_ASSERT_THROWS_NOTHING(att_list = device->get_attribute_list());

        cout << "att_list size = " << att_list->size() << endl;
        TS_ASSERT_EQUALS(102, att_list->size());
//		assert ( (*att_list)[0] == "Short_attr");
//		assert ( (*att_list)[1] == "Long_attr");
//		assert ( (*att_list)[21] == "String_attr_w");

        delete att_list;
    }

    // Test attribute query
    void test_attribute_query(void)
    {
        DeviceAttributeConfig attr_conf;
        TS_ASSERT_THROWS_NOTHING(attr_conf = device->attribute_query("Short_attr"));

        TS_ASSERT_EQUALS(attr_conf.name, "Short_attr");
        TS_ASSERT_EQUALS(attr_conf.data_format, SCALAR);
        TS_ASSERT_EQUALS(attr_conf.data_type, DEV_SHORT);
        TS_ASSERT_EQUALS(attr_conf.description, "No description");
        TS_ASSERT_EQUALS(attr_conf.max_dim_x, 1);
    }

    // Test get_attribute_config
    void test_get_attribute_config(void)
    {
        AttributeInfoList *attr_conf_ptr;
        vector <string> li;
        li.push_back("Long_attr");
        li.push_back("Double_attr");

        TS_ASSERT_THROWS_NOTHING(attr_conf_ptr = device->get_attribute_config(li));

        TS_ASSERT_EQUALS(attr_conf_ptr->size(), 2);

        TS_ASSERT_EQUALS((*attr_conf_ptr)[0].name, "Long_attr");
        TS_ASSERT_EQUALS((*attr_conf_ptr)[0].data_format, SCALAR);
        TS_ASSERT_EQUALS((*attr_conf_ptr)[0].data_type, DEV_LONG);

        TS_ASSERT_EQUALS((*attr_conf_ptr)[1].name, "Double_attr");
        TS_ASSERT_EQUALS((*attr_conf_ptr)[1].data_format, SCALAR);
        TS_ASSERT_EQUALS((*attr_conf_ptr)[1].data_type, DEV_DOUBLE);

        delete attr_conf_ptr;
    }

    //  Test get_command_config
    void test_get_command_config(void)
    {
        CommandInfoList *cmd_conf_ptr;
        vector <string> li;
        li.push_back("state");
        li.push_back("status");

        TS_ASSERT_THROWS_NOTHING(cmd_conf_ptr = device->get_command_config(li));

        TS_ASSERT_EQUALS(cmd_conf_ptr->size(), 2);

        TS_ASSERT_EQUALS((*cmd_conf_ptr)[0].cmd_name, "State");
        TS_ASSERT_EQUALS((*cmd_conf_ptr)[0].in_type, DEV_VOID);
        TS_ASSERT_EQUALS((*cmd_conf_ptr)[0].out_type, DEV_STATE);

        TS_ASSERT_EQUALS((*cmd_conf_ptr)[1].cmd_name, "Status");
        TS_ASSERT_EQUALS((*cmd_conf_ptr)[1].in_type, DEV_VOID);
        TS_ASSERT_EQUALS((*cmd_conf_ptr)[1].out_type, DEV_STRING);

    }

    // test attribute_list_query

//		AttributeInfoList *attr_confs;
//		attr_confs = device->attribute_list_query();

//		assert ( attr_confs->size() == 77 );
//		assert ( (*attr_confs)[0].name == "Short_attr");
//		assert ( (*attr_confs)[0].data_format == SCALAR);
//		assert ( (*attr_confs)[0].data_type == DEV_SHORT);

//		assert ( (*attr_confs)[1].name == "Long_attr");
//		assert ( (*attr_confs)[1].data_type == DEV_LONG);
//		assert ( (*attr_confs)[1].data_format == SCALAR);

//		assert ( (*attr_confs)[21].name == "String_attr_w");
//		assert ( (*attr_confs)[21].data_type == DEV_STRING);
//		assert ( (*attr_confs)[21].data_format == SCALAR);

//		cout << "   Attribute list query --> OK "  << endl;
//		delete attr_confs;

    // Test set_attribute_config
    void test_set_attribute_config(void)
    {
        AttributeInfoList v_conf;
        AttributeInfo co;
        TS_ASSERT_THROWS_NOTHING(co = device->attribute_query("Short_attr"));

#ifdef WIN32
        int pid = _getpid();
#else
        pid_t pid = getpid();
#endif

        stringstream st;

        string s;
        st << pid;
        st >> s;

        co.format = s;
        v_conf.push_back(co);

        TS_ASSERT_THROWS_NOTHING(device->set_attribute_config(v_conf));

        DeviceAttributeConfig res = device->attribute_query("Short_attr");
        TS_ASSERT_EQUALS(res.format, s);

    }

    // Test device version
    void test_device_version(void)
    {
#ifndef COMPAT
        int vers;
        TS_ASSERT_THROWS_NOTHING(vers = device->get_idl_version());
        TS_ASSERT_EQUALS(vers, 6);
#endif
    }

    // Test source
    void test_source(void)
    {
        Tango::DevSource so;
        TS_ASSERT_THROWS_NOTHING(so = device->get_source());
        TS_ASSERT_EQUALS(so, Tango::CACHE_DEV);

        TS_ASSERT_THROWS_NOTHING(device->set_source(Tango::DEV));
        TS_ASSERT_THROWS_NOTHING(so = device->get_source());
        TS_ASSERT_EQUALS(so, Tango::DEV);

        TS_ASSERT_THROWS_NOTHING(device->set_source(Tango::CACHE_DEV));
    }

    // Test get property list
    void test_get_property_list(void)
    {
#ifndef COMPAT
        vector <string> props;
        TS_ASSERT_THROWS_NOTHING(device->get_property_list("*", props));

        cout << "NB prop = " << props.size() << endl;
        for (unsigned long l = 0; l < props.size(); l++)
            cout << "prop = " << props[l] << endl;
        TS_ASSERT_EQUALS(props.size(), 3);
        TS_ASSERT_EQUALS(props[0], "cmd_min_poll_period");
        TS_ASSERT_EQUALS(props[1], "min_poll_period");
        TS_ASSERT_EQUALS(props[2], "tst_property");
#endif
    }
};

#undef cout
#endif // MiscDevProxyTestSuite_h
