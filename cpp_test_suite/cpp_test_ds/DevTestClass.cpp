#include <tango.h>

#include "DevTest.h"
#include "DevTestClass.h"

#include "TypeCmds.h"
#include "SigThrow.h"
#include "IOMisc.h"

#include "IOStr1.h"
#include "IOStr2.h"
#include "IOArray1.h"
#include "IOArray2.h"
#include "IOStrArray.h"
#include "IOStruct.h"
#include "IOSeqVec.h"

#include "IOSetAttr.h"
#include "IOAddAttribute.h"



DevTestClass *DevTestClass::_instance = NULL;

//+----------------------------------------------------------------------------
//
// method : 		DevTestClass::DevTestClass()
//
// description : 	constructor for the DevTestClass
//
// in : - s : The class name
//
//-----------------------------------------------------------------------------

DevTestClass::DevTestClass(string &s):Tango::DeviceClass(s)
{

	cout2 << "Entering DevTestClass constructor" << endl;

	set_type("TestDevice");

	cout2 << "Leaving DevTestClass constructor" << endl;

}

//+----------------------------------------------------------------------------
//
// method : 		DevTestClass::instance
//
// description : 	Create the object if not already done. Otherwise, just
//			return a pointer to the object
//
// in : - name : The class name
//
//-----------------------------------------------------------------------------

DevTestClass *DevTestClass::init(const char *name)
{
	if (_instance == NULL)
	{
		try
		{
			string s(name);
			_instance = new DevTestClass(s);
		}
		catch (bad_alloc)
		{
			throw;
		}
	}
	return _instance;
}

DevTestClass *DevTestClass::instance()
{
	if (_instance == NULL)
	{
		cerr << "Class is not initialised !!" << endl;
		exit(-1);
	}
	return _instance;
}

//+----------------------------------------------------------------------------
//
// method : 		DevTestClass::command_factory
//
// description : 	Create the command object(s) and store them in the
//			command list
//
//-----------------------------------------------------------------------------

void DevTestClass::command_factory()
{
	command_list.push_back(new IOString("IOString",
					    Tango::DEV_STRING,
					    Tango::DEV_STRING,
					    "Word",
					    "the pallindrome is"));
	command_list.push_back(new IOBool("IOBool",
					    Tango::DEV_BOOLEAN,
					    Tango::DEV_BOOLEAN,
					    "Number",
					    "Not number"));
	command_list.push_back(new IOShort("IOShort",
					    Tango::DEV_SHORT,
					    Tango::DEV_SHORT,
					    "Number",
					    "Number * 2"));
	command_list.push_back(new IOLong("IOLong",
					    Tango::DEV_LONG,
					    Tango::DEV_LONG,
					    "Number",
					    "Number * 2"));
	command_list.push_back(new IOFloat("IOFloat",
					    Tango::DEV_FLOAT,
					    Tango::DEV_FLOAT,
					    "Number",
					    "Number * 2"));
	command_list.push_back(new IODouble("IODouble",
					    Tango::DEV_DOUBLE,
					    Tango::DEV_DOUBLE,
					    "Number",
					    "Number * 2"));
	command_list.push_back(new IOUShort("IOUShort",
					    Tango::DEV_USHORT,
					    Tango::DEV_USHORT,
					    "Number",
					    "Number * 2"));
	command_list.push_back(new IOULong("IOULong",
					    Tango::DEV_ULONG,
					    Tango::DEV_ULONG,
					    "Number",
					    "Number * 2"));
	command_list.push_back(new IOBooleanArray("IOBoolArray",
										   Tango::DEVVAR_BOOLEANARRAY,
										   Tango::DEVVAR_BOOLEANARRAY,
										   "Array of boolean",
										   "Echo"));
	command_list.push_back(new IOCharArray("IOCharArray",
					    Tango::DEVVAR_CHARARRAY,
					    Tango::DEVVAR_CHARARRAY,
					    "Array of char",
					    "This reversed array"));
	command_list.push_back(new IOShortArray("IOShortArray",
					    Tango::DEVVAR_SHORTARRAY,
					    Tango::DEVVAR_SHORTARRAY,
					    "Array of short",
					    "This array * 2"));
	command_list.push_back(new IOLongArray("IOLongArray",
					    Tango::DEVVAR_LONGARRAY,
					    Tango::DEVVAR_LONGARRAY,
					    "Array of long",
					    "This array * 2"));
	command_list.push_back(new IOFloatArray("IOFloatArray",
					    Tango::DEVVAR_FLOATARRAY,
					    Tango::DEVVAR_FLOATARRAY,
					    "Array of float",
					    "This array * 2"));
	command_list.push_back(new IODoubleArray("IODoubleArray",
					    Tango::DEVVAR_DOUBLEARRAY,
					    Tango::DEVVAR_DOUBLEARRAY,
					    "Array of double",
					    "This array * 2"));
	command_list.push_back(new IOUShortArray("IOUShortArray",
					    Tango::DEVVAR_USHORTARRAY,
					    Tango::DEVVAR_USHORTARRAY,
					    "Array of unsigned short",
					    "This array * 2"));
	command_list.push_back(new IOULongArray("IOULongArray",
					    Tango::DEVVAR_ULONGARRAY,
					    Tango::DEVVAR_ULONGARRAY,
					    "Array of unsigned long",
					    "This array * 2"));
	command_list.push_back(new IOStringArray("IOStringArray",
					    Tango::DEVVAR_STRINGARRAY,
					    Tango::DEVVAR_STRINGARRAY,
					    "Array of string",
					    "This reversed array "));
	command_list.push_back(new IOLongString("IOLongString",
					    Tango::DEVVAR_LONGSTRINGARRAY,
					    Tango::DEVVAR_LONGSTRINGARRAY,
					    "Array of long and string",
					    "This array of long * 2"));
	command_list.push_back(new IODoubleString("IODoubleString",
					    Tango::DEVVAR_DOUBLESTRINGARRAY,
					    Tango::DEVVAR_DOUBLESTRINGARRAY,
					    "Array of double and string",
					    "This array of long * 2"));
	command_list.push_back(new IOThrow("IOThrow",
					    Tango::DEVVAR_LONGSTRINGARRAY,
					    Tango::DEV_VOID,
					    "Error facility",
					    "An exception"));
	command_list.push_back(new IOReThrow("IOReThrow",
					    Tango::DEVVAR_LONGSTRINGARRAY,
					    Tango::DEV_VOID,
					    "Error facility",
					    "An exception"));
	command_list.push_back(new IOExcept("IOExcept",
					    Tango::DEV_VOID,
					    Tango::DEV_VOID,
					    "Void",
					    "An exception"));
	command_list.push_back(new IOVoid("IOVoid",
					    Tango::DEV_VOID,
					    Tango::DEV_VOID,
					    "void",
					    "void"));
	command_list.push_back(new IOSleep("IOSleep",
					    Tango::DEV_USHORT,
					    Tango::DEV_VOID,
					    "sleeping time",
					    "void"));
	command_list.push_back(new IOState("IOState",
					   Tango::DEV_STATE,
					   Tango::DEV_VOID,
					   "New device state",
					   "void"));
	command_list.push_back(new IORegSig("IORegSig",
					   Tango::DEV_LONG,
					   Tango::DEV_VOID,
					   "Signal number",
					   "void"));
	command_list.push_back(new IORegSigOwn("IORegSigOwn",
					   	Tango::DEV_LONG,
					   	Tango::DEV_VOID,
					   	"Signal number",
					   	"void"));
	command_list.push_back(new IOUnregSig("IOUnregSig",
					   Tango::DEV_LONG,
					   Tango::DEV_VOID,
					   "Signal number",
					   "void"));
	command_list.push_back(new IORegClassSig("IORegClassSig",
					   Tango::DEV_LONG,
					   Tango::DEV_VOID,
					   "Signal number",
					   "void"));
	command_list.push_back(new IOUnregClassSig("IOUnregClassSig",
					   Tango::DEV_LONG,
					   Tango::DEV_VOID,
					   "Signal number",
					   "void"));
	command_list.push_back(new IOStr1("IOStr1",
					   Tango::DEV_VOID,
					   Tango::DEV_STRING,
					   "void",
					   "Just a string dynamically allocated"));
	command_list.push_back(new IOPollStr1("IOPollStr1",
					   Tango::DEV_VOID,
					   Tango::DEV_STRING,
					   "void",
					   "Just a string dynamically allocated changing at each call"));
	command_list.push_back(new IOStr2("IOStr2",
					   Tango::DEV_VOID,
					   Tango::DEV_STRING,
					   "void",
					   "A constant string"));
	command_list.push_back(new IOArray1("IOArray1",
					   Tango::DEV_VOID,
					   Tango::DEVVAR_LONGARRAY,
					   "void",
					   "An array allocated"));
	command_list.push_back(new IOArray2("IOArray2",
					   Tango::DEV_VOID,
					   Tango::DEVVAR_SHORTARRAY,
					   "void",
					   "An array without copying"));
	command_list.push_back(new IOPollArray2("IOPollArray2",
					   Tango::DEV_VOID,
					   Tango::DEVVAR_SHORTARRAY,
					   "void",
					   "An array without copying changing at each call"));
	command_list.push_back(new IOStrArray("IOStrArray",
					   Tango::DEV_VOID,
					   Tango::DEVVAR_STRINGARRAY,
					   "void",
					   "A string array"));
	command_list.push_back(new IOStruct("IOStruct",
					   Tango::DEV_VOID,
					   Tango::DEVVAR_LONGSTRINGARRAY,
					   "void",
					   "A structure type"));
	command_list.push_back(new IODevListByClass("IODevListByClass",
					   Tango::DEV_STRING,
					   Tango::DEV_STRING,
					   "class name",
					   "class first device name"));
	command_list.push_back(new IODevByName("IODevByName",
					   Tango::DEV_STRING,
					   Tango::DEV_STRING,
					   "device name",
					   "device name (returned by name()"));
	command_list.push_back(new IODServDevice("IODServDevice",
					   Tango::DEV_VOID,
					   Tango::DEV_STRING,
					   "void",
					   "dserver device name"));
	command_list.push_back(new IOSetAttr("IOSetAttr",
					   Tango::DEV_LONG,
					   Tango::DEV_VOID,
					   "New attr value",
					   "void"));
	command_list.push_back(new IOAddAttribute("IOAddAttribute",
					   Tango::DEV_STRING,
					   Tango::DEV_VOID,
					   "New attribute name",
					   "void"));
	command_list.push_back(new IORemoveAttribute("IORemoveAttribute",
					   Tango::DEV_STRING,
					   Tango::DEV_VOID,
					   "Attribute name",
					   "void"));
	command_list.push_back(new IOSeqVecChar("IOSeqVecChar",
					   Tango::DEVVAR_CHARARRAY,
					   Tango::DEVVAR_CHARARRAY,
					   "Input char array",
					   "Output char array"));
	command_list.push_back(new IOSeqVecShort("IOSeqVecShort",
					   Tango::DEVVAR_SHORTARRAY,
					   Tango::DEVVAR_SHORTARRAY,
					   "Input short array",
					   "Output short array"));
	command_list.push_back(new IOSeqVecLong("IOSeqVecLong",
					   Tango::DEVVAR_LONGARRAY,
					   Tango::DEVVAR_LONGARRAY,
					   "Input long array",
					   "Output long array"));
	command_list.push_back(new IOSeqVecFloat("IOSeqVecFloat",
					   Tango::DEVVAR_FLOATARRAY,
					   Tango::DEVVAR_FLOATARRAY,
					   "Input float array",
					   "Output float array"));
	command_list.push_back(new IOSeqVecDouble("IOSeqVecDouble",
					   Tango::DEVVAR_DOUBLEARRAY,
					   Tango::DEVVAR_DOUBLEARRAY,
					   "Input double array",
					   "Output double array"));
	command_list.push_back(new IOSeqVecUShort("IOSeqVecUShort",
					   Tango::DEVVAR_USHORTARRAY,
					   Tango::DEVVAR_USHORTARRAY,
					   "Input unsigned short array",
					   "Output unsigned short array"));
	command_list.push_back(new IOSeqVecULong("IOSeqVecULong",
					   Tango::DEVVAR_ULONGARRAY,
					   Tango::DEVVAR_ULONGARRAY,
					   "Input unsigned long array",
					   "Output unsigned long array"));
	command_list.push_back(new IOSeqVecString("IOSeqVecString",
					   Tango::DEVVAR_STRINGARRAY,
					   Tango::DEVVAR_STRINGARRAY,
					   "Input string array",
					   "Output string array"));
	command_list.push_back(new IOStartPoll("IOStartPoll",
					   Tango::DEV_VOID,
					   Tango::DEV_LONG,
					   "Void",
					   "Constant number set to 11"));
//	command_list.back()->set_polling_period(400);
	command_list.push_back(new IOShortSleep("IOShortSleep",
						Tango::DEVVAR_SHORTARRAY,
						Tango::DEV_SHORT,
						"Input short and sleeping time in sec",
						"Output short (in * 2)"));
	command_list.push_back(new IOSleepExcept("IOSleepExcept",
					    	Tango::DEV_SHORT,
					    	Tango::DEV_VOID,
					    	"Sleep time (sec)",
					    	"An exception"));
	command_list.push_back(new IOExit("IOExit",
					  Tango::DEV_VOID,
					  Tango::DEV_VOID,
					  "void",
					  "void"));
	command_list.push_back(new IOTrigPoll("IOTrigPoll",
					      Tango::DEV_STRING,
					      Tango::DEV_VOID,
					      "Command's name to trig polling",
					      "void"));
	command_list.push_back(new IOAttrTrigPoll("IOAttrTrigPoll",
					      Tango::DEV_STRING,
					      Tango::DEV_VOID,
					      "Attribute's name to trig polling",
					      "void"));
	command_list.push_back(new IOInitWAttr("IOInitWAttr",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID,
					      "void",
					      "void"));
	command_list.push_back(new IOAttrThrowEx("IOAttrThrowEx",
					      Tango::DEVVAR_SHORTARRAY,
					      Tango::DEV_VOID,
					      "2 elts : Attr code and throw except flag",
					      "void"));
	command_list.push_back(new IOAddOneElt("IOAddOneElt",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID,
					      "void",
					      "void"));
	command_list.push_back(new IORemoveOneElt("IORemoveOneElt",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID,
					      "void",
					      "void"));
	command_list.push_back(new IOIncValue("IOIncValue",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID,
					      "void",
					      "void"));
	command_list.push_back(new IODecValue("IODecValue",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID,
					      "void",
					      "void"));
	command_list.push_back(new IOChangeQuality("IOChangeQuality",
					      Tango::DEV_SHORT,
					      Tango::DEV_VOID,
					      "0->VALID, 1->INVALID, 2->ALARM, 3->CHANGING",
					      "void"));
	command_list.push_back(new IOPushEvent("IOPushEvent",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID,
					      "void",
					      "void"));
	command_list.push_back(new IOPushDevEncodedEvent("IOPushDevEncodedEvent",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID,
					      "void",
					      "void"));
	command_list.push_back(new IOSubscribeEvent("IOSubscribeEvent",
						  Tango::DEVVAR_STRINGARRAY,
					      Tango::DEV_LONG,
					      "str[0] = device name, str[1] = attribute name, str[2] = event type",
					      "Event identifier"));
	command_list.push_back(new IOUnSubscribeEvent("IOUnSubscribeEvent",
					      Tango::DEV_LONG,
					      Tango::DEV_VOID,
					      "Event identifier",
					      "void"));
	command_list.push_back(new IOGetCbExecuted("IOGetCbExecuted",
					      Tango::DEV_VOID,
					      Tango::DEV_LONG,
					      "void",
					      "Number of times the CB has been executed"));
	command_list.push_back(new IOFillPollBuffAttr("IOFillPollBuffAttr",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID,
					      "void",
					      "void"));
	command_list.push_back(new IOFillPollBuffEncodedAttr("IOFillPollBuffEncodedAttr",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID,
					      "void",
					      "void"));
	command_list.push_back(new IOFillPollBuffCmd("IOFillPollBuffCmd",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID,
					      "void",
					      "void"));
	command_list.push_back(new FileDbCmd("FileDb",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID,
					      "void",
					      "void"));
	command_list.push_back(new OLong("OLong",
					      Tango::DEV_VOID,
					      Tango::DEV_LONG,
					      "void",
					      "A long"));
	command_list.push_back(new OULong("OULong",
					      Tango::DEV_VOID,
					      Tango::DEV_ULONG,
					      "void",
					      "An unsigned long"));
	command_list.push_back(new OLongArray("OLongArray",
					      Tango::DEV_VOID,
					      Tango::DEVVAR_LONGARRAY,
					      "void",
					      "A long array"));
	command_list.push_back(new OULongArray("OULongArray",
					      Tango::DEV_VOID,
					      Tango::DEVVAR_ULONGARRAY,
					      "void",
					      "A unsigned long array"));
	command_list.push_back(new OLongString("OLongString",
					      Tango::DEV_VOID,
					      Tango::DEVVAR_LONGSTRINGARRAY,
					      "void",
					      "A unsigned long and string array"));
	command_list.push_back(new GetLongSize("GetLongSize",
					      Tango::DEV_VOID,
					      Tango::DEV_SHORT,
					      "void",
					      "Sizeof long (32 or 64 bits)"));
	command_list.push_back(new IOSetWAttrLimit("IOSetWAttrLimit",
					      Tango::DEVVAR_DOUBLEARRAY,
					      Tango::DEV_VOID,
					      "arr[0]==0 -> min_value, arr[0]==1 -> max_value, arr[1]=limit",
					      "void"));
	command_list.push_back(new IOLong64("IOLong64",
					    Tango::DEV_LONG64,
					    Tango::DEV_LONG64,
					    "Number",
					    "Number * 2"));
	command_list.push_back(new IOULong64("IOULong64",
					    Tango::DEV_ULONG64,
					    Tango::DEV_ULONG64,
					    "Number",
					    "Number * 2"));
	command_list.push_back(new ChangeEncodedFormat("ChangeEncodedFormat",
					    Tango::DEV_VOID,
					    Tango::DEV_VOID,
					    "void",
					    "void"));
	command_list.push_back(new ChangeEncodedData("ChangeEncodedData",
					    Tango::DEV_VOID,
					    Tango::DEV_VOID,
					    "void",
					    "void"));
	command_list.push_back(new PushDataReady("PushDataReady",
					    Tango::DEVVAR_LONGSTRINGARRAY,
					    Tango::DEV_VOID,
					    "s[0] = attribute name, l[0] = user counter",
					    "void"));
	command_list.push_back(new SubDeviceTst("SubDeviceTst",
					    Tango::DEV_VOID,
					    Tango::DEV_BOOLEAN,
					    "void",
					    "true = sub device connected"));
	command_list.push_back(new IOEncoded("IOEncoded",
					    Tango::DEV_ENCODED,
					    Tango::DEV_ENCODED,
					    "DevEncoded structure",
					    "DevEncoded structure"));
	command_list.push_back(new OEncoded("OEncoded",
					    Tango::DEV_VOID,
					    Tango::DEV_ENCODED,
					    "void",
					    "DevEncoded structure to test polling/history"));
	command_list.push_back(new PollingPoolTst("PollingPoolTst",
					    Tango::DEV_VOID,
					    Tango::DEVVAR_STRINGARRAY,
					    "void",
					    "Polling threads pool configuration"));
	command_list.push_back(new SetGetAlarms("SetGetAlarms",
					    Tango::DEV_VOID,
					    Tango::DEVVAR_STRINGARRAY,
					    "void",
					    "DevVarStringArray containing alarm values"));
	command_list.push_back(new SetGetRanges("SetGetRanges",
					    Tango::DEV_VOID,
					    Tango::DEVVAR_STRINGARRAY,
					    "void",
					    "DevVarStringArray containing ranges values"));
	command_list.push_back(new SetGetProperties("SetGetProperties",
					    Tango::DEV_VOID,
					    Tango::DEVVAR_STRINGARRAY,
					    "void",
					    "DevVarStringArray containing properties values"));
	command_list.push_back(new PollingInDeviceTst("IOPollingInDevice",
					    Tango::DEV_VOID,
					    Tango::DEVVAR_STRINGARRAY,
					    "void",
					    "Polling in device test outputs"));
	command_list.push_back(new SophisPollInDeviceTst("IOSophisticatedPollInDevice",
					    Tango::DEV_VOID,
					    Tango::DEV_VOID,
					    "void",
					    "void"));
	command_list.push_back(new GetPollMess("IOGetPollMess",
					    Tango::DEV_VOID,
					    Tango::DEVVAR_STRINGARRAY,
					    "void",
					    "SophisticatedPollInDevice polling messages"));
	command_list.push_back(new WriteAttrHardwareThrow("WriteAttrHardwareThrow",
					    Tango::DEV_SHORT,
					    Tango::DEV_VOID,
					    "Type of exception to be thrown by write_attr_hardware() method",
					    "void"));
	command_list.push_back(new IOAddCommand("IOAddCommand",
					   Tango::DEV_LONG,
					   Tango::DEV_VOID,
					   "True if command must be added at device level",
					   "void"));
	command_list.push_back(new IORemoveCommand("IORemoveCommand",
					   Tango::DEV_VOID,
					   Tango::DEV_VOID,
					   "void",
					   "void"));
	command_list.push_back(new SetEnumLabels("SetEnumLabels",
					   Tango::DEV_VOID,
					   Tango::DEV_VOID,
					   "void",
					   "void"));
	command_list.push_back(new AddEnumLabel("AddEnumLabel",
					   Tango::DEV_STRING,
					   Tango::DEV_VOID,
					   "New enum label",
					   "void"));
	command_list.push_back(new ForbiddenEnumValue("ForbiddenEnumValue",
					   Tango::DEV_SHORT,
					   Tango::DEV_VOID,
					   "New value for setting enum (Must be > 100)",
					   "void"));

	command_list.push_back(new SetPipeOutput("SetPipeOutput",
					   Tango::DEV_SHORT,
					   Tango::DEV_VOID,
					   "Select RPipe output type",
					   "void"));

	command_list.push_back(new PushPipeEvent("PushPipeEvent",
					   Tango::DEV_SHORT,
					   Tango::DEV_VOID,
					   "Pipe data type",
					   "void"));

	command_list.push_back(new PushStateStatusChangeEvent("PushStateStatusChangeEvent",
					   Tango::DEV_VOID,
					   Tango::DEV_VOID,
					   "void",
					   "void"));

	command_list.push_back(new Tango::TemplCommand((const char *)"IOTempl",
			       static_cast<Tango::CmdMethPtr>(&DevTest::IOTempl)));

	command_list.push_back(new Tango::TemplCommand((const char *)"IOTemplState",
			       static_cast<Tango::CmdMethPtr>(&DevTest::IOTempl),
			       static_cast<Tango::StateMethPtr>(&DevTest::templ_state)));

	command_list.push_back(new Tango::TemplCommandIn<Tango::DevLong>((const char *)"IOTemplIn",
				static_cast<Tango::CmdMethPtr_Lg>(&DevTest::IOTemplIn)));

	command_list.push_back(new Tango::TemplCommandIn<Tango::DevLong>((const char *)"IOTemplInState",
			       static_cast<Tango::CmdMethPtr_Lg>(&DevTest::IOTemplIn),
			       static_cast<Tango::StateMethPtr>(&DevTest::templ_state)));

	command_list.push_back(new Tango::TemplCommandOut<Tango::DevVarLongArray *>((const char *)"IOTemplOut",
				static_cast<Tango::LgA_CmdMethPtr>(&DevTest::IOTemplOut)));

	command_list.push_back(new Tango::TemplCommandOut<Tango::DevVarLongArray *>((const char *)"IOTemplOutState",
			       static_cast<Tango::LgA_CmdMethPtr>(&DevTest::IOTemplOut),
			       static_cast<Tango::StateMethPtr>(&DevTest::templ_state)));

	command_list.push_back(new Tango::TemplCommandInOut<Tango::DevDouble,Tango::DevVarDoubleArray *>((const char *)"IOTemplInOut",
				static_cast<Tango::DbA_CmdMethPtr_Db>(&DevTest::IOTemplInOut)));

	command_list.push_back(new Tango::TemplCommandInOut<Tango::DevDouble,Tango::DevVarDoubleArray *>((const char *)"IOTemplInOutState",
			       static_cast<Tango::DbA_CmdMethPtr_Db>(&DevTest::IOTemplInOut),
			       static_cast<Tango::StateMethPtr>(&DevTest::templ_state)));
}


//+----------------------------------------------------------------------------
//
// method : 		DevTestClass::device_factory
//
// description : 	Create the device object(s) and store them in the
//			device list
//
// in :			Tango_DevVarStringArray *devlist_ptr :
//			The device name list
//
//-----------------------------------------------------------------------------

void DevTestClass::device_factory(const Tango::DevVarStringArray *devlist_ptr) {
  for (unsigned long i = 0;i < devlist_ptr->length();i++) {
    cout4 << "Device name : " << (*devlist_ptr)[i] << endl;
//
// Create device and add it into the device list
//

    device_list.push_back(new DevTest(this,
				      (*devlist_ptr)[i]));

    //
    // Export device to the outside world
    //
    if ((Tango::Util::_UseDb == true) && (Tango::Util::_FileDb == false))
    	export_device(device_list.back());
    else
    	export_device(device_list.back(),(*devlist_ptr)[i]);
  }
}

void DevTestClass::device_name_factory(vector<string> &list_name)
{
	list_name.push_back("The_first_device");
	list_name.push_back("The_second_device");
}

void DevTestClass::signal_handler(long signo)
{

//
// With logging, we must attach a log message to a device
//

	cout1 << "[Class signal handler] received signal number "
	      << signo
	      << " for class "
	      << name
	      << endl;
}

void DevTestClass::attribute_factory(vector<Tango::Attr *> &att_list)
{
  att_list.push_back(new Short_attrAttr());
  att_list.push_back(new Long_attrAttr());
  att_list.back()->set_data_ready_event(true);
  att_list.push_back(new Long64_attrAttr());
  att_list.back()->set_data_ready_event(true);
  att_list.push_back(new Double_attrAttr());
  att_list.push_back(new String_attrAttr());
//  att_list.back()->set_polling_period(250);

  att_list.push_back(new Short_spec_attrAttr());
  att_list.push_back(new Long_spec_attrAttr());
  att_list.push_back(new Double_spec_attrAttr());
  att_list.push_back(new String_spec_attrAttr());

  att_list.push_back(new Short_ima_attrAttr());
  att_list.push_back(new Long_ima_attrAttr());
  att_list.push_back(new Double_ima_attrAttr());
  att_list.push_back(new String_ima_attrAttr());

  att_list.push_back(new attr_no_dataAttr());
  att_list.push_back(new attr_wrong_typeAttr());
  att_list.push_back(new attr_wrong_sizeAttr());
  att_list.push_back(new attr_no_alarmAttr());

  att_list.push_back(new Short_attr_with_wAttr());
  att_list.push_back(new Short_attr_rwAttr());
  att_list.push_back(new Short_attr_wAttr());

  Tango::UserDefaultAttrProp def_prop;
  def_prop.set_label("Test label");
  def_prop.set_description("Test description");
  def_prop.set_unit("Kilogramme");
  def_prop.set_standard_unit("100");
  def_prop.set_display_unit("Et ta soeur");
  def_prop.set_format("Tres long");
  def_prop.set_min_value("0");
  def_prop.set_max_value("100000");
  def_prop.set_min_alarm("1");
  def_prop.set_max_alarm("99999");
  def_prop.set_min_warning("2");
  def_prop.set_max_warning("99998");
  def_prop.set_delta_val("10000");
  def_prop.set_delta_t("1");
  def_prop.set_event_abs_change("30000");
  def_prop.set_event_rel_change("20000");
  def_prop.set_event_period("2000");
  def_prop.set_archive_event_abs_change("33333");
  def_prop.set_archive_event_rel_change("22222");
  def_prop.set_archive_event_period("3000");
//  Tango::Attr *at = new Tango::Attr("Long_attr_w", Tango::DEV_LONG, Tango::WRITE);
  Tango::Attr *at = new Long_attr_wAttr();
  at->set_default_properties(def_prop);
  att_list.push_back(at);

  att_list.push_back(new Double_attr_wAttr());
  att_list.push_back(new String_attr_wAttr());

  att_list.push_back(new attr_asynAttr());
  att_list.push_back(new attr_asyn_toAttr());
  att_list.push_back(new attr_asyn_exceptAttr());
  att_list.push_back(new attr_asyn_writeAttr());
  att_list.push_back(new attr_asyn_write_toAttr());
  att_list.push_back(new attr_asyn_write_exceptAttr());

  att_list.push_back(new PollLong_attrAttr());
  att_list.push_back(new PollString_spec_attrAttr());

  att_list.push_back(new attr_dq_shAttr());
  att_list.push_back(new attr_dq_loAttr());
  att_list.push_back(new attr_dq_dbAttr());
  att_list.push_back(new attr_dq_strAttr());

  Tango::UserDefaultAttrProp def_prop_1;
  def_prop_1.set_max_value("100");
//  Tango::SpectrumAttr *sat = new Tango::SpectrumAttr("Short_spec_attr_w", Tango::DEV_SHORT,Tango::WRITE, 4);
  Tango::SpectrumAttr *sat = new Short_spec_attr_wAttr();
  sat->set_default_properties(def_prop_1);
  att_list.push_back(sat);

  att_list.push_back(new Long_spec_attr_wAttr());
  att_list.push_back(new Double_spec_attr_wAttr());
  att_list.push_back(new String_spec_attr_wAttr());
  att_list.push_back(new Short_ima_attr_wAttr());
  att_list.push_back(new String_ima_attr_wAttr());

  att_list.push_back(new Short_spec_attr_rwAttr());
  att_list.push_back(new String_spec_attr_rwAttr());
  att_list.push_back(new Long_spec_attr_rwAttr());
  att_list.push_back(new Short_ima_attr_rwAttr());
  att_list.push_back(new String_ima_attr_rwAttr());

  att_list.push_back(new Event_change_tstAttr());
  att_list.push_back(new Event64_change_tstAttr());
  att_list.push_back(new Event_quality_tstAttr());
  att_list.push_back(new Long_attr_with_wAttr());

  att_list.push_back(new Short_attr_w2Attr());
  att_list.push_back(new String_attr_w2Attr());

  att_list.push_back(new Poll_buffAttr());
  att_list.push_back(new Poll_buffRWAttr());

  att_list.push_back(new Float_attrAttr());
  att_list.push_back(new Boolean_attrAttr());
  att_list.push_back(new UShort_attrAttr());
  att_list.push_back(new UChar_attrAttr());
  att_list.push_back(new ULong_attrAttr());
  att_list.push_back(new ULong64_attrAttr());

  att_list.push_back(new Float_spec_attrAttr());
  att_list.push_back(new Boolean_spec_attrAttr());
  att_list.push_back(new UShort_spec_attrAttr());
  att_list.push_back(new UChar_spec_attrAttr());

  att_list.push_back(new Float_attr_wAttr());
  att_list.push_back(new Boolean_attr_wAttr());
  att_list.push_back(new UShort_attr_wAttr());
  att_list.push_back(new UChar_attr_wAttr());

  att_list.push_back(new Float_spec_attr_wAttr());
  att_list.push_back(new Boolean_spec_attr_wAttr());
  att_list.push_back(new UShort_spec_attr_wAttr());
  att_list.push_back(new UChar_spec_attr_wAttr());

  att_list.push_back(new Float_ima_attr_wAttr());
  att_list.push_back(new UShort_ima_attr_wAttr());

  att_list.push_back(new Float_spec_attr_rwAttr());
  att_list.push_back(new UChar_spec_attr_rwAttr());

  att_list.push_back(new Boolean_ima_attr_rwAttr());
  att_list.push_back(new UShort_ima_attr_rwAttr());

  att_list.push_back(new slow_actuatorAttr());
  att_list.push_back(new fast_actuatorAttr());

  att_list.push_back(new Long64_attr_rwAttr());
  att_list.push_back(new ULong_attr_rwAttr());
  att_list.push_back(new ULong64_attr_rwAttr());
  att_list.push_back(new State_attr_rwAttr());
  att_list.push_back(new String_attr_rwAttr());

  att_list.push_back(new Long64_spec_attr_rwAttr());
  att_list.push_back(new ULong_spec_attr_rwAttr());
  att_list.push_back(new ULong64_spec_attr_rwAttr());
  att_list.push_back(new State_spec_attr_rwAttr());

  att_list.push_back(new Sub_device_tstAttr());
  att_list.push_back(new SlowAttr());
#ifndef COMPAT
  att_list.push_back(new Encoded_attr_rwAttr());
  att_list.push_back(new Encoded_attr_image());
#endif

  Tango::UserDefaultAttrProp att_no_data_prop;
  att_no_data_prop.set_description("User desc");
  att_no_data_prop.set_min_value("30");
  att_no_data_prop.set_delta_val("77");
  att_no_data_prop.set_delta_t("88");
  att_no_data_prop.set_event_rel_change("55");
  att_no_data_prop.set_event_period("1500");
  Tango::Attr *db_att = new DefUserAttr();
  db_att->set_default_properties(att_no_data_prop);
  att_list.push_back(db_att);

  Tango::Attr *uclass_att = new DefClassUserAttr();
  uclass_att->set_default_properties(att_no_data_prop);
  att_list.push_back(uclass_att);

  att_list.push_back(new DefAttr());
  att_list.push_back(new DefClassAttr());

  Tango::UserDefaultAttrProp att_enum_prop;
  vector<string> v_s;
  v_s.push_back("North");
  v_s.push_back("South");
  v_s.push_back("East");
  v_s.push_back("West");
  att_enum_prop.set_enum_labels(v_s);
  Tango::Attr *en_att = new EnumAttr();
  en_att->set_default_properties(att_enum_prop);
  en_att->set_memorized();
  en_att->set_memorized_init(true);
  att_list.push_back(en_att);

  Tango::Attr *en_s_att = new EnumSpecAttr();
  en_s_att->set_default_properties(att_enum_prop);
  att_list.push_back(en_s_att);

  att_list.push_back(new DynEnumAttr());
  att_list.push_back(new ReynaldPollAttr());
}

void DevTestClass::pipe_factory()
{
	RPipeClass	*pRPipe = new RPipeClass("RPipe",Tango::OPERATOR);
	Tango::UserDefaultPipeProp udpp;
	udpp.set_description("The read pipe");
	udpp.set_label("ReadPipeLabel");
	pRPipe->set_default_properties(udpp);
	pipe_list.push_back(pRPipe);

	RWPipeClass	*pRWPipe = new RWPipeClass("RWPipe",Tango::OPERATOR);
	udpp.set_description("The read_write pipe");
	udpp.set_label("ReadWritePipeLabel");
	pRWPipe->set_default_properties(udpp);
	pipe_list.push_back(pRWPipe);

	ConfPipeClass	*pPipe1 = new ConfPipeClass("PipeConf1",Tango::OPERATOR);
	pipe_list.push_back(pPipe1);

	ConfPipeClass	*pPipe2 = new ConfPipeClass("PipeConf2",Tango::OPERATOR);
	udpp.set_description("A Tango pipe with user defined desc");
	udpp.set_label("PipeLabel");
	pPipe2->set_default_properties(udpp);
	pipe_list.push_back(pPipe2);

	ConfPipeClass	*pPipe3 = new ConfPipeClass("PipeConf3",Tango::OPERATOR);
	udpp.description.clear();
	udpp.set_label("BasePipeLabel");
	pPipe3->set_default_properties(udpp);
	pipe_list.push_back(pPipe3);

	ConfPipeClass	*pPipe4 = new ConfPipeClass("PipeConf4",Tango::OPERATOR);
	pipe_list.push_back(pPipe4);

	ConfPipeClass	*pPipe5 = new ConfPipeClass("PipeConf5",Tango::OPERATOR);
	pipe_list.push_back(pPipe5);

	ConfPipeClass	*pPipe6 = new ConfPipeClass("PipeConf6",Tango::OPERATOR);
	udpp.label.clear();
	udpp.set_description("UserDefinedDesc");
	pPipe6->set_default_properties(udpp);
	pipe_list.push_back(pPipe6);

	ConfPipeClass	*pPipe7 = new ConfPipeClass("PipeConf7",Tango::OPERATOR);
	pipe_list.push_back(pPipe7);

	RPipeDEClass	*pRPipeDE = new RPipeDEClass("RPipeDE",Tango::OPERATOR);
	pipe_list.push_back(pRPipeDE);
}







