//
// dbapi.h -	include file for TANGO database api
//
// 
#ifndef _DBAPI_H
#define _DBAPI_H

#include <vector>
#include <errno.h>

#include <devapi.h>


using namespace std;

namespace Tango {

//
// forward declarations

class DbDatum;
class DbDevInfo;
class DbDevImportInfo;
class DbDevExportInfo;
class DbServerInfo;

typedef vector<DbDatum> DbData;
typedef vector<DbDevInfo> DbDevInfos;
typedef vector<DbDevExportInfo> DbDevExportInfos;
typedef vector<DbDevImportInfo> DbDevImportInfos;

//
// Database - database object for implementing generic high-level
//                 interface for TANGO database api
//

class Database : public Tango::Connection
{
private :	
	virtual string get_corba_name();
	virtual string build_corba_name() {return string("nada");}
	inline string dev_name() { return string("Db device"); }

public :
	Database(CORBA::ORB *orb=NULL);
	Database(string &host, int port, CORBA::ORB *orb=NULL);
	
	void build_connection ();
	~Database();
	inline Device_var &get_dbase() { return device;}

//
// general methods
//

	string get_info();
	
//
// device methods
//

	void add_device(DbDevInfo&);
	void delete_device(string);
	DbDevImportInfo import_device(string &);
	void export_device(DbDevExportInfo &);
	void unexport_device(string);

	DbDatum get_device_name(string &, string &);
	DbDatum get_device_alias(string &);
	DbDatum get_device_exported(string &);
	DbDatum get_device_domain(string &);
	DbDatum get_device_family(string &);
	DbDatum get_device_member(string &);
	
//
// server methods
//
	void add_server(string &, DbDevInfos&);
	void delete_server(string &);
	void export_server(string &, DbDevExportInfos &);
	void unexport_server(string &);
	
	DbServerInfo get_server_info(string &);
	
//
// property methods
//

	void get_property(string, DbData &);  
	void put_property(string, DbData &);  
	void delete_property(string, DbData &); 

	void get_device_property(string, DbData &);
	void put_device_property(string, DbData &);
	void delete_device_property(string, DbData &);
	
	void get_device_attribute_property(string, DbData &);
	void put_device_attribute_property(string, DbData &);
	void delete_device_attribute_property(string, DbData &);
	
	void get_class_property(string, DbData &);
	void put_class_property(string, DbData &);
	void delete_class_property(string, DbData &);
	
	void get_class_attribute_property(string, DbData &);
	void put_class_attribute_property(string, DbData &);
	void delete_class_attribute_property(string, DbData &);
};
//
// DbProperty - a database object for accessing general properties which
//                  are stored in the database
//
class DbProperty
{
public :
	DbProperty(string);
	~DbProperty();
//
// methods
//
	void get(DbData&);
	void put(DbData&);
	void delete_(DbData&);
};

//
// DbDevice - a database object for accessing device related information
//                 in the database
//

class DbDevice
{
private :
	string name;
	Database *dbase;
	int db_ind;
	bool ext_dbase;

public :
	DbDevice(string &);
	DbDevice(string &, Database *);
	DbDevice(string &,string &,string &);
	~DbDevice();
//
// methods
//
	DbDevImportInfo import_device();
	void export_device(DbDevExportInfo&);
	void add_device(DbData&);
	void delete_device();
	void get_property(DbData&);
	void put_property(DbData&);
	void delete_property(DbData&);
	void get_attribute_property(DbData&);
	void put_attribute_property(DbData&);
	void delete_attribute_property(DbData&);
};
//
// DbServer - a database object for accessing server related information
//            in the database
//
class DbServer
{
private :
	string name;
	Database *dbase;
	int db_ind;
	bool ext_dbase;

public :
	DbServer(string);
	DbServer(string, Database*);
	~DbServer();
//
// methods
//
	void add_server(DbDevInfos&);
	void delete_server();
	void export_server(DbDevExportInfos&);
	void unexport_server();
	DbServerInfo get_server_info();
};
//
// DbClass - a database object for accessing class related information
//                in the database
//
class DbClass
{
private :
	string name;
	Database *dbase;
	int db_ind;
	bool ext_dbase;

public :
	DbClass(string, Database*);
	DbClass(string);
	~DbClass();
//
// methods
//
	void get_property(DbData&);
	void put_property(DbData&);
	void delete_property(DbData&);
	void get_attribute_property(DbData&);
	void put_attribute_property(DbData&);
	void delete_attribute_property(DbData&);
};
//
// DbDatum - data object for sending and receiving data from the
//               TANGO database api
//
class DbDatum
{
public :
	enum except_flags
	{
		isempty_flag,
		wrongtype_flag,
		numFlags
	};
	
	string name;
	vector<string> value_string;
//
// constructor methods
//
	DbDatum();
	DbDatum (string);
	~DbDatum();
	int size();
	bool is_empty();
	void exceptions(bitset<numFlags> fl) { exceptions_flags = fl;}
//
// insert methods
//
	void operator << (short);
	void operator << (unsigned short);
	void operator << (long);
	void operator << (unsigned long);
	void operator << (float);
	void operator << (double);
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
//
// extract methods
//
	bool operator >> (short&);
	bool operator >> (unsigned short&);
	bool operator >> (long&);
	bool operator >> (unsigned long&);
	bool operator >> (float&);
	bool operator >> (double&);
	bool operator >> (const char*&);
	bool operator >> (string&);
	bool operator >> (vector<string>&);
	bool operator >> (vector<short>&);
	bool operator >> (vector<unsigned short>&);
	bool operator >> (vector<long>&);
	bool operator >> (vector<unsigned long>&);
	bool operator >> (vector<float>&);
	bool operator >> (vector<double>&);

private :

	int value_type;
	int value_size;
	bitset<numFlags> exceptions_flags;
};
//
// DbDevInfo
//
class DbDevInfo
{
public :
	string name;
	string _class;
	string server;
};
//
// DbDevImportInfo
//
class DbDevImportInfo
{
public :
	string name;
	long exported;
	string ior;
	string version;
};
//
// DbDevExportInfo
//
class DbDevExportInfo
{
public :
	string name;
	string ior;
	string host;
	string version;
	int pid;

};
//
// DbServerInfo
//
class DbServerInfo
{
public :
	string name;
	string host;
	int mode;
	int level;
};

} // End of Tango namespace

#endif /* _DBAPI_H */
