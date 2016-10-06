#include "IOSeqVec.h"

#ifdef WIN32
using namespace Tango;
#endif

//+----------------------------------------------------------------------------
//
// command : 		IOSeqVecChar
// 
// description : 	Test the seq to vect and vect to seq for char type
//
//-----------------------------------------------------------------------------

IOSeqVecChar::IOSeqVecChar(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool IOSeqVecChar::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOSeqVecChar::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
    const Tango::DevVarCharArray *theInputArray;
    extract(in_any,theInputArray);
    
    vector<unsigned char> v1;
    v1 << (*theInputArray);
    cout << "[IOSeqVecChar::execute] " << v1.size() << " elt(s) in temp vector" << endl;
    Tango::DevVarCharArray *theOutputArray = new Tango::DevVarCharArray();
    (*theOutputArray) << v1;
    return insert(theOutputArray);
}

//+----------------------------------------------------------------------------
//
// command : 		IOSeqVecShort
// 
// description : 	Test the seq to vect and vect to seq for short type
//
//-----------------------------------------------------------------------------

IOSeqVecShort::IOSeqVecShort(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool IOSeqVecShort::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOSeqVecShort::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
    const Tango::DevVarShortArray *theInputArray;
    extract(in_any,theInputArray);
    
    vector<short> v1;
    v1 << (*theInputArray);
    cout << "[IOSeqVecShort::execute] " << v1.size() << " elt(s) in temp vector" << endl;
    Tango::DevVarShortArray *theOutputArray = new Tango::DevVarShortArray();
    (*theOutputArray) << v1;
    return insert(theOutputArray);
}

//+----------------------------------------------------------------------------
//
// command : 		IOSeqVecLong
// 
// description : 	Test the seq to vect and vect to seq for long type
//
//-----------------------------------------------------------------------------

IOSeqVecLong::IOSeqVecLong(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool IOSeqVecLong::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOSeqVecLong::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
    const Tango::DevVarLongArray *theInputArray;
    extract(in_any,theInputArray);
    
//    vector<long> v1;
    vector<Tango::DevLong> v1;
    v1 << (*theInputArray);
    cout << "[IOSeqVecLong::execute] " << v1.size() << " elt(s) in temp vector" << endl;
    Tango::DevVarLongArray *theOutputArray = new Tango::DevVarLongArray();
    (*theOutputArray) << v1;
    return insert(theOutputArray);
}

//+----------------------------------------------------------------------------
//
// command : 		IOSeqVecFloat
// 
// description : 	Test the seq to vect and vect to seq for float type
//
//-----------------------------------------------------------------------------

IOSeqVecFloat::IOSeqVecFloat(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool IOSeqVecFloat::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOSeqVecFloat::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
    const Tango::DevVarFloatArray *theInputArray;
    extract(in_any,theInputArray);
    
    vector<float> v1;
    v1 << (*theInputArray);
    cout << "[IOSeqVecFloat::execute] " << v1.size() << " elt(s) in temp vector" << endl;
    Tango::DevVarFloatArray *theOutputArray = new Tango::DevVarFloatArray();
    (*theOutputArray) << v1;
    return insert(theOutputArray);
}

//+----------------------------------------------------------------------------
//
// command : 		IOSeqVecDouble
// 
// description : 	Test the seq to vect and vect to seq for double type
//
//-----------------------------------------------------------------------------

IOSeqVecDouble::IOSeqVecDouble(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool IOSeqVecDouble::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOSeqVecDouble::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
    const Tango::DevVarDoubleArray *theInputArray;
    extract(in_any,theInputArray);
    
    vector<double> v1;
    v1 << (*theInputArray);
    cout << "[IOSeqVecDouble::execute] " << v1.size() << " elt(s) in temp vector" << endl;
    Tango::DevVarDoubleArray *theOutputArray = new Tango::DevVarDoubleArray();
    (*theOutputArray) << v1;
    return insert(theOutputArray);
}

//+----------------------------------------------------------------------------
//
// command : 		IOSeqVecUShort
// 
// description : 	Test the seq to vect and vect to seq for unsigned short type
//
//-----------------------------------------------------------------------------

IOSeqVecUShort::IOSeqVecUShort(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool IOSeqVecUShort::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOSeqVecUShort::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
    const Tango::DevVarUShortArray *theInputArray;
    extract(in_any,theInputArray);
    
    vector<unsigned short> v1;
    v1 << (*theInputArray);
    cout << "[IOSeqVecUShort::execute] " << v1.size() << " elt(s) in temp vector" << endl;
    Tango::DevVarUShortArray *theOutputArray = new Tango::DevVarUShortArray();
    (*theOutputArray) << v1;
    return insert(theOutputArray);
}

//+----------------------------------------------------------------------------
//
// command : 		IOSeqVecULong
// 
// description : 	Test the seq to vect and vect to seq for unsigned long type
//
//-----------------------------------------------------------------------------

IOSeqVecULong::IOSeqVecULong(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool IOSeqVecULong::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOSeqVecULong::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
    const Tango::DevVarULongArray *theInputArray;
    extract(in_any,theInputArray);
    
//    vector<unsigned long> v1;
    vector<Tango::DevULong> v1;
    v1 << (*theInputArray);
    cout << "[IOSeqVecULong::execute] " << v1.size() << " elt(s) in temp vector" << endl;
    Tango::DevVarULongArray *theOutputArray = new Tango::DevVarULongArray();
    (*theOutputArray) << v1;
    return insert(theOutputArray);
}

//+----------------------------------------------------------------------------
//
// command : 		IOSeqVecString
// 
// description : 	Test the seq to vect and vect to seq for string type
//
//-----------------------------------------------------------------------------

IOSeqVecString::IOSeqVecString(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool IOSeqVecString::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOSeqVecString::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{	
    const Tango::DevVarStringArray *theInputArray;
    extract(in_any,theInputArray);
    
    vector<string> v1;
    v1 << (*theInputArray);
    cout << "[IOSeqVecString::execute] " << v1.size() << " elt(s) in temp vector" << endl;
    Tango::DevVarStringArray *theOutputArray = new Tango::DevVarStringArray();
    (*theOutputArray) << v1;
    return insert(theOutputArray);
}
