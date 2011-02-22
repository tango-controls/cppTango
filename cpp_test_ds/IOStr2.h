#include <tango.h>

//
// Define classes for two commands
//

class IOStr2 : public Tango::Command
{
public:
	IOStr2(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOStr2() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

