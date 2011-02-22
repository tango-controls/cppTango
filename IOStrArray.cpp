#include "IOStrArray.h"

//+----------------------------------------------------------------------------
//
// method : 		IOStrArray::IOStrArray()
// 
// description : 	constructor for the IOStrArray command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOStrArray::IOStrArray(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		IOStrArray::is_allowed()
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

bool IOStrArray::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
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
// method : 		IOStrArray::execute()
// 
// description : 	method to trigger the execution of the IOStrArray
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *IOStrArray::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevVarStringArray *argout = new Tango::DevVarStringArray();

    argout->length(2);
    (*argout)[0] = CORBA::string_dup("First string from dev_string_array");
    (*argout)[1] = CORBA::string_dup("Second string from dev_string_array");
     
    return insert(argout);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


