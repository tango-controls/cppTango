#include "IOSetAttr.h"
#include "DevTest.h"

//+----------------------------------------------------------------------------
//
// method : 		IOSetAttr::IOSetAttr()
// 
// description : 	constructor for the IOSetAttr command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOSetAttr::IOSetAttr(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		IOSetAttr::is_allowed()
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

bool IOSetAttr::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if ((device->get_state() == Tango::ON) ||
	    (device->get_state() == Tango::ALARM))
		return(true);
	else
		return(false);
}

//+----------------------------------------------------------------------------
//
// method : 		IOSetAttr::execute()
// 
// description : 	method to trigger the execution of the IOSetAttr
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *IOSetAttr::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevLong theNumber;
    extract(in_any,theNumber);
    cout << "[IOSetAttr::execute] received number " << theNumber << endl;
    DevTest *dev = static_cast<DevTest *>(device);    
    dev->set_attr_long(theNumber);
    return insert();
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


