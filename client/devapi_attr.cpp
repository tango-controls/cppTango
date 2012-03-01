static const char *RcsId = "$Id$\n$Name$";

/////////////////////////////////////////////////////////////////////////////////////
//
// devapi_attrib.cpp 	- C++ source code file for TANGO devapi class DeviceAttribute
//
// programmer(s) 	- Andy Gotz (goetz@esrf.fr)
//
// original 		- February 2002
//
// Copyright (C) :      2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// along with Tango.  If not, see <http://www.gnu.org/licenses/>
//
//      $Revision$
//
////////////////////////////////////////////////////////////////////////////

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// DeviceAttributeExt::assignement operator
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttributeExt &DeviceAttribute::DeviceAttributeExt::operator=(const DeviceAttribute::DeviceAttributeExt &rval)
{
	err_list = rval.err_list;
	w_dim_x = rval.w_dim_x;
	w_dim_y = rval.w_dim_y;

	DeviceAttribute::DeviceAttributeExt &nc_source = const_cast<DeviceAttribute::DeviceAttributeExt &>(rval);
	if (nc_source.Long64Seq.operator->() != NULL)
		Long64Seq = nc_source.Long64Seq._retn();
	if (nc_source.ULongSeq.operator->() != NULL)
		ULongSeq = nc_source.ULongSeq._retn();
	if (nc_source.ULong64Seq.operator->() != NULL)
		ULong64Seq = nc_source.ULong64Seq._retn();
	if (nc_source.StateSeq.operator->() != NULL)
		StateSeq = nc_source.StateSeq._retn();
	if (nc_source.EncodedSeq.operator->() != NULL)
		EncodedSeq = nc_source.EncodedSeq._retn();

	return *this;
}

void DeviceAttribute::DeviceAttributeExt::deep_copy(const DeviceAttribute::DeviceAttributeExt &rval)
{
	err_list = rval.err_list;
	w_dim_x = rval.w_dim_x;
	w_dim_y = rval.w_dim_y;

	Long64Seq = rval.Long64Seq;
	ULongSeq = rval.ULongSeq;
	ULong64Seq = rval.ULong64Seq;
	StateSeq = rval.StateSeq;
	EncodedSeq = rval.EncodedSeq;

}
//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - default constructor to create DeviceAttribute
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute():ext(new DeviceAttributeExt)
{
	name = "Name not set";
	dim_x = 0;
	dim_y = 0;
	time.tv_sec = 0;
	time.tv_usec = 0;
	time.tv_nsec = 0;
	quality = Tango::ATTR_INVALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - copy constructor to create DeviceAttribute
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(const DeviceAttribute & source):ext(Tango_NullPtr)
{
	name = source.name;
	exceptions_flags = source.exceptions_flags;
	dim_x = source.dim_x;
	dim_y = source.dim_y;
	quality = source.quality;
	data_format = source.data_format;
	time = source.time;

#ifdef HAS_RVALUE
    LongSeq = source.LongSeq;
    ShortSeq = source.ShortSeq;
    DoubleSeq = source.DoubleSeq;
    StringSeq = source.StringSeq;
    FloatSeq = source.FloatSeq;
    BooleanSeq = source.BooleanSeq;
    UShortSeq = source.UShortSeq;
    UCharSeq = source.UCharSeq;
#else
	DeviceAttribute &nc_source = const_cast<DeviceAttribute &>(source);
	if (nc_source.LongSeq.operator->() != NULL)
		LongSeq = nc_source.LongSeq._retn();
	if (nc_source.ShortSeq.operator->() != NULL)
		ShortSeq = nc_source.ShortSeq._retn();
	if (nc_source.DoubleSeq.operator->() != NULL)
		DoubleSeq = nc_source.DoubleSeq._retn();
	if (nc_source.StringSeq.operator->() != NULL)
		StringSeq = nc_source.StringSeq._retn();
	if (nc_source.FloatSeq.operator->() != NULL)
		FloatSeq = nc_source.FloatSeq._retn();
	if (nc_source.BooleanSeq.operator->() != NULL)
		BooleanSeq = nc_source.BooleanSeq._retn();
	if (nc_source.UShortSeq.operator->() != NULL)
		UShortSeq = nc_source.UShortSeq._retn();
	if (nc_source.UCharSeq.operator->() != NULL)
		UCharSeq = nc_source.UCharSeq._retn();
#endif

	d_state = source.d_state;
	d_state_filled = source.d_state_filled;

#ifdef HAS_UNIQUE_PTR
    if (source.ext.get() != NULL)
    {
        ext.reset(new DeviceAttributeExt);
        *(ext.get()) = *(source.ext.get());
    }
#else
	if (source.ext != NULL)
	{
		ext = new DeviceAttributeExt();
		*ext = *(source.ext);
	}
	else
		ext = NULL;
#endif
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - move constructor to create DeviceAttribute
//
//-----------------------------------------------------------------------------

#ifdef HAS_RVALUE
DeviceAttribute::DeviceAttribute(DeviceAttribute &&source):ext(Tango_NullPtr)
{
	name = move(source.name);
	exceptions_flags = source.exceptions_flags;
	dim_x = source.dim_x;
	dim_y = source.dim_y;
	quality = source.quality;
	data_format = source.data_format;
	time = source.time;

	if (source.LongSeq.operator->() != NULL)
		LongSeq = source.LongSeq._retn();
	if (source.ShortSeq.operator->() != NULL)
		ShortSeq = source.ShortSeq._retn();
	if (source.DoubleSeq.operator->() != NULL)
		DoubleSeq = source.DoubleSeq._retn();
	if (source.StringSeq.operator->() != NULL)
		StringSeq = source.StringSeq._retn();
	if (source.FloatSeq.operator->() != NULL)
		FloatSeq = source.FloatSeq._retn();
	if (source.BooleanSeq.operator->() != NULL)
		BooleanSeq = source.BooleanSeq._retn();
	if (source.UShortSeq.operator->() != NULL)
		UShortSeq = source.UShortSeq._retn();
	if (source.UCharSeq.operator->() != NULL)
		UCharSeq = source.UCharSeq._retn();

	d_state = source.d_state;
	d_state_filled = source.d_state_filled;

    if (source.ext.get() != NULL)
        ext = move(source.ext);
}
#endif

void DeviceAttribute::deep_copy(const DeviceAttribute & source)
{
	name = source.name;
	exceptions_flags = source.exceptions_flags;
	dim_x = source.dim_x;
	dim_y = source.dim_y;
	quality = source.quality;
	data_format = source.data_format;
	time = source.time;

	LongSeq = source.LongSeq;
	ShortSeq = source.ShortSeq;
	DoubleSeq = source.DoubleSeq;
	StringSeq = source.StringSeq;
	FloatSeq = source.FloatSeq;
	BooleanSeq = source.BooleanSeq;
	UShortSeq = source.UShortSeq;
	UCharSeq = source.UCharSeq;

	d_state = source.d_state;
	d_state_filled = source.d_state_filled;

#ifdef HAS_UNIQUE_PTR
    if (source.ext.get() != NULL)
    {
        ext.reset(new DeviceAttributeExt);
        ext.get()->deep_copy(*(source.ext.get()));
    }
    else
        ext.reset();
#else
	if (source.ext != NULL)
	{
		if (ext == NULL)
			ext = new DeviceAttributeExt();
		ext->deep_copy(*source.ext);
	}
	else
		ext = NULL;
#endif
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::get_x_dimension - Get attribute data transfer dimension
//
//-----------------------------------------------------------------------------

AttributeDimension DeviceAttribute::get_r_dimension()
{
	AttributeDimension d;
	d.dim_x = dim_x;
	d.dim_y = dim_y;

	return d;
}

long DeviceAttribute::get_nb_read()
{
	if (dim_y == 0)
		return dim_x;
	else
		return dim_x * dim_y;
}

AttributeDimension DeviceAttribute::get_w_dimension()
{
	AttributeDimension d;
	d.dim_x = ext->w_dim_x;
	d.dim_y = ext->w_dim_y;

	return d;
}

long DeviceAttribute::get_nb_written()
{
	if (ext->w_dim_y == 0)
		return ext->w_dim_x;
	else
		return ext->w_dim_x * ext->w_dim_y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator=() - assignement operator
//
//-----------------------------------------------------------------------------

DeviceAttribute & DeviceAttribute::operator=(const DeviceAttribute &rval)
{
    if (this != &rval)
    {
        name = rval.name;
        exceptions_flags = rval.exceptions_flags;
        dim_x = rval.dim_x;
        dim_y = rval.dim_y;
        quality = rval.quality;
        data_format = rval.data_format;
        time = rval.time;

#ifdef HAS_RVALUE
        LongSeq = rval.LongSeq;
        ShortSeq = rval.ShortSeq;
        DoubleSeq = rval.DoubleSeq;
        StringSeq = rval.StringSeq;
        FloatSeq = rval.FloatSeq;
        BooleanSeq = rval.BooleanSeq;
        UShortSeq = rval.UShortSeq;
        UCharSeq = rval.UCharSeq;
#else
        DeviceAttribute &nc_rval = const_cast<DeviceAttribute &>(rval);
        if (nc_rval.LongSeq.operator->() != NULL)
            LongSeq = nc_rval.LongSeq._retn();
        if (nc_rval.ShortSeq.operator->() != NULL)
            ShortSeq = nc_rval.ShortSeq._retn();
        if (nc_rval.DoubleSeq.operator->() != NULL)
            DoubleSeq = nc_rval.DoubleSeq._retn();
        if (nc_rval.StringSeq.operator->() != NULL)
            StringSeq = nc_rval.StringSeq._retn();
        if (nc_rval.FloatSeq.operator->() != NULL)
            FloatSeq = nc_rval.FloatSeq._retn();
        if (nc_rval.BooleanSeq.operator->() != NULL)
            BooleanSeq = nc_rval.BooleanSeq._retn();
        if (nc_rval.UShortSeq.operator->() != NULL)
            UShortSeq = nc_rval.UShortSeq._retn();
        if (nc_rval.UCharSeq.operator->() != NULL)
            UCharSeq = nc_rval.UCharSeq._retn();
#endif

        d_state = rval.d_state;
        d_state_filled = rval.d_state_filled;

#ifdef HAS_UNIQUE_PTR
        if (rval.ext.get() != NULL)
        {
            ext.reset(new DeviceAttributeExt);
            *(ext.get()) = *(rval.ext.get());
        }
        else
            ext.reset();
#else
        delete ext;
        if (rval.ext != NULL)
        {
            ext = new DeviceAttributeExt();
            *ext = *(rval.ext);
        }
        else
            ext = NULL;
#endif
    }

	return *this;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator=() - move assignement operator
//
//-----------------------------------------------------------------------------

#ifdef HAS_RVALUE
DeviceAttribute & DeviceAttribute::operator=(DeviceAttribute &&rval)
{
	name = move(rval.name);
	exceptions_flags = rval.exceptions_flags;
	dim_x = rval.dim_x;
	dim_y = rval.dim_y;
	quality = rval.quality;
	data_format = rval.data_format;
	time = rval.time;

	if (rval.LongSeq.operator->() != NULL)
		LongSeq = rval.LongSeq._retn();
	if (rval.ShortSeq.operator->() != NULL)
		ShortSeq = rval.ShortSeq._retn();
	if (rval.DoubleSeq.operator->() != NULL)
		DoubleSeq = rval.DoubleSeq._retn();
	if (rval.StringSeq.operator->() != NULL)
		StringSeq = rval.StringSeq._retn();
	if (rval.FloatSeq.operator->() != NULL)
		FloatSeq = rval.FloatSeq._retn();
	if (rval.BooleanSeq.operator->() != NULL)
		BooleanSeq = rval.BooleanSeq._retn();
	if (rval.UShortSeq.operator->() != NULL)
		UShortSeq = rval.UShortSeq._retn();
	if (rval.UCharSeq.operator->() != NULL)
		UCharSeq = rval.UCharSeq._retn();

	d_state = rval.d_state;
	d_state_filled = rval.d_state_filled;

    if (rval.ext.get() != NULL)
    {
        ext = move(rval.ext);
    }
    else
        ext.reset();

	return *this;
}
#endif

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for short
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string &new_name, short datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ShortSeq = new(DevVarShortArray);
	ShortSeq->length(1);
	ShortSeq[0] = datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, short datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ShortSeq = new(DevVarShortArray);
	ShortSeq->length(1);
	ShortSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for DevLong
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, DevLong datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	LongSeq  = new(DevVarLongArray);
	LongSeq->length(1);
	LongSeq[0] = datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, DevLong datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	LongSeq  = new(DevVarLongArray);
	LongSeq->length(1);
	LongSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for DevLong64
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, DevLong64 datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->Long64Seq  = new(DevVarLong64Array);
	ext->Long64Seq->length(1);
	ext->Long64Seq[0] = datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, DevLong64 datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->Long64Seq  = new(DevVarLong64Array);
	ext->Long64Seq->length(1);
	ext->Long64Seq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for double
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, double datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	DoubleSeq  = new(DevVarDoubleArray);
	DoubleSeq->length(1);
	DoubleSeq[0] = datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, double datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	DoubleSeq  = new(DevVarDoubleArray);
	DoubleSeq->length(1);
	DoubleSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for string
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, string& datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	StringSeq = new(DevVarStringArray);
	StringSeq->length(1);
	StringSeq[0] = string_dup(datum.c_str());
}

DeviceAttribute::DeviceAttribute(const char *new_name, string& datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	StringSeq = new(DevVarStringArray);
	StringSeq->length(1);
	StringSeq[0] = string_dup(datum.c_str());
}

DeviceAttribute::DeviceAttribute(string& new_name, const char *datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	StringSeq = new(DevVarStringArray);
	StringSeq->length(1);
	StringSeq[0] = string_dup(datum);
}

DeviceAttribute::DeviceAttribute(const char *new_name, const char *datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	StringSeq = new(DevVarStringArray);
	StringSeq->length(1);
	StringSeq[0] = string_dup(datum);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for float
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, float datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	FloatSeq  = new(DevVarFloatArray);
	FloatSeq->length(1);
	FloatSeq[0] = datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, float datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	FloatSeq  = new(DevVarFloatArray);
	FloatSeq->length(1);
	FloatSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for boolean
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, bool datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	BooleanSeq  = new(DevVarBooleanArray);
	BooleanSeq->length(1);
	BooleanSeq[0] = datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, bool datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	BooleanSeq  = new(DevVarBooleanArray);
	BooleanSeq->length(1);
	BooleanSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for unsigned short (DevUShort)
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, unsigned short datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UShortSeq = new(DevVarUShortArray);
	UShortSeq->length(1);
	UShortSeq[0] = datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, unsigned short datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UShortSeq = new(DevVarUShortArray);
	UShortSeq->length(1);
	UShortSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for unsigned char (DevUChar)
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, unsigned char datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UCharSeq  = new(DevVarCharArray);
	UCharSeq->length(1);
	UCharSeq[0] = datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, unsigned char datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UCharSeq  = new(DevVarCharArray);
	UCharSeq->length(1);
	UCharSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for DevULong
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, DevULong datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULongSeq  = new(DevVarULongArray);
	ext->ULongSeq->length(1);
	ext->ULongSeq[0] = datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, DevULong datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULongSeq  = new(DevVarULongArray);
	ext->ULongSeq->length(1);
	ext->ULongSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for DevULong64
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, DevULong64 datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULong64Seq  = new(DevVarULong64Array);
	ext->ULong64Seq->length(1);
	ext->ULong64Seq[0] = datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, DevULong64 datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULong64Seq  = new(DevVarULong64Array);
	ext->ULong64Seq->length(1);
	ext->ULong64Seq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for DevState
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, DevState datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->StateSeq  = new(DevVarStateArray);
	ext->StateSeq->length(1);
	ext->StateSeq[0] = datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, DevState datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->StateSeq  = new(DevVarStateArray);
	ext->StateSeq->length(1);
	ext->StateSeq[0] = datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for DevEncoded
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string &new_name, DevEncoded &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->EncodedSeq  = new(DevVarEncodedArray);
	ext->EncodedSeq->length(1);
	ext->EncodedSeq[0] = datum;

}

DeviceAttribute::DeviceAttribute(const char *new_name, DevEncoded &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->EncodedSeq  = new(DevVarEncodedArray);
	ext->EncodedSeq->length(1);
	ext->EncodedSeq[0] = datum;
}
//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of short
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<short> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ShortSeq = new(DevVarShortArray);
	ShortSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<short> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ShortSeq = new(DevVarShortArray);
	ShortSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<short> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ShortSeq = new(DevVarShortArray);
	ShortSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<short> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ShortSeq = new(DevVarShortArray);
	ShortSeq.inout() << datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of DevLong
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<DevLong> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	LongSeq = new(DevVarLongArray);
	LongSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<DevLong> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	LongSeq = new(DevVarLongArray);
	LongSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<DevLong> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	LongSeq = new(DevVarLongArray);
	LongSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<DevLong> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	LongSeq = new(DevVarLongArray);
	LongSeq.inout() << datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of DevLong64
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<DevLong64> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->Long64Seq = new(DevVarLong64Array);
	ext->Long64Seq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<DevLong64> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->Long64Seq = new(DevVarLong64Array);
	ext->Long64Seq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<DevLong64> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->Long64Seq = new(DevVarLong64Array);
	ext->Long64Seq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<DevLong64> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->Long64Seq = new(DevVarLong64Array);
	ext->Long64Seq.inout() << datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of double
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<double> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	DoubleSeq = new(DevVarDoubleArray);
	DoubleSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<double> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	DoubleSeq = new(DevVarDoubleArray);
	DoubleSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<double> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	DoubleSeq = new(DevVarDoubleArray);
	DoubleSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<double> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	DoubleSeq = new(DevVarDoubleArray);
	DoubleSeq.inout() << datum;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of string
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<string> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	StringSeq = new(DevVarStringArray);
	StringSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<string> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	StringSeq = new(DevVarStringArray);
	StringSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<string> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	StringSeq = new(DevVarStringArray);
	StringSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<string> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	StringSeq = new(DevVarStringArray);
	StringSeq.inout() << datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of float
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<float> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	FloatSeq = new(DevVarFloatArray);
	FloatSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<float> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	FloatSeq = new(DevVarFloatArray);
	FloatSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<float> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	FloatSeq = new(DevVarFloatArray);
	FloatSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<float> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	FloatSeq = new(DevVarFloatArray);
	FloatSeq.inout() << datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of bool
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<bool> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	BooleanSeq = new(DevVarBooleanArray);
	BooleanSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<bool> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	BooleanSeq = new(DevVarBooleanArray);
 	BooleanSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<bool> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	BooleanSeq = new(DevVarBooleanArray);
	BooleanSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<bool> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	BooleanSeq = new(DevVarBooleanArray);
	BooleanSeq.inout() << datum;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of unsigned short
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<unsigned short> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UShortSeq = new(DevVarUShortArray);
	UShortSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<unsigned short> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UShortSeq = new(DevVarUShortArray);
	UShortSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<unsigned short> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UShortSeq = new(DevVarUShortArray);
	UShortSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<unsigned short> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UShortSeq = new(DevVarUShortArray);
	UShortSeq.inout() << datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of unsigned char
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<unsigned char> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UCharSeq = new(DevVarCharArray);
	UCharSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<unsigned char> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UCharSeq = new(DevVarCharArray);
	UCharSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<unsigned char> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UCharSeq = new(DevVarCharArray);
	UCharSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<unsigned char> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	UCharSeq = new(DevVarCharArray);
	UCharSeq.inout() << datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of DevULong
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<DevULong> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULongSeq = new(DevVarULongArray);
	ext->ULongSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<DevULong> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULongSeq = new(DevVarULongArray);
	ext->ULongSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<DevULong> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULongSeq = new(DevVarULongArray);
	ext->ULongSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<DevULong> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULongSeq = new(DevVarULongArray);
	ext->ULongSeq.inout() << datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of DevULong64
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<DevULong64> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULong64Seq = new(DevVarULong64Array);
	ext->ULong64Seq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<DevULong64> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULong64Seq = new(DevVarULong64Array);
	ext->ULong64Seq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<DevULong64> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULong64Seq = new(DevVarULong64Array);
	ext->ULong64Seq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<DevULong64> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->ULong64Seq = new(DevVarULong64Array);
	ext->ULong64Seq.inout() << datum;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::DeviceAttribute() - constructor for vector of DevState
//
//-----------------------------------------------------------------------------

DeviceAttribute::DeviceAttribute(string& new_name, vector<DevState> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->StateSeq = new(DevVarStateArray);
	ext->StateSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<DevState> &datum):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->StateSeq = new(DevVarStateArray);
	ext->StateSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(string& new_name, vector<DevState> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->StateSeq = new(DevVarStateArray);
	ext->StateSeq.inout() << datum;
}

DeviceAttribute::DeviceAttribute(const char *new_name, vector<DevState> &datum,int x,int y):ext(new DeviceAttributeExt)
{
	name = new_name;
	dim_x = x;
	dim_y = y;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ext->StateSeq = new(DevVarStateArray);
	ext->StateSeq.inout() << datum;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::~DeviceAttribute() - destructor to destroy DeviceAttribute
//
//-----------------------------------------------------------------------------

DeviceAttribute::~DeviceAttribute()
{
#ifndef HAS_UNIQUE_PTR
    delete ext;
#endif
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::is_empty() - returns true or false if datum is empty
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::is_empty()
{
	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
			return false;
	}

	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
			return false;
	}

	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
			return false;
	}

	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
			return false;
	}

	if (FloatSeq.operator->() != NULL)
	{
		if (FloatSeq->length() != 0)
			return false;
	}

	if (BooleanSeq.operator->() != NULL)
	{
		if (BooleanSeq->length() != 0)
			return false;
	}

	if (UShortSeq.operator->() != NULL)
	{
		if (UShortSeq->length() != 0)
			return false;
	}

	if (UCharSeq.operator->() != NULL)
	{
		if (UCharSeq->length() != 0)
			return false;
	}

	if (ext->Long64Seq.operator->() != NULL)
	{
		if (ext->Long64Seq->length() != 0)
			return false;
	}

	if (ext->ULongSeq.operator->() != NULL)
	{
		if (ext->ULongSeq->length() != 0)
			return false;
	}

	if (ext->ULong64Seq.operator->() != NULL)
	{
		if (ext->ULong64Seq->length() != 0)
			return false;
	}

	if (ext->StateSeq.operator->() != NULL)
	{
		if (ext->StateSeq->length() != 0)
			return false;
	}

	if (ext->EncodedSeq.operator->() != NULL)
	{
		if (ext->EncodedSeq->length() != 0)
			return false;
	}

	if (d_state_filled == true)
		return false;

	if (exceptions_flags.test(isempty_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_EmptyDeviceAttribute",
					(const char*)"cannot extract, no data in DeviceAttribute object ",
					(const char*)"DeviceAttribute::is_empty");
	}
	return true;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::get_type() - returns attribute data type
//
//-----------------------------------------------------------------------------

int DeviceAttribute::get_type()
{
	int data_type;

	if (is_empty() == true)
		return -1;
	else
	{
		if (LongSeq.operator->() != NULL)
			data_type = Tango::DEV_LONG;
		else if (ShortSeq.operator->() != NULL)
			data_type = Tango::DEV_SHORT;
		else if (DoubleSeq.operator->() != NULL)
			data_type = Tango::DEV_DOUBLE;
		else if (FloatSeq.operator->() != NULL)
			data_type = Tango::DEV_FLOAT;
		else if (BooleanSeq.operator->() != NULL)
			data_type = Tango::DEV_BOOLEAN;
		else if (UShortSeq.operator->() != NULL)
			data_type = Tango::DEV_USHORT;
		else if (UCharSeq.operator->() != NULL)
			data_type = Tango::DEV_UCHAR;
		else if (StringSeq.operator->() != NULL)
			data_type = Tango::DEV_STRING;
		else if (ext->Long64Seq.operator->() != NULL)
			data_type = Tango::DEV_LONG64;
		else if (ext->ULongSeq.operator->() != NULL)
			data_type = Tango::DEV_ULONG;
		else if (ext->ULong64Seq.operator->() != NULL)
			data_type = Tango::DEV_ULONG64;
		else if (ext->EncodedSeq.operator->() != NULL)
			data_type = Tango::DEV_ENCODED;
		else if ((ext->StateSeq.operator->() != NULL) || (d_state_filled == true))
			data_type = Tango::DEV_STATE;
        else
            data_type = -1;
	}

	return data_type;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::get_data_format() - returns attribute data format
//
//-----------------------------------------------------------------------------

AttrDataFormat DeviceAttribute::get_data_format()
{
	if (exceptions_flags.test(unknown_format_flag) && (data_format == Tango::FMT_UNKNOWN))
	{
		ApiDataExcept::throw_exception((const char*)"API_EmptyDeviceAttribute",
					(const char*)"Cannot returned data_type from DeviceAttribute object: Not initialised yet or too old device (< V7)",
					(const char*)"DeviceAttribute::get_data_format");
	}
	return data_format;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(short &) - extract a short from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (short &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
			datum = ShortSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}

	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(short) - insert a short into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (short datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

    DevVarShortArray *short_vararr = new(DevVarShortArray);
    short_vararr->length(1);
    (*short_vararr)[0] = datum;
    ShortSeq = short_vararr;

	del_mem(Tango::DEV_SHORT);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevLong &) - extract a DevLong from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevLong &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
			datum = LongSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}

	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevLong) - insert a DevLong into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevLong datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DevVarLongArray *long_vararr = new(DevVarLongArray);
	long_vararr->length(1);
	(*long_vararr)[0] = datum;
	LongSeq = long_vararr;

	del_mem(Tango::DEV_LONG);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevLong64 &) - extract a DevLong64 from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevLong64 &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->Long64Seq.operator->() != NULL)
	{
		if (ext->Long64Seq->length() != 0)
			datum = ext->Long64Seq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}

	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevLong64) - insert a DevLong64 into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevLong64 datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DevVarLong64Array *long_vararr = new(DevVarLong64Array);
	long_vararr->length(1);
	(*long_vararr)[0] = datum;
	ext->Long64Seq = long_vararr;

	del_mem(Tango::DEV_LONG64);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(double &) - extract a double from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (double &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
			datum = DoubleSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(double) - insert a double into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (double datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

    DevVarDoubleArray *double_vararr = new(DevVarDoubleArray);
    double_vararr->length(1);
    (*double_vararr)[0] = datum;
    DoubleSeq = double_vararr;

	del_mem(Tango::DEV_DOUBLE);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(string &) - extract a string from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (string& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
			datum = StringSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(string &) - insert a string into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (string& datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

    DevVarStringArray *string_vararr = new(DevVarStringArray);
    string_vararr->length(1);
    (*string_vararr)[0] = string_dup(datum.c_str());
    StringSeq = string_vararr;

	del_mem(Tango::DEV_STRING);
}

void DeviceAttribute::operator << (DevString datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

    DevVarStringArray *string_vararr = new(DevVarStringArray);
    string_vararr->length(1);
    (*string_vararr)[0] = string_dup(datum);
    StringSeq = string_vararr;

	del_mem(Tango::DEV_STRING);
}

void DeviceAttribute::operator << (const char *datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

    DevVarStringArray *string_vararr = new(DevVarStringArray);
    string_vararr->length(1);
    (*string_vararr)[0] = string_dup(datum);
    StringSeq = string_vararr;

	del_mem(Tango::DEV_STRING);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(float &) - extract a float from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (float &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (FloatSeq.operator->() != NULL)
	{
		if (FloatSeq->length() != 0)
			datum = FloatSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(float) - insert a float into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (float datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

    DevVarFloatArray *float_vararr = new(DevVarFloatArray);
    float_vararr->length(1);
    (*float_vararr)[0] = datum;
    FloatSeq = float_vararr;

	del_mem(Tango::DEV_FLOAT);
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(bool &) - extract a boolean from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (bool &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (BooleanSeq.operator->() != NULL)
	{
		if (BooleanSeq->length() != 0)
			datum = BooleanSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(bool) - insert a boolean into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (bool datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DevVarBooleanArray *bool_vararr = new(DevVarBooleanArray);
	bool_vararr->length(1);
	(*bool_vararr)[0] = datum;
	BooleanSeq = bool_vararr;

	del_mem(Tango::DEV_BOOLEAN);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(unsigned short &) - extract a unsigned short from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (unsigned short &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (UShortSeq.operator->() != NULL)
	{
		if (UShortSeq->length() != 0)
			datum = UShortSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(unsigned short) - insert a unsigned short into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (unsigned short datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DevVarUShortArray *ush_vararr = new(DevVarUShortArray);
	ush_vararr->length(1);
	(*ush_vararr)[0] = datum;
	UShortSeq = ush_vararr;

	del_mem(Tango::DEV_USHORT);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(unsigned char &) - extract a unsigned char from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (unsigned char &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (UCharSeq.operator->() != NULL)
	{
		if (UCharSeq->length() != 0)
			datum = UCharSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(unsigned char) - insert a unsigned char into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (unsigned char datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DevVarCharArray *uch_vararr = new(DevVarCharArray);
	uch_vararr->length(1);
	(*uch_vararr)[0] = datum;
	UCharSeq = uch_vararr;

	del_mem(Tango::DEV_UCHAR);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevULong &) - extract a DevULong from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevULong &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->ULongSeq.operator->() != NULL)
	{
		if (ext->ULongSeq->length() != 0)
			datum = ext->ULongSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}

	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevULong) - insert a DevULong into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevULong datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DevVarULongArray *long_vararr = new(DevVarULongArray);
	long_vararr->length(1);
	(*long_vararr)[0] = datum;
	ext->ULongSeq = long_vararr;

	del_mem(Tango::DEV_ULONG);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevULong64 &) - extract a DevULong64 from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevULong64 &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->ULong64Seq.operator->() != NULL)
	{
		if (ext->ULong64Seq->length() != 0)
			datum = ext->ULong64Seq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}

	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevULong64) - insert a DevULong64 into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevULong64 datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DevVarULong64Array *long_vararr = new(DevVarULong64Array);
	long_vararr->length(1);
	(*long_vararr)[0] = datum;
	ext->ULong64Seq = long_vararr;

	del_mem(Tango::DEV_ULONG64);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevState &) - extract a DevState from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevState &datum)
{
	// check for available data

	bool ret = check_for_data();

	if ( ret == false)
		return false;

	if (d_state_filled == true)
	{
		datum = d_state;
		d_state_filled = false;

		return ret;
	}

	if (ext->StateSeq.operator->() != NULL)
	{
		if (ext->StateSeq->length() != 0)
			datum = ext->StateSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}

	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevState) - insert a DevState into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevState datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DevVarStateArray *state_vararr = new(DevVarStateArray);
	state_vararr->length(1);
	(*state_vararr)[0] = datum;
	ext->StateSeq = state_vararr;

	del_mem(Tango::DEV_STATE);
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevEncoded &) - extract a DevEncoded from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevEncoded &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->EncodedSeq.operator->() != NULL)
	{
		if (ext->EncodedSeq->length() != 0)
			datum = ext->EncodedSeq[0];
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}

	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevEncoded) - insert a DevEncoded into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevEncoded &datum)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DevVarEncodedArray *enc_vararr = new(DevVarEncodedArray);
	enc_vararr->length(1);
	(*enc_vararr)[0] = datum;
	ext->EncodedSeq = enc_vararr;

	del_mem(Tango::DEV_ENCODED);
}

void DeviceAttribute::insert(char *&str,unsigned char *&ptr,unsigned int size)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DevVarEncodedArray *enc_vararr = new(DevVarEncodedArray);
	enc_vararr->length(1);
	(*enc_vararr)[0].encoded_format = CORBA::string_dup(str);
	(*enc_vararr)[0].encoded_data.replace(size,size,(CORBA::Octet *)ptr);
	ext->EncodedSeq = enc_vararr;

	del_mem(Tango::DEV_ENCODED);
}

void DeviceAttribute::insert(string &str,vector<unsigned char> &array)
{
	dim_x = 1;
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DevVarEncodedArray *enc_vararr = new(DevVarEncodedArray);
	enc_vararr->length(1);
	(*enc_vararr)[0].encoded_format = CORBA::string_dup(str.c_str());
	(*enc_vararr)[0].encoded_data << array;
	ext->EncodedSeq = enc_vararr;

	del_mem(Tango::DEV_ENCODED);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<string>) -
// insert a vector of string into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<string> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (StringSeq.operator->() == NULL)
	{
		DevVarStringArray *str_vararr = new(DevVarStringArray);
		StringSeq = str_vararr;
	}
	StringSeq.inout() << datum;

	del_mem(Tango::DEV_STRING);
}

void DeviceAttribute::insert(vector<string> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<string> &) - extract a vector<string> from DeviceData
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<string>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
		{
        	datum.resize(StringSeq->length());

        	for (unsigned int i=0; i<StringSeq->length(); i++)
        	{
                datum[i] = StringSeq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<short>) -
// insert a vector of short into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<short> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (ShortSeq.operator->() == NULL)
	{
		DevVarShortArray *short_vararr = new(DevVarShortArray);
		ShortSeq = short_vararr;
	}
	ShortSeq.inout() << datum;

	del_mem(Tango::DEV_SHORT);
}

void DeviceAttribute::insert(vector<short> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<short> &) - extract a vector<short> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<short>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
		{
			datum.resize(ShortSeq->length());

			for (unsigned int i=0; i<ShortSeq->length(); i++)
			{
				datum[i] = ShortSeq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<DevLong>) -
// insert a vector of DevLong into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<DevLong> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (LongSeq.operator->() == NULL)
	{
		DevVarLongArray *long_vararr = new(DevVarLongArray);
		LongSeq = long_vararr;
	}
    LongSeq.inout() << datum;

	del_mem(Tango::DEV_LONG);
}

void DeviceAttribute::insert(vector<DevLong> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<DevLong> &) - extract a vector<DevLong> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<DevLong>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
		{
			datum.resize(LongSeq->length());

			for (unsigned int i=0; i<LongSeq->length(); i++)
			{
				datum[i] = LongSeq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<DevLong64>) -
// insert a vector of DevLong64 into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<DevLong64> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (ext->Long64Seq.operator->() == NULL)
	{
		DevVarLong64Array *long_vararr = new(DevVarLong64Array);
		ext->Long64Seq = long_vararr;
	}
    ext->Long64Seq.inout() << datum;

	del_mem(Tango::DEV_LONG64);
}

void DeviceAttribute::insert(vector<DevLong64> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<DevLong64> &) - extract a vector<DevLong64> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<DevLong64>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->Long64Seq.operator->() != NULL)
	{
		if (ext->Long64Seq->length() != 0)
		{
			datum.resize(ext->Long64Seq->length());

			for (unsigned int i=0; i<ext->Long64Seq->length(); i++)
			{
				datum[i] = ext->Long64Seq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<double>) -
// insert a vector of double into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<double> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (DoubleSeq.operator->() == NULL)
	{
		DevVarDoubleArray *double_vararr = new(DevVarDoubleArray);
		DoubleSeq = double_vararr;
	}
	DoubleSeq.inout() << datum;

	del_mem(Tango::DEV_DOUBLE);
}

void DeviceAttribute::insert(vector<double> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<double> &) - extract a vector<double> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<double>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
		{
			datum.resize(DoubleSeq->length());

			for (unsigned int i=0; i<DoubleSeq->length(); i++)
			{
				datum[i] = DoubleSeq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<float>) -
// insert a vector of float into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<float> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (FloatSeq.operator->() == NULL)
	{
		DevVarFloatArray *float_vararr = new(DevVarFloatArray);
		FloatSeq = float_vararr;
	}
	FloatSeq.inout() << datum;

	del_mem(Tango::DEV_FLOAT);
}

void DeviceAttribute::insert(vector<float> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<float> &) - extract a vector<float> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<float>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (FloatSeq.operator->() != NULL)
	{
		if (FloatSeq->length() != 0)
		{
			datum.resize(FloatSeq->length());

			for (unsigned int i=0; i<FloatSeq->length(); i++)
			{
				datum[i] = FloatSeq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<bool>) -
// insert a vector of boolean into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<bool> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (BooleanSeq.operator->() == NULL)
	{
		DevVarBooleanArray *bool_vararr = new(DevVarBooleanArray);
		BooleanSeq = bool_vararr;
	}
	BooleanSeq.inout() << datum;

	del_mem(Tango::DEV_BOOLEAN);
}

void DeviceAttribute::insert(vector<bool> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<bool> &) - extract a vector<bool> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<bool>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (BooleanSeq.operator->() != NULL)
	{
		if (BooleanSeq->length() != 0)
		{
			datum.resize(BooleanSeq->length());

			for (unsigned int i=0; i<BooleanSeq->length(); i++)
			{
				datum[i] = BooleanSeq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<unsigned short>) -
// insert a vector of unsigned short into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<unsigned short> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (UShortSeq.operator->() == NULL)
	{
		DevVarUShortArray *ushort_vararr = new(DevVarUShortArray);
		UShortSeq = ushort_vararr;
	}
	UShortSeq.inout() << datum;

	del_mem(Tango::DEV_USHORT);
}

void DeviceAttribute::insert(vector<unsigned short> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<unsigned short> &) - extract a vector<unsigned short> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<unsigned short>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (UShortSeq.operator->() != NULL)
	{
		if (UShortSeq->length() != 0)
		{
			datum.resize(UShortSeq->length());

			for (unsigned int i=0; i<UShortSeq->length(); i++)
			{
				datum[i] = UShortSeq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<unsigned char>) -
// insert a vector of unsigned char into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<unsigned char> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (UCharSeq.operator->() == NULL)
	{
		DevVarUCharArray *uchar_vararr = new(DevVarUCharArray);
		UCharSeq = uchar_vararr;
	}
	UCharSeq.inout() << datum;

	del_mem(Tango::DEV_UCHAR);
}

void DeviceAttribute::insert(vector<unsigned char> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<unsigned char> &) - extract a vector<unsigne char> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<unsigned char>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (UCharSeq.operator->() != NULL)
	{
		if (UCharSeq->length() != 0)
		{
			datum.resize(UCharSeq->length());

			for (unsigned int i=0; i<UCharSeq->length(); i++)
			{
				datum[i] = UCharSeq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<DevULong>) -
// insert a vector of DevULong into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<DevULong> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (ext->ULongSeq.operator->() == NULL)
	{
		DevVarULongArray *long_vararr = new(DevVarULongArray);
		ext->ULongSeq = long_vararr;
	}
    ext->ULongSeq.inout() << datum;

	del_mem(Tango::DEV_ULONG);
}

void DeviceAttribute::insert(vector<DevULong> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<DevULong> &) - extract a vector<DevULong> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<DevULong>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->ULongSeq.operator->() != NULL)
	{
		if (ext->ULongSeq->length() != 0)
		{
			datum.resize(ext->ULongSeq->length());

			for (unsigned int i=0; i<ext->ULongSeq->length(); i++)
			{
				datum[i] = ext->ULongSeq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<DevULong64>) -
// insert a vector of DevULong64 into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<DevULong64> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (ext->ULong64Seq.operator->() == NULL)
	{
		DevVarULong64Array *long_vararr = new(DevVarULong64Array);
		ext->ULong64Seq = long_vararr;
	}
    ext->ULong64Seq.inout() << datum;

	del_mem(Tango::DEV_ULONG64);
}

void DeviceAttribute::insert(vector<DevULong64> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<DevULong64> &) - extract a vector<DevULong64> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<DevULong64>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->ULong64Seq.operator->() != NULL)
	{
		if (ext->ULong64Seq->length() != 0)
		{
			datum.resize(ext->ULong64Seq->length());

			for (unsigned int i=0; i<ext->ULong64Seq->length(); i++)
			{
				datum[i] = ext->ULong64Seq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(vector<DevState>) -
// insert a vector of DevState into DeviceAttribute
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (vector<DevState> &datum)
{
	dim_x = datum.size();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (ext->StateSeq.operator->() == NULL)
	{
		DevVarStateArray *long_vararr = new(DevVarStateArray);
		ext->StateSeq = long_vararr;
	}
    ext->StateSeq.inout() << datum;

	del_mem(Tango::DEV_STATE);
}

void DeviceAttribute::insert(vector<DevState> &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(vector<DevState> &) - extract a vector<DevState> from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (vector<DevState>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->StateSeq.operator->() != NULL)
	{
		if (ext->StateSeq->length() != 0)
		{
        	datum.resize(ext->StateSeq->length());

        	for (unsigned int i=0; i<ext->StateSeq->length(); i++)
        	{
            	datum[i] = ext->StateSeq[i];
       	 	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarShortArray *) - extract a sequence of
// short from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarShortArray* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
		{
			datum = ShortSeq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarLongArray *) - extract a sequence of
// long from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarLongArray* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
		{
			datum = LongSeq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarDoubleArray *) - extract a sequence of
// double from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarDoubleArray* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
		{
			datum = DoubleSeq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarStringArray *) - extract a sequence of
// string from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarStringArray* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
		{
			datum = StringSeq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarFloatArray *) - extract a sequence of
// double from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarFloatArray* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (FloatSeq.operator->() != NULL)
	{
		if (FloatSeq->length() != 0)
		{
			datum = FloatSeq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarBooleanArray *) - extract a sequence of
// bool from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarBooleanArray* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (BooleanSeq.operator->() != NULL)
	{
		if (BooleanSeq->length() != 0)
		{
			datum = BooleanSeq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarUShortArray *) - extract a sequence of
// unsigned short from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarUShortArray* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (UShortSeq.operator->() != NULL)
	{
		if (UShortSeq->length() != 0)
		{
			datum = UShortSeq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarCharArray *) - extract a sequence of
// unsigned char from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarCharArray* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (UCharSeq.operator->() != NULL)
	{
		if (UCharSeq->length() != 0)
		{
			datum = UCharSeq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarLong64Array *) - extract a sequence of
// DevLong64 from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarLong64Array* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->Long64Seq.operator->() != NULL)
	{
		if (ext->Long64Seq->length() != 0)
		{
			datum = ext->Long64Seq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarULongArray *) - extract a sequence of
// DevULong from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarULongArray* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->ULongSeq.operator->() != NULL)
	{
		if (ext->ULongSeq->length() != 0)
		{
			datum = ext->ULongSeq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarULong64Array *) - extract a sequence of
// DevUULong64 from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarULong64Array* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->ULong64Seq.operator->() != NULL)
	{
		if (ext->ULong64Seq->length() != 0)
		{
			datum = ext->ULong64Seq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarStateArray *) - extract a sequence of
// DevState from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarStateArray* &datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->StateSeq.operator->() != NULL)
	{
		if (ext->StateSeq->length() != 0)
		{
			datum = ext->StateSeq._retn();
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator >>(DevVarEncodedArray *) - extract a sequence of
// long from DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::operator >> (DevVarEncodedArray* &datum)
{
	bool ret = true;

	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}

	if (is_empty() == true)
		return false;

	if (ext->EncodedSeq.operator->() != NULL)
	{
		if (ext->EncodedSeq->length() != 0)
		{
			datum = ext->EncodedSeq._retn();
		}
		else
			ret = false;
	}
	else
	{
		ret = false;

		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of DevEncoded",
					(const char*)"DeviceAttribute::operator>>");
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarShortArray &) -
// insert a DevVarShortArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarShortArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	ShortSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		ShortSeq[i] = datum[i];

	del_mem(Tango::DEV_SHORT);
}

void DeviceAttribute::insert(const DevVarShortArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarShortArray *) -
// insert a DevVarShortArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarShortArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	ShortSeq = datum;

	del_mem(Tango::DEV_SHORT);
}

void DeviceAttribute::insert(DevVarShortArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarLongArray &) -
// insert a DevVarLongArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarLongArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	LongSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		LongSeq[i] = datum[i];

	del_mem(Tango::DEV_LONG);
}

void DeviceAttribute::insert(const DevVarLongArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarLongArray *) -
// insert a DevVarLongArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarLongArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	LongSeq = datum;

	del_mem(Tango::DEV_LONG);
}

void DeviceAttribute::insert(DevVarLongArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarDoubleArray &) -
// insert a DevVarDoubleArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarDoubleArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DoubleSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		DoubleSeq[i] = datum[i];

	del_mem(Tango::DEV_DOUBLE);
}

void DeviceAttribute::insert(const DevVarDoubleArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarDoubleArray *) -
// insert a DevVarDoubleArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarDoubleArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	DoubleSeq = datum;

	del_mem(Tango::DEV_DOUBLE);
}

void DeviceAttribute::insert(DevVarDoubleArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarStringArray &) -
// insert a DevVarStringArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarStringArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	StringSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		StringSeq[i] = datum[i];

	del_mem(Tango::DEV_STRING);
}

void DeviceAttribute::insert(const DevVarStringArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarStringArray *) -
// insert a DevVarStringArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarStringArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	StringSeq = datum;

	del_mem(Tango::DEV_STRING);
}

void DeviceAttribute::insert(DevVarStringArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarFloatArray &) -
// insert a DevVarFloatArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarFloatArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	FloatSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		FloatSeq[i] = datum[i];

	del_mem(Tango::DEV_FLOAT);
}

void DeviceAttribute::insert(const DevVarFloatArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarFloatArray *) -
// insert a DevVarFloatArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarFloatArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	FloatSeq = datum;

	del_mem(Tango::DEV_FLOAT);
}

void DeviceAttribute::insert(DevVarFloatArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarBooleanArray &) -
// insert a DevVarBooleanArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarBooleanArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	BooleanSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		BooleanSeq[i] = datum[i];

	del_mem(Tango::DEV_BOOLEAN);
}

void DeviceAttribute::insert(const DevVarBooleanArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarBooleanArray *) -
// insert a DevVarBooleanArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarBooleanArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	BooleanSeq = datum;

	del_mem(Tango::DEV_BOOLEAN);
}

void DeviceAttribute::insert(DevVarBooleanArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarUShortArray &) -
// insert a DevVarUShortArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarUShortArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	UShortSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		UShortSeq[i] = datum[i];

	del_mem(Tango::DEV_USHORT);
}

void DeviceAttribute::insert(const DevVarUShortArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarUShortArray *) -
// insert a DevVarUShortArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarUShortArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	UShortSeq = datum;

	del_mem(Tango::DEV_USHORT);
}

void DeviceAttribute::insert(DevVarUShortArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarCharArray &) -
// insert a DevVarCharArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarCharArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	UCharSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		UCharSeq[i] = datum[i];

	del_mem(Tango::DEV_UCHAR);
}

void DeviceAttribute::insert(const DevVarCharArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarCharArray *) -
// insert a DevVarCharArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarCharArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	UCharSeq = datum;

	del_mem(Tango::DEV_UCHAR);
}

void DeviceAttribute::insert(DevVarCharArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarLong64Array &) -
// insert a DevVarLong64Array by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarLong64Array &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	ext->Long64Seq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		ext->Long64Seq[i] = datum[i];

	del_mem(Tango::DEV_LONG64);
}

void DeviceAttribute::insert(const DevVarLong64Array &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarLong64Array *) -
// insert a DevVarLong64Array by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarLong64Array *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	ext->Long64Seq = datum;

	del_mem(Tango::DEV_LONG64);
}

void DeviceAttribute::insert(DevVarLong64Array *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarULongArray &) -
// insert a DevVarULongArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarULongArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	ext->ULongSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		ext->ULongSeq[i] = datum[i];

	del_mem(Tango::DEV_ULONG);
}

void DeviceAttribute::insert(const DevVarULongArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarULongArray *) -
// insert a DevVarULongArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarULongArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	ext->ULongSeq = datum;

	del_mem(Tango::DEV_ULONG);
}

void DeviceAttribute::insert(DevVarULongArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarULong64Array &) -
// insert a DevVarULong64Array by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarULong64Array &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	ext->ULong64Seq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		ext->ULong64Seq[i] = datum[i];

	del_mem(Tango::DEV_ULONG64);
}

void DeviceAttribute::insert(const DevVarULong64Array &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarULong64Array *) -
// insert a DevVarULong64Array by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarULong64Array *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	ext->ULong64Seq = datum;

	del_mem(Tango::DEV_ULONG64);
}

void DeviceAttribute::insert(DevVarULong64Array *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarStateArray &) -
// insert a DevVarStateArray by reference into the DeviceAttribute.
// This inserter copy the data
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (const DevVarStateArray &datum)
{
	dim_x = datum.length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	ext->StateSeq->length(datum.length());
	for (unsigned int i = 0;i < datum.length();i++)
		ext->StateSeq[i] = datum[i];

	del_mem(Tango::DEV_STATE);
}

void DeviceAttribute::insert(const DevVarStateArray &datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::operator <<(DevVarStateArray *) -
// insert a DevVarStateArray by pointer into the DeviceAttribute.
// This inserter takes onwership of the pointed to memory
//
//-----------------------------------------------------------------------------

void DeviceAttribute::operator << (DevVarStateArray *datum)
{
	dim_x = datum->length();
	dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	ext->StateSeq = datum;

	del_mem(Tango::DEV_STATE);
}

void DeviceAttribute::insert(DevVarStateArray *datum,int x,int y)
{
	*this << datum;
	dim_x = x;
	dim_y = y;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract(char *&,unsigned char *&)
//
// - extract the read value as pointers from the DeviceAttribute
// for the DevEncoded data type
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract(const char *&str,unsigned char *&data_ptr,unsigned int &data_size)
{
// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

// copy the read value to the vector

	if (ext->EncodedSeq.operator->() != NULL)
	{
		if (ext->EncodedSeq->length() != 0)
		{
			str = ext->EncodedSeq[0].encoded_format.in();
			data_ptr = ext->EncodedSeq[0].encoded_data.get_buffer();
			data_size = ext->EncodedSeq[0].encoded_data.length();
		}
		else
			ret = false;
	}
	else
	{
// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract(string &,vector<unsigned char> &)
//
// - extract the read value as pointers from the DeviceAttribute
// for the DevEncoded data type
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract(string &str,vector<unsigned char> &dat)
{
	return extract_read(str,dat);
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(string &,vector<unsigned char> &)
//
// - extract the read value as a string, vector<unsigned char> from the DeviceAttribute
// for the DevEncoded data type
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (string &datum_str,vector<unsigned char> &datum)
{
// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

// copy the read value to the vector

	if (ext->EncodedSeq.operator->() != NULL)
	{
		if (ext->EncodedSeq->length() != 0)
		{
			datum_str = ext->EncodedSeq[0].encoded_format;

			unsigned long length = ext->EncodedSeq[0].encoded_data.length();
			datum.resize(length);

         	for (unsigned long i=0; i<length; i++)
         	{
         		datum[i] = ext->EncodedSeq[0].encoded_data[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(string &,vector<unsigned char> &)
//
// - extract the set value as a string,vector<unsigned char> from the DeviceAttribute
// when the data type is DevEncoded
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (string &datum_str,vector<unsigned char> &datum)
{
// check for available data

	bool ret = check_for_data();
	if (ret == false)
		return false;

	if (ext->EncodedSeq.operator->() != NULL)
	{
		if (ext->EncodedSeq->length() == 2)
		{
			datum_str = ext->EncodedSeq[1].encoded_format;

			unsigned long length = ext->EncodedSeq[1].encoded_data.length();
			datum.resize(length);

         	for (unsigned long i=0; i<length; i++)
         	{
         		datum[i] = ext->EncodedSeq[1].encoded_data[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<string> &)
//
// - extract the read value as a vector<string> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<string>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if (ret == false)
		return false;

	// copy the read value to the vector

	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

			for (long i=0; i<length; i++)
			{
				datum[i] = StringSeq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<string> &)
//
// - extract the set value as a vector<string> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<string>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (StringSeq.operator->() != NULL)
	{
		if (StringSeq->length() != 0)
		{
			// check the size of the setpoint values
			int read_length = check_set_value_size (StringSeq->length());

			// copy the set point values to the vector
			datum.resize(StringSeq->length() - read_length);
			unsigned int k = 0;
			for (unsigned int i=read_length; i<StringSeq->length(); i++, k++)
			{
				datum[k] = StringSeq[i];
			}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<short> &)
//
// - extract the read value as a vector<short> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<short>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	// copy the read value to the vector

	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

         	for (long i=0; i<length; i++)
         	{
         		datum[i] = ShortSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<short> &)
//
// - extract the set value as a vector<short> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<short>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
		{
			// check the size of the setpoint values
			long read_length = check_set_value_size (ShortSeq->length());

			// copy the set point values to the vector
			datum.resize(ShortSeq->length() - read_length);
			unsigned int k = 0;
         	for (unsigned int i=read_length; i<ShortSeq->length(); i++, k++)
         	{
         		datum[k] = ShortSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<DevLong> &)
//
// - extract the read value as a vector<DevLong> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<DevLong>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	// copy the read value to the vector

	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

         	for (long i=0; i<length; i++)
         	{
         		datum[i] = LongSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<DevLong> &)
//
// - extract the set value as a vector<DevLong> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<DevLong>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (LongSeq.operator->() != NULL)
	{
		if (LongSeq->length() != 0)
		{
			// check the size of the setpoint values
			long read_length = check_set_value_size (LongSeq->length());

			// copy the set point values to the vector
			datum.resize(LongSeq->length() - read_length);
			unsigned int k = 0;
         	for (unsigned int i=read_length; i<LongSeq->length(); i++, k++)
         	{
         		datum[k] = LongSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<double> &)
//
// - extract the read value as a vector<double> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<double>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	// copy the read value to the vector

	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

         	for (long i=0; i<length; i++)
         	{
         		datum[i] = DoubleSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<double> &)
//
// - extract the set value as a vector<double> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<double>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (DoubleSeq.operator->() != NULL)
	{
		if (DoubleSeq->length() != 0)
		{
			// check the size of the setpoint values
			int read_length = check_set_value_size (DoubleSeq->length());

			// copy the set point values to the vector
			datum.resize(DoubleSeq->length() - read_length);
			unsigned int k = 0;
         	for (unsigned int i=read_length; i<DoubleSeq->length(); i++, k++)
         	{
         		datum[k] = DoubleSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}



//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<float> &)
//
// - extract the read value as a vector<float> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<float>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	// copy the read value to the vector

	if (FloatSeq.operator->() != NULL)
	{
		if (FloatSeq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

         	for (long i=0; i<length; i++)
         	{
         		datum[i] = FloatSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<float> &)
//
// - extract the set value as a vector<float> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<float>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (FloatSeq.operator->() != NULL)
	{
		if (FloatSeq->length() != 0)
		{
			// check the size of the setpoint values
			int read_length = check_set_value_size (FloatSeq->length());

			// copy the set point values to the vector
			datum.resize(FloatSeq->length() - read_length);
			unsigned int k = 0;
         	for (unsigned int i=read_length; i<FloatSeq->length(); i++, k++)
         	{
         		datum[k] = FloatSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<bool> &)
//
// - extract the read value as a vector<bool> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<bool>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	// copy the read value to the vector

	if (BooleanSeq.operator->() != NULL)
	{
		if (BooleanSeq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

         	for (long i=0; i<length; i++)
         	{
         		datum[i] = BooleanSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<bool> &)
//
// - extract the set value as a vector<bool> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<bool>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (BooleanSeq.operator->() != NULL)
	{
		if (BooleanSeq->length() != 0)
		{
			// check the size of the setpoint values
			long read_length = check_set_value_size (BooleanSeq->length());

			// copy the set point values to the vector
			datum.resize(BooleanSeq->length() - read_length);
			unsigned int k = 0;
         for (unsigned int i=read_length; i<BooleanSeq->length(); i++, k++)
         {
         	datum[k] = BooleanSeq[i];
         }
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<unsigned short> &)
//
// - extract the read value as a vector<unsigned short> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<unsigned short>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	// copy the read value to the vector

	if (UShortSeq.operator->() != NULL)
	{
		if (UShortSeq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

         	for (long i=0; i<length; i++)
         	{
         		datum[i] = UShortSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<unsigned short> &)
//
// - extract the set value as a vector<unsigned short> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<unsigned short>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (UShortSeq.operator->() != NULL)
	{
		if (UShortSeq->length() != 0)
		{
			// check the size of the setpoint values
			long read_length = check_set_value_size (UShortSeq->length());

			// copy the set point values to the vector
			datum.resize(UShortSeq->length() - read_length);
			unsigned int k = 0;
         for (unsigned int i=read_length; i<UShortSeq->length(); i++, k++)
         {
         	datum[k] = UShortSeq[i];
         }
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<unsigned char> &)
//
// - extract the read value as a vector<unsigned char> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<unsigned char>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	// copy the read value to the vector

	if (UCharSeq.operator->() != NULL)
	{
		if (UCharSeq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

         	for (long i=0; i<length; i++)
         	{
         		datum[i] = UCharSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<unsigned char> &)
//
// - extract the set value as a vector<unsigned char> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<unsigned char>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (UCharSeq.operator->() != NULL)
	{
		if (UCharSeq->length() != 0)
		{
			// check the size of the setpoint values
			long read_length = check_set_value_size (UCharSeq->length());

			// copy the set point values to the vector
			datum.resize(UCharSeq->length() - read_length);
			unsigned int k = 0;
         for (unsigned int i=read_length; i<UCharSeq->length(); i++, k++)
         {
         	datum[k] = UCharSeq[i];
         }
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<DevLong64> &)
//
// - extract the read value as a vector<DevLong64> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<DevLong64>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	// copy the read value to the vector

	if (ext->Long64Seq.operator->() != NULL)
	{
		if (ext->Long64Seq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

         	for (long i=0; i<length; i++)
         	{
         		datum[i] = ext->Long64Seq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<DevLong64> &)
//
// - extract the set value as a vector<DevLong64> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<DevLong64>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->Long64Seq.operator->() != NULL)
	{
		if (ext->Long64Seq->length() != 0)
		{
			// check the size of the setpoint values
			long read_length = check_set_value_size (ext->Long64Seq->length());

			// copy the set point values to the vector
			datum.resize(ext->Long64Seq->length() - read_length);
			unsigned int k = 0;
         for (unsigned int i=read_length; i<ext->Long64Seq->length(); i++, k++)
         {
         	datum[k] = ext->Long64Seq[i];
         }
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}


//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<DevULong64> &)
//
// - extract the read value as a vector<DevULong64> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<DevULong64>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	// copy the read value to the vector

	if (ext->ULong64Seq.operator->() != NULL)
	{
		if (ext->ULong64Seq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

         	for (long i=0; i<length; i++)
         	{
         		datum[i] = ext->ULong64Seq[i];
        	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<DevULong64> &)
//
// - extract the set value as a vector<DevULong64> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<DevULong64>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->ULong64Seq.operator->() != NULL)
	{
		if (ext->ULong64Seq->length() != 0)
		{
			// check the size of the setpoint values
			long read_length = check_set_value_size (ext->ULong64Seq->length());

			// copy the set point values to the vector
			datum.resize(ext->ULong64Seq->length() - read_length);
			unsigned int k = 0;
         for (unsigned int i=read_length; i<ext->ULong64Seq->length(); i++, k++)
         {
         	datum[k] = ext->ULong64Seq[i];
         }
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}



//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<DevULong> &)
//
// - extract the read value as a vector<DevULong> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<DevULong>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	// copy the read value to the vector

	if (ext->ULongSeq.operator->() != NULL)
	{
		if (ext->ULongSeq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

         	for (long i=0; i<length; i++)
         	{
         		datum[i] = ext->ULongSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<DevULong> &)
//
// - extract the set value as a vector<DevULong> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<DevULong>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->ULongSeq.operator->() != NULL)
	{
		if (ext->ULongSeq->length() != 0)
		{
			// check the size of the setpoint values
			long read_length = check_set_value_size (ext->ULongSeq->length());

			// copy the set point values to the vector
			datum.resize(ext->ULongSeq->length() - read_length);
			unsigned int k = 0;
         for (unsigned int i=read_length; i<ext->ULongSeq->length(); i++, k++)
         {
         	datum[k] = ext->ULongSeq[i];
         }
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}



//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_read(vector<DevState> &)
//
// - extract the read value as a vector<DevState> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_read (vector<DevState>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	// copy the read value to the vector

	if (ext->StateSeq.operator->() != NULL)
	{
		if (ext->StateSeq->length() != 0)
		{
			long length = get_nb_read();
			datum.resize(length);

         	for (long i=0; i<length; i++)
         	{
         		datum[i] = ext->StateSeq[i];
         	}
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceAttribute::extract_set(vector<DevState> &)
//
// - extract the set value as a vector<DevState> from the DeviceAttribute
//
//-----------------------------------------------------------------------------

bool DeviceAttribute::extract_set (vector<DevState>& datum)
{
	// check for available data

	bool ret = check_for_data();
	if ( ret == false)
		return false;

	if (ext->StateSeq.operator->() != NULL)
	{
		if (ext->StateSeq->length() != 0)
		{
			// check the size of the setpoint values
			long read_length = check_set_value_size (ext->StateSeq->length());

			// copy the set point values to the vector
			datum.resize(ext->StateSeq->length() - read_length);
			unsigned int k = 0;
         for (unsigned int i=read_length; i<ext->StateSeq->length(); i++, k++)
         {
         	datum[k] = ext->StateSeq[i];
         }
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}




//+-------------------------------------------------------------------------
//
// method name : 	check_for data
//
// description : 	Checks whether attribute data is available.
//                In case of missing data, an exception will be
//                thrown when the failed exception flag is set.
//                Otherwise the method returns false.
//
// ret : false when no attribute data is available.
//
//--------------------------------------------------------------------------
bool DeviceAttribute::check_for_data()
{
	if (ext->err_list.operator->() != NULL)
	{
		if (ext->err_list.in().length() != 0)
		{
			if (exceptions_flags.test(failed_flag))
				throw DevFailed(ext->err_list.in());
			else
				return false;
		}
	}

	if (is_empty() == true)
		return false;

	return true;
}

//+-------------------------------------------------------------------------
//
// method name : 	check_wrong_type_exception
//
// description : 	Checks whether the wrongtype exception flag is set
//                and throws an exception in this case.
//                Otherwise the method will return false.
//
// ret : false when no exception was thrown.
//
//--------------------------------------------------------------------------
bool DeviceAttribute::check_wrong_type_exception()
{
	if (exceptions_flags.test(wrongtype_flag))
	{
		ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
					(const char*)"Cannot extract, data in DeviceAttribute object is not an array of short",
					(const char*)"DeviceAttribute::operator>>");
	}

	return false;
}

//+-------------------------------------------------------------------------
//
// method name : 	check_set_value_size
//
// description : 	checks wether set value data is available and
//                calculates the index of the first set value
//                element.
//
// arg(s) : - seq_length : the length of the attribute data array
// ret    :   the index of the first set value element.
//
//--------------------------------------------------------------------------
int DeviceAttribute::check_set_value_size(int seq_length)
{
	// check if the attribute data contains a set value

	if ( get_nb_written() == 0 )
	{
		// no set point available

		ApiDataExcept::throw_exception((const char*)"API_NoSetValueAvailable",
					(const char*)"Cannot extract, data from the DeviceAttribute object. No set value available",
					(const char*)"DeviceAttribute::extract_set");

	}

	// For Tango::WRITE attributes, the read and set value are identical!
	// In this case the number of set values is the same as the number
	// of data elements in the returned sequence.

	if ( get_nb_written() == seq_length )
	{
		return  0;
	}
	else
	{
		return get_nb_read();
	}
}




//+-------------------------------------------------------------------------
//
// method name : 	del_mem
//
// description : 	Delete already allocated memory except for the data
//					just inserted
//
// arg(s) : - data_type : The data type just inserted
//
//--------------------------------------------------------------------------

void DeviceAttribute::del_mem(int data_type)
{
	if ((data_type != Tango::DEV_STRING) && (StringSeq.operator->() != NULL))
			delete StringSeq._retn();
	if ((data_type != Tango::DEV_LONG) && (LongSeq.operator->() != NULL))
			delete LongSeq._retn();
	if ((data_type != Tango::DEV_SHORT) && (ShortSeq.operator->() != NULL))
			delete ShortSeq._retn();
	if ((data_type != Tango::DEV_DOUBLE) && (DoubleSeq.operator->() != NULL))
		delete DoubleSeq._retn();
	if ((data_type != Tango::DEV_FLOAT) && (FloatSeq.operator->() != NULL))
		delete FloatSeq._retn();
	if ((data_type != Tango::DEV_BOOLEAN) && (BooleanSeq.operator->() != NULL))
		delete BooleanSeq._retn();
	if ((data_type != Tango::DEV_USHORT) && (UShortSeq.operator->() != NULL))
		delete UShortSeq._retn();
	if ((data_type != Tango::DEV_UCHAR) && (UCharSeq.operator->() != NULL))
		delete UCharSeq._retn();
	if ((data_type != Tango::DEV_LONG64) && (ext->Long64Seq.operator->() != NULL))
		delete ext->Long64Seq._retn();
	if ((data_type != Tango::DEV_ULONG) && (ext->ULongSeq.operator->() != NULL))
		delete ext->ULongSeq._retn();
	if ((data_type != Tango::DEV_ULONG64) && (ext->ULong64Seq.operator->() != NULL))
		delete ext->ULong64Seq._retn();
	if ((data_type != Tango::DEV_STATE) && (ext->StateSeq.operator->() != NULL))
		delete ext->StateSeq._retn();
	if ((data_type != Tango::DEV_ENCODED) && (ext->EncodedSeq.operator->() != NULL))
		delete ext->EncodedSeq._retn();
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
//
// description : 	Friend function to ease printing instance of the
//			DeviceAttribute class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,DeviceAttribute &da)
{

	bitset<DeviceAttribute::numFlags> bs = da.exceptions();
	da.reset_exceptions(DeviceAttribute::isempty_flag);
	if (da.is_empty() == true)
	{
		da.exceptions(bs);
		o_str << "No data in DeviceAttribute object";
		return o_str;
	}
	da.exceptions(bs);

//
// Print date
//

	if (da.time.tv_sec != 0)
	{
#ifdef _TG_WINDOWS_
		time_t tmp_val = da.time.tv_sec;
		struct tm *tmp_time = localtime(&tmp_val);
		char *tmp_date = asctime(tmp_time);
#else
		char *tmp_date = asctime(localtime((time_t *)&da.time.tv_sec));
#endif
		tmp_date[strlen(tmp_date) - 1] = '\0';
		o_str << tmp_date;
		o_str << " (" << da.time.tv_sec << "," << da.time.tv_usec << " sec) : ";
	}

//
// print attribute name
//

	o_str << da.name;

//
// print dim_x and dim_y
//

	o_str << " (dim_x = " << da.dim_x << ", dim_y = " << da.dim_y << ", ";
	o_str << "w_dim_x = " << da.ext->w_dim_x << ", w_dim_y = " << da.ext->w_dim_y << ", ";

//
// Print quality
//

	o_str << "Data quality factor = ";
	switch (da.quality)
	{
	case Tango::ATTR_VALID:
		o_str << "VALID, ";
		break;

	case Tango::ATTR_INVALID:
		o_str << "INVALID, ";
		break;

	case Tango::ATTR_ALARM:
		o_str << "ALARM, ";
		break;

	case Tango::ATTR_CHANGING:
		o_str << "CHANGING, ";
		break;

	case Tango::ATTR_WARNING:
		o_str << "WARNING, ";
		break;
	}

//
// Print data format
//

	o_str << "Data format = ";
	switch (da.data_format)
	{
	case Tango::SCALAR:
		o_str << "SCALAR)" << endl;
		break;

	case Tango::SPECTRUM:
		o_str << "SPECTRUM)" << endl;
		break;

	case Tango::IMAGE:
		o_str << "IMAGE)" << endl;
		break;

	case Tango::FMT_UNKNOWN:
		o_str << "UNKNOWN)" << endl;
		break;
	}

//
// Print data (if valid)
//

	if (da.quality != Tango::ATTR_INVALID)
	{
		if (da.LongSeq.operator->() != NULL)
			o_str << *(da.LongSeq.operator->());
		else if (da.ext->Long64Seq.operator->() != NULL)
			o_str << *(da.ext->Long64Seq.operator->());
		else if (da.ShortSeq.operator->() != NULL)
			o_str << *(da.ShortSeq.operator->());
		else if (da.DoubleSeq.operator->() != NULL)
			o_str << *(da.DoubleSeq.operator->());
		else if (da.FloatSeq.operator->() != NULL)
			o_str << *(da.FloatSeq.operator->());
		else if (da.BooleanSeq.operator->() != NULL)
			o_str << *(da.BooleanSeq.operator->());
		else if (da.UShortSeq.operator->() != NULL)
			o_str << *(da.UShortSeq.operator->());
		else if (da.UCharSeq.operator->() != NULL)
			o_str << *(da.UCharSeq.operator->());
		else if (da.StringSeq.operator->() != NULL)
			o_str << *(da.StringSeq.operator->());
		else if (da.ext->ULongSeq.operator->() != NULL)
			o_str << *(da.ext->ULongSeq.operator->());
		else if (da.ext->ULong64Seq.operator->() != NULL)
			o_str << *(da.ext->ULong64Seq.operator->());
		else if (da.ext->StateSeq.operator->() != NULL)
			o_str << *(da.ext->StateSeq.operator->());
		else if (da.ext->EncodedSeq.operator->() != NULL)
			o_str << *(da.ext->EncodedSeq.operator->());
		else
			o_str << DevStateName[da.d_state];
	}

	return o_str;
}

} // End of Tango namespace
