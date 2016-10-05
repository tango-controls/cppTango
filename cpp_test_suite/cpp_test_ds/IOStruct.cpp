#include "IOStruct.h"

//+----------------------------------------------------------------------------
//
// method : 		IOStruct::IOStruct()
// 
// description : 	constructor for the IOStruct command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOStruct::IOStruct(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		IOStruct::is_allowed()
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

bool IOStruct::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
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
// method : 		IOStruct::execute()
// 
// description : 	method to trigger the execution of the IOStruct
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *IOStruct::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevVarLongStringArray	*argout = new Tango::DevVarLongStringArray();

	//	Add your own code to control device here
	
    argout->lvalue.length(2);
    argout->lvalue[0] = 1000;
    argout->lvalue[1] = 2000;
		
    argout->svalue.length(2);
    argout->svalue[0] = CORBA::string_dup("First string from dev_struct");
    argout->svalue[1] = CORBA::string_dup("Second string from dev_struct"); 
     
    return insert(argout);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


