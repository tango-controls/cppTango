//////////////////////////////////////////////////////////////////
//
// DeviceProxy.h - include file for TANGO device api
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
//
// $Revision: 1 $
//
///////////////////////////////////////////////////////////////

#ifndef _DEVICEPROXY_H
#define _DEVICEPROXY_H


/****************************************************************************************
 * 																						*
 * 					The DeviceProxy class												*
 * 					--------------------												*
 * 																						*
 ***************************************************************************************/

/**
 * High level class which provides the client with an easy-to-use interface to TANGO devices.
 *
 * The high level object which provides the client with an easy-to-use interface to TANGO devices. DeviceProxy
 * is a handle to the real Device (hence the name Proxy) and is not the real Device (of course).
 * DeviceProxy provides interfaces to all TANGO devices. The DeviceProxy manages timeouts,
 * stateless connections (new DeviceProxy() nearly always works), and reconnection if the device server is
 * restarted.
 *
 * $Author: taurel $
 * $Revision: 1 $
 *
 * @ingroup Client
 * @headerfile tango.h
 */

class DeviceProxy: public Tango::Connection
{
private :
	void real_constructor(string &,bool ch_acc=true);

	Tango::DbDevice 	*db_dev;
	string 				device_name;
	string 				alias_name;
	DeviceInfo 			_info;
	bool 				is_alias;
	DeviceProxy 		*adm_device;
	string 				adm_dev_name;
	omni_mutex 			netcalls_mutex;
	int					lock_ctr;
	int					lock_valid;

	void connect_to_adm_device();

	void retrieve_read_args(TgRequest &,vector<string> &);
	DeviceAttribute *redo_synch_read_call(TgRequest &);
	vector<DeviceAttribute> *redo_synch_reads_call(TgRequest &);
	void redo_synch_write_call(TgRequest &);
	void write_attribute(const AttributeValueList &);
	void write_attribute(const AttributeValueList_4 &);
	void create_locking_thread(ApiUtil *,DevLong);
	void local_import(string &);

	enum read_attr_type
	{
		SIMPLE,
		MULTIPLE
	};


	void read_attr_except(CORBA::Request_ptr,long,read_attr_type);
	void write_attr_except(CORBA::Request_ptr,long,TgRequest::ReqType);
	void check_connect_adm_device();

	friend class AttributeProxy;

protected :
/// @privatesection
	virtual string get_corba_name(bool);
	virtual string build_corba_name();
	virtual int get_lock_ctr() {return lock_ctr;}
	virtual void set_lock_ctr(int lo) {lock_ctr=lo;}

	enum polled_object
	{
		Cmd,
		Attr
	};

	bool is_polled(polled_object,string &, string &);
	virtual void reconnect(bool);
	void get_remaining_param(AttributeInfoListEx *);
	void from_hist4_2_AttHistory(DevAttrHistory_4_var &,vector<DeviceAttributeHistory> *);
	void from_hist4_2_DataHistory(DevCmdHistory_4_var &,vector<DeviceDataHistory> *);
	void ask_locking_status(vector<string> &,vector<DevLong> &);
	void get_locker_host(string &,string &);

	void same_att_name(vector<string> &,const char *);

private:
    class DeviceProxyExt
    {
    public:
        DeviceProxyExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<DeviceProxyExt>  ext_proxy;
#else
	DeviceProxyExt		        *ext_proxy;		// Class extension
#endif

    omni_mutex                  lock_mutex;

public :
/**@name Constructors */
//@{
/**
 * Create a DeviceProxy instance.
 *
 * Create a DeviceProxy to a device of the specified name. The TANGO_HOST environment variable is used
 * to determine which TANGO database to connect to. The client can specify an ORB as argument if she
 * wants to. The constructor will connect to the TANGO database, query for the client’s network address and
 * build a connection to the device. If the device is defined in the TANGO database but the device server is
 * not running DeviceProxy will try to build a connection every time the client tries to access the device. If
 * the device is not defined an exception is thrown. Example :
 * \code
 * DeviceProxy *my_device = new DeviceProxy(“my/own/device”);
 * \endcode
 * See appendix on device naming in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>
 * for all details about Tango device naming syntax.
 * If an alias name is
 * defined for the device, this alias name can be used to create the DeviceProxy instance.
 *
 * @param [in] name The device name
 * @param [in] orb Pointer to the ORB. Default value is fine for 99 % of cases
 *
 * @throws WrongNameSyntax, ConnectionFailed
 *
 */
	DeviceProxy(string &name, CORBA::ORB *orb=NULL);
/**
 * Create a DeviceProxy instance.
 *
 * Create a DeviceProxy to a device of the specified name. The TANGO_HOST environment variable is used
 * to determine which TANGO database to connect to. The client can specify an ORB as argument if she
 * wants to. The constructor will connect to the TANGO database, query for the client’s network address and
 * build a connection to the device. If the device is defined in the TANGO database but the device server is
 * not running DeviceProxy will try to build a connection every time the client tries to access the device. If
 * the device is not defined an exception is thrown. Example :
 * \code
 * DeviceProxy *my_device = new DeviceProxy(“my/own/device”);
 * \endcode
 * See appendix on device naming in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>
 * for all details about Tango device naming syntax.
 * If an alias name is
 * defined for the device, this alias name can be used to create the DeviceProxy instance.
 *
 * @param [in] name The device name
 * @param [in] orb Pointer to the ORB. Default value is fine for 99 % of cases
 *
 * @throws WrongNameSyntax, ConnectionFailed
 *
 */
	DeviceProxy(const char *name, CORBA::ORB *orb=NULL);
//@}
/// @privatesection
	DeviceProxy(string &name, bool ch_access, CORBA::ORB *orb=NULL);
	DeviceProxy(const char *, bool ch_access, CORBA::ORB *orb=NULL);

	DeviceProxy(const DeviceProxy &);
	DeviceProxy & operator=(const DeviceProxy &);
	virtual ~DeviceProxy();

	DeviceProxy():Connection((CORBA::ORB *)NULL),db_dev(NULL),adm_device(NULL),ext_proxy(Tango_NullPtr)
	{dbase_used = false;}
/// @publicsection

//
// general methods
//

/**@name Miscellaneous methods */
//@{
/**
 * Get device info.
 *
 * A method which returns information on the device in a DeviceInfo structure. Example :
 * \code
 * cout << " device info : " << endl
 * DeviceInfo dev_info = my_device->info() << endl;
 * cout << " dev_class " << dev_info.dev_class;
 * cout << " server_id " << dev_info.server_id;
 * cout << " server_host " << dev_info.server_host;
 * cout << " server_version " << dev_info.server_version;
 * cout << " doc_url " << dev_info.doc_url;
 * cout << " device_type " << dev_info.dev_type;
 * \endcode
 * All DeviceInfo fields are strings except server_version
 * which is a long integer.
 *
 * @return Device information structure
 *
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual DeviceInfo const &info();
/**
 * Get device state.
 *
 * A method which returns the state of the device as a Tango::DevState type. Example :
 * \code
 * Tango::DevState dev_state = my_device->state();
 * \endcode
 *
 * @return Device state enumeration
 *
 * @throws ConnectionFailed, CommunicationFailed
 */
	virtual DevState state();
/**
 * Get device status.
 *
 * A method which returns the status of the device as a string. Example :
 * \code
 * cout << "device status: " << my_device->status() << endl;
 * \endcode
 *
 * @return Device status
 *
 * @throws ConnectionFailed, CommunicationFailed
 */
	virtual string status();
/**
 * Ping a device.
 *
 * A method which sends a ping to the device and returns the time elapsed as microseconds. Example :
 * @code
 * cout << " device ping took " << my_device->ping() << " microseconds" << endl;
 * @endcode
 *
 * @throws ConnectionFailed, CommunicationFailed
 */
	virtual int ping();
/**
 * Get device black box content
 *
 * Get the last n commands executed on the device server and return a pointer to a vector of strings containing
 * the date, time, command, and from which client computer the command was executed. This method
 * allocates memory for the vector of strings returned to the caller. It is the caller responsibility to delete this
 * memory.
 *
 * @param [in] nb Number of requested records. If more records than available is requested, only the available records are returned
 * @return Black box content
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual vector<string> *black_box(int nb);
/**
 * Return the device name (from the device itself)
 *
 * Return the device name (from the device itself)
 *
 * @return The device name
 * @throws ConnectionFailed, CommunicationFailed
 */
	virtual string name();
/**
 * Return the administrator device name
 *
 * Returns the name of the corresponding administrator device. This is useful if you need to send an administration
 * command to the device server e.g. restart it.
 *
 * @return The administrator device name
 * @throws ConnectionFailed, CommunicationFailed
 */
	virtual string adm_name();
/**
 * Return the device name as it is stored locally
 *
 * Return the device name as it is stored locally
 *
 * @return The device name
 */
	virtual inline string dev_name() { return device_name; }
/**
 * Returns the device description as a string.
 *
 * Returns the device description as a string.
 *
 * @return The device description
 * @throws ConnectionFailed, CommunicationFailed
 */
	virtual string description();
/**
 * Returns device alias
 *
 * Returns the device alias name if one is defined otherwise, throws a DevFailed exception with the reason
 * field set to Db_AliasNotDefined.
 *
 * @return The device alias
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual string alias();
/**
 * Query the device for import info from the database.
 *
 * Query the device for import info from the database.
 *
 * @return The device import info
 * @throws ConnectionFailed, CommunicationFailed, NonDbDevice
 */
	virtual DbDevImportInfo import_info();
/**
 * Get device Tango lib version
 *
 * Get the version of the Tango library used by the device
 *
 * @return The device Tango lib version
 */
	virtual int get_tango_lib_version();
//@}

/** @name Synchronous command oriented methods */
//@{
/**
 * Query the device for single command information.
 *
 * Query the device for information about a single command. This command returns a single CommandInfo
 * type.
 *
 * @param [in] cmd_name The command name
 * @return The command information structure
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual CommandInfo command_query(string cmd_name);
/**
 * Query the device for all commands information.
 *
 * Query the device for info on all commands. This method returns a vector of CommandInfo types. This
 * method allocates memory for the vector of CommandInfo returned to the caller. It is the caller responsibility
 * to delete this memory
 *
 * @return The command information list: One CommandInfo structure per command
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual CommandInfoList *command_list_query();
/**
 * Retrieve command history from polling buffer.
 *
 * Retrieve command history from the command polling buffer. The first argument is the command name.
 * The second argument is the wanted history depth. This method returns a vector of DeviceDataHistory
 * types. This method allocates memory for the vector of DeviceDataHistory returned to the caller. It is the
 * caller responsibility to delete this memory. See chapter "Advanced Feature"
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>
 * for all details regarding polling.
 * @code
 * DeviceProxy dev = new DeviceProxy("...");
 * vector<DeviceDataHistory> *hist;
 *
 * hist = dev->command_history("Status",5);
 *
 * for (int i = 0;i < 5;i++)
 * {
 *     bool fail = (*hist)[i].has_failed();
 *     if (fail == false)
 *     {
 *        string str;
 *        (*hist)[i] >> str;
 *        cout << "Status = " << str << endl;
 *     }
 *     else
 *     {
 *        cout << "Command failed !" << endl;
 *        cout << "Error level 0 desc = " << ((*hist)[i].errors())[0].desc << endl;
 *     }
 *     cout << "Date = " << (*hist)[i].date().tv_sec << endl;
 * }
 * delete hist;
 * @endcode
 *
 * @param [in] cmd_name The command name
 * @param [in] depth The required history depth
 * @return The command information list: One CommandInfo structure per command
 * @throws NonSupportedFeature, ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual vector<DeviceDataHistory> *command_history(string &cmd_name,int depth);
/**
 * Retrieve command history from polling buffer.
 *
 * Retrieve command history from the command polling buffer. The first argument is the command name.
 * The second argument is the wanted history depth. This method returns a vector of DeviceDataHistory
 * types. This method allocates memory for the vector of DeviceDataHistory returned to the caller. It is the
 * caller responsibility to delete this memory. See chapter "Advanced Feature"
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>
 * for all details regarding polling.
 * @code
 * DeviceProxy dev = new DeviceProxy("...");
 * vector<DeviceDataHistory> *hist;
 *
 * hist = dev->command_history("Status",5);
 *
 * for (int i = 0;i < 5;i++)
 * {
 *     bool fail = (*hist)[i].has_failed();
 *     if (fail == false)
 *     {
 *        string str;
 *        (*hist)[i] >> str;
 *        cout << "Status = " << str << endl;
 *     }
 *     else
 *     {
 *        cout << "Command failed !" << endl;
 *        cout << "Error level 0 desc = " << ((*hist)[i].errors())[0].desc << endl;
 *     }
 *     cout << "Date = " << (*hist)[i].date().tv_sec << endl;
 * }
 * delete hist;
 * @endcode
 *
 * @param [in] cmd_name The command name
 * @param [in] depth The required history depth
 * @return The command information list: One CommandInfo structure per command
 * @throws NonSupportedFeature, ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual vector<DeviceDataHistory> *command_history(const char *cmd_name,int depth)
			{string str(cmd_name);return command_history(str,depth);}
//@}

/** @name Synchronous attribute related methods */
//@{
/**
 * Query the device for attribute information
 *
 * Query the device for information about a single attribute. This command returns a single AttributeInfoEx
 * type.
 *
 * @b NOTE on compatibility between Tango V4 and Tango V5 regarding attribute properties:
 * @n Between Tango V4 and Tango V5, attribute configuration has been modified to incorporate alarm and event
 * related parameters. This explains why it exists two structure types for attribute configuration parameters.
 * All Tango V4 parameters are defined in a structure called AttributeInfo and a new structure called AttributeInfoEx
 * has been defined for all Tango V5 parameters. Nevertheless, AttributeInfoEx inherits from
 * AttributeInfo and it is always possible to call the Tango V5 DeviceProxy::attribute_query() method and to
 * store its result in one AttributeInfo structure thus allowing compatibility for client written for Tango V4
 * but linked with Tango V5. It is also possible for a client written and linked with Tango V5 to call Tango
 * V5 DeviceProxy::attribute_query() method to all kind of Tango devices. For device using Tango V4, the
 * alarm and event related parameters will be retrieved from the database instead of from the device
 *
 * @param [in] att_name The attribute name
 * @return The attribute information structure
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual AttributeInfoEx attribute_query(string att_name) {return get_attribute_config(att_name);}
/**
 * Query the device for information on all attributes
 *
 * Query the device for info on all attributes. This method returns a vector of AttributeInfo types.
 * This method allocates memory for the
 * vector of AttributeInfo structures returned to the caller. It is the caller responsibility to delete this memory.
 * @n See DeviceProxy::attribute_query for a note about compatibility between attribute properties structure
 *
 * @return A vector of AttributeInfo structures with one element per attribute
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual AttributeInfoList *attribute_list_query();
/**
 * Query the device for information on all attributes
 *
 * Query the device for info on all attributes. This method returns a vector of AttributeInfoEx types.
 * This method allocates memory for the
 * vector of AttributeInfoEx structures returned to the caller. It is the caller responsibility to delete this memory.
 * @n See DeviceProxy::attribute_query for a note about compatibility between attribute properties structure
 *
 * @return A vector of AttributeInfoEx structures with one element per attribute
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual AttributeInfoListEx *attribute_list_query_ex();
/**
 * Query all attributes name
 *
 * Return the names of all attributes implemented for this device as a vector of strings. This method allocates
 * memory for the vector of strings returned to the caller. It is the caller responsibility to delete this memory.
 *
 * @return A vector of string with one string per attribute
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual vector<string> *get_attribute_list();
/**
 * Get attribute configuration for a list of attributes
 *
 * Return the attribute configuration for the list of specified attributes. To get all the attributes pass a vector
 * containing the string AllAttr (defined in tango_const.h). This method allocates memory for the vector of
 * AttributeInfo returned to the caller. It is the caller responsibility to delete this memory.
 * @n See DeviceProxy::attribute_query for a note about compatibility between attribute properties structure
 *
 * @param [in] att_names Attributes name list
 * @return A vector of AttributeInfo structures with one element per attribute
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual AttributeInfoList *get_attribute_config(vector<string> &att_names);
/**
 * Get attribute configuration (extended) for a list of attributes
 *
 * Return the extended attribute configuration for the list of specified attributes. To get all the attributes
 * pass a vector containing the string AllAttr (defined in tango_const.h). This method allocates memory for
 * the vector of AttributeInfoEx returned to the caller. It is the caller responsibility to delete this memory.
 * @n See DeviceProxy::attribute_query for a note about compatibility between attribute properties structure
 *
 * @param [in] att_names Attributes name list
 * @return A vector of AttributeInfoEx structures with one element per attribute
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual AttributeInfoListEx *get_attribute_config_ex(vector<string> &att_names);
/**
 * Get attribute configuration (extended) for a single attribute
 *
 * Return the attribute configuration for a single attribute.
 * @n See DeviceProxy::attribute_query for a note about compatibility between attribute properties structure
 *
 * @param [in] att_name Attribute name
 * @return The extended attribute information
 * @throws ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual AttributeInfoEx get_attribute_config(const string &att_name);
/**
 * Set attribute configuration
 *
 * Change the attribute configuration for the specified attributes.
 * @n See DeviceProxy::attribute_query for a note about compatibility between attribute properties structure
 *
 * @param [in] atts New attributes configuration
 * @throws ConnectionFailed, CommunicationFailed, DevUnlocked, DevFailed from device
 */
	virtual void set_attribute_config(AttributeInfoList &atts);
/**
 * Set extended attribute configuration
 *
 * Change the extended attribute configuration for the specified attributes.
 * @n See DeviceProxy::attribute_query for a note about compatibility between attribute properties structure
 *
 * @param [in] atts New extended attributes configuration
 * @throws ConnectionFailed, CommunicationFailed, DevUnlocked, DevFailed from device
 */
	virtual void set_attribute_config(AttributeInfoListEx &atts);
/**
 * Read the list of specified attributes
 *
 * Read the list of specified attributes. To extract the value you have to use the operator of the class DeviceAttribute
 * which corresponds to the data type of the attribute. NOTE: There is no automatic type conversion
 * from the attribute native type to user type e.g. if an attribute returns a short you cannot extract it as a
 * double, you have to extract it as a short. By default, if the server reports error for one of the attribute
 * in the list, this error will be passed to the user using exception when he (she) will try to extract the data
 * form the corresponding See sub-chapter on DeviceAttribute to learn how to change this default behaviour.
 * DeviceAttribute object. This method allocates memory for the vector of DeviceAttribute objects returned
 * to the caller. This is the caller responsibility to delete this memory. Example :
 * @code
 * vector<DeviceAttribute> *devattr;
 * vector<string> attr_names;
 *
 * attr_names.push_back("attribute_1");
 * attr_names.push_back("attribute_2");
 *
 * devattr = device->read_attributes(attr_names);
 *
 * short short_attr_1;
 * long long_attr_2;
 *
 * (*devattr)[0] >> short_attr_1;
 * (*devattr)[1] >> long_attr_2;
 *
 * cout << "my_attribute value " << short_attr;
 * delete devattr;
 * @endcode
 *
 * @param [in] att_names Attribute names
 * @return A vector of DeviceAttribute instances with one element for each read attribute
 * @throws ConnectionFailed, CommunicationFailed
 */
	virtual vector<DeviceAttribute> *read_attributes(vector<string> &att_names);
/**
 * Read the list of specified attributes
 *
 * Read a single attribute. To extract the value you have to use the operator of the class DeviceAttribute
 * which corresponds to the data type of the attribute. NOTE: There is no automatic type conversion from the
 * attribute native type to user type e.g. if an attribute returns a short you cannot extract it as a double (this
 * will return 0) you have to extract it as a short.
 * See usage eample in DeviceProxy::read_attributes
 *
 * @param [in] att_name Attribute name
 * @return The attribute value in a DeviceAttribute instance
 * @throws ConnectionFailed, CommunicationFailed
 */
	virtual DeviceAttribute read_attribute(string &att_name);
/**
 * Read the list of specified attributes
 *
 * Read a single attribute. To extract the value you have to use the operator of the class DeviceAttribute
 * which corresponds to the data type of the attribute. NOTE: There is no automatic type conversion from the
 * attribute native type to user type e.g. if an attribute returns a short you cannot extract it as a double (this
 * will return 0) you have to extract it as a short.
 * See usage eample in DeviceProxy::read_attributes
 *
 * @param [in] att_name Attribute name
 * @return The attribute value in a DeviceAttribute instance
 * @throws ConnectionFailed, CommunicationFailed
 */
	virtual DeviceAttribute read_attribute(const char *att_name) {string str(att_name);return read_attribute(str);}
/**
 * Write the specified attributes
 *
 * Write the specified attributes. To insert the values to write you have to use the operator of the DeviceAttribute
 * class which corresponds to the data type of the attribute. NOTE: There is no automatic type conversion
 * from the user type to the attribute native type e.g. if an attribute expects a short you cannot insert it as
 * a double (this will throw an exception) you have to insert it as a short. Note that this is the only API call
 * which could throw a NamedDevFailedList exception. See @ref except to get all the details on this exception.
 * Example :
 * @code
 * vector<DeviceAttribute> attr_in;
 *
 * string att1_name("First_attr");
 * string att2_name("Second_attr");
 *
 * short short_attr = ...;
 * double double_attr = ...;
 *
 * attr_in.push_back(DeviceAttribute(att1_name,short_attr));
 * attr_in.push_back(DeviceAttribute(att2_name,double_attr));
 *
 * device->write_attributes(attr_in);
 * @endcode
 *
 * @param [in] attr_in Attributes name and value
 * @throws ConnectionFailed, CommunicationFailed, DeviceUnlocked, DevFailed or NamedDevFailedList from device
 */
    virtual void write_attributes(vector<DeviceAttribute> &attr_in);
/**
 * Write a single attribute
 *
 * Write a single attribute. To insert the value to write you have to use the operator of the class DeviceAttribute
 * which corresponds to the data type of the attribute. NOTE: There is no automatic type conversion from the
 * user type to the attribute native type e.g. if an attribute expects a short you cannot insert it as a double (this
 * will throw an exception) you have to insert it as a short. See Deviceproxy::write_attributes for a usage example.
 *
 * @param [in] attr_in Attribute name and value
 * @throws ConnectionFailed, CommunicationFailed, DeviceUnlocked, DevFailed from device
 */
	virtual void write_attribute(DeviceAttribute &attr_in);
/**
 * Write the read a single attribute
 *
 * Write then read a single attribute in a single network call. By default (serialisation by device), the execution
 * of this call in the server can’t be interrupted by other clients. To insert/extract the value to write/read you
 * have to use the operator of the class DeviceAttribute which corresponds to the data type of the attribute.
 * NOTE: There is no automatic type conversion from the user type to the attribute native type e.g. if an
 * attribute expects a short you cannot insert it as a double (this will throw an exception) you have to insert it
 * as a short.
 *
 * @param [in] attr_in Attribute name and value (to be written)
 * @return The read attribute data
 * @throws ConnectionFailed, CommunicationFailed, DeviceUnlocked, DevFailed from device
 */
	virtual DeviceAttribute write_read_attribute(DeviceAttribute &attr_in);
/**
 * Retrieve attribute history from polling buffer
 *
 * Retrieve attribute history from the attribute polling buffer. The first argument is the attribute name. The
 * second argument is the wanted history depth. This method returns a vector of DeviceAttributeHistory
 * types. This method allocates memory for the vector of DeviceAttributeHistory returned to the caller. It is
 * the caller responsibility to delete this memory.
 * See chapter on Advanced Feature
 * in <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>
 * for all details regarding polling.
 * @code
 * DeviceProxy dev = new DeviceProxy("...");
 * vector<DeviceAttributeHistory> *hist;
 *
 * hist = dev->attribute_history("Current",5);
 *
 * for (int i = 0;i < 5;i++)
 * {
 *    bool fail = (*hist)[i].has_failed();
 *    if (fail == false)
 *    {
 *       cout << "Attribute name = " << (*hist)[i].get_name() << endl;
 *       cout << "Attribute quality factor = " << (*hist)[i].get_quality() << endl;
 *       long value;
 *       (*hist)[i] >> value;
 *       cout << "Current = " << value << endl;
 *    }
 *    else
 *    {
 *       cout << "Attribute failed !" << endl;
 *       cout << "Error level 0 desc = " << ((*hist)[i].get_err_stack())[0].desc << endl;
 *    }
 *    cout << "Date = " << (*hist)[i].get_date().tv_sec << endl;
 * }
 * delete hist;
 * @endcode
 *
 * @param [in] att_name Attribute name
 * @param [in] depth The required history depth
 * @return The read attribute history data
 * @throws NonSupportedFeature, ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual vector<DeviceAttributeHistory> *attribute_history(string &att_name,int depth);
/**
 * Retrieve attribute history from polling buffer
 *
 * Retrieve attribute history from the attribute polling buffer. The first argument is the attribute name. The
 * second argument is the wanted history depth. This method returns a vector of DeviceAttributeHistory
 * types. This method allocates memory for the vector of DeviceAttributeHistory returned to the caller. It is
 * the caller responsibility to delete this memory.
 * See chapter on Advanced Feature
 * in <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>
 * for all details regarding polling.
 * @code
 * DeviceProxy dev = new DeviceProxy("...");
 * vector<DeviceAttributeHistory> *hist;
 *
 * hist = dev->attribute_history("Current",5);
 *
 * for (int i = 0;i < 5;i++)
 * {
 *    bool fail = (*hist)[i].has_failed();
 *    if (fail == false)
 *    {
 *       cout << "Attribute name = " << (*hist)[i].get_name() << endl;
 *       cout << "Attribute quality factor = " << (*hist)[i].get_quality() << endl;
 *       long value;
 *       (*hist)[i] >> value;
 *       cout << "Current = " << value << endl;
 *    }
 *    else
 *    {
 *       cout << "Attribute failed !" << endl;
 *       cout << "Error level 0 desc = " << ((*hist)[i].get_err_stack())[0].desc << endl;
 *    }
 *    cout << "Date = " << (*hist)[i].get_date().tv_sec << endl;
 * }
 * delete hist;
 * @endcode
 *
 * @param [in] att_name Attribute name
 * @param [in] depth The required history depth
 * @return The read attribute history data
 * @throws NonSupportedFeature, ConnectionFailed, CommunicationFailed, DevFailed from device
 */
	virtual vector<DeviceAttributeHistory> *attribute_history(const char *att_name,int depth)
			{string str(att_name);return attribute_history(str,depth);}
//@}

/** @name Asynchronous attribute related methods */
//@{
/**
 * Read a single attribute asynchronously
 *
 * Read asynchronously (polling model) a single attribute. This call returns an asynchronous call identifier
 * which is needed to get the attribute value.
 *
 * @param [in] att_name The attributes name
 * @return The call identifier
 * @throws ConnectionFailed
 */
    virtual long read_attribute_asynch(string &att_name);
/**
 * Read a single attribute asynchronously
 *
 * Read asynchronously (polling model) a single attribute. This call returns an asynchronous call identifier
 * which is needed to get the attribute value.
 *
 * @param [in] att_name The attributes name
 * @return The call identifier
 * @throws ConnectionFailed
 */
	virtual long read_attribute_asynch(const char *att_name) {string tmp(att_name);return read_attribute_asynch(tmp);}
/**
 * Read asynchronously alist of attributes
 *
 * Read asynchronously (polling model) the list of specified attributes. This call returns an asynchronous call
 * identifier which is needed to get attributes value.
 *
 * @param [in] att_names The attributes names
 * @return The call identifier
 * @throws ConnectionFailed
 */
	virtual long read_attributes_asynch(vector <string> &att_names);
/**
 * Check if an asynchronous read_attributes call is arrived
 *
 * Check if the answer of an asynchronous read_attribute is arrived (polling model). id is the asynchronous
 * call identifier. If the reply is arrived and if it is a valid reply, it is returned to the caller in a DeviceAttribute
 * object. If the reply is an exception, it is re-thrown by this call. An exception is also thrown in case of the
 * reply is not yet arrived. To extract attribute value, you have to use the operator of the class DeviceAttribute
 * which corresponds to the data type of the attribute. NOTE: There is no automatic type conversion from
 * the attribute native type to user type e.g. if an attribute returns a short you cannot extract it as a double,
 * you have to extract it as a short. Memory has been allocated for the DeviceAttribute object returned to the
 * caller. This is the caller responsibility to delete this memory.
 *
 * @param [in] id The call identifier
 * @return The attribute(s) data
 * @throws AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual vector<DeviceAttribute> *read_attributes_reply(long id);
/**
 * Check if an asynchronous read_attributes call is arrived (with timeout)
 *
 * Check if the answer of an asynchronous read_attribute is arrived (polling model). id is the asynchronous
 * call identifier. If the reply is arrived and if it is a valid reply, it is returned to the caller in a DeviceAttribute
 * object. If the reply is an exception, it is re-thrown by this call. If the reply is not yet arrived, the call will
 * wait (blocking the process) for the time specified in timeout. If after timeout milliseconds, the reply is still
 * not there, an exception is thrown. If timeout is set to 0, the call waits until the reply arrived. To extract
 * attribute value, you have to use the operator of the class DeviceAttribute which corresponds to the data type
 * of the attribute. NOTE: There is no automatic type conversion from the attribute native type to user type
 * e.g. if an attribute returns a short you cannot extract it as a double, you have to extract it as a short. Memory
 * has been allocated for the DeviceAttribute object returned to the caller. This is the caller responsibility to
 * delete this memory.
 *
 * @param [in] id The call identifier
 * @param [in] timeout The timeout value
 * @return The attribute(s) data
 * @throws AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual vector<DeviceAttribute> *read_attributes_reply(long id,long timeout);
/**
 * Check if an asynchronous read_attribute (single attribute) call is arrived
 *
 * Check if the answer of an asynchronous read_attribute is arrived (polling model). id is the asynchronous
 * call identifier. If the reply is arrived and if it is a valid reply, it is returned to the caller in a DeviceAttribute.
 * If the reply is an exception, it is re-thrown by this call. An exception is also thrown in case of the reply
 * is not yet arrived. To extract attribute value, you have to use the operator of the class DeviceAttribute
 * which corresponds to the data type of the attribute. NOTE: There is no automatic type conversion from the
 * attribute native type to user type e.g. if an attribute returns a short you cannot extract it as a double, you
 * have to extract it as a short. Memory has been allocated for the DeviceAttribute object returned to
 * the caller. This is the caller responsibility to delete this memory.
 *
 * @param [in] id The call identifier
 * @return The attribute data
 * @throws AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual DeviceAttribute *read_attribute_reply(long id);
/**
 * Check if an asynchronous read_attribute (single attribute) call is arrived (with timeout)
 *
 * Check if the answer of an asynchronous read_attributes is arrived (polling model). id is the asynchronous
 * call identifier. If the reply is arrived and if it is a valid reply, it is returned to the caller in a DeviceAttribute.
 * If the reply is an exception, it is re-thrown by this call. If the reply is not yet arrived, the call will wait
 * (blocking the process) for the time specified in timeout. If after timeout milliseconds, the reply is still not
 * there, an exception is thrown. If timeout is set to 0, the call waits until the reply arrived. To extract attribute
 * value, you have to use the operator of the class DeviceAttribute which corresponds to the data type of the
 * attribute. NOTE: There is no automatic type conversion from the attribute native type to user type e.g. if an
 * attribute returns a short you cannot extract it as a double, you have to extract it as a short. Memory has been
 * allocated for the DeviceAttribute object returned to the caller. This is the caller responsibility to
 * delete this memory.
 *
 * @param [in] id The call identifier
 * @param [in] timeout The timeout value
 * @return The attribute data
 * @throws AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual DeviceAttribute *read_attribute_reply(long id,long timeout);
/**
 * Write a single attribute asynchronously
 *
 * Write asynchronously (polling model) a single attribute. To insert the value to write you have to use the
 * operator of the class DeviceAttribute which corresponds to the data type of the attribute. NOTE: There is
 * no automatic type conversion from the user type to the attribute native type e.g. if an attribute expects a
 * short you cannot insert it as a double (this will throw an exception) you have to insert it as a short. This
 * call returns an asynchronous call identifier which is needed to get the server reply.
 *
 * @param [in] argin The attribute name and value
 * @return The call identifier
 * @throws ConnectionFailed
 */
	virtual long write_attribute_asynch(DeviceAttribute &argin);
/**
 * Write asynchronously alist of attributes
 *
 * Write asynchronously (polling model) the specified attributes. To insert the value to write you have to use
 * the operator of the class DeviceAttribute which corresponds to the data type of the attribute. NOTE: There
 * is no automatic type conversion from the user type to the attribute native type e.g. if an attribute expects
 * a short you cannot insert it as a double (this will throw an exception) you have to insert it as a short. This
 * call returns an asynchronous call identifier which is needed to get the server reply.
 *
 * @param [in] argin The attributes name and value
 * @return The call identifier
 * @throws ConnectionFailed
 */
	virtual long write_attributes_asynch(vector<DeviceAttribute> &argin);
/**
 * Check if the answer of one asynchronous write_attribute (single attribute) call is arrived
 *
 * Check if the answer of an asynchronous write_attribute is arrived (polling model). id is the asynchronous
 * call identifier. If the reply is arrived and if it is a valid reply, the call returned. If the reply is an exception,
 * it is re-thrown by this call. An exception is also thrown in case of the reply is not yet arrived.
 *
 * @param [in] id The call identifier
 * @throws AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual void write_attribute_reply(long id) {write_attributes_reply(id);}
/**
 * Check if the answer of one asynchronous write_attribute call (single attribute) is arrived with timeout
 *
 * Check if the answer of an asynchronous write_attribute is arrived (polling model). id is the asynchronous
 * call identifier. If the reply is arrived and if it is a valid reply, the call returned. If the reply is an exception,
 * it is re-thrown by this call. If the reply is not yet arrived, the call will wait (blocking the process) for the
 * time specified in timeout. If after timeout milliseconds, the reply is still not there, an exception is thrown.
 * If timeout is set to 0, the call waits until the reply arrived.
 *
 * @param [in] id The call identifier
 * @param [in] timeout The timeout value
 * @throws AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual void write_attribute_reply(long id,long timeout) {write_attributes_reply(id,timeout);}
/**
 * Check if the answer of one asynchronous write_attributes call is arrived
 *
 * Check if the answer of an asynchronous write_attributes is arrived (polling model). id is the asynchronous
 * call identifier. If the reply is arrived and if it is a valid reply, the call returned. If the reply is an exception,
 * it is re-thrown by this call. An exception is also thrown in case of the reply is not yet arrived.
 *
 * @param [in] id The call identifier
 * @throws AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual void write_attributes_reply(long id);
/**
 * Check if the answer of one asynchronous write_attributes call is arrived with timeout
 *
 * Check if the answer of an asynchronous write_attributes is arrived (polling model). id is the asynchronous
 * call identifier. If the reply is arrived and if it is a valid reply, the call returned. If the reply is an exception,
 * it is re-thrown by this call. If the reply is not yet arrived, the call will wait (blocking the process) for the
 * time specified in timeout. If after timeout milliseconds, the reply is still not there, an exception is thrown.
 * If timeout is set to 0, the call waits until the reply arrived.
 *
 * @param [in] id The call identifier
 * @param [in] timeout The timeout value
 * @throws AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual void write_attributes_reply(long id,long timeout);
/**
 * Read a single attribute asynchronously in callback model
 *
 * Read asynchronously (callback model) a single attribute. The last argument is a reference to a callback
 * object. This callback object should be an instance of a user class inheriting from the Tango::CallBack class
 * with the @e attr_read() method overloaded.
 *
 * @param [in] att_name The attribute name
 * @param [in] cb The call-back object
 * @throws ConnectionFailed
 */
	virtual void read_attribute_asynch(const char *att_name,CallBack &cb) {string tmp(att_name);read_attribute_asynch(tmp,cb);}
/**
 * Read a single attribute asynchronously in callback model
 *
 * Read asynchronously (callback model) a single attribute. The last argument is a reference to a callback
 * object. This callback object should be an instance of a user class inheriting from the Tango::CallBack class
 * with the @e attr_read() method overloaded.
 *
 * @param [in] att_name The attribute name
 * @param [in] cb The call-back object
 * @throws ConnectionFailed
 */
	virtual void read_attribute_asynch(string &att_name,CallBack &cb);
/**
 * Read asynchronously in callback model a list of attributes
 *
 * Read asynchronously (callback model) an attribute list. The last argument is a reference to a callback
 * object. This callback object should be an instance of a user class inheriting from the Tango::CallBack class
 * with the @e attr_read() method overloaded.
 *
 * @param [in] att_names The attribute name list
 * @param [in] cb The call-back object
 * @throws ConnectionFailed
 */
	virtual void read_attributes_asynch(vector<string> &att_names,CallBack &cb);
/**
 * Write asynchronously in callback model a single attribute
 *
 * Write asynchronously (callback model) a single attribute. The last argument is a reference to a callback
 * object. This callback object should be an instance of a user class inheriting from the Tango::CallBack class
 * with the @e attr_written() method overloaded.
 *
 * @param [in] argin The attribute name and value
 * @param [in] cb The call-back object
 * @throws ConnectionFailed
 */
	virtual void write_attribute_asynch(DeviceAttribute &argin,CallBack &cb);
/**
 * Write asynchronously in callback model a list of attributes
 *
 * Write asynchronously (callback model) an attribute list. The last argument is a reference to a callback
 * object. This callback object should be an instance of a user class inheriting from the Tango::CallBack class
 * with the @e attr_written() method overloaded.
 *
 * @param [in] argin The attribute names and values
 * @param [in] cb The call-back object
 * @throws ConnectionFailed
 */
	virtual void write_attributes_asynch(vector<DeviceAttribute> &argin,CallBack &cb);
//@}

/** @name Asynchronous attribute related methods */
//@{
/**
 * Get pending asynchronous request number
 *
 * Return number of device asynchronous pending requests. The input parameter is an enumeration with three
 * values which are:
 * @li POLLING : Returns only device polling model asynchronous request number
 * @li CALLBACK : Returns only device callback model asynchronous request number
 * @li ALL_ASYNCH : Returns device asynchronous request number
 *
 * @param [in] req The asynchronous request type
 * @return Pending asynchronous request number
 */
	virtual long pending_asynch_call(asyn_req_type req)
			{if (req == POLLING)return pasyn_ctr;
			else if (req==CALL_BACK) return pasyn_cb_ctr;
			else return (pasyn_ctr + pasyn_cb_ctr);}
//@}

/** @name Polling related methods */
//@{
/**
 * Check if a command is polled
 *
 * Returns true if the command "cmd_name" is polled. Otherwise, returns false.
 *
 * @param [in] cmd_name The command name
 * @return Flag set to true if the command is polled
 */
	virtual bool is_command_polled(string &cmd_name);
/**
 * Check if a command is polled
 *
 * Returns true if the command "cmd_name" is polled. Otherwise, returns false.
 *
 * @param [in] cmd_name The command name
 * @return Flag set to true if the command is polled
 */
	virtual bool is_command_polled(const char *cmd_name) {string tmp(cmd_name);return is_command_polled(tmp);}
/**
 * Check if one attribute is polled
 *
 * Returns true if the attribute "att_name" is polled. Otherwise, returns false.
 *
 * @param [in] att_name The attribute name
 * @return Flag set to true if the attribute is polled
 */
	virtual bool is_attribute_polled(string &att_name);
/**
 * Check if one attribute is polled
 *
 * Returns true if the attribute "att_name" is polled. Otherwise, returns false.
 *
 * @param [in] att_name The attribute name
 * @return Flag set to true if the attribute is polled
 */
	virtual bool is_attribute_polled(const char *att_name) {string tmp(att_name);return is_attribute_polled(tmp);}
/**
 * Get command polling period
 *
 * Returns the command "cmd_name" polling period in mS. If the command is not polled, it returns 0.
 *
 * @param [in] cmd_name The command name
 * @return The command polling period
 */
	virtual int get_command_poll_period(string &cmd_name);
/**
 * Get command polling period
 *
 * Returns the command "cmd_name" polling period in mS. If the command is not polled, it returns 0.
 *
 * @param [in] cmd_name The command name
 * @return The command polling period
 */
	virtual int get_command_poll_period(const char *cmd_name)
			{string tmp(cmd_name);return get_command_poll_period(tmp);}
/**
 * Get attribute polling period
 *
 * Returns the attribute "att_name" polling period in mS. If the attribute is not polled, it returns 0.
 *
 * @param [in] att_name The attribute name
 * @return The attribute polling period
 */
	virtual int get_attribute_poll_period(string &att_name);
/**
 * Get attribute polling period
 *
 * Returns the attribute "att_name" polling period in mS. If the attribute is not polled, it returns 0.
 *
 * @param [in] att_name The attribute name
 * @return The attribute polling period
 */
	virtual int get_attribute_poll_period(const char *att_name)
			{string tmp(att_name);return get_attribute_poll_period(tmp);}
/**
 * Get polling status
 *
 * Returns the device polling status. There is one string for each polled command/attribute. Each string is
 * multi-line string with :
 * @li The attribute/command name
 * @li The attribute/command polling period (in mS)
 * @li The attribute/command polling ring buffer depth
 * @li The time needed for the last command/attribute execution (in mS)
 * @li The time since data in the ring buffer has not been updated
 * @li The delta time between the last records in the ring buffer
 * @li The exception parameters in case of the last command/attribute execution failed
 *
 * This method allocates memory for the vector of string(s) returned to the caller. It is the caller responsibility
 * to delete this memory.
 *
 * @return The polling status
 */
	virtual vector<string> *polling_status();
/**
 * Poll a command
 *
 * Add the command "cmd_name" to the list of polled command. The polling period is specified by "polling_period"
 * (in mS). If the command is already polled, this method will update the polling period according to "polling_period".
 *
 * @param [in] cmd_name The command name
 * @param [in] polling_period The polling period
 */
	virtual void poll_command(string &cmd_name, int polling_period);
/**
 * Poll a command
 *
 * Add the command "cmd_name" to the list of polled command. The polling period is specified by "polling_period"
 * (in mS). If the command is already polled, this method will update the polling period according to "polling_period".
 *
 * @param [in] cmd_name The command name
 * @param [in] polling_period The polling period
 */
	virtual void poll_command(const char *cmd_name, int polling_period) {string tmp(cmd_name);poll_command(tmp,polling_period);}
/**
 * Poll an attribute
 *
 * Add the attribute "att_name" to the list of polled attributes. The polling period is specified by "polling_period" (in
 * mS). If the attribute is already polled, this method will update the polling period according to "polling_period".
 *
 * @param [in] att_name The attribute name
 * @param [in] polling_period The polling period
 */
	virtual void poll_attribute(string &att_name, int polling_period);
/**
 * Poll an attribute
 *
 * Add the attribute "att_name" to the list of polled attributes. The polling period is specified by "polling_period" (in
 * mS). If the attribute is already polled, this method will update the polling period according to "polling_period".
 *
 * @param [in] att_name The attribute name
 * @param [in] polling_period The polling period
 */
	virtual void poll_attribute(const char *att_name, int polling_period) {string tmp(att_name);poll_attribute(tmp,polling_period);}
/**
 * Stop polling a command
 *
 * Remove command "cmd_name" from the list of polled command.
 *
 * @param [in] cmd_name The command name
 */
	virtual void stop_poll_command(string &cmd_name);
/**
 * Stop polling a command
 *
 * Remove command "cmd_name" from the list of polled command.
 *
 * @param [in] cmd_name The command name
 */
	virtual void stop_poll_command(const char *cmd_name) {string tmp(cmd_name);stop_poll_command(tmp);}
/**
 * Stop polling an attribute
 *
 * Remove attribute "att_name" from the list of polled attributes
 *
 * @param [in] att_name The attribute name
 */
	virtual void stop_poll_attribute(string &att_name);
/**
 * Stop polling an attribute
 *
 * Remove attribute "att_name" from the list of polled attributes
 *
 * @param [in] att_name The attribute name
 */
	virtual void stop_poll_attribute(const char *att_name) {string tmp(att_name);stop_poll_attribute(tmp);}
//@}

/** @name Event related methods */
//@{
/**
 * Subscribe for event reception
 *
 * The client call to subscribe for event reception in the push model. The client implements a callback method
 * which is triggered when the event is received. Filtering is done based on the event
 * type. For example when reading the state and the reason specified is "change" the event will be fired only
 * when the state changes. Events consist of an attribute name and the event reason. A standard set of reasons
 * are implemented by the system, additional device specific reasons can be implemented by device servers
 * programmers.
 * The attribute parameter is the device attribute name which will be sent as an event e.g. “current”, event
 * parameter is the event reason,
 * cb is a pointer to a class inheriting from the Tango CallBack class and implementing a push_event() method.
 * The subscribe_event() call returns an event id which has to be specified when unsubscribing from this
 * event. Please, note that the cb parameter is a pointer. The lifetime of the pointed to object must at least
 * be equal to the time when events are requested because only the pointer is stored into the event machinery.
 * The same thing is true for the DeviceProxy instance on which the subscribe_event() method is called.
 *
 * @param [in] att_name The attribute name
 * @param [in] event The event type
 * @param [in] cb The callback object
 * @return The event identifier
 * @throws EventSystemFailed
 */
	virtual int subscribe_event(const string &att_name, EventType event, CallBack *cb);
/**
 * Subscribe for event reception with stateless support
 *
 * The client call to subscribe for event reception in the push model. The client implements a callback method
 * which is triggered when the event is received. Filtering is done based on the event
 * type. For example when reading the state and the reason specified is "change" the event will be fired only
 * when the state changes. Events consist of an attribute name and the event reason. A standard set of reasons
 * are implemented by the system, additional device specific reasons can be implemented by device servers
 * programmers.
 * The attribute parameter is the device attribute name which will be sent as an event e.g. “current”, event
 * parameter is the event reason,
 * cb is a pointer to a class inheriting from the Tango CallBack class and implementing a push_event() method.
 * The last call parameter is named stateless. When the stateless flag is set to false, an exception will be thrown when the event
 * subscription encounters a problem.
 * With the stateless flag set to true, the event subscription will always succeed, even if the corresponding
 * device server is not running. The keep alive thread will try every 10 seconds to subscribe for the specified
 * event. At every subscription retry, a callback is executed which contains the corresponding exception.
 * The subscribe_event() call returns an event id which has to be specified when unsubscribing from this
 * event. Please, note that the cb parameter is a pointer. The lifetime of the pointed to object must at least
 * be equal to the time when events are requested because only the pointer is stored into the event machinery.
 * The same thing is true for the DeviceProxy instance on which the subscribe_event() method is called.
 *
 * @param [in] att_name The attribute name
 * @param [in] event The event type
 * @param [in] cb The callback object
 * @param [in] stateless The stateless flag
 * @return The event identifier
 * @throws EventSystemFailed
 */
	virtual int subscribe_event(const string &att_name, EventType event, CallBack *cb,bool stateless);
/**
 * Subscribe for event reception with event queue
 *
 * The client call to subscribe for event reception in the pull model. Instead of a callback method the client
 * has to specify the size of the event reception buffer.
 * The event reception buffer is implemented as a round robin buffer. This way the client can set-up
 * different ways to receive events.
 * @li Event reception buffer size = 1 : The client is interested only in the value of the last event received.
 * All other events that have been received since the last reading are discarded.
 * @li Event reception buffer size > 1 : The client has chosen to keep an event history of a given size. When
 * more events arrive since the last reading, older events will be discarded.
 * @li Event reception buffer size = ALL_EVENTS : The client buffers all received events. The buffer size
 * is unlimited and only restricted by the available memory for the client.
 *
 * All other parameters (att_name, event, stateless) are similar to those described in DeviceProxy::subscribe_event(const string &,EventType,CallBack *,bool)
 *
 * @param [in] att_name The attribute name
 * @param [in] event The event type
 * @param [in] event_queue_size The event queue size
 * @param [in] stateless The stateless flag
 * @return The event identifier
 * @throws EventSystemFailed
 */
	virtual int subscribe_event(const string &att_name, EventType event, int event_queue_size,bool stateless = false);
/**
 * Unsubscribe for event reception
 *
 * Unsubscribe a client from receiving the event specified by event_id. event_id is the event identifier returned
 * by the DeviceProxy::subscribe_event() method.
 *
 * @param [in] event_id The event identifier
 * @throws EventSystemFailed
 */
	virtual void unsubscribe_event(int event_id);
/**
 * Fire event callback in event pull model
 *
 * The method extracts all waiting events from the event reception buffer and executes the callback method
 * cb for every event. During event subscription the client must have chosen the <B>pull model</B> for this event.
 * event_id is the event identifier returned by the DeviceProxy::subscribe_event() method.
 *
 * @param [in] event_id The event identifier
 * @param [in] cb The callback object
 * @throws EventSystemFailed
 */
	virtual void get_events (int event_id, CallBack *cb);
/**
 * Get arrived events from the event queue in event pull model
 *
 * The method extracts all waiting events from the event reception buffer. The returned event_list is a vector
 * of EventData pointers. The EventData object contains the event information as for the callback methods.
 * During event subscription the client must have chosen the <B>pull model</B> for this event. event_id is the
 * event identifier returned by the DeviceProxy::subscribe_event() method.
 *
 * @param [in] event_id The event identifier
 * @param [out] event_list The event(s) list
 * @throws EventSystemFailed
 */
	virtual void get_events (int event_id, EventDataList &event_list);
/**
 * Get arrived events from event queue in event pull model
 *
 * The method extracts all waiting attribute configuration events from the event reception buffer. The returned
 * event_list is a vector of AttrConfEventData pointers. The AttrConfEventData object contains the event
 * information as for the callback methods.
 * During event subscription the client must have chosen the <B>pull model</B> for this event. event_id is the
 * event identifier returned by the DeviceProxy::subscribe_event() method.
 *
 * @param [in] event_id The event identifier
 * @param [out] event_list The event(s) list
 * @throws EventSystemFailed
 */
	virtual void get_events (int event_id, AttrConfEventDataList &event_list);
/**
 * Get arrived events from event queue in event pull model
 *
 * The method extracts all waiting attribute configuration events from the event reception buffer. The returned
 * event_list is a vector of DataReadyEventData pointers. The DataReadyEventData object contains the event
 * information as for the callback methods.
 * During event subscription the client must have chosen the <B>pull model</B> for this event. event_id is the
 * event identifier returned by the DeviceProxy::subscribe_event() method.
 *
 * @param [in] event_id The event identifier
 * @param [out] event_list The event(s) list
 * @throws EventSystemFailed
 */
	virtual void get_events (int event_id, DataReadyEventDataList &event_list);
/**
 * Get event number in event queue
 *
 * Returns the number of stored events in the event reception buffer. After every call to DeviceProxy:get_events(),
 * the event queue size is 0.
 * During event subscription the client must have chosen the <B>pull model</B> for this event. event_id is the
 * event identifier returned by the DeviceProxy::subscribe_event() method.
 *
 * @param [in] event_id The event identifier
 * @return The event number in queue
 * @throws EventSystemFailed
 */
	virtual int  event_queue_size(int event_id);
/**
 * Get date of the last event in queue
 *
 * Returns the arrival time of the last event stored in the event reception buffer. After every call to Device-
 * Proxy:get_events(), the event reception buffer is empty. In this case an exception will be returned.
 * During event subscription the client must have chosen the <B>pull model</B> for this event. event_id is the
 * event identifier returned by the DeviceProxy::subscribe_event() method.
 *
 * @param [in] event_id The event identifier
 * @return The last event date
 * @throws EventSystemFailed
 */
	virtual TimeVal get_last_event_date(int event_id);
/**
 * Check if the event queue is empty
 *
 * Returns true when the event reception buffer is empty.
 * During event subscription the client must have chosen the <B>pull model</B> for this event. event_id is the
 * event identifier returned by the DeviceProxy::subscribe_event() method.
 *
 * @param [in] event_id The event identifier
 * @return true if the event queue is empty
 * @throws EventSystemFailed
 */
	virtual bool is_event_queue_empty(int event_id);
//@}

/** @name Property related methods */
//@{
/**
 * Get a single device property
 *
 * Get a single property for a device. The property to get is specified as a string. Refer to DbDevice::get_property()
 * and DbData sections below for details on the DbData type.
 *
 * @param [in] prop_name The property name
 * @param [out] db The property value
 * @throws NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database device
 */
	virtual void get_property(string &prop_name, DbData &db);
/**
 * Get a list of device properties
 *
 * Get a list of properties for a device. The properties to get are specified as a vector of strings. Refer to
 * DbDevice::get_property() and DbData sections below for details on the DbData type.
 *
 * @param [in] prop_names The property names
 * @param [out] db The properties values
 * @throws NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database device
 */
	virtual void get_property(vector<string> &prop_names, DbData &db);
/**
 * Get property(ies) for a device
 *
 * Get property(ies) for a device. Properties to get are specified using the DbData type. Refer to DbDevice::
 * get_property() and DbData sections below for details.
 *
 * @param [in,out] db The property(ies) names and values
 * @throws NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database device
 */
	virtual void get_property(DbData &db);
/**
 * Put property(ies) for a device
 *
 * Put property(ies) for a device. Properties to put are specified using the DbData type. Refer to DbDevice::
 * put_property() and DbData sections below for details.
 *
 * @param [in] db The property(ies) names and values
 * @throws NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database device
 */
	virtual void put_property(DbData &db);
/**
 * Delete a single device property
 *
 * Delete a single property for a device. The property to delete is specified as a string.
 *
 * @param [in] prop_name The property name
 * @throws NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database device
 */
	virtual void delete_property(string &prop_name);
/**
 * Delete a list of device properties
 *
 * Delete a list of properties for a device. The properties to delete are specified as a vector of strings.
 *
 * @param [in] prop_names The property names list
 * @throws NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database device
 */
	virtual void delete_property(vector<string> &prop_names);
/**
 * Delete property(ies) for a device
 *
 * Delete property(ies) for a device. Properties to delete are specified using the DbData type. Refer to DbDevice::
 * get_property() and DbData sections below for details.
 *
 * @param [in] db The property names
 * @throws NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database device
 */
	virtual void delete_property(DbData &db);
/**
 * Get list of property names for a device
 *
 * Get the list of property names for the device. The parameter filter allows the user to filter the returned name
 * list. The wildcard character is ’*’. Only one wildcard character is allowed in the filter parameter. The name
 * list is returned in the vector of strings passed as the method second argument.
 *
 * @param [in] filter The filter
 * @param [out] prop_list The device property list
 * @throws NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database device
 */
	virtual void get_property_list(const string &filter,vector<string> &prop_list);
//@}

/** @name Logging related methods */
//@{
#ifdef TANGO_HAS_LOG4TANGO
/**
 * Add a logging target to the device
 *
 * Adds a new logging target to the device. The target_type_name input parameter must follow the
 * format: @b target_type::target_name. Supported target types are:
 * @li console
 * @li file
 * @li device
 *
 * For a device target,
 * the target_name part of the target_type_target_name parameter must contain the name of a log consumer
 * device (as defined in A.8). For a file target, target_name is the full path to the file to log to. If omitted, the
 * device’s name is used to build the file name (which is something like domain_family_member.log). Finally,
 * the target_name part of the target_type_target_name input parameter is ignored in case of a console target
 * and can be omitted.
 *
 * @param [in] target_type_name The target type and name
 * @throws DevFailed from device
 */
	virtual void add_logging_target(const string &target_type_name);
/**
 * Add a logging target to the device
 *
 * Adds a new logging target to the device. The target_type_name input parameter must follow the
 * format: @b target_type::target_name. Supported target types are:
 * @li console
 * @li file
 * @li device
 *
 * For a device target,
 * the target_name part of the target_type_target_name parameter must contain the name of a log consumer
 * device (as defined in A.8). For a file target, target_name is the full path to the file to log to. If omitted, the
 * device’s name is used to build the file name (which is something like domain_family_member.log). Finally,
 * the target_name part of the target_type_target_name input parameter is ignored in case of a console target
 * and can be omitted.
 *
 * @param [in] target_type_name The target type and name
 * @throws DevFailed from device
 */
	virtual void add_logging_target(const char *target_type_name)
			{add_logging_target(string(target_type_name));}
/**
 * Remove a logging target from the device
 *
 * Removes a logging target from the device’s target list. The target_type_name input parameter must
 * follow the format: target_type::target_name. Supported target types are:
 * @li console
 * @li file
 * @li device
 *
 * For a
 * device target, the target_name part of the target_type_target_name parameter must contain the name of a
 * log consumer device (as defined in ). For a file target, target_name is the full path to the file to remove. If
 * omitted, the default log file is removed. Finally, the target_name part of the target_type_target_name input
 * parameter is ignored in case of a console target and can be omitted.
 * If target_name is set to "*", all targets of the specified target_type are removed.
 *
 * @param [in] target_type_name The target type and name
 */
	virtual void remove_logging_target(const string &target_type_name);
/**
 * Remove a logging target from the device
 *
 * Removes a logging target from the device’s target list. The target_type_name input parameter must
 * follow the format: target_type::target_name. Supported target types are:
 * @li console
 * @li file
 * @li device
 *
 * For a
 * device target, the target_name part of the target_type_target_name parameter must contain the name of a
 * log consumer device (as defined in ). For a file target, target_name is the full path to the file to remove. If
 * omitted, the default log file is removed. Finally, the target_name part of the target_type_target_name input
 * parameter is ignored in case of a console target and can be omitted.
 * If target_name is set to "*", all targets of the specified target_type are removed.
 *
 * @param [in] target_type_name The target type and name
 */
	virtual void remove_logging_target(const char *target_type_name)
			{remove_logging_target(string(target_type_name));}
/**
 * Get current device's logging targets
 *
 * Returns a vector of string containing the current device’s logging targets. Each vector element has the
 * following format: target_type::target_name. An empty vector is returned is the device has no logging
 * targets.
 *
 * @return List of loggin target
 */
	virtual vector<string> get_logging_target (void);
/**
 * Get current device's logging level
 *
 * Returns the current device’s logging level (0=OFF, 1=FATAL, 2=ERROR, 3=WARNING, 4=INFO, 5=DEBUG).
 *
 * @return The device logging level
 */
	virtual int get_logging_level (void);
/**
 * Set the  device logging level
 *
 * Changes the device’s logging level. (0=OFF, 1=FATAL, 2=ERROR, 3=WARNING, 4=INFO, 5=DEBUG).
 *
 * @param [in] level The new device logging level
 */
	virtual void set_logging_level (int level);
#endif // TANGO_HAS_LOG4TANGO
//@}

/** @name Locking related methods */
//@{
/**
 * Lock a device
 *
 * Lock a device. The lock_validity is the time (in seconds) the lock is kept valid after the previous lock call.
 * A default value of 10 seconds is provided and should be fine in most cases. In case it is necessary to change
 * the lock validity, it’s not possible to ask for a validity less than a minimum value set to 2 seconds. The
 * library provided an automatic system to periodically re lock the device until an unlock call. No code is
 * needed to start/stop this automatic re-locking system. The locking system is re-entrant. It is then allowed
 * to call this method on a device already locked by the same process. The locking system has the following
 * features:
 * @li It is impossible to lock the database device or any device server process admin device
 * @li Destroying a locked DeviceProxy unlocks the device
 * @li Restarting a locked device keeps the lock
 * @li It is impossible to restart a device locked by someone else
 * @li Restarting a server breaks the lock
 *
 * A locked device is protected against the following calls when executed by another client:
 * @li command_inout call except for device state and status requested via command and for the set of
 * commands defined as allowed following the definition of allowed command in the Tango control
 * access schema.
 * @li write_attribute call
 * @li write_read_attribute call
 * @li set_attribute_config call
 *
 * @param [in] lock_validity The filter
 */
	virtual void lock(int lock_validity=DEFAULT_LOCK_VALIDITY);
/**
 * Unlock a device
 *
 * Unlock a device. If used, the method argument provides a back door on the locking system. If this argument
 * is set to true, the device will be unlocked even if the caller is not the locker. This feature is provided for
 * administration purpose and should be used very carefully. If this feature is used, the locker will receive a
 * DeviceUnlocked exception during the next call which is normally protected by the locking Tango system.
 *
 * @param [in] force The force unlock flag
 */
	virtual void unlock(bool force=false);
/**
 * Get device locking status
 *
 * This method returns a plain string describing the device locking status. This string can be:
 * @li "Device <device name> is not locked" in case the device is not locked
 * @li "Device <device name> is locked by CPP or Python client with PID <pid> from host <host name>"
 * in case the device is locked by a CPP client
 * @li "Device <device name> is locked by JAVA client class <main class> from host <host name>" in case
 * the device is locked by a JAVA client
 *
 * @return The device locking status
 */
	virtual string locking_status();
/**
 * Check if the device is locked
 *
 * Returns true if the device is locked. Otherwise, returns false
 *
 * @return The device locked flag
 */
	virtual bool is_locked();
/**
 * Check if the device is locked by the caller
 *
 * Returns true if the device is locked by the caller. Otherwise, returns false (device not locked or locked by
 * someone else)
 *
 * @return The device locked flag
 */
	virtual bool is_locked_by_me();
/**
 * Get device locking information
 *
 * If the device is locked, this method returns true an set some locker process informations in the structure
 * passed as argument. If the device is not locked, the method returns false. The LockerInfo structure definition
 * is
 * @code
 * typedef union
 * {
 *    pid_t LockerPid;
 *    unsigned long UUID[4];
 * }LockerId;
 *
 * enum LockerLanguage
 * {
 *    CPP,
 *    JAVA
 * };
 *
 * struct LockerInfo
 * {
 *    LockerLanguage ll;
 *    LockerId li;
 *    string locker_host;
 *    string locker_class;
 * };
 * @endcode
 *
 * @param [out] li Device locking information
 * @return The device locked flag
 */
	virtual bool get_locker(LockerInfo &li);
//@}

/// @privatesection

	virtual void parse_name(string &);
	virtual Database *get_device_db();

	DeviceProxy *get_adm_device() {return adm_device;}

//
// attribute methods
//

	void read_attribute(const char *,DeviceAttribute &);
	void read_attribute(string &at,DeviceAttribute &da) {read_attribute(at.c_str(),da);}

//
// Old event methods
//
	virtual int subscribe_event(const string &attr_name, EventType event, CallBack *,
	                   const vector<string> &filters);  // For compatibility with Tango < 8
	virtual int subscribe_event(const string &attr_name, EventType event, CallBack *,
	                   const vector<string> &filters, bool stateless); // For compatibility with Tango < 8
	virtual int subscribe_event(const string &attr_name, EventType event, int event_queue_size,
	                   const vector<string> &filters, bool stateless = false); // For compatibility with Tango < 8

};

#endif /* _DEVICEPROXY_H */
