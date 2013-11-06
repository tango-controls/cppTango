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
#include <fwdattrdesc.h>

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
Attr(att_name.c_str()),full_root_att(root_attribute),fwd_wrongly_conf(false),err_kind(FWD_ERR_UNKNOWN),ext(Tango_NullPtr)
{
	writable = Tango::READ;			// Difficult to switch it to WT_UNKNOWN
//	type = DATA_TYPE_UNKNOWN;
	type = DEV_DOUBLE;
	format = Tango::FMT_UNKNOWN;
	disp_level = OPERATOR;
	assoc_name = AssocWritNotSpec;

	mem = false;
	mem_init = false;

    fire_change_event = false;
    fire_archive_event = false;
    check_change_event = false;
    check_archive_event = false;
    fire_dr_event = false;

    set_cl_name("FwdAttr");
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

	try
	{
#ifdef HAS_LAMBDA_FUNC
		auto pos = find_if(prop_list.begin(),prop_list.end(),
					[] (AttrProperty &ap) -> bool
					{
						return ap.get_name() == "__root_att";
					});
#else
#endif
		if (pos != prop_list.end())
		{
			root_att_db = pos->get_value();
			root_att_db_defined = true;
		}
	}
	catch (...) {}

	if (root_att_db_defined == true)
		full_root_att = root_att_db;

//
// Check root att syntax
//

	if (full_root_att != RootAttNotDef)
	{
		int nb_sep = count(full_root_att.begin(),full_root_att.end(),'/');

		if (nb_sep == 3)
		{
			string::size_type pos = full_root_att.find_last_of('/');
			fwd_root_att = full_root_att.substr(pos + 1);
			fwd_dev_name = full_root_att.substr(0,pos);

			transform(fwd_dev_name.begin(),fwd_dev_name.end(),fwd_dev_name.begin(),::tolower);
			transform(fwd_root_att.begin(),fwd_root_att.end(),fwd_root_att.begin(),::tolower);
		}
		else
		{
			fwd_wrongly_conf = true;
			err_kind = FWD_WRONG_SYNTAX;
			ret = false;
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

	if (fwd_dev_name == dev_name)
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
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttr::get_root_conf(string &dev_name)
{
	try
	{
		RootAttRegistry &dps = Tango::Util::instance()->get_root_att_reg();
		dps.add_root_att(fwd_dev_name,fwd_root_att,dev_name,name,this);
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
	DeviceProxy *root_att_dev = rar.get_root_att_dp(fwd_attr.get_fwd_dev_name());

//
// Read the attribute
//

static DevDouble db;

	DeviceAttribute da = root_att_dev->read_attribute(fwd_attr.get_fwd_att_name());
da >> db;

attr.set_value(&db);
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

void FwdAttr::write(DeviceImpl *dev,WAttribute &attr)
{

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
//			- root_conf : The root attribute configuration
//
//--------------------------------------------------------------------------------------------------------------------

void FwdAttr::init_conf(AttributeInfoEx *root_conf)
{
cout << "In FwdAttr::init_conf()" << endl;
//
// Set main data
//

	type = root_conf->data_type;
	writable = root_conf->writable;
	format = root_conf->data_format;
//	max_x = root_conf->max_dim_x;
//	max_y = root_conf->max_dim_y;
	assoc_name = root_conf->writable_attr_name;
	if (type == READ_WRITE)
		assoc_name = name;
	disp_level = root_conf->disp_level;

//
// Set configuration
//

	UserDefaultAttrProp udap;
	udap.set_label(root_conf->label.c_str());
	udap.set_description(root_conf->description.c_str());
	udap.set_unit(root_conf->unit.c_str());
	udap.set_standard_unit(root_conf->standard_unit.c_str());
	udap.set_display_unit(root_conf->display_unit.c_str());
	udap.set_format(root_conf->format.c_str());
	udap.set_min_alarm(root_conf->alarms.min_alarm.c_str());
	udap.set_max_alarm(root_conf->alarms.max_alarm.c_str());
	udap.set_min_value(root_conf->min_value.c_str());
	udap.set_max_value(root_conf->max_value.c_str());
	udap.set_min_warning(root_conf->alarms.min_warning.c_str());
	udap.set_max_warning(root_conf->alarms.max_warning.c_str());
	udap.set_delta_t(root_conf->alarms.delta_t.c_str());
	udap.set_delta_val(root_conf->alarms.delta_val.c_str());

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

} // End of Tango namespace
