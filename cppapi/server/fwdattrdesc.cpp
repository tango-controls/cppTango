static const char *RcsId = "$Id$";

//+==================================================================================================================
//
// file :               fwdattrdesc.cpp
//
// description :        C++ source code for the FwdAttrDesc classes
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
// $Revision$
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <fwdattrdesc.h>

#include <fwdattribute.tpp>
#include <fwdattribute_spec.tpp>

namespace Tango
{

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttr::FwdAttr
//
// description :
//		Constructor for the FwdAttr class. Initialize data with default value for a wrongly configured
//		forwarded attribute
//
// argument :
//		in :
//			- att_name : The attribute name
//			- root_attribute : The root attribute name
//
//--------------------------------------------------------------------------------------------------------------------


FwdAttr::FwdAttr(const string &att_name,const string &root_attribute):
ImageAttr(att_name.c_str()),full_root_att(root_attribute),fwd_wrongly_conf(false),err_kind(FWD_ERR_UNKNOWN),ext(Tango_nullptr)
{
	writable = Tango::READ;			// Difficult to switch it to WT_UNKNOWN
//	type = DATA_TYPE_UNKNOWN;
	type = DEV_DOUBLE;
	format = Tango::FMT_UNKNOWN;
//	disp_level = DL_UNKNOWN;
	assoc_name = AssocWritNotSpec;

	max_x = 0;
	max_y = 0;

	mem = false;
	mem_init = false;

    fire_change_event = false;
    fire_archive_event = false;
    check_change_event = false;
    check_archive_event = false;
    fire_dr_event = false;

    set_cl_name("FwdAttr");
    set_err_kind(FWD_NO_ERROR);
}

FwdAttr::FwdAttr(const FwdAttr &sou):ImageAttr(sou)
{
	full_root_att = sou.full_root_att;
	fwd_dev_name = sou.fwd_dev_name;
	fwd_root_att = sou.fwd_root_att;
	fwd_wrongly_conf = sou.fwd_wrongly_conf;
	err_kind = sou.err_kind;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttr::validate_fwd_att
//
// description :
//		Validate that we have the knowledge of the root attribute.
//
// argument :
//		in :
//			- prop_list : The attribute property list
//			- dev_name : The device name
//
// return:
//		True if we have the required info to build a forwarded attribute
//
//--------------------------------------------------------------------------------------------------------------------

bool FwdAttr::validate_fwd_att(vector<AttrProperty> &prop_list,const string &dev_name)
{
	bool ret = true;

//
// Check if root attribute is defined in DB (then in received prop list)
// If it's the case, check its syntax
//

	string root_att_db;
	bool root_att_db_defined = false;
	bool is_full_root_att_set = false;

    Util *tg = Util::instance();
    Database *db = tg->get_database();

	try
	{
#ifdef HAS_LAMBDA_FUNC
		auto pos = find_if(prop_list.begin(),prop_list.end(),
					[] (AttrProperty &ap) -> bool
					{
						return ap.get_name() == RootAttrPropName;
					});
#else
		vector<AttrProperty>::iterator pos;
		for (pos = prop_list.begin();pos != prop_list.end();++pos)
		{
			if (pos->get_name() == RootAttrPropName)
				break;
		}
#endif
		if (pos != prop_list.end())
		{
			root_att_db = pos->get_value();
			root_att_db_defined = true;
		}
		else
        {

//
// Write something in DB to help user to create the right entry except if the root att name is hard coded
//

            if (full_root_att == RootAttNotDef)
            {
                DbDatum att(get_name());
                att << (DevShort)1;
                DbDatum root_name(RootAttrPropName);
                root_name << RootAttNotDef;

                DbData db_dat;
                db_dat.push_back(att);
                db_dat.push_back(root_name);

                try
                {
                    if (db != Tango_nullptr)
                        db->put_device_attribute_property(dev_name,db_dat);
                }
                catch(...) {}
            }
        }
	}
	catch (...) {}

	//check if full_root_att is already set
	is_full_root_att_set = full_root_att.size()!=0 && full_root_att.compare(RootAttNotDef)!=0;

	if(!is_full_root_att_set) {
		if (root_att_db_defined)
			full_root_att = root_att_db;
		else
			full_root_att = RootAttNotDef;
	}

//
// Check root att syntax and add TANGO_HOST info in root device name of not given
// Also add dns suffix if not defined in provided TANGO_HOST host name
//

	string fq;
	if (db != Tango_nullptr)
	{
		fq = "tango://";
		string &h = db->get_db_host();
		string &p = db->get_db_port();
		fq = fq + h + ':' + p + '/';
	}
	transform(fq.begin(),fq.end(),fq.begin(),::tolower);

	if (full_root_att != RootAttNotDef)
	{
		int nb_sep = count(full_root_att.begin(),full_root_att.end(),'/');

		if (nb_sep == 3)
		{
			full_root_att.insert(0,fq);
		}
		else if (nb_sep == 6)
		{
			string::size_type pos = full_root_att.find("tango://");
			if (pos != 0)
				ret = false;
			else
			{
				pos = full_root_att.find(':',8);
				string ho = full_root_att.substr(8,pos - 8);
				string::size_type pos1 = ho.find('.');
				size_t old_size = ho.size();
				if (pos1 == string::npos)
					Connection::get_fqdn(ho);
				string dom = ho.substr(old_size);
				full_root_att.insert(pos,dom);
			}
		}
		else
		{
			fwd_wrongly_conf = true;
			err_kind = FWD_WRONG_SYNTAX;
			ret = false;
		}

		if (ret == true)
		{
			string::size_type pos = full_root_att.find_last_of('/');
			fwd_root_att = full_root_att.substr(pos + 1);
			fwd_dev_name = full_root_att.substr(0,pos);

			transform(fwd_dev_name.begin(),fwd_dev_name.end(),fwd_dev_name.begin(),::tolower);
			transform(fwd_root_att.begin(),fwd_root_att.end(),fwd_root_att.begin(),::tolower);
		}
	}
	else
	{
		fwd_wrongly_conf = true;
		err_kind = FWD_MISSING_ROOT;
		ret = false;
	}

//
// Check that the root device is not the local device
//

	string local_dev_name(dev_name);
	local_dev_name.insert(0,fq);

	if (fwd_dev_name == local_dev_name)
	{
		fwd_wrongly_conf = true;
		err_kind = FWD_ROOT_DEV_LOCAL_DEV;
		ret = false;
	}

	return ret;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttr::get_root_conf
//
// description :
//		Get the root attribute configuration
//
// argument :
//		in :
//			- dev_name : The local device name
//			- dev : Device pointer
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttr::get_root_conf(string &dev_name,DeviceImpl *dev)
{
	try
	{
		RootAttRegistry &dps = Tango::Util::instance()->get_root_att_reg();
		dps.add_root_att(fwd_dev_name,fwd_root_att,dev_name,name,this,dev);
	}
	catch (Tango::DevFailed &)
	{
		fwd_wrongly_conf = true;
		throw;
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttr::read
//
// description :
//		Read method for forwarded attribute
//
// argument :
//		in :
//			- dev : The device
//			- attr : The attribute object
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttr::read(DeviceImpl *dev,Attribute &attr)
{
//
// Throw exception in case of fwd att wrongly configured or if the root device is not yet accessible
//

	if (attr.get_data_type() == DATA_TYPE_UNKNOWN)
	{
		string desc("Attribute ");
		desc = desc + name + " is a forwarded attribute and its root device (";
		desc = desc + fwd_dev_name;
		desc = desc + ") is not yet available";
		Tango::Except::throw_exception(API_AttrConfig,desc,"FwdAttr::read");
	}

//
// Retrieve root attribute device proxy object
//

	FwdAttribute &fwd_attr = static_cast<FwdAttribute &>(attr);
	RootAttRegistry &rar = Util::instance()->get_root_att_reg();
	DeviceProxy *root_att_dev;
	try
	{
		root_att_dev = rar.get_root_att_dp(fwd_attr.get_fwd_dev_name());
	}
	catch (Tango::DevFailed &e)
	{
		string desc("Attribute ");
		desc = desc + name + " is a forwarded attribute.\n";
		desc = desc + "Check device status to get more info";
		Tango::Except::re_throw_exception(e,API_AttrConfig,desc,"FwdAttr::read()");
	}

//
// Read the ŕoot attribute
//

	try
	{
		root_att_dev->set_source(dev->get_call_source());
		DeviceAttribute da = root_att_dev->read_attribute(fwd_attr.get_fwd_att_name());

//
// Set the local attribute from the result of the previous read
//

		switch(fwd_attr.get_data_type())
		{
		case DEV_SHORT:
		case DEV_ENUM:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().sh_seq);
			break;

		case DEV_LONG:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().lg_seq);
			break;

		case DEV_FLOAT:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().fl_seq);
			break;

		case DEV_DOUBLE:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().db_seq);
			break;

		case DEV_STRING:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().str_seq);
			break;

		case DEV_USHORT:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().ush_seq);
			break;

		case DEV_BOOLEAN:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().boo_seq);
			break;

		case DEV_UCHAR:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().cha_seq);
			break;

		case DEV_LONG64:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().lg64_seq);
			break;

		case DEV_ULONG:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().ulg_seq);
			break;

		case DEV_ULONG64:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().ulg64_seq);
			break;

		case DEV_STATE:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().state_seq);
			break;

		case DEV_ENCODED:
			fwd_attr.set_local_attribute(da,fwd_attr.get_root_ptr().enc_seq);
			break;

		default:
			break;
		}
	}
	catch (Tango::DevFailed &e)
	{
		stringstream ss;
		ss << "Reading root attribute " << fwd_root_att << " on device " << fwd_dev_name << " failed!";
		Tango::Except::re_throw_exception(e,API_AttributeFailed,ss.str(),"FwdAttr::read");
	}

}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttr::write
//
// description :
//		Write method for forwarded attribute
//
// argument :
//		in :
//			- dev : The device
//			- attr : The attribute object
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttr::write(TANGO_UNUSED(DeviceImpl *dev),WAttribute &attr)
{

//
// Throw exception in case of fwd att wrongly configured or if the root device is not yet accessible
//

	if (attr.get_data_type() == DATA_TYPE_UNKNOWN)
	{
		string desc("Attribute ");
		desc = desc + name + " is a forwarded attribute and its root device (";
		desc = desc + fwd_dev_name;
		desc = desc + ") is not yet available";
		Tango::Except::throw_exception(API_AttrConfig,desc,"FwdAttr::write");
	}

//
// Retrieve root attribute device proxy object
//

	FwdAttribute &fwd_attr = static_cast<FwdAttribute &>(attr);
	RootAttRegistry &rar = Util::instance()->get_root_att_reg();
	DeviceProxy *root_att_dev;
	try
	{
		root_att_dev = rar.get_root_att_dp(fwd_attr.get_fwd_dev_name());
	}
	catch (Tango::DevFailed &e)
	{
		string desc("Attribute ");
		desc = desc + name + " is a forwarded attribute.\n";
		desc = desc + "Check device status to get more info";
		Tango::Except::re_throw_exception(e,API_AttrConfig,desc,"FwdAttr::write()");
	}

//
// Write the ŕoot attribute
//

	DeviceAttribute da;
	da.set_name(fwd_attr.get_fwd_att_name());

	switch(fwd_attr.get_data_type())
	{
	case DEV_SHORT:
	case DEV_ENUM:
		DevShort *ptr_sh;
		fwd_attr.propagate_writen_data(da,attr,ptr_sh,fwd_attr.get_root_ptr().sh_seq);
		break;

	case DEV_LONG:
		DevLong *ptr_lo;
		fwd_attr.propagate_writen_data(da,attr,ptr_lo,fwd_attr.get_root_ptr().lg_seq);
		break;

	case DEV_FLOAT:
		DevFloat *ptr_fl;
		fwd_attr.propagate_writen_data(da,attr,ptr_fl,fwd_attr.get_root_ptr().fl_seq);
		break;

	case DEV_DOUBLE:
		DevDouble *ptr_db;
		fwd_attr.propagate_writen_data(da,attr,ptr_db,fwd_attr.get_root_ptr().db_seq);
		break;

	case DEV_STRING:
		ConstDevString *ptr_str;
		fwd_attr.propagate_writen_data(da,attr,ptr_str,fwd_attr.get_root_ptr().str_seq);
		break;

	case DEV_USHORT:
		DevUShort *ptr_ush;
		fwd_attr.propagate_writen_data(da,attr,ptr_ush,fwd_attr.get_root_ptr().ush_seq);
		break;

	case DEV_BOOLEAN:
		DevBoolean *ptr_bo;
		fwd_attr.propagate_writen_data(da,attr,ptr_bo,fwd_attr.get_root_ptr().boo_seq);
		break;

	case DEV_UCHAR:
		DevUChar *ptr_uch;
		fwd_attr.propagate_writen_data(da,attr,ptr_uch,fwd_attr.get_root_ptr().cha_seq);
		break;

	case DEV_LONG64:
		DevLong64 *ptr_lg64;
		fwd_attr.propagate_writen_data(da,attr,ptr_lg64,fwd_attr.get_root_ptr().lg64_seq);
		break;

	case DEV_ULONG:
		DevULong *ptr_ulg;
		fwd_attr.propagate_writen_data(da,attr,ptr_ulg,fwd_attr.get_root_ptr().ulg_seq);
		break;

	case DEV_ULONG64:
		DevULong64 *ptr_ulg64;
		fwd_attr.propagate_writen_data(da,attr,ptr_ulg64,fwd_attr.get_root_ptr().ulg64_seq);
		break;

	case DEV_STATE:
		DevState *ptr_sta;
		fwd_attr.propagate_writen_data(da,attr,ptr_sta,fwd_attr.get_root_ptr().state_seq);
		break;

	case DEV_ENCODED:
		DevEncoded *ptr_enc;
		fwd_attr.propagate_writen_data(da,attr,ptr_enc,fwd_attr.get_root_ptr().enc_seq);
		break;

	default:
		break;
	}

	try
	{
		 root_att_dev->write_attribute(da);
	}
	catch (Tango::DevFailed &e)
	{
		stringstream ss;
		ss << "Writing root attribute " << fwd_root_att << " on device " << fwd_dev_name << " failed!";
		Tango::Except::re_throw_exception(e,API_AttributeFailed,ss.str(),"FwdAttr::write");
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttr::init_conf()
//
// description :
//		This method initialize FwdAttr class data member in case of forwarded attribute from the root attribute
//		configuration received during the attribute change event subscription
//
// argument :
//		in :
//			- ev_data : The event data
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttr::init_conf(AttrConfEventData *ev_data)
{

//
// Set main data
//

	type = ev_data->attr_conf->data_type;
	writable = ev_data->attr_conf->writable;
	format = ev_data->attr_conf->data_format;
	max_x = ev_data->attr_conf->max_dim_x;
	max_y = ev_data->attr_conf->max_dim_y;
	assoc_name = ev_data->attr_conf->writable_attr_name;
	if (writable == READ_WRITE)
		assoc_name = name;
	disp_level = ev_data->attr_conf->disp_level;
	switch (ev_data->attr_conf->memorized)
	{
	case NOT_KNOWN:
	case NONE:
		mem = false;
		mem_init = false;
		break;

	case MEMORIZED:
		mem = true;
		mem_init = false;
		break;

	case MEMORIZED_WRITE_INIT:
		mem = true;
		mem_init = true;
		break;

	default:
		break;
	}

//
// Set configuration
// If we already have a label in our conf, save it and reapply it
//

	string local_label;
	try
	{
		local_label = get_label_from_default_properties();
	}
	catch (Tango::DevFailed &e) {}

	UserDefaultAttrProp udap;
	if (local_label.empty() == false)
		udap.set_label(local_label.c_str());
	udap.set_description(ev_data->attr_conf->description.c_str());
	udap.set_unit(ev_data->attr_conf->unit.c_str());
	udap.set_standard_unit(ev_data->attr_conf->standard_unit.c_str());
	udap.set_display_unit(ev_data->attr_conf->display_unit.c_str());
	udap.set_format(ev_data->attr_conf->format.c_str());
	udap.set_min_value(ev_data->attr_conf->min_value.c_str());
	udap.set_max_value(ev_data->attr_conf->max_value.c_str());
	udap.set_enum_labels(ev_data->attr_conf->enum_labels);

	udap.set_min_alarm(ev_data->attr_conf->alarms.min_alarm.c_str());
	udap.set_max_alarm(ev_data->attr_conf->alarms.max_alarm.c_str());
	udap.set_min_warning(ev_data->attr_conf->alarms.min_warning.c_str());
	udap.set_max_warning(ev_data->attr_conf->alarms.max_warning.c_str());
	udap.set_delta_val(ev_data->attr_conf->alarms.delta_val.c_str());
	udap.set_delta_t(ev_data->attr_conf->alarms.delta_t.c_str());

	udap.set_event_abs_change(ev_data->attr_conf->events.ch_event.abs_change.c_str());
	udap.set_event_rel_change(ev_data->attr_conf->events.ch_event.rel_change.c_str());
	udap.set_event_period(ev_data->attr_conf->events.per_event.period.c_str());
	udap.set_archive_event_abs_change(ev_data->attr_conf->events.arch_event.archive_abs_change.c_str());
	udap.set_archive_event_rel_change(ev_data->attr_conf->events.arch_event.archive_rel_change.c_str());
	udap.set_archive_event_period(ev_data->attr_conf->events.arch_event.archive_period.c_str());

	this->Attr::set_default_properties(udap);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttr::set_default_properties
//
// description :
//		This method set the default user properties in the Attr object. At this level, each attribute property is
//		represented by one instance of the Attrproperty class.
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttr::set_default_properties(UserDefaultFwdAttrProp &prop_list)
{
	if ((prop_list.label.empty() == false) &&
		(TG_strcasecmp(prop_list.label.c_str(),AlrmValueNotSpec) != 0) &&
		(TG_strcasecmp(prop_list.label.c_str(),NotANumber) != 0))
		user_default_properties.push_back(AttrProperty("label",prop_list.label));
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttr::get_label_from_default_properties
//
// description :
//		Get the label property from the user default properties list. Throw exception if not in list
//
// return :
//		The label property value
//
//--------------------------------------------------------------------------------------------------------------------

string &FwdAttr::get_label_from_default_properties()
{
	size_t nb_prop = user_default_properties.size();
	size_t ctr;
	for (ctr = 0;ctr < nb_prop;ctr++)
	{
		if (user_default_properties[ctr].get_name() == "label")
			break;
	}

	if (ctr == nb_prop)
	{
		Except::throw_exception(API_AttrOptProp,"Property label not defined in list",
								"FwdAttr::get_label_from_default_properties");
	}

	return user_default_properties[ctr].get_value();
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::remove_useless_prop
//
// description :
//		Remove classical attribute property(ies) if found in prop_list input argument. For forwarded attribute,
//      the configuration is taken from the root att and should not be taken from the DB. If we found some
//      useless properties, remove them from the prop list and inform the user to clean up the database.
//
// argument :
//		in :
//			- prop_list : The property list
//          - dev_name : The device name
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttr::remove_useless_prop(vector<AttrProperty> &prop_list,string &dev_name,MultiAttribute *m_attr)
{
    vector<AttrProperty>::iterator ite;

    for (ite = prop_list.begin();ite != prop_list.end();)
    {
        if (ite->get_name() == "label" || ite->get_name() == "__root_att")
        {
            ++ite;
            continue;
        }

        if (m_attr->is_opt_prop(ite->get_name()) == true)
        {
            cerr << "Warning: The forwarded attribute " << get_name()  << " belonging to device "  << dev_name;
            cerr << "  has the property " << ite->get_name() << " defined in DB.\n";
            cerr << "This property will not be taken into account. Please clean up your DB." << endl;
            ite = prop_list.erase(ite);
        }
        else
            ++ite;
    }
}

} // End of Tango namespace
