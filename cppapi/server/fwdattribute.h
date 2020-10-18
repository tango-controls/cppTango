
//===================================================================================================================
//
// file :		fwdattribute.h
//
// description :	Include file for the FwdAttribute classes.
//
// project :		TANGO
//
// author(s) :		E.Taurel
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
//
//====================================================================================================================

#ifndef _FWDATTRIBUTE_H
#define _FWDATTRIBUTE_H

#include <tango.h>
#include <tango/server/w_attribute.h>

#include <memory>

#ifdef _TG_WINDOWS_
#include <sys/types.h>
#include <sys/timeb.h>
#endif

namespace Tango
{

class FwdAttributePrivate;

class FwdAttribute : public WAttribute
{
public:
	FwdAttribute(std::unique_ptr<FwdAttributePrivate>);

	const FwdAttributePrivate& get_impl() const;
	FwdAttributePrivate& get_impl();

private:
	FwdAttributePrivate* impl;
};

class FwdAttributePrivate: public WAttributePrivate
{
public:
	FwdAttributePrivate(std::vector<AttrProperty> &,Attr &,std::string &,long);
	~FwdAttributePrivate();

	virtual bool is_fwd_att() {return true;}
	std::string &get_fwd_dev_name() {return fwd_dev_name;}
	std::string &get_fwd_att_name() {return fwd_att_name;}

	void set_att_config(const Tango::AttributeConfig_5 &);
	void set_att_config(const Tango::AttributeConfig_3 &) {}
	void set_att_config(AttributeInfoEx *);

	void upd_att_config_base(const char *);
	void upd_att_config(const Tango::AttributeConfig_5 &);
	void upd_att_config(const Tango::AttributeConfig_3 &);

	void upd_att_label(const char *);
	bool new_att_conf(const Tango::AttributeConfig_3 *,const Tango::AttributeConfig_5 *);

	Attr_Value &get_root_ptr() {return r_val;}

	template<typename T>
	void set_local_attribute(FwdAttribute&, DeviceAttribute &, T* &);

	template<typename T,typename V>
	void propagate_writen_data(DeviceAttribute &da,WAttribute &attr,T *&,V *&);

	template<typename T>
	bool new_att_conf_base(const T&);

	DevAttrHistory_5 *read_root_att_history(long n);
	AttributeValueList_5 *write_read_root_att(AttributeValueList_4&);

protected:
	void convert_event_prop(std::string &,double *);

	std::string				fwd_dev_name;					// Root dev name for fwd attribute
	std::string				fwd_att_name;					// Root att name for fwd attribute

	AttrQuality 		qual;
#ifdef _TG_WINDOWS_
	struct _timeb		tv;
#else
	timeval 			tv;
#endif
	Attr_Value			r_val;
};

} // End of Tango namespace

#endif // _FWDATTRIBUTE_H
