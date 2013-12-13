//===================================================================================================================
//
// file :		fwdattrdesc.h
//
// description :	Include file for the FwdAttr classes hierarchy. Three classes are declared in this file :
//				The FwdAttr class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
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
//===================================================================================================================

#ifndef _FWDATTRDESC_H
#define _FWDATTRDESC_H

#include <tango.h>

namespace Tango
{

class UserDefaultFwdAttrProp
{
public:

/**@name Constructor
 * Only one constructor is defined for this class */
//@{
/**
 * Constructs a newly allocated UserDefaultAttrProp object.
 */
	UserDefaultFwdAttrProp():ext(Tango_nullptr) {}
//@}

/**@name Set default property methods */
//@{
/**
 * Set default label property
 *
 * @param	def_label	The user default label property
 */
	void set_label(const string &def_label)
	{
		label = def_label;
	}
//@}

/// @privatesection
	~UserDefaultFwdAttrProp() {}

	string 			label;

private:
    class UserDefaultFwdAttrPropExt
    {
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<UserDefaultFwdAttrPropExt>  	ext;           // Class extension
#else
	UserDefaultFwdAttrPropExt	            *ext;
#endif
};

class FwdAttr: public ImageAttr
{
public:
	FwdAttr(const string &,const string &root_attribute=RootAttNotDef);

	virtual bool is_fwd() {return true;}

	string &get_fwd_root_att() {return fwd_root_att;}
	string &get_fwd_dev_name() {return fwd_dev_name;}
	string &get_full_root_att() {return full_root_att;}
	bool is_correctly_configured() {return fwd_wrongly_conf;}
	FwdAttError get_err_kind() {return err_kind;}
	void set_err_kind(FwdAttError _e) {err_kind = _e;}

	virtual void read(DeviceImpl *,Attribute &);
	virtual void write(DeviceImpl *,WAttribute &);
	virtual bool is_allowed(DeviceImpl *,AttReqType) {return true;}

	virtual void init_conf(AttrConfEventData *);
	bool validate_fwd_att(vector<AttrProperty> &,const string &);
	void get_root_conf(string &,DeviceImpl *);

 	void set_default_properties(UserDefaultFwdAttrProp &prop);
 	string &get_label_from_default_properties();

protected:
	string				full_root_att;			// Root att (dev_name/att_name)
	string				fwd_dev_name;			// Root att device name (lower case)
	string				fwd_root_att;			// Root att (lower case)
	bool				fwd_wrongly_conf;
	FwdAttError			err_kind;

private:
    class FwdAttrExt
    {
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<FwdAttrExt>    	ext;           // Class extension
#else
	FwdAttrExt		        	*ext;
#endif
};


} // End of Tango namespace

#endif /* _FWDATTRDESC_H */
