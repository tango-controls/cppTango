#include <tango.h>

//
// Define classes for two commands
//

class IOArray2 : public Tango::Command
{
public:
	IOArray2(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOArray2() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOPollArray2 : public Tango::Command
{
public:
	IOPollArray2(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOPollArray2() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
	
	long num;
};
