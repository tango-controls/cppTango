#include "IOMisc.h"
#include "DevTest.h"

//+----------------------------------------------------------------------------
//
// method : 		IODServDevice::IODServDevice()
//
// description : 	constructor for the IODServDevice command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IODServDevice::IODServDevice(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool IODServDevice::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *IODServDevice::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
  try {
    cout << "[IODServDevice::execute]" << endl;

    Tango::DeviceImpl *dev;
    Tango::Util *tg = Tango::Util::instance();
    dev = tg->get_dserver_device();

    CORBA::String_var d_name = dev->name();
    return insert(static_cast<const char *>(d_name));
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IODevByName::IODevByName()
//
// description : 	constructor for the IODevByName command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IODevByName::IODevByName(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}

bool IODevByName::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IODevByName::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
  try {
    Tango::DevString dev_name;
    extract(in_any,dev_name);
    cout << "[IODevByName::execute] received dev name " << dev_name << endl;

    Tango::DeviceImpl *dev;
    Tango::Util *tg = Tango::Util::instance();
    dev = tg->get_device_by_name(dev_name);

    CORBA::String_var d_name = dev->name();
    return insert(static_cast<const char *>(d_name));
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IODevListByClass::IODevListByClass()
//
// description : 	constructor for the IODevListByClass command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IODevListByClass::IODevListByClass(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool IODevListByClass::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IODevListByClass::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
  try {
    Tango::DevString class_name;
    extract(in_any,class_name);
    cout << "[IODevListByClass::execute] received class name " << class_name << endl;

    vector<Tango::DeviceImpl *> d_list;
    Tango::Util *tg = Tango::Util::instance();
    d_list = tg->get_device_list_by_class(class_name);

    CORBA::String_var d_name = d_list[0]->name();
    char *t = strrchr(d_name.inout(),'/');
    if (t != NULL)
    {
    	*t='\0';
    }

    return insert(static_cast<const char *>(d_name));
  }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
      throw ;
    }
}


//+----------------------------------------------------------------------------
//
// method : 		IOSleep::IOSleep()
//
// description : 	constructor for the IOSleep command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOSleep::IOSleep(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}


bool IOSleep::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOSleep::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
  try {
    Tango::DevUShort sleeping_Time;

    extract(in_any,sleeping_Time);
    cout << "[IOSleep::execute] sleeping time " << sleeping_Time << endl;
#ifdef WIN32
    Sleep(sleeping_Time);
#else
    sleep(sleeping_Time);
#endif
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
// method : 		IOState::IOState()
//
// description : 	constructor for the IOState command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOState::IOState(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOState::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOState::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
  try {
    Tango::DevState theState;
    extract(in_any,theState);
    cout << "[IOState::execute] received state " << theState << endl;
    device->set_state(theState);
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
// method : 		IOStartPoll::IOStartPoll()
//
// description : 	constructor for the IOState command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOStartPoll::IOStartPoll(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOStartPoll::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOStartPoll::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	long theNumber = 11;
    	return insert(theNumber);
}


//+----------------------------------------------------------------------------
//
// method : 		IOShortSleep::IOShortSleep()
//
// description : 	constructor for the IOState command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOShortSleep::IOShortSleep(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOShortSleep::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOShortSleep::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	const Tango::DevVarShortArray *in_array;
	extract(in_any,in_array);

	Tango_sleep((*in_array)[1]);
	short ret = (*in_array)[0] * 2;
    	return insert(ret);
}


//+----------------------------------------------------------------------------
//
// method : 		IOSleepExcept::IOSleepExcept()
//
// description : 	constructor for the IOState command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOSleepExcept::IOSleepExcept(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOSleepExcept::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOSleepExcept::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	Tango::DevShort in;
	extract(in_any,in);

	Tango_sleep(in);

  	Tango::Except::throw_exception((const char *)"aaa",
  			         	(const char *)"This is a test ",
			         	(const char *)"IOSleepExcept::execute()");

    	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IOExit::IOExit()
//
// description : 	constructor for the IOState command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOExit::IOExit(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOExit::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}

CORBA::Any *IOExit::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	exit(0);

	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IOTrigPoll::IOTrigPoll()
//
// description : 	constructor for the IOTrigPoll command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOTrigPoll::IOTrigPoll(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOTrigPoll::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOTrigPoll::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	Tango::DevString cmd_name;
	extract(in_any,cmd_name);

	Tango::Util *tg = Tango::Util::instance();

#ifndef COMPAT
	tg->trigger_cmd_polling(device,cmd_name);
#else
	string name(cmd_name);
	tg->trigger_cmd_polling(device,name);
#endif

    return insert();
}


//+----------------------------------------------------------------------------
//
// method : 		IOAttrTrigPoll::IOAttrTrigPoll()
//
// description : 	constructor for the IOAttrTrigPoll command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOAttrTrigPoll::IOAttrTrigPoll(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOAttrTrigPoll::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOAttrTrigPoll::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	Tango::DevString att_name;
	extract(in_any,att_name);

	Tango::Util *tg = Tango::Util::instance();

#ifndef COMPAT
	tg->trigger_attr_polling(device,att_name);
#else
	string name(att_name);
	tg->trigger_attr_polling(device,name);
#endif

    return insert();
}


//+----------------------------------------------------------------------------
//
// method : 		IOInitWAttr::IOInitWAttr()
//
// description : 	constructor for the IOInitWAttr command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOInitWAttr::IOInitWAttr(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOInitWAttr::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOInitWAttr::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
// Give some default value to some writable attribute

	Tango::WAttribute &w_sh_attr = device->get_device_attr()->get_w_attr_by_name("Short_attr_w");
	w_sh_attr.set_write_value((Tango::DevShort)10);

	Tango::WAttribute &w_lg_attr = device->get_device_attr()->get_w_attr_by_name("Long_attr_w");
	w_lg_attr.set_write_value((Tango::DevLong)100);

	Tango::WAttribute &w_db_attr = device->get_device_attr()->get_w_attr_by_name("Double_attr_w");
	w_db_attr.set_write_value((Tango::DevDouble)1.1);

	Tango::WAttribute &w_st_attr = device->get_device_attr()->get_w_attr_by_name("String_attr_w");
	w_st_attr.set_write_value((Tango::DevString)"Init");

    	return insert();
}


//+----------------------------------------------------------------------------
//
// method : 		IOAttrThrowEx::IOAttrThrowEx()
//
// description : 	constructor for the IOAttrThrowEx command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOAttrThrowEx::IOAttrThrowEx(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOAttrThrowEx::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOAttrThrowEx::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	const Tango::DevVarShortArray *in;
	extract(in_any,in);

	if ((*in)[0] == 0)
	{
		if ((*in)[1] == 0)
			(static_cast<DevTest *>(device))->Short_attr_except = false;
		else
			(static_cast<DevTest *>(device))->Short_attr_except = true;
	}
	else if ((*in)[0] == 1)
	{
		if ((*in)[1] == 0)
			(static_cast<DevTest *>(device))->event_change_attr_except = false;
		else
			(static_cast<DevTest *>(device))->event_change_attr_except = true;
	}
	else if ((*in)[0] == 2)
	{
		if ((*in)[1] == 0)
			(static_cast<DevTest *>(device))->event_quality_attr_except = false;
		else
			(static_cast<DevTest *>(device))->event_quality_attr_except = true;
	}
	else if ((*in)[0] == 3)
	{
		if ((*in)[1] == 0)
			(static_cast<DevTest *>(device))->event_throw_out_of_sync = false;
		else
			(static_cast<DevTest *>(device))->event_throw_out_of_sync = true;
	}
    	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IOAddOneElt::IOAddOneElt()
//
// description : 	constructor for the IOAddOneElt command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOAddOneElt::IOAddOneElt(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOAddOneElt::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOAddOneElt::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	(static_cast<DevTest *>(device))->attr_event_size++;
	(static_cast<DevTest *>(device))->attr_event64_size++;

    	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IORemoveOneElt::IORemoveOneElt()
//
// description : 	constructor for the IORemoveOneElt command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IORemoveOneElt::IORemoveOneElt(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IORemoveOneElt::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IORemoveOneElt::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	(static_cast<DevTest *>(device))->attr_event_size--;
	(static_cast<DevTest *>(device))->attr_event64_size--;

    	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IOIncValue::IOIncValue()
//
// description : 	constructor for the IOIncValue command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOIncValue::IOIncValue(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOIncValue::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOIncValue::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	((static_cast<DevTest *>(device))->attr_event[2])++;
	((static_cast<DevTest *>(device))->attr_event64[0])++;

    	return insert();
}


//+----------------------------------------------------------------------------
//
// method : 		IODecValue::IODecValue()
//
// description : 	constructor for the IODecValue command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IODecValue::IODecValue(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IODecValue::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IODecValue::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	((static_cast<DevTest *>(device))->attr_event[2])--;
	((static_cast<DevTest *>(device))->attr_event64[0])--;

    	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IOChangeQuality::IOChangeQuality()
//
// description : 	constructor for the IOChangeQuality command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOChangeQuality::IOChangeQuality(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOChangeQuality::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOChangeQuality::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
    	Tango::DevShort theShort;
    	extract(in_any,theShort);

	switch (theShort)
	{
	case 0 : (static_cast<DevTest *>(device))->attr_event_qua = Tango::ATTR_VALID;
		  break;

	case 1 : (static_cast<DevTest *>(device))->attr_event_qua = Tango::ATTR_INVALID;
		  break;

	case 2 : (static_cast<DevTest *>(device))->attr_event_qua = Tango::ATTR_ALARM;
		  break;

	case 3 : (static_cast<DevTest *>(device))->attr_event_qua = Tango::ATTR_CHANGING;
		  break;
	}

    	return insert();
}


//+----------------------------------------------------------------------------
//
// method : 		IOPushEvent::IOPushEvent()
//
// description : 	constructor for the IOPushEvent command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOPushEvent::IOPushEvent(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOPushEvent::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOPushEvent::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	static_cast<DevTest *>(device)->IOPushEvent();

    	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IOPushDevEncodedEvent::IOPushDevEncodedEvent()
//
// description : 	constructor for the IOPushDevEncodedEvent command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOPushDevEncodedEvent::IOPushDevEncodedEvent(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOPushDevEncodedEvent::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOPushDevEncodedEvent::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	static_cast<DevTest *>(device)->IOPushDevEncodedEvent();

	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IOSubscribeEvent::IOSubscribeEvent()
//
// description : 	constructor for the IOSubscribeEvent command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOSubscribeEvent::IOSubscribeEvent(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOSubscribeEvent::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOSubscribeEvent::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	static_cast<DevTest *>(device)->IOSubscribeEvent();

    	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IOUnSubscribeEvent::IOUnSubscribeEvent()
//
// description : 	constructor for the IOUnSubscribeEvent command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOUnSubscribeEvent::IOUnSubscribeEvent(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOUnSubscribeEvent::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

		return(true);
}



CORBA::Any *IOUnSubscribeEvent::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	static_cast<DevTest *>(device)->IOUnSubscribeEvent();

    	return insert();
}


//+----------------------------------------------------------------------------
//
// method : 		IOGetCbExecuted::IOGetCbExecuted()
//
// description : 	constructor for the IOGetCbExecuted command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOGetCbExecuted::IOGetCbExecuted(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOGetCbExecuted::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

	return(true);
}



CORBA::Any *IOGetCbExecuted::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
    	cout << "[IOGetCbExecuted::execute] received number " << endl;

	long exec = (static_cast<DevTest *>(device))->cb.cb_executed;
    	return insert(exec);
}


//+----------------------------------------------------------------------------
//
// method : 		IOFillPollBuffAttr::IOFillPollBuffAttr()
//
// description : 	constructor for the IOFillPollBuffAttr command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOFillPollBuffAttr::IOFillPollBuffAttr(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOFillPollBuffAttr::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

	return(true);
}



CORBA::Any *IOFillPollBuffAttr::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	(static_cast<DevTest *>(device))->IOFillPollBuffAttr();
    	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IOFillPollBuffEncodedAttr::IOFillPollBuffEncodedAttr()
//
// description : 	constructor for the IOFillPollBuffEncodedAttr command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOFillPollBuffEncodedAttr::IOFillPollBuffEncodedAttr(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOFillPollBuffEncodedAttr::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

	return(true);
}



CORBA::Any *IOFillPollBuffEncodedAttr::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	(static_cast<DevTest *>(device))->IOFillPollBuffEncodedAttr();
    	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IOFillPollBuffCmd::IOFillPollBuffCmd()
//
// description : 	constructor for the IOFillPollBuffCmd command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOFillPollBuffCmd::IOFillPollBuffCmd(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOFillPollBuffCmd::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

	return(true);
}



CORBA::Any *IOFillPollBuffCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	(static_cast<DevTest *>(device))->IOFillPollBuffCmd();
    	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		FileDbCmd::FileDbCmd()
//
// description : 	constructor for the FileDbCmd command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

FileDbCmd::FileDbCmd(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool FileDbCmd::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

	return(true);
}



CORBA::Any *FileDbCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	(static_cast<DevTest *>(device))->FileDb();
    	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		GetLongSize::GetLongSize()
//
// description : 	constructor for the GetLongSize command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

GetLongSize::GetLongSize(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool GetLongSize::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

	return(true);
}



CORBA::Any *GetLongSize::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	short size;
#ifdef TANGO_LONG64
	size = 64;
#else
	size = 32;
#endif
    	return insert(size);
}


//+----------------------------------------------------------------------------
//
// method : 		ChangeEncodedFormat::ChangeEncodedFormat()
//
// description : 	constructor for the ChangeEncodedFormat command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

ChangeEncodedFormat::ChangeEncodedFormat(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool ChangeEncodedFormat::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

	return(true);
}



CORBA::Any *ChangeEncodedFormat::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	static bool togle = false;

#ifndef COMPAT
	if (togle == false)
	{
		(static_cast<DevTest *>(device))->enc_attr.encoded_format = CORBA::string_dup("This format");
		togle = true;
	}
	else
	{
		(static_cast<DevTest *>(device))->enc_attr.encoded_format = CORBA::string_dup("Another format");
		togle = false;
	}
#endif

	return insert();
}


//+----------------------------------------------------------------------------
//
// method : 		ChangeEncodedData::ChangeEncodedData()
//
// description : 	constructor for the ChangeEncodedFormat command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

ChangeEncodedData::ChangeEncodedData(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool ChangeEncodedData::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

	return(true);
}



CORBA::Any *ChangeEncodedData::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
#ifndef COMPAT
	(static_cast<DevTest *>(device))->enc_attr.encoded_data[2]++;
#endif
	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		PushDataReady::PushDataReady()
//
// description : 	constructor for the PushDataReady command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

PushDataReady::PushDataReady(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool PushDataReady::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

	return(true);
}



CORBA::Any *PushDataReady::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	const Tango::DevVarLongStringArray *in;
	extract(in_any,in);

	(static_cast<DevTest *>(device))->push_data_ready(in);
	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		IOSetWAttrLimit::IOSetWAttrLimit()
//
// description : 	constructor for the IOSetWAttrLimit command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

IOSetWAttrLimit::IOSetWAttrLimit(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool IOSetWAttrLimit::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allways allowed
//

	return(true);
}



CORBA::Any *IOSetWAttrLimit::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	const Tango::DevVarDoubleArray *in;
	extract(in_any,in);

	(static_cast<DevTest *>(device))->IOSetWAttrLimit(in);
    	return insert();
}


//+----------------------------------------------------------------------------
//	A thread class to test the registration of
//	sub device connections in an external thread
//
//	The thread is executed when calling
//	the command SubDeviceTst.
//-----------------------------------------------------------------------------

class AcquisitionThread : public omni_thread
{
public :
	AcquisitionThread();

private :
	void *run_undetached(void *arg);
};

AcquisitionThread::AcquisitionThread () : omni_thread()
{
	start_undetached();
}

void *AcquisitionThread::run_undetached (void *arg)
{
	Tango::Util *tg = Tango::Util::instance();

//
// sort the devices in the ascending name order
//

	vector<Tango::DeviceImpl *> &dev_list = tg->get_device_list_by_class("DevTest");
	vector<Tango::DeviceImpl *> dev_list_sorted = dev_list;

	size_t n = dev_list_sorted.size();
	// the second device on the list is selected to be the sub device, so the list has to comprise of 2 or more elements
	if(n > 1) {
		// bubble sort
		do {
			size_t i = 0;
			for(i; i < n-1; i++)
			{
				if(dev_list_sorted[i]->get_name() > dev_list_sorted[i+1]->get_name())
				{
					Tango::DeviceImpl *dev_tmp;
					dev_tmp = dev_list_sorted[i];
					dev_list_sorted[i] = dev_list_sorted[i+1];
					dev_list_sorted[i+1] = dev_tmp;
				}
			}
			n = i;
		}
		while(n != 1);

		cout << "Thread : Connect device = " << dev_list_sorted[1]->get_name() << endl;

		try
		{
			Tango::DeviceProxy *dev = new Tango::DeviceProxy(dev_list_sorted[1]->get_name());
		}
		catch(...)
		{
		}
	}

	return NULL;
}

//+----------------------------------------------------------------------------
//
// method : 		SubDeviceTst::SubDeviceTst()
//
// description : 	constructor for the SubDeviceTst command of the
//					DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

SubDeviceTst::SubDeviceTst(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool SubDeviceTst::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
//
// command allways allowed
//
	return(true);
}

CORBA::Any *SubDeviceTst::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
	bool connected = false;

	try
	{
		omni_thread *acquisition_thread = new AcquisitionThread();

		Tango::Util *tg = Tango::Util::instance();


//
// sort the devices in the ascending name order
//

		vector<Tango::DeviceImpl *> &dev_list = tg->get_device_list_by_class("DevTest");
		vector<Tango::DeviceImpl *> dev_list_sorted = dev_list;

		size_t n = dev_list_sorted.size();
		// the third device on the list is selected to be the sub device, so the list has to comprise of 3 or more elements
		if(n > 2) {
			// bubble sort
			do {
				size_t i = 0;
				for(i; i < n-1; i++)
				{
					if(dev_list_sorted[i]->get_name() > dev_list_sorted[i+1]->get_name())
					{
						Tango::DeviceImpl *dev_tmp;
						dev_tmp = dev_list_sorted[i];
						dev_list_sorted[i] = dev_list_sorted[i+1];
						dev_list_sorted[i+1] = dev_tmp;
					}
				}
				n = i;
			}
			while(n != 1);

			Tango::DeviceProxy *remote_dev;
			remote_dev = new Tango::DeviceProxy(dev_list_sorted[2]->get_name());
			connected = true;
		}
		else
			connected = false;
	}
	catch (...)
	{
		connected = false;
	}

	return insert(connected);
}


//+----------------------------------------------------------------------------
//
// method : 		PollingPoolTst::PollingPoolTst()
//
// description : 	constructor for the PollingPoolTst command of the
//					DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

PollingPoolTst::PollingPoolTst(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool PollingPoolTst::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
//
// command allways allowed
//
	return(true);
}

CORBA::Any *PollingPoolTst::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
    Tango::DevVarStringArray *theOutputArray = new Tango::DevVarStringArray();
    vector<string> pool_conf;

    Tango::Util *tg = Tango::Util::instance();
    tg->build_first_pool_conf(pool_conf);

    (*theOutputArray) << pool_conf;
    return insert(theOutputArray);
}


//+----------------------------------------------------------------------------
//
// method : 		PollingInDeviceTst::PollingInDeviceTst()
//
// description : 	constructor for the PollingInDeviceTst command of the
//					DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

PollingInDeviceTst::PollingInDeviceTst(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Tango::Command(name,in,out,in_desc,out_desc)
{
}

bool PollingInDeviceTst::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
//
// command allways allowed
//
	return(true);
}

CORBA::Any *PollingInDeviceTst::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
    Tango::DevVarStringArray *theOutputArray = (static_cast<DevTest *>(device))->IOPollingInDevice();
    return insert(theOutputArray);
}


//+----------------------------------------------------------------------------
//
// method : 		IOSetGetRanges::IOSetGetRanges()
//
// description : 	constructor for the IOSetGetRanges command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

SetGetRanges::SetGetRanges(const char *name,Tango::CmdArgType in,
		   Tango::CmdArgType out,const char *in_desc,
		   const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}


bool SetGetRanges::is_allowed(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *SetGetRanges::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
  try {
    cout << "[SetGetRanges::execute]" << endl;

    Tango::MultiAttribute *attributes = device->get_device_attr();
//    Tango::Attribute attr;
    Tango::DevVarStringArray *ranges = new Tango::DevVarStringArray();
//	ranges.length(4);
	TangoSys_MemStream str;
	vector<string> ranges_vec;

	{
//		{
//		Tango::Attribute &attr = attributes->get_attr_by_name("Long_attr");
////		Tango::DevLong lg_min_alarm = 20;
//		Tango::AttributeConfig attr_cfg;
//		attr.get_properties(attr_cfg);
//		attr_cfg.min_alarm = "15";
//		attr_cfg.description = AlrmValueNotSpec;
//		attr_cfg.min_value = AlrmValueNotSpec;
//		attr.upd_database(attr_cfg, device->get_name());
//		attr.set_properties(attr_cfg, device);
////		attr.set_min_alarm(lg_min_alarm);
//		return insert(ranges);
//		}

		Tango::Attribute &attr = attributes->get_attr_by_name("Double_attr");
		Tango::DevDouble db, db_min_alarm = -999.99, db_min_warning = -888.88, db_max_warning = 888.88, db_max_alarm = 999.99;
		attr.set_min_alarm(db_min_alarm);
		attr.set_min_warning(db_min_warning);
		attr.set_max_warning(db_max_warning);
		attr.set_max_alarm(db_max_alarm);
		attr.get_min_alarm(db);
		str << attr.get_name();
		ranges_vec.push_back(str.str());
		str.str("");
		str.clear();
		str << db;
		ranges_vec.push_back(str.str());
		attr.get_min_warning(db);
		str.str("");
		str.clear();
		str << db;
		ranges_vec.push_back(str.str());
		attr.get_max_warning(db);
		str.str("");
		str.clear();
		str << db;
		ranges_vec.push_back(str.str());
		attr.get_max_alarm(db);
		str.str("");
		str.clear();
		str << db;
		ranges_vec.push_back(str.str());
	}

	{
		Tango::Attribute &attr = attributes->get_attr_by_name("Float_attr");
		Tango::DevFloat fl, fl_min_alarm = -777.77, fl_min_warning = -666.66, fl_max_warning = 666.66, fl_max_alarm = 777.77;
		attr.set_min_alarm(fl_min_alarm);
		attr.set_min_warning(fl_min_warning);
		attr.set_max_warning(fl_max_warning);
		attr.set_max_alarm(fl_max_alarm);
		attr.get_min_alarm(fl);
		str.str("");
		str.clear();
		str << attr.get_name();
		ranges_vec.push_back(str.str());
		str.str("");
		str.clear();
		str << fl;
		ranges_vec.push_back(str.str());
		attr.get_min_warning(fl);
		str.str("");
		str.clear();
		str << fl;
		ranges_vec.push_back(str.str());
		attr.get_max_warning(fl);
		str.str("");
		str.clear();
		str << fl;
		ranges_vec.push_back(str.str());
		attr.get_max_alarm(fl);
		str.str("");
		str.clear();
		str << fl;
		ranges_vec.push_back(str.str());
	}

	{
		Tango::Attribute &attr = attributes->get_attr_by_name("Long_attr");
		Tango::DevLong lg, lg_min_alarm = 1000, lg_min_warning = 1100, lg_max_warning = 1400, lg_max_alarm = 1500;
		attr.set_min_alarm(lg_min_alarm);
		attr.set_min_warning(lg_min_warning);
		attr.set_max_warning(lg_max_warning);
		attr.set_max_alarm(lg_max_alarm);
		attr.get_min_alarm(lg);
		str.str("");
		str.clear();
		str << attr.get_name();
		ranges_vec.push_back(str.str());
		str.str("");
		str.clear();
		str << lg;
		ranges_vec.push_back(str.str());
		attr.get_min_warning(lg);
		str.str("");
		str.clear();
		str << lg;
		ranges_vec.push_back(str.str());
		attr.get_max_warning(lg);
		str.str("");
		str.clear();
		str << lg;
		ranges_vec.push_back(str.str());
		attr.get_max_alarm(lg);
		str.str("");
		str.clear();
		str << lg;
		ranges_vec.push_back(str.str());
	}

	{
		Tango::Attribute &attr = attributes->get_attr_by_name("Long64_attr");
		Tango::DevLong64 lg64, lg64_min_alarm = -90000, lg64_min_warning = -80000, lg64_max_warning = 80000, lg64_max_alarm = 90000;
		attr.set_min_alarm(lg64_min_alarm);
		attr.set_min_warning(lg64_min_warning);
		attr.set_max_warning(lg64_max_warning);
		attr.set_max_alarm(lg64_max_alarm);
		attr.get_min_alarm(lg64);
		str.str("");
		str.clear();
		str << attr.get_name();
		ranges_vec.push_back(str.str());
		str.str("");
		str.clear();
		str << lg64;
		ranges_vec.push_back(str.str());
		attr.get_min_warning(lg64);
		str.str("");
		str.clear();
		str << lg64;
		ranges_vec.push_back(str.str());
		attr.get_max_warning(lg64);
		str.str("");
		str.clear();
		str << lg64;
		ranges_vec.push_back(str.str());
		attr.get_max_alarm(lg64);
		str.str("");
		str.clear();
		str << lg64;
		ranges_vec.push_back(str.str());
	}

	{
		Tango::Attribute &attr = attributes->get_attr_by_name("Short_attr");
		Tango::DevShort sh, sh_min_alarm = -5000, sh_min_warning = -4000, sh_max_warning = 4000, sh_max_alarm = 5000;
		attr.set_min_alarm(sh_min_alarm);
		attr.set_min_warning(sh_min_warning);
		attr.set_max_warning(sh_max_warning);
		attr.set_max_alarm(sh_max_alarm);
		attr.get_min_alarm(sh);
		str.str("");
		str.clear();
		str << attr.get_name();
		ranges_vec.push_back(str.str());
		str.str("");
		str.clear();
		str << sh;
		ranges_vec.push_back(str.str());
		attr.get_min_warning(sh);
		str.str("");
		str.clear();
		str << sh;
		ranges_vec.push_back(str.str());
		attr.get_max_warning(sh);
		str.str("");
		str.clear();
		str << sh;
		ranges_vec.push_back(str.str());
		attr.get_max_alarm(sh);
		str.str("");
		str.clear();
		str << sh;
		ranges_vec.push_back(str.str());
	}

	{
		Tango::Attribute &attr = attributes->get_attr_by_name("UChar_attr");
		Tango::DevUChar uch, uch_min_alarm = 0, uch_min_warning = 1, uch_max_warning = 240, uch_max_alarm = 250;
		attr.set_min_alarm(uch_min_alarm);
		attr.set_min_warning(uch_min_warning);
		attr.set_max_warning(uch_max_warning);
		attr.set_max_alarm(uch_max_alarm);
		attr.get_min_alarm(uch);
		str.str("");
		str.clear();
		str << attr.get_name();
		ranges_vec.push_back(str.str());
		str.str("");
		str.clear();
		str << (short)uch;
		ranges_vec.push_back(str.str());
		attr.get_min_warning(uch);
		str.str("");
		str.clear();
		str << (short)uch;
		ranges_vec.push_back(str.str());
		attr.get_max_warning(uch);
		str.str("");
		str.clear();
		str << (short)uch;
		ranges_vec.push_back(str.str());
		attr.get_max_alarm(uch);
		str.str("");
		str.clear();
		str << (short)uch;
		ranges_vec.push_back(str.str());
	}

	{
		Tango::Attribute &attr = attributes->get_attr_by_name("ULong_attr");
		Tango::DevULong ulg, ulg_min_alarm = 0, ulg_min_warning = 1, ulg_max_warning = 666666, ulg_max_alarm = 777777;
		attr.set_min_alarm(ulg_min_alarm);
		attr.set_min_warning(ulg_min_warning);
		attr.set_max_warning(ulg_max_warning);
		attr.set_max_alarm(ulg_max_alarm);
		attr.get_min_alarm(ulg);
		str.str("");
		str.clear();
		str << attr.get_name();
		ranges_vec.push_back(str.str());
		str.str("");
		str.clear();
		str << ulg;
		ranges_vec.push_back(str.str());
		attr.get_min_warning(ulg);
		str.str("");
		str.clear();
		str << ulg;
		ranges_vec.push_back(str.str());
		attr.get_max_warning(ulg);
		str.str("");
		str.clear();
		str << ulg;
		ranges_vec.push_back(str.str());
		attr.get_max_alarm(ulg);
		str.str("");
		str.clear();
		str << ulg;
		ranges_vec.push_back(str.str());
	}

	{
		Tango::Attribute &attr = attributes->get_attr_by_name("ULong64_attr");
		Tango::DevULong64 ulg64, ulg64_min_alarm = 0, ulg64_min_warning = 1, ulg64_max_warning = 88888888, ulg64_max_alarm = 99999999;
		attr.set_min_alarm(ulg64_min_alarm);
		attr.set_min_warning(ulg64_min_warning);
		attr.set_max_warning(ulg64_max_warning);
		attr.set_max_alarm(ulg64_max_alarm);
		attr.get_min_alarm(ulg64);
		str.str("");
		str.clear();
		str << attr.get_name();
		ranges_vec.push_back(str.str());
		str.str("");
		str.clear();
		str << ulg64;
		ranges_vec.push_back(str.str());
		attr.get_min_warning(ulg64);
		str.str("");
		str.clear();
		str << ulg64;
		ranges_vec.push_back(str.str());
		attr.get_max_warning(ulg64);
		str.str("");
		str.clear();
		str << ulg64;
		ranges_vec.push_back(str.str());
		attr.get_max_alarm(ulg64);
		str.str("");
		str.clear();
		str << ulg64;
		ranges_vec.push_back(str.str());
	}

	{
		Tango::Attribute &attr = attributes->get_attr_by_name("UShort_attr");
		Tango::DevUShort ush, ush_min_alarm = 0, ush_min_warning = 1, ush_max_warning = 20000, ush_max_alarm = 30000;
		attr.set_min_alarm(ush_min_alarm);
		attr.set_min_warning(ush_min_warning);
		attr.set_max_warning(ush_max_warning);
		attr.set_max_alarm(ush_max_alarm);
		attr.get_min_alarm(ush);
		str.str("");
		str.clear();
		str << attr.get_name();
		ranges_vec.push_back(str.str());
		str.str("");
		str.clear();
		str << ush;
		ranges_vec.push_back(str.str());
		attr.get_min_warning(ush);
		str.str("");
		str.clear();
		str << ush;
		ranges_vec.push_back(str.str());
		attr.get_max_warning(ush);
		str.str("");
		str.clear();
		str << ush;
		ranges_vec.push_back(str.str());
		attr.get_max_alarm(ush);
		str.str("");
		str.clear();
		str << ush;
		ranges_vec.push_back(str.str());
	}

    ranges->length(ranges_vec.size());
    for(unsigned int i = 0; i < ranges_vec.size(); i++)
    {
    	cout << ranges_vec[i] << endl;
    	(*ranges)[i] = Tango::string_dup(ranges_vec[i].c_str());
    }

//    Tango::DevVarStringArray names;
//    names.length(1);
//    names[0] = "Long_attr";
//    Tango::AttributeConfigList *config = (static_cast<DevTest *>(device))->get_attribute_config(names);
//    cout << "Configuration length : " << config->length() << endl;

    cout << "Ranges have been set" << endl;

//    Tango::DeviceImpl *dev;
//    Tango::Util *tg = Tango::Util::instance();
//    dev = tg->get_get_dserver_device();
//
//    Tango::MultiAttribute *attributes = dev->get_device_attr();
//    Tango::Attribute attribute = attributes->get_attr_by_name("State");
//
//    cout << "## Attribute label: " << attribute.get_label() << endl;

//    get_attr_by_ind();

//    CORBA::String_var d_name = dev->name();
//    return insert(static_cast<const char *>(d_name));
    return insert(ranges);
  }
  catch (CORBA::Exception &e)
    {
	  cout << "Exception while setting ranges" << endl;
      Tango::Except::print_exception(e);
      throw ;
    }
}
