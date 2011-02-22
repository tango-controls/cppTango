#include "IOAddAttribute.h"

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

bool IOAddAttribute::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
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
    Tango::Attr *at = new Tango::Attr("Added_short_attr", Tango::DEV_SHORT, Tango::READ);
    Tango::UserDefaultAttrProp def_prop;
    def_prop.set_label("Test label");
    def_prop.set_description("Test description");
    def_prop.set_format("Illisible");
    at->set_default_properties(def_prop);	    
    device->add_attribute(at);
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

bool IORemoveAttribute::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
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
    string st("Added_short_attr");	    
    device->remove_attribute(st,true);
    return insert();
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}
