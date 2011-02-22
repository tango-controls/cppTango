#include <tango.h>

//
// Define classes for two commands
//

class IOSetAttr : public Tango::Command
{
public:
	IOSetAttr(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSetAttr() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

