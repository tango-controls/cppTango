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
//===================================================================================================================

#ifndef _FWDATTRDESC_H
#define _FWDATTRDESC_H

#include <tango.h>

namespace Tango
{

/**
 * User class to set forwarded attribute default properties.
 *
 * This class is used to set forwarded attribute default properties. Three levels of
 * attributes properties setting are implemented within Tango. The highest
 * property setting level is the database. Then the user default (set using
 * this UserDefaultFwdAttrProp class) and finally a Tango library default
 * value
 *
 *
 * @headerfile tango.h
 * @ingroup Server
 */

class UserDefaultFwdAttrProp
{
public:

/**@name Constructor
 * Only one constructor is defined for this class */
//@{
/**
 * Constructs a newly allocated UserDefaultAttrProp object.
 */
	UserDefaultFwdAttrProp():ext(nullptr) {}
//@}

/**@name Set default property methods */
//@{
/**
 * Set default label property
 *
 * @param	def_label	The user default label property
 */
	void set_label(const std::string &def_label)
	{
		label = def_label;
	}
//@}

/// @privatesection
	~UserDefaultFwdAttrProp() {}

	std::string 			label;

private:
    class UserDefaultFwdAttrPropExt
    {
    };

    std::unique_ptr<UserDefaultFwdAttrPropExt>  	ext;           // Class extension
};

class MultiAttribute;

/**
 * User class to create a forwarded attribute object.
 *
 * Information from this class and information fetched out from the Tango
 * database allows the Tango core software to create the FwdAttribute object
 * for the forwarded attribute created by the user.
 *
 *
 * @headerfile tango.h
 * @ingroup Server
 */

class FwdAttr: public ImageAttr
{
public:
/**@name Constructors
 * Only one constructor is defined for this class */
//@{
/**
 * Constructs a newly allocated FwdAttr object.
 *
 * @param 	name	The attribute name
 * @param	root_attribute	The root attribute name (FQAN)
 *
 */
	FwdAttr(const std::string &name,const std::string &root_attribute=RootAttNotDef);
//@}

/**@name Miscellaneous methods*/
//@{
/**
 * Set default attribute properties
 *
 * @param	prop	The user default property class
 */
 	void set_default_properties(UserDefaultFwdAttrProp &prop);
//@}

/// @privatesection
    FwdAttr(const FwdAttr &);
	virtual bool is_fwd() {return true;}

	std::string &get_fwd_root_att() {return fwd_root_att;}
	std::string &get_fwd_dev_name() {return fwd_dev_name;}
	std::string &get_full_root_att() {return full_root_att;}
	bool is_correctly_configured() {return fwd_wrongly_conf;}
	FwdAttError get_err_kind() {return err_kind;}
	void set_err_kind(FwdAttError _e) {err_kind = _e;}

	virtual void read(DeviceImpl *,Attribute &);
	virtual void write(DeviceImpl *,WAttribute &);
	virtual bool is_allowed(DeviceImpl *,AttReqType) {return true;}

	virtual void init_conf(AttrConfEventData *);
	bool validate_fwd_att(std::vector<AttrProperty> &,const std::string &);
	void get_root_conf(std::string &,DeviceImpl *);

    void remove_useless_prop(std::vector<AttrProperty> &,std::string &,MultiAttribute *);
 	std::string &get_label_from_default_properties();

protected:
/// @privatesection
	std::string				full_root_att;			// Root att (dev_name/att_name)
	std::string				fwd_dev_name;			// Root att device name (lower case)
	std::string				fwd_root_att;			// Root att (lower case)
	bool				fwd_wrongly_conf;
	FwdAttError			err_kind;

private:
    class FwdAttrExt
    {
    };

    std::unique_ptr<FwdAttrExt>    	ext;           // Class extension
};


} // End of Tango namespace

#endif /* _FWDATTRDESC_H */
