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

IODServDevice::IODServDevice(const char *name, Tango::CmdArgType in,
							 Tango::CmdArgType out, const char *in_desc,
							 const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}


bool IODServDevice::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *IODServDevice::execute(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{
	try
	{
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

IODevByName::IODevByName(const char *name, Tango::CmdArgType in,
						 Tango::CmdArgType out, const char *in_desc,
						 const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}

bool IODevByName::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IODevByName::execute(TANGO_UNUSED(Tango::DeviceImpl *device), const CORBA::Any &in_any)
{
	try
	{
		Tango::DevString dev_name;
		extract(in_any, dev_name);
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

IODevListByClass::IODevListByClass(const char *name, Tango::CmdArgType in,
								   Tango::CmdArgType out, const char *in_desc,
								   const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}


bool IODevListByClass::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IODevListByClass::execute(TANGO_UNUSED(Tango::DeviceImpl *device), const CORBA::Any &in_any)
{
	try
	{
		Tango::DevString class_name;
		extract(in_any, class_name);
		cout << "[IODevListByClass::execute] received class name " << class_name << endl;

		vector<Tango::DeviceImpl *> d_list;
		Tango::Util *tg = Tango::Util::instance();
		d_list = tg->get_device_list_by_class(class_name);

		CORBA::String_var d_name = d_list[0]->name();
		char *t = strrchr(d_name.inout(), '/');
		if (t != NULL)
		{
			*t = '\0';
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

IOSleep::IOSleep(const char *name, Tango::CmdArgType in,
				 Tango::CmdArgType out, const char *in_desc,
				 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}


bool IOSleep::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}


CORBA::Any *IOSleep::execute(TANGO_UNUSED(Tango::DeviceImpl *device), const CORBA::Any &in_any)
{
	try
	{
		Tango::DevUShort sleeping_Time;

		extract(in_any, sleeping_Time);
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

IOState::IOState(const char *name, Tango::CmdArgType in,
				 Tango::CmdArgType out, const char *in_desc,
				 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOState::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOState::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	try
	{
		Tango::DevState theState;
		extract(in_any, theState);
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

IOStartPoll::IOStartPoll(const char *name, Tango::CmdArgType in,
						 Tango::CmdArgType out, const char *in_desc,
						 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOStartPoll::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOStartPoll::execute(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
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

IOShortSleep::IOShortSleep(const char *name, Tango::CmdArgType in,
						   Tango::CmdArgType out, const char *in_desc,
						   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOShortSleep::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOShortSleep::execute(TANGO_UNUSED(Tango::DeviceImpl *device), const CORBA::Any &in_any)
{
	const Tango::DevVarShortArray *in_array;
	extract(in_any, in_array);

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

IOSleepExcept::IOSleepExcept(const char *name, Tango::CmdArgType in,
							 Tango::CmdArgType out, const char *in_desc,
							 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOSleepExcept::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOSleepExcept::execute(TANGO_UNUSED(Tango::DeviceImpl *device), const CORBA::Any &in_any)
{
	Tango::DevShort in;
	extract(in_any, in);

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

IOExit::IOExit(const char *name, Tango::CmdArgType in,
			   Tango::CmdArgType out, const char *in_desc,
			   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOExit::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}

CORBA::Any *IOExit::execute(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
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

IOTrigPoll::IOTrigPoll(const char *name, Tango::CmdArgType in,
					   Tango::CmdArgType out, const char *in_desc,
					   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOTrigPoll::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOTrigPoll::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	Tango::DevString cmd_name;
	extract(in_any, cmd_name);

	Tango::Util *tg = Tango::Util::instance();

#ifndef COMPAT
	tg->trigger_cmd_polling(device, cmd_name);
#else
	string name(cmd_name);
	tg->trigger_cmd_polling(device, name);
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

IOAttrTrigPoll::IOAttrTrigPoll(const char *name, Tango::CmdArgType in,
							   Tango::CmdArgType out, const char *in_desc,
							   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOAttrTrigPoll::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOAttrTrigPoll::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	Tango::DevString att_name;
	extract(in_any, att_name);

	Tango::Util *tg = Tango::Util::instance();

#ifndef COMPAT
	tg->trigger_attr_polling(device, att_name);
#else
	string name(att_name);
	tg->trigger_attr_polling(device, name);
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

IOInitWAttr::IOInitWAttr(const char *name, Tango::CmdArgType in,
						 Tango::CmdArgType out, const char *in_desc,
						 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOInitWAttr::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOInitWAttr::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

IOAttrThrowEx::IOAttrThrowEx(const char *name, Tango::CmdArgType in,
							 Tango::CmdArgType out, const char *in_desc,
							 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOAttrThrowEx::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOAttrThrowEx::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	const Tango::DevVarShortArray *in;
	extract(in_any, in);

	DevTest& dev = static_cast<DevTest&>(*device);

	const int flag_disc = (*in)[0];
	const bool flag_value = (*in)[1];
	bool default_flag = false;

	bool& flag =
		(flag_disc == 0) ? dev.Short_attr_except :
		(flag_disc == 1) ? dev.event_change_attr_except :
		(flag_disc == 2) ? dev.event_quality_attr_except :
		(flag_disc == 3) ? dev.event_throw_out_of_sync :
		(flag_disc == 4) ? dev.Short_attr_w_except :
		(flag_disc == 5) ? dev.Long_attr_except :
		default_flag;

	flag = flag_value;

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

IOAddOneElt::IOAddOneElt(const char *name, Tango::CmdArgType in,
						 Tango::CmdArgType out, const char *in_desc,
						 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOAddOneElt::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOAddOneElt::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

IORemoveOneElt::IORemoveOneElt(const char *name, Tango::CmdArgType in,
							   Tango::CmdArgType out, const char *in_desc,
							   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IORemoveOneElt::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IORemoveOneElt::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

IOIncValue::IOIncValue(const char *name, Tango::CmdArgType in,
					   Tango::CmdArgType out, const char *in_desc,
					   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOIncValue::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOIncValue::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

IODecValue::IODecValue(const char *name, Tango::CmdArgType in,
					   Tango::CmdArgType out, const char *in_desc,
					   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IODecValue::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IODecValue::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

IOChangeQuality::IOChangeQuality(const char *name, Tango::CmdArgType in,
								 Tango::CmdArgType out, const char *in_desc,
								 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOChangeQuality::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOChangeQuality::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	Tango::DevShort theShort;
	extract(in_any, theShort);

	switch (theShort)
	{
	case 0 :
		(static_cast<DevTest *>(device))->attr_event_qua = Tango::ATTR_VALID;
		break;

	case 1 :
		(static_cast<DevTest *>(device))->attr_event_qua = Tango::ATTR_INVALID;
		break;

	case 2 :
		(static_cast<DevTest *>(device))->attr_event_qua = Tango::ATTR_ALARM;
		break;

	case 3 :
		(static_cast<DevTest *>(device))->attr_event_qua = Tango::ATTR_CHANGING;
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

IOPushEvent::IOPushEvent(const char *name, Tango::CmdArgType in,
						 Tango::CmdArgType out, const char *in_desc,
						 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOPushEvent::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOPushEvent::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

IOPushDevEncodedEvent::IOPushDevEncodedEvent(const char *name, Tango::CmdArgType in,
		Tango::CmdArgType out, const char *in_desc,
		const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOPushDevEncodedEvent::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOPushDevEncodedEvent::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

IOSubscribeEvent::IOSubscribeEvent(const char *name, Tango::CmdArgType in,
								   Tango::CmdArgType out, const char *in_desc,
								   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOSubscribeEvent::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOSubscribeEvent::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	const Tango::DevVarStringArray *theInputArray;
	extract(in_any, theInputArray);

	Tango::DevLong ret = static_cast<DevTest *>(device)->IOSubscribeEvent(theInputArray);

	return insert(ret);
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

IOUnSubscribeEvent::IOUnSubscribeEvent(const char *name, Tango::CmdArgType in,
									   Tango::CmdArgType out, const char *in_desc,
									   const char *out_desc)
: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOUnSubscribeEvent::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOUnSubscribeEvent::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	Tango::DevLong in_data;
	extract(in_any, in_data);

	static_cast<DevTest *>(device)->IOUnSubscribeEvent(in_data);

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

IOGetCbExecuted::IOGetCbExecuted(const char *name, Tango::CmdArgType in,
								 Tango::CmdArgType out, const char *in_desc,
								 const char *out_desc)
: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOGetCbExecuted::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOGetCbExecuted::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	cout << "[IOGetCbExecuted::execute] received, returned value = " << (static_cast<DevTest *>(device))->cb.cb_executed << endl;

	Tango::DevLong exec = (static_cast<DevTest *>(device))->cb.cb_executed;
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

IOFillPollBuffAttr::IOFillPollBuffAttr(const char *name, Tango::CmdArgType in,
									   Tango::CmdArgType out, const char *in_desc,
									   const char *out_desc)
: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOFillPollBuffAttr::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOFillPollBuffAttr::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

IOFillPollBuffEncodedAttr::IOFillPollBuffEncodedAttr(const char *name, Tango::CmdArgType in,
		Tango::CmdArgType out, const char *in_desc,
		const char *out_desc)
: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOFillPollBuffEncodedAttr::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOFillPollBuffEncodedAttr::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

IOFillPollBuffCmd::IOFillPollBuffCmd(const char *name, Tango::CmdArgType in,
									 Tango::CmdArgType out, const char *in_desc,
									 const char *out_desc)
: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOFillPollBuffCmd::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOFillPollBuffCmd::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

FileDbCmd::FileDbCmd(const char *name, Tango::CmdArgType in,
					 Tango::CmdArgType out, const char *in_desc,
					 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool FileDbCmd::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *FileDbCmd::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

GetLongSize::GetLongSize(const char *name, Tango::CmdArgType in,
						 Tango::CmdArgType out, const char *in_desc,
						 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool GetLongSize::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *GetLongSize::execute(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
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

ChangeEncodedFormat::ChangeEncodedFormat(const char *name, Tango::CmdArgType in,
		Tango::CmdArgType out, const char *in_desc,
		const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool ChangeEncodedFormat::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *ChangeEncodedFormat::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	static bool togle = false;

#ifndef COMPAT
	if (togle == false)
	{
		(static_cast<DevTest *>(device))->enc_attr.encoded_format = Tango::string_dup("This format");
		togle = true;
	}
	else
	{
		(static_cast<DevTest *>(device))->enc_attr.encoded_format = Tango::string_dup("Another format");
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

ChangeEncodedData::ChangeEncodedData(const char *name, Tango::CmdArgType in,
									 Tango::CmdArgType out, const char *in_desc,
									 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool ChangeEncodedData::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *ChangeEncodedData::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
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

PushDataReady::PushDataReady(const char *name, Tango::CmdArgType in,
							 Tango::CmdArgType out, const char *in_desc,
							 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool PushDataReady::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *PushDataReady::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	const Tango::DevVarLongStringArray *in;
	extract(in_any, in);

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

IOSetWAttrLimit::IOSetWAttrLimit(const char *name, Tango::CmdArgType in,
								 Tango::CmdArgType out, const char *in_desc,
								 const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool IOSetWAttrLimit::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command always allowed
//

	return(true);
}



CORBA::Any *IOSetWAttrLimit::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	const Tango::DevVarDoubleArray *in;
	extract(in_any, in);

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

void *AcquisitionThread::run_undetached (TANGO_UNUSED(void *arg))
{
	Tango::Util *tg = Tango::Util::instance();

//
// sort the devices in the ascending name order
//

	vector<Tango::DeviceImpl *> &dev_list = tg->get_device_list_by_class("DevTest");
	vector<Tango::DeviceImpl *> dev_list_sorted = dev_list;

	size_t n = dev_list_sorted.size();
	// the second device on the list is selected to be the sub device, so the list has to comprise of 2 or more elements
	if (n > 1)
	{
		// bubble sort
		do
		{
			size_t i = 0;
			for (; i < n - 1; i++)
			{
				if (dev_list_sorted[i]->get_name() > dev_list_sorted[i + 1]->get_name())
				{
					Tango::DeviceImpl *dev_tmp;
					dev_tmp = dev_list_sorted[i];
					dev_list_sorted[i] = dev_list_sorted[i + 1];
					dev_list_sorted[i + 1] = dev_tmp;
				}
			}
			n = i;
		}
		while (n != 1);

		cout << "Thread : Connect device = " << dev_list_sorted[1]->get_name() << endl;

		try
		{
			Tango::DeviceProxy *dev = new Tango::DeviceProxy(dev_list_sorted[1]->get_name());
		}
		catch (...)
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

SubDeviceTst::SubDeviceTst(const char *name, Tango::CmdArgType in,
						   Tango::CmdArgType out, const char *in_desc,
						   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool SubDeviceTst::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{
//
// command always allowed
//
	return(true);
}

CORBA::Any *SubDeviceTst::execute(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
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
		if (n > 2)
		{
			// bubble sort
			do
			{
				size_t i = 0;
				for (; i < n - 1; i++)
				{
					if (dev_list_sorted[i]->get_name() > dev_list_sorted[i + 1]->get_name())
					{
						Tango::DeviceImpl *dev_tmp;
						dev_tmp = dev_list_sorted[i];
						dev_list_sorted[i] = dev_list_sorted[i + 1];
						dev_list_sorted[i + 1] = dev_tmp;
					}
				}
				n = i;
			}
			while (n != 1);

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

PollingPoolTst::PollingPoolTst(const char *name, Tango::CmdArgType in,
							   Tango::CmdArgType out, const char *in_desc,
							   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool PollingPoolTst::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{
//
// command always allowed
//
	return(true);
}

CORBA::Any *PollingPoolTst::execute(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
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

PollingInDeviceTst::PollingInDeviceTst(const char *name, Tango::CmdArgType in,
									   Tango::CmdArgType out, const char *in_desc,
									   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool PollingInDeviceTst::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{
//
// command always allowed
//
	return(true);
}

CORBA::Any *PollingInDeviceTst::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	Tango::DevVarStringArray *theOutputArray = (static_cast<DevTest *>(device))->IOPollingInDevice();
	return insert(theOutputArray);
}


//+----------------------------------------------------------------------------
//	A thread class to test the Reynald polling mode
//
//	The thread is executed when calling
//	the command IOSophisticatedPollInDevice.
//-----------------------------------------------------------------------------

ReynaldPollThread::ReynaldPollThread(Tango::DeviceImpl *_d):dev(_d)
{
    start();
}

void ReynaldPollThread::run (TANGO_UNUSED(void *arg))
{
    string att1_name("Double_spec_attr");
    string att2_name("Short_attr");
    string att3_name("ReynaldPollAttr");

    stringstream ss;

#ifdef WIN32
    Sleep(300);
#else
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 300000000;
    nanosleep(&ts,NULL);
#endif // WIN32
    DevTest *local_dev = static_cast<DevTest *>(dev);

    ss << "Attribute " << att1_name << " polling period = " << local_dev->get_att_poll_period(att1_name);
    local_dev->poll_messages.push_back(ss.str());
    ss.str("");
    ss.clear();

    ss << "Attribute " << att2_name << " polling period = " << local_dev->get_att_poll_period(att2_name);
    local_dev->poll_messages.push_back(ss.str());
    ss.str("");
    ss.clear();

    ss << "Attribute " << att3_name << " polling period = " << local_dev->get_att_poll_period(att3_name);
    local_dev->poll_messages.push_back(ss.str());
    ss.str("");
    ss.clear();

// Sleep 600 mS and do the same

#ifdef WIN32
    Sleep(600);
#else
    ts.tv_sec = 0;
    ts.tv_nsec = 600000000;
    nanosleep(&ts,NULL);
#endif

    ss << "Attribute " << att1_name << " polling period = " << local_dev->get_att_poll_period(att1_name);
    local_dev->poll_messages.push_back(ss.str());
    ss.str("");
    ss.clear();

    ss << "Attribute " << att2_name << " polling period = " << local_dev->get_att_poll_period(att2_name);
    local_dev->poll_messages.push_back(ss.str());
    ss.str("");
    ss.clear();

    ss << "Attribute " << att3_name << " polling period = " << local_dev->get_att_poll_period(att3_name);
    local_dev->poll_messages.push_back(ss.str());
    ss.str("");
    ss.clear();

// Sleep 1500 mS and do the same

#ifdef WIN32
    Sleep(1500);
#else
    ts.tv_sec = 1;
    ts.tv_nsec = 500000000;
    nanosleep(&ts,NULL);
#endif // WIN32

    ss << "Attribute " << att1_name << " polling period = " << local_dev->get_att_poll_period(att1_name);
    local_dev->poll_messages.push_back(ss.str());
    ss.str("");
    ss.clear();

    ss << "Attribute " << att2_name << " polling period = " << local_dev->get_att_poll_period(att2_name);
    local_dev->poll_messages.push_back(ss.str());
    ss.str("");
    ss.clear();

    ss << "Attribute " << att3_name << " polling period = " << local_dev->get_att_poll_period(att3_name);
    local_dev->poll_messages.push_back(ss.str());
    ss.str("");
    ss.clear();

// Stop polling

    local_dev->stop_poll_att(att1_name);
    local_dev->stop_poll_att(att2_name);
    local_dev->stop_poll_att(att3_name);

    cout << "ReynaldPollThread exiting" << endl;
}

//+----------------------------------------------------------------------------
//
// method : 		SophisPollInDeviceTst::SophisPollInDeviceTst()
//
// description : 	constructor for the SophisPollInDeviceTst command of the
//					DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

SophisPollInDeviceTst::SophisPollInDeviceTst(const char *name, Tango::CmdArgType in,
									   Tango::CmdArgType out, const char *in_desc,
									   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool SophisPollInDeviceTst::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{
//
// command always allowed
//
	return(true);
}

CORBA::Any *SophisPollInDeviceTst::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
    ReynaldPollThread *rpt = new ReynaldPollThread(device);
	(static_cast<DevTest *>(device))->IOSophisticatedPollInDevice();
	return insert();
}


//+----------------------------------------------------------------------------
//
// method : 		GetPollMess::GetPollMess()
//
// description : 	constructor for the GetPollMess command of the
//					DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

GetPollMess::GetPollMess(const char *name, Tango::CmdArgType in,
									   Tango::CmdArgType out, const char *in_desc,
									   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool GetPollMess::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{
//
// command always allowed
//
	return(true);
}

CORBA::Any *GetPollMess::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	Tango::DevVarStringArray *strs = (static_cast<DevTest *>(device))->IOGetPollMess();
	return insert(strs);
}

//+----------------------------------------------------------------------------
//
// method : 		WriteAttrHardwareThrow::WriteAttrHardwareThrow()
//
// description : 	constructor for the WriteAttrHardwareThrow command of the
//					DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

WriteAttrHardwareThrow::WriteAttrHardwareThrow(const char *name, Tango::CmdArgType in,
									   Tango::CmdArgType out, const char *in_desc,
									   const char *out_desc)
	: Tango::Command(name, in, out, in_desc, out_desc)
{
}

bool WriteAttrHardwareThrow::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{
//
// command always allowed
//
	return(true);
}

CORBA::Any *WriteAttrHardwareThrow::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	Tango::DevShort in_data;
	extract(in_any, in_data);

	(static_cast<DevTest *>(device))->set_wattr_throw(in_data);

	return insert();
}


//+----------------------------------------------------------------------------
//
// method : 		SetGetAlarms::SetGetAlarms()
//
// description : 	constructor for the IOSetGetAlarms command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

SetGetAlarms::SetGetAlarms(const char *name, Tango::CmdArgType in,
						   Tango::CmdArgType out, const char *in_desc,
						   const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}


bool SetGetAlarms::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *SetGetAlarms::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// Sets alarm values using functions executed on the server side. Reads the previously set values
// with functions called also on the server side and creates a vector of string representations
// of the read values. This vector is returned in the form of DevVarStringArray *.
//

	try
	{
		cout << "[SetGetAlarms::execute]" << endl;

		Tango::MultiAttribute *attributes = device->get_device_attr();
		Tango::DevVarStringArray *alarms = new Tango::DevVarStringArray();

		TangoSys_MemStream str;
		vector<string> alarms_vec;
		Tango::AttributeConfig_5 conf;
		Tango::Attribute *attr_ptr = NULL;

		try
		{
			Tango::Attribute &attr = attributes->get_attr_by_name("Double_attr");
			attr.get_properties(conf);
			attr_ptr = &attr;

			Tango::DevDouble db, db_min_alarm = -999.99, db_min_warning = -888.88, db_max_warning = 888.88, db_max_alarm = 999.99;
			attr.set_min_alarm(db_min_alarm);
			attr.set_min_warning(db_min_warning);
			attr.set_max_warning(db_max_warning);
			attr.set_max_alarm(db_max_alarm);

			str << attr.get_name();
			alarms_vec.push_back(str.str());
			attr.get_min_alarm(db);
			str.str("");
			str.clear();
			str << db;
			alarms_vec.push_back(str.str());
			attr.get_min_warning(db);
			str.str("");
			str.clear();
			str << db;
			alarms_vec.push_back(str.str());
			attr.get_max_warning(db);
			str.str("");
			str.clear();
			str << db;
			alarms_vec.push_back(str.str());
			attr.get_max_alarm(db);
			str.str("");
			str.clear();
			str << db;
			alarms_vec.push_back(str.str());

			attr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::Attribute &attr = attributes->get_attr_by_name("Float_attr");
			attr.get_properties(conf);
			attr_ptr = &attr;

			Tango::DevFloat fl, fl_min_alarm = -777.77, fl_min_warning = -666.66, fl_max_warning = 666.66, fl_max_alarm = 777.77;
			attr.set_min_alarm(fl_min_alarm);
			attr.set_min_warning(fl_min_warning);
			attr.set_max_warning(fl_max_warning);
			attr.set_max_alarm(fl_max_alarm);

			str.str("");
			str.clear();
			str << attr.get_name();
			alarms_vec.push_back(str.str());
			attr.get_min_alarm(fl);
			str.str("");
			str.clear();
			str << fl;
			alarms_vec.push_back(str.str());
			attr.get_min_warning(fl);
			str.str("");
			str.clear();
			str << fl;
			alarms_vec.push_back(str.str());
			attr.get_max_warning(fl);
			str.str("");
			str.clear();
			str << fl;
			alarms_vec.push_back(str.str());
			attr.get_max_alarm(fl);
			str.str("");
			str.clear();
			str << fl;
			alarms_vec.push_back(str.str());

			attr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::Attribute &attr = attributes->get_attr_by_name("Long_attr");
			attr.get_properties(conf);
			attr_ptr = &attr;

			Tango::DevLong lg, lg_min_alarm = 1000, lg_min_warning = 1100, lg_max_warning = 1400, lg_max_alarm = 1500;
			attr.set_min_alarm(lg_min_alarm);
			attr.set_min_warning(lg_min_warning);
			attr.set_max_warning(lg_max_warning);
			attr.set_max_alarm(lg_max_alarm);

			str.str("");
			str.clear();
			str << attr.get_name();
			alarms_vec.push_back(str.str());
			attr.get_min_alarm(lg);
			str.str("");
			str.clear();
			str << lg;
			alarms_vec.push_back(str.str());
			attr.get_min_warning(lg);
			str.str("");
			str.clear();
			str << lg;
			alarms_vec.push_back(str.str());
			attr.get_max_warning(lg);
			str.str("");
			str.clear();
			str << lg;
			alarms_vec.push_back(str.str());
			attr.get_max_alarm(lg);
			str.str("");
			str.clear();
			str << lg;
			alarms_vec.push_back(str.str());

			attr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::Attribute &attr = attributes->get_attr_by_name("Long64_attr");
			attr.get_properties(conf);
			attr_ptr = &attr;

			Tango::DevLong64 lg64, lg64_min_alarm = -90000, lg64_min_warning = -80000, lg64_max_warning = 80000, lg64_max_alarm = 90000;
			attr.set_min_alarm(lg64_min_alarm);
			attr.set_min_warning(lg64_min_warning);
			attr.set_max_warning(lg64_max_warning);
			attr.set_max_alarm(lg64_max_alarm);

			str.str("");
			str.clear();
			str << attr.get_name();
			alarms_vec.push_back(str.str());
			attr.get_min_alarm(lg64);
			str.str("");
			str.clear();
			str << lg64;
			alarms_vec.push_back(str.str());
			attr.get_min_warning(lg64);
			str.str("");
			str.clear();
			str << lg64;
			alarms_vec.push_back(str.str());
			attr.get_max_warning(lg64);
			str.str("");
			str.clear();
			str << lg64;
			alarms_vec.push_back(str.str());
			attr.get_max_alarm(lg64);
			str.str("");
			str.clear();
			str << lg64;
			alarms_vec.push_back(str.str());

			attr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::Attribute &attr = attributes->get_attr_by_name("Short_attr");
			attr.get_properties(conf);
			attr_ptr = &attr;

			Tango::DevShort sh, sh_min_alarm = -5000, sh_min_warning = -4000, sh_max_warning = 4000, sh_max_alarm = 5000;
			attr.set_min_alarm(sh_min_alarm);
			attr.set_min_warning(sh_min_warning);
			attr.set_max_warning(sh_max_warning);
			attr.set_max_alarm(sh_max_alarm);

			str.str("");
			str.clear();
			str << attr.get_name();
			alarms_vec.push_back(str.str());
			attr.get_min_alarm(sh);
			str.str("");
			str.clear();
			str << sh;
			alarms_vec.push_back(str.str());
			attr.get_min_warning(sh);
			str.str("");
			str.clear();
			str << sh;
			alarms_vec.push_back(str.str());
			attr.get_max_warning(sh);
			str.str("");
			str.clear();
			str << sh;
			alarms_vec.push_back(str.str());
			attr.get_max_alarm(sh);
			str.str("");
			str.clear();
			str << sh;
			alarms_vec.push_back(str.str());

			attr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::Attribute &attr = attributes->get_attr_by_name("UChar_attr");
			attr.get_properties(conf);
			attr_ptr = &attr;

			Tango::DevUChar uch, uch_min_alarm = 1, uch_min_warning = 2, uch_max_warning = 230, uch_max_alarm = 240;
			attr.set_min_alarm(uch_min_alarm);
			attr.set_min_warning(uch_min_warning);
			attr.set_max_warning(uch_max_warning);
			attr.set_max_alarm(uch_max_alarm);

			str.str("");
			str.clear();
			str << attr.get_name();
			alarms_vec.push_back(str.str());
			attr.get_min_alarm(uch);
			str.str("");
			str.clear();
			str << (short)uch;
			alarms_vec.push_back(str.str());
			attr.get_min_warning(uch);
			str.str("");
			str.clear();
			str << (short)uch;
			alarms_vec.push_back(str.str());
			attr.get_max_warning(uch);
			str.str("");
			str.clear();
			str << (short)uch;
			alarms_vec.push_back(str.str());
			attr.get_max_alarm(uch);
			str.str("");
			str.clear();
			str << (short)uch;
			alarms_vec.push_back(str.str());

			attr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::Attribute &attr = attributes->get_attr_by_name("ULong_attr");
			attr.get_properties(conf);
			attr_ptr = &attr;

			Tango::DevULong ulg, ulg_min_alarm = 1, ulg_min_warning = 2, ulg_max_warning = 666666, ulg_max_alarm = 777777;
			attr.set_min_alarm(ulg_min_alarm);
			attr.set_min_warning(ulg_min_warning);
			attr.set_max_warning(ulg_max_warning);
			attr.set_max_alarm(ulg_max_alarm);

			str.str("");
			str.clear();
			str << attr.get_name();
			alarms_vec.push_back(str.str());
			attr.get_min_alarm(ulg);
			str.str("");
			str.clear();
			str << ulg;
			alarms_vec.push_back(str.str());
			attr.get_min_warning(ulg);
			str.str("");
			str.clear();
			str << ulg;
			alarms_vec.push_back(str.str());
			attr.get_max_warning(ulg);
			str.str("");
			str.clear();
			str << ulg;
			alarms_vec.push_back(str.str());
			attr.get_max_alarm(ulg);
			str.str("");
			str.clear();
			str << ulg;
			alarms_vec.push_back(str.str());

			attr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::Attribute &attr = attributes->get_attr_by_name("ULong64_attr");
			attr.get_properties(conf);
			attr_ptr = &attr;

			Tango::DevULong64 ulg64, ulg64_min_alarm = 1, ulg64_min_warning = 2, ulg64_max_warning = 77777777, ulg64_max_alarm = 88888888;
			attr.set_min_alarm(ulg64_min_alarm);
			attr.set_min_warning(ulg64_min_warning);
			attr.set_max_warning(ulg64_max_warning);
			attr.set_max_alarm(ulg64_max_alarm);

			str.str("");
			str.clear();
			str << attr.get_name();
			alarms_vec.push_back(str.str());
			attr.get_min_alarm(ulg64);
			str.str("");
			str.clear();
			str << ulg64;
			alarms_vec.push_back(str.str());
			attr.get_min_warning(ulg64);
			str.str("");
			str.clear();
			str << ulg64;
			alarms_vec.push_back(str.str());
			attr.get_max_warning(ulg64);
			str.str("");
			str.clear();
			str << ulg64;
			alarms_vec.push_back(str.str());
			attr.get_max_alarm(ulg64);
			str.str("");
			str.clear();
			str << ulg64;
			alarms_vec.push_back(str.str());

			attr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::Attribute &attr = attributes->get_attr_by_name("UShort_attr");
			attr.get_properties(conf);
			attr_ptr = &attr;

			Tango::DevUShort ush, ush_min_alarm = 1, ush_min_warning = 2, ush_max_warning = 20000, ush_max_alarm = 30000;
			attr.set_min_alarm(ush_min_alarm);
			attr.set_min_warning(ush_min_warning);
			attr.set_max_warning(ush_max_warning);
			attr.set_max_alarm(ush_max_alarm);

			str.str("");
			str.clear();
			str << attr.get_name();
			alarms_vec.push_back(str.str());
			attr.get_min_alarm(ush);
			str.str("");
			str.clear();
			str << ush;
			alarms_vec.push_back(str.str());
			attr.get_min_warning(ush);
			str.str("");
			str.clear();
			str << ush;
			alarms_vec.push_back(str.str());
			attr.get_max_warning(ush);
			str.str("");
			str.clear();
			str << ush;
			alarms_vec.push_back(str.str());
			attr.get_max_alarm(ush);
			str.str("");
			str.clear();
			str << ush;
			alarms_vec.push_back(str.str());

			attr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		alarms->length(alarms_vec.size());
		for (unsigned int i = 0; i < alarms_vec.size(); i++)
		{
//			cout << alarms_vec[i] << endl;
			(*alarms)[i] = Tango::string_dup(alarms_vec[i].c_str());
		}

		cout << "Alarms have been set" << endl;

		return insert(alarms);
	}
	catch (CORBA::Exception &e)
	{
		cout << "Exception while setting alarms" << endl;
		Tango::Except::print_exception(e);
		throw ;
	}
}


//+----------------------------------------------------------------------------
//
// method : 		SetGetAlarms::SetGetRanges()
//
// description : 	constructor for the SetGetRanges command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

SetGetRanges::SetGetRanges(const char *name, Tango::CmdArgType in,
						   Tango::CmdArgType out, const char *in_desc,
						   const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}


bool SetGetRanges::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *SetGetRanges::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// Sets ranges values using functions executed on the server side. Reads the previously set values
// with functions called also on the server side and creates a vector of string representations
// of the read values. This vector is returned in the form of DevVarStringArray *.
//

	try
	{
		cout << "[SetGetRanges::execute]" << endl;

		Tango::MultiAttribute *attributes = device->get_device_attr();
		Tango::DevVarStringArray *ranges = new Tango::DevVarStringArray();

		TangoSys_MemStream str;
		vector<string> ranges_vec;
		Tango::AttributeConfig conf;
		Tango::WAttribute *wattr_ptr = NULL;

		try
		{
			Tango::WAttribute &wattr = attributes->get_w_attr_by_name("Double_attr_w");
			wattr.get_properties(conf);
			wattr_ptr = &wattr;

			Tango::DevDouble db, db_min_value = -1111.11, db_max_value = 1111.11;
			wattr.set_min_value(db_min_value);
			wattr.set_max_value(db_max_value);

			str << wattr.get_name();
			ranges_vec.push_back(str.str());
			wattr.get_min_value(db);
			str.str("");
			str.clear();
			str << db;
			ranges_vec.push_back(str.str());
			wattr.get_max_value(db);
			str.str("");
			str.clear();
			str << db;
			ranges_vec.push_back(str.str());

			wattr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (wattr_ptr != NULL)
				wattr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::WAttribute &wattr = attributes->get_w_attr_by_name("Float_attr_w");
			wattr.get_properties(conf);
			wattr_ptr = &wattr;

			Tango::DevFloat fl, fl_min_value = -888.88, fl_max_value = 888.88;
			wattr.set_min_value(fl_min_value);
			wattr.set_max_value(fl_max_value);

			str.str("");
			str.clear();
			str << wattr.get_name();
			ranges_vec.push_back(str.str());
			wattr.get_min_value(fl);
			str.str("");
			str.clear();
			str << fl;
			ranges_vec.push_back(str.str());
			wattr.get_max_value(fl);
			str.str("");
			str.clear();
			str << fl;
			ranges_vec.push_back(str.str());

			wattr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (wattr_ptr != NULL)
				wattr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::WAttribute &wattr = attributes->get_w_attr_by_name("Long_attr_w");
			wattr.get_properties(conf);
			wattr_ptr = &wattr;

			Tango::DevLong lg, lg_min_value = 900, lg_max_value = 1600;
			wattr.set_min_value(lg_min_value);
			wattr.set_max_value(lg_max_value);

			str.str("");
			str.clear();
			str << wattr.get_name();
			ranges_vec.push_back(str.str());
			wattr.get_min_value(lg);
			str.str("");
			str.clear();
			str << lg;
			ranges_vec.push_back(str.str());
			wattr.get_max_value(lg);
			str.str("");
			str.clear();
			str << lg;
			ranges_vec.push_back(str.str());

			wattr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (wattr_ptr != NULL)
				wattr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::WAttribute &wattr = attributes->get_w_attr_by_name("Long64_attr_rw");
			wattr.get_properties(conf);
			wattr_ptr = &wattr;

			Tango::DevLong64 lg64, lg64_min_value = -100000, lg64_max_value = 100000;
			wattr.set_min_value(lg64_min_value);
			wattr.set_max_value(lg64_max_value);

			str.str("");
			str.clear();
			str << wattr.get_name();
			ranges_vec.push_back(str.str());
			wattr.get_min_value(lg64);
			str.str("");
			str.clear();
			str << lg64;
			ranges_vec.push_back(str.str());
			wattr.get_max_value(lg64);
			str.str("");
			str.clear();
			str << lg64;
			ranges_vec.push_back(str.str());

			wattr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (wattr_ptr != NULL)
				wattr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::WAttribute &wattr = attributes->get_w_attr_by_name("Short_attr_w");
			wattr.get_properties(conf);
			wattr_ptr = &wattr;

			Tango::DevShort sh, sh_min_value = -6000, sh_max_value = 6000;
			wattr.set_min_value(sh_min_value);
			wattr.set_max_value(sh_max_value);

			str.str("");
			str.clear();
			str << wattr.get_name();
			ranges_vec.push_back(str.str());
			wattr.get_min_value(sh);
			str.str("");
			str.clear();
			str << sh;
			ranges_vec.push_back(str.str());
			wattr.get_max_value(sh);
			str.str("");
			str.clear();
			str << sh;
			ranges_vec.push_back(str.str());

			wattr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (wattr_ptr != NULL)
				wattr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::WAttribute &wattr = attributes->get_w_attr_by_name("UChar_attr_w");
			wattr.get_properties(conf);
			wattr_ptr = &wattr;

			Tango::DevUChar uch, uch_min_value = 0, uch_max_value = 250;
			wattr.set_min_value(uch_min_value);
			wattr.set_max_value(uch_max_value);

			str.str("");
			str.clear();
			str << wattr.get_name();
			ranges_vec.push_back(str.str());
			wattr.get_min_value(uch);
			str.str("");
			str.clear();
			str << (short)uch;
			ranges_vec.push_back(str.str());
			wattr.get_max_value(uch);
			str.str("");
			str.clear();
			str << (short) uch;
			ranges_vec.push_back(str.str());

			wattr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (wattr_ptr != NULL)
				wattr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::WAttribute &wattr = attributes->get_w_attr_by_name("ULong_attr_rw");
			wattr.get_properties(conf);
			wattr_ptr = &wattr;

			Tango::DevULong ulg, ulg_min_value = 0, ulg_max_value = 888888;
			wattr.set_min_value(ulg_min_value);
			wattr.set_max_value(ulg_max_value);

			str.str("");
			str.clear();
			str << wattr.get_name();
			ranges_vec.push_back(str.str());
			wattr.get_min_value(ulg);
			str.str("");
			str.clear();
			str << ulg;
			ranges_vec.push_back(str.str());
			wattr.get_max_value(ulg);
			str.str("");
			str.clear();
			str << ulg;
			ranges_vec.push_back(str.str());

			wattr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (wattr_ptr != NULL)
				wattr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::WAttribute &wattr = attributes->get_w_attr_by_name("ULong64_attr_rw");
			wattr.get_properties(conf);
			wattr_ptr = &wattr;

			Tango::DevULong64 ulg64, ulg64_min_value = 0, ulg64_max_value = 99999999;
			wattr.set_min_value(ulg64_min_value);
			wattr.set_max_value(ulg64_max_value);

			str.str("");
			str.clear();
			str << wattr.get_name();
			ranges_vec.push_back(str.str());
			wattr.get_min_value(ulg64);
			str.str("");
			str.clear();
			str << ulg64;
			ranges_vec.push_back(str.str());
			wattr.get_max_value(ulg64);
			str.str("");
			str.clear();
			str << ulg64;
			ranges_vec.push_back(str.str());

			wattr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (wattr_ptr != NULL)
				wattr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::WAttribute &wattr = attributes->get_w_attr_by_name("UShort_attr_w");
			wattr.get_properties(conf);
			wattr_ptr = &wattr;

			Tango::DevUShort ush, ush_min_value = 0, ush_max_value = 40000;
			wattr.set_min_value(ush_min_value);
			wattr.set_max_value(ush_max_value);

			str.str("");
			str.clear();
			str << wattr.get_name();
			ranges_vec.push_back(str.str());
			wattr.get_min_value(ush);
			str.str("");
			str.clear();
			str << ush;
			ranges_vec.push_back(str.str());
			wattr.get_max_value(ush);
			str.str("");
			str.clear();
			str << ush;
			ranges_vec.push_back(str.str());

			wattr.set_upd_properties(conf);
		}
		catch (Tango::DevFailed &e)
		{
			if (wattr_ptr != NULL)
				wattr_ptr->set_upd_properties(conf);
			throw e;
		}

		ranges->length(ranges_vec.size());
		for (unsigned int i = 0; i < ranges_vec.size(); i++)
		{
//			cout << ranges_vec[i] << endl;
			(*ranges)[i] = Tango::string_dup(ranges_vec[i].c_str());
		}

		cout << "Ranges have been set" << endl;

		return insert(ranges);
	}
	catch (CORBA::Exception &e)
	{
		cout << "Exception while setting ranges" << endl;
		Tango::Except::print_exception(e);
		throw ;
	}
}


//+----------------------------------------------------------------------------
//
// method : 		SetGetProperties::SetGetProperties()
//
// description : 	constructor for the SetGetProperties command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

SetGetProperties::SetGetProperties(const char *name, Tango::CmdArgType in,
								   Tango::CmdArgType out, const char *in_desc,
								   const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}


bool SetGetProperties::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

void SetGetProperties::set_vect(vector<double> &vect, double v1, double v2)
{
	vect.clear();
	vect.push_back(v1);
	vect.push_back(v2);
}

CORBA::Any *SetGetProperties::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

	Tango::Util *tg = Tango::Util::instance();
	Tango::Database *db = tg->get_database();
	db->set_timeout_millis(Tango::DB_TIMEOUT * 3);

//
// Sets properties using functions executed on the server side. Reads the previously set values
// with functions called also on the server side and creates a vector of string representations
// of the read values. This vector is returned in the form of DevVarStringArray *.
//

	try
	{
		cout << "[SetGetProperties::execute]" << endl;

		Tango::MultiAttribute *attributes = device->get_device_attr();
		Tango::DevVarStringArray *props = new Tango::DevVarStringArray();

		vector<string> props_vec;
		Tango::AttributeConfig_5 conf;
		Tango::Attribute *attr_ptr = NULL;
		vector<Tango::DevDouble> changes;

		try
		{
			Tango::MultiAttrProp<Tango::DevDouble> multi_prop, multi_prop_get;
			Tango::Attribute &attr = attributes->get_attr_by_name("Double_attr");

			attr.get_properties(conf);
			attr_ptr = &attr;

			// test properties provided as strings
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = "0";
			multi_prop.max_value = "200";
			multi_prop.min_alarm = "10";
			multi_prop.max_alarm = "190";
			multi_prop.min_warning = "20";
			multi_prop.max_warning = "180";
			multi_prop.delta_t = "5";
			multi_prop.delta_val = "10";
			multi_prop.event_period = "300";
			multi_prop.archive_period = "400";
			multi_prop.rel_change = "0.2,0.3";
			multi_prop.abs_change = "40,50";
			multi_prop.archive_rel_change = "0.60,0.70";
			multi_prop.archive_abs_change = "80,90";
			attr.set_properties(multi_prop);

Tango_sleep(1);

			attr.get_properties(multi_prop_get);

			props_vec.push_back("Double_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
			// test properties provided as actual values
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = 1.0;
			multi_prop.max_value = 201.0;
			multi_prop.min_alarm = 11.0;
			multi_prop.max_alarm = 191.0;
			multi_prop.min_warning = 21.0;
			multi_prop.max_warning = 181;
			multi_prop.delta_t = 6;
			multi_prop.delta_val = 11.0;
			multi_prop.event_period = 301;
			multi_prop.archive_period = 401;
			set_vect(changes, 0.3, 0.4);
			multi_prop.rel_change = changes;
			set_vect(changes, 41.0, 51.0);
			multi_prop.abs_change = changes;
			set_vect(changes, 0.7, 0.8);
			multi_prop.archive_rel_change = changes;
			set_vect(changes, 81.0, 91.0);
			multi_prop.archive_abs_change = changes;
			attr.set_properties(multi_prop);

			attr.get_properties(multi_prop_get);
Tango_sleep(1);
			props_vec.push_back("Double_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
		}
		catch (Tango::DevFailed &e)
		{
			db->set_timeout_millis(Tango::DB_TIMEOUT);
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::MultiAttrProp<Tango::DevFloat> multi_prop, multi_prop_get;
			Tango::Attribute &attr = attributes->get_attr_by_name("Float_attr");

			attr.get_properties(conf);
			attr_ptr = &attr;

			// test properties provided as strings
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = "0";
			multi_prop.max_value = "200";
			multi_prop.min_alarm = "10";
			multi_prop.max_alarm = "190";
			multi_prop.min_warning = "20";
			multi_prop.max_warning = "180";
			multi_prop.delta_t = "5";
			multi_prop.delta_val = "10";
			multi_prop.event_period = "300";
			multi_prop.archive_period = "400";
			multi_prop.rel_change = "0.2,0.3";
			multi_prop.abs_change = "40,50";
			multi_prop.archive_rel_change = "0.6,0.7";
			multi_prop.archive_abs_change = "80,90";
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("Float_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
			// test properties provided as actual values
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = 1.0;
			multi_prop.max_value = 201.0;
			multi_prop.min_alarm = 11.0;
			multi_prop.max_alarm = 191.0;
			multi_prop.min_warning = 21.0;
			multi_prop.max_warning = 181.0;
			multi_prop.delta_t = 6;
			multi_prop.delta_val = 11.0;
			multi_prop.event_period = 301;
			multi_prop.archive_period = 401;
			set_vect(changes, 0.3, 0.4);
			multi_prop.rel_change = changes;
			set_vect(changes, 41.0, 51.0);
			multi_prop.abs_change = changes;
			set_vect(changes, 0.7, 0.8);
			multi_prop.archive_rel_change = changes;
			set_vect(changes, 81.0, 91.0);
			multi_prop.archive_abs_change = changes;
			attr.set_properties(multi_prop);

			attr.get_properties(multi_prop_get);
Tango_sleep(1);
			props_vec.push_back("Float_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
		}
		catch (Tango::DevFailed &e)
		{
			db->set_timeout_millis(Tango::DB_TIMEOUT);
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}


		try
		{
			Tango::MultiAttrProp<Tango::DevLong> multi_prop, multi_prop_get;
			Tango::Attribute &attr = attributes->get_attr_by_name("Long_attr");

			attr.get_properties(conf);
			attr_ptr = &attr;

			// test properties provided as strings
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = "0";
			multi_prop.max_value = "200";
			multi_prop.min_alarm = "10";
			multi_prop.max_alarm = "190";
			multi_prop.min_warning = "20";
			multi_prop.max_warning = "180";
			multi_prop.delta_t = "5";
			multi_prop.delta_val = "10";
			multi_prop.event_period = "300";
			multi_prop.archive_period = "400";
			multi_prop.rel_change = "0.2,0.3";
			multi_prop.abs_change = "40,50";
			multi_prop.archive_rel_change = "0.6,0.7";
			multi_prop.archive_abs_change = "80,90";
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("Long_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
			// test properties provided as actual values
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = 1;
			multi_prop.max_value = 201;
			multi_prop.min_alarm = 11;
			multi_prop.max_alarm = 191;
			multi_prop.min_warning = 21;
			multi_prop.max_warning = 181;
			multi_prop.delta_t = 6;
			multi_prop.delta_val = 11;
			multi_prop.event_period = 301;
			multi_prop.archive_period = 401;
			set_vect(changes, 0.3, 0.4);
			multi_prop.rel_change = changes;
			set_vect(changes, 41.0, 51.0);
			multi_prop.abs_change = changes;
			set_vect(changes, 0.7, 0.8);
			multi_prop.archive_rel_change = changes;
			set_vect(changes, 81.0, 91.0);
			multi_prop.archive_abs_change = changes;
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("Long_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
		}
		catch (Tango::DevFailed &e)
		{
			db->set_timeout_millis(Tango::DB_TIMEOUT);
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::MultiAttrProp<Tango::DevLong64> multi_prop, multi_prop_get;
			Tango::Attribute &attr = attributes->get_attr_by_name("Long64_attr");

			attr.get_properties(conf);
			attr_ptr = &attr;

			// test properties provided as strings
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = "0";
			multi_prop.max_value = "200";
			multi_prop.min_alarm = "10";
			multi_prop.max_alarm = "190";
			multi_prop.min_warning = "20";
			multi_prop.max_warning = "180";
			multi_prop.delta_t = "5";
			multi_prop.delta_val = "10";
			multi_prop.event_period = "300";
			multi_prop.archive_period = "400";
			multi_prop.rel_change = "0.2,0.3";
			multi_prop.abs_change = "40,50";
			multi_prop.archive_rel_change = "0.6,0.7";
			multi_prop.archive_abs_change = "80,90";
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("Long64_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
			// test properties provided as actual values
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = 1;
			multi_prop.max_value = 201;
			multi_prop.min_alarm = 11;
			multi_prop.max_alarm = 191;
			multi_prop.min_warning = 21;
			multi_prop.max_warning = 181;
			multi_prop.delta_t = 6;
			multi_prop.delta_val = 11;
			multi_prop.event_period = 301;
			multi_prop.archive_period = 401;
			set_vect(changes, 0.3, 0.4);
			multi_prop.rel_change = changes;
			set_vect(changes, 41.0, 51.0);
			multi_prop.abs_change = changes;
			set_vect(changes, 0.7, 0.8);
			multi_prop.archive_rel_change = changes;
			set_vect(changes, 81.0, 91.0);
			multi_prop.archive_abs_change = changes;
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("Long64_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
		}
		catch (Tango::DevFailed &e)
		{
			db->set_timeout_millis(Tango::DB_TIMEOUT);
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}


		try
		{
			Tango::MultiAttrProp<Tango::DevShort> multi_prop, multi_prop_get;
			Tango::Attribute &attr = attributes->get_attr_by_name("Short_attr");

			attr.get_properties(conf);
			attr_ptr = &attr;

			// test properties provided as strings
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = "0";
			multi_prop.max_value = "200";
			multi_prop.min_alarm = "10";
			multi_prop.max_alarm = "190";
			multi_prop.min_warning = "20";
			multi_prop.max_warning = "180";
			multi_prop.delta_t = "5";
			multi_prop.delta_val = "10";
			multi_prop.event_period = "300";
			multi_prop.archive_period = "400";
			multi_prop.rel_change = "0.2,0.3";
			multi_prop.abs_change = "40,50";
			multi_prop.archive_rel_change = "0.6,0.7";
			multi_prop.archive_abs_change = "80,90";
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("Short_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
			// test properties provided as actual values
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = 1;
			multi_prop.max_value = 201;
			multi_prop.min_alarm = 11;
			multi_prop.max_alarm = 191;
			multi_prop.min_warning = 21;
			multi_prop.max_warning = 181;
			multi_prop.delta_t = 6;
			multi_prop.delta_val = 11;
			multi_prop.event_period = 301;
			multi_prop.archive_period = 401;
			set_vect(changes, 0.3, 0.4);
			multi_prop.rel_change = changes;
			set_vect(changes, 41.0, 51.0);
			multi_prop.abs_change = changes;
			set_vect(changes, 0.7, 0.8);
			multi_prop.archive_rel_change = changes;
			set_vect(changes, 81.0, 91.0);
			multi_prop.archive_abs_change = changes;
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("Short_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
		}
		catch (Tango::DevFailed &e)
		{
			db->set_timeout_millis(Tango::DB_TIMEOUT);
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}


		try
		{
			Tango::MultiAttrProp<Tango::DevUChar> multi_prop, multi_prop_get;
			Tango::Attribute &attr = attributes->get_attr_by_name("UChar_attr");

			attr.get_properties(conf);
			attr_ptr = &attr;

			// test properties provided as strings
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = "-0.5";
			multi_prop.max_value = "-200.55";
			multi_prop.min_alarm = "-10.0";
			multi_prop.max_alarm = "-190.5";
			multi_prop.min_warning = "-20.0";
			multi_prop.max_warning = "-180.0";
			multi_prop.delta_t = "5";
			multi_prop.delta_val = "10";
			multi_prop.event_period = "300";
			multi_prop.archive_period = "400";
			multi_prop.rel_change = "0.2,0.3";
			multi_prop.abs_change = "40,50";
			multi_prop.archive_rel_change = "0.6,0.7";
			multi_prop.archive_abs_change = "80,90";
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("UChar_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
			// test properties provided as actual values
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = 1;
			multi_prop.max_value = 201;
			multi_prop.min_alarm = 11;
			multi_prop.max_alarm = 191;
			multi_prop.min_warning = 21;
			multi_prop.max_warning = 181;
			multi_prop.delta_t = 6;
			multi_prop.delta_val = 11;
			multi_prop.event_period = 301;
			multi_prop.archive_period = 401;
			set_vect(changes, 0.3, 0.4);
			multi_prop.rel_change = changes;
			set_vect(changes, 41.0, 51.0);
			multi_prop.abs_change = changes;
			set_vect(changes, 0.7, 0.8);
			multi_prop.archive_rel_change = changes;
			set_vect(changes, 81.0, 91.0);
			multi_prop.archive_abs_change = changes;
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("UChar_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
		}
		catch (Tango::DevFailed &e)
		{
			db->set_timeout_millis(Tango::DB_TIMEOUT);
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::MultiAttrProp<Tango::DevULong> multi_prop, multi_prop_get;
			Tango::Attribute &attr = attributes->get_attr_by_name("ULong_attr");

			attr.get_properties(conf);
			attr_ptr = &attr;

			// test properties provided as strings
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = "-0.5";
			multi_prop.max_value = "-200.55";
			multi_prop.min_alarm = "-10.0";
			multi_prop.max_alarm = "-190.5";
			multi_prop.min_warning = "-20.0";
			multi_prop.max_warning = "-180.0";
			multi_prop.delta_t = "5";
			multi_prop.delta_val = "10";
			multi_prop.event_period = "300";
			multi_prop.archive_period = "400";
			multi_prop.rel_change = "0.2,0.3";
			multi_prop.abs_change = "40,50";
			multi_prop.archive_rel_change = "0.6,0.7";
			multi_prop.archive_abs_change = "80,90";
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("ULong_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
			// test properties provided as actual values
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = 1;
			multi_prop.max_value = 201;
			multi_prop.min_alarm = 11;
			multi_prop.max_alarm = 191;
			multi_prop.min_warning = 21;
			multi_prop.max_warning = 181;
			multi_prop.delta_t = 6;
			multi_prop.delta_val = 11;
			multi_prop.event_period = 301;
			multi_prop.archive_period = 401;
			set_vect(changes, 0.3, 0.4);
			multi_prop.rel_change = changes;
			set_vect(changes, 41.0, 51.0);
			multi_prop.abs_change = changes;
			set_vect(changes, 0.7, 0.8);
			multi_prop.archive_rel_change = changes;
			set_vect(changes, 81.0, 91.0);
			multi_prop.archive_abs_change = changes;
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("ULong_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
		}
		catch (Tango::DevFailed &e)
		{
			db->set_timeout_millis(Tango::DB_TIMEOUT);
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}


		try
		{
			Tango::MultiAttrProp<Tango::DevULong64> multi_prop, multi_prop_get;
			Tango::Attribute &attr = attributes->get_attr_by_name("ULong64_attr");

			attr.get_properties(conf);
			attr_ptr = &attr;

			// test properties provided as strings
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = "-0.5";
			multi_prop.max_value = "-200.55";
			multi_prop.min_alarm = "-10.0";
			multi_prop.max_alarm = "-190.5";
			multi_prop.min_warning = "-20.0";
			multi_prop.max_warning = "-180.0";
			multi_prop.delta_t = "5";
			multi_prop.delta_val = "10";
			multi_prop.event_period = "300";
			multi_prop.archive_period = "400";
			multi_prop.rel_change = "0.2,0.3";
			multi_prop.abs_change = "40,50";
			multi_prop.archive_rel_change = "0.6,0.7";
			multi_prop.archive_abs_change = "80,90";
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("ULong64_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
			// test properties provided as actual values
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = 1;
			multi_prop.max_value = 201;
			multi_prop.min_alarm = 11;
			multi_prop.max_alarm = 191;
			multi_prop.min_warning = 21;
			multi_prop.max_warning = 181;
			multi_prop.delta_t = 6;
			multi_prop.delta_val = 11;
			multi_prop.event_period = 301;
			multi_prop.archive_period = 401;
			set_vect(changes, 0.3, 0.4);
			multi_prop.rel_change = changes;
			set_vect(changes, 41.0, 51.0);
			multi_prop.abs_change = changes;
			set_vect(changes, 0.7, 0.8);
			multi_prop.archive_rel_change = changes;
			set_vect(changes, 81.0, 91.0);
			multi_prop.archive_abs_change = changes;
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("ULong64_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
		}
		catch (Tango::DevFailed &e)
		{
			db->set_timeout_millis(Tango::DB_TIMEOUT);
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::MultiAttrProp<Tango::DevUShort> multi_prop, multi_prop_get;
			Tango::Attribute &attr = attributes->get_attr_by_name("UShort_attr");

			attr.get_properties(conf);
			attr_ptr = &attr;

			// test properties provided as strings
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = "-0.5";
			multi_prop.max_value = "-200.55";
			multi_prop.min_alarm = "-10.0";
			multi_prop.max_alarm = "-190.5";
			multi_prop.min_warning = "-20.0";
			multi_prop.max_warning = "-180.0";
			multi_prop.delta_t = "5";
			multi_prop.delta_val = "10";
			multi_prop.event_period = "300";
			multi_prop.archive_period = "400";
			multi_prop.rel_change = "0.2,0.3";
			multi_prop.abs_change = "40,50";
			multi_prop.archive_rel_change = "0.6,0.7";
			multi_prop.archive_abs_change = "80,90";
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("UShort_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
			// test properties provided as actual values
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = 1;
			multi_prop.max_value = 201;
			multi_prop.min_alarm = 11;
			multi_prop.max_alarm = 191;
			multi_prop.min_warning = 21;
			multi_prop.max_warning = 181;
			multi_prop.delta_t = 6;
			multi_prop.delta_val = 11;
			multi_prop.event_period = 301;
			multi_prop.archive_period = 401;
			set_vect(changes, 0.3, 0.4);
			multi_prop.rel_change = changes;
			set_vect(changes, 41.0, 51.0);
			multi_prop.abs_change = changes;
			set_vect(changes, 0.7, 0.8);
			multi_prop.archive_rel_change = changes;
			set_vect(changes, 81.0, 91.0);
			multi_prop.archive_abs_change = changes;
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("UShort_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
		}
		catch (Tango::DevFailed &e)
		{
			db->set_timeout_millis(Tango::DB_TIMEOUT);
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}

		try
		{
			Tango::MultiAttrProp<Tango::DevEncoded> multi_prop, multi_prop_get;
			Tango::Attribute &attr = attributes->get_attr_by_name("Encoded_attr");

			attr.get_properties(conf);
			attr_ptr = &attr;

			// test properties provided as strings
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = "-0.5";
			multi_prop.max_value = "-200.55";
			multi_prop.min_alarm = "-10.0";
			multi_prop.max_alarm = "-190.5";
			multi_prop.min_warning = "-20.0";
			multi_prop.max_warning = "-180.0";
			multi_prop.delta_t = "5";
			multi_prop.delta_val = "10";
			multi_prop.event_period = "300";
			multi_prop.archive_period = "400";
			multi_prop.rel_change = "0.2,0.3";
			multi_prop.abs_change = "40,50";
			multi_prop.archive_rel_change = "0.6,0.7";
			multi_prop.archive_abs_change = "80,90";
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("Encoded_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
			// test properties provided as actual values
			attr.get_properties(multi_prop);
			multi_prop.label = "Test_label";
			multi_prop.description = "Test_description";
			multi_prop.unit = "Test_unit";
			multi_prop.standard_unit = "Test_standard_unit";
			multi_prop.display_unit = "Test_display_unit";
			multi_prop.format = "Test_format";
			multi_prop.min_value = 1;
			multi_prop.max_value = 201;
			multi_prop.min_alarm = 11;
			multi_prop.max_alarm = 191;
			multi_prop.min_warning = 21;
			multi_prop.max_warning = 181;
			multi_prop.delta_t = 6;
			multi_prop.delta_val = 11;
			multi_prop.event_period = 301;
			multi_prop.archive_period = 401;
			set_vect(changes, 0.3, 0.4);
			multi_prop.rel_change = changes;
			set_vect(changes, 41.0, 51.0);
			multi_prop.abs_change = changes;
			set_vect(changes, 0.7, 0.8);
			multi_prop.archive_rel_change = changes;
			set_vect(changes, 81.0, 91.0);
			multi_prop.archive_abs_change = changes;
			attr.set_properties(multi_prop);
Tango_sleep(1);
			attr.get_properties(multi_prop_get);

			props_vec.push_back("Encoded_attr");
			props_vec.push_back(multi_prop_get.label);
			props_vec.push_back(multi_prop_get.description);
			props_vec.push_back(multi_prop_get.unit);
			props_vec.push_back(multi_prop_get.standard_unit);
			props_vec.push_back(multi_prop_get.display_unit);
			props_vec.push_back(multi_prop_get.format);
			props_vec.push_back(multi_prop_get.min_value);
			props_vec.push_back(multi_prop_get.max_value);
			props_vec.push_back(multi_prop_get.min_alarm);
			props_vec.push_back(multi_prop_get.max_alarm);
			props_vec.push_back(multi_prop_get.min_warning);
			props_vec.push_back(multi_prop_get.max_warning);
			props_vec.push_back(multi_prop_get.delta_t);
			props_vec.push_back(multi_prop_get.delta_val);
			props_vec.push_back(multi_prop_get.event_period);
			props_vec.push_back(multi_prop_get.archive_period);
			props_vec.push_back(multi_prop_get.rel_change);
			props_vec.push_back(multi_prop_get.abs_change);
			props_vec.push_back(multi_prop_get.archive_rel_change);
			props_vec.push_back(multi_prop_get.archive_abs_change);

			attr.set_upd_properties(conf);
Tango_sleep(1);
		}
		catch (Tango::DevFailed &e)
		{
			db->set_timeout_millis(Tango::DB_TIMEOUT);
			if (attr_ptr != NULL)
				attr_ptr->set_upd_properties(conf);
			throw e;
		}



//		try
//		{
//			Tango::MultiAttrProp<Tango::DevDouble> multi_prop, multi_prop_get;
//			Tango::WAttribute &wattr = attributes->get_w_attr_by_name("UChar_attr_w");
//
//			attr.get_properties(conf);
//			attr_ptr = &attr;
//
//			attr.get_properties(multi_prop);
//			multi_prop.label = "Test_label";
//			multi_prop.description = "Test_description";
//			multi_prop.unit = "Test_unit";
//			multi_prop.standard_unit = "Test_standard_unit";
//			multi_prop.display_unit = "Test_display_unit";
//			multi_prop.format = "Test_format";
//			multi_prop.min_value = "0";
//			multi_prop.max_value = "200";
//			multi_prop.min_alarm = "10";
//			multi_prop.max_alarm = "190";
//			multi_prop.min_warning = "20";
//			multi_prop.max_warning = "180";
//			multi_prop.delta_t = "5";
//			multi_prop.delta_val = "10";
//			multi_prop.event_period = "300";
//			multi_prop.archive_period = "400";
//			multi_prop.rel_change = "0.2,0.3";
//			multi_prop.abs_change = "40,50";
//			multi_prop.archive_rel_change = "0.6,0.7";
//			multi_prop.archive_abs_change = "80,90";
//			attr.set_properties(multi_prop);
//
//			attr.get_properties(multi_prop_get);
//
//			props_vec.push_back(attr_names[i]);
//			props_vec.push_back(multi_prop_get.label);
//			props_vec.push_back(multi_prop_get.description);
//			props_vec.push_back(multi_prop_get.unit);
//			props_vec.push_back(multi_prop_get.standard_unit);
//			props_vec.push_back(multi_prop_get.display_unit);
//			props_vec.push_back(multi_prop_get.format);
//			props_vec.push_back(multi_prop_get.min_value);
//			props_vec.push_back(multi_prop_get.max_value);
//			props_vec.push_back(multi_prop_get.min_alarm);
//			props_vec.push_back(multi_prop_get.max_alarm);
//			props_vec.push_back(multi_prop_get.min_warning);
//			props_vec.push_back(multi_prop_get.max_warning);
//			props_vec.push_back(multi_prop_get.delta_t);
//			props_vec.push_back(multi_prop_get.delta_val);
//			props_vec.push_back(multi_prop_get.event_period);
//			props_vec.push_back(multi_prop_get.archive_period);
//			props_vec.push_back(multi_prop_get.rel_change);
//			props_vec.push_back(multi_prop_get.abs_change);
//			props_vec.push_back(multi_prop_get.archive_rel_change);
//			props_vec.push_back(multi_prop_get.archive_abs_change);
//
//			attr.set_upd_properties(conf);
//		}
//		catch(Tango::DevFailed &e)
//		{
//			db->set_timeout_millis(Tango::DB_TIMEOUT);
//			if(attr_ptr != NULL)
//				attr_ptr->set_upd_properties(conf);
//			throw e;
//		}

		props->length(props_vec.size());
		for (unsigned int i = 0; i < props_vec.size(); i++)
		{
//			cout << props_vec[i] << endl;
			(*props)[i] = Tango::string_dup(props_vec[i].c_str());
		}

		cout << "Properties have been set" << endl;

		return insert(props);
	}
	catch (CORBA::Exception &e)
	{
		db->set_timeout_millis(Tango::DB_TIMEOUT);
		cout << "Exception while setting properties" << endl;
		Tango::Except::print_exception(e);
		throw ;
	}
}

//+----------------------------------------------------------------------------
//
// method : 		SetEnumLabels::SetEnumLabels()
//
// description : 	constructor for the SetEnumLabels command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

SetEnumLabels::SetEnumLabels(const char *name, Tango::CmdArgType in,
								   Tango::CmdArgType out, const char *in_desc,
								   const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}


bool SetEnumLabels::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *SetEnumLabels::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	(static_cast<DevTest *>(device))->set_enum_labels();
	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		AddEnumLabel::AddEnumLabel()
//
// description : 	constructor for the AddEnumLabels command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

AddEnumLabel::AddEnumLabel(const char *name, Tango::CmdArgType in,
								   Tango::CmdArgType out, const char *in_desc,
								   const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}


bool AddEnumLabel::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *AddEnumLabel::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	Tango::DevString new_label;
	extract(in_any, new_label);
	(static_cast<DevTest *>(device))->add_enum_label(new_label);
	return insert();
}


//+----------------------------------------------------------------------------
//
// method : 		ForbiddenEnumValue::ForbiddenEnumValue()
//
// description : 	constructor for the ForbiddenEnumValue command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

ForbiddenEnumValue::ForbiddenEnumValue(const char *name, Tango::CmdArgType in,
								   Tango::CmdArgType out, const char *in_desc,
								   const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}


bool ForbiddenEnumValue::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *ForbiddenEnumValue::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	Tango::DevShort new_value;
	extract(in_any, new_value);

	DevTest *dev = (static_cast<DevTest *>(device));
	dev->enum_value = new_value;
	return insert();
}


//+----------------------------------------------------------------------------
//
// method : 		SetPipeOutput::SetPipeOutput()
//
// description : 	constructor for the SetPipeOutput command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

SetPipeOutput::SetPipeOutput(const char *name, Tango::CmdArgType in,
								   Tango::CmdArgType out, const char *in_desc,
								   const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}


bool SetPipeOutput::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *SetPipeOutput::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	Tango::DevShort new_value;
	extract(in_any, new_value);

	DevTest *dev = (static_cast<DevTest *>(device));
	dev->rpipe_type = new_value;
	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		PushPipeEvent::PushPipeEvent()
//
// description : 	constructor for the PushPipeEvent command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

PushPipeEvent::PushPipeEvent(const char *name, Tango::CmdArgType in,
								   Tango::CmdArgType out, const char *in_desc,
								   const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}


bool PushPipeEvent::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *PushPipeEvent::execute(Tango::DeviceImpl *device, const CORBA::Any &in_any)
{
	Tango::DevShort new_value;
	extract(in_any, new_value);

	DevTest *dev = (static_cast<DevTest *>(device));
	dev->cmd_push_pipe_event(new_value);
	return insert();
}

//+----------------------------------------------------------------------------
//
// method : 		PushStateStatusChangeEvent::PushStateStatusChangeEvent()
//
// description : 	constructor for the PushStateStatusChangeEvent command of the
//			DevTest.
//
// In : - name : The command name
//	- in : The input parameter type
//	- out : The output parameter type
//	- in_desc : The input parameter description
//	- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------

PushStateStatusChangeEvent::PushStateStatusChangeEvent(const char *name, Tango::CmdArgType in,
								   Tango::CmdArgType out, const char *in_desc,
								   const char *out_desc)
	: Command(name, in, out, in_desc, out_desc)
{
}


bool PushStateStatusChangeEvent::is_allowed(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

//
// command allowed only if the device is on
//

	if (device->get_state() == Tango::ON)
		return(true);
	else
		return(false);
}

CORBA::Any *PushStateStatusChangeEvent::execute(Tango::DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	DevTest *dev = (static_cast<DevTest *>(device));
	dev->cmd_push_state_status_event();
	return insert();
}

