#include <tango.h>

//
// Define classes for two commands
//

class IOArray1 : public Tango::Command
{
public:
	IOArray1(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOArray1() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

