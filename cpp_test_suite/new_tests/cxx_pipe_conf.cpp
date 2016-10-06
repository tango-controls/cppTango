#ifndef PipeConfTestSuite_h
#define PipeConfTestSuite_h

#include <cxxtest/TestSuite.h>
#include <cxxtest/TangoPrinter.h>
#include <tango.h>
#include <iostream>

using namespace Tango;
using namespace std;

#define cout cout << "\t"

#undef SUITE_NAME
#define SUITE_NAME PipeConfTestSuite

class PipeConfTestSuite: public CxxTest::TestSuite
{
protected:
	DeviceProxy 			*device1;
	DeviceProxy				*device2;
	string 					device1_name;
	string					device2_name;
	DeviceProxy 			*root_admin;


	bool					verbose;

public:
	SUITE_NAME()
	{
//
// Arguments check -------------------------------------------------
//

		// user arguments, obtained from the command line sequentially
		device1_name = CxxTest::TangoPrinter::get_param("device1");
		device2_name = CxxTest::TangoPrinter::get_param("device2");
		string full_ds_name = CxxTest::TangoPrinter::get_param("fulldsname");

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

			device2 = new DeviceProxy(device2_name);
			device2->ping();

			string root_adm_name("dserver/");
			root_adm_name = root_adm_name + full_ds_name;
			root_admin = new DeviceProxy(root_adm_name);
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
		delete device2;
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


// Test pipe config

	void test_pipe_with_default_conf(void)
	{
		PipeInfo pi;
		pi = device1->get_pipe_config("PipeConf1");

		TS_ASSERT(pi.name == "PipeConf1");
		TS_ASSERT(pi.disp_level == OPERATOR);
		TS_ASSERT(pi.writable == PIPE_READ);

		TS_ASSERT(pi.label == "PipeConf1");
		TS_ASSERT(pi.description == "No description");
	}

	void test_pipe_with_user_conf(void)
	{
		PipeInfo pi;
		pi = device1->get_pipe_config("PipeConf2");

		TS_ASSERT(pi.name == "PipeConf2");
		TS_ASSERT(pi.disp_level == OPERATOR);
		TS_ASSERT(pi.writable == PIPE_READ);

		TS_ASSERT(pi.label == "PipeLabel");
		TS_ASSERT(pi.description == "A Tango pipe with user defined desc");
	}

	void test_pipe_with_user_and_db_conf(void)
	{
		PipeInfo pi;
		pi = device1->get_pipe_config("PipeConf3");

		TS_ASSERT(pi.name == "PipeConf3");
		TS_ASSERT(pi.disp_level == OPERATOR);
		TS_ASSERT(pi.writable == PIPE_READ);

		TS_ASSERT(pi.label == "OverWrittenPipeLabel");
		TS_ASSERT(pi.description == "No description");
	}

	void test_pipe_with_db_class_device_conf(void)
	{
		PipeInfo pi;
		pi = device1->get_pipe_config("PipeConf4");

		TS_ASSERT(pi.name == "PipeConf4");
		TS_ASSERT(pi.disp_level == OPERATOR);
		TS_ASSERT(pi.writable == PIPE_READ);

		TS_ASSERT(pi.label == "DB_class_def_label");
		TS_ASSERT(pi.description == "DB_device_def_desc");
	}

	void test_pipe_with_db_class_conf(void)
	{
		PipeInfo pi;
		pi = device1->get_pipe_config("PipeConf5");

		TS_ASSERT(pi.name == "PipeConf5");
		TS_ASSERT(pi.disp_level == OPERATOR);
		TS_ASSERT(pi.writable == PIPE_READ);

		TS_ASSERT(pi.label == "ClassDefinedLabel");
		TS_ASSERT(pi.description == "No description");
	}


	/**
	 * The following tests check that class and device level defined properties override user defined ones,
	 * i.e. db->put_device_pipe_property => db->put_class_pipe_property => user (DevTestClass::pipe_factory)
	 *
	 */
	void test_pipe_conf_return_to_xxx_no_class_no_lib(void)
	{
		string pipe_name("PipeConf1");
        check_description(pipe_name,"Dev desc","No description","No description","No description");
 	}

	void test_pipe_conf_return_to_xxx_class_no_lib(void)
	{
		string pipe_name("PipeConf7");
        check_description(pipe_name,"Dev desc","AnotherClassDefinedDesc","No description","No description");
	}

	void test_pipe_conf_return_to_xxx_no_class_lib(void)
	{
        string pipe_name("PipeConf2");
        check_description(pipe_name,"Dev desc","A Tango pipe with user defined desc","A Tango pipe with user defined desc","No description");
	}

	void test_pipe_conf_return_to_xxx_class_lib(void)
	{
        string pipe_name("PipeConf6");
        check_description(pipe_name,"Dev desc","ClassDefinedDesc","UserDefinedDesc","No description");
	}

	void test_pipe_conf_on_diff_devices(void)
	{
		Tango::PipeInfo pi;
		Tango::PipeInfo pi2;
		Tango::PipeInfoList pi_list;

		string pipe_name("PipeConf1");
		pi = device1->get_pipe_config(pipe_name);

		pi.description = "toto";
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT(pi.description == "toto");

		pi2 = device2->get_pipe_config(pipe_name);
cout << "pi2.description = " << pi2.description << endl;
		TS_ASSERT(pi2.description == "No description");

// Return to lib

		pi.description = "not specified";
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi = device1->get_pipe_config(pipe_name);
		assert (pi.description == "No description");
	}

	void check_description(string &pipe_name,
                       		const char *dev_desc,
                       		const char *class_desc,
                       		const char *user_desc,
                       		const char *lib_desc)
	{

// Set-up

		Tango::PipeInfo pi;
		Tango::PipeInfoList pi_list;

		pi = device1->get_pipe_config(pipe_name);

		pi.description = dev_desc;
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT(pi.description == dev_desc);

// Return to class

		pi.description = "Nan";
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT(pi.description == class_desc);

		Tango::DeviceData dd;
		string dev_name = device1->name();
		dd << dev_name;
		root_admin->command_inout("DevRestart",dd);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT(pi.description == class_desc);

		pi.description = dev_desc;
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

// Return to user

		pi.description = "";
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == user_desc);
		root_admin->command_inout("DevRestart",dd);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == user_desc);

		pi.description = dev_desc;
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);      

// Return to lib

		pi.description = "not specified";
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi = device1->get_pipe_config(pipe_name);
		assert (pi.description == lib_desc);

		root_admin->command_inout("DevRestart",dd);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == lib_desc);

		pi.description = dev_desc;
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

// Return to class bis

		pi.description = "Nan";
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == class_desc);

		root_admin->command_inout("DevRestart",dd);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == class_desc);

//  Return to user bis

		pi.description = "";
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == user_desc);

		root_admin->command_inout("DevRestart",dd);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == user_desc);

// return to lib bis

		pi.description = "Not specified";
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == lib_desc);

		root_admin->command_inout("DevRestart",dd);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == lib_desc);

// User input == user default

		pi.description = dev_desc;
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi.description = user_desc;
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == user_desc); 

		root_admin->command_inout("DevRestart",dd);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == user_desc); 

// User input == class default

		pi.description = dev_desc;
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi.description = class_desc;
		pi_list.clear();
		pi_list.push_back(pi);
		device1->set_pipe_config(pi_list);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == class_desc); 

		root_admin->command_inout("DevRestart",dd);

		pi = device1->get_pipe_config(pipe_name);
		TS_ASSERT (pi.description == class_desc);    
	}

};


#undef cout
#endif // PipeTestSuite_h
