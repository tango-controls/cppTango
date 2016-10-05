#include <tango.h>

//
// Define classes for two commands
//

class IOThrow : public Tango::Command {
public:
	IOThrow(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOThrow() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOExcept : public Tango::Command {
public:
	IOExcept(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOExcept() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOReThrow : public Tango::Command {
public:
	IOReThrow(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOReThrow() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

/*---------------------------------------------------------------------------*/


class IORegClassSig : public Tango::Command {
public:
	IORegClassSig(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IORegClassSig() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IORegSig : public Tango::Command {
public:
	IORegSig(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IORegSig() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IORegSigOwn : public Tango::Command {
public:
	IORegSigOwn(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IORegSigOwn() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOUnregClassSig : public Tango::Command {
public:
	IOUnregClassSig(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOUnregClassSig() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOUnregSig : public Tango::Command {
public:
	IOUnregSig(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOUnregSig() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

