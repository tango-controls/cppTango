
//+==================================================================================================================
//
// dbapi.h -	include file for TANGO database api
//
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
//
//-================================================================================================================


#ifndef _DATABASE_H
#define _DATABASE_H

/****************************************************************************************
 * 																						*
 * 					The Database class													*
 * 					------------------													*
 * 																						*
 ***************************************************************************************/

/**
 * A high level object which contains the link to the database.
 *
 * This class has methods for all database commands e.g. get_device_property(),
 * device_list(), info(), etc.
 *
 * $Author: taurel $
 * $Revision: 1 $
 *
 * @headerfile tango.h
 * @ingroup DBase
 */

class Database : public Tango::Connection
{
private :
	virtual std::string get_corba_name(bool);
	virtual std::string build_corba_name() {return std::string("nada");}
	virtual int get_lock_ctr() {return 0;}
	virtual void set_lock_ctr(int) {}

    class DatabaseExt
    {
    public:
        DatabaseExt() {};

		std::string	orig_tango_host;
    };

#ifdef HAS_UNIQUE_PTR
    std::unique_ptr<DatabaseExt>     ext;
#else
	DatabaseExt			        *ext;
#endif

	bool				db_multi_svc;
	std::vector<std::string>		multi_db_port;
	std::vector<std::string>		multi_db_host;
	FileDatabase 		*filedb;
	std::string 				file_name;
	int					serv_version;

	AccessProxy			*access_proxy;
	bool				access_checked;
	DevErrorList		access_except_errors;

	std::map<std::string,std::string>	dev_class_cache;
	std::string				db_device_name;

	bool				access_service_defined;

    Tango::Util         *db_tg;
    omni_mutex          map_mutex;

	DbDatum         make_string_array(std::string, CORBA::Any_var &);
	std::vector<DbHistory> make_history_array(bool, CORBA::Any_var &);

	void check_access();
	inline std::string dev_name();
	void set_server_release();
	void check_access_and_get();

public :
/**@name Constructors */
//@{
/**
 * Create a TANGO Database object.
 *
 * The constructor uses the environment variable “TANGO_HOST” to
 * determine which instance of the TANGO database to connect to. Example :
 * @code
 * using namespace Tango;
 * Database *db = new Database();
 * @endcode
 *
 * @param [in] orb	The CORBA ORB pointer. Default value is fine for 99 % of cases
 *
 */
	Database(CORBA::ORB *orb=NULL);
// @}

/**@name General methods */
//@{
/**
 * Get database info.
 *
 * Query the database for some general info about the tables in the database.
 * Result is returned as a string. Example :
 * @code
 * cout << db->get_info() << std::endl;
 * @endcode
 * will return information like this:
 * @code
 * Running since 2000-11-06 14:10:46
 * Devices defined = 115
 * Devices exported = 41
 * Device servers defined = 47
 * Device servers exported = 17
 * Class properties defined = 5
 * Device properties defined = 130
 * Class attribute properties defined = 20
 * Device attribute properties defined = 92
 * @endcode
 *
 * @return The string giving database info
 *
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	std::string get_info();
//@}

/**@name Device oriented methods */
//@{
/**
 * Add a device into the database.
 *
 * Add a device to the database. The device name, server and class are specified in the DbDevInfo structure.
 * Example :
 * @code
 * DbDevInfo my_device_info;
 *
 * my_device_info.name = “my/own/device”;
 * my_device_info._class = “MyDevice”;
 * my_device_info.server = “MyServer/test”;
 *
 * db->add_device(my_device_info);
 * @endcode
 *
 * @param [in] dev_info A reference to a DbDevInfo instance with all device info.
 *
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed
 */
	void add_device(DbDevInfo &dev_info);
/**
 * Delete a device from the database.
 *
 * Delete the device of the specified name from the database. Example
 * @code
 * db->delete_device(“my/own/device”);
 * @endcode
 *
 * @param [in] dev_name The device name
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device (DB_SQLError, DB_DeviceNotDefined)
 */
	void delete_device(std::string dev_name);
/**
 * Import a device from the database.
 *
 * Query the database for the export info of the specified device.
 * The command returns the information in a DbDevImportInfo structure. Example :
 * @code
 * DbDevImportInfo my_device_import;
 *
 * my_device_import = db->import_device(“my/own/device”);
 *
 * cout << “ device ” << my_device_import.name;
 * cout << “exported ” << my_device_import.exported;
 * cout << “ior ” << my_device_import.ior;
 * cout << “version ” << my_device_import.version;
 * cout << std::endl;
 * @endcode
 *
 * @param [in] dev_name The device name
 * @return One instance of a DbDevImportInfo class
 *
 * @exception ConnectionFailed, CommunicationFailed, DevFailed
 */
	DbDevImportInfo import_device(std::string &dev_name);
/**
 * Export a device into the database.
 *
 * Update the export info for this device in the database. Device name, server, class, pid and version are
 * specified in the DbDevExportInfo structure. Example :
 * @code
 * DbDevExportInfo my_device_export;
 *
 * my_device_export.name = “my/own/device”;
 * my_device_export.ior = “the real ior”;
 * my_device_export.host = “dumela”;
 * my_device_export.version = “1.0”;
 * my_device_export.pid = get_pid();
 *
 * db->export_device(my_device_export);
 * @endcode
 *
 * @param [in] info The device export information
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device (DB_SQLError, DB_DeviceNotDefined)
 */
	void export_device(DbDevExportInfo &info);
/**
 * Unexport a device in the database.
 *
 * Mark the specified device as un-exported in the database. Example :
 * @code
 * db->unexport_device(“my/own/device”);
 * @endcode
 *
 * @param [in] dev_name The device name
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device (DB_SQLError)
 */
	void unexport_device(std::string dev_name);
/**
 * Get device information
 *
 * Return miscellaneous device information from the database (not from the device itself)
 *
 * @param [in] dev_name The device name
 * @return The device information class instance
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDevFullInfo get_device_info(std::string &dev_name);
/**
 * Get class name for a device
 *
 * Return the class of the specified device.
 * @code
 * std::string devname("sr/rf-cavity/1");
 * std::string classname = db->get_class_for_device(devname);
 * @endcode
 *
 * @param [in] dev_name The device name
 * @return The device class name
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	std::string get_class_for_device(std::string &dev_name);
/**
 * Get device inheritance scheme
 *
 * Return the class inheritance scheme of the specified device
 * @code
 * std::string devname("sr/rf-cavity/1");
 *
 * DbDatum db_datum = db->get_class_inheritance_for_device(devname);
 *
 * std::vector<std::string> class_list;
 * db_datum >> class_list;
 * @endcode
 *
 * @param [in] dev_name The device name
 * @return The device inheritance
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_class_inheritance_for_device(std::string &dev_name);
//@}

/**@name Server oriented methods */
//@{
/**
 * Create a device server process in database.
 *
 * Add a group of devices to the database.
 * The device names, server names and classes are specified in a vector of DbDevInfo structures.
 *
 * @param [in] ds_name The full device server process name
 * @param [in] devs Vector of DbDevInfo instances
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device (DB_SQLError)
 */
	void add_server(std::string &ds_name, DbDevInfos &devs);
/**
 * Delete a device server process from the database.
 *
 * Delete the device server and its associated devices from the database.
 *
 * @param [in] ds_name The full device server process name
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device (DB_SQLError)
 */
	void delete_server(std::string &ds_name);
/**
 * Delete a device server process from the database.
 *
 * Export a group of devices to the database.
 * The device names, IOR, class, server name, pid etc. are specified in the vector of DbDevExportInfo structures.
 *
 * @param [in] devs Devices information in a vector of DbDevExportInfo
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device (DB_SQLError)
 */
	void export_server(DbDevExportInfos &devs);
/**
 * Unexport all devices from a device server in the database.
 *
 * Mark all devices exported for this device server process as unexported.
 *
 * @param [in] ds_name The full device server process name
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device (DB_SQLError)
 */
	void unexport_server(std::string &ds_name);
/**
 * Rename a device server in the database.
 *
 * Rename a device server process in the database.
 *
 * @param [in] old_ds_name The old device server process name
 * @param [in] new_ds_name The new device server process name
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device (DB_SQLError)
 */
	void rename_server(const std::string &old_ds_name,const std::string &new_ds_name);
//@}

/**@name Services oriented methods */
//@{
/**
 * Get services list from database
 *
 * Query database for specified services. The instname parameter can be a wildcard character ("*").
 * @code
 * string servicename("HdbManager");
 * string instname("ctrm");
 *
 * DbDatum db_datum = db->get_services(servicename,instname);
 *
 * vector<string> service_list;
 * db_datum >> service_list;
 * @endcode
 *
 * @param [in] service_name The service name
 * @param [in] inst_name The instance name
 * @return A service list matching the input parameters
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_services(std::string &service_name,std::string &inst_name);
/**
 * Get services list from database
 *
 * Query database for specified services. The vector of strings returned in the DbDatum
 * object contains pair of strings &lt;instance_name&gt; followed by &lt;device name&gt;.
 * @code
 * std::string servicename("HdbManager");
 *
 * DbDatum db_datum = db->get_device_service_list(servicename);
 *
 * std::vector<std::string> service_device_list;
 * db_datum >> service_device_list;
 * @endcode
 *
 * @param [in] service_name The service name
 * @return A service/device std::list matching the input parameter
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_device_service_list(std::string &service_name);
/**
 * Register a service in the database
 *
 * Register the specified service wihtin the database.
 * @code
 * std::string servicename("HdbManager");
 * std::string instname("ctrm");
 * std::string devname("sys/hdb/1");
 *
 * db->register_service(servicename,instname,devname);
 * @endcode
 *
 * @param [in] service_name The service name
 * @param [in] inst_name The instance name
 * @param [in] dev_name The device name implementing the service
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void register_service(std::string &service_name,std::string &inst_name,std::string &dev_name);
/**
 * Unregister a service from the database
 *
 * Unregister the specified service from the database.
 * @code
 * std::string servicename("HdbManager");
 * std::string instname("ctrm");
 *
 * db->unregister_service(servicename,instname);
 * @endcode
 *
 * @param [in] service_name The service name
 * @param [in] inst_name The instance name
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void unregister_service(std::string &service_name,std::string &inst_name);
//@}

/**@name Object property oriented methods */
//@{
/**
 * Get object property value
 *
 * Query the database for a list of object (i.e. non-device) properties for the specified object. The property
 * names are specified by the vector of DbDatum structures. The method returns the properties in the same
 * DbDatum structures. To retrieve the properties use the extract operator >>. Here is an example of how to
 * use the DbData type to specify and extract properties :
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum(“velocity”));
 * db_data.push_back(DbDatum(“acceleration”));
 *
 * db->get_property(“mymotor”, db_data);
 *
 * float velocity, acceleration;
 * db_data[0] >> velocity;
 * db_data[1] >> acceleration;
 * @endcode
 *
 * @param [in] obj_name The object (free property) name
 * @param [in,out] db The property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_property(std::string obj_name, DbData &db) {get_property(obj_name,db,NULL);}
/**
 * Put object property value in database
 *
 * Insert or update a list of properties for the specified object. The property names and their values are
 * specified by the vector of DbDatum structures. Use the insert operator >> to insert the properties into the
 * DbDatum structures. Here is an example of how to insert properties into the database using this method :
 * @code
 * DbDatum velocity(“velocity”), acceleration(“acceleration”);
 * DbData db_data;
 *
 * velocity << 100000.0;
 * acceleration << 500000.0;
 * db_data.push_back(velocity);
 * db_data.push_back(acceleration);
 *
 * db->put_property(“mymotor”, db_data);
 * @endcode
 *
 * @param [in] obj_name The object (free property) name
 * @param [in] db The property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void put_property(std::string obj_name, DbData &db);
/**
 * Delete object property from database
 *
 * Delete a list of properties for the specified object. The property names are specified by the vector of
 * DbDatum structures. Here is an example of how to delete properties from the database using this method :
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum(“velocity”));
 * db_data.push_back(DbDatum(“acceleration”));
 *
 * db->delete_property(“mymotor”, db_data);
 * @endcode
 *
 * @param [in] obj_name The object (free property) name
 * @param [in] db The property names
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void delete_property(std::string obj_name, DbData &db);
/**
 * Get object property history from database
 *
 * Get the list of the last 10 modifications of the specifed object property. Note that propname can contain a
 * wildcard character (eg: "prop*").
 * @code
 * std::vector<DbHistory> hist;
 * DbDatum result;
 * std::string objname("jlptest");
 * std::string propname("test_prop");
 *
 * hist = db->get_property_history(objname,propname);
 *
 * // Print the modification history of the specified property
 * for(int i=0;i<hist.size();i++)
 * {
 *    cout << "Name:" << hist[i].get_name() << std::endl;
 *    cout << "Date:" << hist[i].get_date() << std::endl;
 *    if( hist[i].is_deleted() )
 *    {
 *        cout << "Deleted !" << std::endl;
 *    }
 *    else
 *    {
 *        hist[i].get_value() >> result;
 *        for (int j=0; j<result.size(); j++)
 *           cout << "Value:" << result[j] << std::endl;
 *    }
 * }
 * @endcode
 *
 * @param [in] obj_name The object (free property) name
 * @param [in] prop_name The property name
 * @return A vector of DbHistory instances
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	std::vector<DbHistory> get_property_history(std::string &obj_name,std::string &prop_name);
//@}

/**@name Device property oriented methods */
//@{
/**
 * Get device property value
 *
 * Query the database for a list of device properties for the specified object. The property names are specified
 * by the vector of DbDatum structures. The method returns the properties in the same DbDatum structures.
 * To retrieve the properties use the extract operator >>. Here is an example of how to use the DbData type to
 * specify and extract properties :
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum(“velocity”));
 * db_data.push_back(DbDatum(“acceleration”));
 *
 * db->get_device_property(“id11/motor/1”, db_data);
 *
 * float velocity, acceleration;
 * db_data[0] >> velocity;
 * db_data[1] >> acceleration;
 * @endcode
 *
 * @param [in] dev_name The device name
 * @param [in,out] db The property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_device_property(std::string dev_name, DbData &db) {get_device_property(dev_name,db,NULL);}
/**
 * Put device property value in database
 *
 * Insert or update a list of properties for the specified device. The property names and their values are
 * specified by the vector of DbDatum structures. Use the insert operator >> to insert the properties into the
 * DbDatum structures. Here is an example of how to insert properties into the database using this method :
 * @code
 * DbDatum velocity(“velocity”), acceleration(“acceleration”);
 * DbData db_data;
 *
 * velocity << 100000.0;
 * acceleration << 500000.0;
 * db_data.push_back(velocity);
 * db_data.push_back(acceleration);
 *
 * db->put_device_property(“id11/motor/1”, db_data);
 * @endcode
 *
 * @param [in] dev_name The device name
 * @param [in] db The property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void put_device_property(std::string dev_name, DbData &db);
/**
 * Delete device property from database
 *
 * Delete a list of properties for the specified device. The property names are specified by the vector of
 * DbDatum structures. Here is an example of how to delete properties from the database using this method :
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum(“velocity”));
 * db_data.push_back(DbDatum(“acceleration”));
 *
 * db->delete_device_property(“id11/motor/1”, db_data);
 * @endcode
 *
 * @param [in] dev_name The device name
 * @param [in] db The property names
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void delete_device_property(std::string dev_name, DbData &db);
/**
 * Get device property history from database
 *
 * Get the list of the last 10 modifications of the specifed device property. Note that prop_name can contain
 * a wildcard character (eg: "prop*"). An example of usage of a similar function can be found in the
 * documentation of the get_property_history() function.
 *
 * @param [in] dev_name The device name
 * @param [in] prop_name The property name
 * @return A vector of DbHistory instances
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	std::vector<DbHistory> get_device_property_history(std::string &dev_name,std::string &prop_name);
//@}

/**@name Device attribute property oriented methods */
//@{
/**
 * Get device attribute property value
 *
 * Query the database for a list of device attribute properties for the specified object. The attribute names
 * are specified by the vector of DbDatum structures. The method returns all the properties for the specified
 * attributes. The attribute names are returned with the number of properties specified as their value. The
 * first DbDatum element of the returned DbData vector contains the first attribute name and the first attribute
 * property number. The following DbDatum element contains the first attribute property name and property
 * values. To retrieve the properties use the extract operator >>. Here is an example of how to use the DbData
 * type to specify and extract attribute properties :
 * @code
 * DbData db_data;
 *
 * db_data.push_back(DbDatum("velocity"));
 * db_data.push_back(DbDatum("acceleration"));
 *
 * db->get_device_attribute_property("id11/motor/1", db_data);
 *
 * float vel_max, vel_min, acc_max, acc_min;
 * for (int i=0;i < db_data.size();i++)
 * {
 *    long nb_prop;
 *    std::string &att_name = db_data[i].name;
 *    db_data[i] >> nb_prop;
 *    i++;
 *    for (int k=0;k < nb_prop;k++)
 *    {
 *        std::string &prop_name = db_data[i].name;
 *        if (att_name == "velocity")
 *        {
 *           if (prop_name == "min")
 *              db_data[i] >> vel_min;
 *           else if (att_name == "max")
 *              db_data[i] >> vel_max;
 *        }
 *        else
 *        {
 *           if (prop_name == "min")
 *              db_data[i] >> acc_min;
 *           else
 *              db_data[i] >> acc_max;
 *        }
 *        i++;
 *     }
 * }
 * @endcode
 *
 * @param [in] dev_name The device name
 * @param [in,out] db The property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_device_attribute_property(std::string dev_name, DbData &db) {get_device_attribute_property(dev_name,db,NULL);}
/**
 * Put device attribute property value in database
 *
 * Insert or update a list of attribute properties for the specified device. The attribute property names and their
 * values are specified by the vector of DbDatum structures. Use the insert operator >> to insert the properties
 * into the DbDatum structures. Here is an example of how to insert/update properties min, max for attribute
 * velocity and properties min, max for attribute acceleration of device id11/motor/1 into the database using
 * this method :
 * @code
 * DbDatum vel("velocity");                // We want to put properties for attribute "velocity"
 * DbDatum vel_min("min"), vel_max("max");
 * DbDatum acc("acceleration")             // We want to put properties for attribute "acceleration"
 * DbDatum acc_min("min"), acc_max("max");
 * DbData db_data;
 *
 * vel << 2;                               // Two properties for attribute "velocity"
 * vel_min << 0.0;                         // Value for property min
 * vel_max << 1000000.0;                   // Value for property max
 *
 * db_data.push_back(vel);
 * db_data.push_back(vel_min);
 * db_data.push_back(vel_max);
 *
 * acc << 2;                               // Two properties for attribute "acceleration"
 * acc_min << 0.0;                         // Value for property min
 * acc_max << 8000000;                     // Value for property max
 *
 * db_data.push_back(acc);
 * db_data.push_back(acc_min);
 * db_data.push_back(acc_max);
 *
 * db->put_device_attribute_property(“id11/motor/1”, db_data);
 * @endcode
 *
 * @param [in] dev_name The device name
 * @param [in] db The property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void put_device_attribute_property(std::string dev_name, DbData &db);
/**
 * Delete device attribute property from database
 *
 * Delete a list of attribute properties for the specified device. The attribute names are specified by the vector
 * of DbDatum structures. Here is an example of how to delete the unit property of the velocity attribute of
 * the id11/motor/1 device using this method :
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum("velocity"));
 * db_data.push_back(DbDatum("unit"));
 *
 * db->delete_device_attribute_property("id11/motor/1", db_data);
 * @endcode
 *
 * @param [in] dev_name The device name
 * @param [in] db The property names
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void delete_device_attribute_property(std::string dev_name, DbData &db);
/**
 * Get device attribute property history from database
 *
 * Get the list of the last 10 modifications of the specifed device attribute property. Note that prop_name
 * and att_name can contain
 * a wildcard character (eg: "prop*"). An example of usage of a similar function can be found in the
 * documentation of the get_property_history() function.
 *
 * @param [in] dev_name The device name
 * @param [in] prop_name The property name
 * @param [in] att_name The property name
 * @return A vector of DbHistory instances
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	std::vector<DbHistory> get_device_attribute_property_history(std::string &dev_name,std::string &prop_name,std::string &att_name);
/**
 * Get list of attribute with data in database for a specific device
 *
 * Get the list of attribute(s) with some data defined in database for a specified device.
 * Note that this is not the list of all device attributes because not all attribute(s) have
 * some data in database
 *
 * @param [in] dev_name The device name
 * @param [in] att_list The attribute name list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_device_attribute_list(std::string &dev_name,std::vector<std::string> &att_list);
/**
 * Get list of pipe with data in database for a specific device
 *
 * Get the list of pipe(s) with some data defined in database for a specified device.
 * Note that this is not the list of all device pipes because not all pipe(s) have
 * some data in database
 *
 * @param [in] dev_name The device name
 * @param [in] pipe_list The pipe name list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_device_pipe_list(const std::string &dev_name,std::vector<std::string> &pipe_list);
//@}

/**@name Device pipe property oriented methods */
//@{
/**
 * Get device pipe property value
 *
 * Query the database for a list of device pipe properties for the specified object. The pipe names
 * are specified by the vector of DbDatum structures. The method returns all the properties for the specified
 * pipes. The pipe names are returned with the number of properties specified as their value. The
 * first DbDatum element of the returned DbData vector contains the first pipe name and the first pipe
 * property number. The following DbDatum element contains the first pipe property name and property
 * values. To retrieve the properties use the extract operator >>. Here is an example of how to use the DbData
 * type to specify and extract pipe properties :
 * @code
 * DbData db_data;
 *
 * db_data.push_back(DbDatum("velocity"));
 * db_data.push_back(DbDatum("acceleration"));
 *
 * db->get_device_pipe_property("id11/motor/1", db_data);
 *
 * float vel_max, vel_min, acc_max, acc_min;
 * for (int i=0;i < db_data.size();i++)
 * {
 *    long nb_prop;
 *    std::string &pipe_name = db_data[i].name;
 *    db_data[i] >> nb_prop;
 *    i++;
 *    for (int k=0;k < nb_prop;k++)
 *    {
 *        std::string &prop_name = db_data[i].name;
 *        if (pipe_name == "velocity")
 *        {
 *           if (prop_name == "min")
 *              db_data[i] >> vel_min;
 *           else if (att_name == "max")
 *              db_data[i] >> vel_max;
 *        }
 *        else
 *        {
 *           if (prop_name == "min")
 *              db_data[i] >> acc_min;
 *           else
 *              db_data[i] >> acc_max;
 *        }
 *        i++;
 *     }
 * }
 * @endcode
 *
 * @param [in] dev_name The device name
 * @param [in,out] db The pipe/property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_device_pipe_property(std::string dev_name, DbData &db) {get_device_pipe_property(dev_name,db,NULL);}
/**
 * Put device pipe property value in database
 *
 * Insert or update a list of pipe properties for the specified device. The pipe property names and their
 * values are specified by the vector of DbDatum structures. Use the insert operator >> to insert the properties
 * into the DbDatum structures. Here is an example of how to insert/update properties min, max for pipe
 * velocity and properties min, max for pipe acceleration of device id11/motor/1 into the database using
 * this method :
 * @code
 * DbDatum vel("velocity");                // We want to put properties for pipe "velocity"
 * DbDatum vel_min("min"), vel_max("max");
 * DbDatum acc("acceleration")             // We want to put properties for pipe "acceleration"
 * DbDatum acc_min("min"), acc_max("max");
 * DbData db_data;
 *
 * vel << 2;                               // Two properties for pipe "velocity"
 * vel_min << 0.0;                         // Value for property min
 * vel_max << 1000000.0;                   // Value for property max
 *
 * db_data.push_back(vel);
 * db_data.push_back(vel_min);
 * db_data.push_back(vel_max);
 *
 * acc << 2;                               // Two properties for pipe "acceleration"
 * acc_min << 0.0;                         // Value for property min
 * acc_max << 8000000;                     // Value for property max
 *
 * db_data.push_back(acc);
 * db_data.push_back(acc_min);
 * db_data.push_back(acc_max);
 *
 * db->put_device_pipe_property(“id11/motor/1”, db_data);
 * @endcode
 *
 * @param [in] dev_name The device name
 * @param [in] db The pipe/property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void put_device_pipe_property(std::string dev_name, DbData &db);
/**
 * Delete device pipe property from database
 *
 * Delete a list of pipe properties for the specified device. The pipe names are specified by the vector
 * of DbDatum structures. Here is an example of how to delete the unit property of the velocity pipe of
 * the id11/motor/1 device using this method :
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum("velocity"));
 * db_data.push_back(DbDatum("unit"));
 *
 * db->delete_device_attribute_property("id11/motor/1", db_data);
 * @endcode
 *
 * @param [in] dev_name The device name
 * @param [in] db The pipe/property names
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void delete_device_pipe_property(std::string dev_name, DbData &db);
/**
 * Get device pipe property history from database
 *
 * Get the list of the last 10 modifications of the specifed device pipe property. Note that prop_name
 * and pipe_name can contain
 * a wildcard character (eg: "prop*"). An example of usage of a similar function can be found in the
 * documentation of the get_property_history() function.
 *
 * @param [in] dev_name The device name
 * @param [in] pipe_name The property name
 * @param [in] prop_name The pipe name
 * @return A vector of DbHistory instances
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	std::vector<DbHistory> get_device_pipe_property_history(std::string &dev_name,std::string &pipe_name,std::string &prop_name);
//@}

/**@name Class property oriented methods */
//@{
/**
 * Get class property value
 *
 * Query the database for a list of class properties. The property names are specified by the vector of DbDatum
 * structures. The method returns the properties in the same DbDatum structures. To retrieve the properties
 * use the extract operator >>. Here is an example of how to use the DbData type to specify and extract
 * properties :
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum("velocity"));
 * db_data.push_back(DbDatum("acceleration"));
 *
 * db->get_class_property("StepperMotor", db_data);
 *
 * float velocity, acceleration;
 * db_data[0] >> velocity;
 * db_data[1] >> acceleration;
 * @endcode
 *
 * @param [in] class_name The class name
 * @param [in,out] db The property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_class_property(std::string class_name,DbData &db) {get_class_property(class_name,db,NULL);}
/**
 * Put class property value in database
 *
 * Insert or update a list of properties for the specified class. The property names and their values are specified
 * by the vector of DbDatum structures. Use the insert operator >> to insert the properties into the DbDatum
 * structures. Here is an example of how to insert properties into the database using this method :
 * @code
 * DbDatum velocity("velocity"), acceleration("acceleration");
 * DbData db_data;
 *
 * velocity << 100000.0;
 * acceleration << 500000.0;
 * db_data.push_back(velocity);
 * db_data.push_back(acceleration);
 *
 * db->put_class_property("StepperMotor", db_data);
 * @endcode
 *
 * @param [in] class_name The class name
 * @param [in] db The property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void put_class_property(std::string class_name, DbData &db);
/**
 * Delete class property from database
 *
 * Delete a list of properties for the specified class. The property names are specified by the vector of Db-
 * Datum structures. Here is an example of how to delete properties from the database using this method
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum("velocity"));
 * db_data.push_back(DbDatum("acceleration"));
 *
 * db->delete_class_property("StepperMotor", db_data);
 * @endcode
 *
 * @param [in] class_name The class name
 * @param [in] db The property names
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void delete_class_property(std::string class_name, DbData &db);
/**
 * Get class property history from database
 *
 * Get the list of the last 10 modifications of the specifed class property. Note that prop_name
 * can contain
 * a wildcard character (eg: "prop*"). An example of usage of a similar function can be found in the
 * documentation of the get_property_history() function.
 *
 * @param [in] class_name The class name
 * @param [in] prop_name The property name
 * @return A vector of DbHistory instances
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	std::vector<DbHistory> get_class_property_history(std::string &class_name,std::string &prop_name);
//@}

/**@name Class attribute property oriented methods */
//@{
/**
 * Get class attribute property value
 *
 * Query the database for a list of class attribute properties for the specified object. The attribute names are
 * specified by the vector of DbDatum structures. The method returns all the properties for the specified
 * attributes. The attribute names are returned with the number of properties specified as their value. The
 * first DbDatum element of the returned DbData vector contains the first attribute name and the first attribute
 * property number. The following DbDatum element contains the first attribute property name and property
 * values. To retrieve the properties use the extract operator >>. Here is an example of how to use the DbData
 * type to specify and extract attribute properties :
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum("velocity"));
 * db_data.push_back(DbDatum("acceleration"));
 *
 * db->get_class_attribute_property("StepperMotor", db_data);
 *
 * float vel_max, vel_min, acc_max, acc_min;
 * for (int i=0; i< db_data.size(); i++)
 * {
 *    long nb_prop;
 *    std::string &att_name = db_data[i].name;
 *    db_data[i] >> nb_prop;
 *    i++;
 *    for (int k=0;k < nb_prop;k++)
 *    {
 *        std::string &prop_name = db_data[i].name;
 *        if (att_name == "velocity")
 *        {
 *           if (prop_name == "min")
 *              db_data[i] >> vel_min;
 *           else if (att_name == "max")
 *              db_data[i] >> vel_max;
 *        }
 *        else
 *        {
 *           if (prop_name == "min")
 *              db_data[i] >> acc_min;
 *           else
 *              db_data[i] >> acc_max;
 *        }
 *        i++;
 *    }
 * }
 * @endcode
 *
 * @param [in] class_name The class name
 * @param [in,out] db The property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_class_attribute_property(std::string class_name,DbData &db) {get_class_attribute_property(class_name,db,NULL);}
/**
 * Put class attribute property value in database
 *
 * Insert or update a list of attribute properties for the specified class. The attribute property names and their
 * values are specified by the vector of DbDatum structures. Use the insert operator >> to insert the properties
 * into the DbDatum structures. Here is an example of how to insert/update min, max properties for attribute
 * velocity and min, max properties for attribute acceleration properties belonging to class StepperMotor into
 * the database using this method :
 * @code
 * DbDatum velocity("velocity"), vel_min("min"), vel_max("max");
 * DbDatum acceleration("acceleration"), acc_min("min"), acc_max("max");
 * DbData db_data;
 *
 * velocity << 2;
 * vel_min << 0.0;
 * vel_max << 1000000.0;
 *
 * db_data.push_back(velocity);
 * db_data.push_back(vel_min);
 * db_data.push_back(vel_max);
 *
 * acceleration << 2;
 * acc_min << 0.0;
 * acc_max << 8000000;
 *
 * db_data.push_back(acceleration);
 * db_data.push_back(acc_min);
 * db_data.push_back(acc_max);
 *
 * db->put_class_attribute_property("StepperMotor", db_data);
 * @endcode
 *
 * @param [in] class_name The class name
 * @param [in] db The property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void put_class_attribute_property(std::string class_name, DbData &db);
/**
 * Delete class attribute property from database
 *
 * Delete a std::list of attribute properties for the specified class. The attribute names are specified by the std::vector
 * of DbDatum structures. All properties belonging to the std::listed attributes are deleted. Here is an example of
 * how to delete the unit property of the velocity attribute of the StepperMotor class from the database using
 * this method :
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum("velocity"));
 * db_data.push_back(DbDatum("unit"));
 *
 * db->delete_class_attribute_property("StepperMotor", db_data);
 * @endcode
 *
 * @param [in] class_name The class name
 * @param [in] db The property names
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void delete_class_attribute_property(std::string class_name, DbData &db);
/**
 * Get class attribute property history from database
 *
 * Get the std::list of the last 10 modifications of the specifed class attribute property. Note that prop_name
 * and att_name can contain
 * a wildcard character (eg: "prop*"). An example of usage of a similar function can be found in the
 * documentation of the get_property_history() function.
 *
 * @param [in] class_name The class name
 * @param [in] att_name The attribute name
 * @param [in] prop_name The property name
 * @return A std::vector of DbHistory instances
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	std::vector<DbHistory> get_class_attribute_property_history(std::string &class_name,std::string &att_name,std::string &prop_name);
//@}

/**@name Class pipe property oriented methods */
//@{
/**
 * Get class pipe property value
 *
 * Query the database for a std::list of class pipe properties for the specified object. The pipe names are
 * specified by the std::vector of DbDatum structures. The method returns all the properties for the specified
 * pipes. The pipe names are returned with the number of properties specified as their value. The
 * first DbDatum element of the returned DbData std::vector contains the first pipe name and the first pipe
 * property number. The following DbDatum element contains the first pipe property name and property
 * values. To retrieve the properties use the extract operator >>. Here is an example of how to use the DbData
 * type to specify and extract pipe properties :
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum("pipe_image"));
 * db_data.push_back(DbDatum("pipe_misc"));
 *
 * db->get_class_pipe_property("MyDetector", db_data);
 *
 * int max_x, min_x, size;
 * for (int i=0; i< db_data.size(); i++)
 * {
 *    long nb_prop;
 *    std::string &pipe_name = db_data[i].name;
 *    db_data[i] >> nb_prop;
 *    i++;
 *    for (int k=0;k < nb_prop;k++)
 *    {
 *        std::string &prop_name = db_data[i].name;
 *        if (pipe_name == "pipe_image")
 *        {
 *           if (prop_name == "min_x")
 *              db_data[i] >> min_x;
 *           else if (att_name == "max_x")
 *              db_data[i] >> max_x;
 *        }
 *        else
 *        {
 *           if (prop_name == "size")
 *              db_data[i] >> size;
 *        }
 *        i++;
 *    }
 * }
 * @endcode
 *
 * @param [in] class_name The class name
 * @param [in,out] db The pipe/property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_class_pipe_property(std::string class_name,DbData &db) {get_class_pipe_property(class_name,db,NULL);}
/**
 * Put class pipe property value in database
 *
 * Insert or update a std::list of pipe properties for the specified class. The pipe property names and their
 * values are specified by the std::vector of DbDatum structures. Use the insert operator >> to insert the properties
 * into the DbDatum structures. Here is an example of how to insert/update min, max properties for pipe
 * velocity and min, max properties for pipe acceleration properties belonging to class StepperMotor into
 * the database using this method :
 * @code
 * DbDatum velocity("velocity"), vel_min("min"), vel_max("max");
 * DbDatum acceleration("acceleration"), acc_min("min"), acc_max("max");
 * DbData db_data;
 *
 * velocity << 2;
 * vel_min << 0.0;
 * vel_max << 1000000.0;
 *
 * db_data.push_back(velocity);
 * db_data.push_back(vel_min);
 * db_data.push_back(vel_max);
 *
 * acceleration << 2;
 * acc_min << 0.0;
 * acc_max << 8000000;
 *
 * db_data.push_back(acceleration);
 * db_data.push_back(acc_min);
 * db_data.push_back(acc_max);
 *
 * db->put_class_pipe_property("StepperMotor", db_data);
 * @endcode
 *
 * @param [in] class_name The class name
 * @param [in] db The pipe/property names and values
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void put_class_pipe_property(std::string class_name, DbData &db);
/**
 * Delete class pipe property from database
 *
 * Delete a std::list of pipe properties for the specified class. The pipe names are specified by the std::vector
 * of DbDatum structures. All properties belonging to the std::listed pipes are deleted. Here is an example of
 * how to delete the unit property of the velocity pipe of the StepperMotor class from the database using
 * this method :
 * @code
 * DbData db_data;
 * db_data.push_back(DbDatum("velocity"));
 * db_data.push_back(DbDatum("unit"));
 *
 * db->delete_class_pipe_property("StepperMotor", db_data);
 * @endcode
 *
 * @param [in] class_name The class name
 * @param [in] db The pipe/property names
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void delete_class_pipe_property(std::string class_name, DbData &db);
/**
 * Get class pipe property history from database
 *
 * Get the std::list of the last 10 modifications of the specifed class pipe property. Note that prop_name
 * and pipe_name can contain
 * a wildcard character (eg: "prop*"). An example of usage of a similar function can be found in the
 * documentation of the get_property_history() function.
 *
 * @param [in] class_name The class name
 * @param [in] pipe_name The pipe name
 * @param [in] prop_name The property name
 * @return A std::vector of DbHistory instances
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	std::vector<DbHistory> get_class_pipe_property_history(std::string &class_name,std::string &pipe_name,std::string &prop_name);
//@}

/**@name Alias oriented methods */
//@{
/**
 * Get device name from its alias
 *
 * Get the device name from its alias. The device alias is specified by alias and the device name
 * is returned in dev_name. If there is no device defined with the given alias, a DevFailed exception is thrown.
 *
 * @param [in] alias The device alias
 * @param [out] dev_name The device name
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_device_from_alias(std::string alias,std::string &dev_name);
/**
 * Get device alias form its name
 *
 * Get the device alias from its name. The device name is specified by dev_name and the device alias is
 * returned in alias. If there is no alias defined for the device, a DevFailed exception is thrown.
 *
 * @param [in] dev_name The device name
 * @param [out] alias The device alias
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_alias_from_device(std::string dev_name,std::string &alias);
/**
 * Get device alias from its name
 *
 * @deprecated use get_alias_from_device()
 *
 * Get the device alias name from its name. The device name is specified by dev_name and the device alias
 * name is returned in dev_alias. If there is no alias defined for the device, a DevFailed exception is thrown.
 *
 * @param [in] dev_name The device name
 * @param [out] dev_alias The device alias
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_alias(std::string dev_name,std::string &dev_alias);
/**
 * Get device name from its alias
 *
 * @deprecated use get_device_from_alias()
 *
 * Get the device name from an alias. The device alias is specified by dev_alias and the device name is
 * returned in dev_name. If there is no device with the given alias, a DevFailed exception is thrown.
 *
 * @param [in] dev_alias The device alias
 * @param [out] dev_name The device name
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_device_alias(std::string dev_alias,std::string &dev_name);
/**
 * Define device alias
 *
 * Create a device alias. Alias name has to be uniq within a Tango control system and you will receive an
 * exception if the alias is already defined.
 *
 * @param [in] dev_name The device name
 * @param [in] dev_alias The device alias
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void put_device_alias(std::string &dev_name,std::string &dev_alias);
/**
 * Delete device alias
 *
 * Delete a device alias.
 *
 * @param [in] dev_alias The device alias
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void delete_device_alias(std::string &dev_alias);
/**
 * Get attribute name from its alias
 *
 * Get the attribute name from its alias. The attribute alias is specified by alias and the attribute name
 * is returned in att_name. If there is no attribute defined with the given alias, a DevFailed exception is thrown.
 *
 * @param [in] alias The attribute alias
 * @param [out] att_name The attribute name
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_attribute_from_alias(std::string alias,std::string &att_name);
/**
 * Get attribute alias form its name
 *
 * Get the attribute alias from its name. The attribute name is specified by att_name and the attribute alias is
 * returned in alias. If there is no alias defined for the attribute, a DevFailed exception is thrown.
 *
 * @param [in] att_name The attribute name
 * @param [out] alias The attribute alias
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_alias_from_attribute(std::string att_name,std::string &alias);
/**
 * Get attribute name from its alias
 *
 * Get the full attribute name from an alias. The attribute alias is specified by att_alias and the full attribute
 * name is returned in att_name. If there is no attribute with the given alias, a DevFailed exception is thrown.
 *
 * @param [in] att_alias The attribute alias
 * @param [out] att_name The attribute name
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void get_attribute_alias(std::string att_alias, std::string &att_name);
/**
 * Define attribute alias
 *
 * Set an alias for an attribute name. The attribute alias is specified by att_alias and the attribute name is
 * specifed by att_name. If the given alias already exists, a DevFailed exception is thrown.
 *
 * @param [in] att_name The attribute name
 * @param [in] att_alias The attribute alias
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void put_attribute_alias(std::string &att_name,std::string &att_alias);
/**
 * Delete attribute alias
 *
 * Remove the alias associated to an attribute name.
 *
 * @param [in] att_alias The attribute alias
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	void delete_attribute_alias(std::string &att_alias);
//@}

/**@name Database browsing oriented methods */
//@{
/**
 * Get host std::list
 *
 * Returns the std::list of all host names registered in the database.
 * @code
 * DbDatum db_datum = db->get_host_list();
 *
 * std::vector<std::string> host_list;
 * db_datum >> host_list;
 * @endcode
 *
 * @return A host name std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_host_list();
/**
 * Get host std::list with name matching a wildcard
 *
 * Returns the std::list of all host names registered in the database which match the specified wildcard (eg: "lc0*")
 * @code
 * std::string wildcard("l-c0*");
 *
 * DbDatum db_datum = db->get_host_list(wildcard);
 *
 * std::vector<std::string> host_list;
 * db_datum >> host_list;
 * @endcode
 *
 * @param [in] wildcard The wildcard
 * @return A host name std::list matching the input
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_host_list(std::string &wildcard);
/**
 * Get std::list of Tango classes embedded in a device server proess
 *
 * Query the database for a std::list of classes instancied by the specified server.
 * The DServer class exists in all TANGO servers and for this reason this class
 * is removed from the returned std::list.
 * @code
 * std::string server("Serial/1");
 *
 * DbDatum db_datum = db->get_server_class_list(server);
 *
 * std::vector<std::string> class_list;
 * db_datum >> class_list;
 * @endcode
 *
 * @param [in] ds_name The full device server process name
 * @return The std::list of Tango classes embedded in the specified server process
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_server_class_list(std::string &ds_name);
/**
 * Get std::list of all Tango device server process
 *
 * Return the std::list of all server names registered in the database.
 * @code
 * DbDatum db_datum = db->get_server_name_list();
 *
 * std::vector<std::string> server_list;
 * db_datum >> server_list;
 * @endcode
 *
 * @return The std::list of all server names
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_server_name_list();
/**
 * Get std::list of instances
 *
 * Return the std::list of all instance names existing in the database for the specifed server.
 * @code
 * std::string servername("Serial");
 *
 * DbDatum db_datum = db->get_instance_name_list(servername);
 *
 * std::vector<std::string> instance_list;
 * db_datum >> instance_list;
 * @endcode
 *
 * @param [in] ds_name The device server process executable name
 * @return The std::list of all instances for the specified device server process
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_instance_name_list(std::string &ds_name);
/**
 * Get std::list of device server processes
 *
 * Return the std::list of all servers registered in the database.
 * @code
 * DbDatum db_datum = db->get_server_list();
 *
 * std::vector<std::string> server_list;
 * db_datum >> server_list;
 * @endcode
 *
 * @return The std::list of all device server processes defined in database
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_server_list();
/**
 * Get std::list of device server processes with a wildcard
 *
 * Return the std::list of all servers registered in the database which match the specified wildcard (eg: "Serial/l*").
 * @code
 * std::string wildcard("Serial/l*");
 *
 * DbDatum db_datum = db->get_server_list(wildcard);
 *
 * std::vector<std::string> server_list;
 * db_datum >> server_list;
 * @endcode
 *
 * @param [in] wildcard The wildcard
 * @return The std::list of device server processes defined in database matching the specified wildcard
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_server_list(std::string &wildcard);
/**
 * Get std::list of device server processes running on a host
 *
 * Query the database for a std::list of servers registred on the specified host.
 * @code
 * std::string host("kidiboo");
 *
 * DbDatum db_datum = db->get_host_server_list(wildcard);
 *
 * std::vector<std::string> server_list;
 * db_datum >> server_list;
 * @endcode
 *
 * @param [in] host_name The host name
 * @return The std::list of device server processes
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_host_server_list(std::string &host_name);
/**
 * Get std::list of devices served by a device server process
 *
 * Query the database for a std::list of devices served by the specified server (1st parameter)
 * and of the specified class (2nd parameter).
 *
 * @param [in] ds_name The device server name (executable/instance)
 * @param [in] class_name The class name
 * @return The std::list of devices
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_device_name(std::string &ds_name, std::string &class_name);
/**
 * Get std::list of exported devices
 *
 * Query the database for a std::list of exported devices whose names satisfy the supplied filter
 * (* is wildcard for any character(s)).
 *
 * @param [in] filter The filter
 * @return The std::list of exported devices
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_device_exported(std::string &filter);
/**
 * Get std::list of device domain names
 *
 * Query the database for a std::list of device domain names which match the wildcard provided.
 * Wildcard character * matches any number of characters. Domain names are case insensitive.
 *
 * @param [in] wildcard The wildcard
 * @return The device domain names std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_device_domain(std::string &wildcard);
/**
 * Get std::list of device family name
 *
 * Query the database for a std::list of device family names which match the wildcard provided.
 * Wildcard character * matches any number of characters. Family names are case insensitive.
 *
 * @param [in] wildcard The wildcard
 * @return The device family names std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_device_family(std::string &wildcard);
/**
 * Get std::list of device member name
 *
 * Query the database for a std::list of device member names which match the wildcard provided.
 * Wildcard character * matches any number of characters. Member names are case insensitive.
 *
 * @param [in] wildcard The wildcard
 * @return The device member names std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_device_member(std::string &wildcard);
/**
 * Get std::list of devices/classes for a specified device server
 *
 * Query the database for a std::list of devices and classes served by the specified server.
 * Return a std::list with the following structure: {device name,class name,device name,class name,...}
 * @code
 * std::string server("Serial/1");
 *
 * DbDatum db_datum = db->get_device_class_list(server);
 *
 * std::vector<std::string> dev_list;
 * db_datum >> dev_list;
 * @endcode
 *
 * @param [in] ds_name The full device server process name
 * @return The device / class std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_device_class_list(std::string &ds_name);
/**
 * Get std::list of exported device for a class
 *
 * Query database for std::list of exported devices for the specified class.
 * @code
 * std::string classname("MyClass");
 *
 * DbDatum db_datum = db->get_device_exported_for_class(classname);
 *
 * std::vector<std::string> dev_list;
 * db_datum >> dev_list;
 * @endcode
 *
 * @param [in] class_name The Tango device class name
 * @return The exported device std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_device_exported_for_class(std::string &class_name);
/**
 * Get object (free property) std::list
 *
 * Query the database for a std::list of object (free properties) for which properties are defined and which match
 * the specified wildcard.
 * @code
 * std::string wildcard("Optic*");
 *
 * DbDatum db_datum = db->get_object_list(wildcard);
 *
 * std::vector<std::string> obj_list;
 * db_datum >> obj_list;
 * @endcode
 *
 * @param [in] wildcard The wildcard
 * @return The object (free property) std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_object_list(std::string &wildcard);
/**
 * Get object property std::list
 *
 * Query the database for a std::list of properties defined for the specified object and which match the specified wildcard.
 * @code
 * std::string objname("OpticID9");
 * std::string wildcard("Delta*");
 *
 * DbDatum db_datum = db->get_object_property_list(objname,wildcard);
 *
 * std::vector<std::string> prop_list;
 * db_datum >> prop_list;
 * @endcode
 *
 * @param [in] obj_name The object (free property) name
 * @param [in] wildcard The wildcard
 * @return The object (free property) property std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
    DbDatum get_object_property_list(std::string &obj_name,std::string &wildcard);
/**
 * Get Tango class std::list
 *
 * Query the database for a std::list of classes which match the specified wildcard.
 * @code
 * std::string wildcard("Motor*");
 *
 * DbDatum db_datum = db->get_class_list(wildcard);
 *
 * std::vector<std::string> class_list;
 * db_datum >> class_list;
 * @endcode
 *
 * @param [in] wildcard The wildcard
 * @return The class std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_class_list(std::string &wildcard);
/**
 * Get class property std::list
 *
 * Query the database for a std::list of properties defined for the specified class.
 * @code
 * std::string classname("MyClass");
 *
 * DbDatum db_datum = db->get_object_property_list(classname);
 *
 * std::vector<std::string> prop_list;
 * db_datum >> prop_list;
 * @endcode
 *
 * @param [in] class_name The class name
 * @return The class property std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_class_property_list(std::string &class_name);
/**
 * Get class attribute std::list
 *
 * Query the database for a std::list of attributes defined for the specified class which match the specified wildcard.
 * @code
 * std::string classname("MyClass");
 * std::string wildcard("*");
 *
 * DbDatum db_datum = db->get_class_attribute_list(classname,wildcard);
 *
 * std::vector<std::string> att_list;
 * db_datum >> att_list;
 * @endcode
 *
 * @param [in] class_name The class name
 * @param [in] wildcard The wildcard
 * @return The class property std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_class_attribute_list(std::string &class_name,std::string &wildcard);
/**
 * Get class pipe std::list
 *
 * Query the database for a std::list of pipes defined for the specified class which match the specified wildcard.
 * @code
 * std::string classname("MyClass");
 * std::string wildcard("*");
 *
 * DbDatum db_datum = db->get_class_pipe_list(classname,wildcard);
 *
 * std::vector<std::string> pipe_list;
 * db_datum >> pipe_list;
 * @endcode
 *
 * @param [in] class_name The class name
 * @param [in] wildcard The wildcard
 * @return The class pipe std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_class_pipe_list(const std::string &class_name,const std::string &wildcard);
/**
 * Get device alias std::list
 *
 * Get device alias std::list. The parameter is a std::string to filter the alias std::list returned. Wildcard (*) is supported.
 * For instance, if the std::string passed as the method parameter is initialised with only the * character, all
 * the defined device alias will be returned. The DbDatum returned by this method is initialised with an array
 * of std::strings and must be extracted into a std::vector<std::string>. If there is no alias with the given filter, the returned
 * array will have a 0 size.
 * @code
 * DbData db_data;
 * std::string filter("*");
 *
 * db_data = db->get_device_alias_list(filter);
 *
 * std::vector<std::string> al_list;
 * db_data >> al_list;
 *
 * cout << al_list.size() << " device alias defined in db" << std::endl;
 * for (int i=0;i < al_list.size();i++)
 *     cout << "alias = " << al_list[i] << std::endl;
 * @endcode
 *
 * @param [in] filter The filter
 * @return The device alias std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_device_alias_list(std::string &filter);
/**
 * Get attribute alias std::list
 *
 * Get attribute alias std::list. The parameter is a std::string to filter the alias std::list returned. Wildcard (*) is
 * supported. For instance, if the std::string passed as the method parameter is initialised with only the
 * * character, all the defined attribute alias will be returned. The DbDatum returned by this method is
 * initialised with an array of std::strings and must be extracted into a std::vector<std::string>. If there is no alias with the
 * given filter, the returned array will have a 0 size.
 *
 * @param [in] filter The filter
 * @return The attribute alias std::list
 *
 * @exception ConnectionFailed,CommunicationFailed,DevFailed from device
 */
	DbDatum get_attribute_alias_list(std::string &filter);
//@}


///@privatesection
	Database(std::string &host, int port, CORBA::ORB *orb=NULL);
	Database(std::string &file);

	Database(const Database &);
	Database & operator=(const Database &);

	void write_filedatabase();
	void reread_filedatabase();
	void write_event_channel_ior_filedatabase(std::string &);
	void build_connection ();
	void post_reconnection();
	~Database();
	inline Device_var &get_dbase() { return device;}
	void check_tango_host(const char *);
	AccessControlType check_access_control(std::string &);
	bool is_control_access_checked() {return access_checked;}
	void set_access_checked(bool val) {access_checked = val;}

	void set_tango_utils(Tango::Util *ptr) {db_tg=ptr;}
	int get_server_release() {return serv_version;}

	DevErrorList &get_access_except_errors() {return access_except_errors;}
	void clear_access_except_errors() {access_except_errors.length(0);}
	bool is_command_allowed(std::string &,std::string &);

	bool is_multi_tango_host() {return db_multi_svc;}
	std::vector<std::string> &get_multi_host() {return multi_db_host;}
	std::vector<std::string> &get_multi_port() {return multi_db_port;}

	const std::string &get_file_name();
	const std::string &get_orig_tango_host() {return ext->orig_tango_host;}
	void set_orig_tango_host(const std::string &_s) {ext->orig_tango_host=_s;}

#ifdef _TG_WINDOWS_
	Database(CORBA::ORB *orb,std::string &,std::string &);
	long get_tango_host_from_reg(char **,std::string &,std::string &);
#endif

//
// general methods
//

	CORBA::Any *fill_server_cache(std::string &,std::string &);

//
// device methods
//

	DbDatum get_device_name(std::string &, std::string &,DbServerCache *dsc);

//
// server methods
//

	DbServerInfo get_server_info(std::string &);
	void put_server_info(DbServerInfo &);
	void delete_server_info(std::string &);

//
// property methods
//

	void get_property(std::string, DbData &,DbServerCache *dsc);
	void get_property_forced(std::string, DbData &,DbServerCache *dsc = NULL);
	void get_device_property(std::string, DbData &, DbServerCache *dsc);
	DbDatum get_device_property_list(std::string &,std::string &);
	void get_device_property_list(std::string &,const std::string &,std::vector<std::string> &,DbServerCache *dsc = NULL);
	void get_device_attribute_property(std::string, DbData &, DbServerCache *dsc);
	void get_device_pipe_property(std::string, DbData &, DbServerCache *dsc);
	void delete_all_device_attribute_property(std::string, DbData &);
	void delete_all_device_pipe_property(std::string, DbData &);
	void get_class_property(std::string, DbData &, DbServerCache *dsc);
	void get_class_attribute_property(std::string, DbData &, DbServerCache *dsc);
	void get_class_pipe_property(std::string, DbData &, DbServerCache *dsc);

//
// event methods
//

	void export_event(DevVarStringArray *);
	void unexport_event(std::string &);
	CORBA::Any *import_event(std::string &);

};

//
// Some Database class inline methods
//

inline std::string Database::dev_name()
{
	if (db_device_name.empty() == true)
	{
		CORBA::String_var n = device->name();
		db_device_name = n;
	}
	return db_device_name;
}

#endif /* _DATABASE_H */
