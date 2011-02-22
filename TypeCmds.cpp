#include "TypeCmds.h"

#define DEV_DEBUG_STREAM(dev) \
  if (dev->get_logger()->is_debug_enabled())\
    dev->get_logger()->debug_stream()

//+----------------------------------------------------------------------------
//
// method : 		IOVoid::IOVoid()
// 
// description : 	constructor for the IOVoid command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOVoid::IOVoid(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOVoid::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOVoid::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    cout << "[IOVoid::execute] " << endl;
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
// method : 		IOBool::IOBool()
// 
// description : 	constructor for the IOBool command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOBool::IOBool(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOBool::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOBool::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevBoolean theNumber;
    extract(in_any,theNumber);
    cout << "[IOBool::execute] received number " << theNumber << endl;
    theNumber = !theNumber;
    cout << "[IOBool::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOShort::IOShort()
// 
// description : 	constructor for the IOShort command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOShort::IOShort(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOShort::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOShort::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevShort theNumber;
    extract(in_any,theNumber);
    cout << "[IOShort::execute] received number " << theNumber << endl;
    theNumber = theNumber * 2;
    cout << "[IOShort::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}

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

IOLong::IOLong(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOLong::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOLong::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevLong theNumber;
    extract(in_any,theNumber);
    cout << "[IOLong::execute] received number " << theNumber << endl;
    DEV_DEBUG_STREAM(device) << "[IOLong::execute] received number " << theNumber;
    theNumber = theNumber * 2;
    cout << "[IOLong::execute] return number " << theNumber << endl;
    DEV_DEBUG_STREAM(device) << "[IOLong::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOLong64::IOLong64()
// 
// description : 	constructor for the IOLong64 command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOLong64::IOLong64(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOLong64::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOLong64::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevLong theNumber;
    extract(in_any,theNumber);
    cout << "[IOLong64::execute] received number " << theNumber << endl;
    DEV_DEBUG_STREAM(device) << "[IOLong64::execute] received number " << theNumber;
    theNumber = theNumber * 2;
    cout << "[IOLong64::execute] return number " << theNumber << endl;
    DEV_DEBUG_STREAM(device) << "[IOLong64::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}

//+----------------------------------------------------------------------------
//
// method : 		IOFloat::IOFloat()
// 
// description : 	constructor for the IOFloat command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOFloat::IOFloat(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOFloat::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOFloat::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevFloat theNumber;
    extract(in_any,theNumber);
    cout << "[IOFloat::execute] received number " << theNumber << endl;
    theNumber = theNumber * 2;
    cout << "[IOFloat::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IODouble::IODouble()
// 
// description : 	constructor for the IODouble command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IODouble::IODouble(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IODouble::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IODouble::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevDouble theNumber;
    extract(in_any,theNumber);
    cout << "[IODouble::execute] received number " << theNumber << endl;
    theNumber = theNumber * 2;
    cout << "[IODouble::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOUShort::IOUShort()
// 
// description : 	constructor for the IOUShort command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOUShort::IOUShort(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOUShort::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOUShort::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevUShort theNumber;
    extract(in_any,theNumber);
    cout << "[IOUShort::execute] received number " << theNumber << endl;
    theNumber = theNumber * 2;
    cout << "[IOUShort::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}

//+----------------------------------------------------------------------------
//
// method : 		IOULong::IOULong()
// 
// description : 	constructor for the IOULong command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOULong::IOULong(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOULong::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOULong::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevULong theNumber;
    extract(in_any,theNumber);
    cout << "[IOULong::execute] received number " << theNumber << endl;
    theNumber = theNumber * 2;
    cout << "[IOULong::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}



//+----------------------------------------------------------------------------
//
// method : 		IOULong64::IOULong64()
// 
// description : 	constructor for the IOULong64 command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOULong64::IOULong64(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOULong64::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOULong64::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevULong64 theNumber;
    extract(in_any,theNumber);
    cout << "[IOULong64::execute] received number " << theNumber << endl;
    theNumber = theNumber * 2;
    cout << "[IOULong64::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOString::IOString()
// 
// description : 	constructor for the IOString command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOString::IOString(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOString::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOString::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout2 << "[IOString::execute] arrived" << endl;
	try {

	  Tango::DevString theWord;
	  extract(in_any,theWord);
	  string palindrome;
	  string firstWord = theWord;
	  string::reverse_iterator currentChar(firstWord.rbegin());
	  string::reverse_iterator endChar(firstWord.rend());
	  
	  cout << "[IOString::execute] firstWord = " << firstWord << endl;
	  
	  for ( ; currentChar != endChar; currentChar++) {
	    cout << "[IOString::execute]  currentChar = " << *currentChar <<endl;
	    palindrome += *currentChar;
	  }
	  cout << "[IOString::execute] palindrome = " << palindrome << endl; 
	  return insert(palindrome.c_str());
	}
	catch (CORBA::Exception &e)
	  {
	    Tango::Except::print_exception(e);
	  }
}


//+----------------------------------------------------------------------------
//
// method : 		IOCharArray::IOCharArray()
// 
// description : 	constructor for the IOCharArray command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOCharArray::IOCharArray(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOCharArray::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *IOCharArray::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    cout << "[IOCharArray::execute] entering "  << endl;
    const Tango::DevVarCharArray *theCharArray;
    extract(in_any,theCharArray);
    Tango::DevVarCharArray *theReturnedArray = new Tango::DevVarCharArray();
    theReturnedArray->length(theCharArray->length());
    for (unsigned int i=0; i<theCharArray->length(); i++) {
      cout << "[IOCharArray::execute] received char " << (*theCharArray)[i] << endl;
      (*theReturnedArray)[theCharArray->length()-i-1] = (*theCharArray)[i] ;
    }
    return insert(theReturnedArray);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOShortArray::IOShortArray()
// 
// description : 	constructor for the IOShortArray command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOShortArray::IOShortArray(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOShortArray::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOShortArray::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    const Tango::DevVarShortArray *theNumberArray;
    extract(in_any,theNumberArray);
    Tango::DevVarShortArray *theReturnedArray = new Tango::DevVarShortArray();
    theReturnedArray->length(theNumberArray->length());
    for (unsigned int i=0; i<theNumberArray->length(); i++) {
      cout << "[IOShortArray::execute] received number " << (*theNumberArray)[i] << endl;
      (*theReturnedArray)[i] = (*theNumberArray)[i] * 2;
      cout << "[IOShortArray::execute] return number " << (*theReturnedArray)[i] << endl;
    }
    return insert(theReturnedArray);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOLongArray::IOLongArray()
// 
// description : 	constructor for the IOLongArray command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOLongArray::IOLongArray(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOLongArray::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOLongArray::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    const Tango::DevVarLongArray *theNumberArray;
    extract(in_any,theNumberArray);
    Tango::DevVarLongArray *theReturnedArray = new Tango::DevVarLongArray();
    theReturnedArray->length(theNumberArray->length());
    for (unsigned int i=0; i<theNumberArray->length(); i++) {
      cout << "[IOLongArray::execute] received number " << (*theNumberArray)[i] << endl;
      (*theReturnedArray)[i] = (*theNumberArray)[i] * 2;
      cout << "[IOLongArray::execute] return number " << (*theReturnedArray)[i] << endl;
    }
    return insert(theReturnedArray);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOFloatArray::IOFloatArray()
// 
// description : 	constructor for the IOFloatArray command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOFloatArray::IOFloatArray(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOFloatArray::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOFloatArray::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    const Tango::DevVarFloatArray *theNumberArray;
    extract(in_any,theNumberArray);
    Tango::DevVarFloatArray *theReturnedArray = new Tango::DevVarFloatArray();
    theReturnedArray->length(theNumberArray->length());
    for (unsigned int i=0; i<theNumberArray->length(); i++) {
      cout << "[IOFloatArray::execute] received number " << (*theNumberArray)[i] << endl;
      (*theReturnedArray)[i] = (*theNumberArray)[i] * 2;
      cout << "[IOFloatArray::execute] return number " << (*theReturnedArray)[i] << endl;
    }
    return insert(theReturnedArray);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}

//+----------------------------------------------------------------------------
//
// method : 		IODoubleArray::IODoubleArray()
// 
// description : 	constructor for the IODoubleArray command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IODoubleArray::IODoubleArray(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool IODoubleArray::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IODoubleArray::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    const Tango::DevVarDoubleArray *theNumberArray;
    extract(in_any,theNumberArray);
    Tango::DevVarDoubleArray *theReturnedArray = new Tango::DevVarDoubleArray();
    theReturnedArray->length(theNumberArray->length());
    for (unsigned int i=0; i<theNumberArray->length(); i++) {
      cout << "[IODoubleArray::execute] received number " << (*theNumberArray)[i] << endl;
      (*theReturnedArray)[i] = (*theNumberArray)[i] * 2;
      cout << "[IODoubleArray::execute] return number " << (*theReturnedArray)[i] << endl;
    }
    return insert(theReturnedArray);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOUShortArray::IOUShortArray()
// 
// description : 	constructor for the IOUShortArray command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOUShortArray::IOUShortArray(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOUShortArray::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOUShortArray::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    const Tango::DevVarUShortArray *theNumberArray;
    extract(in_any,theNumberArray);
    Tango::DevVarUShortArray *theReturnedArray = new Tango::DevVarUShortArray();
    theReturnedArray->length(theNumberArray->length());
    for (unsigned int i=0; i<theNumberArray->length(); i++) {
      cout << "[IOUShortArray::execute] received number " << (*theNumberArray)[i] << endl;
      (*theReturnedArray)[i] = (*theNumberArray)[i] * 2;
      cout << "[IOUShortArray::execute] return number " << (*theReturnedArray)[i] << endl;
    }
    return insert(theReturnedArray);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOULongArray::IOULongArray()
// 
// description : 	constructor for the IOULongArray command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOULongArray::IOULongArray(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOULongArray::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOULongArray::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    const Tango::DevVarULongArray *theNumberArray;
    extract(in_any,theNumberArray);
    Tango::DevVarULongArray *theReturnedArray = new Tango::DevVarULongArray();
    theReturnedArray->length(theNumberArray->length());
    for (unsigned int i=0; i<theNumberArray->length(); i++) {
      cout << "[IOULongArray::execute] received number " << (*theNumberArray)[i] << endl;
      (*theReturnedArray)[i] = (*theNumberArray)[i] * 2;
      cout << "[IOULongArray::execute] return number " << (*theReturnedArray)[i] << endl;
    }
    return insert(theReturnedArray);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOStringArray::IOStringArray()
// 
// description : 	constructor for the IOStringArray command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOStringArray::IOStringArray(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}

bool IOStringArray::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOStringArray::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    const Tango::DevVarStringArray *theStringArray;
    extract(in_any,theStringArray);
    Tango::DevVarStringArray *theReturnedArray = new Tango::DevVarStringArray();
    theReturnedArray->length(theStringArray->length());
    for (unsigned int i=0; i<theStringArray->length(); i++) {
      cout << "[IOStringArray::execute] received String " << (*theStringArray)[i].in() << endl;
      (*theReturnedArray)[theStringArray->length()-i-1] = (*theStringArray)[i] ;
      cout << "[IOStringArray::execute] return String " << (*theReturnedArray)[i].in() << endl;
    }
    return insert(theReturnedArray);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOLongString::IOLongString()
// 
// description : 	constructor for the IOLongString command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOLongString::IOLongString(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOLongString::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOLongString::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    const Tango::DevVarLongStringArray *theReceived;
    extract(in_any,theReceived);
    Tango::DevVarLongStringArray *theReturned = new Tango::DevVarLongStringArray;
    unsigned int i;

    ((*theReturned).lvalue).length(((*theReceived).lvalue).length());
    for (i=0; i<((*theReceived).lvalue).length(); i++) {
      cout << "[IOLongString::execute] received number " << (*theReceived).lvalue[i] << endl;
      (*theReturned).lvalue[i] = (*theReceived).lvalue[i] * 2;
      cout << "[IOLongString::execute] return number " << (*theReturned).lvalue[i] << endl;
    }
    ((*theReturned).svalue).length(((*theReceived).svalue).length());
    for (i=0; i<((*theReceived).svalue).length(); i++) {
      cout << "[IOLongString::execute] received string " << (*theReceived).svalue[i].in() << endl;
      (*theReturned).svalue[i] = CORBA::string_dup((*theReceived).svalue[i]);
      cout << "[IOLongString::execute] return string " << (*theReturned).svalue[i].in() << endl;
    }
    return insert(theReturned);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IODoubleString::IODoubleString()
// 
// description : 	constructor for the IODoubleString command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IODoubleString::IODoubleString(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}



bool IODoubleString::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IODoubleString::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    const Tango::DevVarDoubleStringArray *theReceived;
    Tango::DevVarDoubleStringArray *theReturned = new Tango::DevVarDoubleStringArray();
    unsigned int i;

    extract(in_any,theReceived);
    ((*theReturned).dvalue).length(((*theReceived).dvalue).length());
    for (i=0; i<((*theReceived).dvalue).length(); i++) {
      cout << "[IODoubleString::execute] received number " << (*theReceived).dvalue[i] << endl;
      (*theReturned).dvalue[i] = (*theReceived).dvalue[i] * 2;
      cout << "[IODoubleString::execute] return number " << (*theReturned).dvalue[i] << endl;
    }
    ((*theReturned).svalue).length(((*theReceived).svalue).length());
    for (i=0; i<((*theReceived).svalue).length(); i++) {
      cout << "[IODoubleString::execute] received string " << (*theReceived).svalue[i].in() << endl;
      (*theReturned).svalue[i] = (*theReceived).svalue[i];
      cout << "[IODoubleString::execute] return string " << (*theReturned).svalue[i].in() << endl;
    }
    return insert(theReturned);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		OLong::OLong()
// 
// description : 	constructor for the OLong command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

OLong::OLong(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool OLong::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *OLong::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    long theNumber = 22;
    cout << "[OLong::execute] return number " << theNumber << endl;
    DEV_DEBUG_STREAM(device) << "[OLong::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		OULong::OULong()
// 
// description : 	constructor for the OULong command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

OULong::OULong(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool OULong::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *OULong::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    unsigned long theNumber = 333;
    cout << "[OULong::execute] return number " << theNumber << endl;
    return insert(theNumber);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		OLongArray::OLongArray()
// 
// description : 	constructor for the OLongArray command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

OLongArray::OLongArray(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool OLongArray::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *OLongArray::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevVarLongArray *theReturnedArray = new Tango::DevVarLongArray();
    theReturnedArray->length(4);
    for (int i=0; i<4; i++) {
      (*theReturnedArray)[i] = 555 + i;
      cout << "[OLongArray::execute] return number " << (*theReturnedArray)[i] << endl;
    }
    return insert(theReturnedArray);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		OULongArray::OULongArray()
// 
// description : 	constructor for the OULongArray command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

OULongArray::OULongArray(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool OULongArray::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *OULongArray::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevVarULongArray *theReturnedArray = new Tango::DevVarULongArray();
    theReturnedArray->length(3);
    for (int i=0; i<3; i++) {
      (*theReturnedArray)[i] = 777 + i;
      cout << "[OULongArray::execute] return number " << (*theReturnedArray)[i] << endl;
    }
    return insert(theReturnedArray);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		OLongString::OLongString()
// 
// description : 	constructor for the IOLongString command of the 
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

OLongString::OLongString(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool OLongString::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *OLongString::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
  try {
    Tango::DevVarLongStringArray *theReturned = new Tango::DevVarLongStringArray();
    int i;

    ((*theReturned).lvalue).length(6);
    for (i=0; i<6; i++) {
      (*theReturned).lvalue[i] = 999 + i;
      cout << "[OLongString::execute] return number " << (*theReturned).lvalue[i] << endl;
    }
    ((*theReturned).svalue).length(1);
    for (i=0; i<1; i++) {
      (*theReturned).svalue[i] = Tango::string_dup("Hola todos");
      cout << "[OLongString::execute] return string " << (*theReturned).svalue[i].in() << endl;
    }
    return insert(theReturned);
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}
