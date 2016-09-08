#include <tango.h>

//
// Define classes for two commands
//

class IOSeqVecChar : public Tango::Command
{
public:
	IOSeqVecChar(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSeqVecChar() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOSeqVecShort : public Tango::Command
{
public:
	IOSeqVecShort(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSeqVecShort() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOSeqVecLong : public Tango::Command
{
public:
	IOSeqVecLong(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSeqVecLong() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOSeqVecFloat : public Tango::Command
{
public:
	IOSeqVecFloat(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSeqVecFloat() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOSeqVecDouble : public Tango::Command
{
public:
	IOSeqVecDouble(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSeqVecDouble() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOSeqVecUShort : public Tango::Command
{
public:
	IOSeqVecUShort(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSeqVecUShort() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOSeqVecULong : public Tango::Command
{
public:
	IOSeqVecULong(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSeqVecULong() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOSeqVecString : public Tango::Command
{
public:
	IOSeqVecString(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOSeqVecString() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};
