static const char *RcsId = "$Id$";

//====================================================================================================================
//
// file :               attrsetval.cpp
//
// description :        C++ source code for the Attribute class set_value() method family.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2013
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or
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
//====================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <attribute.h>
#include <classattribute.h>
#include <eventsupplier.h>

#include <functional>
#include <algorithm>

#ifdef _TG_WINDOWS_
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* _TG_WINDOWS_ */
#include <math.h>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_value
//
// description : 	Set the attribute read value and quality. This method
//			automatically set the date when it has been called
//			This method is overloaded several times for all the
//			supported attribute data type
//
// in :			p_data : The attribute read value
//			x : The attribute x dimension (default is 1)
//			y : The atttribute y dimension (default is 0)
//			release : A flag set to true if memory must be
//				  de-allocated (default is false)
//
//--------------------------------------------------------------------------

void Attribute::set_value(Tango::DevShort *p_data,long x,long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_SHORT)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,
				      o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,
				      o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_sh[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.sh_seq = new Tango::DevVarShortArray(data_size);
				value.sh_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.sh_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevShort *tmp_ptr = new Tango::DevShort[1];
				*tmp_ptr = *p_data;
				value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete [] p_data;
				else
					delete p_data;
			}
			else
				value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevLong *p_data,long x,long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_LONG)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,
				      o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.lg_seq = new Tango::DevVarLongArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_lo[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.lg_seq = new Tango::DevVarLongArray(data_size);
				value.lg_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.lg_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevLong *tmp_ptr = new Tango::DevLong[1];
				*tmp_ptr = *p_data;
				value.lg_seq = new Tango::DevVarLongArray(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete [] p_data;
				else
					delete p_data;
			}
			else
				value.lg_seq = new Tango::DevVarLongArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}


void Attribute::set_value(Tango::DevLong64 *p_data,long x,long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_LONG64)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,
				      o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.lg64_seq = new Tango::DevVarLong64Array(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_lo64[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.lg64_seq = new Tango::DevVarLong64Array(data_size);
				value.lg64_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.lg64_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevLong64 *tmp_ptr = new Tango::DevLong64[1];
				*tmp_ptr = *p_data;
				value.lg64_seq = new Tango::DevVarLong64Array(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete [] p_data;
				else
					delete p_data;
			}
			else
				value.lg64_seq = new Tango::DevVarLong64Array(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}


void Attribute::set_value(Tango::DevFloat *p_data,long x, long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_FLOAT)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.fl_seq = new Tango::DevVarFloatArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_fl[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.fl_seq = new Tango::DevVarFloatArray(data_size);
				value.fl_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.fl_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevFloat *tmp_ptr = new Tango::DevFloat[1];
				*tmp_ptr = *p_data;
				value.fl_seq = new Tango::DevVarFloatArray(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete [] p_data;
				else
					delete p_data;
			}
			else
				value.fl_seq = new Tango::DevVarFloatArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevDouble *p_data,long x, long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_DOUBLE)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.db_seq = new Tango::DevVarDoubleArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_db[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.db_seq = new Tango::DevVarDoubleArray(data_size);
				value.db_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.db_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevDouble *tmp_ptr = new Tango::DevDouble[1];
				*tmp_ptr = *p_data;
				value.db_seq = new Tango::DevVarDoubleArray(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete [] p_data;
				else
					delete p_data;
			}
			else
				value.db_seq = new Tango::DevVarDoubleArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevString *p_data,long x, long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_STRING)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		if (release == true)
		{
			char **strvec = Tango::DevVarStringArray::allocbuf(data_size);
			for (int i = 0;i < data_size;i++)
				strvec[i] = p_data[i];
			value.str_seq = new Tango::DevVarStringArray(data_size,data_size,strvec,release);
			delete [] p_data;
		}
		else
			value.str_seq = new Tango::DevVarStringArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_str[0] = *p_data;
				SAFE_DELETE(p_data);
				scalar_str_attr_release = release;
			}
			else
			{
				value.str_seq = new Tango::DevVarStringArray(data_size);
				value.str_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.str_seq)[k] = CORBA::string_dup(p_data[k]);
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if (release == true)
			{
				char **strvec = Tango::DevVarStringArray::allocbuf(data_size);
				for (int i = 0;i < data_size;i++)
					strvec[i] = p_data[i];
				value.str_seq = new Tango::DevVarStringArray(data_size,data_size,strvec,release);
				if (data_format == Tango::SCALAR)
					delete p_data;
				else
					delete [] p_data;
			}
			else
				value.str_seq = new Tango::DevVarStringArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevUShort *p_data,long x, long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_USHORT)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.ush_seq = new Tango::DevVarUShortArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_ush[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.ush_seq = new Tango::DevVarUShortArray(data_size);
				value.ush_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.ush_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevUShort *tmp_ptr = new Tango::DevUShort[1];
				*tmp_ptr = *p_data;
				value.ush_seq = new Tango::DevVarUShortArray(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete [] p_data;
				else
					delete p_data;
			}
			else
				value.ush_seq = new Tango::DevVarUShortArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}


void Attribute::set_value(Tango::DevBoolean *p_data,long x, long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_BOOLEAN)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.boo_seq = new Tango::DevVarBooleanArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_boo[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.boo_seq = new Tango::DevVarBooleanArray(data_size);
				value.boo_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.boo_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevBoolean *tmp_ptr = new Tango::DevBoolean[1];
				*tmp_ptr = *p_data;
				value.boo_seq = new Tango::DevVarBooleanArray(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete [] p_data;
				else
					delete p_data;
			}
			else
				value.boo_seq = new Tango::DevVarBooleanArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}


void Attribute::set_value(Tango::DevUChar *p_data,long x, long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_UCHAR)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.cha_seq = new Tango::DevVarCharArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_cha[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.cha_seq = new Tango::DevVarCharArray(data_size);
				value.cha_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.cha_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevUChar *tmp_ptr = new Tango::DevUChar[1];
				*tmp_ptr = *p_data;
				value.cha_seq = new Tango::DevVarCharArray(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete [] p_data;
				else
					delete p_data;
			}
			else
				value.cha_seq = new Tango::DevVarCharArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevULong *p_data,long x,long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_ULONG)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,
				      o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.ulg_seq = new Tango::DevVarULongArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_ulo[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.ulg_seq = new Tango::DevVarULongArray(data_size);
				value.ulg_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.ulg_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevULong *tmp_ptr = new Tango::DevULong[1];
				*tmp_ptr = *p_data;
				value.ulg_seq = new Tango::DevVarULongArray(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete [] p_data;
				else
					delete p_data;
			}
			else
				value.ulg_seq = new Tango::DevVarULongArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevULong64 *p_data,long x,long y,bool release)
{

//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_ULONG64)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,
				      o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.ulg64_seq = new Tango::DevVarULong64Array(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_ulo64[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.ulg64_seq = new Tango::DevVarULong64Array(data_size);
				value.ulg64_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.ulg64_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevULong64 *tmp_ptr = new Tango::DevULong64[1];
				*tmp_ptr = *p_data;
				value.ulg64_seq = new Tango::DevVarULong64Array(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete [] p_data;
				else
					delete p_data;
			}
			else
				value.ulg64_seq = new Tango::DevVarULong64Array(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevState *p_data,long x,long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_STATE)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,
				      o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}


//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.state_seq = new Tango::DevVarStateArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_state[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.state_seq = new Tango::DevVarStateArray(data_size);
				value.state_seq->length(data_size);
				for (int k = 0;k < data_size;k++)
					(*value.state_seq)[k] = p_data[k];
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevState *tmp_ptr = new Tango::DevState[1];
				*tmp_ptr = *p_data;
				value.state_seq = new Tango::DevVarStateArray(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete p_data;
				else
					delete p_data;
			}
			else
				value.state_seq = new Tango::DevVarStateArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevEncoded *p_data,long x, long y,bool release)
{
//
// Throw exception if pointer is null
//

	CHECK_PTR(p_data,name);

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_ENCODED)
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(p_data);

		TangoSys_OMemStream o;

		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"Attribute::set_value()");
	}

//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.enc_seq = new Tango::DevVarEncodedArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			tmp_enc[0] = *p_data;
			SAFE_DELETE(p_data);
		}
		else
		{
			if (release == true)
			{
				DevEncoded *tmp_ptr = new DevEncoded[1];

				tmp_ptr->encoded_format = p_data->encoded_format;

				unsigned long nb_data = p_data->encoded_data.length();
				tmp_ptr->encoded_data.replace(nb_data,nb_data,p_data->encoded_data.get_buffer(true),true);
				p_data->encoded_data.replace(0,0,NULL,false);

				value.enc_seq = new Tango::DevVarEncodedArray(data_size,data_size,tmp_ptr,true);
				delete p_data;
			}
			else
				value.enc_seq = new Tango::DevVarEncodedArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevString *p_data_str,Tango::DevUChar *p_data,long size,bool release)
{
	if (p_data_str == NULL || p_data == NULL)
	{
		TangoSys_OMemStream o;
		o << "Data pointer for attribute " << name << " is NULL!" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
                            (const char *)"Attribute::set_value()");
	}

	if (release == false)
	{
		enc_help.encoded_format = CORBA::string_dup(*p_data_str);
		enc_help.encoded_data.replace(size,size,p_data,false);

		set_value(&enc_help);
	}
	else
	{
		DevEncoded *enc_ptr = new DevEncoded;
		enc_ptr->encoded_format = CORBA::string_dup(*p_data_str);
		delete [] *p_data_str;
		enc_ptr->encoded_data.replace(size,size,p_data,true);

		set_value(enc_ptr,1,0,true);
	}
}

void Attribute::set_value(Tango::EncodedAttribute *attr)
{
	CHECK_PTR(attr,name);

	Tango::DevString *f    = attr->get_format();
	Tango::DevUChar  *d    = attr->get_data();
	long              size = attr->get_size();

	if( *f==NULL )
	{
		TangoSys_OMemStream o;
		o << "DevEncoded format for attribute " << name << " not specified" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
                            (const char *)"Attribute::set_value()");
	}

	if( size==0 || !d )
	{
		TangoSys_OMemStream o;
		o << "DevEncoded data for attribute " << name << " not specified" << ends;
		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
								(const char *)"Attribute::set_value()");
	}

	set_value(f,d,size,false);

	if (attr->get_exclusion() == true)
	{
		set_user_attr_mutex(attr->get_mutex());
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_value_date_quality
//
// description : 	Set the attribute read value date and quality.
//			This method is overloaded several times for all the
//			supported attribute data type
//
// in :			p_data : The attribute read value
//			t : The attribute date
//			qual : The attribute quality
//			x : The attribute x dimension (default is 1)
//			y : The atttribute y dimension (default is 0)
//			release : A flag set to true if memory must be
//				  de-allocated (default is false)
//
//--------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevShort *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevShort *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevShort *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevLong *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevLong *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevLong *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif


//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevLong64 *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevLong64 *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevLong64 *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevFloat *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevFloat *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevFloat *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevDouble *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevDouble *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevDouble *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevString *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevString *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevString *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevBoolean *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevBoolean *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevBoolean *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevUShort *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevUShort *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevUShort *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevUChar *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevUChar *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevUChar *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevULong *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevULong *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevULong *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevULong64 *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevULong64 *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevULong64 *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevState *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevState *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
void Attribute::set_value_date_quality(Tango::DevState *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

//---------------------------------------------------------------------------

void Attribute::set_value_date_quality(Tango::DevEncoded *p_data,time_t t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
}

void Attribute::set_value_date_quality(Tango::DevString *p_data_str,Tango::DevUChar *p_data,long size,time_t t,
				    Tango::AttrQuality qual,
				    bool release)
{
	set_value(p_data_str,p_data,size,release);
	set_quality(qual,false);
	set_date(t);
}

#ifdef _TG_WINDOWS_
void Attribute::set_value_date_quality(Tango::DevEncoded *p_data,struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
}

void Attribute::set_value_date_quality(Tango::DevString *p_data_str,Tango::DevUChar *p_data,long size,struct _timeb &t,
				    Tango::AttrQuality qual,
				    bool release)
{
	set_value(p_data_str,p_data,size,release);
	set_quality(qual,false);
	set_date(t);
}
#else
void Attribute::set_value_date_quality(Tango::DevEncoded *p_data,struct timeval &t,
				    Tango::AttrQuality qual,
				    long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);
}

void Attribute::set_value_date_quality(Tango::DevString *p_data_str,Tango::DevUChar *p_data,long size,struct timeval &t,
				    Tango::AttrQuality qual,
				    bool release)
{
	set_value(p_data_str,p_data,size,release);
	set_quality(qual,false);
	set_date(t);
}
#endif

} // End of Tango namespace
