static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               w_attribute.cpp
//
// description :        C++ source code for the WAttribute class.
//			This class is used to manage attribute.
//			A Tango Device object instance has one
//			MultiAttribute object which is an aggregate of
//			Attribute or WAttribute objects
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
//						European Synchrotron Radiation Facility
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
//-============================================================================


#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <attribute.h>
#include <w_attribute.h>
#include <classattribute.h>

#include <functional>
#include <algorithm>

#ifdef _TG_WINDOWS_
#include <sys/types.h>
#include <sys/timeb.h>
#include <float.h>
#else
#include <sys/time.h>
#endif /* _TG_WINDOWS_ */
#include <math.h>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::WAttribute
//
// description : 	constructor for the WAttribute class from the
//			attribute property vector, its type and the device
//			name
//
// argument : in : 	- prop_list : The attribute property list
//			- type : The attrubute data type
//			- dev_name : The device name
//
//--------------------------------------------------------------------------


WAttribute::WAttribute(vector<AttrProperty> &prop_list,
		       Attr &tmp_attr,string &dev_name,long idx)
:Attribute(prop_list,tmp_attr,dev_name,idx),memorized(false),memorized_init(true),w_ext(new WAttributeExt)
{

//
// Init some data
//

	short_val = old_short_val = 0;
	long_val = old_long_val = 0;
	double_val = old_double_val = 0.0;
	float_val = old_float_val = 0.0;
	boolean_val = old_boolean_val = true;
	ushort_val = old_ushort_val = 0;
	uchar_val = old_uchar_val = 0;
	w_ext->long64_val = w_ext->old_long64_val = 0;
	w_ext->ulong_val = w_ext->old_ulong_val = 0;
	w_ext->ulong64_val = w_ext->old_ulong64_val = 0;
	w_ext->dev_state_val = w_ext->old_dev_state_val = Tango::UNKNOWN;
	str_val = CORBA::string_dup("Not initialised");
	old_str_val = CORBA::string_dup("Not initialised");
	encoded_val.encoded_data.length(0);
	encoded_val.encoded_format = CORBA::string_dup("Not initialised");
	old_encoded_val.encoded_data.length(0);
	old_encoded_val.encoded_format = CORBA::string_dup("Not initialised");

	short_array_val.length(1);
	short_array_val[0] = 0;
	long_array_val.length(1);
	long_array_val[0] = 0;
	double_array_val.length(1);
	double_array_val[0] = 0.0;
	str_array_val.length(1);
	str_array_val[0] = CORBA::string_dup("Not initialised");
	float_array_val.length(1);
	float_array_val[0] = 0.0;
	boolean_array_val.length(1);
	boolean_array_val[0] = true;
	ushort_array_val.length(1);
	ushort_array_val[0] = 0;
	uchar_array_val.length(1);
	uchar_array_val[0] = 0;
	w_ext->long64_array_val.length(1);
	w_ext->long64_array_val[0] = 0;
	w_ext->ulong_array_val.length(1);
	w_ext->ulong_array_val[0] = 0;
	w_ext->ulong64_array_val.length(1);
	w_ext->ulong64_array_val[0] = 0;
	w_ext->state_array_val.length(1);
	w_ext->state_array_val[0] = Tango::UNKNOWN;


	short_ptr = &short_val;
	w_dim_x = 1;
	w_dim_y = 0;
	write_date.tv_sec = write_date.tv_usec = 0;

//
// Init memorized field and eventually get the memorized value
//

	set_memorized(tmp_attr.get_memorized());
	set_memorized_init(tmp_attr.get_memorized_init());

	if (is_memorized() == true)
	{
		try
		{
			mem_value = get_attr_value(prop_list,MemAttrPropName);
		}
		catch (Tango::DevFailed)
		{
			mem_value = MemNotUsed;
		}
	}
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::~WAttribute
//
// description : 	destructor for the WAttribute class
//
//--------------------------------------------------------------------------

WAttribute::~WAttribute()
{
#ifndef HAS_UNIQUE_PTR
	delete w_ext;
#endif
	CORBA::string_free(str_val);
	CORBA::string_free(old_str_val);
//	CORBA::string_free(encoded_val.encoded_format);
//	CORBA::string_free(old_encoded_val.encoded_format);
}


//+-------------------------------------------------------------------------
//
// method : 		WAttribute::set_rvalue
//
// description : 	This method is used when a Writable attribute is
//			set to set the value in the Attribute class. This
//			is necessary for the read_attribute CORBA operation
//			which takes its data from this internal Attribute
//			class data.
//			It is used in the read_attributes code in the
//			device class
//
//--------------------------------------------------------------------------

void WAttribute::set_rvalue()
{
	switch(data_type)
	{
	case Tango::DEV_SHORT:
		if (data_format == Tango::SCALAR)
			set_value(&short_val,1,0,false);
		else
			set_value(const_cast<DevShort *>(short_array_val.get_buffer()),w_dim_x,w_dim_y,false);
		break;

	case Tango::DEV_LONG:
		if (data_format == Tango::SCALAR)
			set_value(&long_val,1,0,false);
		else
			set_value(const_cast<DevLong *>(long_array_val.get_buffer()),w_dim_x,w_dim_y,false);
		break;

	case Tango::DEV_LONG64:
		if (data_format == Tango::SCALAR)
			set_value(&w_ext->long64_val,1,0,false);
		else
			set_value(const_cast<DevLong64 *>(w_ext->long64_array_val.get_buffer()),w_dim_x,w_dim_y,false);
		break;

	case Tango::DEV_DOUBLE:
		if (data_format == Tango::SCALAR)
			set_value(&double_val,1,0,false);
		else
			set_value(const_cast<DevDouble *>(double_array_val.get_buffer()),w_dim_x,w_dim_y,false);
		break;

	case Tango::DEV_STRING:
		if (data_format == Tango::SCALAR)
			set_value(&str_val,1,0,false);
		else
			set_value(const_cast<DevString *>(str_array_val.get_buffer()),w_dim_x,w_dim_y,false);
		break;

	case Tango::DEV_FLOAT:
		if (data_format == Tango::SCALAR)
			set_value(&float_val,1,0,false);
		else
			set_value(const_cast<DevFloat *>(float_array_val.get_buffer()),w_dim_x,w_dim_y,false);
		break;

	case Tango::DEV_BOOLEAN:
		if (data_format == Tango::SCALAR)
			set_value(&boolean_val,1,0,false);
		else
			set_value(const_cast<DevBoolean *>(boolean_array_val.get_buffer()),w_dim_x,w_dim_y,false);
		break;

	case Tango::DEV_USHORT:
		if (data_format == Tango::SCALAR)
			set_value(&ushort_val,1,0,false);
		else
			set_value(const_cast<DevUShort *>(ushort_array_val.get_buffer()),w_dim_x,w_dim_y,false);
		break;

	case Tango::DEV_UCHAR:
		if (data_format == Tango::SCALAR)
			set_value(&uchar_val,1,0,false);
		else
			set_value(const_cast<DevUChar *>(uchar_array_val.get_buffer()),w_dim_x,w_dim_y,false);
		break;

	case Tango::DEV_ULONG:
		if (data_format == Tango::SCALAR)
			set_value(&w_ext->ulong_val,1,0,false);
		else
			set_value(const_cast<DevULong *>(w_ext->ulong_array_val.get_buffer()),w_dim_x,w_dim_y,false);
		break;

	case Tango::DEV_ULONG64:
		if (data_format == Tango::SCALAR)
			set_value(&w_ext->ulong64_val,1,0,false);
		else
			set_value(const_cast<DevULong64 *>(w_ext->ulong64_array_val.get_buffer()),w_dim_x,w_dim_y,false);
		break;

	case Tango::DEV_ENCODED:
		set_value(&encoded_val,1,0,false);
		break;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::check_written_value
//
// description : 	Check the value sent by the caller and copy incoming data
//					for SCALAR attribute only
//
// in :			any : Reference to the CORBA Any object
//
//--------------------------------------------------------------------------

void WAttribute::check_written_value(const CORBA::Any &any,unsigned long x,unsigned long y)
{
	CORBA::ULong nb_data;
	unsigned long i;

	switch (data_type)
	{
	case Tango::DEV_SHORT :
        {

//
// Check data type inside the any and data number
//

		const Tango::DevVarShortArray *sh_ptr;
		if ((any >>= sh_ptr) == false)
		{
			TangoSys_OMemStream o;

			o << "Incompatible attribute type, expected type is : Tango::DevVarShortArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::check_written_value()");
		}
		nb_data = sh_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}


//
// Check the incoming value against min or max_value if needed
//

        TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
        {
            AutoTangoMonitor sync1(&mon1);
            if (check_min_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*sh_ptr)[i] < min_value.sh)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is below the minimum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                    o.str(),
                                    (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
            if (check_max_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*sh_ptr)[i] > max_value.sh)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is above the maximum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                     o.str(),
                                    (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
        }

		short_ptr = sh_ptr->get_buffer();
		if (data_format == Tango::SCALAR)
		{
			old_short_val = short_val;
			short_val = (*sh_ptr)[0];
			w_dim_x = 1;
			w_dim_y = 0;
		}
		else
		{
			w_dim_x = x;
			w_dim_y = y;
		}
        }
		break;

	case Tango::DEV_LONG :
        {

//
// Check data type inside the any
//

		const Tango::DevVarLongArray *lg_ptr;
		if ((any >>= lg_ptr) == false)
		{
			TangoSys_OMemStream o;

			o << "Incompatible attribute type, expected type is : Tango::DevVarLongArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::check_written_value()");
		}
		nb_data = lg_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}

//
// Check the incoming value
//

        TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
        {
            AutoTangoMonitor sync1(&mon1);
            if (check_min_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*lg_ptr)[i] < min_value.lg)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is below the minimum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
            if (check_max_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*lg_ptr)[i] > max_value.lg)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is above the maximum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
        }

		long_ptr = lg_ptr->get_buffer();
		if (data_format == Tango::SCALAR)
		{
			old_long_val = long_val;
			long_val = (*lg_ptr)[0];
			w_dim_x = 1;
			w_dim_y = 0;
		}
		else
		{
			w_dim_x = x;
			w_dim_y = y;
		}
        }
		break;


	case Tango::DEV_LONG64 :
        {

//
// Check data type inside the any
//

		const Tango::DevVarLong64Array *lg64_ptr;
		if ((any >>= lg64_ptr) == false)
		{
			TangoSys_OMemStream o;

			o << "Incompatible attribute type, expected type is : Tango::DevVarLong64Array (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::check_written_value()");
		}
		nb_data = lg64_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}

//
// Check the incoming value
//

        TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
        {
            AutoTangoMonitor sync1(&mon1);
            if (check_min_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*lg64_ptr)[i] < min_value.lg64)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is below the minimum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
            if (check_max_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*lg64_ptr)[i] > max_value.lg64)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is above the maximum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
        }

		w_ext->long64_ptr = lg64_ptr->get_buffer();
		if (data_format == Tango::SCALAR)
		{
			w_ext->old_long64_val = w_ext->long64_val;
			w_ext->long64_val = (*lg64_ptr)[0];
			w_dim_x = 1;
			w_dim_y = 0;
		}
		else
		{
			w_dim_x = x;
			w_dim_y = y;
		}
        }
		break;

	case Tango::DEV_DOUBLE :
        {

//
// Check data type inside the any
//

		const Tango::DevVarDoubleArray *db_ptr;
		if ((any >>= db_ptr) == false)
		{
			TangoSys_OMemStream o;

			o << "Incompatible attribute type, expected type is : Tango::DevVarDoubleArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::check_written_value()");
		}
		nb_data = db_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}

//
// Check the incoming value
// First check for NaN, INF
//

        TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
        {
            AutoTangoMonitor sync1(&mon1);
            for (i = 0;i < nb_data;i++)
            {
#ifdef _TG_WINDOWS_
                if (_finite((*db_ptr)[i]) == 0)
#else
                if (isfinite((*db_ptr)[i]) == 0)
#endif
                {
                    TangoSys_OMemStream o;

                    o << "Set value for attribute " << name;
                    o << " is a NaN or INF value (at least element " << i << ")" << ends;
                    Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                              o.str(),
                              (const char *)"WAttribute::check_written_value()");
                }

                if (check_min_value == true)
                {
                    if ((*db_ptr)[i] < min_value.db)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is below the minimum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }

                if (check_max_value == true)
                {
                    if ((*db_ptr)[i] > max_value.db)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is above the maximum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
        }

		double_ptr = db_ptr->get_buffer();
		if (data_format == Tango::SCALAR)
		{
			old_double_val = double_val;
			double_val = (*db_ptr)[0];
			w_dim_x = 1;
			w_dim_y = 0;
		}
		else
		{
			w_dim_x = x;
			w_dim_y = y;
		}
        }
		break;

	case Tango::DEV_STRING :

//
// Check data type inside the any
//

		const Tango::DevVarStringArray *string_ptr;
		if ((any >>= string_ptr) == false)
		{
			TangoSys_OMemStream o;

			o << "Incompatible attribute type, expected type is : Tango::DevVarStringArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::check_written_value()");
		}
		nb_data = string_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}

		str_ptr = string_ptr->get_buffer();

		if (data_format == Tango::SCALAR)
		{
			CORBA::string_free(old_str_val);
			old_str_val = CORBA::string_dup(str_val);
			CORBA::string_free(str_val);

			str_val = CORBA::string_dup((*string_ptr)[0]);
			w_dim_x = 1;
			w_dim_y = 0;
		}
		else
		{
			w_dim_x = x;
			w_dim_y = y;
		}
		break;

	case Tango::DEV_FLOAT :
        {

//
// Check data type inside the any
//

		const Tango::DevVarFloatArray *fl_ptr;
		if ((any >>= fl_ptr) == false)
		{
			TangoSys_OMemStream o;

			o << "Incompatible attribute type, expected type is : Tango::DevVarFloatArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::check_written_value()");
		}
		nb_data = fl_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}

//
// Check the incoming value
// First check for NaN, INF
//

        TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
        {
            AutoTangoMonitor sync1(&mon1);
            for (i = 0;i < nb_data;i++)
            {
#ifdef _TG_WINDOWS_
                if (_finite((*fl_ptr)[i]) == 0)
#else
                if (isfinite((*fl_ptr)[i]) == 0)
#endif
                {
                    TangoSys_OMemStream o;

                    o << "Set value for attribute " << name;
                    o << " is a NaN or INF value (at least element " << i << ")" << ends;
                    Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                              o.str(),
                              (const char *)"WAttribute::check_written_value()");
                }

                if (check_min_value == true)
                {
                    if ((*fl_ptr)[i] < min_value.fl)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is below the minimum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }

                if (check_max_value == true)
                {
                    if ((*fl_ptr)[i] > max_value.fl)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is above the maximum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
        }

		float_ptr = fl_ptr->get_buffer();
		if (data_format == Tango::SCALAR)
		{
			old_float_val = float_val;
			float_val = (*fl_ptr)[0];
			w_dim_x = 1;
			w_dim_y = 0;
		}
		else
		{
			w_dim_x = x;
			w_dim_y = y;
		}
        }
		break;

	case Tango::DEV_USHORT :
        {

//
// Check data type inside the any
//

		const Tango::DevVarUShortArray *ush_ptr;
		if ((any >>= ush_ptr) == false)
		{
			TangoSys_OMemStream o;

			o << "Incompatible attribute type, expected type is : Tango::DevVarUShortArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::check_written_value()");
		}
		nb_data = ush_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}

//
// Check the incoming value
//

        TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
        {
            AutoTangoMonitor sync1(&mon1);
            if (check_min_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*ush_ptr)[i] < min_value.ush)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is below the minimum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
            if (check_max_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*ush_ptr)[i] > max_value.ush)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is above the maximum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
        }

		ushort_ptr = ush_ptr->get_buffer();
		if (data_format == Tango::SCALAR)
		{
			old_ushort_val = ushort_val;
			ushort_val = (*ush_ptr)[0];
			w_dim_x = 1;
			w_dim_y = 0;
		}
		else
		{
			w_dim_x = x;
			w_dim_y = y;
		}
        }
		break;

	case Tango::DEV_UCHAR :
        {

//
// Check data type inside the any
//

		const Tango::DevVarCharArray *uch_ptr;
		if ((any >>= uch_ptr) == false)
		{
			TangoSys_OMemStream o;

			o << "Incompatible attribute type, expected type is : Tango::DevVarCharArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::check_written_value()");
		}
		nb_data = uch_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}

//
// Check the incoming value
//

        TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
        {
            AutoTangoMonitor sync1(&mon1);
            if (check_min_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*uch_ptr)[i] < min_value.uch)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is below the minimum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
            if (check_max_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*uch_ptr)[i] > max_value.uch)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is above the maximum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
        }

		uchar_ptr = uch_ptr->get_buffer();
		if (data_format == Tango::SCALAR)
		{
			old_uchar_val = uchar_val;
			uchar_val = (*uch_ptr)[0];
			w_dim_x = 1;
			w_dim_y = 0;
		}
		else
		{
			w_dim_x = x;
			w_dim_y = y;
		}
        }
		break;

	case Tango::DEV_ULONG :
        {
//
// Check data type inside the any
//

		const Tango::DevVarULongArray *ulo_ptr;
		if ((any >>= ulo_ptr) == false)
		{
			TangoSys_OMemStream o;

			o << "Incompatible attribute type, expected type is : Tango::DevVarULongArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::check_written_value()");
		}
		nb_data = ulo_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}

//
// Check the incoming value
//

        TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
        {
            AutoTangoMonitor sync1(&mon1);
            if (check_min_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*ulo_ptr)[i] < min_value.ulg)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is below the minimum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
            if (check_max_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*ulo_ptr)[i] > max_value.ulg)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is above the maximum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
        }

		w_ext->ulong_ptr = ulo_ptr->get_buffer();
		if (data_format == Tango::SCALAR)
		{
			w_ext->old_ulong_val = w_ext->ulong_val;
			w_ext->ulong_val = (*ulo_ptr)[0];
			w_dim_x = 1;
			w_dim_y = 0;
		}
		else
		{
			w_dim_x = x;
			w_dim_y = y;
		}
        }
		break;

	case Tango::DEV_ULONG64 :
        {
//
// Check data type inside the any
//

		const Tango::DevVarULong64Array *ulg64_ptr;
		if ((any >>= ulg64_ptr) == false)
		{
			TangoSys_OMemStream o;

			o << "Incompatible attribute type, expected type is : Tango::DevVarULong64Array (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::check_written_value()");
		}
		nb_data = ulg64_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}

//
// Check the incoming value
//

        TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
        {
            AutoTangoMonitor sync1(&mon1);
            if (check_min_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*ulg64_ptr)[i] < min_value.ulg64)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is below the minimum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
            if (check_max_value == true)
            {
                for (i = 0;i < nb_data;i++)
                {
                    if ((*ulg64_ptr)[i] > max_value.ulg64)
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is above the maximum authorized (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }
                }
            }
        }

		w_ext->ulong64_ptr = ulg64_ptr->get_buffer();
		if (data_format == Tango::SCALAR)
		{
			w_ext->old_ulong64_val = w_ext->ulong64_val;
			w_ext->ulong64_val = (*ulg64_ptr)[0];
			w_dim_x = 1;
			w_dim_y = 0;
		}
		else
		{
			w_dim_x = x;
			w_dim_y = y;
		}
        }
		break;

	case Tango::DEV_BOOLEAN :

//
// Check data type inside the any
//

		const Tango::DevVarBooleanArray *boo_ptr;
		if ((any >>= boo_ptr) == false)
		{
			TangoSys_OMemStream o;

			o << "Incompatible attribute type, expected type is : Tango::DevVarBooleanArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::check_written_value()");
		}
		nb_data = boo_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::check_written_value()");
			}
		}

		boolean_ptr = boo_ptr->get_buffer();
		if (data_format == Tango::SCALAR)
		{
			old_boolean_val = boolean_val;
			boolean_val = (*boo_ptr)[0];
			w_dim_x = 1;
			w_dim_y = 0;
		}
		else
		{
			w_dim_x = x;
			w_dim_y = y;
		}
		break;
	}
}

void WAttribute::check_written_value(const Tango::AttrValUnion &att_union,unsigned long x,unsigned long y)
{
	unsigned int nb_data;
	unsigned int i;

	switch (data_type)
	{
	case Tango::DEV_SHORT :
		{

//
// Check data type inside the union and data number
//

			if (att_union._d() != ATT_SHORT)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarShortArray (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}

			const Tango::DevVarShortArray &sh_seq = att_union.short_att_value();
			nb_data = sh_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}


//
// Check the incoming value against min or max_value if needed
//

            TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
            {
                AutoTangoMonitor sync1(&mon1);
                if (check_min_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (sh_seq[i] < min_value.sh)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is below the minimum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                        o.str(),
                                        (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
                if (check_max_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (sh_seq[i] > max_value.sh)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is above the maximum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                         o.str(),
                                        (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
            }

			short_ptr = sh_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				old_short_val = short_val;
				short_val = sh_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;

	case Tango::DEV_LONG :
		{
//
// Check data type inside the union
//

			if (att_union._d() != ATT_LONG)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarLongArray (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}
			const Tango::DevVarLongArray &lg_seq = att_union.long_att_value();
			nb_data = lg_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}

//
// Check the incoming value
//

            TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
            {
                AutoTangoMonitor sync1(&mon1);
                if (check_min_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (lg_seq[i] < min_value.lg)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is below the minimum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
                if (check_max_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (lg_seq[i] > max_value.lg)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is above the maximum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
            }

			long_ptr = lg_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				old_long_val = long_val;
				long_val = lg_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;


	case Tango::DEV_LONG64 :
		{
//
// Check data type inside the union
//

			if (att_union._d() != ATT_LONG64)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarLong64Array (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}
			const Tango::DevVarLong64Array &lg64_seq = att_union.long64_att_value();
			nb_data = lg64_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}

//
// Check the incoming value
//

            TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
            {
                AutoTangoMonitor sync1(&mon1);
                if (check_min_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (lg64_seq[i] < min_value.lg64)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is below the minimum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
                if (check_max_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (lg64_seq[i] > max_value.lg64)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is above the maximum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
            }

			w_ext->long64_ptr = lg64_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				w_ext->old_long64_val = w_ext->long64_val;
				w_ext->long64_val = lg64_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;

	case Tango::DEV_DOUBLE :
		{
//
// Check data type inside the union
//

			if (att_union._d() != ATT_DOUBLE)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarDoubleArray (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}
			const Tango::DevVarDoubleArray &db_seq = att_union.double_att_value();
			nb_data = db_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}

//
// Check the incoming value
// First check for NaN, INF
//

            TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
            {
                AutoTangoMonitor sync1(&mon1);
                for (i = 0;i < nb_data;i++)
                {
#ifdef _TG_WINDOWS_
                    if (_finite(db_seq[i]) == 0)
#else
                    if (isfinite(db_seq[i]) == 0)
#endif
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is a NaN or INF value (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }

                    if (check_min_value == true)
                    {
                        if (db_seq[i] < min_value.db)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is below the minimum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }

                    if (check_max_value == true)
                    {
                        if (db_seq[i] > max_value.db)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is above the maximum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
            }

			double_ptr = db_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				old_double_val = double_val;
				double_val = db_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;

	case Tango::DEV_STRING :
		{
//
// Check data type inside the union
//

			if (att_union._d() != ATT_STRING)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarStringArray (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}
			const Tango::DevVarStringArray &string_seq = att_union.string_att_value();
			nb_data = string_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}

			str_ptr = string_seq.get_buffer();

			if (data_format == Tango::SCALAR)
			{
				CORBA::string_free(old_str_val);
				old_str_val = CORBA::string_dup(str_val);
				CORBA::string_free(str_val);

				str_val = CORBA::string_dup(string_seq[0]);
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;

	case Tango::DEV_FLOAT :
		{
//
// Check data type inside the union
//

			if (att_union._d() != ATT_FLOAT)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarFloatArray (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}
			const Tango::DevVarFloatArray &fl_seq = att_union.float_att_value();
			nb_data = fl_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}

//
// Check the incoming value
// First check for NaN, INF
//

            TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
            {
                AutoTangoMonitor sync1(&mon1);
                for (i = 0;i < nb_data;i++)
                {
#ifdef _TG_WINDOWS_
                    if (_finite(fl_seq[i]) == 0)
#else
                    if (isfinite(fl_seq[i]) == 0)
#endif
                    {
                        TangoSys_OMemStream o;

                        o << "Set value for attribute " << name;
                        o << " is a NaN or INF value (at least element " << i << ")" << ends;
                        Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                  o.str(),
                                  (const char *)"WAttribute::check_written_value()");
                    }

                    if (check_min_value == true)
                    {
                        if (fl_seq[i] < min_value.fl)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is below the minimum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }

                    if (check_max_value == true)
                    {
                        if (fl_seq[i] > max_value.fl)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is above the maximum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
            }

			float_ptr = fl_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				old_float_val = float_val;
				float_val = fl_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;

	case Tango::DEV_USHORT :
		{
//
// Check data type inside the union
//

			if (att_union._d() != ATT_USHORT)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarUShortArray (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}
			const Tango::DevVarUShortArray &ush_seq = att_union.ushort_att_value();
			nb_data = ush_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}

//
// Check the incoming value
//

            TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
            {
                AutoTangoMonitor sync1(&mon1);
                if (check_min_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (ush_seq[i] < min_value.ush)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is below the minimum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
                if (check_max_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (ush_seq[i] > max_value.ush)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is above the maximum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
            }

			ushort_ptr = ush_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				old_ushort_val = ushort_val;
				ushort_val = ush_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;

	case Tango::DEV_UCHAR :
		{
//
// Check data type inside the union
//

			if (att_union._d() != ATT_UCHAR)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarCharArray (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}
			const Tango::DevVarCharArray &uch_seq = att_union.uchar_att_value();
			nb_data = uch_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}

//
// Check the incoming value
//

            TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
            {
                AutoTangoMonitor sync1(&mon1);
                if (check_min_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (uch_seq[i] < min_value.uch)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is below the minimum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
                if (check_max_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (uch_seq[i] > max_value.uch)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is above the maximum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
            }

			uchar_ptr = uch_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				old_uchar_val = uchar_val;
				uchar_val = uch_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;

	case Tango::DEV_ULONG :
		{
//
// Check data type inside the union
//

			if (att_union._d() != ATT_ULONG)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarULongArray (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}
			const Tango::DevVarULongArray &ulo_seq = att_union.ulong_att_value();
			nb_data = ulo_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}

//
// Check the incoming value
//

            TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
            {
                AutoTangoMonitor sync1(&mon1);
                if (check_min_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (ulo_seq[i] < min_value.ulg)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is below the minimum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
                if (check_max_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (ulo_seq[i] > max_value.ulg)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is above the maximum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
            }

			w_ext->ulong_ptr = ulo_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				w_ext->old_ulong_val = w_ext->ulong_val;
				w_ext->ulong_val = ulo_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;

	case Tango::DEV_ULONG64 :
		{
//
// Check data type inside the union
//

			if (att_union._d() != ATT_ULONG64)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarULong64Array (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}
			const Tango::DevVarULong64Array &ulo64_seq = att_union.ulong64_att_value();
			nb_data = ulo64_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}

//
// Check the incoming value
//

            TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
            {
                AutoTangoMonitor sync1(&mon1);
                if (check_min_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (ulo64_seq[i] < min_value.ulg64)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is below the minimum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
                if (check_max_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        if (ulo64_seq[i] > max_value.ulg64)
                        {
                            TangoSys_OMemStream o;

                            o << "Set value for attribute " << name;
                            o << " is above the maximum authorized (at least element " << i << ")" << ends;
                            Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                      o.str(),
                                      (const char *)"WAttribute::check_written_value()");
                        }
                    }
                }
            }

			w_ext->ulong64_ptr = ulo64_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				w_ext->old_ulong64_val = w_ext->ulong64_val;
				w_ext->ulong64_val = ulo64_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;

	case Tango::DEV_STATE :
		{
//
// Check data type inside the union
//

			if (att_union._d() != ATT_STATE)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarStateArray (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}
			const Tango::DevVarStateArray &sta_seq = att_union.state_att_value();
			nb_data = sta_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}

//
// Check the incoming value
//

			if (check_min_value == true)
			{
				for (i = 0;i < nb_data;i++)
				{
					if (sta_seq[i] < min_value.d_sta)
					{
						TangoSys_OMemStream o;

						o << "Set value for attribute " << name;
						o << " is below the minimum authorized (at least element " << i << ")" << ends;
						Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						    	  o.str(),
						    	  (const char *)"WAttribute::check_written_value()");
					}
				}
			}
			if (check_max_value == true)
			{
				for (i = 0;i < nb_data;i++)
				{
					if (sta_seq[i] > max_value.d_sta)
					{
						TangoSys_OMemStream o;

						o << "Set value for attribute " << name;
						o << " is above the maximum authorized (at least element " << i << ")" << ends;
						Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						    	  o.str(),
						    	  (const char *)"WAttribute::check_written_value()");
					}
				}
			}

			w_ext->state_ptr = sta_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				w_ext->old_dev_state_val = w_ext->dev_state_val;
				w_ext->dev_state_val = sta_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;

	case Tango::DEV_BOOLEAN :
		{
//
// Check data type inside the union
//

			if (att_union._d() != ATT_BOOL)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarBooleanArray (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}
			const Tango::DevVarBooleanArray &boo_seq = att_union.bool_att_value();
			nb_data = boo_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        	(const char *)"Incorrect data number",
					        	(const char *)"WAttribute::check_written_value()");
				}
			}

			boolean_ptr = boo_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				old_boolean_val = boolean_val;
				boolean_val = boo_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;

	case Tango::DEV_ENCODED :
		{
			if (att_union._d() != ATT_ENCODED)
			{
				TangoSys_OMemStream o;

				o << "Incompatible attribute type, expected type is : Tango::DevVarEncodedArray (even for single value)" << ends;
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					    	  o.str(),
					    	  (const char *)"WAttribute::check_written_value()");
			}

			const Tango::DevVarEncodedArray &enc_seq = att_union.encoded_att_value();
			nb_data = enc_seq.length();
			if (y == 0)
			{
				if (nb_data != x)
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        		(const char *)"Incorrect data number",
					        		(const char *)"WAttribute::check_written_value()");
				}
			}
			else
			{
				if (nb_data != (x * y))
				{
					Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        		(const char *)"Incorrect data number",
					        		(const char *)"WAttribute::check_written_value()");
				}
			}

//
// Check the incoming value against min or max_value if needed
//

            TangoMonitor &mon1 = get_att_device()->get_att_conf_monitor();
            {
                AutoTangoMonitor sync1(&mon1);
                unsigned int j;
                if (check_min_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        CORBA::ULong nb_data_elt = enc_seq[i].encoded_data.length();
                        for (j = 0;j < nb_data_elt;j++)
                        {
                            if (enc_seq[i].encoded_data[j] < min_value.uch)
                            {
                                TangoSys_OMemStream o;

                                o << "Set value for attribute " << name;
                                o << " is below the minimum authorized (at least element " << i << ")" << ends;
                                Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                            o.str(),
                                            (const char *)"WAttribute::check_written_value()");
                            }
                        }
                    }
                }
                if (check_max_value == true)
                {
                    for (i = 0;i < nb_data;i++)
                    {
                        CORBA::ULong nb_data_elt = enc_seq[i].encoded_data.length();
                        for (j = 0;j < nb_data_elt;j++)
                        {
                            if (enc_seq[i].encoded_data[j] > max_value.uch)
                            {
                                TangoSys_OMemStream o;

                                o << "Set value for attribute " << name;
                                o << " is above the maximum authorized (at least element " << i << ")" << ends;
                                Except::throw_exception((const char *)"API_WAttrOutsideLimit",
                                             o.str(),
                                            (const char *)"WAttribute::check_written_value()");
                            }
                        }
                    }
                }
            }

			encoded_ptr = enc_seq.get_buffer();
			if (data_format == Tango::SCALAR)
			{
				old_encoded_val = encoded_val;
				encoded_val = enc_seq[0];
				w_dim_x = 1;
				w_dim_y = 0;
			}
			else
			{
				w_dim_x = x;
				w_dim_y = y;
			}
		}
		break;
	}

}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::get_write_value_length
//
// description : 	Returm to the caller the length of the new value to
//			be written into the attribute
//
//--------------------------------------------------------------------------

long WAttribute::get_write_value_length()
{
	long ret_val;

	if (data_format == Tango::SCALAR)
		ret_val = 1;
	else if (data_format == Tango::SPECTRUM)
		ret_val = w_dim_x;
	else
		ret_val = w_dim_x * w_dim_y;

	return ret_val;
}


//+-------------------------------------------------------------------------
//
// method : 		WAttribute::set_write_value() methods
//
// description : 	Set the attribute internal value.
//			There are different methods according to the
//			attribute data type and the attribute type (scalar,
//       spectrum or image)
//
//--------------------------------------------------------------------------

// DevShort:

void WAttribute::set_write_value(Tango::DevShort val)
{
	CORBA::Any tmp_any;
	Tango::DevVarShortArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	tmp_any <<= tmp_seq;

	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevShort *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarShortArray tmp_seq(nb_data,nb_data,val,false);

	CORBA::Any 	tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<Tango::DevShort> &val, long x, long y)
{
	Tango::DevVarShortArray tmp_seq(val.size(),val.size(),&val[0],false);

	CORBA::Any 	tmp_any;
	tmp_any <<= tmp_seq;

	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

// DevLong:

void WAttribute::set_write_value(Tango::DevLong val)
{
	Tango::DevVarLongArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;

	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevLong *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarLongArray tmp_seq(nb_data,nb_data,val,false);

	CORBA::Any 	tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<Tango::DevLong> &val, long x, long y)
{
	Tango::DevVarLongArray tmp_seq(val.size(),val.size(),&val[0],false);

	CORBA::Any 	tmp_any;
	tmp_any <<= tmp_seq;

	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

// DevLong64:

void WAttribute::set_write_value(Tango::DevLong64 val)
{
	Tango::DevVarLong64Array tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevLong64 *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarLong64Array tmp_seq(nb_data, nb_data, val, false);

	CORBA::Any 	tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<Tango::DevLong64> &val, long x, long y)
{
	Tango::DevVarLong64Array tmp_seq(val.size(),val.size(),&val[0],false);

	CORBA::Any 	tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

// DevDouble:

void WAttribute::set_write_value(Tango::DevDouble val)
{
	Tango::DevVarDoubleArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevDouble *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarDoubleArray tmp_seq(nb_data,nb_data,val,false);

	CORBA::Any 	tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<Tango::DevDouble> &val, long x, long y)
{
	CORBA::Any 	tmp_any;
	Tango::DevVarDoubleArray tmp_seq(val.size(),val.size(),&val[0],false);

	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

// DevString:

void WAttribute::set_write_value(Tango::DevString val)
{
	Tango::DevVarStringArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = CORBA::string_dup(val);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(string &val)
{
	Tango::DevVarStringArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val.c_str();

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevString *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarStringArray tmp_seq(nb_data,nb_data,val,false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<string> &val, long x, long y)
{
	Tango::DevVarStringArray tmp_seq;
	tmp_seq << val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

// DevFloat:

void WAttribute::set_write_value(Tango::DevFloat val)
{
	Tango::DevVarFloatArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevFloat *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarFloatArray tmp_seq(nb_data,nb_data,val,false);
	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<Tango::DevFloat> &val, long x, long y)
{
	Tango::DevVarFloatArray tmp_seq(val.size(),val.size(),&val[0],false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

// DevBoolean:

void WAttribute::set_write_value(Tango::DevBoolean val)
{
	Tango::DevVarBooleanArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevBoolean *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarBooleanArray tmp_seq(nb_data,nb_data,val,false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<Tango::DevBoolean> &val, long x, long y)
{
	Tango::DevVarBooleanArray tmp_seq;
	tmp_seq << val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

// DevUShort:

void WAttribute::set_write_value(Tango::DevUShort val)
{
	Tango::DevVarUShortArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevUShort *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarUShortArray tmp_seq(nb_data,nb_data,val,false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<Tango::DevUShort> &val, long x, long y)
{
	Tango::DevVarUShortArray tmp_seq(val.size(),val.size(),&val[0],false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

// DevUChar:

void WAttribute::set_write_value(Tango::DevUChar val)
{
	Tango::DevVarCharArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevUChar *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarUCharArray tmp_seq(nb_data,nb_data,val,false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<Tango::DevUChar> &val, long x, long y)
{
	Tango::DevVarUCharArray tmp_seq(val.size(),val.size(),&val[0],false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

// DevULong:

void WAttribute::set_write_value(Tango::DevULong val)
{
	Tango::DevVarULongArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevULong *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarULongArray tmp_seq(nb_data,nb_data,val,false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<Tango::DevULong> &val, long x, long y)
{
	Tango::DevVarULongArray tmp_seq(val.size(),val.size(),&val[0],false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

// DevULong64:

void WAttribute::set_write_value(Tango::DevULong64 val)
{
	Tango::DevVarULong64Array tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevULong64 *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarULong64Array tmp_seq(nb_data,nb_data,val,false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<Tango::DevULong64> &val, long x, long y)
{
	Tango::DevVarULong64Array tmp_seq(val.size(),val.size(),&val[0],false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

// DevState:

void WAttribute::set_write_value(Tango::DevState val)
{
	Tango::DevVarStateArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevState *val, long x, long y)
{
	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	Tango::DevVarStateArray tmp_seq(nb_data,nb_data,val,false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(vector<Tango::DevState> &val, long x, long y)
{
	Tango::DevVarStateArray tmp_seq(val.size(),val.size(),&val[0],false);

	CORBA::Any tmp_any;
	tmp_any <<= tmp_seq;
	check_written_value(tmp_any, x, y);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

void WAttribute::set_write_value(Tango::DevEncoded *, TANGO_UNUSED(long x),TANGO_UNUSED(long y))
{

//
// Dummy method just to make compiler happy when using fill_attr_polling_buffer for DevEncoded
// attribute
// Should never be called
//

	Tango::Except::throw_exception((const char *)"API_NotSupportedFeature",
								  (const char *)"This is a not supported call in case of DevEncoded attribute",
								  (const char *)"Wattribute::set_write_value()");
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::rollback
//
// description : 	Reset the internal data to its value before the
//			set_write_value method was applied (Useful in case of
//			error in the set_write_value method)
//
//--------------------------------------------------------------------------

void WAttribute::rollback()
{
	switch (data_type)
	{
	case Tango::DEV_SHORT :
		short_val = old_short_val;
		break;

	case Tango::DEV_LONG :
		long_val = old_long_val;
		break;

	case Tango::DEV_LONG64 :
		w_ext->long64_val = w_ext->old_long64_val;
		break;

	case Tango::DEV_DOUBLE :
		double_val = old_double_val;
		break;

	case Tango::DEV_STRING :
		CORBA::string_free(str_val);
		str_val = CORBA::string_dup(old_str_val);
		break;

	case Tango::DEV_FLOAT :
		float_val = old_float_val;
		break;

	case Tango::DEV_BOOLEAN :
		boolean_val = old_boolean_val;
		break;

	case Tango::DEV_USHORT :
		double_val = old_double_val;
		break;

	case Tango::DEV_UCHAR :
		CORBA::string_free(str_val);
		break;

	case Tango::DEV_ULONG :
		w_ext->ulong_val = w_ext->old_ulong_val;
		break;

	case Tango::DEV_ULONG64 :
		w_ext->ulong64_val = w_ext->old_ulong64_val;
		break;

	case Tango::DEV_STATE :
		w_ext->dev_state_val = w_ext->old_dev_state_val;
		break;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::copy_data
//
// description : 	Copy data into the attribute object in order to return
//			them in case of a read on this attribute
//
// in :			any : Reference to the CORBA Any object
//
//--------------------------------------------------------------------------

void WAttribute::copy_data(const CORBA::Any &any)
{
	switch (data_type)
	{
	case Tango::DEV_SHORT :
		const Tango::DevVarShortArray *sh_ptr;
		any >>= sh_ptr;
		short_array_val = *sh_ptr;
		break;

	case Tango::DEV_LONG :
		const Tango::DevVarLongArray *lo_ptr;
		any >>= lo_ptr;
		long_array_val = *lo_ptr;
		break;

	case Tango::DEV_LONG64 :
		const Tango::DevVarLong64Array *lo64_ptr;
		any >>= lo64_ptr;
		w_ext->long64_array_val = *lo64_ptr;
		break;

	case Tango::DEV_DOUBLE :
		const Tango::DevVarDoubleArray *db_ptr;
		any >>= db_ptr;
		double_array_val = *db_ptr;
		break;

	case Tango::DEV_STRING :
		const Tango::DevVarStringArray *tmp_str_ptr;
		any >>= tmp_str_ptr;
		str_array_val = *tmp_str_ptr;
		break;

	case Tango::DEV_FLOAT :
		const Tango::DevVarFloatArray *fl_ptr;
		any >>= fl_ptr;
		float_array_val = *fl_ptr;
		break;

	case Tango::DEV_BOOLEAN :
		const Tango::DevVarBooleanArray *boo_ptr;
		any >>= boo_ptr;
		boolean_array_val = *boo_ptr;
		break;

	case Tango::DEV_USHORT :
		const Tango::DevVarUShortArray *ush_ptr;
		any >>= ush_ptr;
		ushort_array_val = *ush_ptr;
		break;

	case Tango::DEV_UCHAR :
		const Tango::DevVarCharArray *uch_ptr;
		any >>= uch_ptr;
		uchar_array_val = *uch_ptr;
		break;

	case Tango::DEV_ULONG :
		const Tango::DevVarULongArray *ulo_ptr;
		any >>= ulo_ptr;
		w_ext->ulong_array_val = *ulo_ptr;
		break;

	case Tango::DEV_ULONG64 :
		const Tango::DevVarULong64Array *ulo64_ptr;
		any >>= ulo64_ptr;
		w_ext->ulong64_array_val = *ulo64_ptr;
		break;

	case Tango::DEV_STATE :
		const Tango::DevVarStateArray *sta_ptr;
		any >>= sta_ptr;
		w_ext->state_array_val = *sta_ptr;
		break;
	}
}


void WAttribute::copy_data(const Tango::AttrValUnion &the_union)
{
	switch (data_type)
	{
	case Tango::DEV_SHORT :
		short_array_val = the_union.short_att_value();
		break;

	case Tango::DEV_LONG :
		long_array_val = the_union.long_att_value();
		break;

	case Tango::DEV_LONG64 :
		w_ext->long64_array_val = the_union.long64_att_value();
		break;

	case Tango::DEV_DOUBLE :
		double_array_val = the_union.double_att_value();
		break;

	case Tango::DEV_STRING :
		str_array_val = the_union.string_att_value();
		break;

	case Tango::DEV_FLOAT :
		float_array_val = the_union.float_att_value();
		break;

	case Tango::DEV_BOOLEAN :
		boolean_array_val = the_union.bool_att_value();
		break;

	case Tango::DEV_USHORT :
		ushort_array_val = the_union.ushort_att_value();
		break;

	case Tango::DEV_UCHAR :
		uchar_array_val = the_union.uchar_att_value();
		break;

	case Tango::DEV_ULONG :
		w_ext->ulong_array_val = the_union.ulong_att_value();
		break;

	case Tango::DEV_ULONG64 :
		w_ext->ulong64_array_val = the_union.ulong64_att_value();
		break;

	case Tango::DEV_STATE :
		w_ext->state_array_val = the_union.state_att_value();
		break;
	}
}
//+-------------------------------------------------------------------------
//
// method : 		WAttribute::set_written_date
//
// description : 	Memorized when the attribute is written
//
//--------------------------------------------------------------------------

void WAttribute::set_written_date()
{
#ifdef _TG_WINDOWS_
	struct _timeb t;
	_ftime(&t);

	write_date.tv_sec = (CORBA::Long)t.time;
	write_date.tv_usec = (CORBA::Long)(t.millitm * 1000);
#else
	struct timezone tz;
	struct timeval tv;
	gettimeofday(&tv,&tz);

	write_date.tv_sec = (CORBA::Long)tv.tv_sec;
	write_date.tv_usec = (CORBA::Long)tv.tv_usec;
#endif
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::check_rds_alarm
//
// description : 	Check if the attribute is in read different from set
//			alarm.
//
// This method returns true if the attribute has a read too different than the
// the last set value. Otherwise, returns false.
//
//--------------------------------------------------------------------------

bool WAttribute::check_rds_alarm()
{
	bool ret = false;

//
// Return immediately if the attribute has never been written
//

	if (write_date.tv_sec == 0)
		return false;

//
// First, check if it is necessary to check attribute value
// Give some time to the device to change its output
//

	struct timeval tv;
#ifdef _TG_WINDOWS_
	struct _timeb t;
	_ftime(&t);

	tv.tv_sec = (CORBA::Long)t.time;
	tv.tv_usec = (CORBA::Long)(t.millitm * 1000);
#else
	struct timezone tz;
	gettimeofday(&tv,&tz);
#endif

	long time_diff;
	COMPUTE_TIME_DIFF(time_diff,write_date,tv);

	if (time_diff >= delta_t)
	{

//
// Now check attribute value with again a switch on attribute data type
//

		long nb_written,nb_read,nb_data,i;

		switch (data_type)
		{
		case Tango::DEV_SHORT:
			nb_written = short_array_val.length();
			nb_read = (data_format == Tango::SCALAR) ? 1 : value.sh_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				short delta = (data_format == Tango::SCALAR) ? short_array_val[0] - tmp_sh[0] : short_array_val[i] - (*value.sh_seq)[i];
				if (abs(delta) >= delta_val.sh)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(rds);
					ret = true;
					break;
				}
			}
			break;

		case Tango::DEV_LONG:
			nb_written = long_array_val.length();
			nb_read = (data_format == Tango::SCALAR) ? 1 : value.lg_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				DevLong delta = (data_format == Tango::SCALAR) ? long_array_val[0] - tmp_lo[0] : long_array_val[i] - (*value.lg_seq)[i];
				if (abs(delta) >= delta_val.lg)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(rds);
					ret = true;
					break;
				}
			}
			break;

		case Tango::DEV_LONG64:
			nb_written = w_ext->long64_array_val.length();
			nb_read = (data_format == Tango::SCALAR) ? 1 : value.lg64_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				DevLong64 delta = (data_format == Tango::SCALAR) ? w_ext->long64_array_val[0] - get_tmp_scalar_long64()[0] : w_ext->long64_array_val[i] - (*value.lg64_seq)[i];

				DevLong64 abs_delta;
				if (delta < 0)
					abs_delta = -delta;
				else
					abs_delta = delta;

				if (abs_delta >= delta_val.lg64)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(rds);
					ret = true;
					break;
				}
			}
			break;

		case Tango::DEV_DOUBLE:
			nb_written = double_array_val.length();
			nb_read = (data_format == Tango::SCALAR) ? 1 : value.db_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				// check for NAN values
				if ( data_format == Tango::SCALAR )
				{
					if ( Tango_isnan(double_array_val[0]) || Tango_isnan(tmp_db[0]) )
					{
						// send an alarm if only read or set value are NAN
						if ( !(Tango_isnan(double_array_val[0]) && Tango_isnan(tmp_db[0])) )
						{
							quality = Tango::ATTR_ALARM;
							alarm.set(rds);
							ret = true;
							break;
						}
					}
				}
				else
				{
					if ( Tango_isnan(double_array_val[i]) || Tango_isnan((*value.db_seq)[i]) )
					{
						// send an alarm if only read or set value are NAN
						if ( !(Tango_isnan(double_array_val[i]) && Tango_isnan((*value.db_seq)[i])) )
						{
							quality = Tango::ATTR_ALARM;
							alarm.set(rds);
							ret = true;
							break;
						}
					}
				}


				double delta = (data_format == Tango::SCALAR) ? double_array_val[0] - tmp_db[0] : double_array_val[i] - (*value.db_seq)[i];
				if (fabs(delta) >= delta_val.db)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(rds);
					ret = true;
					break;
				}
			}
			break;

		case Tango::DEV_FLOAT:
			nb_written = float_array_val.length();
			nb_read = (data_format == Tango::SCALAR) ? 1 : value.fl_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				// check for NAN values
				if ( data_format == Tango::SCALAR )
				{
					if ( Tango_isnan(float_array_val[0]) || Tango_isnan(tmp_fl[0]) )
					{
						// send an alarm if only read or set value are NAN
						if ( !(Tango_isnan(float_array_val[0]) && Tango_isnan(tmp_fl[0])) )
						{
							quality = Tango::ATTR_ALARM;
							alarm.set(rds);
							ret = true;
							break;
						}
					}
				}
				else
				{
					if ( Tango_isnan(float_array_val[i]) || Tango_isnan((*value.fl_seq)[i]) )
					{
						// send an alarm if only read or set value are NAN
						if ( !(Tango_isnan(float_array_val[i]) && Tango_isnan((*value.fl_seq)[i])) )
						{
							quality = Tango::ATTR_ALARM;
							alarm.set(rds);
							ret = true;
							break;
						}
					}
				}

				float delta = (data_format == Tango::SCALAR) ? float_array_val[0] - tmp_fl[0] : float_array_val[i] - (*value.fl_seq)[i];
				double delta_d = (double)delta;
				if (((float)fabs(delta_d)) >= delta_val.fl)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(rds);
					ret = true;
					break;
				}
			}
			break;

		case Tango::DEV_USHORT:
			nb_written = ushort_array_val.length();
			nb_read = (data_format == Tango::SCALAR) ? 1 : value.ush_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				unsigned short delta = (data_format == Tango::SCALAR) ? ushort_array_val[0] - tmp_ush[0] : ushort_array_val[i] - (*value.ush_seq)[i];
				if (abs(delta) >= delta_val.ush)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(rds);
					ret = true;
					break;
				}
			}
			break;

		case Tango::DEV_UCHAR:
			nb_written = uchar_array_val.length();
			nb_read = (data_format == Tango::SCALAR) ? 1 : value.cha_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				unsigned char delta = (data_format == Tango::SCALAR) ? uchar_array_val[0] - tmp_cha[0] : uchar_array_val[i] - (*value.cha_seq)[i];
				if (abs(delta) >= delta_val.uch)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(rds);
					ret = true;
					break;
				}
			}
			break;

		case Tango::DEV_ULONG:
			nb_written = w_ext->ulong_array_val.length();
			nb_read = (data_format == Tango::SCALAR) ? 1 : value.ulg_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				DevLong delta = (data_format == Tango::SCALAR) ? w_ext->ulong_array_val[0] - get_tmp_scalar_ulong()[0] : w_ext->ulong_array_val[i] - (*value.ulg_seq)[i];
				if ((unsigned int)abs(delta) >= delta_val.ulg)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(rds);
					ret = true;
					break;
				}
			}
			break;

		case Tango::DEV_ULONG64:
			nb_written = w_ext->ulong64_array_val.length();
			nb_read = (data_format == Tango::SCALAR) ? 1 : value.ulg64_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				DevLong64 delta = (data_format == Tango::SCALAR) ? w_ext->ulong64_array_val[0] - get_tmp_scalar_ulong64()[0] : w_ext->ulong64_array_val[i] - (*value.ulg64_seq)[i];

				DevULong64 abs_delta;
				if (delta < 0)
					abs_delta = -delta;
				else
					abs_delta = delta;

				if (abs_delta >= delta_val.ulg64)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(rds);
					ret = true;
					break;
				}
			}
			break;

		case Tango::DEV_ENCODED:
			nb_written = ::strlen(encoded_val.encoded_format.in());
			nb_read = ::strlen((*value.enc_seq)[0].encoded_format.in());
			if (nb_written != nb_read)
			{
				quality = Tango::ATTR_ALARM;
				alarm.set(rds);
				ret = true;
				break;
			}
			if (::strcmp(encoded_val.encoded_format.in(),(*value.enc_seq)[0].encoded_format.in()) != 0)
			{
				quality = Tango::ATTR_ALARM;
				alarm.set(rds);
				ret = true;
				break;
			}

			nb_written = encoded_val.encoded_data.length();
			nb_read = (*value.enc_seq)[0].encoded_data.length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				unsigned char delta = encoded_val.encoded_data[i] - (*value.enc_seq)[0].encoded_data[i];
				if (abs(delta) >= delta_val.uch)
				{
					quality = Tango::ATTR_ALARM;
					alarm.set(rds);
					ret = true;
					break;
				}
			}
			break;
		}
	}

	return ret;
}


void WAttribute::set_min_value(char *new_min_value_str)
{
	set_min_value(string(new_min_value_str));
}

void WAttribute::set_min_value(const char *new_min_value_str)
{
	set_min_value(string(new_min_value_str));
}


void WAttribute::set_max_value(char *new_max_value_str)
{
	set_max_value(string(new_max_value_str));
}

void WAttribute::set_max_value(const char *new_max_value_str)
{
	set_max_value(string(new_max_value_str));
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::mem_value_below_above()
//
// description : 	Check if the attribute last written value is below
//                  (or above) the new threshold sent by the requester
//
// Arg in :			check_type : Which check has to be done: Below or above
//
// This method returns true if the new threshold wanted by the user is not
// coherent with the memorized value. Otherwise, returns false.
//
//--------------------------------------------------------------------------

bool WAttribute::mem_value_below_above(MinMaxValueCheck check_type,string &mem_value)
{
	bool ret = false;

//
// Check last written attribute value with the new threshold
//

    long nb_written,i;
    stringstream ss;

    switch (data_type)
    {
    case Tango::DEV_SHORT:
        nb_written = short_array_val.length();
        for (i = 0;i < nb_written;i++)
        {
            if (check_type == MIN)
            {
                if (short_array_val[i] < min_value.sh)
                {
                    ss << short_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
            else
            {
                if (short_array_val[i] > max_value.sh)
                {
                    ss << short_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
        }
        break;

    case Tango::DEV_LONG:
        nb_written = long_array_val.length();
        for (i = 0;i < nb_written;i++)
        {
            if (check_type == MIN)
            {
                if (long_array_val[i] < min_value.lg)
                {
                    ss << long_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
            else
            {
                if (long_array_val[i] > max_value.lg)
                {
                    ss << long_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
        }
        break;

    case Tango::DEV_LONG64:
        nb_written = w_ext->long64_array_val.length();
        for (i = 0;i < nb_written;i++)
        {
            if (check_type == MIN)
            {
                if (w_ext->long64_array_val[i] < min_value.lg64)
                {
                    ss << w_ext->long64_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
            else
            {
                if (w_ext->long64_array_val[i] > max_value.lg64)
                {
                    ss << w_ext->long64_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
        }
        break;

    case Tango::DEV_DOUBLE:
        nb_written = double_array_val.length();
        for (i = 0;i < nb_written;i++)
        {
            if (check_type == MIN)
            {
                if (double_array_val[i] < min_value.db)
                {
                    ss << double_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
            else
            {
                if (double_array_val[i] > max_value.db)
                {
                    ss << double_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
        }
        break;

    case Tango::DEV_FLOAT:
        nb_written = float_array_val.length();
        for (i = 0;i < nb_written;i++)
        {
            if (check_type == MIN)
            {
                if (float_array_val[i] < min_value.fl)
                {
                    ss << float_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
            else
            {
                if (float_array_val[i] > max_value.fl)
                {
                    ss << float_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
        }
        break;

    case Tango::DEV_USHORT:
        nb_written = ushort_array_val.length();
        for (i = 0;i < nb_written;i++)
        {
            if (check_type == MIN)
            {
                if (ushort_array_val[i] < min_value.ush)
                {
                    ss << ushort_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
            else
            {
                if (ushort_array_val[i] > max_value.ush)
                {
                    ss << ushort_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
        }
        break;

    case Tango::DEV_UCHAR:
        nb_written = uchar_array_val.length();
        for (i = 0;i < nb_written;i++)
        {
            if (check_type == MIN)
            {
                if (uchar_array_val[i] < min_value.uch)
                {
                    ss << uchar_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
            else
            {
                if (uchar_array_val[i] > max_value.uch)
                {
                    ss << uchar_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
        }
        break;

    case Tango::DEV_ULONG:
        nb_written = w_ext->ulong_array_val.length();
        for (i = 0;i < nb_written;i++)
        {
            if (check_type == MIN)
            {
                if (w_ext->ulong_array_val[i] < min_value.ulg)
                {
                    ss << w_ext->ulong_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
            else
            {
                if (w_ext->ulong_array_val[i] > max_value.ulg)
                {
                    ss << w_ext->ulong_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
        }
        break;

    case Tango::DEV_ULONG64:
        nb_written = w_ext->ulong64_array_val.length();
        for (i = 0;i < nb_written;i++)
        {
            if (check_type == MIN)
            {
                if (w_ext->ulong64_array_val[i] < min_value.ulg64)
                {
                    ss << w_ext->ulong64_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
            else
            {
                if (w_ext->ulong64_array_val[i] > max_value.ulg64)
                {
                    ss << w_ext->ulong64_array_val[i];
                    mem_value = ss.str();
                    ret = true;
                    break;
                }
            }
        }
        break;

    default:
        break;
    }

	return ret;
}


} // End of Tango namespace
