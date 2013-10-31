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
		cout << "Event name = " << ev->event << endl;
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
					if (ite->second.fwd_attr == nullptr)
					{
						cout << "Change in Attribute config..........." << endl;
						map<string,DeviceProxy *>::iterator ite3;
						ite3 = local_dps.find(ite->second.local_name);
						if (ite3 == local_dps.end())
						{
// TODO: What to do here ???
						}
						else
						{
							AttributeInfoListEx aile;
							aile.push_back(*(ev->attr_conf));

							aile[0].name = ite->second.local_att_name;

cout <<  "Calling set_attribute_config on device " << ite3->second->name() << endl;
							ite3->second->set_attribute_config(aile);
						}
					}
					else
						ite->second.fwd_attr->init_conf(ev->attr_conf);
				}
				else
				{
// TODO: What to do here ????
				}
			}
		}
	}
	catch (Tango::DevFailed &e)
	{
		Tango::Except::print_exception(e);
// TODO: What to do here ??? Is exception possible???
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
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::RootAttConfCallBack::add_att(string &root_att_name,string &local_dev_name,
													string &local_att_name,FwdAttr *att)
{
	DeviceProxy *the_local_dev;
	try
	{
		the_local_dev = nullptr;

		struct NameFwdAttr nf;
		nf.local_name = local_dev_name;
		nf.local_att_name = local_att_name;
		nf.fwd_attr = att;

		map<string,DeviceProxy *>::iterator ite;
		ite = local_dps.find(local_dev_name);
		if (ite == local_dps.end())
			the_local_dev = new DeviceProxy(local_dev_name);

		{
			omni_mutex_lock oml(the_lock);
			map_attrdesc.insert({root_att_name,nf});
			if (the_local_dev != nullptr)
				local_dps.insert({local_dev_name,the_local_dev});
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
// corresponding Deviceproxy entry
//

for (const auto &elem:local_dps)
	cout << "Local dps key = " << elem.first << endl;
		if (used_elsewhere == false)
		{
			map<string,DeviceProxy *>::iterator ite_dp = local_dps.find(local_dev_name);
cout << "Deleting DP for " << ite_dp->first << endl;
 			delete ite_dp->second;
			local_dps.erase(ite_dp);
		}

	}
	else
	{
// TODO: What to do here??
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
// TODO: What to do here ????????
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
//		Count how many times the root device with name given as parameter is used as root device name
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
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::add_root_att(string &device_name,string &att_name,string &local_dev_name,string &local_att_name,
								   FwdAttr *attdesc)
{
	try
	{
		DeviceProxy *the_dev;
		map<string,DeviceProxy *>::iterator ite;
		ite = dps.find(device_name);
		if (ite == dps.end())
		{
			the_dev = new DeviceProxy(device_name);
			dps.insert({device_name,the_dev});
		}
		else
			the_dev = ite->second;

		string a_name = device_name + '/' + att_name;
		bool already_there = cbp.is_root_att_in_map(a_name);
		if (already_there == false)
		{
			cbp.add_att(a_name,local_dev_name,local_att_name,attdesc);
			int event_id = the_dev->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,&cbp,true);
			map_event_id.insert({a_name,event_id});
		}
		else
		{
// TODO: What to do here ???
		}
	}
	catch (DevFailed &e)
	{
// TODO: What to do here ???
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
	cbp.remove_att(full_root_att_name);

//
// If the root device is used only once, unsubscribe from the event and remove its device proxy
//

	if (co == 1)
	{
		map<string,int>::iterator ite = map_event_id.find(full_root_att_name);
		if (ite != map_event_id.end())
		{
			map<string,DeviceProxy *>::iterator pos = dps.find(root_dev_name);
			if (pos != dps.end())
			{
cout << "Calling unsubscribe_event" << endl;
				pos->second->unsubscribe_event(ite->second);

				map_event_id.erase(ite);
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
//
// argument :
//		in :
//			- device_name : The device name
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

} // End of Tango namespace
