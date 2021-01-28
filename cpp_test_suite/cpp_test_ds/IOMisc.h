#include <tango.h>

//
// Define classes for two commands
//

class IODServDevice : public Tango::Command
{
public:
	IODServDevice(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IODServDevice() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IODevByName : public Tango::Command
{
public:
	IODevByName(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IODevByName() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IODevListByClass : public Tango::Command
{
public:
	IODevListByClass(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IODevListByClass() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOSleep : public Tango::Command {
public:
	IOSleep(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSleep() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOState : public Tango::Command {
public:
	IOState(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOState() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOStartPoll : public Tango::Command {
public:
	IOStartPoll(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOStartPoll() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOShortSleep : public Tango::Command {
public:
	IOShortSleep(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOShortSleep() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOSleepExcept : public Tango::Command {
public:
	IOSleepExcept(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSleepExcept() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOExit : public Tango::Command {
public:
	IOExit(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOExit() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOTrigPoll : public Tango::Command {
public:
	IOTrigPoll(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOTrigPoll() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOAttrTrigPoll : public Tango::Command {
public:
	IOAttrTrigPoll(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOAttrTrigPoll() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOInitWAttr : public Tango::Command {
public:
	IOInitWAttr(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOInitWAttr() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOAttrThrowEx : public Tango::Command {
public:
	IOAttrThrowEx(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOAttrThrowEx() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOAddOneElt : public Tango::Command {
public:
	IOAddOneElt(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOAddOneElt() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IORemoveOneElt : public Tango::Command {
public:
	IORemoveOneElt(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IORemoveOneElt() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOIncValue : public Tango::Command {
public:
	IOIncValue(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOIncValue() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IODecValue : public Tango::Command {
public:
	IODecValue(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IODecValue() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOChangeQuality : public Tango::Command {
public:
	IOChangeQuality(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOChangeQuality() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOPushEvent : public Tango::Command {
public:
	IOPushEvent(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOPushEvent() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOPushDevEncodedEvent : public Tango::Command {
public:
	IOPushDevEncodedEvent(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOPushDevEncodedEvent() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOSubscribeEvent : public Tango::Command {
public:
	IOSubscribeEvent(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSubscribeEvent() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOUnSubscribeEvent : public Tango::Command {
public:
	IOUnSubscribeEvent(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOUnSubscribeEvent() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOGetCbExecuted : public Tango::Command {
public:
	IOGetCbExecuted(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOGetCbExecuted() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOFillPollBuffAttr : public Tango::Command {
public:
	IOFillPollBuffAttr(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOFillPollBuffAttr() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOFillPollBuffEncodedAttr : public Tango::Command {
public:
	IOFillPollBuffEncodedAttr(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOFillPollBuffEncodedAttr() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOFillPollBuffCmd : public Tango::Command {
public:
	IOFillPollBuffCmd(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOFillPollBuffCmd() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class FileDbCmd : public Tango::Command {
public:
	FileDbCmd(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~FileDbCmd() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class GetLongSize : public Tango::Command {
public:
	GetLongSize(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~GetLongSize() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOSetWAttrLimit : public Tango::Command {
public:
	IOSetWAttrLimit(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSetWAttrLimit() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class ChangeEncodedFormat : public Tango::Command {
public:
	ChangeEncodedFormat(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~ChangeEncodedFormat() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class ChangeEncodedData : public Tango::Command {
public:
	ChangeEncodedData(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~ChangeEncodedData() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class PushDataReady : public Tango::Command {
public:
	PushDataReady(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~PushDataReady() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class SubDeviceTst : public Tango::Command {
public:
	SubDeviceTst(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~SubDeviceTst() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class PollingPoolTst : public Tango::Command {
public:
	PollingPoolTst(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~PollingPoolTst() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class SetGetAlarms : public Tango::Command
{
public:
	SetGetAlarms(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~SetGetAlarms() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class SetGetRanges : public Tango::Command
{
public:
	SetGetRanges(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~SetGetRanges() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class SetGetProperties : public Tango::Command
{
public:
	SetGetProperties(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~SetGetProperties() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);

	void set_vect(std::vector<Tango::DevDouble> &,double,double);
};

class PollingInDeviceTst : public Tango::Command {
public:
	PollingInDeviceTst(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~PollingInDeviceTst() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class SophisPollInDeviceTst : public Tango::Command {
public:
	SophisPollInDeviceTst(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~SophisPollInDeviceTst() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class GetPollMess : public Tango::Command {
public:
	GetPollMess(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~GetPollMess() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class WriteAttrHardwareThrow : public Tango::Command {
public:
	WriteAttrHardwareThrow(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~WriteAttrHardwareThrow() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class SetEnumLabels : public Tango::Command {
public:
	SetEnumLabels(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~SetEnumLabels() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class AddEnumLabel : public Tango::Command {
public:
	AddEnumLabel(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~AddEnumLabel() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class ForbiddenEnumValue : public Tango::Command {
public:
	ForbiddenEnumValue(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~ForbiddenEnumValue() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class SetPipeOutput : public Tango::Command {
public:
	SetPipeOutput(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~SetPipeOutput() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class PushPipeEvent : public Tango::Command {
public:
	PushPipeEvent(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~PushPipeEvent() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class PushStateStatusChangeEvent : public Tango::Command {
public:
	PushStateStatusChangeEvent(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~PushStateStatusChangeEvent() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class CheckLocationTransparency : public Tango::Command
{
public:
    using Tango::Command::Command;
    ~CheckLocationTransparency() = default;

    bool is_allowed(Tango::DeviceImpl*, const CORBA::Any&);
    CORBA::Any* execute(Tango::DeviceImpl*, const CORBA::Any&);
};

class ReynaldPollThread : public omni_thread
{
public :
	ReynaldPollThread(Tango::DeviceImpl *);

private :
	void run(void *arg);
	Tango::DeviceImpl      *dev;
};
