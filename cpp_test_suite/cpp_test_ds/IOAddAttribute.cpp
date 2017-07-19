#include "IOAddAttribute.h"
#include <DevTest.h>

//+----------------------------------------------------------------------------
//
// method : 		IOAddAttribute::IOAddAttribute()
//
// description : 	constructor for the IOAddAttribute command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOAddAttribute::IOAddAttribute(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		IOAddAttribute::is_allowed()
//
// description : 	method to test whether command is allowed or not in this
//			state. In this case, the command is allowed only if
//			the device is in ON state
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns :	boolean - true == is allowed , false == not allowed
//
//-----------------------------------------------------------------------------

bool IOAddAttribute::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

//+----------------------------------------------------------------------------
//
// method : 		IOAddAttribute::execute()
//
// description : 	method to trigger the execution of the IOAddAttribute
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *IOAddAttribute::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
  try {
    cout << "[IOAddAttribute::execute] " << endl;

    Tango::DevString new_att;
	extract(in_any, new_att);
	string str(new_att);
	transform(str.begin(),str.end(),str.begin(),::tolower);

	if (str == "added_short_attr")
	{
		Tango::Attr *at = new Tango::Attr("Added_short_attr", Tango::DEV_SHORT, Tango::READ);
		Tango::UserDefaultAttrProp def_prop;
		def_prop.set_label("Test label");
		def_prop.set_description("Test description");
		def_prop.set_format("Illisible");
		at->set_default_properties(def_prop);
		device->add_attribute(at);
	}
	else if (str == "added_enum_attr")
	{
		Tango::Attr *at = new Tango::Attr("Added_enum_attr", Tango::DEV_ENUM, Tango::READ);
		Tango::UserDefaultAttrProp def_prop;
		vector<string> v_s;
		v_s.push_back("Red");
		v_s.push_back("Green");
		v_s.push_back("Blue");
		def_prop.set_enum_labels(v_s);
		at->set_default_properties(def_prop);
		device->add_attribute(at);
	}

    return insert();
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IORemoveAttribute::IORemoveAttribute()
//
// description : 	constructor for the IOAddAttribute command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IORemoveAttribute::IORemoveAttribute(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		IORemoveAttribute::is_allowed()
//
// description : 	method to test whether command is allowed or not in this
//			state. In this case, the command is allowed only if
//			the device is in ON state
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns :	boolean - true == is allowed , false == not allowed
//
//-----------------------------------------------------------------------------

bool IORemoveAttribute::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

//+----------------------------------------------------------------------------
//
// method : 		IORemoveAttribute::execute()
//
// description : 	method to trigger the execution of the IOAddAttribute
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *IORemoveAttribute::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
  try {
    cout << "[IORemoveAttribute::execute] " << endl;

	Tango::DevString att_name;
	extract(in_any, att_name);
	string str(att_name);
/*	transform(str.begin(),str.end(),str.begin(),::tolower);*/

    device->remove_attribute(str,true);
    return insert();
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}

//-------------------------------------------------------------------------------------------------------------

//+----------------------------------------------------------------------------
//
// method : 		DynCommand::DynCommand()
//
// description : 	constructor for the IOAddCommand command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

DynCommand::DynCommand(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		DynCommand::is_allowed()
//
// description : 	method to test whether command is allowed or not in this
//			state. In this case, the command is allowed only if
//			the device is in ON state
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns :	boolean - true == is allowed , false == not allowed
//
//-----------------------------------------------------------------------------

bool DynCommand::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

//+----------------------------------------------------------------------------
//
// method : 		DynCommand::execute()
//
// description : 	method to trigger the execution of the IOAddCommand
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *DynCommand::execute(TANGO_UNUSED(Tango::DeviceImpl *device),TANGO_UNUSED(const CORBA::Any &in_any))
{
	try
	{
		cout << "[DynCommand::execute] " << endl;
		Tango::DevFloat theNumber = 4.0;
		return insert(theNumber);
	}
	catch (CORBA::Exception &e)
	{
		Tango::Except::print_exception(e);
		throw ;
    }
}

//------------------------------------------------------------------------------------------------------------


//+----------------------------------------------------------------------------
//
// method : 		IOAddCommand::IOAddCommand()
//
// description : 	constructor for the IOAddCommand command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOAddCommand::IOAddCommand(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		IOAddCommand::is_allowed()
//
// description : 	method to test whether command is allowed or not in this
//			state. In this case, the command is allowed only if
//			the device is in ON state
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns :	boolean - true == is allowed , false == not allowed
//
//-----------------------------------------------------------------------------

bool IOAddCommand::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

//+----------------------------------------------------------------------------
//
// method : 		IOAddCommand::execute()
//
// description : 	method to trigger the execution of the IOAddCommand
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *IOAddCommand::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	try
	{
		cout << "[IOAddCommand::execute] " << endl;
		Tango::DevLong cmd_arg;
		extract(in_any,cmd_arg);

		bool device_level = false;
		if (cmd_arg >= 1)
			device_level = true;
		int loop = 1;
		if (cmd_arg == 2)
			loop = 3;
		for (int ctr = 0;ctr < loop;ctr++)
		{
			stringstream ss;
			ss << "Added_cmd";
			if (ctr != 0)
				ss << "_" << ctr;

			Tango::Command *at = new DynCommand(ss.str().c_str(), Tango::DEV_VOID, Tango::DEV_FLOAT,"","Output value (4.0)");
			device->add_command(at,device_level);
		}

		return insert();
	}
	catch (CORBA::Exception &e)
	{
		Tango::Except::print_exception(e);
		throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IORemoveCommand::IORemoveCommand()
//
// description : 	constructor for the IOAddAttribute command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IORemoveCommand::IORemoveCommand(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		IORemoveCommand::is_allowed()
//
// description : 	method to test whether command is allowed or not in this
//			state. In this case, the command is allowed only if
//			the device is in ON state
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns :	boolean - true == is allowed , false == not allowed
//
//-----------------------------------------------------------------------------

bool IORemoveCommand::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

//+----------------------------------------------------------------------------
//
// method : 		IORemoveCommand::execute()
//
// description : 	method to trigger the execution of the IOAddAttribute
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *IORemoveCommand::execute(Tango::DeviceImpl *device,TANGO_UNUSED(const CORBA::Any &in_any))
{
	try
	{
		cout << "[IORemoveCommand::execute] " << endl;
		string st("Added_cmd");
		device->remove_command(st,true);
		return insert();
	}
	catch (CORBA::Exception &e)
	{
		Tango::Except::print_exception(e);
		throw ;
	}
}
