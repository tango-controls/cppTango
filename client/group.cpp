//=============================================================================
//
// file :               group.cpp
//
// description :        Tango Group impl.
//
// project :            TANGO
//
// author(s) :          N.Leclercq
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#include <group.h>

//-----------------------------------------------------------------------------
// LOCAL DEBUGGING MACRO
//-----------------------------------------------------------------------------
//#define _LOCAL_DEBUGGING  

namespace Tango
{
//=============================================================================
// STATIC MEMBERS
//=============================================================================
bool GroupReply::exception_enabled = false;

//=============================================================================
// class GroupElementFactory
//=============================================================================
GroupElements GroupElementFactory::instanciate (const std::string& p)
{
#if defined(_LOCAL_DEBUGGING)
  cerr << "GroupElementFactory::instanciate::pattern [" << p << "]" << endl;
#endif
  //- a vector to store device names
  std::vector<std::string> dnl(0);
  //- is <p> a device name or a device name pattern ?
  if (p.find('*', 0) == std::string::npos) {
#if defined(_LOCAL_DEBUGGING)
    cerr << "\t|- pattern is pure device name" << endl;
#endif
    dnl.push_back(p);
  }
  else {
    Database db;
    //- ask the db the list of device matching pattern p
    DbDatum dbd = db.get_device_exported(const_cast<std::string&>(p));
    //- extract device names from dbd
    dbd >> dnl;
#if defined(_LOCAL_DEBUGGING)
    cerr << "\t|- db.get_device_exported::found ";
    cerr << dnl.size() << " device names matching pattern" << endl;
    for (unsigned int dn = 0; dn < dnl.size(); dn++) {
      cerr << "\t\t|- " << dnl[dn] << endl;
    }
#endif 
  }
  //- build the returned GroupElementList
  GroupElements tel(0);
  GroupDeviceElement* tde;
  for (unsigned int i = 0; i < dnl.size(); i++) {
    tde = new GroupDeviceElement(dnl[i]);
    if (tde) {
      tel.push_back(tde);
    } 
  }
#if defined(_LOCAL_DEBUGGING)
    cerr << "\t|- GroupElementList contains " << tel.size() << " elements" << endl;
#endif
  //- return the list to the caller
  return tel;
}

//=============================================================================
// class GroupCmdReply 
//=============================================================================
GroupReply::GroupReply ()
  : dev_name_m("unknown"), 
    obj_name_m("unknown"), 
    has_failed_m(false),
    group_element_enabled_m(true)
{

}
//-----------------------------------------------------------------------------
GroupReply::GroupReply (const GroupReply& _src)
 : dev_name_m(_src.dev_name_m), 
   obj_name_m(_src.obj_name_m), 
   has_failed_m(_src.has_failed_m),
   group_element_enabled_m(_src.group_element_enabled_m)
{

}
//-----------------------------------------------------------------------------
GroupReply::GroupReply (const std::string& _dev_name, 
                        const std::string& _obj_name,
                        bool group_element_enabled)
 : dev_name_m(_dev_name), 
   obj_name_m(_obj_name), 
   has_failed_m(false),
   group_element_enabled_m(group_element_enabled)
{

}
//-----------------------------------------------------------------------------
GroupReply::GroupReply (const std::string& _dev_name, 
                        const std::string& _obj_name,
                        const Tango::DevFailed& exception,
                        bool group_element_enabled)
 : dev_name_m(_dev_name), 
   obj_name_m(_obj_name), 
   has_failed_m(true), 
   group_element_enabled_m(group_element_enabled),
   exception_m(exception)
{

}
//-----------------------------------------------------------------------------
GroupReply::~GroupReply () 
{
  //-noop impl
}
//-----------------------------------------------------------------------------
bool GroupReply::enable_exception (bool exception_mode) 
{
  bool tmp = GroupReply::exception_enabled;
  GroupReply::exception_enabled = exception_mode;
  return tmp;
}

//=============================================================================
// class GroupReplyList
//=============================================================================
GroupReplyList::GroupReplyList () 
  : std::vector<GroupReply>(0), has_failed_m(false) 
{
  //-noop impl
}
//-----------------------------------------------------------------------------
GroupReplyList::~GroupReplyList () 
{
  //-noop impl
}

//=============================================================================
// class GroupCmdReply : reply to command executed on a group
//=============================================================================
GroupCmdReply::GroupCmdReply ()
 : GroupReply()
{
  //-noop impl
}
//-----------------------------------------------------------------------------
GroupCmdReply::GroupCmdReply (const GroupCmdReply& src)
  : GroupReply(src), data_m(src.data_m)
{
  
}
//-----------------------------------------------------------------------------
GroupCmdReply::GroupCmdReply (const std::string& _dev_name, 
                              const std::string& _obj_name,
                              const Tango::DeviceData& _dev_data)
 : GroupReply(_dev_name, _obj_name, true), data_m(_dev_data)
{
  //-noop impl   
}
//-----------------------------------------------------------------------------
GroupCmdReply::GroupCmdReply (const std::string& _dev_name, 
                              const std::string& _obj_name,
                              const Tango::DevFailed& _exception)
 : GroupReply(_dev_name, _obj_name, _exception, true)
{
  //-noop impl  
}
//-----------------------------------------------------------------------------
GroupCmdReply::GroupCmdReply (const std::string& _dev_name, 
                              const std::string& _obj_name,
                              bool group_element_enabled)
 : GroupReply(_dev_name, _obj_name, group_element_enabled)
{
  //-noop impl  
}
//-----------------------------------------------------------------------------
GroupCmdReply::~GroupCmdReply () 
{
  //-noop impl
}
//-----------------------------------------------------------------------------
/*const*/ DeviceData& GroupCmdReply::get_data (void) 
{
  if (group_element_enabled_m == false && exception_enabled) 
  {
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].desc = CORBA::string_dup("no available data");
		errors[0].reason = CORBA::string_dup("no data - group member is disabled");
		errors[0].origin = CORBA::string_dup("GroupCmdReply::get_data");
    DevFailed df(errors);
    throw df;
  }
  else if (has_failed_m && exception_enabled) 
  {
    throw exception_m;
  }
  std::bitset<DeviceData::numFlags> bs;
  data_m.exceptions(exception_enabled ? bs.set() : bs.reset());
  return data_m; 
}
//-----------------------------------------------------------------------------
bool GroupCmdReply::extract (std::vector<long>& vl, std::vector<std::string>& vs) 
{
  bool result = true;
  if (group_element_enabled_m == false) 
  {
    if (exception_enabled) 
    {
      Tango::DevErrorList errors(1);
		  errors.length(1);
		  errors[0].severity = Tango::ERR;
		  errors[0].desc = CORBA::string_dup("no available data");
		  errors[0].reason = CORBA::string_dup("no data - group member is disabled");
		  errors[0].origin = CORBA::string_dup("GroupCmdReply::extract");
      DevFailed df(errors);
      throw df;
    }
    result = false;
  }
  else if (has_failed_m == true) 
  {
    if (exception_enabled)
      throw exception_m;
    result = false;
  }
  else {
    std::bitset<DeviceData::numFlags> bs;
    data_m.exceptions(exception_enabled ? bs.set() : bs.reset()); 
    try 
    {
      result = data_m.extract(vl, vs);
    }
    catch (const Tango::DevFailed& df) 
    {
      exception_m = df;
      if (exception_enabled) 
        throw exception_m;
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
		    errors[0].origin = CORBA::string_dup("GroupCmdReply::extract");
        DevFailed df(errors);
        exception_m = df;
        throw exception_m;
      }
      result = false;
    }
  }
  return result;
}
//-----------------------------------------------------------------------------
bool GroupCmdReply::extract (std::vector<double>& vd, std::vector<std::string>& vs) 
{
  bool result = true;
  if (group_element_enabled_m == false) 
  {
    if (exception_enabled) 
    {
      Tango::DevErrorList errors(1);
		  errors.length(1);
		  errors[0].severity = Tango::ERR;
		  errors[0].desc = CORBA::string_dup("no available data");
		  errors[0].reason = CORBA::string_dup("no data - group member is disabled");
		  errors[0].origin = CORBA::string_dup("GroupCmdReply::extract");
      DevFailed df(errors);
      throw df;
    }
    result = false;
  }
  else if (has_failed_m == true) 
  {
    if (exception_enabled)
      throw exception_m;
    result = false;
  }
  else 
  {
    std::bitset<DeviceData::numFlags> bs;
    data_m.exceptions(exception_enabled ? bs.set() : bs.reset()); 
    try {
      result = data_m.extract(vd, vs);
    }
    catch (const Tango::DevFailed& df) {
      exception_m = df;
      if (exception_enabled) {
        throw exception_m;
      }
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
		    errors[0].origin = CORBA::string_dup("GroupCmdReply::extract");
        DevFailed df(errors);
        exception_m = df;
        throw exception_m;
      }
      result = false;
    }
  }
  return result;
}

//=============================================================================
// class GroupCmdReplyList
//=============================================================================
GroupCmdReplyList::GroupCmdReplyList () 
  : std::vector<GroupCmdReply>(0), has_failed_m(false) 
{
  //-noop impl
}
//-----------------------------------------------------------------------------
GroupCmdReplyList::~GroupCmdReplyList () 
{
  //-noop impl
}

//=============================================================================
// class GroupAttrReply : reply to read/write attr on a group
//=============================================================================
GroupAttrReply::GroupAttrReply ()
 : GroupReply()
{
  //-noop impl
}
//-----------------------------------------------------------------------------
GroupAttrReply::GroupAttrReply (const GroupAttrReply& src)
  : GroupReply(src), data_m(src.data_m)
{
  
}
//-----------------------------------------------------------------------------
GroupAttrReply::GroupAttrReply (const std::string& _dev_name, 
                                const std::string& _obj_name,
                                const Tango::DeviceAttribute& _dev_attr)
 : GroupReply(_dev_name, _obj_name, true), data_m(_dev_attr)
{
   
}
//-----------------------------------------------------------------------------
GroupAttrReply::GroupAttrReply (const std::string& _dev_name, 
                                const std::string& _obj_name,
                                const Tango::DevFailed& _exception)
 : GroupReply(_dev_name, _obj_name, _exception, true)
{
  //-noop impl  
}
//-----------------------------------------------------------------------------
GroupAttrReply::GroupAttrReply (const std::string& _dev_name, 
                                const std::string& _obj_name,
                                bool _group_element_enabled)
 : GroupReply(_dev_name, _obj_name, _group_element_enabled)
{
  //-noop impl   
}
//-----------------------------------------------------------------------------
GroupAttrReply::~GroupAttrReply () 
{
  //-noop impl
}
//-----------------------------------------------------------------------------
/*const*/ DeviceAttribute& GroupAttrReply::get_data (void) 
{
  if (group_element_enabled_m == false && exception_enabled) 
  {
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].desc = CORBA::string_dup("no available data");
		errors[0].reason = CORBA::string_dup("no data - group member is disabled");
		errors[0].origin = CORBA::string_dup("GroupCmdReply::get_data");
    DevFailed df(errors);
    throw df;
  }
  else if (has_failed_m && exception_enabled) 
  {
    throw exception_m;
  }
  std::bitset<DeviceAttribute::numFlags> bs;
  data_m.exceptions(exception_enabled ? bs.set() : bs.reset());
  return data_m; 
}

//=============================================================================
// class GroupAttrReplyList
//=============================================================================
GroupAttrReplyList::GroupAttrReplyList () 
  : std::vector<GroupAttrReply>(0), has_failed_m(false) 
{
  //-noop impl
}
//-----------------------------------------------------------------------------
GroupAttrReplyList::~GroupAttrReplyList () 
{
  //-noop impl
}

//=============================================================================
// class GroupElement
//=============================================================================
GroupElement::GroupElement (const std::string& _name, GroupElement* _parent) 
  : name(_name), parent(_parent), enabled(true)
{
  //- noop ctor
}
//-----------------------------------------------------------------------------
GroupElement::~GroupElement()
{
  //- noop dtor
}
//-----------------------------------------------------------------------------
void GroupElement::add (const std::string& s)
{
  //- noop default impl
}
//-----------------------------------------------------------------------------
void GroupElement::add (const std::vector<std::string>& sl)
{
  //- noop default impl
}
//-----------------------------------------------------------------------------
void GroupElement::remove (const std::string& s, bool fwd)
{
  //- noop default impl
}
//-----------------------------------------------------------------------------
void GroupElement::remove (const std::vector<std::string>& sl, bool fwd)
{
  //- noop default impl
}
//-----------------------------------------------------------------------------
bool GroupElement::contains (const std::string& n, bool fwd)
{
  std::string::size_type pos = n.find('*',0);
  return (pos == std::string::npos) ? name_equals(n) : name_matches(n);
} 
//-----------------------------------------------------------------------------
GroupElement* GroupElement::find (const std::string& n, bool fwd) 
{
  return name_equals(n) ? this : 0;
} 
//-----------------------------------------------------------------------------
DeviceProxy* GroupElement::get_device (const std::string& n) 
{
  return 0;
} 
//-----------------------------------------------------------------------------
DeviceProxy* GroupElement::get_device (long idx) 
{
  return 0;
} 
//-----------------------------------------------------------------------------
DeviceProxy* GroupElement::operator[] (long idx) 
{
  return 0;
} 
//-----------------------------------------------------------------------------
Group* GroupElement::get_group (const std::string& n) 
{
  return 0;
} 
//-----------------------------------------------------------------------------
bool GroupElement::ping (bool fwd)
{
  return false;
}
//-----------------------------------------------------------------------------
TokenList GroupElement::tokenize (const std::string& p)
{
#if defined(_LOCAL_DEBUGGING)
  cerr << "\t|- Group::tokenize::pattern [" << p << "]" << endl;
#endif
	int done = 0;
  std::string token;
  std::string::size_type pos;
  std::string::size_type last_pos = 0;
  std::vector<std::string> tokens(0);
	do {
		pos = p.find('*', last_pos);
		if (pos != 0) {
			if (pos == std::string::npos) {
				if (last_pos >= p.size())
					break;
				pos = p.size();
				done = 1;
			}
			token.assign(p.begin() + last_pos, p.begin() + pos);
			tokens.push_back(token);
		}
		last_pos = pos + 1;
  } while (!done);
#if defined(_LOCAL_DEBUGGING)
  cerr << "\t\t|- tokens: [";
  for (int t = 0; t < tokens.size(); t++) {
    cerr << tokens[t];
    cerr << ((t == tokens.size() - 1) ? "]" : ";"); 
  }
  cerr << endl;
#endif
  return tokens;
}
//-----------------------------------------------------------------------------
bool GroupElement::match (const std::string& _p, const TokenList& tokens)
{
  unsigned int t;
  std::string p(_p);
  std::transform(p.begin(),p.end(),p.begin(),::tolower);
#if defined(_LOCAL_DEBUGGING)
  cerr << "\t|- Group::match::pattern " << _p << endl;
#endif
  bool result = false;
  std::string::size_type pos;
  for (t = 0, pos = 0; t < tokens.size(); t++) 
  {
    string token(tokens[t]);
    std::transform(token.begin(),token.end(),token.begin(),::tolower);
	  pos = p.find(token, pos);
	  if (pos == std::string::npos)
		  break;
  }
  if (t == tokens.size()) {
    result = true;
  }
#if defined(_LOCAL_DEBUGGING)
  if (result == true) {
    cerr << "\t\t|- Group::match::pattern matches tokens" << endl;
  } else {
    cerr << "\t\t|- Group::match::pattern does NOT match tokens" << endl;
  }
#endif
  return result;
}
//-----------------------------------------------------------------------------
bool GroupElement::name_matches (const std::string& n) 
{
  TokenList tokens = tokenize(n);
  return match(name, tokens) || match(get_fully_qualified_name(), tokens);
}
//-----------------------------------------------------------------------------
bool GroupElement::name_equals (const std::string& in_name) 
{
  std::string lin(in_name);
  std::transform(lin.begin(),lin.end(),lin.begin(),::tolower);

  std::string lfqn(get_fully_qualified_name());
  std::transform(lfqn.begin(),lfqn.end(),lfqn.begin(),::tolower);

  std::string ln(name);
  std::transform(ln.begin(),ln.end(),ln.begin(),::tolower);

  return lin == ln || lin == lfqn;
}

//=============================================================================
// class Group
//=============================================================================
Group::Group (const std::string& name) 
  : GroupElement(name), asynch_req_id(0) 
{
  //- noop ctor
}
//-----------------------------------------------------------------------------
Group::~Group () 
{
  remove_all();
  arp.clear();
}
//-----------------------------------------------------------------------------
std::vector<std::string> Group::get_device_list (bool fwd) 
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  std::vector<std::string> dl(0);
  std::vector<std::string> sub_dl(0);
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; it++) {
    if ((*it)->is_device()) {
      dl.push_back((*it)->get_name());
    }
    else if (fwd) {
      sub_dl = (static_cast<Group*>(*it))->get_device_list(fwd);
      if (sub_dl.empty() == false) {
        dl.insert(dl.end(), sub_dl.begin(), sub_dl.end());
      }
      sub_dl.clear();
    }
  }
  return dl;
}
//-----------------------------------------------------------------------------
GroupElements Group::get_hiearchy (void)
{
  GroupElements te(0);
  GroupElements sub_te(0);
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; it++) {
    if ((*it)->is_device()) {
      te.push_back(*it);
    }
    else {
      sub_te = (static_cast<Group*>(*it))->get_hiearchy();
      if (sub_te.empty() == false) {
        te.insert(te.end(), sub_te.begin(), sub_te.end());
      }
      sub_te.clear();
    }
  }
  return te;
}
//-----------------------------------------------------------------------------
long Group::get_size (bool fwd)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  return get_size_i(fwd);
}
//-----------------------------------------------------------------------------
long Group::get_size_i (bool fwd)
{
  long size = 0;
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; it++) {
    if ((*it)->is_device() || fwd) {
      size += (*it)->get_size();
    }
  }
  return size;
}
//-----------------------------------------------------------------------------
void Group::add (Group* g)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
#if defined(_LOCAL_DEBUGGING)
  cerr << "Group::add::adding sub-group " << (g ? g->get_name() : "NULL") << endl;
#endif
  if (g == 0 || get_group_i(g->get_name()) == g || g->get_group_i(get_name()) == this) {
#if defined(_LOCAL_DEBUGGING)
    cerr << "Group::add::failed to add group" << (g ? (g->get_name() + " (self ref. or already in group)") : "NULL") << endl;
#endif
    return;
  }
  add_i(g, false);
}
//-----------------------------------------------------------------------------
void Group::add (const std::string& p)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif  
  GroupElements el = GroupElementFactory::instanciate(p);
  for (unsigned int e = 0; e < el.size(); e++) {
    if (el[e] && add_i(el[e]) == false) {
      delete el[e];
    }
  }
}
//-----------------------------------------------------------------------------
void Group::add (const std::vector<std::string>& pl)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  for (unsigned int p = 0; p < pl.size(); p++) {
    GroupElements el = GroupElementFactory::instanciate(pl[p]);
    for (unsigned int e = 0; e < el.size(); e++) {
      if (el[e] && add_i(el[e]) == false) {
        delete el[e];
      }
    }
  }
}
//-----------------------------------------------------------------------------
bool Group::add_i (GroupElement* e, bool fwd)
{
  if (e == 0 || e == this) {
#if defined(_LOCAL_DEBUGGING)
    cerr << "Group::add_i::failed to add " << (e ? (e->get_name() + " (null or self ref)") : "NULL") << endl;
#endif
    return false;
  }
  GroupElement* te = find_i(e->get_name(), fwd);
  if (te != 0 && te != this) {
#if defined(_LOCAL_DEBUGGING)
    cerr << "Group::add_i::failed to add " << e->get_name() << " (already in group)" << endl;
#endif
    return false;
  }
  elements.push_back(e);
  e->set_parent(this);
#if defined(_LOCAL_DEBUGGING)
  cerr << "Group::add_i::";
  if (b == true) {
    cerr << "added ";
  } else {
    cerr << "failed to add ";
  }
  cerr << e->get_name() << endl;
#endif
  return true;
}
//-----------------------------------------------------------------------------
void Group::remove (const std::string& p, bool fwd) 
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
#if defined(_LOCAL_DEBUGGING)
  cerr << "Group::remove::pattern [" << p << "]" << endl;
#endif
  remove_i(p, fwd);
}
//-----------------------------------------------------------------------------
void Group::remove (const std::vector<std::string>& pl, bool fwd)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
#if defined(_LOCAL_DEBUGGING)
  cerr << "Group::remove::pattern list" << endl;
#endif
  for (unsigned int p = 0; p < pl.size(); p++) {
    remove_i(pl[p], fwd);
  }
}
//-----------------------------------------------------------------------------
void Group::remove_i (const std::string& p, bool fwd)
{ 
#if defined(_LOCAL_DEBUGGING)
  cerr << "\t|- Group::remove_i" << endl;
#endif
  if (name_equals(p)) {
#if defined(_LOCAL_DEBUGGING)
    cerr << "Group::remove_i::failed to remove " << _p << " (can't remove self)" << endl;
#endif
    return;
  }
  GroupElementsIterator it;
  if (p.find('*', 0) == std::string::npos) {
    for (it = elements.begin(); it != elements.end(); it++) {
      if ((*it)->name_equals(p)) {
        delete *it;
        elements.erase(it);
        break;
      }
    }
  }
  else {
    std::vector<GroupElement*> remove_list(0);
    for (it = elements.begin(); it != elements.end(); it++) {
      if ((*it)->name_matches(p)) {
        remove_list.push_back(*it);
      }
    }
    for (unsigned int i = 0; i < remove_list.size(); i++) {
      it = std::find(elements.begin(), elements.end(), remove_list[i]);
      if (it != elements.end()) {
# if defined(_LOCAL_DEBUGGING)
        cerr << "\t|- Group::remove_i::removing " << (*it)->get_name() << endl;
# endif // _LOCAL_DEBUGGING
        delete remove_list[i];
        elements.erase(it); 
      }
    }
  }
  if (fwd == true) {
    for (it = elements.begin(); it != elements.end(); it++) {
      (*it)->remove(p, fwd);
    }
  }
}
//-----------------------------------------------------------------------------
void Group::remove_all (void)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
#if defined(_LOCAL_DEBUGGING)
  cerr << "Group::remove_all::" << get_name() << endl;
#endif
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; ++it) {
    delete *it;
  }
  elements.clear();
}
//-----------------------------------------------------------------------------
bool Group::contains (const std::string& n, bool fwd)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  return (find_i(n, fwd) != 0) ? true : false;
}
//-----------------------------------------------------------------------------
GroupElement* Group::find (const std::string& n, bool fwd)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  return find_i(n, fwd);
}
//-----------------------------------------------------------------------------
GroupElement* Group::find_i (const std::string& n, bool fwd)
{
  std::string::size_type pos = n.find('*',0);
  bool is_pattern = (pos != std::string::npos) ? true : false;
  if (is_pattern) {
    if (name_matches(n))
      return this;
  }
  else {
    if (name_equals(n))
      return this;
  }
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; ++it) 
  {
    if (is_pattern) {
      if ((*it)->name_matches(n))
        return *it; 
    }
    else {
      if ((*it)->name_equals(n))
        return *it;    
    }
  }
  GroupElement* e = 0;
  if (fwd) {
    it = elements.begin();
    end = elements.end();
    while (it != end && e == 0) {
      e = (*it)->find(n);
      it++;
    }
  }
  return e;
}
//-----------------------------------------------------------------------------
DeviceProxy* Group::get_device (const std::string& n) 
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  DeviceProxy* dp = 0;
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  while (it != end && dp == 0) {
    dp = (*it)->get_device(n);
    it++;
  }
  return dp;
} 
//-----------------------------------------------------------------------------
DeviceProxy* Group::get_device (long idx) 
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  DeviceProxy* dp = 0;
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  while (it != end && dp == 0) {
    dp = (*it)->get_device(idx--);
    it++;
  }
  return dp;
} 
//-----------------------------------------------------------------------------
DeviceProxy* Group::operator[] (long idx) 
{
  return get_device(idx); 
} 
//-----------------------------------------------------------------------------
Group* Group::get_group (const std::string& n) 
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  return get_group_i(n); 
} 
//-----------------------------------------------------------------------------
Group* Group::get_group_i (const std::string& n) 
{
  GroupElement* e = find_i(n, true);
  return (e != 0 && e->is_group()) ? (Group*)e : 0;
} 
//-----------------------------------------------------------------------------
void Group::enable (const std::string& n, bool fwd) 
{
  GroupElement* e = find_i(n, fwd);
  if (e) e->enable();
} 
//-----------------------------------------------------------------------------
void Group::disable (const std::string& n, bool fwd) 
{
  GroupElement* e = find_i(n, fwd);
  if (e) e->disable();
} 
//-----------------------------------------------------------------------------
bool Group::ping (bool fwd)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  bool result = true;
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end && result != false ; ++it) {
    if ((*it)->is_device() || fwd) {
      result = (*it)->ping(fwd);    
    }
  }
  return result;
}
//-----------------------------------------------------------------------------
GroupCmdReplyList Group::command_inout (const std::string& c, bool fwd)
{
  long id = command_inout_asynch(c, false, fwd);
  return command_inout_reply(id);
}
//-----------------------------------------------------------------------------
GroupCmdReplyList Group::command_inout (const std::string& c, const DeviceData& d, bool fwd)
{
  long id = command_inout_asynch(c, d, false, fwd);
  return command_inout_reply(id);
}
//-----------------------------------------------------------------------------
long Group::command_inout_asynch (const std::string& c, bool fgt, bool fwd, long id)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  if (id == -1) {
    id = next_req_id();
  }
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; ++it) {
    if ((*it)->is_device() || fwd) {
      (*it)->command_inout_asynch(c, fgt, fwd, id); 
    }
  }
  if (fgt == false) {
    push_async_request(id, fwd);
  }
  return id;
}
//-----------------------------------------------------------------------------
long Group::command_inout_asynch (const std::string& c, const DeviceData& d, bool fgt, bool fwd, long id)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  if (id == -1) {
    id = next_req_id();
  }
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; ++it) {
    if ((*it)->is_device() || fwd) {
      (*it)->command_inout_asynch(c, d, fgt, fwd, id);
    }
  }
  if (fgt == false) {
    push_async_request(id, fwd);
  }
  return id;
}
//-----------------------------------------------------------------------------
GroupCmdReplyList Group::command_inout_reply (long req_id, long tmo)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  AsynchRequestDescIt r = arp.find(req_id);
  if (r == arp.end()) {
    Tango::DevErrorList errors(1);
    errors.length(1);
    errors[0].severity = Tango::ERR;
    errors[0].reason = CORBA::string_dup("API_BadAsynPollId");
    errors[0].desc = CORBA::string_dup("Invalid asynch. request identifier specified");
    errors[0].origin = CORBA::string_dup("Group::command_inout_reply");
    throw DevFailed(errors);
  }
  GroupCmdReplyList reply;
  GroupCmdReplyList sub_reply;
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; ++it) {
    if ((*it)->is_device() || r->second) {
      sub_reply = (*it)->command_inout_reply(req_id, tmo);
      if (sub_reply.empty() == false) {
        reply.insert(reply.end(), sub_reply.begin(), sub_reply.end());
      }
      if (sub_reply.has_failed_m) {
        reply.has_failed_m = true;
      }
      sub_reply.reset();
    }
  }
  arp.erase(r);
  return reply;
}
//-----------------------------------------------------------------------------
GroupAttrReplyList Group::read_attribute (const std::string& a, bool fwd)
{
  long id = read_attribute_asynch(a, fwd);
  return read_attribute_reply(id);
}
//-----------------------------------------------------------------------------
GroupAttrReplyList Group::read_attributes (const std::vector<std::string>& al, bool fwd)
{
  long id = read_attributes_asynch(al, fwd);
  return read_attributes_reply(id);
}
//-----------------------------------------------------------------------------
long Group::read_attribute_asynch (const std::string& a, bool fwd, long id)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  if (id == -1) {
    id = next_req_id();
  }
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; ++it) {
    if ((*it)->is_device() || fwd) {
      id = (*it)->read_attribute_asynch(a, fwd, id);
    }
  }
  push_async_request(id, fwd);
  return id;
} 
//-----------------------------------------------------------------------------
GroupAttrReplyList Group::read_attribute_reply (long req_id, long tmo)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  AsynchRequestDescIt r = arp.find(req_id);
  if (r == arp.end()) {
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].reason = CORBA::string_dup("API_BadAsynPollId");
    errors[0].desc = CORBA::string_dup("Invalid asynch. request identifier specified");
		errors[0].origin = CORBA::string_dup("Group::read_attribute_reply");
    throw DevFailed(errors);
  }
  GroupAttrReplyList reply;
  GroupAttrReplyList sub_reply;
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; ++it) {
    if ((*it)->is_device() || r->second) {
      sub_reply = (*it)->read_attribute_reply(req_id, tmo);
      if (sub_reply.empty() == false) {
        reply.insert(reply.end(), sub_reply.begin(), sub_reply.end());
      }
      if (sub_reply.has_failed_m) {
        reply.has_failed_m = true;
      }
      sub_reply.reset();
    }
  }
  arp.erase(r);
  return reply;
}
//-----------------------------------------------------------------------------
long Group::read_attributes_asynch (const std::vector<std::string>& al, bool fwd, long id)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  if (id == -1) {
    id = next_req_id();
  }
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; ++it) {
    if ((*it)->is_device() || fwd) {
      id = (*it)->read_attributes_asynch(al, fwd, id);
    }
  }
  push_async_request(id, fwd);
  return id;
} 
//-----------------------------------------------------------------------------
GroupAttrReplyList Group::read_attributes_reply (long req_id, long tmo)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  AsynchRequestDescIt r = arp.find(req_id);
  if (r == arp.end()) {
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].reason = CORBA::string_dup("API_BadAsynPollId");
    errors[0].desc = CORBA::string_dup("Invalid asynch. request identifier specified");
		errors[0].origin = CORBA::string_dup("Group::read_attributes_reply");
    throw DevFailed(errors);
  }
  GroupAttrReplyList reply;
  GroupAttrReplyList sub_reply;
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; ++it) {
    if ((*it)->is_device() || r->second) {
      sub_reply = (*it)->read_attributes_reply(req_id, tmo);
      if (sub_reply.empty() == false) {
        reply.insert(reply.end(), sub_reply.begin(), sub_reply.end());
      }
      if (sub_reply.has_failed_m) {
        reply.has_failed_m = true;
      }
      sub_reply.reset();
    }
  }
  arp.erase(r);
  return reply;
}
//-----------------------------------------------------------------------------
GroupReplyList Group::write_attribute (const DeviceAttribute& d, bool fwd)
{
  long id = write_attribute_asynch(d, fwd);
  return write_attribute_reply(id);
} 
//-----------------------------------------------------------------------------
long Group::write_attribute_asynch (const DeviceAttribute& d, bool fwd, long id)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  if (id == -1) {
    id = next_req_id();
  }
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; ++it) {
    if ((*it)->is_device() || fwd) {
      id = (*it)->write_attribute_asynch(d, fwd, id);
    }
  }
  push_async_request(id, fwd);
  return id;
}
//-----------------------------------------------------------------------------
GroupReplyList Group::write_attribute_reply (long req_id, long tmo)
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif
  AsynchRequestDescIt r = arp.find(req_id);
  if (r == arp.end()) {
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].reason = CORBA::string_dup("API_BadAsynPollId");
    errors[0].desc = CORBA::string_dup("Invalid asynch. request identifier specified");
		errors[0].origin = CORBA::string_dup("Group::write_attribute_reply");
    throw DevFailed(errors);
  }
  GroupReplyList reply;
  GroupReplyList sub_reply;
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end; ++it) {
    if ((*it)->is_device() || r->second) {
      sub_reply = (*it)->write_attribute_reply(req_id, tmo);
      if (sub_reply.empty() == false) {
        reply.insert(reply.end(), sub_reply.begin(), sub_reply.end());
      }
      if (sub_reply.has_failed_m) {
        reply.has_failed_m = true;
      }
      sub_reply.reset();
    }
  }
  arp.erase(r);
  return reply;
}
//-----------------------------------------------------------------------------
long Group::next_req_id (void) 
{
  asynch_req_id = (asynch_req_id++) % (long)0xFF;
  return asynch_req_id;
}
//-----------------------------------------------------------------------------
void Group::dump (int indent_level)
{
//#if defined(_LOCAL_DEBUGGING)
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);
#endif

  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (int i = 0; i < indent_level ; i++) {
    cerr << "\t";
  }
  cerr << "|- GROUP: " 
       << get_fully_qualified_name() 
       << " [" 
       << get_size_i(false) 
       << ":"
       << get_size_i(true) 
       << "]" 
       << endl;  
  for (; it != end ; ++it) { 
    (*it)->dump(indent_level + 1);
  }
//#endif
} 
//-----------------------------------------------------------------------------
void Group::dump (TangoSys_OMemStream& oms, int indent_level) 
{
#ifdef TANGO_GROUP_HAS_THREAD_SAFE_IMPL
  omni_mutex_lock guard(elements_mutex);  
#endif

  for (int i = 0; i < indent_level ; i++) {
    oms << "\t";
  }
  oms << "|- GROUP: " 
       << get_fully_qualified_name() 
       << " [" 
       << get_size_i(false) 
       << ":"
       << get_size_i(true) 
       << "]" 
       << endl;   
  GroupElementsIterator it = elements.begin();
  GroupElementsIterator end = elements.end();
  for (; it != end ; ++it) { 
    (*it)->dump(oms, indent_level + 1);
  }

  if (indent_level == 0) {
    oms << ends;
  }
} 
//-----------------------------------------------------------------------------
bool Group::is_device (void) 
{
  return false;
}
//-----------------------------------------------------------------------------
bool Group::is_group (void)
{
  return true;
}
//-----------------------------------------------------------------------------
void Group::push_async_request(long rid, bool forwarded)
{
  arp.insert(AsynchRequestDescVal(rid, forwarded));
}
//-----------------------------------------------------------------------------
void Group::pop_async_request(long rid)
{
  AsynchRequestDescIt r = arp.find(rid);
  if (r != arp.end()) {
    arp.erase(r);
  }
}

//=============================================================================
// class GroupDeviceElement
//=============================================================================
GroupDeviceElement::GroupDeviceElement (const std::string& name) 
  : GroupElement(name), dp(0)
{
  try {
    connect();
  }
  catch (...) {
    //- ignore error
  } 
}
//-----------------------------------------------------------------------------
GroupDeviceElement::~GroupDeviceElement () 
{
  disconnect();
  arp.clear();
}
//-----------------------------------------------------------------------------
DeviceProxy* GroupDeviceElement::get_device (const std::string& _n) 
{
  try {
    std::string n(_n);
    std::transform(n.begin(),n.end(),n.begin(),::tolower);
    if (name_equals(n)) {
      return dev_proxy();
    }
  } 
  catch (...) {
    //- ignore exception then return null
  }
  return 0;
} 
//-----------------------------------------------------------------------------
DeviceProxy* GroupDeviceElement::get_device (long idx) 
{
  return ((--idx) == 0) ? dev_proxy() : 0; 
} 
//-----------------------------------------------------------------------------
DeviceProxy* GroupDeviceElement::operator[] (long idx) 
{
  return get_device(idx); 
} 
//-----------------------------------------------------------------------------
DeviceProxy* GroupDeviceElement::connect (void)
{
  disconnect();
  try { 
    dp = new DeviceProxy(const_cast<std::string&>(get_name()));
    dp->set_transparency_reconnection(true);
  }
  catch (...) {
    disconnect();
    throw;
  }
  return dp;
}
//-----------------------------------------------------------------------------
void GroupDeviceElement::disconnect (void)
{
  //-TODO: how to handle pending asynch calls ?
  if (dp) {
    delete dp;
    dp = 0;
  }
}
//-----------------------------------------------------------------------------
bool GroupDeviceElement::ping (bool fwd)
{
  bool result = true;
  try 
  {
    dev_proxy()->ping();
  }
  catch (...) 
  {
    result = false;
  }
  return result;
} 
//-----------------------------------------------------------------------------
long GroupDeviceElement::command_inout_asynch (const std::string& c, bool fgt, bool fwd, long id)
{
  if (is_enabled() == false)
  {
    if (fgt == false) 
    {
      arp.insert(AsynchRequestRepValue(id, AsynchRequest(id, c, false)));
    }
  }
  else
  {
    try 
    {
      long req_id = dev_proxy()->command_inout_asynch(const_cast<std::string&>(c), fgt);
      if (fgt == false) 
      {
        arp.insert(AsynchRequestRepValue(id, AsynchRequest(req_id, c)));
      }
    }
    catch (const Tango::DevFailed& df) 
    {
      if (fgt == false) 
      {
        arp.insert(AsynchRequestRepValue(id, AsynchRequest(-1, c, df)));
      }
    }
    catch (...) 
    {
      if (fgt == false) 
      {
        //- create a pseudo devfailed
        Tango::DevErrorList errors(1);
		    errors.length(1);
		    errors[0].severity = Tango::ERR;
		    errors[0].desc = CORBA::string_dup("unknown error");
		    errors[0].reason = CORBA::string_dup("unknown exception caught");
		    errors[0].origin = CORBA::string_dup("GroupDeviceElement::command_inout_asynch");
        DevFailed df(errors);
        arp.insert(AsynchRequestRepValue(id, AsynchRequest(-1, c, df)));
      }
    }
  }
  return id;
}
//-----------------------------------------------------------------------------
long GroupDeviceElement::command_inout_asynch (const std::string& c, const DeviceData& d, bool fgt, bool fwd, long id)
{
  if (is_enabled() == false)
  {
    if (fgt == false) 
    {
      arp.insert(AsynchRequestRepValue(id, AsynchRequest(id, c, false)));
    }
  }
  else
  {
    try 
    {
      long req_id = dev_proxy()->command_inout_asynch(const_cast<std::string&>(c), const_cast<DeviceData&>(d), fgt);
      if (fgt == false) 
      {
        arp.insert(AsynchRequestRepValue(id, AsynchRequest(req_id, c)));
      }
    }
    catch (const Tango::DevFailed& df) 
    {
      if (fgt == false) 
      { 
        arp.insert(AsynchRequestRepValue(id, AsynchRequest(-1, c, df)));
      }
    }
    catch (...) 
    {
      if (fgt == false) 
      {
        //- create a pseudo devfailed
        Tango::DevErrorList errors(1);
		    errors.length(1);
		    errors[0].severity = Tango::ERR;
		    errors[0].desc = CORBA::string_dup("unknown error");
		    errors[0].reason = CORBA::string_dup("unknown exception caught");
		    errors[0].origin = CORBA::string_dup("GroupDeviceElement::command_inout_asynch");
        DevFailed df(errors);
        arp.insert(AsynchRequestRepValue(id, AsynchRequest(-1, c, df)));
      }
    }
  }
  return id;
}
//-----------------------------------------------------------------------------
GroupCmdReplyList GroupDeviceElement::command_inout_reply (long id, long tmo)
{
  GroupCmdReplyList rl;

  //- search actual asynch request id in this' repository
  AsynchRequestRepIterator it = arp.find(id);
  if (it == arp.end()) 
  {
    //- error description
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].reason = CORBA::string_dup("API_BadAsynPollId");
    errors[0].desc = CORBA::string_dup("Invalid asynch. request identifier specified");
		errors[0].origin = CORBA::string_dup("GroupDeviceElement::command_inout_reply");
    DevFailed df(errors);
    //- populate the returned vector
    rl.push_back(GroupCmdReply(get_name(), "unknown", df));
    return rl;
  }

  //- no reply if this group element is diabled
  if (is_enabled() == false) 
  {
    rl.push_back(GroupCmdReply(get_name(), it->second.obj_names[0], false));
    //- remove request from repository
    arp.erase(it);
    return rl;
  }

  //- if got error during asynch call then previously stored exception is the reply 
  if (it->second.rq_id == -1) 
  {
    rl.push_back(GroupCmdReply(get_name(), it->second.obj_names[0], it->second.rq_ex));
    //- remove request from repository
    arp.erase(it);
    return rl;
  }

  //- get reply from device
  try 
  {
    DeviceData dd = dev_proxy()->command_inout_reply(it->second.rq_id, tmo);
    rl.push_back(GroupCmdReply(get_name(), it->second.obj_names[0], dd));
  }
  catch (const Tango::DevFailed& df) 
  {
    rl.push_back(GroupCmdReply(get_name(), it->second.obj_names[0], df));
  }
  catch (...) 
  {
    //- create a pseudo devfailed
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].desc = CORBA::string_dup("unknown error");
		errors[0].reason = CORBA::string_dup("unknown exception caught");
		errors[0].origin = CORBA::string_dup("GroupDeviceElement::command_inout_reply");
    DevFailed df(errors);
    rl.push_back(GroupCmdReply(get_name(), it->second.obj_names[0], df));
  }

  //- remove request from repository
  arp.erase(it);

  return rl;
}
//-----------------------------------------------------------------------------
long GroupDeviceElement::read_attribute_asynch (const std::string& a, bool fwd, long id)
{
  if (is_enabled() == false)
  {
    arp.insert(AsynchRequestRepValue(id, AsynchRequest(id, a, false)));
  }
  else
  {
    try 
    {
      long req_id = dev_proxy()->read_attribute_asynch(const_cast<std::string&>(a));
      arp.insert(AsynchRequestRepValue(id, AsynchRequest(req_id, a)));
    }
    catch (const Tango::DevFailed& df) 
    {
      arp.insert(AsynchRequestRepValue(id, AsynchRequest(-1, a, df)));
    }
    catch (...) 
    {
      //- create a pseudo devfailed
      Tango::DevErrorList errors(1);
		  errors.length(1);
		  errors[0].severity = Tango::ERR;
		  errors[0].desc = CORBA::string_dup("unknown error");
		  errors[0].reason = CORBA::string_dup("unknown exception caught");
		  errors[0].origin = CORBA::string_dup("GroupDeviceElement::read_attribute_asynch");
      DevFailed df(errors);
      arp.insert(AsynchRequestRepValue(id, AsynchRequest(-1, a, df)));
    }
  }
  return id;
}
//-----------------------------------------------------------------------------
GroupAttrReplyList GroupDeviceElement::read_attribute_reply (long id, long tmo)
{
  GroupAttrReplyList rl;

  //- search actual asynch request id in this' repository
  AsynchRequestRepIterator it = arp.find(id);
  if (it == arp.end()) 
  {
    //- error description
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].reason = CORBA::string_dup("API_BadAsynPollId");
    errors[0].desc = CORBA::string_dup("Invalid asynch. request identifier specified");
		errors[0].origin = CORBA::string_dup("GroupDeviceElement::read_attribute_reply");
    DevFailed df(errors);
    //- populate the returned vector
    rl.push_back(GroupAttrReply(get_name(), "unknown", df));
    return rl;
  }

  //- no reply if this group element is diabled
  if (is_enabled() == false) 
  {
    rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[0], false));
    //- remove request from repository
    arp.erase(it);
    return rl;
  }

  //- if got error during asynch call then previously stored exception is the reply 
  if (it->second.rq_id == -1) 
  {
    rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[0], it->second.rq_ex));
    //- remove request from repository
    arp.erase(it);
    return rl;
  }

  //- get reply from device 
  try 
  {
    DeviceAttribute* da = dev_proxy()->read_attribute_reply(it->second.rq_id, tmo);
    if (da == 0) 
    {
      Tango::DevErrorList errors(1);
		  errors.length(1);
		  errors[0].severity = Tango::ERR;
		  errors[0].desc = CORBA::string_dup("internal error");
		  errors[0].reason = CORBA::string_dup("Tango::DeviceProxy::read_attribute_reply returned NULL");
		  errors[0].origin = CORBA::string_dup("GroupDeviceElement::read_attribute_reply");
      DevFailed df(errors);
      rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[0], df));
    }
    else 
    {
      rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[0], *da));
      delete da;
    }
  }
  catch (const Tango::DevFailed& df) 
  {
    rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[0], df));
  }
  catch (...) 
  {
    //- create a pseudo devfailed
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].desc = CORBA::string_dup("unknown error");
		errors[0].reason = CORBA::string_dup("unknown exception caught");
		errors[0].origin = CORBA::string_dup("GroupDeviceElement::read_attribute_reply");
    DevFailed df(errors);
    rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[0], df));
  }

  //- remove request from repository
  arp.erase(it);

  return rl;
}
//-----------------------------------------------------------------------------
long GroupDeviceElement::read_attributes_asynch (const std::vector<std::string>& al, bool fwd, long id)
{
  if (is_enabled() == false)
  {
    arp.insert(AsynchRequestRepValue(id, AsynchRequest(id, al, false)));
  }
  else
  {
    try 
    {
      long req_id = dev_proxy()->read_attributes_asynch(const_cast<std::vector<std::string>&>(al));
      arp.insert(AsynchRequestRepValue(id, AsynchRequest(req_id, al)));
    }
    catch (const Tango::DevFailed& df) 
    {
      arp.insert(AsynchRequestRepValue(id, AsynchRequest(-1, al, df)));
    }
    catch (...) 
    {
      //- create a pseudo devfailed
      Tango::DevErrorList errors(1);
		  errors.length(1);
		  errors[0].severity = Tango::ERR;
		  errors[0].desc = CORBA::string_dup("unknown error");
		  errors[0].reason = CORBA::string_dup("unknown exception caught");
		  errors[0].origin = CORBA::string_dup("GroupDeviceElement::read_attributes_asynch");
      DevFailed df(errors);
      arp.insert(AsynchRequestRepValue(id, AsynchRequest(-1, al, df)));
    }
  }
  return id;
}
//-----------------------------------------------------------------------------
GroupAttrReplyList GroupDeviceElement::read_attributes_reply (long id, long tmo)
{
  unsigned int a = 0;
  GroupAttrReplyList rl;

  //- search actual asynch request id in this' repository
  AsynchRequestRepIterator it = arp.find(id);
  if (it == arp.end()) {
    //- error description
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].reason = CORBA::string_dup("API_BadAsynPollId");
    errors[0].desc = CORBA::string_dup("Invalid asynch. request identifier specified");
		errors[0].origin = CORBA::string_dup("GroupDeviceElement::read_attribute_reply");
    DevFailed df(errors);
    //- populate the returned vector
    rl.push_back(GroupAttrReply(get_name(), "unknown", df));
    return rl;
  }

  //- no reply if this group element is diabled
  if (is_enabled() == false) 
  {
    for (a = 0; a < it->second.obj_names.size(); a++) 
    {
      rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[a], false));
    }
    //- remove request from repository
    arp.erase(it);
    return rl;
  }

  //- if got error during asynch call then previously stored exception is the reply 
  if (it->second.rq_id == -1) 
  {
    for (a = 0; a < it->second.obj_names.size(); a++) 
    {
      rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[a], it->second.rq_ex));
    }
    //- remove request from repository
    arp.erase(it);
    return rl;
  }

  //- get reply from device 
  try 
  {
    std::vector<DeviceAttribute>* dal = dev_proxy()->read_attributes_reply(it->second.rq_id, tmo);
    if (dal == 0) {
      Tango::DevErrorList errors(1);
		  errors.length(1);
		  errors[0].severity = Tango::ERR;
		  errors[0].desc = CORBA::string_dup("internal error");
		  errors[0].reason = CORBA::string_dup("Tango::DeviceProxy::read_attribute_reply returned NULL");
		  errors[0].origin = CORBA::string_dup("GroupDeviceElement::read_attribute_reply");
      DevFailed df(errors);
      for (a = 0; a < it->second.obj_names.size(); a++) {
        rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[a], df));
      }
    }
    else {
      for (a = 0; a < it->second.obj_names.size(); a++) {
        rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[a], (*dal)[a]));
      }
      delete dal;
    }
  }
  catch (const Tango::DevFailed& df) {
    for (a = 0; a < it->second.obj_names.size(); a++) {
      rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[a], df));
    }
  }
  catch (...) {
    //- create a pseudo devfailed
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].desc = CORBA::string_dup("unknown error");
		errors[0].reason = CORBA::string_dup("unknown exception caught");
		errors[0].origin = CORBA::string_dup("GroupDeviceElement::read_attribute_reply");
    DevFailed df(errors);
    for (a = 0; a < it->second.obj_names.size(); a++) {
      rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[a], df));
    }
  }

  //- remove request from repository
  arp.erase(it);

  return rl;
}
//-----------------------------------------------------------------------------
long GroupDeviceElement::write_attribute_asynch (const DeviceAttribute& d, bool fwd, long id)
{
  if (is_enabled() == false)
  {
    arp.insert(AsynchRequestRepValue(id, AsynchRequest(id, const_cast<DeviceAttribute&>(d).get_name(), false)));
  }
  else
  {
    try 
    {
      long req_id = dev_proxy()->write_attribute_asynch(const_cast<DeviceAttribute&>(d));
      arp.insert(AsynchRequestRepValue(id, AsynchRequest(req_id, const_cast<DeviceAttribute&>(d).get_name())));
    }
    catch (const Tango::DevFailed& df) 
    {
      arp.insert(AsynchRequestRepValue(id, AsynchRequest(-1, const_cast<DeviceAttribute&>(d).get_name(), df)));
    }
    catch (...) 
    {
      //- create a pseudo devfailed
      Tango::DevErrorList errors(1);
		  errors.length(1);
		  errors[0].severity = Tango::ERR;
		  errors[0].desc = CORBA::string_dup("unknown error");
		  errors[0].reason = CORBA::string_dup("unknown exception caught");
		  errors[0].origin = CORBA::string_dup("GroupDeviceElement::write_attribute_asynch");
      DevFailed df(errors);
      arp.insert(AsynchRequestRepValue(id, AsynchRequest(-1, const_cast<DeviceAttribute&>(d).get_name(), df)));
    }
  }
  return id;
}
//-----------------------------------------------------------------------------
GroupReplyList GroupDeviceElement::write_attribute_reply (long id, long tmo)
{
  GroupReplyList rl;
  //- search actual asynch request id in this' repository
  AsynchRequestRepIterator it = arp.find(id);
  if (it == arp.end()) 
  {
    //- error description
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].reason = CORBA::string_dup("API_BadAsynPollId");
    errors[0].desc = CORBA::string_dup("Invalid asynch. request identifier specified");
		errors[0].origin = CORBA::string_dup("GroupDeviceElement::write_attribute_reply");
    DevFailed df(errors);
    //- populate the returned vector
    rl.push_back(GroupReply(get_name(), "unknown", df));
    return rl;
  }

  //- no reply if this group element is diabled
  if (is_enabled() == false)
  {
    rl.push_back(GroupAttrReply(get_name(), it->second.obj_names[0], false));
    //- remove request from repository
    arp.erase(it);
    return rl;
  }

  //- if got error during asynch call then previously stored exception is the reply 
  if (it->second.rq_id == -1) 
  {
    rl.push_back(GroupReply(get_name(), it->second.obj_names[0], it->second.rq_ex));
    //- remove request from repository
    arp.erase(it);
    return rl;
  }

  //- get reply from device 
  try 
  {
    dev_proxy()->write_attribute_reply(it->second.rq_id, tmo);
    rl.push_back(GroupReply(get_name(), it->second.obj_names[0]));
  }
  catch (const Tango::DevFailed& df) 
  {
    rl.push_back(GroupReply(get_name(), it->second.obj_names[0], df));
  }
  catch (...) 
  {
    //- create a pseudo devfailed
    Tango::DevErrorList errors(1);
		errors.length(1);
		errors[0].severity = Tango::ERR;
		errors[0].desc = CORBA::string_dup("unknown error");
		errors[0].reason = CORBA::string_dup("unknown exception caught");
		errors[0].origin = CORBA::string_dup("GroupDeviceElement::write_attribute_reply");
    DevFailed df(errors);
    rl.push_back(GroupReply(get_name(), it->second.obj_names[0], df));
  }
  //- remove request from repository
  arp.erase(it);
  return rl;
}
//-----------------------------------------------------------------------------
void GroupDeviceElement::dump (int indent_level)
{
#if defined(_LOCAL_DEBUGGING)
  for (int i = 0; i < indent_level ; i++) {
    cerr << "\t";
  }
  cerr << "|- DEVICE: " << get_name() << endl;
#endif
} 
//-----------------------------------------------------------------------------
void GroupDeviceElement::dump (TangoSys_OMemStream& oms, int indent_level)
{
  for (int i = 0; i < indent_level ; i++) {
    oms << "\t";
  }
  oms << "|- DEVICE: " << get_name() << endl;
} 
//-----------------------------------------------------------------------------
bool GroupDeviceElement::is_device (void) 
{
  return true;
}
//-----------------------------------------------------------------------------
bool GroupDeviceElement::is_group (void)
{
  return false;
}
//-----------------------------------------------------------------------------
long GroupDeviceElement::get_size (bool fwd)
{
  return 1;
}

} // namespace Tango


