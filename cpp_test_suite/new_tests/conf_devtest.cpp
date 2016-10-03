#include <tango.h>

using namespace Tango;

#define CLASS_NAME "DevTest"

void print_changes(string &desc, string &server, DbData &db_data) {
    cout << desc << " -> " << server << " : " << endl;

    DbData::iterator it = db_data.begin();
    while (it != db_data.end()) {
        cout << "   " << it->name;
        for (size_t i = 0; i < it->value_string.size(); i++)
            cout << ((i == 0) ? (" = ") : (" , ")) << it->value_string[i];
//			cout << "   " << ((i == 0) ? ((str << it->value_string[i], (str >> nb_prop && str.eof()) ? " : " : (" = " + it->value_string[i]))) : (" , " + it->value_string[i]));
        cout << endl;
        ++it;
    }
    cout << endl;
}

void print_changes(const char *desc, const char *server, DbData &db_data) {
    string desc_str(desc), server_str(server);
    print_changes(desc_str, server_str, db_data);
}

/*
 * This is a utility that configures properties of test device servers provided as
 * command line parameters. Attribute values, their ranges and other properties are
 * set to defaults used in the test suite.
 */
int main(int argc, char **argv) {
    if (argc < 7) {
        cout << "usage: " << argv[0] << " dserver device1 device2 device3 device1_alias attribute_alias fwd_device" <<
        endl;
        exit(-1);
    }

    string dserver_name = argv[1], device1_name = argv[2], device2_name = argv[3], device3_name = argv[4], device1_alias = argv[5], attribute_alias = argv[6], fwd_dev_name = argv[7];;

    Database *db = new Database();
    DbData db_data;

    long num_prop;
    string str;
    vector<string> str_vec;
    DevLong lg;
    DeviceData din;
    DbDevInfos db_dev_infos;
    DbDevInfo db_dev_info_1, db_dev_info_2, db_dev_info_3;

    //Define device server
    str = dserver_name;
    db_dev_info_1.name = device1_name;
    db_dev_info_1._class = CLASS_NAME;
    db_dev_info_2.name = device2_name;
    db_dev_info_2._class = CLASS_NAME;
    db_dev_info_3.name = device3_name;
    db_dev_info_3._class = CLASS_NAME;
    db_dev_infos.push_back(db_dev_info_1);
    db_dev_infos.push_back(db_dev_info_2);
    db_dev_infos.push_back(db_dev_info_3);

    try {
        db->add_server(str, db_dev_infos);
        for (size_t i = 0; i < db_dev_infos.size(); i++)
            cout << "Added test server : " << str << " -> " << db_dev_infos[i].name << ", class : " <<
            db_dev_infos[i]._class << endl;
        cout << endl;
    }
    catch (...) {
        cout << "Exception: cannot create test server" << endl;
    }


    db_dev_infos.clear();

    //Define device server
    str = "FwdTest/test";
    db_dev_info_1.name = fwd_dev_name;
    db_dev_info_1._class = "FwdTest";
    db_dev_infos.push_back(db_dev_info_1);
    try {
        db->add_server(str, db_dev_infos);
        for (size_t i = 0; i < db_dev_infos.size(); i++)
            cout << "Added test server : " << str << " -> " << db_dev_infos[i].name << ", class : " <<
            db_dev_infos[i]._class << endl;
        cout << endl;
    }
    catch (...) {
        cout << "Exception: cannot create test server" << endl;
    }

    db_dev_infos.clear();
//
// DsCache/test pseudo server (creation & properties)
//

    str = "DsCache/test";
    db_dev_info_1.name = "test/cache1/1";
    db_dev_info_1._class = "CacheTest1";
    db_dev_info_2.name = "test/cache1/2";
    db_dev_info_2._class = "CacheTest1";
    db_dev_info_3.name = "test/cache2/1";
    db_dev_info_3._class = "CacheTest2";
    db_dev_infos.push_back(db_dev_info_1);
    db_dev_infos.push_back(db_dev_info_2);
    db_dev_infos.push_back(db_dev_info_3);

    try {
        db->add_server(str, db_dev_infos);
        for (size_t i = 0; i < db_dev_infos.size(); i++)
            cout << "Added pseudo server : " << str << " -> " << db_dev_infos[i].name << ", class : " <<
            db_dev_infos[i]._class << endl;
        cout << endl;
    }
    catch (...) {
        cout << "Exception: cannot create DsCache/test pseudo server" << endl;
    }

    db_data.clear();
    DbDatum tst_property_1("tst_property");
    lg = 15;
    tst_property_1 << lg;
    db_data.push_back(tst_property_1);

    try {
        db->put_device_property(db_dev_info_1.name, db_data);
        print_changes("Device properties", db_dev_info_1.name.c_str(), db_data);
    }
    catch (...) {
        cout << "Exception: cannot set test/cache1/1 properties" << endl;
    }

    db_data.clear();
    DbDatum tst_property_2("tst_property");
    lg = 25;
    tst_property_2 << lg;
    db_data.push_back(tst_property_2);

    try {
        db->put_device_property(db_dev_info_2.name, db_data);
        print_changes("Device properties", db_dev_info_2.name.c_str(), db_data);
    }
    catch (...) {
        cout << "Exception: cannot set test/cache1/2 properties" << endl;
    }


//
// dserver properties
//

    db_data.clear();
    DbDatum logging_level("logging_level");
    string logging_str = "WARNING";
    logging_level << logging_str;
    db_data.push_back(logging_level);

    try {
        db->put_device_property(dserver_name, db_data);
        print_changes("Dserver properties", dserver_name.c_str(), db_data);
    }
    catch (...) {
        cout << "Exception: cannot set dserver properties" << endl;
    }


//
// Properties common to all devices
//

// class properties

    db_data.clear();
    DbDatum allowed_access_cmd("AllowedAccessCmd");
    str = "IOFloat";
    allowed_access_cmd << str;
    db_data.push_back(allowed_access_cmd);

    try {
        db->put_class_property(CLASS_NAME, db_data);
        print_changes("Class properties", CLASS_NAME, db_data);
    }
    catch (...) {
        cout << "Exception: cannot set class properties" << endl;
    }


// attribute properties defined on class level

    db_data.clear();
    DbDatum class_string_spec_attr("String_spec_attr"), class_string_spec_attr_label(
            "label"), class_string_spec_attr_unit("unit"), class_string_spec_attr_format("format");

    num_prop = 3;
    class_string_spec_attr << num_prop;
    db_data.push_back(class_string_spec_attr);
    str = "Class label";
    class_string_spec_attr_label << str;
    db_data.push_back(class_string_spec_attr_label);
    str = "Class unit";
    class_string_spec_attr_unit << str;
    db_data.push_back(class_string_spec_attr_unit);
    str = "Class format";
    class_string_spec_attr_format << str;
    db_data.push_back(class_string_spec_attr_format);

    DbDatum class_added_short_attr("Added_short_attr"), class_added_short_attr_label("label");

    num_prop = 1;
    class_added_short_attr << num_prop;
    db_data.push_back(class_added_short_attr);
    str = "From db (class)";
    class_added_short_attr_label << str;
    db_data.push_back(class_added_short_attr_label);

    DbDatum class_def_class_attr("DefClassAttr"), class_def_class_attr_description(
            "description"), class_def_class_attr_min_value("min_value"), class_def_class_attr_event_period(
            "event_period"), class_def_class_attr_rel_change("rel_change"), class_def_class_attr_delta_val(
            "delta_val"), class_def_class_attr_delta_t("delta_t");

    num_prop = 6;
    class_def_class_attr << num_prop;
    db_data.push_back(class_def_class_attr);
    str = "Class desc";
    class_def_class_attr_description << str;
    db_data.push_back(class_def_class_attr_description);
    str = "20";
    class_def_class_attr_min_value << str;
    db_data.push_back(class_def_class_attr_min_value);
    str = "500";
    class_def_class_attr_event_period << str;
    db_data.push_back(class_def_class_attr_event_period);
    str = "33,44";
    class_def_class_attr_rel_change << str;
    db_data.push_back(class_def_class_attr_rel_change);
    str = "5";
    class_def_class_attr_delta_val << str;
    db_data.push_back(class_def_class_attr_delta_val);
    str = "2";
    class_def_class_attr_delta_t << str;
    db_data.push_back(class_def_class_attr_delta_t);

    DbDatum class_def_class_user_attr("DefClassUserAttr"), class_def_class_user_attr_description(
            "description"), class_def_class_user_attr_min_value("min_value"), class_def_class_user_attr_event_period(
            "event_period"), class_def_class_user_attr_rel_change("rel_change"), class_def_class_user_attr_delta_val(
            "delta_val"), class_def_class_user_attr_delta_t("delta_t");

    num_prop = 6;
    class_def_class_user_attr << num_prop;
    db_data.push_back(class_def_class_user_attr);
    str = "Class description";
    class_def_class_user_attr_description << str;
    db_data.push_back(class_def_class_user_attr_description);
    str = "20";
    class_def_class_user_attr_min_value << str;
    db_data.push_back(class_def_class_user_attr_min_value);
    str = "500";
    class_def_class_user_attr_event_period << str;
    db_data.push_back(class_def_class_user_attr_event_period);
    str = "33,44";
    class_def_class_user_attr_rel_change << str;
    db_data.push_back(class_def_class_user_attr_rel_change);
    str = "5";
    class_def_class_user_attr_delta_val << str;
    db_data.push_back(class_def_class_user_attr_delta_val);
    str = "2";
    class_def_class_user_attr_delta_t << str;
    db_data.push_back(class_def_class_user_attr_delta_t);

    try {
        db->put_class_attribute_property(CLASS_NAME, db_data);
        print_changes("Class level attribute properties", CLASS_NAME, db_data);
    }
    catch (...) {
        cout << "Exception: cannot set attribute properties defined on class level" << endl;
    }

// attribute properties defined on device level

    db_data.clear();
    DbDatum long_attr("Long_attr"), long_attr_min_alarm("min_alarm"), long_attr_max_alarm("max_alarm");

    num_prop = 2;
    long_attr << num_prop;
    db_data.push_back(long_attr);
    lg = 1000;
    long_attr_min_alarm << lg;
    db_data.push_back(long_attr_min_alarm);
    lg = 1500;
    long_attr_max_alarm << lg;
    db_data.push_back(long_attr_max_alarm);

    for (int i = 2; i < argc; i++) {
        try {
            db->put_device_attribute_property(argv[i], db_data);
            print_changes("Common device properties", argv[i], db_data);
        }
        catch (...) {
            cout << "Exception: cannot set common properties for the device: " << argv[i] << endl;
        }
    }

//
// Device specific properties
//

// device1

    db_data.clear();
    DbDatum added_short_attr("Added_short_attr"), added_short_attr_format("format");

    num_prop = 1;

    added_short_attr << num_prop;
    db_data.push_back(added_short_attr);
    str = "From db (device)";
    added_short_attr_format << str;
    db_data.push_back(added_short_attr_format);

    try {
        db->put_device_attribute_property(device1_name, db_data);
        print_changes("Device specific attribute properties", device1_name.c_str(), db_data);
    }
    catch (...) {
        cout << "Exception: cannot set specific attribute properties for the device: " << device1_name << endl;
    }

    db_data.clear();
    DbDatum min_poll_period("min_poll_period");
    lg = 200;
    min_poll_period << lg;
    db_data.push_back(min_poll_period);

    DbDatum cmd_min_poll_period("cmd_min_poll_period");
    str_vec.push_back("IOExcept");
    str_vec.push_back("500");
    cmd_min_poll_period << str_vec;
    str_vec.clear();
    db_data.push_back(cmd_min_poll_period);

    DbDatum tst_property("tst_property");
    lg = 25;
    tst_property << lg;
    db_data.push_back(tst_property);

    try {
        db->put_device_property(device1_name, db_data);
        print_changes("Device specific properties", device1_name.c_str(), db_data);
    }
    catch (...) {
        cout << "Exception: cannot set specific properties for the device: " << device1_name << endl;
    }

    try {
        db->put_device_alias(device1_name, device1_alias);
        cout << "Device alias -> " << device1_name << " :\n   " << device1_alias << "\n" << endl;
    }
    catch (...) {
        cout << "Exception: cannot set device alias for the device: " << device1_name << endl;
    }

    try {
        str = device1_name + "/" + "Short_attr";
        db->put_attribute_alias(str, attribute_alias);
        cout << "Attribute alias -> " << str << " :\n   " << attribute_alias << "\n" << endl;
    }
    catch (...) {
        cout << "Exception: cannot set attribute alias for the attribute: " << device1_name + "/" + "Short_attr" <<
        endl;
    }

// device2

    db_data.clear();
    DbDatum string_spec_attr("String_spec_attr"), string_spec_attr_label("label"), string_spec_attr_unit(
            "unit"), string_spec_attr_format("format");

    num_prop = 3;

    string_spec_attr << num_prop;
    db_data.push_back(string_spec_attr);
    str = "Device label";
    string_spec_attr_label << str;
    db_data.push_back(string_spec_attr_label);
    str = "Device unit";
    string_spec_attr_unit << str;
    db_data.push_back(string_spec_attr_unit);
    str = "%s";
    string_spec_attr_format << str;
    db_data.push_back(string_spec_attr_format);

    try {
        db->put_device_attribute_property(device2_name, db_data);
        print_changes("Device specific properties", device2_name.c_str(), db_data);
    }
    catch (...) {
        cout << "Exception: cannot set specific properties for the device: " << device2_name << endl;
    }

    db_data.clear();


    Tango::DbDatum fwd_att1("fwd_short_rw");
    Tango::DbDatum root_att1("__root_att");
    fwd_att1 << (short) 1;
    string r_name = device1_name + "/short_attr_rw";;
    root_att1 << r_name;
    db_data.push_back(fwd_att1);
    db_data.push_back(root_att1);

    Tango::DbDatum fwd_att2("fwd_spec_double");
    Tango::DbDatum root_att2("__root_att");
    fwd_att2 << (short) 1;
    r_name = device1_name + "/double_spec_attr";;
    root_att2 << r_name;
    db_data.push_back(fwd_att2);
    db_data.push_back(root_att2);

    Tango::DbDatum fwd_att3("fwd_string_w");
    Tango::DbDatum root_att3("__root_att");
    fwd_att3 << (short) 1;
    r_name = device1_name + "/string_attr_w2";;
    root_att3 << r_name;
    db_data.push_back(fwd_att3);
    db_data.push_back(root_att3);

    Tango::DbDatum fwd_att4("fwd_ima_string_rw");
    Tango::DbDatum root_att4("__root_att");
    fwd_att4 << (short) 1;
    r_name = device2_name + "/string_ima_attr_rw";;
    root_att4 << r_name;
    db_data.push_back(fwd_att4);
    db_data.push_back(root_att4);



    try {
        db->put_device_attribute_property(fwd_dev_name, db_data);
        print_changes("Device specific attribute properties", fwd_dev_name.c_str(), db_data);

    }
    catch (...) {
        cout << "Exception: cannot set specific attribute properties for the device: " << fwd_dev_name << endl;
    }

    db_data.clear();

    //TODO rewrite using functional style

    //pipe class level configuration
    DbDatum pipeConf4("PipeConf4");
    pipeConf4 << (short) 1;
    DbDatum pipeConf4DbClassLabel("label");
    pipeConf4DbClassLabel << "DB_class_def_label";
    DbDatum pipeConf4DbClassDescription("description");
    pipeConf4DbClassDescription << "Bidon";

    DbDatum pipeConf5("PipeConf5");
    pipeConf5 << (short) 1;
    DbDatum pipeConf5DbClassLabel("label");
    pipeConf5DbClassLabel << "ClassDefinedLabel";

    DbDatum pipeConf6("PipeConf6");
    pipeConf6 << (short) 1;
    DbDatum pipeConf6DbClassDesc("description");
    pipeConf6DbClassDesc << "ClassDefinedDesc";

    DbDatum pipeConf7("PipeConf7");
    pipeConf7 << (short) 1;
    DbDatum pipeConf7DbClassDesc("description");
    pipeConf7DbClassDesc << "AnotherClassDefinedDesc";


    db_data.push_back(pipeConf4);
    db_data.push_back(pipeConf4DbClassLabel);
    db_data.push_back(pipeConf4DbClassDescription);
    db_data.push_back(pipeConf5);
    db_data.push_back(pipeConf5DbClassLabel);
    db_data.push_back(pipeConf6);
    db_data.push_back(pipeConf6DbClassDesc);
    db_data.push_back(pipeConf7);
    db_data.push_back(pipeConf7DbClassDesc);


    try{
        db->put_class_pipe_property(CLASS_NAME, db_data);
        print_changes("Set pipe properties at class level",CLASS_NAME, db_data);
    }
    catch (...){
        cout << "Exception: cannot set pipe properties at class level: " << CLASS_NAME << endl;
    }

    db_data.clear();

    //pipe class level configuration
    DbDatum pipeConf3("PipeConf3");
    pipeConf3 << (short) 1;
    DbDatum pipeConf3DbClassLabel("label");
    pipeConf3DbClassLabel << "OverWrittenPipeLabel";

    DbDatum pipeConf4_dev("PipeConf4");
    pipeConf4_dev << (short) 1;
    DbDatum pipeConf4_devDbClassDesc("description");
    pipeConf4_devDbClassDesc << "DB_device_def_desc";

    db_data.push_back(pipeConf3);
    db_data.push_back(pipeConf3DbClassLabel);
    db_data.push_back(pipeConf4_dev);
    db_data.push_back(pipeConf4_devDbClassDesc);

    try{
        db->put_device_pipe_property(device1_name, db_data);
        print_changes("Set pipe properties at device level" , device1_name.c_str(), db_data);
    }
    catch (...){
        cout << "Exception: cannot set specific attribute properties for the device: " << fwd_dev_name << endl;
    }

    delete db;

    return 0;
}
