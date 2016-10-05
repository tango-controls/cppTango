#include "IOArray2.h"
#include "DevTest.h"

//+----------------------------------------------------------------------------
//
// method : 		IOArray2::IOArray2()
// 
// description : 	constructor for the IOArray2 command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOArray2::IOArray2(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}

//+----------------------------------------------------------------------------
//
// method : 		IOArray2::is_allowed()
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

bool IOArray2::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
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
// method : 		IOArray2::execute()
// 
// description : 	method to trigger the execution of the IOArray2
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *IOArray2::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevVarShortArray *argout;
    short *data_ptr;
    long data_length;

    DevTest *dev = static_cast<DevTest *>(device);    
    (dev->get_short_array())[0] = 100;
    (dev->get_short_array())[1] = 200;
        
    data_ptr = dev->get_short_array();
    data_length = 2;
    
    argout = dev->create_DevVarShortArray(data_ptr,data_length);
 
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
// method : 		IOPollArray2::IOPollArray2()
// 
// description : 	constructor for the IOPollArray2 command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOPollArray2::IOPollArray2(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
	num = 0;
}

//+----------------------------------------------------------------------------
//
// method : 		IOPollArray2::is_allowed()
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

bool IOPollArray2::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
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
// method : 		IOPollArray2::execute()
// 
// description : 	method to trigger the execution of the IOPollArray2
//			command
//
// in : - device : The device on which the command must be excuted
//	- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------


CORBA::Any *IOPollArray2::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevVarShortArray *argout;
    short *data_ptr;
    long data_length;

    DevTest *dev = static_cast<DevTest *>(device);
    num++;
    if ((num % 2) == 0)
    {   
    	(dev->get_short_array())[0] = 100;
    	(dev->get_short_array())[1] = 200;
cout << "Value are 100,200" << endl;
    }
    else
    {
        (dev->get_short_array())[0] = 300;
	(dev->get_short_array())[1] = 400;
cout << "Value are 300,400" << endl;
    }
        
    data_ptr = dev->get_short_array();
    data_length = 2;
    
    argout = dev->create_DevVarShortArray(data_ptr,data_length);
 
    return insert(argout);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}
