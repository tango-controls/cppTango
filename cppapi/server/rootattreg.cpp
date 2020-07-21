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
// Copyright (C) :      2013,2014,2015
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
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <rootattreg.h>
#include <eventsupplier.h>

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
//cout << "One attribute configuration change event received" << std::endl;
//cout << "Attr name = " << ev->attr_name << std::endl;
//cout << "Event name = " << ev->event << std::endl;
//cout << "Error flag = " << std::boolalpha << ev->err << std::endl;

		if (ev->err == false)
		{
			std::string att_name = ev->attr_name;

			{
				omni_mutex_lock oml(the_lock);
				std::map<std::string,struct NameFwdAttr>::iterator ite;
				ite = map_attrdesc.find(att_name);
				if (ite != map_attrdesc.end())
				{
					if (ite->second.fwd_attr == nullptr || ite->second.fwd_attr_cl == nullptr)
					{
//
// Event received while everything is OK for the fwd attribute
//

						std::map<std::string,DeviceImpl *>::iterator ite3;
						ite3 = local_dis.find(ite->second.local_name);
						if (ite3 == local_dis.end())
						{
							std::cerr << "RootAttRegistry::RootAttConfCallBack::push_event(): Device " ;
							std::cerr << ite->second.local_name << " not found in map (local_dis)! Map corrupted?" << std::endl;
						}
						else
						{
							Device_5Impl *the_dev = static_cast<Device_5Impl *>(ite3->second);
							if (the_dev != nullptr)
							{

//
// If the callback is executed due to a synchronous call after a re-connection (ptr null), the info we received do
// not contain a AttributeConfig structure. In this case, create one from the AttributeInfoEx
//

								FwdAttrConfEventData *ev_fwd = static_cast<FwdAttrConfEventData *>(ev);
								AttributeConfig_5 *ptr = const_cast<AttributeConfig_5 *>(ev_fwd->get_fwd_attr_conf());
								if (ptr == nullptr)
								{
									ptr = AttributeConfigList_5::allocbuf(1);
									ApiUtil::AttributeInfoEx_to_AttributeConfig(ev->attr_conf,ptr);
								}

								AttributeConfigList_5 conf_list(1,1,ptr,false);

//
// The attribute name, root_attr_name and label are local values
//

								conf_list[0].name = ite->second.local_att_name.c_str();
								conf_list[0].root_attr_name = ite->first.c_str();
								if (ite->second.local_label.empty() == false)
									conf_list[0].label = ite->second.local_label.c_str();

								the_dev->set_attribute_config_5(conf_list,ci);
							}
							else
							{
								std::cerr << "RootAttRegistry::RootAttConfCallBack::push_event(): Device " ;
								std::cerr << ite->second.local_name << " has a null pointer in DeviceImpl * map (local_dis)" << std::endl;
							}
						}
					}
					else
					{

//
// Synchronous event due to subscription or event received after a successfull re-connection if the server
// was started while the root device was off
//

//cout << "Err kind = " << ite->second.fwd_attr->get_err_kind() << std::endl;
						if (ite->second.fwd_attr->get_err_kind() == FWD_ROOT_DEV_NOT_STARTED)
						{
							std::map<std::string,DeviceImpl *>::iterator ite3;
							ite3 = local_dis.find(ite->second.local_name);
							if (ite3 == local_dis.end())
							{
								std::cerr << "RootAttRegistry::RootAttConfCallBack::push_event(): Device " ;
								std::cerr << ite->second.local_name << " not found in map (local_dis)! Map corrupted?" << std::endl;
							}
							else
							{
								std::string::size_type pos = att_name.rfind('/');
								std::string root_dev_name = att_name.substr(0,pos);

								bool rel_ok = rar->check_root_dev_release(root_dev_name);

								Device_5Impl *the_dev = static_cast<Device_5Impl *>(ite3->second);
								if (rel_ok == true)
								{
									if (ite->second.local_label.empty() == false)
										ev->attr_conf->label = ite->second.local_label;

									the_dev->add_attribute(ite->second.fwd_attr);

									Attribute &the_fwd_att = the_dev->get_device_attr()->get_attr_by_name(ite->second.local_att_name.c_str());
									static_cast<FwdAttribute &>(the_fwd_att).get_impl().set_att_config(ev->attr_conf);

									MultiAttribute *m_att = the_dev->get_device_attr();
									m_att->update(the_fwd_att,ite->second.local_name);

									ite->second.fwd_attr = nullptr;

									the_dev->rem_wrong_fwd_att(att_name);
									the_dev->set_run_att_conf_loop(true);

//
// Now, we can really start polling this attribute (if required)
//

									std::vector<std::string> &poll_attr_list = the_dev->get_polled_attr();
									std::string local_att_lower(ite->second.local_att_name);
									std::transform(local_att_lower.begin(),local_att_lower.end(),local_att_lower.begin(),::tolower);
									std::vector<std::string>::iterator pos = find(poll_attr_list.begin(),poll_attr_list.end(),local_att_lower);
									if (pos != poll_attr_list.end())
									{
										DevVarLongStringArray send;
										send.lvalue.length(1);
										send.svalue.length(3);

										send.svalue[0] = Tango::string_dup(the_dev->get_name().c_str());
										send.svalue[1] = Tango::string_dup("attribute");
										send.svalue[2] = Tango::string_dup((*pos).c_str());

										std::stringstream ss;
										long upd;
										ss << *(pos + 1);
										ss >> upd;
										if (upd < 0)
											upd = -upd;

										send.lvalue[0] = upd;

										DServer *adm_dev = Util::instance()->get_dserver_device();
										adm_dev->add_obj_polling(&send,false);
									}
								}
								else
								{
									std::string root_att_name = att_name.substr(pos + 1);
									the_dev->update_wrong_conf_att(att_name,FWD_TOO_OLD_ROOT_DEVICE);
									the_dev->set_run_att_conf_loop(true);
								}
							}
						}
						else
						{

//
// Classical event due to synchronous call during subscription
//

							ite->second.fwd_attr_cl->init_conf(ev);
							ite->second.fwd_attr_cl = nullptr;
						}
					}
				}
				else
				{
					std::cerr << "RootAttRegistry::RootAttConfCallBack::push_event(): " ;
					std::cerr << "Root attribute " << att_name << " not found in map (map_attrdesc)! Map corrupted?" << std::endl;
				}
			}
		}
	}
	catch (Tango::DevFailed &e)
	{
		std::cerr << "RootAttRegistry::RootAttConfCallBack::push_event(): Exception!" ;
		Tango::Except::print_exception(e);
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttUserCallBack::push_event
//
// description :
//		Method called when root attribute event(s) are received (except att conf change event)
//
// argument :
//		in :
//			- ev : The event data
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::RootAttUserCallBack::push_event(Tango::EventData *ev)
{
	try
	{
//cout << "One event received" << std::endl;
//cout << "Attr name = " << ev->attr_name << std::endl;

		ZmqEventSupplier *zes = Util::instance()->get_zmq_event_supplier();

//
// Get local device name from event name
//

		std::string local_name = rar->get_local_att_name(ev->attr_name);
		std::string::size_type pos = local_name.rfind('/');
		std::string local_dev_name = local_name.substr(0,pos);
		std::string local_att_name = local_name.substr(pos + 1);

		DeviceImpl *dev = rar->get_local_dev(local_dev_name);

		EventSupplier::SuppliedEventData ad;
		::memset(&ad,0,sizeof(ad));

		std::string event_name = EVENT_COMPAT_IDL5 + ev->event;

		if (ev->err == true)
		{
			DevFailed df(ev->errors);
			zes->push_event(dev,event_name,dummy_vs,dummy_vd,dummy_vs,dummy_vl,ad,local_att_name,&df,true);
		}
		else
		{
			FwdEventData *ev_fwd = static_cast<FwdEventData *>(ev);
			const AttributeValue_5 *ptr = ev_fwd->get_av_5();
			zmq::message_t *zmq_mess_ptr = ev_fwd->get_zmq_mess_ptr();

			if (ptr != nullptr || zmq_mess_ptr != nullptr)
			{

//
// Now, forward the event
//


				if (ptr != nullptr)
					ad.attr_val_5 = ptr;
				else
					ad.zmq_mess = zmq_mess_ptr;

				zes->push_event(dev,event_name,dummy_vs,dummy_vd,dummy_vs,dummy_vl,ad,local_att_name,nullptr,true);
			}
		}
	}
	catch (Tango::DevFailed &e)
	{
		std::cerr << "RootAttRegistry::RootAttUserCallBack::push_event(): Exception!" ;
		Tango::Except::print_exception(e);
	}
}

void RootAttRegistry::RootAttUserCallBack::push_event(Tango::DataReadyEventData *ev)
{
	try
	{
		ZmqEventSupplier *zes = Util::instance()->get_zmq_event_supplier();

//
// First, extract root att name (root_dev_name/att_name) from the received attribute name
//

		std::string::size_type pos = ev->attr_name.find("//");
		pos = pos + 2;
		pos = ev->attr_name.find('/',pos);
		pos = pos + 1;
		std::string::size_type pos_end = ev->attr_name.rfind('.');
		std::string root_att_name = ev->attr_name.substr(pos,pos_end - pos);

		std::string local_name = rar->get_local_att_name(root_att_name);
		pos = local_name.rfind('/');
		std::string local_dev_name = local_name.substr(0,pos);
		std::string local_att_name = local_name.substr(pos + 1);

		DeviceImpl *dev = rar->get_local_dev(local_dev_name);

		EventSupplier::SuppliedEventData ad;
		::memset(&ad,0,sizeof(ad));

		if (ev->err == true)
		{
			DevFailed df(ev->errors);
			zes->push_event(dev,ev->event,dummy_vs,dummy_vd,dummy_vs,dummy_vl,ad,local_att_name,&df,true);
		}
		else
		{
			dev->push_data_ready_event(local_att_name,ev->ctr);
		}
	}
	catch (Tango::DevFailed &e)
	{
		std::cerr << "RootAttRegistry::RootAttUserCallBack::push_event(): Exception!" ;
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

void RootAttRegistry::RootAttConfCallBack::add_att(std::string &root_att_name,std::string &local_dev_name,
													std::string &local_att_name,FwdAttr *att,DeviceImpl *dev)
{
	DeviceImpl *the_local_dev;
	try
	{
		the_local_dev = nullptr;

		struct NameFwdAttr nf;
		nf.local_name = local_dev_name;
		nf.local_att_name = local_att_name;
		nf.fwd_attr = new FwdAttr(*att);
		nf.fwd_attr_cl = att;

		std::vector<AttrProperty> &def_user_prop = att->get_user_default_properties();
		std::vector<AttrProperty>::iterator pos;
		for (pos = def_user_prop.begin();pos != def_user_prop.end();++pos)
		{
            if (pos->get_name() == "label")
            {
				nf.local_label = pos->get_value();
                break;
			}
		}
		if (pos == def_user_prop.end())
			nf.local_label = local_att_name;

		std::map<std::string,DeviceImpl *>::iterator ite;
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

void RootAttRegistry::RootAttConfCallBack::remove_att(std::string &root_att_name)
{
	omni_mutex_lock oml(the_lock);

//
// Get local dev name and check if the same local dev name is used for other forwarded att
//

	std::map<std::string,NameFwdAttr>::iterator ite;
	ite = map_attrdesc.find(root_att_name);

	if (ite != map_attrdesc.end())
	{
		std::string local_dev_name = ite->second.local_name;
		map_attrdesc.erase(ite);

		bool used_elsewhere = false;
		std::map<std::string,NameFwdAttr>::iterator pos;

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
			std::map<std::string,DeviceImpl *>::iterator ite_dp = local_dis.find(local_dev_name);
			local_dis.erase(ite_dp);
		}
	}
	else
	{
		std::string desc("Root attribute ");
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

void RootAttRegistry::RootAttConfCallBack::clear_attrdesc(std::string &root_att_name)
{
	std::map<std::string,NameFwdAttr>::iterator ite;
	ite = map_attrdesc.find(root_att_name);
	if (ite != map_attrdesc.end())
	{
		ite->second.fwd_attr = nullptr;
	}
	else
	{
		std::string desc("Root attribute ");
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
//		Check if one attribute is defined in the map as one of the root attribute
//
// argument :
//		in :
//			- root_att_name : The attribute name (dev_name/att_name)
//
// return :
//		True is the attribute is in the map
//
//--------------------------------------------------------------------------------------------------------------------


bool RootAttRegistry::RootAttConfCallBack::is_root_att_in_map(std::string &root_att_name)
{
	bool ret = false;

	omni_mutex_lock oml(the_lock);
	std::map<std::string,NameFwdAttr>::iterator ite;
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

int RootAttRegistry::RootAttConfCallBack::count_root_dev(std::string &root_dev_name)
{
	int ret = 0;
	std::map<std::string,NameFwdAttr>::iterator pos;

	for (pos = map_attrdesc.begin();pos != map_attrdesc.end();++pos)
	{
		std::string::size_type p = pos->first.rfind('/');
		std::string key_dev = pos->first.substr(0,p);

		if (key_dev == root_dev_name)
			ret++;
	}

	return ret;
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

void RootAttRegistry::RootAttConfCallBack::update_device_impl(std::string &local_dev_name,DeviceImpl *local_dev)
{
	omni_mutex_lock oml(the_lock);
	std::map<std::string,DeviceImpl *>::iterator ite;
	ite = local_dis.find(local_dev_name);
	if (ite != local_dis.end())
	{
		ite->second = local_dev;
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::update_err_kind
//
// description :
//		Update error type in map for one specific root attribute
//
// argument :
//		in :
//			- root_att_name : The root attribute name
//			- err : The new error code
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::RootAttConfCallBack::update_err_kind(std::string &root_att_name,FwdAttError err)
{
	omni_mutex_lock oml(the_lock);
	std::map<std::string,struct NameFwdAttr>::iterator ite;
	ite = map_attrdesc.find(root_att_name);
	if (ite != map_attrdesc.end())
	{
		ite->second.fwd_attr->set_err_kind(err);
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::is_root_dev_not_started_err
//
// description :
//		Check if there is some forwarded attribute with error set to FWD_ROOT_DEV_NOT_STARTED
//
// return :
//		True if one of the process fwd attribute has its error set to root device not started yet.
//
//--------------------------------------------------------------------------------------------------------------------

bool RootAttRegistry::RootAttConfCallBack::is_root_dev_not_started_err()
{
	omni_mutex_lock oml(the_lock);
	std::map<std::string,struct NameFwdAttr>::iterator ite;
	for (ite = map_attrdesc.begin();ite != map_attrdesc.end();++ite)
    {
        if (ite->second.fwd_attr != NULL)
        {
            if (ite->second.fwd_attr->get_err_kind() == FWD_ROOT_DEV_NOT_STARTED)
                return true;
        }
    }

    return false;
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

void RootAttRegistry::add_root_att(std::string &device_name,std::string &att_name,std::string &local_dev_name,std::string &local_att_name,
								   FwdAttr *attdesc,DeviceImpl *dev)
{
	DeviceProxy *the_dev;
	std::map<std::string,DeviceProxy *>::iterator ite;
	ite = dps.find(device_name);
	if (ite == dps.end())
	{
		try
		{
			the_dev = new DeviceProxy(device_name);
		}
		catch (Tango::DevFailed &e)
		{
			attdesc->set_err_kind(FWD_WRONG_DEV);

			std::string desc("The root device ");
			desc = desc + device_name + " is not defined in database";
			Except::throw_exception(API_AttrNotAllowed,desc,"RootAttRegistry::add_root_att");
		}
		int idl_vers = the_dev->get_idl_version();
		if (idl_vers > 0 && idl_vers < MIN_IDL_CONF5)
		{
			attdesc->set_err_kind(FWD_TOO_OLD_ROOT_DEVICE);
			delete the_dev;

			std::string desc("The root device ");
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
// First subscription is in statefull mode while the second one is in stateless mode
//

	std::string a_name = device_name + '/' + att_name;
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
			{
				bool loop = check_loop(device_name,att_name,local_dev_name,local_att_name);
				if (loop == true)
					attdesc->set_err_kind(FWD_CONF_LOOP);
				else
					attdesc->set_err_kind(FWD_WRONG_ATTR);
			}
			else if (::strcmp(e.errors[0].reason.in(),API_CantConnectToDevice) == 0 ||
					 ::strcmp(e.errors[0].reason.in(),API_DeviceNotExported) == 0 ||
					 ::strcmp(e.errors[0].reason.in(),API_ZmqFailed) == 0)
				attdesc->set_err_kind(FWD_ROOT_DEV_NOT_STARTED);
            else if (e.errors.length() > 1 && ::strcmp(e.errors[1].reason.in(),API_CantConnectToDevice) == 0)
            {
                Util *tg = Util::instance();
                std::string ds_name = tg->get_ds_name();
                std::transform(ds_name.begin(),ds_name.end(),ds_name.begin(),::tolower);
                std::string err_desc(e.errors[1].desc.in());

                if (err_desc.find(ds_name) != std::string::npos)
                    attdesc->set_err_kind(FWD_ROOT_DEV_NOT_STARTED);
            }
			else
				attdesc->set_err_kind(FWD_WRONG_DEV);

			event_id = the_dev->subscribe_event(att_name,Tango::ATTR_CONF_EVENT,&cbp,true);
			map_event_id.insert({a_name,event_id});

            cbp.update_err_kind(a_name,attdesc->get_err_kind());
			Tango::Except::re_throw_exception(e,"API_DummyException","nothing","RootAttRegistry::add_root_att");
		}
	}
	else
	{
		Tango::Util *tg = Util::instance();
		if (tg->is_device_restarting(local_dev_name) == false)
		{
			attdesc->set_err_kind(FWD_DOUBLE_USED);
            cbp.update_err_kind(a_name,attdesc->get_err_kind());

			std::string desc("It's not supported to have in the same device server process two times the same root attribute (");
			desc = desc + a_name + ")";
			Except::throw_exception(API_AttrNotAllowed,desc,"RootAttRegistry::add_root_att");
		}
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

void RootAttRegistry::clear_attrdesc(std::string &dev_name,std::string &att_name)
{
	std::string full_att_name = dev_name + '/' + att_name;
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

void RootAttRegistry::remove_root_att(std::string &root_dev_name,std::string &root_att_name)
{
	std::string full_root_att_name = root_dev_name + '/' + root_att_name;
	std::map<std::string,DeviceProxy *>::iterator pos = dps.find(root_dev_name);

//
// Unsubscribe from all user events registered on this forwarded attribute (if any)
//

	if (pos != dps.end())
	{
		std::map<std::string,std::vector<UserEvent> >::iterator it;
		it = map_event_id_user.find(full_root_att_name);

		if (it != map_event_id_user.end())
		{
			for (const auto &elem:it->second)
			{
				pos->second->unsubscribe_event(elem.event_id);
			}
			map_event_id_user.erase(it);
		}
	}

//
// Maybe this root device is used by other forwarded attribute in this device server
//

	int co = cbp.count_root_dev(root_dev_name);
	if (co != 0)
		cbp.remove_att(full_root_att_name);

//
// Unsubscribe from the event and if the root device is not used elsewhere, remove its device proxy
//

	std::map<std::string,int>::iterator ite = map_event_id.find(full_root_att_name);
	if (ite != map_event_id.end())
	{
		if (pos != dps.end())
		{
			pos->second->unsubscribe_event(ite->second);

			map_event_id.erase(ite);
			if (co == 1)
			{
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

DeviceProxy *RootAttRegistry::get_root_att_dp(std::string &device_name)
{
	std::map<std::string,DeviceProxy *>::iterator ite;
	ite = dps.find(device_name);
	if (ite == dps.end())
	{
		std::stringstream ss;
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
//			- root_name : The root attribute name (root dev_name/att_name)
//
// return :
//		The local attribute name
//
//--------------------------------------------------------------------------------------------------------------------

std::string RootAttRegistry::RootAttConfCallBack::get_local_att_name(const std::string &root_name)
{
	std::map<std::string,struct NameFwdAttr>::iterator ite;
	ite = map_attrdesc.find(root_name);
	if (ite == map_attrdesc.end())
	{
		std::stringstream ss;
		ss << root_name << " not registered in map of root attribute!";
		Except::throw_exception(API_FwdAttrInconsistency,ss.str(),"RootAttRegistry::get_local_att_name");
	}

	std::string loc_name = ite->second.local_name + '/' + ite->second.local_att_name;
	return loc_name;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::RootAttConfCallBack::get_local_dev
//
// description :
//		Get the local device pointer from a local device name
//
// argument :
//		in :
//			- local_dev_name : The local device name
//
// return :
//		The local device ptr
//
//--------------------------------------------------------------------------------------------------------------------

DeviceImpl *RootAttRegistry::RootAttConfCallBack::get_local_dev(std::string &local_dev_name)
{
	std::map<std::string,DeviceImpl *>::iterator ite;
	ite = local_dis.find(local_dev_name);
	if (ite == local_dis.end())
	{
		std::stringstream ss;
		ss << local_dev_name << " not registered in map of local device!";
		Except::throw_exception(API_FwdAttrInconsistency,ss.str(),"RootAttRegistry::get_local_dev");
	}
	return ite->second;
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

void RootAttRegistry::RootAttConfCallBack::update_label(std::string &root_name,std::string &new_label)
{
	std::map<std::string,struct NameFwdAttr>::iterator ite;
	ite = map_attrdesc.find(root_name);

	if (ite != map_attrdesc.end())
	{
		ite->second.local_label = new_label;
	}
	else
	{
		std::stringstream ss;
		ss << root_name << " not registered in map of root attribute!";
		Except::throw_exception(API_FwdAttrInconsistency,ss.str(),"RootAttRegistry::update_label");
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::check_root_dev_release
//
// description :
//		Check if the root device support IDL 5 or more
//
// argument :
//		in :
//			- root_dev_name : The root device name
//
// returns :
//		True if the root device is IDL 5 or more
//
//--------------------------------------------------------------------------------------------------------------------

bool RootAttRegistry::check_root_dev_release(std::string &root_dev_name)
{
	bool ret = true;

	DeviceProxy *dp = get_root_att_dp(root_dev_name);
	int idl_vers = dp->get_idl_version();
	if (idl_vers < MIN_IDL_CONF5)
		ret = false;

	return ret;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::is_event_subscribed
//
// description :
//		Check if one event is already subscribed
//
// argument :
//		in :
//			- ev : The event name (device_name/att_name)
//			- et : The event type
//
// returns :
//		True if the event is subscribed
//
//--------------------------------------------------------------------------------------------------------------------

bool RootAttRegistry::is_event_subscribed(std::string &ev,EventType et)
{
	bool ret = false;
	std::map<std::string,std::vector<UserEvent> >::iterator pos;

	{
		ReaderLock rl(id_user_lock);
		pos = map_event_id_user.find(ev);
		if (pos != map_event_id_user.end())
		{
			for (const auto &elem:pos->second)
			{
				if (elem.event_type == et)
				{
					ret = true;
					break;
				}
			}
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::subscribe_user_event
//
// description :
//		Subscribe to event from the root attribute
//
// argument :
//		in :
//			- dev_name : The root device name
//			- att_name : The attribute name
//			- et : The event type
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::subscribe_user_event(std::string &dev_name,std::string &att_name,EventType et)
{

//
// Find the proxy and create registered event name
//

	DeviceProxy *dp = get_root_att_dp(dev_name);
	std::string f_ev_name = dev_name + '/' + att_name;

//
// Subscribe to the event and store the event_id in the map. If the att is already known in the map, simply add
// one element in the vector of subscribed events
//

	int ev_id;
	UserEvent ue;
	ue.event_type = et;

	try
	{
		ev_id = dp->subscribe_event(att_name,et,&cbu);
		ue.event_id = ev_id;
	}
	catch (Tango::DevFailed &e)
	{
		std::string reason(e.errors[0].reason.in());
		if (reason == API_CantConnectToDevice)
		{
			ev_id = dp->subscribe_event(att_name,et,&cbu,true);
			ue.event_id = ev_id;
		}
		else
			throw;
	}

//
// Update map
//

	{
		WriterLock wl(id_user_lock);
		std::map<std::string,std::vector<UserEvent> >::iterator pos;

		pos = map_event_id_user.find(f_ev_name);
		if (pos == map_event_id_user.end())
		{
			std::vector<UserEvent> v_ue;
			v_ue.push_back(ue);

			map_event_id_user.insert(make_pair(f_ev_name,v_ue));
		}
		else
		{
			pos->second.push_back(ue);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::unsubscribe_user_event
//
// description :
//		Unsubscribe to event from the root attribute
//
// argument :
//		in :
//			- dev_name : The root device name
//			- att_name : The attribute name
//			- et : The event type
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::unsubscribe_user_event(std::string &dev_name,std::string &att_name,EventType et)
{

//
// Find the proxy and create registered event name
//

	DeviceProxy *dp = get_root_att_dp(dev_name);
	std::string f_ev_name = dev_name + '/' + att_name;

//
// Find the event_id
//

	int local_evid = 0;

	{
		ReaderLock rl(id_user_lock);
		std::map<std::string,std::vector<UserEvent> >::iterator pos;

		pos = map_event_id_user.find(f_ev_name);
		std::vector<UserEvent> &v_ue = pos->second;
		for (size_t loop = 0;loop < v_ue.size();++loop)
		{
			if (v_ue[loop].event_type == et)
			{
				local_evid = v_ue[loop].event_id;
				break;
			}
		}
	}

	if (local_evid == 0)
	{
		std::stringstream ss;
		ss << f_ev_name << " not found in map of user event on root attribute!";
		Except::throw_exception(API_FwdAttrInconsistency,ss.str(),"RootAttRegistry::unsubscribe_user_event");
	}

//
// Unsubscribe to the event.
//

	dp->unsubscribe_event(local_evid);

//
// Remove entry from vector in map. Eventually remove entry from map if vector becomes empty
//

	{
		WriterLock wl(id_user_lock);
		std::map<std::string,std::vector<UserEvent> >::iterator pos;

		pos = map_event_id_user.find(f_ev_name);
		std::vector<UserEvent> &v_ue = pos->second;
		std::vector<UserEvent>::iterator iter;
		for (iter = v_ue.begin();iter != v_ue.end();++iter)
		{
			if (iter->event_type == et)
			{
				v_ue.erase(iter);
				break;
			}
		}

		if (v_ue.empty() == true)
		{
			map_event_id_user.erase(pos);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::auto_unsub
//
// description :
//		Automatic unsubscription to event(s) on root device(s)
//
//--------------------------------------------------------------------------------------------------------------------

void RootAttRegistry::auto_unsub()
{

//
// Return immediately if there is no user events
//

	if (map_event_id_user.empty() == true)
		return;

//
// A loop on each events
//

    time_t now = time(NULL);

	{
		WriterLock wl(id_user_lock);

		std::map<std::string,std::vector<UserEvent> >::iterator ite;
		for (ite = map_event_id_user.begin();ite != map_event_id_user.end();++ite)
		{

//
// Get local dev_name and att name
//

			std::string local_name = get_local_att_name(ite->first);
			std::string::size_type pos = local_name.rfind('/');
			std::string local_dev_name = local_name.substr(0,pos);
			std::string local_att_name = local_name.substr(pos + 1);

//
// Get Attribute and DeviceImpl objects
//

			DeviceImpl *dev = cbp.get_local_dev(local_dev_name);
			AttributePrivate &att = dev->get_device_attr()->get_attr_by_name(local_att_name.c_str()).get_impl();

//
// A loop on each user event registered for this atttribute
//

			time_t delta_t = 0;
			std::vector<UserEvent>::iterator posi;

			for (posi = ite->second.begin();posi < ite->second.end();++posi)
			{
				{
					omni_mutex_lock oml(EventSupplier::get_event_mutex());
					switch (posi->event_type)
					{
						case CHANGE_EVENT:
						delta_t = now - att.get_change5_event_sub();
						break;

						case PERIODIC_EVENT:
						delta_t = now - att.get_periodic5_event_sub();
						break;

						case QUALITY_EVENT:
						delta_t = now - att.get_quality_event_sub();
						break;

						case ARCHIVE_EVENT:
						delta_t = now - att.get_archive5_event_sub();
						break;

						case USER_EVENT:
						delta_t = now - att.get_user5_event_sub();
						break;

						case DATA_READY_EVENT:
						delta_t = now - att.get_data_ready_event_sub();
						break;

						default:
						break;
					}
				}

//
// Unsubscribe to the event if the last subscription is too old and erase entry in vector of subscribed events
//

				if (delta_t >= EVENT_RESUBSCRIBE_PERIOD)
				{
					std::string::size_type po = ite->first.rfind('/');
					std::string root_dev = ite->first.substr(0,po);

					DeviceProxy *dp = get_root_att_dp(root_dev);
					dp->unsubscribe_event(posi->event_id);

					posi = ite->second.erase(posi);
				}
			}

//
// Remove entry in map if no more events on this forwarded attribute
//

			if (ite->second.empty() == true)
				map_event_id_user.erase(ite);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		RootAttRegistry::check_loop
//
// description :
//		Check if there is a loop in the root attribute configuration.
//		This is a one level only check. Could (Should) be imprved to be a n level check
//
// argument :
//		in :
//			- device_name : The device name
//			- att_name : The attribute name
//			- local_dev_name : The local device name
//			- local_att_name : The local attribute name
//
//--------------------------------------------------------------------------------------------------------------------

bool RootAttRegistry::check_loop(std::string &device_name,std::string &att_name,std::string &local_dev_name,std::string &local_att_name)
{
	std::string tg_host;
	int tg_port;
	bool ret = false;

	try
	{

//
// Get tango host in the device name and the local one
//

		Util::tango_host_from_fqan(device_name,tg_host,tg_port);

		Util *tg = Util::instance();
		Database *db = tg->get_database();
		std::string db_host = db->get_db_host();
		int db_port = db->get_db_port_num();

//
// Extract device name from fqdn
//

		std::string::size_type pos = device_name.find('/',8);
		std::string res_dev_name = device_name.substr(pos + 1);

//
// Retrieve root attribute property
//

		DbData db_data;
		db_data.push_back(DbDatum(att_name));

		if (db_port == tg_port && db_host == tg_host)
		{
			db->get_device_attribute_property(res_dev_name,db_data);
		}
		else
		{
			Database other_db(tg_host,tg_port);
			other_db.get_device_attribute_property(res_dev_name,db_data);
		}

		std::string new_root_att;
		DevLong nb_prop;
		db_data[0] >> nb_prop;

		if (nb_prop != 0)
		{
			bool prop_found = false;
			for (int k=0;k < nb_prop;k++)
			{
				std::string &prop_name = db_data[k + 1].name;
				if (prop_name == RootAttrPropName)
				{
					db_data[k + 1] >> new_root_att;
					prop_found = true;
					break;
				}
			}

//
// If the root attribute property is found, compare it with the local device/local_att name. If they are equal,
// it is a loop!
//

			if (prop_found == true)
			{
				std::transform(new_root_att.begin(),new_root_att.end(),new_root_att.begin(),::tolower);
				std::string full_local_att_name = local_dev_name + '/' + local_att_name;
				std::transform(full_local_att_name.begin(),full_local_att_name.end(),full_local_att_name.begin(),::tolower);

				if (full_local_att_name == new_root_att)
					ret = true;
			}
		}
	}
	catch (Tango::DevFailed &) {}

	return ret;
}


} // End of Tango namespace
