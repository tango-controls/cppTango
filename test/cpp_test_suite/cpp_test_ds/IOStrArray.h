#include <tango.h>

//
// Define classes for two commands
//

class IOStrArray : public Tango::Command
{
public:
	IOStrArray(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOStrArray() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

