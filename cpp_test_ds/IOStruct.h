#include <tango.h>

//
// Define classes for two commands
//

class IOStruct : public Tango::Command
{
public:
	IOStruct(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOStruct() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

