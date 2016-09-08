#include "IOLong.h"

//+----------------------------------------------------------------------------
//
// method : 		IOLong::IOLong()
// 
// description : 	constructor for the IOLong command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOLong::IOLong(const char *name,Tango_CmdArgType in,
		   Tango_CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		IOLong::is_allowed()
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

bool IOLong::is_allowed(DeviceImpl *device, const CORBA_Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango_ON)
		return(true);
	else
		return(false);
}

//+----------------------------------------------------------------------------
//
// method : 		IOLong::execute()
// 
// description : 	method to trigger the execution of the IOLong
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA_Any *IOLong::execute(DeviceImpl *device,const CORBA_Any &in_any)
{	
  try {
    Tango_DevLong theNumber;
    extract(in_any,theNumber);
    cout << "[IOLong::execute] received number " << theNumber << endl;
    theNumber = theNumber * 2;
    cout << "[IOLong::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA_Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


