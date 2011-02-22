#include <tango.h>


class IOVoid : public Tango::Command {
public:
	IOVoid(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOVoid() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOBool : public Tango::Command {
public:
	IOBool(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOBool() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOShort : public Tango::Command
{
public:
	IOShort(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOShort() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOLong : public Tango::Command
{
public:
	IOLong(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOLong() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOLong64 : public Tango::Command
{
public:
	IOLong64(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOLong64() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOFloat : public Tango::Command
{
public:
	IOFloat(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOFloat() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IODouble : public Tango::Command
{
public:
	IODouble(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IODouble() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOUShort : public Tango::Command
{
public:
	IOUShort(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOUShort() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOULong : public Tango::Command
{
public:
	IOULong(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOULong() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class IOULong64 : public Tango::Command
{
public:
	IOULong64(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOULong64() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOString : public Tango::Command
{
public:
	IOString(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOString() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOCharArray : public Tango::Command
{
public:
	IOCharArray(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOCharArray() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOShortArray : public Tango::Command
{
public:
	IOShortArray(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOShortArray() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOLongArray : public Tango::Command
{
public:
	IOLongArray(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOLongArray() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOFloatArray : public Tango::Command
{
public:
	IOFloatArray(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOFloatArray() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IODoubleArray : public Tango::Command
{
public:
	IODoubleArray(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IODoubleArray() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOUShortArray : public Tango::Command
{
public:
	IOUShortArray(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOUShortArray() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOULongArray : public Tango::Command
{
public:
	IOULongArray(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOULongArray() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOStringArray : public Tango::Command
{
public:
	IOStringArray(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOStringArray() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IOLongString : public Tango::Command
{
public:
	IOLongString(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IOLongString() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class IODoubleString : public Tango::Command
{
public:
	IODoubleString(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~IODoubleString() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class OLong : public Tango::Command
{
public:
	OLong(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~OLong() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class OULong : public Tango::Command
{
public:
	OULong(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~OULong() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class OLongArray : public Tango::Command
{
public:
	OLongArray(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~OLongArray() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


class OULongArray : public Tango::Command
{
public:
	OULongArray(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~OULongArray() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

class OLongString : public Tango::Command
{
public:
	OLongString(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	~OLongString() {};
	
	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};
