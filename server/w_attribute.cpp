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
// $Revision$
//
// $Log$
// Revision 1.8  2005/05/25 06:05:40  taurel
// - Again some minors changes before 5.2
//
// Revision 1.7  2005/05/20 06:25:13  taurel
// - Small changes for VC7
//
// Revision 1.6  2005/05/10 16:10:18  taurel
// - Fix windows VC7 warnings
//
// Revision 1.5  2005/05/04 11:50:30  taurel
// - Changes for 32<-->64 bits data exchange
//
// Revision 1.4  2005/03/03 15:36:26  taurel
// - Change in attribute quality factor change event. It is now fired by the Attribute
// set_quailty() and set_value_date_quality() methods. For scannable device.
//
// Revision 1.3  2005/01/13 09:27:54  taurel
// Fix some bugs :
// - R/W attribute : W value not returned when read if set by set_write_value
// - Core dumped when retrieving attribute polling history for Device_2Impl device which
//   has stored an exception
// - Remove device_name in lib default attribute label property
// - Lib default value for label not store in db any more
// - Size of the DaData used by the Database::get_device_attribute_property() and
//   Database::get_class_attribute_property()
// - R/W attribute: W part not returned when read for Device_2Impl device
// Some changes :
// - Improvement of the -file option error management (now throw exception in case of
//   error)
// - Reset "string" attribute property to the default value (lib or user) when new
//   value is an empty string
//
// Revision 1.1.2.3  2004/10/27 05:59:47  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 1.1.2.2  2004/09/27 09:10:06  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 1.1.2.1  2004/09/15 06:48:06  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
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

#ifdef WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* WIN32 */
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
:Attribute(prop_list,tmp_attr,dev_name,idx),memorized(false),memorized_init(true),w_ext(NULL)
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
	str_val = CORBA::string_dup("Not initialised");
	old_str_val = CORBA::string_dup("Not initialised");
	
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
	
				
	short_ptr = &short_val;
	w_dim_x = w_dim_y = 0;
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
	CORBA::string_free(str_val);
	CORBA::string_free(old_str_val);
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
	}
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::set_write_value
// 
// description : 	Set the value inside the Wattribute object from
//			the Any object received from the network. Some special
//			treatement for string due to memory allocation.
//
// in :			any : Reference to the CORBA Any object
//
//--------------------------------------------------------------------------

void WAttribute::set_write_value(const CORBA::Any &any,unsigned long x,unsigned long y)
{
	CORBA::ULong nb_data;
	unsigned long i;
	
	switch (data_type)
	{
	case Tango::DEV_SHORT :
	
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
					      (const char *)"WAttribute::set_write_value()");
		}
		nb_data = sh_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");			
			}
		}
		
		
//
// Check the incoming value against min or max_value if needed
//

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
						      	(const char *)"WAttribute::set_write_value()");
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
						      	(const char *)"WAttribute::set_write_value()");
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
		
		break;
		
	case Tango::DEV_LONG :
	
//
// Check data type inside the any
//

		const Tango::DevVarLongArray *lg_ptr;
		if ((any >>= lg_ptr) == false)
		{
			TangoSys_OMemStream o;

#ifdef TANGO_LONG32
			const Tango::DevVarLongLongArray *lglg_ptr;
			if ((any >>= lglg_ptr) == true)
				o << "Cannot extract data, data sent by a 64 bits computer" << ends;
			else
				o << "Incompatible attribute type, expected type is : Tango::DevVarLongArray (even for single value)" << ends;

			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::set_write_value()");
#else
			const Tango::DevVarLong32Array *lg32_ptr;
			if ((any >>= lg32_ptr) == false)
			{
				o << "Incompatible attribute type, expected type is : Tango::DevVarLongArray (even for single value)" << ends;
				
				Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::set_write_value()");
			}
			else
			{
				static Tango::DevVarLongArray *tmp;
				if (tmp != NULL)
					delete tmp;
				tmp = new Tango::DevVarLongArray;
			
				unsigned int nb_data = lg32_ptr->length();
				tmp->length(nb_data);
				const int *in_ptr = reinterpret_cast<const int *>(lg32_ptr->get_buffer());
				for (unsigned int l = 0;l < nb_data;l++)
				{
					(*tmp)[l] = in_ptr[l];
				}
			
				lg_ptr = tmp;
			}
#endif

		}
		nb_data = lg_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");			
			}
		}
		
//
// Check the incoming value
//

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
						      (const char *)"WAttribute::set_write_value()");
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
						      (const char *)"WAttribute::set_write_value()");
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

		break;
		
	case Tango::DEV_DOUBLE :
	
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
					      (const char *)"WAttribute::set_write_value()");
		}
		nb_data = db_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");			
			}
		}
				
//
// Check the incoming value
//

		if (check_min_value == true)
		{
			for (i = 0;i < nb_data;i++)
			{
				if ((*db_ptr)[i] < min_value.db)
				{
					TangoSys_OMemStream o;
	
					o << "Set value for attribute " << name;
					o << " is below the minimum authorized (at least element " << i << ")" << ends;
					Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						      o.str(),
						      (const char *)"WAttribute::set_write_value()");
				}
			}
		}
		if (check_max_value == true)
		{
			for (i = 0;i < nb_data;i++)
			{
				if ((*db_ptr)[i] > max_value.db)
				{
					TangoSys_OMemStream o;
	
					o << "Set value for attribute " << name;
					o << " is above the maximum authorized (at least element " << i << ")" << ends;
					Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						      o.str(),
						      (const char *)"WAttribute::set_write_value()");
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
					      (const char *)"WAttribute::set_write_value()");
		}
		nb_data = string_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");			
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
					      (const char *)"WAttribute::set_write_value()");
		}
		nb_data = fl_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");			
			}
		}
				
//
// Check the incoming value
//

		if (check_min_value == true)
		{
			for (i = 0;i < nb_data;i++)
			{
				if ((*fl_ptr)[i] < min_value.fl)
				{
					TangoSys_OMemStream o;
	
					o << "Set value for attribute " << name;
					o << " is below the minimum authorized (at least element " << i << ")" << ends;
					Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						      o.str(),
						      (const char *)"WAttribute::set_write_value()");
				}
			}
		}
		if (check_max_value == true)
		{
			for (i = 0;i < nb_data;i++)
			{
				if ((*fl_ptr)[i] > max_value.fl)
				{
					TangoSys_OMemStream o;
	
					o << "Set value for attribute " << name;
					o << " is above the maximum authorized (at least element " << i << ")" << ends;
					Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						      o.str(),
						      (const char *)"WAttribute::set_write_value()");
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
		break;
		
	case Tango::DEV_USHORT :
	
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
					      (const char *)"WAttribute::set_write_value()");
		}
		nb_data = ush_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");			
			}
		}
				
//
// Check the incoming value
//

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
						      (const char *)"WAttribute::set_write_value()");
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
						      (const char *)"WAttribute::set_write_value()");
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
		break;
		
	case Tango::DEV_UCHAR :
	
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
					      (const char *)"WAttribute::set_write_value()");
		}
		nb_data = uch_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");			
			}
		}
				
//
// Check the incoming value
//

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
						      (const char *)"WAttribute::set_write_value()");
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
						      (const char *)"WAttribute::set_write_value()");
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
					      (const char *)"WAttribute::set_write_value()");
		}
		nb_data = boo_ptr->length();
		if (y == 0)
		{
			if (nb_data != x)
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");
			}
		}
		else
		{
			if (nb_data != (x * y))
			{
				Except::throw_exception((const char *)"API_AttrIncorrectDataNumber",
					        (const char *)"Incorrect data number",
					        (const char *)"WAttribute::set_write_value()");			
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
//			There are different methods according to 
//			attribute data type
//
//--------------------------------------------------------------------------

void WAttribute::set_write_value(Tango::DevShort val)
{
	CORBA::Any tmp_any;
	Tango::DevVarShortArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;
	tmp_any <<= tmp_seq;
	
	set_write_value(tmp_any,1,0);
	copy_data(tmp_any);	
}

void WAttribute::set_write_value(Tango::DevLong val)
{
	CORBA::Any tmp_any;
	Tango::DevVarLongArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;
	tmp_any <<= tmp_seq;
	
	set_write_value(tmp_any,1,0);
	copy_data(tmp_any);
}

void WAttribute::set_write_value(Tango::DevDouble val)
{
	CORBA::Any tmp_any;
	Tango::DevVarDoubleArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;
	tmp_any <<= tmp_seq;
	
	set_write_value(tmp_any,1,0);
	copy_data(tmp_any);
}

void WAttribute::set_write_value(Tango::DevString val)
{
	CORBA::Any tmp_any;
	Tango::DevVarStringArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = CORBA::string_dup(val);
	tmp_any <<= tmp_seq;
	
	set_write_value(tmp_any,1,0);
	copy_data(tmp_any);
}

void WAttribute::set_write_value(string &val)
{
	CORBA::Any tmp_any;
	Tango::DevVarStringArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val.c_str();
	tmp_any <<= tmp_seq;

	set_write_value(tmp_any,1,0);
	copy_data(tmp_any);
}

void WAttribute::set_write_value(Tango::DevFloat val)
{
	CORBA::Any tmp_any;
	Tango::DevVarFloatArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;
	tmp_any <<= tmp_seq;
	
	set_write_value(tmp_any,1,0);
	copy_data(tmp_any);	
}

void WAttribute::set_write_value(Tango::DevBoolean val)
{
	CORBA::Any tmp_any;
	Tango::DevVarBooleanArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;
	tmp_any <<= tmp_seq;
	
	set_write_value(tmp_any,1,0);
	copy_data(tmp_any);
}

void WAttribute::set_write_value(Tango::DevUShort val)
{
	CORBA::Any tmp_any;
	Tango::DevVarUShortArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;
	tmp_any <<= tmp_seq;
	
	set_write_value(tmp_any,1,0);
	copy_data(tmp_any);
}

void WAttribute::set_write_value(Tango::DevUChar val)
{
	CORBA::Any tmp_any;
	Tango::DevVarCharArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;
	tmp_any <<= tmp_seq;
	
	set_write_value(tmp_any,1,0);
	copy_data(tmp_any);
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
		if ((any >>= lo_ptr) == false)
		{
#ifdef TANGO_LONG64
			const Tango::DevVarLong32Array *lg32_ptr;
			any >>= lg32_ptr;
			
			unsigned int nb_data = lg32_ptr->length();
			long_array_val.length(nb_data);
			const int *in_ptr = reinterpret_cast<const int *>(lg32_ptr->get_buffer());
			for (unsigned int l = 0;l < nb_data;l++)
			{
				long_array_val[l] = in_ptr[l];
			}
#endif
		}
		else
			long_array_val = *lo_ptr;	
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
#ifdef WIN32
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
#ifdef WIN32
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
			nb_read = value.sh_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				short delta = short_array_val[i] - (*value.sh_seq)[i];
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
			nb_read = value.lg_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				long delta = long_array_val[i] - (*value.lg_seq)[i];
				if (abs(delta) >= delta_val.lg)
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
			nb_read = value.db_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				double delta = double_array_val[i] - (*value.db_seq)[i];
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
			nb_read = value.fl_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				float delta = float_array_val[i] - (*value.fl_seq)[i];
#if ((defined __SUNPRO_CC) || (defined WIN32) || (defined GCC_SOLARIS))
				double delta_d = (double)delta;
				if (((float)fabs(delta_d)) >= delta_val.fl)
#else
				if (fabsf(delta) >= delta_val.fl)
#endif
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
			nb_read = value.ush_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				unsigned short delta = ushort_array_val[i] - (*value.ush_seq)[i];
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
			nb_read = value.cha_seq->length();
			nb_data = (nb_written > nb_read) ? nb_read : nb_written;
			for (i = 0;i < nb_data;i++)
			{
				unsigned char delta = uchar_array_val[i] - (*value.cha_seq)[i];
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

} // End of Tango namespace
