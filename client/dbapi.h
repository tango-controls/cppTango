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
class DbHistory;

class DatabaseExt;
class DbDeviceExt;
class DbClassExt;
class DbServerExt;
class DbDatumExt;
class FileDatabase;

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
	
	bool		db_multi_svc;
	vector<string>	multi_db_port;
	vector<string>	multi_db_host;
	DatabaseExt	*ext;
	FileDatabase 	*filedb; 
	string 		file_name;
	int		serv_version;
	DbDatum         make_string_array(string, CORBA::Any_var &);
	vector<DbHistory> make_history_array(bool, CORBA::Any_var &);

public :
	Database(CORBA::ORB *orb=NULL);
	Database(string &host, int port, CORBA::ORB *orb=NULL);
	Database(string &file);
	
	void write_filedatabase();
	void reread_filedatabase();
	void build_connection ();
	~Database();
	inline Device_var &get_dbase() { return device;}
	void check_tango_host(char *);

#ifdef _TG_WINDOWS_
	Database(CORBA::ORB *orb,string &,string &);
	long get_tango_host_from_reg(char **,string &,string &);
#endif

//
// general methods
//

	string get_info();
	DbDatum get_host_list();
	DbDatum get_host_list(string &);
        DbDatum get_services(string &,string &);
        void register_service(string &,string &,string &);
        void unregister_service(string &,string &);
	
//
// device methods
//

	void add_device(DbDevInfo&);
	void delete_device(string);
	DbDevImportInfo import_device(string &);
	void export_device(DbDevExportInfo &);
	void unexport_device(string);

	DbDatum get_device_name(string &, string &);
	DbDatum get_device_exported(string &);
	DbDatum get_device_domain(string &);
	DbDatum get_device_family(string &);
	DbDatum get_device_member(string &);
	void get_device_alias(string,string &);
	void get_alias(string,string &);
	DbDatum get_device_alias_list(string &);
	string get_class_for_device(string &);
	DbDatum get_class_inheritance_for_device(string &);
	DbDatum get_device_exported_for_class(string &);
	void put_device_alias(string &,string &);
        void delete_device_alias(string &);
	
//
// server methods
//
	void add_server(string &, DbDevInfos&);
	void delete_server(string &);
	void export_server(DbDevExportInfos &);
	void unexport_server(string &);
	
	DbServerInfo get_server_info(string &);
	void put_server_info(DbServerInfo &);
	void delete_server_info(string &);
	DbDatum get_server_class_list(string &);
	DbDatum get_server_name_list();
	DbDatum get_instance_name_list(string &);
	DbDatum get_server_list();
	DbDatum get_server_list(string &);
	DbDatum get_host_server_list(string &);
        DbDatum get_device_class_list(string &);
	
//
// property methods
//

	void get_property(string, DbData &);  
	void put_property(string, DbData &);  
	void delete_property(string, DbData &);
	vector<DbHistory> get_property_history(string &,string &);
	DbDatum get_object_list(string &);
	DbDatum get_object_property_list(string &,string &);

	void get_device_property(string, DbData &);
	void put_device_property(string, DbData &);
	void delete_device_property(string, DbData &);
	vector<DbHistory> get_device_property_history(string &,string &);
	DbDatum get_device_property_list(string &,string &);
	
	void get_device_attribute_property(string, DbData &);
	void put_device_attribute_property(string, DbData &);
	void delete_device_attribute_property(string, DbData &);
	vector<DbHistory> get_device_attribute_property_history(string &,string &,string &);
	
	void get_class_property(string, DbData &);
	void put_class_property(string, DbData &);
	void delete_class_property(string, DbData &);
	vector<DbHistory> get_class_property_history(string &,string &);
	DbDatum get_class_list(string &);
	DbDatum get_class_property_list(string &);
	
	void get_class_attribute_property(string, DbData &);
	void put_class_attribute_property(string, DbData &);
	void delete_class_attribute_property(string, DbData &);
	vector<DbHistory> get_class_attribute_property_history(string &,string &,string &);
	DbDatum get_class_attribute_list(string &,string &);
	

// attribute methods

	void get_attribute_alias(string, string&);
	DbDatum get_attribute_alias_list(string &);
	void put_attribute_alias(string &,string &);
	void delete_attribute_alias(string &);

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
	string 		name;
	Database 	*dbase;
	int 		db_ind;
	bool 		ext_dbase;
	
	DbDeviceExt	*ext;

public :
	DbDevice(string &);
	DbDevice(string &, Database *);
	DbDevice(string &,string &,string &);
	~DbDevice();
	void set_name(string &new_name) {name = new_name;}
	Database *get_dbase();
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
// DbAttribute - a database object for accessing Attribute related information
//                 in the database
//

class DbAttribute
{
private :
	string name;
	string device_name;
	Database *dbase;
	int db_ind;
	bool ext_dbase;

public :
	DbAttribute(string &, string &);
	DbAttribute(string &, string &, Database *);
	DbAttribute(string &,string &, string &,string &);
	~DbAttribute();
//
// methods
//
	void get_property(DbData&);
	void put_property(DbData&);
	void delete_property(DbData&);
};
//
// DbServer - a database object for accessing server related information
//            in the database
//
class DbServer
{
private :
	string 		name;
	Database 	*dbase;
	int 		db_ind;
	bool 		ext_dbase;
	DbServerExt	*ext;

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
	string 		name;
	Database 	*dbase;
	int 		db_ind;
	bool 		ext_dbase;
	
	DbClassExt	*ext;

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
	bitset<numFlags> exceptions() {return exceptions_flags;}
	void reset_exceptions(except_flags fl) {exceptions_flags.reset((size_t)fl);}
	void set_exceptions(except_flags fl) {exceptions_flags.set((size_t)fl);}
		
//
// insert methods
//

	void operator << (bool);
	void operator << (short);
	void operator << (unsigned char);
	void operator << (unsigned short);
	void operator << (DevLong);
	void operator << (DevULong);
	void operator << (DevLong64);
	void operator << (DevULong64);
	void operator << (float);
	void operator << (double);
	void operator << (char *);
//	void operator << (char *&);
	void operator << (const char *);
//	void operator << (const char *&);
	void operator << (string&);
	
	void operator << (vector<string>&);
	void operator << (vector<short>&);
	void operator << (vector<unsigned short>&);
	void operator << (vector<DevLong>&);
	void operator << (vector<DevULong>&);
	void operator << (vector<DevLong64>&);
	void operator << (vector<DevULong64>&);
	void operator << (vector<float>&);
	void operator << (vector<double>&);
	
//
// extract methods
//

	bool operator >> (bool&);
	bool operator >> (short&);
	bool operator >> (unsigned char&);
	bool operator >> (unsigned short&);
	bool operator >> (DevLong&);
	bool operator >> (DevULong&);
	bool operator >> (DevLong64&);
	bool operator >> (DevULong64&);
	bool operator >> (float&);
	bool operator >> (double&);
	bool operator >> (const char*&);
	bool operator >> (string&);
	
	bool operator >> (vector<string>&);
	bool operator >> (vector<short>&);
	bool operator >> (vector<unsigned short>&);
	bool operator >> (vector<DevLong>&);
	bool operator >> (vector<DevULong>&);
	bool operator >> (vector<DevLong64>&);
	bool operator >> (vector<DevULong64>&);
	bool operator >> (vector<float>&);
	bool operator >> (vector<double>&);

private :

	int 				value_type;
	int 				value_size;
	bitset<numFlags> 	exceptions_flags;
	
	DbDatumExt			*ext;
};
//
// DbHistory data object for receiving data history from the
//           TANGO database api
//
class DbHistory
{
public:

//
// constructor methods
//

  DbHistory(string ,string ,vector<string> &);
  DbHistory(string ,string ,string ,vector<string> &);

//
// getter methods
//

  string get_name();
  string get_attribute_name();
  string get_date();
  DbDatum get_value();
  bool is_deleted();
  
private:
  
  string  propname;   // Property name
  string  attname;    // Attribute name (Not used for device properties)
  DbDatum value;      // Property value  
  string  date;       // Update date
  bool    deleted;    // Deleted flag
  
  string format_mysql_date(string );  
  void make_db_datum(vector<string> &);
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

//
// Some extension classes
//
class DatabaseExt
{
public:
	DatabaseExt() {};
};

class DbDeviceExt
{
public:
	DbDeviceExt() {};
};

class DbClassExt
{
public:
	DbClassExt() {};
};

class DbServerExt
{
public:
	DbServerExt() {};
};

class DbDatumExt
{
public:
	DbDatumExt() {};
};

} // End of Tango namespace

#endif /* _DBAPI_H */
