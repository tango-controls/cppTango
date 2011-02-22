#include <tango.h>

//
// Define classes for two commands
//

class IOAddAttribute : public Tango::Command {
public:
	IOAddAttribute(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOAddAttribute() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IORemoveAttribute : public Tango::Command {
public:
	IORemoveAttribute(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IORemoveAttribute() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};
