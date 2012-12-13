//=============================================================================
//
// file :               group.h
//
// description :        Include for Tango Group impl.
//
// project :            TANGO
//
// author(s) :          N.Leclercq
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
//                      European Synchrotron Radiation Facility
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
// $Revision$
//
//=============================================================================

#ifndef _GROUP_H_
#define _GROUP_H_

#include <tango.h>

namespace Tango {

/** @defgroup Grp Group Client classes
 * @ingroup Client
 */
//=============================================================================
// THREAD SAFE IMPL OPTION
//=============================================================================
#define TANGO_GROUP_HAS_THREAD_SAFE_IMPL 1

//=============================================================================
// FORWARD DECLARATIONS
//=============================================================================
class Group;
class GroupElement;

//=============================================================================
// Misc. Typedefs
//=============================================================================
//- group content (individual devices and/or sub-groups)
typedef std::vector<GroupElement*> GroupElements;
//- group content iterator
typedef GroupElements::iterator GroupElementsIterator;
//-----------------------------------------------------------------------------
//- define what is a list of token (for name pattern management)
typedef std::vector<std::string> TokenList;
//=============================================================================
// class ExtRequestDesc : an asynch. request holder for groups
//-----------------------------------------------------------------------------
class AsynchRequest
{
  //- An asynch. request repository is maintain by each GroupDeviceElement
  friend class GroupDeviceElement;

  public:
    //- ctor
    AsynchRequest (long _rid, const std::string& _obj_name, bool ge_enabled = true)
      : rq_id(_rid), group_element_enabled_m(ge_enabled)
    {
      obj_names.push_back(_obj_name);
    };
    //- ctor
    AsynchRequest (long _rid, const std::vector<std::string>& _obj_names, bool ge_enabled = true)
      : rq_id(_rid), group_element_enabled_m(ge_enabled)
    {
      obj_names = _obj_names;
    };
    //- ctor
    AsynchRequest (long _rid, const std::string& _obj_name, const DevFailed& _df)
      : rq_id(_rid), rq_ex(_df), group_element_enabled_m(true)
    {
      obj_names.push_back(_obj_name);
    };
    //- ctor
    AsynchRequest (long _rid, const std::vector<std::string>& _obj_names, const DevFailed& _df)
      : rq_id(_rid), rq_ex(_df), group_element_enabled_m(true)
    {
      obj_names = _obj_names;
    };
    //- dtor
    virtual ~AsynchRequest () {
      //-noop impl
    };
    //- group_element_enabled accessor
    inline bool group_element_enabled () const {
      return group_element_enabled_m;
    }
  private:
    //- request ID
    long rq_id;
    //- name of requested objects (command or attribute)
    std::vector<std::string> obj_names;
    //- DevFailed containing potential error
    DevFailed rq_ex;
    //- true is the associated group member is enabled, false otherwise
    bool group_element_enabled_m;
};
//-----------------------------------------------------------------------------
//- asynch. request repository
typedef std::map<long, AsynchRequest> AsynchRequestRep;
//- asynch. request repository iterator
typedef AsynchRequestRep::iterator AsynchRequestRepIterator;
//- asynch. request repository value
typedef AsynchRequestRep::value_type AsynchRequestRepValue;
//=============================================================================

//=============================================================================
// class GroupReply : base class for group reply
//=============================================================================

/**
 * Base class for Group reply
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Grp
 */

class GroupReply
{
public:

///@privatesection
  //- default ctor
  GroupReply ();
  //- copy ctor
  GroupReply (const GroupReply& src);
  //- ctor
  GroupReply (const std::string& dev_name,
              const std::string& obj_name,
              bool group_element_enabled = true);
  //- ctor
  GroupReply (const std::string& dev_name,
              const std::string& obj_name,
              const DevFailed& exception,
              bool group_element_enabled = true);
  //- dtor
  virtual ~GroupReply ();
  //- group_element_enabled accessor
  inline bool group_element_enabled () const {
    return group_element_enabled_m;
  }
///@publicsection
   //- enable/disable exception - returns the previous mode
/**
 * Enable/Disable exception
 *
 * Set the group exception mode. If set to true, exception will be thrown
 * (when needed) by the library when the user get command  execution result.
 * If set to false (the default), the user has to deal with the has_failed()
 * exception to manage cases of wrong execution command.
 *
 * @param [in] exception_mode The new exception mode
 * @return The previous exception mode
 */
  static bool enable_exception (bool exception_mode = true);
  //- has_failed accessor
/**
 * Check if an error has occured
 *
 * Returns a boolean set to true if the command executed on the group
 * element has failed. Otherwise, returns false
 *
 * @return The error flag
 */
  inline bool has_failed () const {
    return has_failed_m;
  }
  //- device name accessor
/**
 * Get device name
 *
 * Returns the device name for the group element
 *
 * @return The device name
 */
  inline const std::string& dev_name () const {
    return dev_name_m;
  }
//- object (i.e. command or attribute) name accessor
/**
 * Get object name
 *
 * Returns the object name (i.e. command or attribute) for the group element
 *
 * @return The object name
 */
  inline const std::string& obj_name () const {
    return obj_name_m;
  }
  //- error stack accessor
/**
 * Get error stack
 *
 * Returns the error stack for the group element
 *
 * @return The error stack
 */
  inline const DevErrorList& get_err_stack () const {
    return exception_m.errors;
  }
protected:
///@privatesection
  //- exception flag (enable/disable)
  static bool exception_enabled;
  //- the device name
  std::string dev_name_m;
  //- command or attribute name
  std::string obj_name_m;
  //- has_failed_m: true is an error occurred, false otherwise
  bool has_failed_m;
  //- group_element_enabled_m : true is the associated group member is enabled, false otherwise
  bool group_element_enabled_m;
  //- exception: valid if has_failed_m set to true
  DevFailed exception_m;
};

//=============================================================================
// class GroupCmdReply : reply to command executed on a group
//=============================================================================
/**
 * Single element group reply for a command execution
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Grp
 */
class GroupCmdReply : public GroupReply
{
public:
///@privatesection
  //- default ctor
  GroupCmdReply ();
  //- copy ctor
  GroupCmdReply (const GroupCmdReply& src);
  //-
  GroupCmdReply (const std::string& dev_name,
                 const std::string& obj_name,
                 const DeviceData& data);
  //- ctor
  GroupCmdReply (const std::string& dev_name,
                 const std::string& obj_name,
                 const DevFailed& exception);
  //- ctor
  GroupCmdReply (const std::string& dev_name,
                 const std::string& obj_name,
                 bool group_element_enabled);
  //- dtor
  virtual ~GroupCmdReply ();
///@publicsection
  //- data accessor (may throw Tango::DevFailed)
/**
 * Get command data
 *
 * Get command result for a device member of a group hierarchy
 *
 * @return The command data
 */
  DeviceData& get_data ();
  //- template data exctractor method
/**
 * Get command data and extract them
 *
 * Get command result for a device member of a group hierarchy and extract
 * them in the provided variable
 *
 * @param [out] dest The variable in which the data should be extracted
 * @return Flag set to true if the extraction succeeds
 */
  template <typename T> bool operator>> (T& dest);
  //- data exctractor method for DevVarLongStringArray
/**
 * Get command data and extract them (DevVarLongStringArray)
 *
 * Get command result for a device member of a group hierarchy and extract
 * them in the provided variable in case the command returns a
 * data of the DevVarLongStringArray type
 *
 * @param [out] vl The array of DevLong part of the DevVarLongStringArray
 * @param [out] vs The array of string part of the DevVarLongStringArray data
 * @return Flag set to true if the extraction succeeds
 */
  bool extract (std::vector<DevLong>& vl, std::vector<std::string>& vs);
  //- data exctractor method for DevVarDoubleStringArray
/**
 * Get command data and extract them (DevVarDoubleStringArray)
 *
 * Get command result for a device member of a group hierarchy and extract
 * them in the provided variable in case the command returns a
 * data of the DevVarDoubleStringArray type
 *
 * @param [out] vd The array of DevDouble part of the DevVarDoubleStringArray
 * @param [out] vs The array of string part of the DevVarDoubleStringArray data
 * @return Flag set to true if the extraction succeeds
 */
  bool extract (std::vector<double>& vd, std::vector<std::string>& vs);
private:
  //- data: valid if GroupReply::has_failed_m set to false and
  //- GroupReply::enabled_m set to true
  DeviceData data_m;
};

//=============================================================================
// class GroupAttrReply : reply to an attribute reading executed on a group
//=============================================================================
/**
 * Single element group reply for a read attribute execution
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Grp
 */
class GroupAttrReply : public GroupReply
{
public:
///@privatesection
  //- ctor
  GroupAttrReply ();
  //- copy ctor
  GroupAttrReply (const GroupAttrReply& src);
  //- ctor
  GroupAttrReply (const std::string& dev_name,
                  const std::string& obj_name,
                  const DeviceAttribute& data);
  //- ctor
  GroupAttrReply (const std::string& dev_name,
                  const std::string& obj_name,
                  const DevFailed& exception);
  //- ctor
  GroupAttrReply (const std::string& dev_name,
                  const std::string& obj_name,
                  bool group_element_enabled);
  //- dtor
  virtual ~GroupAttrReply ();
///@publicsection
  //- data accessor (may throw Tango::DevFailed)
/**
 * Get attribute data
 *
 * Get attribute data for a device member of a group hierarchy
 *
 * @return The attribute data
 */
  DeviceAttribute& get_data ();
  //- template data exctractor method
/**
 * Get attribute data and extract them
 *
 * Get attribute data for a device member of a group hierarchy and extract
 * them in the provided variable
 *
 * @param [out] dest The variable in which the data should be extracted
 * @return Flag set to true if the extraction succeeds
 */
  template <typename T> bool operator>> (T& dest);
private:
  //- data: valid if GroupReply::has_failed_m set to false and
  //- GroupReply::enabled_m set to true
  DeviceAttribute data_m;
};

//=============================================================================
// class GroupReplyList : a simple list of GroupReply
//=============================================================================
/**
 * Group reply for a write_attribute execution
 *
 * This class inherits from @b vector<GroupReply> and therefore, each
 * device in the  group heierarchy has his own GroupReply object
 * which can be retrieved with the classical vector [] operator
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Grp
 */
class GroupReplyList : public std::vector<GroupReply>
{
///@privatesection
  typedef std::vector<GroupReply> Inherited;

  friend class Group;

public:
///@privatesection
  //- ctor
  GroupReplyList();
  //- dtor
  virtual ~GroupReplyList();
///@publicsection
  //- has_failed accessor
/**
 * Check if an error has occured
 *
 * Returns a boolean set to true if the write_attribute executed on the group has failed
 * for any device member of the hierarchy. Otherwise, returns false
 *
 * @return The error flag
 */
  bool has_failed () const {
    return has_failed_m;
  }
  //- reset the error list
/**
 * Reset the object
 *
 * This methods empty the inherited vector and reset the error flag
 */
  inline void reset () {
    clear();
    has_failed_m = false;
  };
///@privatesection
  //- push_back overload
  void push_back (const GroupReply& r) {
    if (r.has_failed()) {
      has_failed_m = true;
    }
    Inherited::push_back(r);
  }
private:
  //- has_failed_m: true if at least one error occurred, false otherwise
  bool has_failed_m;
};

//=============================================================================
// class GroupCmdReplyList : a simple list of GroupCmdReply
//=============================================================================
/**
 * Group reply for a command execution
 *
 * This class inherits from @b vector<GroupCmdReply> and therefore, each
 * device in the  group heierarchy has his own GroupCmdReply object
 * which can be retrieved with the classical vector [] operator
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Grp
 */

class GroupCmdReplyList : public std::vector<GroupCmdReply>
{
///@privatesection
  typedef std::vector<GroupCmdReply> Inherited;

  friend class Group;

public:
///@privatesection
  //- ctor
  GroupCmdReplyList();
  //- dtor
  virtual ~GroupCmdReplyList();
///@publicsection
  //- has_failed accessor method
/**
 * Check if an error has occured
 *
 * Returns a boolean set to true if the command executed on the group has failed
 * for any device member of the hierarchy. Otherwise, returns false
 *
 * @return The error flag
 */
  bool has_failed () const {
    return has_failed_m;
  }
  //- reset the error list
/**
 * Reset the object
 *
 * This methods empty the inherited vector and reset the error flag
 */
  inline void reset () {
    clear();
    has_failed_m = false;
  };
///@privatesection
  //- push_back overload
  void push_back (const GroupCmdReply& cr) {
    if (cr.has_failed()) {
      has_failed_m = true;
    }
    Inherited::push_back(cr);
  }
private:
  //- has_failed_m: true if at least one error occurred, false otherwise
  bool has_failed_m;
};

//=============================================================================
// class GroupAttrReplyList : a simple list of GroupAttrReply
//=============================================================================
/**
 * Group reply for a read_attribute execution
 *
 * This class inherits from @b vector<GroupAttrReply> and therefore, each
 * device in the  group heierarchy has his own GroupAttrReply object
 * which can be retrieved with the classical vector [] operator
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Grp
 */
class GroupAttrReplyList : public std::vector<GroupAttrReply>
{
///@privatesection
  typedef std::vector<GroupAttrReply> Inherited;

  friend class Group;

public:
///@privatesection
  //- ctor
  GroupAttrReplyList();
  //- dtor
  virtual ~GroupAttrReplyList();
///@publicsection
  //- has_failed accessor method
/**
 * Check if an error has occured
 *
 * Returns a boolean set to true if the read_attribute executed on the group has failed
 * for any device member of the hierarchy. Otherwise, returns false
 *
 * @return The error flag
 */
  bool has_failed () const {
    return has_failed_m;
  }
  //- reset the error list
/**
 * Reset the object
 *
 * This methods empty the inherited vector and reset the error flag
 */
  inline void reset () {
    clear();
    has_failed_m = false;
  };
///@privatesection
  //- push_back overload
  void push_back (const GroupAttrReply& ar) {
    if (ar.has_failed()) {
      has_failed_m = true;
    }
    Inherited::push_back(ar);
  }
private:
  //- has_failed_m: true if at least one error occurred, false otherwise
  bool has_failed_m;
};

//=============================================================================
// class GroupElementFactory : a GroupElement factory
//=============================================================================
class GroupElementFactory
{
  friend class Group;

  //- instanciatethe GroupElement which name matches the specified pattern with the specified timeout
  //- timeout = -1 => do not change the timeout
  static GroupElements instanciate (const std::string& p, int tmo = -1);

  static void parse_name (const std::string& p, std::string &db_host, int &db_port, std::string &dev_pattern);

  //- forbidden methods
  GroupElementFactory();
  ~GroupElementFactory();
  GroupElementFactory& operator=(const GroupElementFactory& other);
};

//=============================================================================
// class GroupElement: base class for all tango group element
//=============================================================================
class GroupElement
{
  friend class Group;

public:
  //---------------------------------------------
  //- Group management methods
  //---------------------------------------------
  //-
  virtual bool contains (const std::string& n, bool fwd = true);
  //-
  virtual DeviceProxy* get_device (const std::string& n) = 0;
  //-
  virtual DeviceProxy* get_device (long idx) = 0;
  //-
  virtual DeviceProxy* operator[] (long idx) = 0;

  //---------------------------------------------
  //- a la DeviceProxy interface
  //---------------------------------------------
  //-
  virtual bool ping (bool fwd = true) = 0;
  //-
  virtual void set_timeout_millis (int tmo_ms) = 0;

  //---------------------------------------------
  //- Misc.
  //---------------------------------------------
  //-
  inline const std::string& get_name () const {
    return name;
  };
  //-
  inline const std::string get_fully_qualified_name () const {
    if (parent) {
      return parent->get_fully_qualified_name() + "." + name;
    }
    return name;
  };
  //-
  inline void enable () {
    enabled = true;
  };
  //-
  inline void disable () {
    enabled = false;
  };
  //-
  inline bool is_enabled () const {
    return enabled;
  };
  bool name_equals (const std::string& n);
  //-
  bool name_matches (const std::string& n);
  //-
  virtual void dump (int indent_level = 0) = 0;
  //-
  virtual void dump (TangoSys_OMemStream& oms, int indent_level = 0) = 0;
  //-
  virtual bool is_connected();

protected:
  //- ctor: creates an GroupElement named <name>
  GroupElement (const std::string& name, GroupElement* parent = 0);
  //- dtor
  virtual ~GroupElement();

private:
  //- element name
  std::string name;
  //- parent element
  GroupElement* parent;
  //- enabled: true is this group member is enabled, false otherwise
  bool enabled;

  //- forbidden methods
  GroupElement ();
  GroupElement (const GroupElement&);
  GroupElement& operator=(const GroupElement&);

  //- element name tokenizer
  TokenList tokenize_i (const std::string& p);

  //- element name pattern matching
  bool match_i (const std::string& p, const TokenList& tokens);

  //- element identification
  virtual bool is_device_i () = 0;
  virtual bool is_group_i () = 0;

  //- group interface
  virtual long get_size_i (bool fwd = true) = 0;

  //- element searching
  virtual GroupElement* find_i (const std::string& n, bool fwd = true);

  //- private part of the asynch command exec. impl.
  virtual long command_inout_asynch_i (const std::string& c, bool fgt, bool fwd , long ari) = 0;
  virtual long command_inout_asynch_i (const std::string& c,  const DeviceData& d, bool fgt, bool fwd, long ari) = 0;
  virtual GroupCmdReplyList command_inout_reply_i (long req_id, long tmo_ms) = 0;

  //- private part of the asynch attribute(s) reading impl.
  virtual long read_attribute_asynch_i (const std::string& a, bool fwd, long ari) = 0;
  virtual GroupAttrReplyList read_attribute_reply_i (long req_id, long tmo_ms) = 0;
  virtual long read_attributes_asynch_i (const std::vector<std::string>& al, bool fwd, long ari) = 0;
  virtual GroupAttrReplyList read_attributes_reply_i (long req_id, long tmo_ms) = 0;

  //- private part of the asynch attribute writting impl.
  virtual long write_attribute_asynch_i (const DeviceAttribute& d, bool fwd, long ari) = 0;
  virtual GroupReplyList write_attribute_reply_i (long req_id, long tmo_ms) = 0;

  //- set the parent element, returns previous parent or 0 (null) if none
  GroupElement* set_parent (GroupElement* _parent);
};

//=============================================================================
// class Group: actual tango group implementation
//=============================================================================

/**
 * High level class allowing the user to handle Tango group
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Grp
 */

class Group : public GroupElement
{
  typedef std::map<long, bool> AsynchRequestDesc;
  typedef AsynchRequestDesc::iterator AsynchRequestDescIt;
  typedef AsynchRequestDesc::value_type AsynchRequestDescVal;

public:

  //---------------------------------------------
  //- Ctor & Dtor
  //---------------------------------------------
/**@name Constructor and destructor */
//@{
/**
 * Create a Group instance
 *
 * Instanciate an empty group. The group name allows retrieving a sub-group in the hierarchy.
 *
 * @param name The group name
 */
  Group (const std::string& name);
/**
 * Create a Group instance
 *
 * Delete a group and all its elements.
 * Be aware that a group always gets the ownership of its children and deletes themwhen it is itself deleted.
 * Therefore, never try to delete a Group (respectively a DeviceProxy) returned by a call to @e Tango::Group::get_group()
 * (respectively to @e Tango::Group::get_device()). Use the @e Tango::Group::remove() method instead.
 */
  virtual ~Group();
//@}

  //---------------------------------------------
  //- Group management methods
  //---------------------------------------------
/**@name Group management related methods */
//@{
/**
 * Attaches a (sub) group
 *
 *  Be aware that a group always gets the ownership of its children and deletes them when it is itself
 * deleted. Therefore, never try to delete a Group attached to a Group. Use the Group::remove() method
 * instead.
 * If timeout_ms parameter is different from -1, the client side timeout associated to each device composing
 * the group added is set to timeout_ms milliseconds. If timeout_ms is -1, timeouts are not changed.
 * This method does nothing if the specified group is already attached (i.e. it is silently ignored) and
 * timeout_ms = -1.
 * If the specified group is already attached and timeout_ms is different from -1, the client side timeout of
 * each device composing the group given in parameter is set to timeout_ms milliseconds.
 *
 * @param [in] group The group to be attached
 * @param [in] tmo_ms The timeout value
 */
  virtual void add (Group* group, int tmo_ms = -1);
/**
 * Attaches any device which name matches the specified pattern
 *
 * The pattern parameter can be a simple device name or a device name pattern (e.g. domain_* / family/
 * member_*).
 * This method first asks to the Tango database the list of device names matching the pattern. Devices are
 * then attached to the group in the order in which they are returned by the database.
 * Any device already present in the hierarchy (i.e. a device belonging to the group or to one of its
 * subgroups) is silently ignored but its client side timeout is set to timeout_ms milliseconds if timeout_ms is
 * different from -1.
 * Set the client side timeout of each device matching the specified pattern to timeout_ms milliseconds if
 * timeout_ms is different from -1.
 *
 * @param [in] pattern The device selection pattern
 * @param [in] tmo_ms The timeout value
 */
  virtual void add (const std::string& pattern, int tmo_ms = -1);
/**
 * Attaches any device which name matches one of the specified pattern
 *
 * The patterns parameter can be an array of device names and/or device name patterns.
 * Thismethod first asks to the Tango database the list of device namesmatching one the patterns. Devices
 * are then attached to the group in the order in which they are returned by the database.
 * Any device already present in the hierarchy (i.e. a device belonging to the group or to one of its
 * subgroups), is silently ignored but its client side timeout is set to timeout_ms milliseconds if timeout_ms is
 * different from -1.
 * If timeout_ms is different from -1, the client side timeouts of all devices matching the specified patterns
 * are set to timeout_ms milliseconds.
 *
 * @param [in] patterns The device selection pattern list
 * @param [in] tmo_ms The timeout value
 */
  virtual void add (const std::vector<std::string>& patterns, int tmo_ms = -1);
#ifdef GEN_DOC
/**
 * Removes any group or device which name matches the specified pattern.
 *
 * The pattern parameter can be a group name, a device name or a device name pattern (e.g domain_* /family/member_*).
 * Since we can have groups with the same name in the hierarchy, a group name can be fully qualified to
 * specify which group should be removed. Considering the following group:
 * @verbatim
  -> gauges
     | -> cell-01
     |     | -> penning
     |     |     | -> ...
     |     | -> pirani
     |     |     | -> ...
     | -> cell-02
     |     | -> penning
     |     |     | -> ...
     |     | -> pirani
     |     |     | -> ...
     | -> cell-03
     |     | -> ...

   @endverbatim
 * A call to gauges->remove("penning") will remove any group named "penning" in the hierarchy while
 * gauges->remove("gauges.cell-02.penning") will only remove the specified group.
 * If fwd is set to true (the default), the remove request is also forwarded to subgroups. Otherwise, it is
 * only applied to the local set of elements. For instance, the following code remove any stepper motor in the
 * hierarchy:
 * @code
 * root_group->remove("*/stepper_motor/*");
 * @endcode
 *
 * @param [in] pattern The device selection pattern
 * @param [in] fwd The forward flag
 */
#endif
  virtual void remove (const std::string& pattern, bool fwd = true);
/**
 * Removes any group or device which name matches any of the specified patterns.
 *
 * The patterns parameter can be an array of group names and/or device names and/or device name patterns.
 * Since we can have groups with the same name in the hierarchy, a group name can be fully qualified to
 * specify which group should be removed. See previous method for details.
 * If fwd is set to true (the default), the remove request is also forwarded to subgroups. Otherwise, it is
 * only applied to the local set of elements.
 *
 * @param [in] patterns The device selection patterns
 * @param [in] fwd The forward flag
 */
  virtual void remove (const std::vector<std::string>& patterns, bool fwd = true);
/**
 * Removes all elements in the group
 *
 * Removes all elements in the group. After such a call, the group is empty.
 */
  virtual void remove_all ();
/**
 * Check if the hierarchy contains groups and/or devices which name matches the specified pattern.
 *
 * Returns true if the hierarchy contains groups and/or devices which name matches the specified pattern.
 * Returns false otherwise.
 * The pattern can be a fully qualified or simple group name, a device name or a device name pattern.
 * If fwd is set to true (the default), the request is also forwarded to subgroups. Otherwise, it is only
 * applied to the local set of elements.
 *
 * @param [in] pattern The device selection pattern
 * @param [in] fwd The forward flag
 * @return True if the hierarchy contains the element
 */
  virtual bool contains (const std::string& pattern, bool fwd = true);
/**
 * Returns a reference to the specified device
 *
 * Returns a reference to the specified device or NULL if there is no device by that name in the group. This
 * method may throw an exception in case the specified device belongs to the group but can’t be reached (not
 * registered, down...). See example below. See also the Tango::DeviceProxy class documentation for details.
 * @code
 * try
 * {
 *    Tango::DeviceProxy *dp = g->get_device("my/device/01");
 *    if (dp == 0)
 *    {
 *       // my/device/01 does not belongs to the group
 *    }
 * }
 * catch (const Tango::DevFailed &df)
 * {
 *    // my/device/01 belongs to the group but can’t be reached
 * }
 * @endcode
 * The request is systematically forwarded to subgroups (i.e. if no device named device_name could be
 * found in the local set of devices, the request is forwarded to subgroups).
 * Be aware that a group always gets the ownership of its children and deletes them when it is itself
 * deleted. Therefore, never try to delete a DeviceProxy returned by the Group::get_device()method. Use the
 * Tango::Group::remove() method instead.
 *
 * @param [in] device_name The device name
 * @return True if the hierarchy contains the element
 */
  virtual DeviceProxy* get_device (const std::string& device_name);
/**
 * Returns a reference to the "idx-th" device in the hierarchy
 *
 * Returns a reference to the "idx-th" device in the hierarchy or NULL if the hierarchy contains less than
 * "idx" devices. This method may throw an exception in case the specified device belongs to the group but
 * can’t be reached (not registered, down...). See previous example. See also the Tango::DeviceProxy class
 * documentation for details.
 * The request is systematically forwarded to subgroups (i.e. if the local set of devices contains less than
 * "idx" devices, the request is forwarded to subgroups).
 * Be aware that a group always gets the ownership of its children and deletes them when it is itself
 * deleted. Therefore, never try to delete a DeviceProxy returned by the Group::get_device()method. Use the
 * Tango::Group::remove() method instead.
 *
 * @param [in] idx The device name
 * @return device reference
 */
  virtual DeviceProxy* get_device (long idx);
/**
 * Returns a reference to the "idx-th" device in the hierarchy
 *
 * Returns a reference to the "idx-th" device in the hierarchy or NULL if the hierarchy contains less than "idx"
 * devices. See the Tango::DeviceProxy class documentation for details.
 * The request is systematically forwarded to subgroups (i.e. if the local set of devices contains less than
 * "idx" devices, the request is forwarded to subgroups).
 * Be aware that a group always gets the ownership of its children and deletes them when it is itself
 * deleted. Therefore, never try to delete a DeviceProxy returned by the Group::get_device()method. Use the
 * Tango::Group::remove() method instead.
 *
 * @param [in] idx The device name
 * @return device reference
 */
  virtual DeviceProxy* operator[] (long idx);
/**
 * Returns a reference to the specified group
 *
 * Returns a reference to the specified group or NULL if there is no group by that name. The group_name
 * can be a fully qualified name.
 * Considering the following group:
 * @verbatim
  -> gauges
     | -> cell-01
     |     | -> penning
     |     |     | -> ...
     |     | -> pirani
     |     |     | -> ...
     | -> cell-02
     |     | -> penning
     |     |     | -> ...
     |     | -> pirani
     |     |     | -> ...
     | -> cell-03
     |     | -> ...

   @endverbatim
 * A call to gauges->get_group("penning") returns the first group named "penning" in the hierarchy (i.e.
 * gauges.cell-01.penning)while gauges->get_group("gauges.cell-02.penning”) returns the specified group.
 * The request is systematically forwarded to subgroups (i.e. if no group named group_name could be
 * found in the local set of elements, the request is forwarded to subgroups).
 * Be aware that a group always gets the ownership of its children and deletes them when it is itself
 * deleted. Therefore, never try to delete a Group returned by the Group::get_group() method. Use the
 * Tango::Group::remove() method instead.
 *
 * @param [in] group_name The group name
 * @return group pointer
 */
  virtual Group* get_group (const std::string& group_name);
/**
 * Return the number of devices in the hierarchy
 *
 * Return the number of devices in the hierarchy (respectively the number of device in the group) if the
 * forward option is set to true (respectively set to false)
 *
 * @param [in] fwd The forward flag
 * @return group size
 */
  long get_size (bool fwd = true);
/**
 * Returns the list of devices currently in the hierarchy.
 *
 * Returns the list of devices currently in the hierarchy.
 * If fwd is set to true (the default) the request is forwarded to subgroups. Otherwise, it is only applied to
 * the local set of devices.
 * Considering the following hierarchy:
 * @code
 * g2->add("my/device/04"); g2->add("my/device/05");
 *
 * g4->add("my/device/08"); g4->add("my/device/09");
 *
 * g3->add("my/device/06");
 * g3->addg(g4);
 * g3->add("my/device/07");
 *
 * g1->add("my/device/01");
 * g1->add(g2);
 * g1->add("my/device/03");
 * g1->add(g3);
 * g1->add("my/device/02");
 * @endcode
 * The returned vector content depends on the value of the forward option. If set to true, the results will be
 * organized as follows:
 * @code
 * std::vector<std::string> dl = g1->get_device_list(true);
 * @endcode
 * dl[0] contains "my/device/01" which belongs to g1
 * @n dl[1] contains "my/device/04" which belongs to g1.g2
 * @n dl[2] contains "my/device/05" which belongs to g1.g2
 * @n dl[3] contains "my/device/03" which belongs to g1
 * @n dl[4] contains "my/device/06" which belongs to g1.g3
 * @n dl[5] contains "my/device/08" which belongs to g1.g3.g4
 * @n dl[6] contains "my/device/09" which belongs to g1.g3.g4
 * @n dl[7] contains "my/device/07" which belongs to g1.g3
 * @n dl[8] contains "my/device/02" which belongs to g1
 * @n @n If the forward option is set to false, the results are:
 * @code
 * std::vector<std::string> dl = g1->get_device_list(false);
 * @endcode
 * dl[0] contains "my/device/01" which belongs to g1
 * @n dl[1] contains "my/device/03" which belongs to g1
 * @n dl[2] contains "my/device/02" which belongs to g1
 *
 * @param [in] fwd The forward flag
 * @return group size
 */
  std::vector<std::string> get_device_list (bool fwd = true);
  //-

//@}
///@privatesection
  //-
  virtual Group* get_parent () const;
  //-
  void enable (const std::string& device_name, bool fwd = true);
  //-
  void disable (const std::string& device_name, bool fwd = true);
  //-
  bool is_enabled (const std::string& device_name, bool fwd = true) {
    GroupElement * ge = this->find_i(device_name, fwd);
    return ge ? ge->is_enabled() : false;
  };
  //-
  bool is_root_group () const;

  //---------------------------------------------
  //- a la DeviceProxy interface
  //---------------------------------------------
///@publicsection
///@name a la Deviceproxy interface
//@{
  //- misc.
  //---------------------------------------------
/**
 * Ping all devices in a group
 *
 * Ping all devices in a group. This method returns true if all devices in the group are alive, false otherwise.
 * If fwd is set to true (the default), the request is also forwarded to subgroups. Otherwise, it is only
 * applied to the local set of devices.
 *
 * @param [in] fwd The forward flag
 * @return True if all devices are alive
 */
  virtual bool ping (bool fwd = true);
/**
 * Set client side timeout for all devices in the group
 *
 * Set client side timeout for all devices composing the group in milliseconds. Any method which takes longer
 * than this time to execute will throw an exception.
 *
 * @param [in] tmo_ms The timeout value
 */
  virtual void set_timeout_millis (int tmo_ms);

  //- command execution
  //---------------------------------------------
/**
 * Executes a Tango command on a group
 *
 * Executes a Tango command on a group. This method is synchronous and does not return until replies are
 * obtained or timeouts occurred.
 * The parameter c is the name of the command.
 * If fwd is set to true (the default), the request is also forwarded to subgroups. Otherwise, it is only
 * applied to the local set of devices.
 * Command results are returned in a GroupCmdReplyList. See Obtaining command result for details
 * (Chapter 4.7.3.1 in <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>). See also Case 1 of executing a command
 * (Chapter 4.7.3.2 in <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] c The command name
 * @param [in] fwd The forward flag
 * @return The group command result
 */
  GroupCmdReplyList command_inout (const std::string& c, bool fwd = true);
/**
 * Executes a Tango command with the same input data on a group
 *
 * Executes a Tango command on each device in the group. This method is synchronous and does not return
 * until replies are obtained or timeouts occurred.
 * The parameter c is the name of the command.
 * The second parameter d is a Tango generic container for command carrying the command argument.
 * See the Tango::DeviceData documentation.
 * If fwd is set to true (the default), the request is also forwarded to subgroups. Otherwise, it is only
 * applied to the local set of devices.
 * Command results are returned in a GroupCmdReplyList. See Obtaining command results
 * (Chapter 4.7.3.1 in <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for
 * details. See also Case 2 of executing a command (Chapter 4.7.3.4 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] c The command name
 * @param [in] d The command data
 * @param [in] fwd The forward flag
 * @return The group command result
 */
  GroupCmdReplyList command_inout (const std::string& c, const DeviceData& d, bool fwd = true);
/**
 * Executes a Tango command with the different input data on a group (Using DeviceData class instances)
 *
 * Executes a Tango command on each device in the group. This method is synchronous and does not return
 * until replies are obtained or timeouts occurred.
 * This implementation of command_inout allows passing a specific input argument to each device in
 * the group. In order to use this form of command_inout, the user must have an "a priori" and "perfect"
 * knowledge of the devices order in the group.
 * @n The parameter c is the name of the command.
 * The std::vector d contains a specific argument value for each device in the group. Its size must equal Group::get_size(fwd).
 * Otherwise, an exception is thrown. The order of the argument values must follows the order of the devices
 * in the group (d[0] => 1st device, d[1] => 2nd device and so on).
 * If fwd is set to true (the default), the request is also forwarded to subgroups. Otherwise, it is only
 * applied to the local set of devices.
 * Command results are returned in a GroupCmdReplyList. See Obtaining command results (Chpater 4.7.3.1 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>)
 * for details. See also Case 3 of executing a command (Chapter 4.7.3.5 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example of this special form of
 * command_inout.
 *
 * @param [in] c The command name
 * @param [in] d The command data
 * @param [in] fwd The forward flag
 * @return The group command result
 */
  GroupCmdReplyList command_inout (const std::string& c, const std::vector<DeviceData>& d, bool fwd = true);
/**
 * Executes a Tango command with the different input data on a group
 *
 * Executes a Tango command on each device in the group. This method is synchronous and does not return
 * until replies are obtained or timeouts occurred.
 * This implementation of command_inout allows passing a specific input argument to each device in
 * the group. In order to use this form of command_inout, the user must have an "a priori" and "perfect"
 * knowledge of the devices order in the group.
 * @n The parameter c is the name of the command.
 * The std::vector d contains a specific argument value for each device in the group. Since this method is a
 * template, d is able to contain any Tango command argument type. Its size must equal Group::get_size(fwd).
 * Otherwise, an exception is thrown. The order of the argument values must follows the order of the devices
 * in the group (d[0] => 1st device, d[1] => 2nd device and so on).
 * If fwd is set to true (the default), the request is also forwarded to subgroups. Otherwise, it is only
 * applied to the local set of devices.
 * Command results are returned in a GroupCmdReplyList. See Obtaining command results (Chpater 4.7.3.1 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>)
 * for details. See also Case 3 of executing a command (Chapter 4.7.3.5 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example of this special form of
 * command_inout.
 *
 * @tparam T The command input data type
 * @param [in] c The command name
 * @param [in] d The command data
 * @param [in] fwd The forward flag
 * @return The group command result
 */
  template<typename T> GroupCmdReplyList command_inout (const std::string& c, const std::vector<T>& d, bool fwd = true);
/**
 * Executes a Tango command on each device in the group asynchronously.
 *
 * Executes a Tango command on each device in the group asynchronously. The method sends the request
 * to all devices and returns immediately. Pass the returned request id to Group::command_inout_reply() to
 * obtain the results.
 * The parameter c is the name of the command.
 * The parameter fgt is a fire and forget flag. If set to true, it means that no reply is expected (i.e. the caller
 * does not care about it and will not even try to get it). A false default value is provided.
 * If the parameter fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only
 * applied to the local set of devices.
 * See Case 1 of Executing a command (Chapter 4.7.3.2 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] c The command name
 * @param [in] fgt The command data
 * @param [in] fwd The forward flag
 * @return The call identifier
 */
  long command_inout_asynch (const std::string& c, bool fgt = false, bool fwd = true);
/**
 * Executes a Tango command with same input data on each device in the group asynchronously.
 *
 * Executes a Tango command on each device in the group asynchronously. The method sends the request
 * to all devices and returns immediately. Pass the returned request id to Group::command_inout_reply() to
 * obtain the results.
 * The parameter c is the name of the command.
 * The second parameter d is a Tango generic container for command carrying the command argument.
 * See the Tango::DeviceData documentation for details.
 * The parameter fgt is a fire and forget flag. If set to true, it means that no reply is expected (i.e. the caller
 * does not care about it and will not even try to get it). A false default value is provided.
 * If the parameter fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only
 * applied to the local set of devices.
 * See Case 2 of Executing a command (Chapter 4.7.3.4 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] c The command name
 * @param [in] d The command input data
 * @param [in] fgt The command data
 * @param [in] fwd The forward flag
 * @return The call identifier
 */
  long command_inout_asynch (const std::string& c, const DeviceData& d, bool fgt = false, bool fwd = true);
/**
 * Executes a Tango command with different input data (using DeviceData object) on each device in the group asynchronously.
 *
 * Executes a Tango command on each device in the group asynchronously. The method send the request to
 * all devices and return immediately. Pass the returned request id to Group::command_inout_reply to obtain
 * the results.
 * This implementation of command_inout allows passing a specific input argument to each device in the
 * group. In order to use this form of command_inout_asynch, the user must have an "a priori" and "perfect"
 * knowledge of the devices order in the group.
 * The parameter c is the name of the command.
 * The std::vector d contains a specific argument value for each device in the group. Its size must equal Group::get_size(fwd). Otherwise, an exception is
 * thrown. The order of the argument values must follows the order of the devices in the group (d[0] => 1st
 * device, d[1] => 2nd device and so on).
 * The parameter fgt is a fire and forget flag. If set to true, it means that no reply is expected (i.e. the caller
 * does not care about it and will not even try to get it). A false default value is provided.
 * If fwd is set to true (the default), the request is also forwarded to subgroups. Otherwise, it is only
 * applied to the local set of devices.
 * See Case 3 of Executing a command (Chapter 4.7.3.5 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example of this special form of command_inout.
 *
 * @param [in] c The command name
 * @param [in] d The command input data
 * @param [in] fgt The command data
 * @param [in] fwd The forward flag
 * @return The call identifier
 */
  long command_inout_asynch (const std::string& c, const std::vector<DeviceData>& d, bool fgt = false, bool fwd = true);
/**
 * Executes a Tango command with different input data on each device in the group asynchronously.
 *
 * Executes a Tango command on each device in the group asynchronously. The method send the request to
 * all devices and return immediately. Pass the returned request id to Group::command_inout_reply to obtain
 * the results.
 * This implementation of command_inout allows passing a specific input argument to each device in the
 * group. In order to use this form of command_inout_asynch, the user must have an "a priori" and "perfect"
 * knowledge of the devices order in the group.
 * The parameter c is the name of the command.
 * The std::vector d contains a specific argument value for each device in the group. Since it's a template data type, d is able to contain
 * any Tango command argument type. Its size must equal Group::get_size(fwd). Otherwise, an exception is
 * thrown. The order of the argument values must follows the order of the devices in the group (d[0] => 1st
 * device, d[1] => 2nd device and so on).
 * The parameter fgt is a fire and forget flag. If set to true, it means that no reply is expected (i.e. the caller
 * does not care about it and will not even try to get it). A false default value is provided.
 * If fwd is set to true (the default), the request is also forwarded to subgroups. Otherwise, it is only
 * applied to the local set of devices.
 * See Case 3 of Executing a command (Chapter 4.7.3.5 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example of this special form of command_inout.
 *
 * @tparam T The command input data type
 * @param [in] c The command name
 * @param [in] d The command input data
 * @param [in] fgt The command data
 * @param [in] fwd The forward flag
 * @return The call identifier
 */
  template<typename T> long command_inout_asynch (const std::string& c, const std::vector<T>& d, bool fgt = false, bool fwd = true);
/**
 * Returns the results of an asynchronous command.
 *
 * Returns the results of an asynchronous command.
 * The first parameter req_id is a request identifier previously returned by one of the command_inout_asynch
 * methods.
 * For each device in the hierarchy, if the command result is not yet available, command_inout_replywait
 * timeout_ms milliseconds before throwing an exception. This exception will be part of the global reply. If
 * timeout_ms is set to 0, command_inout_reply waits "indefinitely".
 * Command results are returned in a GroupCmdReplyList. See Obtaining command results (Chapter 4.7.3.1 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for
 * details.
 *
 * @param [in] req_id The request identifier
 * @param [in] tmo_ms The timeout value
 * @return The group command result
 */
  GroupCmdReplyList command_inout_reply (long req_id, long tmo_ms = 0);

  //- attribute(s) reading
  //---------------------------------------------
/**
 * Reads an attribute on each device in the group
 *
 * Reads an attribute on each device in the group. This method is synchronous and does not return until replies
 * are obtained or timeouts occurred.
 * The parameter a is the name of the attribute to read.
 * If fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only applied to the
 * local set of devices.
 * Attribute values are returned in a GroupAttrReplyList. See Obtaining attribute values (Chapter 4.7.4.1 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for
 * details. See also Reading an attribute (Chapter 4.7.4 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] a The attribute name
 * @param [in] fwd The forward flag
 * @return The group attribute data
 */
  GroupAttrReplyList read_attribute (const std::string& a, bool fwd = true);
/**
 * Reads several attributes on each device in the group
 *
 * Reads several attributes on each device in the group. This method is synchronous and does not return until replies
 * are obtained or timeouts occurred.
 * The parameter al is the list of attributes to be read.
 * If fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only applied to the
 * local set of devices.
 * Attribute values are returned in a GroupAttrReplyList. See Obtaining attribute values (Chapter 4.7.4.1 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for
 * details. See also Reading an attribute (Chapter 4.7.4 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] al The attribute name list
 * @param [in] fwd The forward flag
 * @return The group attribute data
 */
  GroupAttrReplyList read_attributes (const std::vector<std::string>& al, bool fwd = true);
/**
 * Reads an attribute on each device in the group asynchronously
 *
 * Reads an attribute on each device in the group asynchronously. The method sends the request to all devices
 * and returns immediately. Pass the returned request id to Group::read_attribute_reply() to obtain the results.
 * The parameter a is the name of the attribute to read.
 * If fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only applied to the
 * local set of devices.
 * The last parameter (rsv) is reserved for internal purpose and should not be modify. It may disappear in
 * a near future.
 * See Reading an attribute (Chapter 4.7.4 in <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] a The attribute name
 * @param [in] fwd The forward flag
 * @return The call identifier
 */
  long read_attribute_asynch (const std::string& a, bool fwd = true);
/**
 * Reads several attributes on each device in the group asynchronously
 *
 * Reads several attribute on each device in the group asynchronously. The method sends the request to all devices
 * and returns immediately. Pass the returned request id to Group::read_attribute_reply() to obtain the results.
 * The parameter a is the name of the attribute to read.
 * If fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only applied to the
 * local set of devices.
 * The last parameter (rsv) is reserved for internal purpose and should not be modify. It may disappear in
 * a near future.
 * See Reading an attribute (Chapter 4.7.4 in <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] al The attribute name list
 * @param [in] fwd The forward flag
 * @return The call identifier
 */
  long read_attributes_asynch (const std::vector<std::string>& al, bool fwd = true);
/**
 * Returns the results of an asynchronous attribute reading
 *
 * Returns the results of an asynchronous attribute reading.
 * The first parameter req_id is a request identifier previously returned by read_attribute_asynch.
 * For each device in the hierarchy, if the attribute value is not yet available, read_attribute_reply wait
 * timeout_ms milliseconds before throwing an exception. This exception will be part of the global reply. If
 * timeout_ms is set to 0, read_attribute_reply waits "indefinitely".
 * Replies are returned in a GroupAttrReplyList. See Obtaining attribute values (Chapter 4.7.4.1 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for details
 *
 * @param [in] req_id The attribute name list
 * @param [in] tmo_ms The timeout value
 * @return The group attribute data
 */
  GroupAttrReplyList read_attribute_reply (long req_id, long tmo_ms = 0);
/**
 * Returns the results of an asynchronous attributes reading
 *
 * Returns the results of an asynchronous attributes reading.
 * The first parameter req_id is a request identifier previously returned by read_attribute_asynch.
 * For each device in the hierarchy, if the attribute value is not yet available, read_attribute_reply wait
 * timeout_ms milliseconds before throwing an exception. This exception will be part of the global reply. If
 * timeout_ms is set to 0, read_attribute_reply waits "indefinitely".
 * Replies are returned in a GroupAttrReplyList. See Obtaining attribute values (Chapter 4.7.4.1 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for details
 *
 * @param [in] req_id The attribute name list
 * @param [in] tmo_ms The timeout value
 * @return The group attribute data
 */
  GroupAttrReplyList read_attributes_reply (long req_id, long tmo_ms = 0);

  //- attribute writting
  //---------------------------------------------
/**
 * Writes an attribute on each device in the group
 *
 * Writes an attribute on each device in the group. This method is synchronous and does not return until
 * acknowledgements are obtained or timeouts occurred.
 * The first parameter d is a Tango generic container for attribute carrying both the attribute name and the
 * value. See the Tango::DeviceAttribute documentation for details.
 * If fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only applied to the
 * local set of devices.
 * Acknowledgements are returned in a GroupReplyList. See Obtaining acknowledgements (Chapter 4.7.5.1 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for
 * details. See also Case 1 of Writing an attribute (Chapter 4.7.5.2 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] d The attribute name and value
 * @param [in] fwd The forward flag
 * @return The group reply
 */
  GroupReplyList write_attribute (const DeviceAttribute& d, bool fwd = true);
/**
 * Writes several attributes on each device in the group (using DeviceAttribute)
 *
 * Writes several attributes on each device in the group. This method is synchronous and does not return until
 * acknowledgements are obtained or timeouts occurred.
 * The first parameter d is a vector of Tango generic container for attribute carrying both the attribute name and the
 * value. See the Tango::DeviceAttribute documentation for details.
 * If fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only applied to the
 * local set of devices.
 * Acknowledgements are returned in a GroupReplyList. See Obtaining acknowledgements (Chapter 4.7.5.1 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for
 * details. See also Case 2 of Writing an attribute (Chapter 4.7.5.3 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] d The attribute names and values
 * @param [in] fwd The forward flag
 * @return The group reply
 */
  GroupReplyList write_attribute (const std::vector<DeviceAttribute>& d, bool fwd = true);
/**
 * Writes one attributes on each device in the group with specific value per device
 *
 * Writes an attribute on each device in the group. This method is synchronous and does not return until
 * replies are obtained or timeouts occurred.
 * This implementation of write_attribute allows writing a specific value to each device in the group. In
 * order to use this form of write_attribute, the user must have an "a priori" and "perfect" knowledge of the
 * devices order in the group.
 * The parameter a is the name of the attribute.
 * The std::vector d contains a specific value for each device in the group. Since this method is a template,
 * d is able to contain any Tango attribute type. Its size must equal Group::get_size(fwd). Otherwise, an
 * exception is thrown. The order of the attribute values must follows the order of the devices in the group
 * (d[0] => 1st device, d[1] => 2nd device and so on).
 * If fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only applied to the
 * local set of devices.
 * Acknowledgements are returned in a GroupReplyList. See Obtaining acknowledgements (Chapter 4.7.5.1 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for
 * details. See also Case 2 of Writing an attribute (Chapter 4.7.5.3 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @tparam T The attribute data type
 * @param [in] n The attribute name
 * @param [in] d The attribute names and values
 * @param [in] fwd The forward flag
 * @return The group reply
 */
  template<typename T> GroupReplyList write_attribute (const std::string& n, const std::vector<T>& d, bool fwd = true);
/**
 * Writes an attribute on each device in the group asynchronously.
 *
 * Write an attribute on each device in  the group asynchronously. The method sends the request to all
 * devices and returns immediately. Pass the returned request id to Group::write_attribute_reply() to obtain
 * the acknowledgements.
 * The first parameter d is a Tango generic container for attribute carrying both the attribute name and the
 * value. See the Tango::DeviceAttribute documentation for details.
 * If fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only applied to the
 * local set of devices.
 * See Case 1 of Writing an attribute (Chapter 4.7.5.2 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] d The attribute name and value
 * @param [in] fwd The forward flag
 * @return The call identifier
 */
  long write_attribute_asynch (const DeviceAttribute& d, bool fwd = true);
/**
 * Writes several attributes on each device in the group asynchronously.
 *
 * Write several attributes on each device in  the group asynchronously. The method sends the request to all
 * devices and returns immediately. Pass the returned request id to Group::write_attribute_reply() to obtain
 * the acknowledgements.
 * The first parameter d is a vector of Tango generic container for attribute carrying both the attribute name and the
 * value. See the Tango::DeviceAttribute documentation for details.
 * If fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only applied to the
 * local set of devices.
 * See Case 1 of Writing an attribute (Chapter 4.7.5.2 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @param [in] d The attribute name and value
 * @param [in] fwd The forward flag
 * @return The call identifier
 */
  long write_attribute_asynch (const std::vector<DeviceAttribute>& d, bool fwd = true);
/**
 * Writes an attribute on each device in the group asynchronously (Without DeviceAttribute data)
 *
 * Writes an attribute on each device in the group asynchronously. The method sends the request to all
 * devices and returns immediately. Pass the returned request id to Group::write_attribute_reply() to obtain
 * the acknowledgements.
 * This implementation of write_attribute_asynch allows writing a specific value to each device in the
 * group. In order to use this form of write_attribute_asynch, the user must have an "a priori" and "perfect"
 * knowledge of the devices order in the group.
 * The parameter a is the name of the attribute.
 * The std::vector d contains a specific value for each device in the group. Since this method is a template,
 * d is able to contain any Tango attribute type. Its size must equal Group::get_size(fwd). Otherwise, an
 * exception is thrown. The order of the attribute values must follows the order of the devices in the group
 * (d[0] => 1st device, d[1] => 2nd device and so on).
 * If fwd is set to true (the default) request is forwarded to subgroups. Otherwise, it is only applied to the
 * local set of devices.
 * See Case2 of Writing an attribute (Chapter 4.7.5.3 in <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for an example.
 *
 * @tparam T The attribute data type
 * @param [in] a The attribute name
 * @param [in] d The attribute value(s)
 * @param [in] fwd The forward flag
 * @return The call identifier
 */
  template<typename T> long write_attribute_asynch (const std::string &a, const std::vector<T> &d, bool fwd = true);
/**
 * Returns the acknowledgements of an asynchronous attribute writing.
 *
 * Returns the acknowledgements of an asynchronous attribute writing.
 * The first parameter req_id is a request identifier previously returned by one of the write_attribute_asynch
 * implementation.
 * For each device in the hierarchy, if the acknowledgement is not yet available, write_attribute_replywait
 * timeout_ms milliseconds before throwing an exception. This exception will be part of the global reply. If
 * timeout_ms is set to 0, write_attribute_reply waits "indefinitely".
 * Acknowledgements are returned in a GroupReplyList. See Obtaining acknowledgements (Chapter 4.7.5.1 in
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>) for
 * details.
 *
 * @param [in] req_id The request identifier
 * @param [in] tmo_ms The timeout value
 * @return The attribute writing acknowledgements
 */
  GroupReplyList write_attribute_reply (long req_id, long tmo_ms = 0);

///@privatesection
  //---------------------------------------------
  //- Misc.
  //---------------------------------------------
  //-
  virtual void dump (int indent_level = 0);
  //-
  virtual void dump (TangoSys_OMemStream& oms, int indent_level = 0);
//@}

private:
  //-
  long next_asynch_request_id ();
  //-
  bool add_i (GroupElement* e, bool fwd = true);
  //-
  void remove_i (const std::string& p, bool fwd = true);
  //-
  GroupElement* find_i (const std::string& n, bool fwd = true);
  //-
  GroupElements get_hiearchy ();
  //-
  Group* get_group_i (const std::string& n);
  //-
  long get_size_i (bool fwd);
  //-
  void push_async_request (long rid, bool fwded);
  //-
  void pop_async_request (long rid);
  //-
  virtual bool is_device_i ();
  //-
  virtual bool is_group_i ();

#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex elements_mutex;
#endif
  //- elements
  GroupElements elements;
  //- asynch request repository
  AsynchRequestDesc arp;
  //- pseudo asynch. req. id generator
  long asynch_req_id;

  //- forbidden methods
  Group ();
  Group (const Group&);
  Group& operator=(const Group&);

  //- private part of the asynch impl
  virtual long command_inout_asynch_i (const std::string& c, bool fgt, bool fwd, long ari);
  virtual long command_inout_asynch_i (const std::string& c, const DeviceData& d, bool fgt, bool fwd, long ari);
  virtual long command_inout_asynch_i (const std::string& c, const std::vector<DeviceData>& d, bool fgt, bool fwd, long ari);
  template<typename T> long command_inout_asynch_i (const std::string& c, /*const*/ std::vector<T>& d, bool fgt, bool fwd, long ari);
  virtual GroupCmdReplyList command_inout_reply_i (long req_id, long tmo_ms);

  virtual long read_attribute_asynch_i (const std::string& a, bool fwd, long ari);
  virtual GroupAttrReplyList read_attribute_reply_i (long req_id, long tmo_ms);

  virtual long read_attributes_asynch_i (const std::vector<std::string>& al, bool fwd, long ari);
  virtual GroupAttrReplyList read_attributes_reply_i (long req_id, long tmo_ms);

  virtual long write_attribute_asynch_i (const DeviceAttribute& d, bool fwd, long ari);
  virtual long write_attribute_asynch_i (const std::vector<DeviceAttribute>& d, bool fwd, long ari);
  template<typename T> long write_attribute_asynch_i (const std::string& a, /*const*/ std::vector<T>& d, bool fwd, long ari);
  virtual GroupReplyList write_attribute_reply_i (long req_id, long tmo_ms);
};

//=============================================================================
// class GroupDeviceElement: a device element
//=============================================================================
class GroupDeviceElement : public GroupElement
{
  friend class Group;
  friend class GroupElementFactory;

public:

  //---------------------------------------------
  //- Group management
  //---------------------------------------------
  //-
  virtual DeviceProxy* get_device (const std::string& n);
  //-
  virtual DeviceProxy* get_device (long idx);
  //-
  virtual DeviceProxy* operator[] (long idx);

  //---------------------------------------------
  //- a la DeviceProxy interface
  //---------------------------------------------
  //-
  virtual bool ping (bool fwd = true);
  //-
  virtual void set_timeout_millis (int tmo_ms);

  //---------------------------------------------
  //- Misc
  //---------------------------------------------
  //-
  virtual void dump (int indent_level = 0);
  //-
  virtual void dump (TangoSys_OMemStream& oms, int indent_level = 0);
  //-
  virtual bool is_connected();

private:
  //- the device proxy
  DeviceProxy *dp;
  //- asynch request repository
  AsynchRequestRep arp;

  //- forbidden methods
  GroupDeviceElement ();
  GroupDeviceElement (const GroupDeviceElement&);
  GroupDeviceElement& operator=(const GroupDeviceElement&);

  //- ctor: creates an GroupDeviceElement named <name>
  GroupDeviceElement (const std::string& name);
  //- ctor: creates a GroupDeviceElement named <name> with timeout set to tmo_ms milliseconds
  GroupDeviceElement (const std::string& name, int tmo_ms);
  //- dtor: release resources
  virtual ~GroupDeviceElement();

  //- build connection to the device (may throw DevFailed)
  DeviceProxy * connect ();
  //- close connection
  void disconnect ();

  //- a trick to get a valid device proxy or an exception
  inline DeviceProxy* dev_proxy () {
    return dp ? dp : connect();
  }

  //- element identification
  virtual bool is_device_i ();
  virtual bool is_group_i ();

  //- size (group interface)
  virtual long get_size_i (bool fwd = true);

  //- private part of the asynch impl
  virtual long command_inout_asynch_i (const std::string& c, bool fgt, bool fwd, long ari);
  virtual long command_inout_asynch_i (const std::string& c,  const DeviceData& d, bool fgt, bool fwd, long ari);
  virtual GroupCmdReplyList command_inout_reply_i (long req_id, long tmo_ms);

  virtual long read_attribute_asynch_i (const std::string& a, bool fwd, long ari);
  virtual GroupAttrReplyList read_attribute_reply_i (long req_id, long tmo_ms);

  virtual long read_attributes_asynch_i (const std::vector<std::string>& al, bool fwd, long ari);
  virtual GroupAttrReplyList read_attributes_reply_i (long req_id, long tmo_ms);

  virtual long write_attribute_asynch_i (const DeviceAttribute& d, bool fwd, long ari);
  virtual GroupReplyList write_attribute_reply_i (long req_id, long tmo_ms);
};

//=============================================================================
// GroupCmdReply::operator>> template impl.
//=============================================================================
template <typename T>
bool GroupCmdReply::operator>> (T& dest)
{
  bool result = true;
  if (GroupReply::group_element_enabled_m == false)
  {
    if (exception_enabled)
    {
      Tango::DevErrorList errors(1);
      errors.length(1);
      errors[0].severity = Tango::ERR;
      errors[0].desc = CORBA::string_dup("no available data");
      errors[0].reason = CORBA::string_dup("no data - group member is disabled");
      errors[0].origin = CORBA::string_dup("GroupCmdReply::operator>>");
      DevFailed df(errors);
      throw df;
    }
    result = false;
  }
  else if (GroupReply::has_failed_m == true)
  {
    if (exception_enabled)
      throw GroupReply::exception_m;
    result = false;
  }
  else
  {
    std::bitset<DeviceData::numFlags> bs;
    data_m.exceptions(exception_enabled ? bs.set() : bs.reset());
    try
    {
      result = data_m >> dest;
    }
    catch (const DevFailed& df)
    {
      GroupReply::exception_m = df;
      if (exception_enabled)
        throw GroupReply::exception_m;
      result = false;
    }
    catch (...) {
      if (exception_enabled)
      {
        Tango::DevErrorList errors(1);
        errors.length(1);
        errors[0].severity = Tango::ERR;
        errors[0].desc = CORBA::string_dup("unknown exception caught");
        errors[0].reason = CORBA::string_dup("an error occured while trying to extract data");
        errors[0].origin = CORBA::string_dup("GroupCmdReply::operator>>");
        DevFailed df(errors);
        GroupReply::exception_m = df;
        throw GroupReply::exception_m;
      }
      result = false;
    }
  }
  return result;
}

//=============================================================================
// GroupAttrReply::operator>> template impl.
//=============================================================================
template <typename T>
bool GroupAttrReply::operator>> (T& dest)
{
  bool result = true;
  if (GroupReply::group_element_enabled_m == false)
  {
    if (exception_enabled)
    {
      Tango::DevErrorList errors(1);
	    errors.length(1);
	    errors[0].severity = Tango::ERR;
	    errors[0].desc = CORBA::string_dup("no available data");
	    errors[0].reason = CORBA::string_dup("no data - group member is disabled");
	    errors[0].origin = CORBA::string_dup("GroupAttrReply::operator>>");
      DevFailed df(errors);
      throw df;
    }
    result = false;
  }
  else if (GroupReply::has_failed_m == true)
  {
    if (exception_enabled)
      throw GroupReply::exception_m;
    result = false;
  }
  else
  {
    std::bitset<DeviceAttribute::numFlags> bs;
    data_m.exceptions(exception_enabled ? bs.set() : bs.reset());
    bool result;
    try
    {
      result = data_m >> dest;
    }
    catch (const DevFailed& df)
    {
      GroupReply::exception_m = df;
      if (exception_enabled)
        throw GroupReply::exception_m;
      result = false;
    }
    catch (...)
    {
      if (exception_enabled)
      {
        Tango::DevErrorList errors(1);
	      errors.length(1);
	      errors[0].severity = Tango::ERR;
	      errors[0].desc = CORBA::string_dup("unknown exception caught");
	      errors[0].reason = CORBA::string_dup("an error occured while trying to extract data");
	      errors[0].origin = CORBA::string_dup("GroupAttrReply::operator>>");
        DevFailed df(errors);
        GroupReply::exception_m = df;
        throw GroupReply::exception_m;
      }
      result = false;
    }
  }
  return result;
}

//=============================================================================
// Group::command_inout template impl.
//=============================================================================
template<typename T>
GroupCmdReplyList Group::command_inout (const std::string& c, const std::vector<T>& d, bool fwd)
{
  long id = command_inout_asynch_i<T>(c, const_cast<std::vector<T>&>(d), false, fwd, -1);
  return command_inout_reply_i(id, 0);
}

//=============================================================================
// Group::command_inout_asynch template impl.
//=============================================================================
template<typename T>
long Group::command_inout_asynch (const std::string& c, const std::vector<T>& d, bool fgt, bool fwd)
{
  return command_inout_asynch_i<T>(c, const_cast<std::vector<T>&>(d), fgt, fwd, -1);
}

//=============================================================================
// Group::command_inout_asynch template impl.
//=============================================================================
template<typename T>
long Group::command_inout_asynch_i (const std::string& c, /*const*/ std::vector<T>& d, bool fgt, bool fwd, long ari)
{
  #ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
    omni_mutex_lock guard(elements_mutex);
  #endif

  long gsize = get_size_i(fwd);
  if (gsize != static_cast<long>(d.size()))
  {
    TangoSys_OMemStream desc;
	  desc << "the size of the input argument list must equal the number of device in the group"
         << " [expected:"
         << gsize
         << " - got:"
         << d.size()
         << "]"
         << ends;
    ApiDataExcept::throw_exception((const char*)API_MethodArgument,
                                   (const char*)desc.str().c_str(),
                                   (const char*)"Group::command_inout_asynch");
  }

  if (ari == -1)
    ari = next_asynch_request_id();

  for (unsigned int i = 0, j = 0; i < elements.size(); i++)
  {
    if (elements[i]->is_device_i())
    {
      Tango::DeviceData dd;
      dd << d[j++];
      elements[i]->command_inout_asynch_i(c, dd, fgt, false, ari);
    }
    else if (fwd)
    {
      Tango::Group * g = reinterpret_cast<Tango::Group*>(elements[i]);
      long gsize = g->get_size_i(fwd);
      std::vector<T> sub_d(d.begin() + j,  d.begin() + j + gsize);
      reinterpret_cast<Tango::Group*>(elements[i])->command_inout_asynch_i<T>(c, sub_d, fgt, fwd, ari);
      j += gsize;
    }
  }

  if (fgt == false) {
    push_async_request(ari, fwd);
  }

  return ari;
}

//=============================================================================
// Group::write_attribute_asynch template impl.
//=============================================================================
template<typename T>
GroupReplyList Group::write_attribute (const std::string& a, const std::vector<T>& d, bool fwd)
{
  long id = write_attribute_asynch_i<T>(a, const_cast<std::vector<T>&>(d), fwd, -1);
  return write_attribute_reply(id);
}

//=============================================================================
// Group::write_attribute_asynch template impl.
//=============================================================================
template<typename T>
long Group::write_attribute_asynch (const std::string& a, const std::vector<T>& d, bool fwd)
{
  return write_attribute_asynch_i<T>(a, const_cast<std::vector<T>&>(d), fwd, -1);
}

//=============================================================================
// Group::write_attribute_asynch_i template impl.
//=============================================================================
template<typename T>
long Group::write_attribute_asynch_i (const std::string& a, /*const*/ std::vector<T>& d, bool fwd, long ari)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif

  GroupReplyList rl;
  long gsize = get_size_i(fwd);
  if (gsize != static_cast<long>(d.size()))
  {
    TangoSys_OMemStream desc;
	  desc << "the size of the input argument list must equal the number of device in the group"
         << " [expected:"
         << gsize
         << " - got:"
         << d.size()
         << "]"
         << ends;
    ApiDataExcept::throw_exception((const char*)API_MethodArgument,
                                   (const char*)desc.str().c_str(),
                                   (const char*)"Group::write_attribute_asynch");
  }

  if (ari == -1)
    ari = next_asynch_request_id();

  DeviceAttribute da;
  da.name = a;

  for (unsigned int i = 0, j = 0; i < elements.size(); i++)
  {
    if (elements[i]->is_device_i())
    {
      da << d[j++];
      elements[i]->write_attribute_asynch_i(da, false, ari);
    }
    else if (fwd)
    {
      Tango::Group * g = reinterpret_cast<Tango::Group*>(elements[i]);
      long gsize = g->get_size_i(fwd);
      std::vector<T> sub_d(d.begin() + j,  d.begin() + j + gsize);
      reinterpret_cast<Tango::Group*>(elements[i])->write_attribute_asynch_i(a, sub_d, fwd, ari);
      j += gsize;
    }
  }
  push_async_request(ari, fwd);
  return ari;
}

} // namespace Tango


#endif /* _GROUP_H_ */

