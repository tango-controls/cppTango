#include "IOStr1.h"

//+----------------------------------------------------------------------------
//
// method : 		IOStr1::IOStr1()
// 
// description : 	constructor for the IOStr1 command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOStr1::IOStr1(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		IOStr1::is_allowed()
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

bool IOStr1::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
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
// method : 		IOStr1::execute()
// 
// description : 	method to trigger the execution of the IOStr1
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *IOStr1::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevString argout;
 
    argout = new char[40];
    strcpy(argout,"Hello from IOStr1");
    
    return insert(argout);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOPollStr1::IOPollStr1()
// 
// description : 	constructor for the IOPollStr1 command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOPollStr1::IOPollStr1(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
	num = 0;
}

//+----------------------------------------------------------------------------
//
// method : 		IOPollStr1::is_allowed()
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

bool IOPollStr1::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
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
// method : 		IOPollStr1::execute()
// 
// description : 	method to trigger the execution of the IOPollStr1
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *IOPollStr1::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
  try {
    Tango::DevString argout;
 
    argout = new char[40];
    num++;
    if ((num % 3) == 0)
    	strcpy(argout,"Even value from IOPollStr1");
    else if ((num % 3)  == 1)
    	strcpy(argout,"Odd value from IOPollStr1");
    else
    {
    	delete [] argout;
    	Tango::Except::throw_exception((const char*)"qqq",(const char *)"www",(const char *)"eee");
    }
    
    return insert(argout);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}
