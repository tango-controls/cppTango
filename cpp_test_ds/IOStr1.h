#include <tango.h>

//
// Define classes for two commands
//

class IOStr1 : public Tango::Command
{
public:
	IOStr1(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOStr1() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOPollStr1 : public Tango::Command
{
public:
	IOPollStr1(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOPollStr1() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
	
	long num;
};
