#include <tango.h>

//
// Define classes for four commands
//

class IOAddAttribute : public Tango::Command
{
public:
	IOAddAttribute(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOAddAttribute() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IORemoveAttribute : public Tango::Command
{
public:
	IORemoveAttribute(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IORemoveAttribute() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOAddCommand : public Tango::Command
{
public:
	IOAddCommand(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOAddCommand() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IORemoveCommand : public Tango::Command
{
public:
	IORemoveCommand(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IORemoveCommand() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class DynCommand : public Tango::Command
{
public:
	DynCommand(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~DynCommand() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};
