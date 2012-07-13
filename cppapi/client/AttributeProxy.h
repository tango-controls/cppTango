//////////////////////////////////////////////////////////////////
//
// devapi.h - include file for TANGO device api
//
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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


#ifndef _ATTRIBUTEPROXY_H
#define _ATTRIBUTEPROXY_H


/****************************************************************************************
 * 																						*
 * 					The AttributeProxy class											*
 * 					--------------------												*
 * 																						*
 ***************************************************************************************/

/**
 * Easy to use interface to Tango device attribute
 *
 * The high level object which provides the client with an easy-to-use interface to TANGO device attributes.
 * AttributeProxy is a handle to the real attribute (hence the name Proxy) and is not the real attribute (of
 * course). The AttributeProxy manages timeouts, stateless connections (new AttributeProxy() nearly always
 * works), and reconnection if the device server is restarted.
 *
 * $Author: taurel $
 * $Revision: 1 $
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class AttributeProxy
{
private :
	string 				attr_name;
	string 				device_name;
	Tango::DeviceProxy 	*dev_proxy;
	Tango::DbAttribute 	*db_attr;
	bool    			dbase_used;		// Dev. with database
	bool    			from_env_var;   // DB from TANGO_HOST

	string  			host;           // DS host (if dbase_used=false)
	string  			port;           // DS port (if dbase_used=false)
	int     			port_num;       // DS port (as number)

	string  			db_host;        // DB host
	string  			db_port;        // DB port
	int     			db_port_num;    // DB port (as number)

	void real_constructor(string &);
	void ctor_from_dp(const DeviceProxy *,string &);

    class AttributeProxyExt
    {
    public:
        AttributeProxyExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<AttributeProxyExt>   ext;
#else
	AttributeProxyExt	            *ext;     		// Class extension
#endif

public :
///@name Constructors
//@{
/**
 * Create a AttributeProxy object.
 *
 * Create an AttributeProxy to an attribute of the specified name. The constructor will connect to the TANGO
 * database, query for the device to which the attribute belongs to network address and build a connection
 * to this device. If the device to which the attribute belongs to is defined in the TANGO database but the
 * device server is not running, AttributeProxy will try to build a connection every time the client tries to
 * access the attribute. If an alias name is defined for the attribute, this alias name can be used to create
 * the AttributeProxy instance. If a device name alias is defined for the device, it can be used instead of the
 * three fields device name. If the device to which the attribute belongs to is not defined in the database, an
 * exception is thrown. Examples :
 * @code
 * AttributeProxy *my_attr = new AttributeProxy("my/own/device/attr");
 * AttributeProxy *my_attr_bis = new AttributeProxy("attr_alias");
 * AttributeProxy *my_attr_ter = new AttributeProxy("dev_alias/attr");
 * @endcode
 *
 * @param [in] name	The attribute name
 */
	AttributeProxy(string &name);
/**
 * Create a AttributeProxy object.
 *
 * Idem previous constructor
 *
 * @param [in] name	The attribute name
 */
	AttributeProxy(const char *name);
//@}

///@name Miscellaneous methods
//@{
/**
 * Get attribute name
 *
 * Returns the attribute name
 *
 * @return The attribute name
 */
	virtual inline string name() { return attr_name; }
/**
 * Get associated DeviceProxy instance
 *
 * Returns the DeviceProxy instance used to communicate with the device to which the attributes belongs
 *
 * @return The underlying DeviceProxy object
 */
	virtual inline DeviceProxy* get_device_proxy() { return dev_proxy; }
/**
 * Get device status
 *
 * A method which return the status of the device to which the attribute belongs to. The status is returned as
 * a string. Example :
 * @code
 * cout << "device status: " << my_attr->status() << endl;
 * @endcode
 *
 * @return The underlying device status
 * @exception ConnectionFailed, CommunnicationFailed
 */
	virtual string status();
/**
 * Get device state
 *
 * A method which returns the state of the device to which the attribute belongs to. This state is returned as a
 * Tango::DevState type. Example :
 * @code
 * dev_state = my_attr->state() << endl;
 * @endcode
 *
 * @return The underlying device state
 * @exception ConnectionFailed, CommunnicationFailed
 */
	virtual DevState state();
/**
 * Ping the device
 *
 * A method which sends a ping to the device to which the attribute belongs and returns the time elapsed in
 * microseconds. Example :
 * @code
 * cout << "device ping took " << my_device->ping() << “ microseconds” << endl;
 * @endcode
 *
 * @return Time needed by the ping call
 * @exception ConnectionFailed, CommunnicationFailed
 */
	virtual int ping();
//@}

///@name Synchronous methods
//@{
/**
 * Get attribute configuration
 *
 * Return the attribute configuration
 *
 * @return The attribute configuration data
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	virtual AttributeInfoEx get_config();
/**
 * Set attribute configuration
 *
 * Change the attribute configuration.
 *
 * @param [in] ai The new attribute configuration data
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 * @deprecated Use the set_config() method with AttributeInfoEx parameter data type
 */
	virtual void set_config(AttributeInfo &ai);
/**
 * Set extended attribute configuration
 *
 * Change the attribute configuration.
 *
 * @param [in] ai The new extended attribute configuration data
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	virtual void set_config(AttributeInfoEx &ai);
/**
 * Read attribute value
 *
 * Read the attribute. To extract the value you have to use the operator of the class DeviceAttribute which
 * corresponds to the data type of the attribute. NOTE: There is no automatic type conversion from the
 * attribute native type to user type e.g. if an attribute returns a short you cannot extract it as a double (this
 * will return 0) you have to extract it as a short.
 *
 * @return The attribute value
 * @exception ConnectionFailed, CommunnicationFailed, DevFailed from device
 */
	virtual DeviceAttribute read();
/**
 * Write attribute value
 *
 * Write the attribute. To insert the value to write you have to use the operator of the class DeviceAttribute
 * which corresponds to the data type of the attribute. NOTE: There is no automatic type conversion from the
 * user type to the attribute native type e.g. if an attribute expects a short you cannot insert it as a double (this
 * will throw an exception) you have to insert it as a short.
 *
 * @param [in] da The new attribute value
 * @exception ConnectionFailed, CommunnicationFailed, DevUnlocked, DevFailed from device
 */
	virtual void write(DeviceAttribute &da);
/**
 * Write the Read attribute value
 *
 * Write then read a single attribute in a single network call. By default (serialisation by device), the execution
 * of this call in the server can’t be interrupted by other clients. To insert/extract the value to write/read you
 * have to use the operator of the class DeviceAttribute which corresponds to the data type of the attribute.
 * NOTE: There is no automatic type conversion from the user type to the attribute native type e.g. if an
 * attribute expects a short you cannot insert it as a double (this will throw an exception) you have to insert it
 * as a short.
 *
 * @param [in] da The new attribute value
 * @return The new attribute value
 * @exception ConnectionFailed, CommunnicationFailed, DevUnlocked, DevFailed from device
 */
	virtual DeviceAttribute write_read(DeviceAttribute &da);
/**
 * Get attribute history from polling buffer
 *
 * Retrieve attribute history from the attribute polling buffer. The argument is the wanted history depth. This
 * method returns a vector of DeviceAttributeHistory types. This method allocates memory for the vector of
 * DeviceAttributeHistory returned to the caller. It is the caller responsibility to delete this memory.
 * See Tango book chapter on Advanced Feature for all details regarding
 * polling.
 * @code
 * AttributeProxy attr = new AttributeProxy("my/own/device/Current");
 * vector<DeviceAttributeHistory> *hist;
 *
 * hist = attr->history(5);
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
 * @param [in] depth The required history depth
 * @return The attribute value history
 * @exception ConnectionFailed, CommunnicationFailed, NonSupportedFeature, DevFailed from device
 */
	virtual vector<DeviceAttributeHistory> *history(int depth);
//@}

///@name Asynchronous methods
//@{
/**
 * Read attribute value asynchronously in polling model
 *
 * Read the attribute asynchronously (polling model). This call returns an asynchronous call identifier which
 * is needed to get the attribute value.
 *
 * @return The asynchronous call identifier
 * @exception ConnectionFailed
 */
	virtual long read_asynch() {return dev_proxy->read_attribute_asynch(attr_name);}
/**
 * Get asynchronous read attribute call reply
 *
 * Check if the answer of an asynchronous read is arrived (polling model). id is the asynchronous call identifier.
 * If the reply is arrived and if it is a valid reply, it is returned to the caller in a DeviceAttribute object. If
 * the reply is an exception, it is re-thrown by this call. An exception is also thrown in case of the reply is not
 * yet arrived. To extract attribute value, you have to use the operator of the class DeviceAttribute which corresponds
 * to the data type of the attribute. NOTE: There is no automatic type conversion from the attribute
 * native type to user type e.g. if an attribute returns a short you cannot extract it as a double, you have to
 * extract it as a short. Memory has been allocated for the DeviceAttribute object returned to the caller. This
 * is the caller responsibility to delete this memory.
 *
 * @param [in] id The asynchronous identifier
 * @return The attribute value
 * @exception AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual DeviceAttribute *read_reply(long id) {return dev_proxy->read_attribute_reply(id);}
/**
 * Get asynchronous read attribute call reply with timeout
 *
 * Check if the answer of an asynchronous read is arrived (polling model). id is the asynchronous call identifier.
 * If the reply is arrived and if it is a valid reply, it is returned to the caller in a DeviceAttribute object.
 * If the reply is an exception, it is re-thrown by this call. If the reply is not yet arrived, the call will wait
 * (blocking the process) for the time specified in timeout. If after timeout milliseconds, the reply is still not
 * there, an exception is thrown. If timeout is set to 0, the call waits until the reply arrived. To extract attribute
 * value, you have to use the operator of the class DeviceAttribute which corresponds to the data type of the
 * attribute. NOTE: There is no automatic type conversion from the attribute native type to user type e.g. if an
 * attribute returns a short you cannot extract it as a double, you have to extract it as a short. Memory has been
 * allocated for the DeviceAttribute object returned to the caller. This is the caller responsibility to delete this
 * memory.
 *
 * @param [in] id The asynchronous identifier
 * @param [in] to The timeout value
 * @return The attribute value
 * @exception AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual DeviceAttribute *read_reply(long id,long to) {return dev_proxy->read_attribute_reply(id,to);}
/**
 * Write attribute value asynchrnously in polling model
 *
 * Write the attribute asynchronously (polling model). To insert the value to write you have to use the operator
 * of the class DeviceAttribute which corresponds to the data type of the attribute. NOTE: There is no
 * automatic type conversion from the user type to the attribute native type e.g. if an attribute expects a short
 * you cannot insert it as a double (this will throw an exception) you have to insert it as a short. This call
 * returns an asynchronous call identifier which is needed to get the server reply.
 *
 * @param [in] da The attribute value
 * @return The asynchrnous call identifier
 * @exception ConnectionFailed
 */
	virtual long write_asynch(DeviceAttribute &da) {return dev_proxy->write_attribute_asynch(da);}
/**
 * Get asynchronous write attribute call reply
 *
 * Check if the answer of an asynchronous write is arrived (polling model). id is the asynchronous call
 * identifier. If the reply is arrived and if it is a valid reply, the call returned. If the reply is an exception, it is
 * re-thrown by this call. An exception is also thrown in case of the reply is not yet arrived.
 *
 * @param [in] id The asynchronous identifier
 * @exception AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual void write_reply(long id) {dev_proxy->write_attribute_reply(id);}
/**
 * Get asynchronous write attribute call reply with timeout
 *
 * Check if the answer of an asynchronous write is arrived (polling model). id is the asynchronous call
 * identifier. If the reply is arrived and if it is a valid reply, the call returned. If the reply is an exception, it is
 * re-thrown by this call. If the reply is not yet arrived, the call will wait (blocking the process) for the time
 * specified in timeout. If after timeout milliseconds, the reply is still not there, an exception is thrown. If
 * timeout is set to 0, the call waits until the reply arrived.
 *
 * @param [in] id The asynchronous identifier
 * @param [in] to The timeout value
 * @exception AsynCall, AsynReplyNotArrived, CommunicationFailed, DevFailed from device
 */
	virtual void write_reply(long id,long to) {dev_proxy->write_attribute_reply(id,to);}
/**
 * Read attribute asynchronously in callback model
 *
 * Read the attribute asynchronously using the callback model. The argument is a reference to a callback
 * object. This callback object should be an instance of a user class inheriting from the Tango::CallBack class
 * with the attr_read() method overloaded.
 *
 * @param [in] cb The callback object
 * @exception ConnectionFailed
 */
	virtual void read_asynch(CallBack &cb) {dev_proxy->read_attribute_asynch(attr_name,cb);}
/**
 * Write attribute asynchronously in callback model
 *
 * Write the attribute asynchronously using the callback model. The argument is a reference to a callback
 * object. This callback object should be an instance of a user class inheriting from the Tango::CallBack class
 * with the attr_written() method overloaded.
 *
 * @param [in] da The new attribute value
 * @param [in] cb The callback object
 * @exception ConnectionFailed
 */
	virtual void write_asynch(DeviceAttribute &da,CallBack &cb) {dev_proxy->write_attribute_asynch(da,cb);}
//@}

///@name Polling related methods
//@{
/**
 * Check if the attribute is polled
 *
 * Returns true if the attribute is polled. Otherwise, returns false.
 *
 * @return Boolean true id the attribute is polled
 */
	virtual bool is_polled();
/**
 * Get attribute polling period
 *
 * Returns the attribute polling period in mS. If the attribute is not polled, it returns 0.
 *
 * @return The polling period
 */
	virtual int get_poll_period();
/**
 * Set attribute polling period
 *
 * Add the attribute to the list of polled attributes. The polling period is specified by "period" (in mS). If the
 * attribute is already polled, this method will update the polling period according to "period".
 *
 * @param [in] period The polling period
 */
	virtual void poll(int period);
/**
 * Stop attribute polling
 *
 * Remove attribute from the list of polled attributes.
 */
	virtual void stop_poll();
//@}

///@name Event related methods
//@{
/**
 * Subscribe to attribute event
 *
 * The client call to subscribe for event reception in the pushmodel. The client implements a callbackmethod
 * which is triggered when the event is received either by polling or a dedicated thread. Filtering is done based
 * on the reason specified and the event type. For example when reading the state and the reason specified is
 * "change" the event will be fired only when the state changes. Events consist of an attribute name and the
 * event reason. A standard set of reasons are implemented by the system, additional device specific reasons
 * an be implemented by device servers programmers.
 * cb is a pointer to a class inheriting fromthe Tango CallBack class and implementing a push_event() method.
 * The subscribe_event()
 * call returns an event id which has to be specified when unsubscribing from this event.
 *
 * @param [in] event The event type (reason)
 * @param [in] cb The event callback
 * @return The event identifier
 * @exception EventSystemFailed
 */
	virtual int subscribe_event (EventType event, CallBack *cb);
/**
 * Stateless subscription to attribute event
 *
 * This subscribe eventmethod has the same functionality as described in the last section. It adds an additional
 * flag called stateless. When the stateless flag is set to false, an exception will be thrown when the event
 * subscription encounters a problem.
 * With the stateless flag set to true, the event subscription will always succeed, even if the corresponding
 * device server is not running. The keep alive thread will try every 10 seconds to subscribe for the specified
 * event. At every subscription retry, a callback is executed which contains the corresponding exception.
 *
 * @param [in] event The event type (reason)
 * @param [in] cb The event callback
 * @param [in] stateless The stateless flag
 * @return The event identifier
 * @exception EventSystemFailed
 */
	virtual int subscribe_event (EventType event, CallBack *cb,bool stateless);
/**
 * Stateless subscription to attribute event with event queue
 *
 * The client call to subscribe for event reception in the pull model. Instead of a callback method the client
 * has to specify the size of the event reception buffer.
 * The event reception buffer is implemented as a round robin buffer. This way the client can set-up
 * different ways to receive events.
 * @li Event reception buffer size = 1 : The client is interested only in the value of the last event received. All
 * other events that have been received since the last reading are discarded.
 * @li Event reception buffer size > 1 : The client has chosen to keep an event history of a given size. When
 * more events arrive since the last reading, older events will be discarded.
 * @li Event reception buffer size = ALL_EVENTS : The client buffers all received events. The buffer size is
 * unlimited and only restricted by the available memory for the client.
 *
 * All other parameters are similar to the descriptions given in the last two sections.
 *
 * @param [in] event The event type (reason)
 * @param [in] event_queue_size The event queue size
 * @param [in] stateless The stateless flag
 * @return The event identifier
 * @exception EventSystemFailed
 */
	virtual int subscribe_event (EventType event, int event_queue_size, bool stateless = false);
/**
 * Unsubsribe to attribute event
 *
 * Unsubscribe a client from receiving the event specified by event_id. event_id is the event identifier returned
 * by the AttributeProxy::subscribe_event() method.
 *
 * @param [in] ev_id The event identifier
 * @exception EventSystemFailed
 */
	virtual void unsubscribe_event (int ev_id) {dev_proxy->unsubscribe_event(ev_id);}
/**
 * Get events from event queue (pull model)
 *
 * The method extracts all waiting events from the event reception buffer and executes the callback method
 * cb for every event. During event subscription the client must have chosen the pull model for this event.
 * event_id is the event identifier returned by the AttributeProxy::subscribe_event()method.
 *
 * @param [in] event_id The event identifier
 * @param [in] cb The event callback
 * @exception EventSystemFailed
 */
	virtual void get_events (int event_id, CallBack *cb)
	               {dev_proxy->get_events (event_id, cb);}
/**
 * Get events from event queue (pull model)
 *
 * The method extracts all waiting events from the event reception buffer. The returned event_list is a vector
 * of EventData pointers. The EventData object contains the event information as for the callback methods.
 * During event subscription the client must have chosen the pull model for this event. event_id is the
 * event identifier returned by the AttributeProxy::subscribe_event()method.
 *
 * @param [in] event_id The event identifier
 * @param [out] event_list The event list
 * @exception EventSystemFailed
 */
	virtual void get_events (int event_id, EventDataList &event_list)
	               {dev_proxy->get_events (event_id, event_list);}
/**
 * Get events from event queue (pull model)
 *
 * The method extracts all waiting attribute configuration events from the event reception buffer. The returned
 * event_list is a vector of AttrConfEventData pointers. The AttrConfEventData object contains the event
 * information as for the callback methods.
 * During event subscription the client must have chosen the pull model for this event. event_id is the
 * event identifier returned by the AttributeProxy::subscribe_event()method.
 *
 * @param [in] event_id The event identifier
 * @param [out] event_list The event list
 * @exception EventSystemFailed
 */
	virtual void get_events (int event_id, AttrConfEventDataList &event_list)
	               {dev_proxy->get_events (event_id, event_list);}
/**
 * Get events number in queue
 *
 * Returns the number of stored events in the event reception buffer. After every call to DeviceProxy:get_events(),
 * the event queue size is 0.
 * During event subscription the client must have chosen the pull model for this event. event_id is the
 * event identifier returned by the AttributeProxy::subscribe_event()method.
 *
 * @param [in] event_id The event identifier
 * @return The event number in the queue
 * @exception EventSystemFailed
 */
	virtual int  event_queue_size(int event_id)
	               {return dev_proxy->event_queue_size(event_id);}
/**
 * Get last event date
 *
 * Returns the arrival time of the last event stored in the event reception buffer. After every call to
 * AttributeProxy:get_events(), the event reception buffer is empty. In this case an exception will be returned.
 * During event subscription the client must have chosen the pull model for this event. event_id is the
 * event identifier returned by the AttributeProxy::subscribe_event()method.
 *
 * @param [in] event_id The event identifier
 * @return The last event date
 * @exception EventSystemFailed
 */
	virtual TimeVal get_last_event_date(int event_id)
	               {return dev_proxy->get_last_event_date(event_id);}
/**
 * Check if the event queue is empty
 *
 * Returns true when the event reception buffer is empty.
 * During event subscription the client must have chosen the pull model for this event. event_id is the
 * event identifier returned by the AttributeProxy::subscribe_event()method.
 *
 * @param [in] event_id The event identifier
 * @return The event queue empty flag
 * @exception EventSystemFailed
 */
	virtual bool is_event_queue_empty(int event_id)
	               {return dev_proxy->is_event_queue_empty(event_id);}
//@}

///@name Property related methods
//@{
/**
 * Get single attribute property
 *
 * Get a single property for the attribute. The property to get is specified as a string. Refer to DbDevice::
 * get_property() and DbData sections for details on the DbData type.
 *
 * @param [in] prop_name The property name
 * @param [out] db Property value
 * @exception NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database
 */
	virtual void get_property(string &prop_name, DbData &db);
/**
 * Get multiple attribute property
 *
 * Get a list of properties for the attribute. The properties to get are specified as a vector of strings. Refer to
 * DbDevice::get_property() and DbData sections for details on the DbData type.
 *
 * @param [in] prop_names The property names
 * @param [out] db Properties value
 * @exception NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database
 */
	virtual void get_property(vector<string> &prop_names, DbData &db);
/**
 * Get attribute property(ies)
 *
 * Get property(ies) for the attribute. Properties to get are specified using the DbData type. Refer to DbDevice::
 * get_property() and DbData sections for details.
 *
 * @param [in,out] db Properties value
 * @exception NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database
 */
	virtual void get_property(DbData &db);
/**
 * Put attribute property(ies)
 *
 * Put property(ies) for an attribute. Properties to put are specified using the DbData type. Refer to DbDevice::
 * put_property() and DbData sections for details.
 *
 * @param [in,out] db Properties value
 * @exception NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database
 */
	virtual void put_property(DbData &db);
/**
 * Delete a single attribute property
 *
 * Delete a single property for an attribute. The property to delete is specified as a string. Refer to DbDevice::
 * delete_property() and DbData sections for details on the DbData type.
 *
 * @param [in] prop_name The property name
 * @exception NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database
 */
	virtual void delete_property(string &prop_name);
/**
 * Delete a list of attribute property
 *
 * Delete a list of properties for an attribute. The properties to delete are specified as a vector of strings. Refer
 * to DbDevice::get_property() and DbData sections for details on the DbData type.
 *
 * @param [in] prop_names The properties name
 * @exception NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database
 */
	virtual void delete_property(vector<string> &prop_names);
/**
 * Delete attribute property(ies)
 *
 * Delete property(ies) for an attribute. Properties to delete are specified using the DbData type. Refer to
 * DbDevice::get_property() and DbData sections for details.
 *
 * @param [in] db The properties name
 * @exception NonDbDevice, ConnectionFailed, CommunicationFailed, DevFailed from database
 */
	virtual void delete_property(DbData &db);

//@}

/// @privatesection

//
// general methods
//
	virtual void parse_name(string &);
	virtual void set_transparency_reconnection(bool);
	virtual bool get_transparency_reconnection();

//
// Old event methods
//

	virtual int subscribe_event (EventType event, CallBack *,const vector<string> &filters); // For compatibility
	virtual int subscribe_event (EventType event, CallBack *,const vector<string> &filters, bool stateless); // For compatibility
	virtual int subscribe_event (EventType event, int event_queue_size,const vector<string> &filters, bool stateless = false); // For compatibility


	AttributeProxy(const DeviceProxy *,string &);
	AttributeProxy(const DeviceProxy *,const char *);
	AttributeProxy(const AttributeProxy &);
	AttributeProxy & operator=(const AttributeProxy &);
	virtual ~AttributeProxy();
};


#endif /* _ATTRIBUTEPROXY_H */
