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
class GroupReply
{
public:
  //- enable/disable exception - returns the previous mode
  static bool enable_exception (bool exception_mode = true);
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
  //- has_failed accessor
  inline bool has_failed () const {
    return has_failed_m;
  }
  //- group_element_enabled accessor
  inline bool group_element_enabled () const {
    return group_element_enabled_m;
  }
  //- device name accessor
  inline const std::string& dev_name () const {
    return dev_name_m;
  }
  //- object (i.e. command or attribute) name accessor
  inline const std::string& obj_name () const {
    return obj_name_m;
  }
  //- error stack accessor
  inline const DevErrorList& get_err_stack () const {
    return exception_m.errors;
  }
protected:
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
class GroupCmdReply : public GroupReply
{
public:
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
  //- data accessor (may throw Tango::DevFailed)
  DeviceData& get_data ();
  //- template data exctractor method
  template <typename T> bool operator>> (T& dest);
  //- data exctractor method for DevVarLongStringArray
  bool extract (std::vector<DevLong>& vl, std::vector<std::string>& vs);
  //- data exctractor method for DevVarDoubleStringArray
  bool extract (std::vector<double>& vd, std::vector<std::string>& vs);
private:
  //- data: valid if GroupReply::has_failed_m set to false and
  //- GroupReply::enabled_m set to true
  DeviceData data_m;
};

//=============================================================================
// class GroupAttrReply : reply to an attribute reading executed on a group
//=============================================================================
class GroupAttrReply : public GroupReply
{
public:
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
  //- data accessor (may throw Tango::DevFailed)
  DeviceAttribute& get_data ();
  //- template data exctractor method
  template <typename T> bool operator>> (T& dest);
private:
  //- data: valid if GroupReply::has_failed_m set to false and
  //- GroupReply::enabled_m set to true
  DeviceAttribute data_m;
};

//=============================================================================
// class GroupReplyList : a simple list of GroupReply
//=============================================================================
class GroupReplyList : public std::vector<GroupReply>
{
  typedef std::vector<GroupReply> Inherited;

  friend class Group;

public:
  //- ctor
  GroupReplyList();
  //- dtor
  virtual ~GroupReplyList();
  //- has_failed accessor
  bool has_failed () const {
    return has_failed_m;
  }
  //- reset the error list
  inline void reset () {
    clear();
    has_failed_m = false;
  };
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
class GroupCmdReplyList : public std::vector<GroupCmdReply>
{
  typedef std::vector<GroupCmdReply> Inherited;

  friend class Group;

public:
  //- ctor
  GroupCmdReplyList();
  //- dtor
  virtual ~GroupCmdReplyList();
  //- has_failed accessor method
  bool has_failed () const {
    return has_failed_m;
  }
  //- reset the error list
  inline void reset () {
    clear();
    has_failed_m = false;
  };
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
class GroupAttrReplyList : public std::vector<GroupAttrReply>
{
  typedef std::vector<GroupAttrReply> Inherited;

  friend class Group;

public:
  //- ctor
  GroupAttrReplyList();
  //- dtor
  virtual ~GroupAttrReplyList();
  //- has_failed accessor method
  bool has_failed () const {
    return has_failed_m;
  }
  //- reset the error list
  inline void reset () {
    clear();
    has_failed_m = false;
  };
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
class Group : public GroupElement
{
  typedef std::map<long, bool> AsynchRequestDesc;
  typedef AsynchRequestDesc::iterator AsynchRequestDescIt;
  typedef AsynchRequestDesc::value_type AsynchRequestDescVal;

public:

  //---------------------------------------------
  //- Ctor & Dtor
  //---------------------------------------------
  //- ctor: creates an empty group named <name>
  Group (const std::string& name);
  //- dtor: release resources
  virtual ~Group();

  //---------------------------------------------
  //- Group management methods
  //---------------------------------------------
  //-
  virtual void add (Group* group, int tmo_ms = -1);
  //-
  virtual void add (const std::string& pattern, int tmo_ms = -1);
  //-
  virtual void add (const std::vector<std::string>& patterns, int tmo_ms = -1);
  //-
  virtual void remove (const std::string& pattern, bool fwd = true);
  //-
  virtual void remove (const std::vector<std::string>& patterns, bool fwd = true);
  //-
  virtual void remove_all ();
  //-
  virtual bool contains (const std::string& pattern, bool fwd = true);
  //-
  virtual DeviceProxy* get_device (const std::string& device_name);
  //-
  virtual DeviceProxy* get_device (long idx);
  //-
  virtual DeviceProxy* operator[] (long idx);
  //-
  virtual Group* get_group (const std::string& group_name);
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

  //- misc.
  //---------------------------------------------
  virtual bool ping (bool fwd = true);
  //-
  virtual void set_timeout_millis (int tmo_ms);

  //- command execution
  //---------------------------------------------
  GroupCmdReplyList command_inout (const std::string& c, bool fwd = true);
  //-
  GroupCmdReplyList command_inout (const std::string& c, const DeviceData& d, bool fwd = true);
  //-
  GroupCmdReplyList command_inout (const std::string& c, const std::vector<DeviceData>& d, bool fwd = true);
  //-
  template<typename T> GroupCmdReplyList command_inout (const std::string& c, const std::vector<T>& d, bool fwd = true);
  //-
  long command_inout_asynch (const std::string& c, bool fgt = false, bool fwd = true);
  //-
  long command_inout_asynch (const std::string& c, const DeviceData& d, bool fgt = false, bool fwd = true);
  //-
  long command_inout_asynch (const std::string& c, const std::vector<DeviceData>& d, bool fgt = false, bool fwd = true);
  //-
  template<typename T> long command_inout_asynch (const std::string& c, const std::vector<T>& d, bool fgt = false, bool fwd = true);
  //-
  GroupCmdReplyList command_inout_reply (long req_id, long tmo_ms = 0);

  //- attribute(s) reading
  //---------------------------------------------
  GroupAttrReplyList read_attribute (const std::string& a, bool fwd = true);
  //-
  GroupAttrReplyList read_attributes (const std::vector<std::string>& al, bool fwd = true);
  //-
  long read_attribute_asynch (const std::string& a, bool fwd = true);
  //-
  long read_attributes_asynch (const std::vector<std::string>& al, bool fwd = true);
  //-
  GroupAttrReplyList read_attribute_reply (long req_id, long tmo_ms = 0);
  //-
  GroupAttrReplyList read_attributes_reply (long req_id, long tmo_ms = 0);

  //- attribute writting
  //---------------------------------------------
  GroupReplyList write_attribute (const DeviceAttribute& d, bool fwd = true);
  //-
  GroupReplyList write_attribute (const std::vector<DeviceAttribute>& d, bool fwd = true);
  //-
  template<typename T> GroupReplyList write_attribute (const std::string& n, const std::vector<T>& d, bool fwd = true);
  //-
  long write_attribute_asynch (const DeviceAttribute& d, bool fwd = true);
  //-
  long write_attribute_asynch (const std::vector<DeviceAttribute>& d, bool fwd = true);
  //-
  template<typename T> long write_attribute_asynch (const std::string& a, const std::vector<T>& d, bool fwd = true);
  //-
  GroupReplyList write_attribute_reply (long req_id, long tmo_ms = 0);

  //---------------------------------------------
  //- Misc.
  //---------------------------------------------
  //-
  std::vector<std::string> get_device_list (bool fwd = true);
  //-
  long get_size (bool fwd = true);
  //-
  virtual void dump (int indent_level = 0);
  //-
  virtual void dump (TangoSys_OMemStream& oms, int indent_level = 0);

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
    ApiDataExcept::throw_exception((const char*)"API_MethodArgument",
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
    ApiDataExcept::throw_exception((const char*)"API_MethodArgument",
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

