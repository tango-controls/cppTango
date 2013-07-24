//
// dbapi.h -	include file for TANGO database api
//
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.


#ifndef _DBDEVICE_H
#define _DBDEVICE_H

/****************************************************************************************
 * 																						*
 * 					The DbDevice class													*
 * 					------------------													*
 * 																						*
 ***************************************************************************************/

/**
 * A high level object for a device interface to the database
 *
 * A database object for a device which can be used to query or modify properties, import and export information
 * for a device. This class provides an easy to use interface for device objects in the database. It uses
 * the methods of the Database class therefore the reader is referred to these for the exact calling syntax and
 * examples. The following methods are defined for the DbDevice class :
 *
 * $Author: taurel $
 * $Revision: 1 $
 *
 * @headerfile tango.h
 * @ingroup DBase
 */

class DbDevice
{
private :
	string 		name;
	Database 	*dbase;
	int 		db_ind;
	bool 		ext_dbase;

    class DbDeviceExt
    {
    public:
        DbDeviceExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<DbDeviceExt>     ext;
#else
	DbDeviceExt	                *ext;
#endif

public :
/**@name Constructors */
//@{
/**
 * Create a DbDevice object.
 *
 * A constructor for a DbDevice object for a device in the TANGO database specified by the TANGO_HOST
 * environment variable.
 *
 * @param [in] dev_name	The device name
 *
 */
	DbDevice(string &dev_name);
/**
 * Create a DbDevice object using a specified database
 *
 * A constructor for a DbDevice object for the device in the specified database. This method reuses the
 * Database supplied by the programmer.
 *
 * @param [in] dev_name	The device name
 * @param [in] db The database object
 *
 */
	DbDevice(string &dev_name, Database *db);
//@}


/**@name device oriented methods */
//@{
/**
 * Import the device from database
 *
 * Query the database for the import info of this device. Returns a DbDevImportInfo structure.
 *
 * @return [in] The device import information
 *
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	DbDevImportInfo import_device();
/**
 * Export device info to the database
 *
 * Update the export info for this device in the database.
 *
 * @param [in] dev_info Device export info
 *
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	void export_device(DbDevExportInfo &dev_info);
//@}

/**@name property oriented methods */
//@{
/**
 * Get device property from database
 *
 * Query the database for the list of properties of this device. See Database::get_device_property() for an
 * example of how to specify and retrieve the properties.
 *
 * @param [in,out] db Property name(s) and value
 *
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	void get_property(DbData &db);
/**
 * Update device property in database
 *
 * Update the list of properties for this device in the database. See Database::put_device_property() for an
 * example of how to specify the properties.
 *
 * @param [in] db Property name(s) and value
 *
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	void put_property(DbData &db);
/**
 * Remove device property from database
 *
 * Delete the list of specified properties for this device in the database. See Database::delete_property() for
 * an example of how to specify the properties.
 *
 * @param [in] db Property name(s)
 *
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	void delete_property(DbData &db);
/**
 * Get device attribute property from database
 *
 * Query the database for the list of attribute properties of this device. See Database::get_device_attribute_property()
 * for an example of how to specify and retrieve the properties.
 *
 * @param [in,out] db Property name(s) and value
 *
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	void get_attribute_property(DbData &db);
/**
 * Update device attribute property in database
 *
 * Update the list of attribute properties for this device in the database. See Database::put_device_attribute_property()
 * for an example of how to specify the properties.
 *
 * @param [in] db Property name(s) and value
 *
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	void put_attribute_property(DbData &db);
/**
 * Remove device attribute property from database
 *
 * Delete all properties for the list of specified attributes for this device in the database. See Database::delete_device_attribute_property()
 * for an example of how to specify the properties.
 *
 * @param [in] db Property name(s)
 *
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	void delete_attribute_property(DbData &db);
//@}

/// @privatesection

	DbDevice(string &,string &,string &);
	~DbDevice();
	void set_name(string &new_name) {name = new_name;}
	Database *get_dbase();
	void set_dbase(Database *db) {dbase = db;}

	AccessControlType check_access_control();
	void clear_access_except_errors();
	void get_property_list(const string &,vector<string> &);
};
#endif /* _DBDEVICE_H */
