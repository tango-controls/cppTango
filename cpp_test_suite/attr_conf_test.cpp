#include <tango.h>
#include <time.h>
#include <assert.h>

using namespace std;


void check_description(Tango::DeviceProxy &d,Tango::DeviceProxy &,string &,const char *,const char *,const char *,const char *);
void check_min_value(Tango::DeviceProxy &d,Tango::DeviceProxy &,string &,const char *,const char *,const char *,const char *);
void check_ev_period_value(Tango::DeviceProxy &d,Tango::DeviceProxy &,string &,const char *,const char *,const char *,const char *);
void check_rel_change_value(Tango::DeviceProxy &d,Tango::DeviceProxy &,string &,const char *,const char *,const char *,const char *);
void check_delta_val_value(Tango::DeviceProxy &d,Tango::DeviceProxy &,string &,const char *,const char *,const char *,const char *);

int main(int argc,char *argv[])
{
	if (argc != 2)
	{
		cerr << "Usage: attr_conf <device name>" << endl;
		exit(-1);
	}

	try
	{
		Tango::DeviceProxy dev(argv[1]);
        string adm_name = dev.adm_name();
        Tango::DeviceProxy admin_dev(adm_name);

//
// For string attribute property
//

		string att("DefAttr");

		cout << "DeviceProxy instance created successfully" << endl;

        check_description(dev,admin_dev,att,"Dev desc","No description","No description","No description");
        cout << "        Description : no class, no lib --> OK" << endl;

        att = "DefClassAttr";
        check_description(dev,admin_dev,att,"Dev desc","Class desc","No description","No description");
        cout << "        Description : class, no lib --> OK" << endl;

        att = "DefUserAttr";
        check_description(dev,admin_dev,att,"Dev desc","User desc","User desc","No description");
        cout << "        Description : no class, lib --> OK" << endl;

        att = "DefClassUserAttr";
        check_description(dev,admin_dev,att,"Dev desc","Class description","User desc","No description");
        cout << "        Description : class, lib --> OK" << endl;

//
// For number attribute property
//

        att = "DefAttr";
        check_min_value(dev,admin_dev,att,"10","Not specified","Not specified","Not specified");
        cout << "\n        min_value : no class, no lib --> OK" << endl;

        att = "DefClassAttr";
        check_min_value(dev,admin_dev,att,"10","20","Not specified","Not specified");
        cout << "        min_value : class, no lib --> OK" << endl;

        att = "DefUserAttr";
        check_min_value(dev,admin_dev,att,"10","30","30","Not specified");
        cout << "        min_value : no class, lib --> OK" << endl;

        att = "DefClassUserAttr";
        check_min_value(dev,admin_dev,att,"10","20","30","Not specified");
        cout << "        min_value : class, lib --> OK" << endl;

//
// For event_period attribute property
//

        att = "DefAttr";
        check_ev_period_value(dev,admin_dev,att,"2500","1000","1000","1000");
        cout << "\n        event_period : no class, no lib --> OK" << endl;

        att = "DefClassAttr";
        check_ev_period_value(dev,admin_dev,att,"2500","500","1000","1000");
        cout << "        event_period : class, no lib --> OK" << endl;

        att = "DefUserAttr";
        check_ev_period_value(dev,admin_dev,att,"2500","1500","1500","1000");
        cout << "        event_period : no class, lib --> OK" << endl;

        att = "DefClassUserAttr";
        check_ev_period_value(dev,admin_dev,att,"2500","500","1500","1000");
        cout << "        event_period : class, lib --> OK" << endl;

//
// For rel_change attribute property
//

        att = "DefAttr";
        check_rel_change_value(dev,admin_dev,att,"12","Not specified","Not specified","Not specified");
        cout << "\n        rel_change : no class, no lib --> OK" << endl;

        att = "DefClassAttr";
        check_rel_change_value(dev,admin_dev,att,"12","33,34","Not specified","Not specified");
        cout << "        rel_change : class, no lib --> OK" << endl;

        att = "DefUserAttr";
        check_rel_change_value(dev,admin_dev,att,"12","55","55","Not specified");
        cout << "        rel_change : no class, lib --> OK" << endl;

        att = "DefClassUserAttr";
        check_rel_change_value(dev,admin_dev,att,"12","33,34","55","Not specified");
        cout << "        rel_change : class, lib --> OK" << endl;

//
// For rds alarm
//

        att = "DefAttr";
        check_delta_val_value(dev,admin_dev,att,"222","Not specified","Not specified","Not specified");
        cout << "\n        delta_val : no class, no lib --> OK" << endl;

        att = "DefClassAttr";
        check_delta_val_value(dev,admin_dev,att,"222","5","Not specified","Not specified");
        cout << "        delta_val : class, no lib --> OK" << endl;

        att = "DefUserAttr";
        check_delta_val_value(dev,admin_dev,att,"222","77","77","Not specified");
        cout << "        delta_val : no class, lib --> OK" << endl;

        att = "DefClassUserAttr";
        check_delta_val_value(dev,admin_dev,att,"222","5","77","Not specified");
        cout << "        delta_val : class, lib --> OK" << endl;
    }
	catch (Tango::DevFailed &e)
	{
		Tango::Except::print_exception(e);
		exit(-1);
	}
}

void check_description(Tango::DeviceProxy &dev,
                       Tango::DeviceProxy &admin_dev,
                       string &att,
                       const char *dev_desc,
                       const char *class_desc,
                       const char *user_desc,
                       const char *lib_desc)
{

// Set-up

	Tango::AttributeInfoEx aie;
    Tango::AttributeInfoListEx aie_list;

    aie = dev.get_attribute_config(att);

    aie.description = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.description == dev_desc);

// Return to class

    aie.description = "Nan";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.description == class_desc);

    Tango::DeviceData dd;
    string dev_name = dev.name();
    dd << dev_name;
    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.description == class_desc);

    aie.description = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

// Return to user

    aie.description = "";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.description == user_desc);
    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.description == user_desc);

    aie.description = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);      

// Return to lib

    aie.description = "not specified";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.description == lib_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.description == lib_desc);

    aie.description = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

// Return to class bis

    aie.description = "Nan";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.description == class_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.description == class_desc);

//  Return to user bis

    aie.description = "";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.description == user_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.description == user_desc);

// return to lib bis

    aie.description = "Not specified";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.description == lib_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.description == lib_desc);

// User input == user default

    aie.description = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie.description = user_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.description == user_desc); 

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.description == user_desc); 

// User input == class default

    aie.description = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie.description = class_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.description == class_desc); 

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.description == class_desc);    
}




void check_min_value(Tango::DeviceProxy &dev,
                       Tango::DeviceProxy &admin_dev,
                       string &att,
                       const char *dev_desc,
                       const char *class_desc,
                       const char *user_desc,
                       const char *lib_desc)
{

// Set-up

	Tango::AttributeInfoEx aie;
    Tango::AttributeInfoListEx aie_list;

    aie = dev.get_attribute_config(att);

    aie.min_value = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == dev_desc);

// Return to class

    aie.min_value = "Nan";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == class_desc);

    Tango::DeviceData dd;
    string dev_name = dev.name();
    dd << dev_name;
    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == class_desc);

    aie.min_value = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

// Return to user

    aie.min_value = "";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == user_desc);
    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == user_desc);

    aie.min_value = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list); 

// Return to lib

    aie.min_value = "not specified";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == lib_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == lib_desc);

    aie.min_value = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

// Return to class bis

    aie.min_value = "Nan";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == class_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == class_desc);

//  Return to user bis

    aie.min_value = "";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == user_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == user_desc);

// return to lib bis

    aie.min_value = "Not specified";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == lib_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == lib_desc);

// User input == user default

    aie.min_value = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie.min_value = user_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == user_desc); 

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == user_desc); 

// User input == class default

    aie.min_value = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie.min_value = class_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == class_desc); 

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.min_value == class_desc);           
}


void check_ev_period_value(Tango::DeviceProxy &dev,
                       Tango::DeviceProxy &admin_dev,
                       string &att,
                       const char *dev_desc,
                       const char *class_desc,
                       const char *user_desc,
                       const char *lib_desc)
{

// Set-up

	Tango::AttributeInfoEx aie;
    Tango::AttributeInfoListEx aie_list;

    aie = dev.get_attribute_config(att);

    aie.events.per_event.period = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == dev_desc);

// Return to class

    aie.events.per_event.period = "Nan";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == class_desc);

    Tango::DeviceData dd;
    string dev_name = dev.name();
    dd << dev_name;
    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == class_desc);

    aie.events.per_event.period = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

// Return to user

    aie.events.per_event.period = "";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == user_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == user_desc);

    aie.events.per_event.period = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list); 

// Return to lib

    aie.events.per_event.period = "not specified";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == lib_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == lib_desc);

    aie.events.per_event.period = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

// Return to class bis

    aie.events.per_event.period = "Nan";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == class_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == class_desc);

//  Return to user bis

    aie.events.per_event.period = "";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == user_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == user_desc);

// return to lib bis

    aie.events.per_event.period = "Not specified";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == lib_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == lib_desc);

// User input == user default

    aie.events.per_event.period = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie.events.per_event.period = user_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == user_desc); 

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == user_desc); 

// User input == class default

    aie.events.per_event.period = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie.events.per_event.period = class_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == class_desc); 

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.per_event.period == class_desc);      
}

void check_rel_change_value(Tango::DeviceProxy &dev,
                       Tango::DeviceProxy &admin_dev,
                       string &att,
                       const char *dev_desc,
                       const char *class_desc,
                       const char *user_desc,
                       const char *lib_desc)
{

// Set-up

	Tango::AttributeInfoEx aie;
    Tango::AttributeInfoListEx aie_list;

    aie = dev.get_attribute_config(att);

    aie.events.ch_event.rel_change = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == dev_desc);

// Return to class

    aie.events.ch_event.rel_change = "Nan";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == class_desc);

    Tango::DeviceData dd;
    string dev_name = dev.name();
    dd << dev_name;
    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == class_desc);

    aie.events.ch_event.rel_change = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

// Return to user

    aie.events.ch_event.rel_change = "";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == user_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == user_desc);

    aie.events.ch_event.rel_change = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list); 

// Return to lib

    aie.events.ch_event.rel_change = "not specified";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == lib_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == lib_desc);

    aie.events.ch_event.rel_change = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

// Return to class bis

    aie.events.ch_event.rel_change = "Nan";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == class_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == class_desc);

//  Return to user bis

    aie.events.ch_event.rel_change = "";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == user_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == user_desc);

// return to lib bis

    aie.events.ch_event.rel_change = "Not specified";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == lib_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == lib_desc);

// User input == user default

    aie.events.ch_event.rel_change = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie.events.ch_event.rel_change = user_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == user_desc); 

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == user_desc); 

// User input == class default

    aie.events.ch_event.rel_change = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie.events.ch_event.rel_change = class_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == class_desc); 

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.events.ch_event.rel_change == class_desc);   
       
}



void check_delta_val_value(Tango::DeviceProxy &dev,
                       Tango::DeviceProxy &admin_dev,
                       string &att,
                       const char *dev_desc,
                       const char *class_desc,
                       const char *user_desc,
                       const char *lib_desc)
{

// Set-up

	Tango::AttributeInfoEx aie;
    Tango::AttributeInfoListEx aie_list;

    aie = dev.get_attribute_config(att);

    aie.alarms.delta_val = dev_desc;
    aie.alarms.delta_t = "200";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == dev_desc);

// Return to class

    aie.alarms.delta_val = "Nan";
    aie.alarms.delta_t = "Nan";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == class_desc);

    Tango::DeviceData dd;
    string dev_name = dev.name();
    dd << dev_name;
    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == class_desc);

    aie.alarms.delta_val = dev_desc;
    aie.alarms.delta_t = "200";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

// Return to user

    aie.alarms.delta_val = "";
    aie.alarms.delta_t = "";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == user_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == user_desc);

    aie.alarms.delta_val = dev_desc;
    aie.alarms.delta_t = "200";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list); 

// Return to lib

    aie.alarms.delta_val = "not specified";
    aie.alarms.delta_t = "Not SPECIFIED";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == lib_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == lib_desc);

    aie.alarms.delta_val = dev_desc;
    aie.alarms.delta_t = "200";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

// Return to class bis

    aie.alarms.delta_val = "Nan";
    aie.alarms.delta_t = "Nan";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == class_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == class_desc);

//  Return to user bis

    aie.alarms.delta_val = "";
    aie.alarms.delta_t = "";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == user_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == user_desc);

// return to lib bis

    aie.alarms.delta_val = "Not specified";
    aie.alarms.delta_t = "Not specified";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == lib_desc);

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == lib_desc);

// User input == user default

/*    aie.alarms.delta_val = dev_desc;
    aie.alarms.delta_t = "200";
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie.alarms.delta_val = user_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == user_desc); 

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == user_desc); 

// User input == class default

    aie.alarms.delta_val = dev_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie.alarms.delta_val = class_desc;
    aie_list.clear();
    aie_list.push_back(aie);
    dev.set_attribute_config(aie_list);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == class_desc); 

    admin_dev.command_inout("DevRestart",dd);

    aie = dev.get_attribute_config(att);
    assert (aie.alarms.delta_val == class_desc); */  
       
}

