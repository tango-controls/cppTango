//
// dsapi.h - include file for TANGO device api
//
// 

#ifndef _DEVAPI_H
#define _DEVAPI_H

#include <tango.h>
#include <vector>

namespace Tango {

//
// forward declarations

class DeviceData;
class DbDevice;
class DbDatum;

typedef vector<DbDatum> DbData;

typedef struct _DevCommandInfo {
	string cmd_name;
	long cmd_tag;
	long in_type;
	long out_type;
	string in_type_desc;
	string out_type_desc;
	} DevCommandInfo;

typedef vector<DevCommandInfo> DevCommandInfoList;

class Connection 
{
private :
	void create_orb();

protected :
	string host;
	string port;
	string ior;
	CORBA::ORB *orb;
	Tango::Device_var device;
	int timeout;

public :
	Connection(CORBA::ORB *orb = NULL);
	~Connection();

	void connect(string name);
	void reconnect();
	void set_timeout(int timeout);
	int get_timeout();
	CORBA::ORB *get_orb() {return orb;};

	DeviceData command_inout(string);
	DeviceData command_inout(string, DeviceData&);
	CORBA::Any_var command_inout(string, CORBA::Any&);
};

//
// DeviceProxy - device proxy object for implementing generic high-level
//               interface for TANGO device objects
//

class DeviceProxy: public Tango::Connection
{
private :
	Tango::DbDevice *db_dev;
	string device_name;

public :
	DeviceProxy(string name, CORBA::ORB *orb=NULL);
	~DeviceProxy();

//
// general methods
//
	string get_info();
	string status();
	int ping();
//
// device methods
//
	DevCommandInfoList command_list_query();
//
// property methods
//
	void get_property(DbData&);
	void put_property(DbData&);
	void delete_property(DbData&);
	void get_attribute_property(string, DbData&);
	void put_attribute_property(string, DbData&);
	void delete_attribute_property(string, DbData&);
};

class DeviceData
{

public :
//
// constructor methods
//
        enum except_flags {
                no_flags,
                isempty_flag,
                wrongtype_flag};

	DeviceData();
	DeviceData(const DeviceData &);
	~DeviceData();
	int size();
	bool is_empty();
	bool any_is_null();
	void exceptions(except_flags);
	CORBA::Any_var any;
//
// insert methods
//
	void operator << (short&);
	void operator << (unsigned short&);
	void operator << (long&);
	void operator << (unsigned long&);
	void operator << (float&);
	void operator << (double&);
	void operator << (char*&);
	void operator << (const char*&);
	void operator << (string&);
	void operator << (vector<string>&);
	void operator << (vector<short>&);
	void operator << (vector<unsigned short>&);
	void operator << (vector<long>&);
	void operator << (vector<unsigned long>&);
	void operator << (vector<float>&);
	void operator << (vector<double>&);
	void insert(vector<long>&, vector<string>&);
	void insert(vector<double>&, vector<string>&);
//
// extract methods
//
	void operator >> (short&);
	void operator >> (unsigned short&);
	void operator >> (long&);
	void operator >> (unsigned long&);
	void operator >> (float&);
	void operator >> (double&);
	void operator >> (const char*&);
	void operator >> (string&);
	void operator >> (vector<string>&);
	void operator >> (vector<short>&);
	void operator >> (vector<unsigned short>&);
	void operator >> (vector<long>&);
	void operator >> (vector<unsigned long>&);
	void operator >> (vector<float>&);
	void operator >> (vector<double>&);
	void extract(vector<long>&, vector<string>&);
	void extract(vector<double>&, vector<string>&);


private :

	int value_type;
	int value_size;
	except_flags exceptions_flags;
};
}
#endif /* _DEVAPI_H */
