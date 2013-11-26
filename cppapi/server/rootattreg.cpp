static const char *RcsId = "$Id$";

//+==================================================================================================================
//
// file :               rootattreg.cpp
//
// description :        C++ source code for the RootAttRegistry and RootAttConfCallBack classes
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// Copyright (C) :      2013
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <rootattreg.h>

namespace Tango
{

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::push_event
//
// description :
//		Method called when root attribute configuration change event(s) are received
//		According to which attribute the event has been received for, update its config.
//
// argument :
//		in :
//			- ev : The event data
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::RootAttConfCallBack::push_event(Tango::AttrConfEventData *ev)
{
	try
	{
cout << "One attribute configuration change event received" << endl;
cout << "Attr name = " << ev->attr_name << endl;

		if (ev->err == true)
			cout << "The att change event is an error" << endl;
		else
		{

//
// The attribute name received in the callback follows the syntax tango:://host:port/dev/att
// We want only the dev/att part which starts after the first / after the first 8 chars (tango://)
//

			string::size_type pos;
			pos = ev->attr_name.find('/',8);
			string att_name = ev->attr_name.substr(pos + 1);

			{
				omni_mutex_lock oml(the_lock);
				map<string,struct NameFwdAttr>::iterator ite;
				ite = map_attrdesc.find(att_name);
				if (ite != map_attrdesc.end())
				{

//
// Classical event: Use the DeviceImpl ptr to the local device to set the att. environment but do not change att name
// and do not change att label if locally defined
//

					if (ite->second.fwd_attr == nullptr)
					{
						cout << "Change in Attribute config..........." << endl;
						map<string,DeviceImpl *>::iterator ite3;
						ite3 = local_dis.find(ite->second.local_name);
						if (ite3 == local_dis.end())
						{
							cerr << "RootAttRegistry::RootAttConfCallBack::push_event(): Device " ;
							cerr << ite->second.local_name << " not found in map (local_dis)! Map corrupted?" << endl;
						}
						else
						{
							Device_5Impl *the_dev = static_cast<Device_5Impl *>(ite3->second);
							if (the_dev != nullptr)
							{
								FwdAttrConfEventData *ev_fwd = static_cast<FwdAttrConfEventData *>(ev);
								AttributeConfigList_5 conf_list(1,1,const_cast<AttributeConfig_5 *>(ev_fwd->get_fwd_attr_conf()));

//
// The attribute name, root_attr_name and label are local values
//

								conf_list[0].name = ite->second.local_att_name.c_str();
								conf_list[0].root_attr_name = ite->first.c_str();
								if (ite->second.local_label.empty() == false)
									conf_list[0].label = ite->second.local_label.c_str();

cout << "Calling set_attribute_config_5" << endl;
								the_dev->set_attribute_config_5(conf_list,ci);
							}
							else
							{
								cerr << "RootAttRegistry::RootAttConfCallBack::push_event(): Device " ;
								cerr << ite->second.local_name << " has a null pointer in DeviceImpl * map (local_dis)" << endl;
							}
						}
					}
					else
					{

//
// Synchronous event due to subscription or event received after a successfull re-connection if the server
// was started while the root device was off
//

						if (ite->second.fwd_attr->get_err_kind() == FWD_ROOT_DEV_NOT_STARTED)
						{
							cout << "After re-connection" << endl;
							map<string,DeviceImpl *>::iterator ite3;
							ite3 = local_dis.find(ite->second.local_name);
							if (ite3 == local_dis.end())
							{
								cerr << "RootAttRegistry::RootAttConfCallBack::push_event(): Device " ;
								cerr << ite->second.local_name << " not found in map (local_dis)! Map corrupted?" << endl;
							}
							else
							{
								if (ite->second.local_label.empty() == false)
									ev->attr_conf->label = ite->second.local_label;

								Device_5Impl *the_dev = static_cast<Device_5Impl *>(ite3->second);
								Attribute &the_fwd_att = the_dev->get_device_attr()->get_attr_by_name(ite->second.local_att_name.c_str());
								static_cast<FwdAttribute &>(the_fwd_att).set_att_config(ev->attr_conf);
								MultiAttribute *m_att = the_dev->get_device_attr();
								m_att->update(the_fwd_att);

/*								DeviceImpl *the_dev = ite3->second;
								the_dev->add_attribute(ite->second.fwd_attr); */
								ite->second.fwd_attr = nullptr;

								the_dev->rem_wrong_fwd_att(att_name);
								the_dev->set_run_att_conf_loop(true);
							}
						}
						else
						{
							ite->second.fwd_attr->init_conf(ev);
							ite->second.fwd_attr = nullptr;
						}
					}
				}
				else
				{
					cerr << "RootAttRegistry::RootAttConfCallBack::push_event(): " ;
					cerr << "Root attribute " << att_name << " not found in map (map_attrdesc)! Map corrupted?" << endl;
				}
			}
		}
	}
	catch (Tango::DevFailed &e)
	{
		cerr << "RootAttRegistry::RootAttConfCallBack::push_event(): Exception!" ;
		Tango::Except::print_exception(e);
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::add_att
//
// description :
//		Add a attribute object (still in its FwdAttr form) in the map of subscribed object
//
// argument :
//		in :
//			- root_att_name : The attribute name (dev_name/att_name)
//			- local_dev_name : The local device name
//			- local_att_name : The local attribute name
//			- att: The attribute pointer when still a FwdAttr object
//			- dev : The local device pointer (DeviceImpl *)
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::RootAttConfCallBack::add_att(string &root_att_name,string &local_dev_name,
													string &local_att_name,FwdAttr *att,DeviceImpl *dev)
{
	DeviceImpl *the_local_dev;
	try
	{
		the_local_dev = nullptr;

		struct NameFwdAttr nf;
		nf.local_name = local_dev_name;
		nf.local_att_name = local_att_name;
		nf.fwd_attr = att;

		vector<AttrProperty> &def_user_prop = att->get_user_default_properties();
		vector<AttrProperty>::iterator pos;
		for (pos = def_user_prop.begin();pos != def_user_prop.end();++pos)
		{
            if (pos->get_name() == "label")
            {
				nf.local_label = pos->get_value();
                break;
			}
		}

		map<string,DeviceImpl *>::iterator ite;
		ite = local_dis.find(local_dev_name);
		if (ite == local_dis.end())
			the_local_dev = dev;

		{
			omni_mutex_lock oml(the_lock);
			map_attrdesc.insert({root_att_name,nf});
			if (the_local_dev != nullptr)
				local_dis.insert({local_dev_name,the_local_dev});
		}
	}
	catch (DevFailed &e) {}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::remove_att
//
// description :
//		Remove a attribute object (still in its FwdAttr form) in the map of subscribed object
//
// argument :
//		in :
//			- root_att_name : The attribute name (dev_name/att_name)
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::RootAttConfCallBack::remove_att(string &root_att_name)
{

//
// Get local dev name and check if the same local dev name is used for other forwarded att
//

	omni_mutex_lock oml(the_lock);
	map<string,NameFwdAttr>::iterator ite;
	ite = map_attrdesc.find(root_att_name);

	if (ite != map_attrdesc.end())
	{
		string local_dev_name = ite->second.local_name;
		map_attrdesc.erase(ite);

		bool used_elsewhere = false;
		map<string,NameFwdAttr>::iterator pos;

		for (pos = map_attrdesc.begin();pos != map_attrdesc.end();++pos)
		{
			if (pos->second.local_name == local_dev_name)
			{
				used_elsewhere = true;
				break;
			}
		}

//
// If the local device name is not referenced in any entry in the map_attrdesc map, we can remove the
// corresponding DeviceImpl entry
//

		if (used_elsewhere == false)
		{
			map<string,DeviceImpl *>::iterator ite_dp = local_dis.find(local_dev_name);
			local_dis.erase(ite_dp);
		}
	}
	else
	{
		string desc("Root attribute ");
		desc = desc + root_att_name + " not found in class map!";

		Except::throw_exception(API_AttrNotFound,desc,"RootAttConfCallBack::remove_att");
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::clear_attrdesc
//
// description :
//
//
// argument :
//		in :
//			- root_att_name : The root attribute name
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::RootAttConfCallBack::clear_attrdesc(string &root_att_name)
{
	map<string,NameFwdAttr>::iterator ite;
	ite = map_attrdesc.find(root_att_name);
	if (ite != map_attrdesc.end())
	{
		ite->second.fwd_attr = nullptr;
	}
	else
	{
		string desc("Root attribute ");
		desc = desc + root_att_name + " not found in class map!";

		Except::throw_exception(API_AttrNotFound,desc,"RootAttConfCallBack::clear_attrdesc");
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::is_root_att_in_map
//
// description :
//		Check if one attribute is defined in the map
//
// argument :
//		in :
//			- root_att_name : The attribute name (dev_name/att_name)
//
// return :
//		True is the attribute is in the map
//
//--------------------------------------------------------------------------------------------------------------------


bool RootAttRegistry::RootAttConfCallBack::is_root_att_in_map(string &root_att_name)
{
	bool ret = false;

	omni_mutex_lock oml(the_lock);
	map<string,NameFwdAttr>::iterator ite;
	ite = map_attrdesc.find(root_att_name);
	if (ite != map_attrdesc.end())
		ret = true;

	return ret;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::count_root_dev
//
// description :
//		Count how many times the root device with name given as parameter is used as root device name by all the
//		registred forwarded attributes
//
// argument :
//		in :
//			- root_dev_name : The root device name
//
// return :
//		Number of time the root device name is used in map_attrdesc key
//
//--------------------------------------------------------------------------------------------------------------------

int RootAttRegistry::RootAttConfCallBack::count_root_dev(string &root_dev_name)
{
	int ret = 0;
	map<string,NameFwdAttr>::iterator pos;

	for (pos = map_attrdesc.begin();pos != map_attrdesc.end();++pos)
	{
		string::size_type p = pos->first.rfind('/');
		string key_dev = pos->first.substr(0,p);

		if (key_dev == root_dev_name)
			ret++;
	}

	return ret;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::null_device_impl
//
// description :
//
//
// argument :
//		in :
//			- local_dev_name : The local device name
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::RootAttConfCallBack::null_device_impl(string &local_dev_name)
{

}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::update_device_impl
//
// description :
//		Update local device pointer in map
//
// argument :
//		in :
//			- local_dev_name : The local device name
//			- local_dev : The new local device pointer
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::RootAttConfCallBack::update_device_impl(string &local_dev_name,DeviceImpl *local_dev)
{
	omni_mutex_lock oml(the_lock);
	map<string,DeviceImpl *>::iterator ite;
	ite = local_dis.find(local_dev_name);
	if (ite != local_dis.end())
	{
		ite->second = local_dev;
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::add_root_att
//
// description :
//		Add a new root attribute. This means create a device proxy to the device (if not already done) and
//		subscribe to the root attribute configuration change event
//
// argument :
//		in :
//			- device_name : The device name
//			- att_name : The attribute name
//			- local_dev_name : The local device name
//			- local_att_name : The local attribute name
//			- attdesc : The attribute object when still a FwdAttr object
//			- dev : Device pointer
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::add_root_att(string &device_name,string &att_name,string &local_dev_name,string &local_att_name,
								   FwdAttr *attdesc,DeviceImpl *dev)
{
	DeviceProxy *the_dev;
	map<string,DeviceProxy *>::iterator ite;
	ite = dps.find(device_name);
	if (ite == dps.end())
	{
		the_dev = new DeviceProxy(device_name);
		if (the_dev->get_idl_version() < 5)
		{
			attdesc->set_err_kind(FWD_TOO_OLD_ROOT_DEVICE);
			delete the_dev;

			string desc("The root device ");
			desc = desc + device_name + " is too old to support forwarded attribute. It requires IDL >= 5";
			Except::throw_exception(API_AttrNotAllowed,desc,"RootAttRegistry::add_root_att");
		}
		dps.insert({device_name,the_dev});
	}
	else
		the_dev = ite->second;

//
// When the device is not there, subscribing to att change event throws one exception
// and it does call the callback with error flag set. Because we want to know that the root device is not yet
// ready, implement a two steps subscription
//

	string a_name = device_name + '/' + att_name;
	bool already_there = cbp.is_root_att_in_map(a_name);
	if (already_there == false)
	{
		cbp.add_att(a_name,local_dev_name,local_att_name,attdesc,dev);
		int event_id;

		try
		{
			event_id = the_dev->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,&cbp);
			map_event_id.insert({a_name,event_id});
		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),API_AttrNotFound) == 0)
				attdesc->set_err_kind(FWD_WRONG_ATTR);
			else if (::strcmp(e.errors[0].reason.in(),API_BadConfigurationProperty) == 0)
				attdesc->set_err_kind(FWD_ROOT_DEV_NOT_STARTED);
			else
				attdesc->set_err_kind(FWD_WRONG_DEV);

			event_id = the_dev->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,&cbp,true);
			map_event_id.insert({a_name,event_id});

			Tango::Except::re_throw_exception(e,"API_DummyException","nothing","RootAttRegistry::add_root_att");
		}
	}
	else
	{
		attdesc->set_err_kind(FWD_DOUBLE_USED);

		string desc("It's not supported to have in the same device server process two times the same root attribute (");
		desc = desc + a_name + ")";
		Except::throw_exception(API_AttrNotAllowed,desc,"RootAttRegistry::add_root_att");
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::clear_attrdesc
//
// description :
//
//
// argument :
//		in :
//			- device_name : The device name
//			- att_name : The attribute name
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::clear_attrdesc(string &dev_name,string &att_name)
{
	string full_att_name = dev_name + '/' + att_name;
	cbp.clear_attrdesc(full_att_name);
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::remove_root_att
//
// description :
//		Remove a forwarded attribute from the registry
//
// argument :
//		in :
//			- root_dev_name : The root device name
//			- root_att_name : The root attribute name
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::remove_root_att(string &root_dev_name,string &root_att_name)
{
	int co = cbp.count_root_dev(root_dev_name);
cout << "Root dev name " << root_dev_name << " used " << co << " times" << endl;
	string full_root_att_name = root_dev_name + '/' + root_att_name;
	if (co != 0)
		cbp.remove_att(full_root_att_name);

//
// If the root device is used only once, unsubscribe from the event and remove its device proxy
//

	map<string,int>::iterator ite = map_event_id.find(full_root_att_name);
	if (ite != map_event_id.end())
	{
		map<string,DeviceProxy *>::iterator pos = dps.find(root_dev_name);
		if (pos != dps.end())
		{
cout << "Calling unsubscribe_event" << endl;
			pos->second->unsubscribe_event(ite->second);

			map_event_id.erase(ite);
			if (co == 1)
			{
cout << "Deleting DP for " << pos->first << endl;
				delete pos->second;
				dps.erase(pos);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::get_root_att_dp
//
// description :
//		Get the DeviceProxy instance used in forwarded attribute from a root device name
//		Throws exception in case of not
//
// argument :
//		in :
//			- device_name : The device name
//
// return :
//		The DeviceProxy pointer.
//
//--------------------------------------------------------------------------------------------------------------------

DeviceProxy *RootAttRegistry::get_root_att_dp(string &device_name)
{
	map<string,DeviceProxy *>::iterator ite;
	ite = dps.find(device_name);
	if (ite == dps.end())
	{
		stringstream ss;
		ss << device_name << " not registered in map of root attribute devices!";
		Except::throw_exception(API_FwdAttrInconsistency,ss.str(),"RootAttRegistry::get_root_att_dp");
	}

	return ite->second;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::get_local_att_name
//
// description :
//		Get the local attribute name from a root attribute name
//
// argument :
//		in :
//			- root_name : The device name
//
// return :
//		The local attribute name
//
//--------------------------------------------------------------------------------------------------------------------

string RootAttRegistry::RootAttConfCallBack::get_local_att_name(string &root_name)
{
	map<string,struct NameFwdAttr>::iterator ite;
	ite = map_attrdesc.find(root_name);
	if (ite == map_attrdesc.end())
	{
		stringstream ss;
		ss << root_name << " not registered in map of root attribute!";
		Except::throw_exception(API_FwdAttrInconsistency,ss.str(),"RootAttRegistry::get_local_att_name");
	}

	string loc_name = ite->second.local_name + '/' + ite->second.local_att_name;
	return loc_name;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::update_label
//
// description :
//		Update the attribute label stored in map_attrdesc map
//
// argument :
//		in :
//			- root_name : The full root attribute name
//			- new_label : The new label
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::RootAttConfCallBack::update_label(string &root_name,string &new_label)
{
	map<string,struct NameFwdAttr>::iterator ite;
	ite = map_attrdesc.find(root_name);

	if (ite != map_attrdesc.end())
	{
		ite->second.local_label = new_label;
	}
	else
	{
		stringstream ss;
		ss << root_name << " not registered in map of root attribute!";
		Except::throw_exception(API_FwdAttrInconsistency,ss.str(),"RootAttRegistry::update_label");
	}
}

} // End of Tango namespace
