static const char *RcsId = "$Id$";

//===================================================================================================================
//
// devapi_pipe.cpp 	- C++ source code file for TANGO devapi class DevicePipe
//
// programmer(s) 	- E. Taurel
//
// original 		- March 2001
//
// Copyright (C) :      2014,2015
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
// version 		- $Revision$
//
//====================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

using namespace CORBA;

namespace Tango
{

//------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DevicePipe::DevicePipe()
//
// description:
//		Constructor for DevicePipe class
//
//------------------------------------------------------------------------------------------------------------------

DevicePipe::DevicePipe():ext(Tango_nullptr)
{
}

DevicePipe::DevicePipe(const string &pipe_name):name(pipe_name),ext(Tango_nullptr)
{
}

DevicePipe::DevicePipe(const string &pipe_name,const string &root_blob_name):name(pipe_name),ext(Tango_nullptr)
{
	the_root_blob.set_name(root_blob_name);
}

//-----------------------------------------------------------------------------
//
// DevicePipe::DevicePipe() - copy constructor to create DevicePipe
//
//-----------------------------------------------------------------------------

DevicePipe::DevicePipe(const DevicePipe & source):ext(Tango_nullptr)
{
	name = source.name;
	time = source.time;
	the_root_blob = source.the_root_blob;

#ifdef HAS_UNIQUE_PTR
    if (source.ext.get() != NULL)
    {
        ext.reset(new DevicePipeExt);
        *(ext.get()) = *(source.ext.get());
    }
#else
	if (source.ext != NULL)
	{
		ext = new DevicePipeExt();
		*ext = *(source.ext);
	}
	else
		ext = NULL;
#endif
}

//-----------------------------------------------------------------------------
//
// DevicePipe::operator= - assignment operator for DevicePipe class
//
//-----------------------------------------------------------------------------

DevicePipe &DevicePipe::operator=(const DevicePipe &rhs)
{
	if (this != &rhs)
    {
		name = rhs.name;
    	time = rhs.time;
		the_root_blob = rhs.the_root_blob;

#ifdef HAS_UNIQUE_PTR
		if (rhs.ext.get() != NULL)
		{
			ext.reset(new DevicePipeExt);
			*(ext.get()) = *(rhs.ext.get());
		}
#else
		if (rhs.ext != NULL)
		{
			ext = new DevicePipeExt();
			*ext = *(rhs.ext);
		}
		else
			ext = NULL;
#endif
    }
	return *this;
}

//-----------------------------------------------------------------------------
//
// DevicePipe::DevicePipe() - Move copy constructor to create DevicePipe
//
//-----------------------------------------------------------------------------

#ifdef HAS_RVALUE
DevicePipe::DevicePipe(DevicePipe && source):ext(Tango_nullptr)
{
	name = move(source.name);
	time = source.time;
	the_root_blob = move(source.the_root_blob);

    if (source.ext.get() != NULL)
    {
        ext = move(source.ext);
    }
}
#endif // HAS_RVALUE

//-----------------------------------------------------------------------------
//
// DevicePipe::operator= - Move assignment operator for DevicePipe class
//
//-----------------------------------------------------------------------------

#ifdef HAS_RVALUE
DevicePipe &DevicePipe::operator=(DevicePipe &&rhs)
{
	name = move(rhs.name);
	time = rhs.time;
	the_root_blob = move(rhs.the_root_blob);

	if (rhs.ext.get() != NULL)
	{
		ext = move(rhs.ext);
	}
	else
		ext.reset();

	return *this;
}
#endif

//----------------------------------------------------------------------------------------------------------------
//
// DevicePipe::~DevicePipe() - destructor to destroy DevicePipe
//
//----------------------------------------------------------------------------------------------------------------

DevicePipe::~DevicePipe()
{
#ifndef HAS_UNIQUE_PTR
    delete ext;
#endif
}

DevicePipe &DevicePipe::operator[](const string &_na)
{
	the_root_blob.operator[](_na);
	return *this;
}

//*************************************************************************************************************
//
//						DevicePipeBlob class
//
//*************************************************************************************************************

//------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DevicePipeBlob::DevicePipeBlob()
//
// description:
//		Constructor for DevicePipeBlob class
//
//------------------------------------------------------------------------------------------------------------------

DevicePipeBlob::DevicePipeBlob():failed(false),insert_elt_array(Tango_nullptr),insert_ctr(0),
extract_elt_array(Tango_nullptr),extract_ctr(0),extract_delete(false),ext(Tango_nullptr)
{
	exceptions_flags.set();
	ext_state.reset();
	insert_ind = -1;
	extract_ind = -1;
}


DevicePipeBlob::DevicePipeBlob(const string &blob_name):name(blob_name),failed(false),
insert_elt_array(Tango_nullptr),insert_ctr(0),extract_elt_array(Tango_nullptr),extract_ctr(0),
extract_delete(false),ext(Tango_nullptr)
{
	exceptions_flags.set();
	ext_state.reset();
	insert_ind = -1;
	extract_ind = -1;
}

//----------------------------------------------------------------------------------------------------------------
//
// DevicePipeBlob::~DevicePipeBlob() - destructor to destroy DevicePipeBlob instance
//
//----------------------------------------------------------------------------------------------------------------

DevicePipeBlob::~DevicePipeBlob()
{
	if (extract_delete == true)
		delete extract_elt_array;

#ifndef HAS_UNIQUE_PTR
    delete ext;
#endif
}

//-----------------------------------------------------------------------------
//
// DevicePipeBlob::DevicePipeBlob() - copy constructor to create DevicePipeBlob
//
//-----------------------------------------------------------------------------

DevicePipeBlob::DevicePipeBlob(const DevicePipeBlob & source):ext(Tango_nullptr)
{
	name = source.name;
	exceptions_flags = source.exceptions_flags;
	ext_state = source.ext_state;
	failed = source.failed;
	insert_ind = source.insert_ind;
	extract_ind = source.extract_ind;

	if (source.insert_elt_array != Tango_nullptr)
		(*insert_elt_array) = (*source.insert_elt_array);
	else
		insert_elt_array = Tango_nullptr;
	insert_ctr = source.insert_ctr;

	if (source.extract_elt_array != Tango_nullptr)
	{
		DevVarPipeDataEltArray *tmp = new DevVarPipeDataEltArray();
		*tmp = (*source.extract_elt_array);
		set_extract_data(tmp);
		extract_delete = true;
	}
	else
	{
		extract_delete = source.extract_delete;
	}
	extract_ctr = source.extract_ctr;

#ifdef HAS_UNIQUE_PTR
    if (source.ext.get() != NULL)
    {
        ext.reset(new DevicePipeBlobExt);
        *(ext.get()) = *(source.ext.get());
    }
#else
	if (source.ext != NULL)
	{
		ext = new DevicePipeBlobExt();
		*ext = *(source.ext);
	}
	else
		ext = NULL;
#endif
}

//-----------------------------------------------------------------------------
//
// DevicePipeBlob::operator= - assignment operator for DevicePipeBlob class
//
//-----------------------------------------------------------------------------

DevicePipeBlob &DevicePipeBlob::operator=(const DevicePipeBlob &rhs)
{
	if (this != &rhs)
    {
		name = rhs.name;
		exceptions_flags = rhs.exceptions_flags;
		ext_state = rhs.ext_state;
		failed = rhs.failed;
		insert_ind = rhs.insert_ind;
		extract_ind = rhs.extract_ind;

		if (rhs.insert_elt_array != Tango_nullptr)
			(*insert_elt_array) = (*rhs.insert_elt_array);
		else
			insert_elt_array = Tango_nullptr;
		insert_ctr = rhs.insert_ctr;

		if (rhs.extract_elt_array != Tango_nullptr)
		{
			DevVarPipeDataEltArray *tmp = new DevVarPipeDataEltArray();
			*tmp = (*rhs.extract_elt_array);
			set_extract_data(tmp);
			extract_delete = true;
		}
		else
		{
			extract_delete = rhs.extract_delete;
		}
		extract_ctr = rhs.extract_ctr;

#ifdef HAS_UNIQUE_PTR
        if (rhs.ext.get() != NULL)
        {
            ext.reset(new DevicePipeBlobExt);
            *(ext.get()) = *(rhs.ext.get());
        }
        else
            ext.reset();
#else
        delete ext;
        if (rhs.ext != NULL)
        {
            ext = new DevicePipeBlobExt();
            *ext = *(rhs.ext);
        }
        else
            ext = NULL;
#endif
    }
	return *this;
}

//-----------------------------------------------------------------------------
//
// DevicePipeBlob::DevicePipeBlob() - Move copy constructor to create DevicePipeBlob
//
//-----------------------------------------------------------------------------

#ifdef HAS_RVALUE
DevicePipeBlob::DevicePipeBlob(DevicePipeBlob && source):ext(Tango_nullptr)
{
	name = move(source.name);
	exceptions_flags = source.exceptions_flags;
	ext_state = source.ext_state;
	failed = source.failed;
	insert_ind = source.insert_ind;
	extract_ind = source.extract_ind;

	if (source.insert_elt_array != Tango_nullptr)
		(*insert_elt_array) = (*source.insert_elt_array);
	else
		insert_elt_array = Tango_nullptr;
	insert_ctr = source.insert_ctr;

	if (source.extract_elt_array != Tango_nullptr)
	{
		DevVarPipeDataEltArray *tmp = new DevVarPipeDataEltArray();
		*tmp = (*source.extract_elt_array);
		set_extract_data(tmp);
		extract_delete = true;
	}
	else
	{
		extract_delete = source.extract_delete;
	}
	extract_ctr = source.extract_ctr;

    if (source.ext.get() != NULL)
    {
        ext = move(source.ext);
    }
}
#endif

//-----------------------------------------------------------------------------
//
// DevicePipeBlob::operator= - Move assignment operator for DevicePipeBlob class
//
//-----------------------------------------------------------------------------

#ifdef HAS_RVALUE
DevicePipeBlob &DevicePipeBlob::operator=(DevicePipeBlob &&rhs)
{
	name = move(rhs.name);
	exceptions_flags = rhs.exceptions_flags;
	ext_state = rhs.ext_state;
	failed = rhs.failed;
	insert_ind = rhs.insert_ind;
	extract_ind = rhs.extract_ind;

	if (rhs.insert_elt_array != Tango_nullptr)
		insert_elt_array = rhs.insert_elt_array;
	rhs.insert_elt_array = Tango_nullptr;
	insert_ctr = rhs.insert_ctr;

	if (extract_delete == true)
		delete extract_elt_array;

	if (rhs.extract_elt_array != Tango_nullptr)
		extract_elt_array = rhs.extract_elt_array;
	rhs.extract_elt_array = Tango_nullptr;
	extract_delete = rhs.extract_delete;
	extract_ctr = rhs.extract_ctr;


    if (rhs.ext.get() != NULL)
    {
        ext = move(rhs.ext);
    }
    else
		ext.reset();

	return *this;
}
#endif

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::get_data_elt_name
//
// description :
//		Get all data element name
//
// return:
//		vector with the data elt name
//
//-------------------------------------------------------------------------------------------------------------------

vector<string> DevicePipeBlob::get_data_elt_names()
{
	vector<string> v_str;
	size_t nb_elt = extract_elt_array->length();

	for (size_t loop = 0;loop < nb_elt;loop++)
	{
		v_str.push_back(string((*extract_elt_array)[loop].name.in()));
	}

	return v_str;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::get_data_elt_name
//
// description :
//		Get data element name for a single data element
//
// argument :
//		in:
//			- _ind : Data element index in the blob
//
// return:
//		The data elt name
//
//-------------------------------------------------------------------------------------------------------------------

string DevicePipeBlob::get_data_elt_name(size_t _ind)
{
	if (_ind > extract_elt_array->length())
	{
		stringstream ss;

		ss << "Given index (" << _ind << ") is above the number of data element in the data blob (" << get_data_elt_nb() << ")";
		Except::throw_exception(API_PipeWrongArg,ss.str(),"DevicePipeBlob::get_data_elt_name()");
	}
	string tmp((*extract_elt_array)[_ind].name.in());
	return tmp;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::get_data_elt_type
//
// description :
//		Get data element name for a single data element
//
// argument :
//		in:
//			- _ind : Data element index in the blob
//
// return:
//		The data elt name
//
//-------------------------------------------------------------------------------------------------------------------

int DevicePipeBlob::get_data_elt_type(size_t _ind)
{
	int ret = 0;

	if (_ind > extract_elt_array->length())
	{
		stringstream ss;

		ss << "Given index (" << _ind << ") is above the number of data element in the data blob (" << get_data_elt_nb() << ")";
		Except::throw_exception(API_PipeWrongArg,ss.str(),"DevicePipeBlob::get_data_elt_type()");
	}

//
// Do we have a Blob at this index?
//

	if ((*extract_elt_array)[_ind].inner_blob.length() != 0)
	{
		ret = DEV_PIPE_BLOB;
	}
	else
	{
        string d_type = (*extract_elt_array)[_ind].inner_blob_name.in();
		switch((*extract_elt_array)[_ind].value._d())
		{
			case ATT_BOOL:
			{
				if (d_type == SCALAR_PIPE)
                    ret = DEV_BOOLEAN;
                else
                    ret = DEVVAR_BOOLEANARRAY;
			}
			break;

			case ATT_SHORT:
			{
				if (d_type == SCALAR_PIPE)
					ret = DEV_SHORT;
				else
					ret = DEVVAR_SHORTARRAY;
			}
			break;

			case ATT_LONG:
			{
				if (d_type == SCALAR_PIPE)
                    ret = DEV_LONG;
                else
                    ret = DEVVAR_LONGARRAY;
			}
			break;

			case ATT_LONG64:
			{
				if (d_type == SCALAR_PIPE)
					ret = DEV_LONG64;
				else
					ret = DEVVAR_LONG64ARRAY;
			}
			break;

			case ATT_FLOAT:
			{
				if (d_type == SCALAR_PIPE)
                    ret = DEV_FLOAT;
                else
					ret = DEVVAR_FLOATARRAY;
			}
			break;

			case ATT_DOUBLE:
			{
				if (d_type == SCALAR_PIPE)
                    ret = DEV_DOUBLE;
                else
					ret = DEVVAR_DOUBLEARRAY;
			}
			break;

			case ATT_UCHAR:
			{
				if (d_type == SCALAR_PIPE)
                    ret = DEV_UCHAR;
                else
					ret = DEVVAR_CHARARRAY;
			}
			break;

			case ATT_USHORT:
			{
				if (d_type == SCALAR_PIPE)
                    ret = DEV_USHORT;
                else
					ret = DEVVAR_USHORTARRAY;
			}
			break;

			case ATT_ULONG:
			{
				if (d_type == SCALAR_PIPE)
                    ret = DEV_ULONG;
                else
					ret = DEVVAR_ULONGARRAY;
			}
			break;

			case ATT_ULONG64:
			{
				if (d_type == SCALAR_PIPE)
                    ret = DEV_ULONG64;
                else
					ret = DEVVAR_ULONG64ARRAY;
			}
			break;

			case ATT_STRING:
			{
				if (d_type == SCALAR_PIPE)
                    ret = DEV_STRING;
                else
					ret = DEVVAR_STRINGARRAY;
			}
			break;

			case ATT_STATE:
			{
				if (d_type == SCALAR_PIPE)
                    ret = DEV_STATE;
                else
					ret = DEVVAR_STATEARRAY;
			}
			break;

			case ATT_ENCODED:
			{
				ret = DEV_ENCODED;
			}
			break;

			default:
			Except::throw_exception(API_PipeWrongArg,
									"Unsupported data type in data element! (ATT_NO_DATA, DEVICE_STATE)",
									"DevicePipeBlob::get_data_elt_type()");
			break;
		}
	}

	return ret;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::get_extract_ind_from_name
//
// description :
//		Get the index of a data element in the extract blob from its name. Throw exception if not found
//
// argument :
//		in:
//			- _na : Data element name
//
// return:
//		The data elt index.
//
//-------------------------------------------------------------------------------------------------------------------

size_t DevicePipeBlob::get_extract_ind_from_name(const string &_na)
{
	string lower_name(_na);
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);

	bool found = false;
	size_t loop;

	if (extract_elt_array == Tango_nullptr)
	{
		Except::throw_exception(API_PipeNoDataElement,
								"No data element available for extraction",
								"DevicePipeBlob::get_extract_ind_from_name()");
	}

	if (extract_ctr > 0)
	{
		Except::throw_exception(API_NotSupportedFeature,
								"Not supported to mix extraction type (operator >> and operator [])",
								"DevicePipeBlob::get_extract_ind_from_name()");
	}

	for (loop = 0;loop < extract_elt_array->length();loop++)
	{
		string tmp((*extract_elt_array)[loop].name.in());
		transform(tmp.begin(),tmp.end(),tmp.begin(),::tolower);

		if (tmp == lower_name)
		{
			found = true;
			break;
		}
	}

	if (found == false)
	{
		stringstream ss;

		ss << "Can't get data element with name " << _na;
		Except::throw_exception(API_PipeWrongArg,ss.str(),"DevicePipeBlob::get_extract_ind_from_name()");
	}

	extract_ctr = -1;
	return loop;
}

size_t DevicePipeBlob::get_insert_ind_from_name(const string &_na)
{
	string lower_name(_na);
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);

	bool found = false;
	size_t loop;

	if (insert_elt_array == Tango_nullptr)
	{
		Except::throw_exception(API_PipeNoDataElement,
								"No data element available for insertion",
								"DevicePipeBlob::get_insert_ind_from_name()");
	}

	if (insert_ctr > 0)
	{
		Except::throw_exception(API_NotSupportedFeature,
								"Not supported to mix insertion type (operator << and operator [])",
								"DevicePipeBlob::get_insert_ind_from_name()");
	}

	for (loop = 0;loop < insert_elt_array->length();loop++)
	{
		string tmp((*insert_elt_array)[loop].name.in());
		transform(tmp.begin(),tmp.end(),tmp.begin(),::tolower);

		if (tmp == lower_name)
		{
			found = true;
			break;
		}
	}

	if (found == false)
	{
		stringstream ss;

		ss << "Can't get data element with name " << _na;
		Except::throw_exception(API_PipeWrongArg,ss.str(),"DevicePipeBlob::get_insert_ind_from_name()");
	}

	insert_ctr = -1;
	return loop;
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::operator[]
//
// description :
//		Return the object itself with the extract counter set to the ind of the data element with name given as
//		parameter. This allows user to write code like
//			pipe_blob["MyDataElt"] >> sh;
//
// argument :
//		in:
//			- _na : Data element name
//
// return:
//		The DevicePipeBlob object itself
//
//-------------------------------------------------------------------------------------------------------------------

DevicePipeBlob &DevicePipeBlob::operator[](const string &_na)
{
	int ind;
	try
	{
		ind = get_extract_ind_from_name(_na);
		extract_ind = ind;
	}
	catch (Tango::DevFailed &e)
	{
		string reason(e.errors[0].reason.in());
		if (reason != API_PipeNoDataElement)
			throw;
	}

	try
	{
		ind = get_insert_ind_from_name(_na);
		insert_ind = ind;
	}
	catch (Tango::DevFailed &e)
	{
		string reason(e.errors[0].reason.in());
		if (reason != API_PipeNoDataElement)
			throw;
	}

	return *this;
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::set_data_elt_names
//
// description :
//		Set the blob data element name(s)
//
// argument :
//		in:
//			- elt_names : Vector with data elements name
//
//-------------------------------------------------------------------------------------------------------------------


void DevicePipeBlob::set_data_elt_names(vector<string> &elt_names)
{

//
// Check that we do not have two times the same DE name (case independant)
//

	if (elt_names.size() > 1)
	{
	    unsigned int i;
		vector<string> same_de = elt_names;

		for (i = 0;i < same_de.size();++i)
			transform(same_de[i].begin(),same_de[i].end(),same_de[i].begin(),::tolower);
		sort(same_de.begin(),same_de.end());
		vector<string> same_de_lower = same_de;

		vector<string>::iterator pos = unique(same_de.begin(),same_de.end());

		int duplicate_de;
		duplicate_de = distance(elt_names.begin(),elt_names.end()) - distance(same_de.begin(),pos);

		if (duplicate_de != 0)
		{
			stringstream desc;
			desc << "Several times the same data element name in provided vector: ";
			int ctr = 0;
			for (i = 0;i < same_de_lower.size() - 1;i++)
			{
				if (same_de_lower[i] == same_de_lower[i + 1])
				{
					ctr++;
					desc << same_de_lower[i];
					if (ctr < duplicate_de)
						desc << ", ";
				}
			}
			ApiConnExcept::throw_exception(API_PipeDuplicateDEName,desc.str(),"set_data_elt_names");
		}
	}

//
// Init insert elt array and set names
//

	insert_elt_array = new DevVarPipeDataEltArray();
	insert_elt_array->length(elt_names.size());

	for (size_t loop = 0;loop < elt_names.size();loop++)
	{
		(*insert_elt_array)[loop].name = CORBA::string_dup(elt_names[loop].c_str());
		(*insert_elt_array)[loop].value.union_no_data(true);
		(*insert_elt_array)[loop].inner_blob.length(0);
	}

	insert_ctr = 0;
	extract_ctr = 0;
	insert_ind = -1;
	extract_ind = -1;
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::set_data_elt_nb
//
// description :
//		Set the blob data element number
//
// argument :
//		in:
//			- _nb : The data elements number
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::set_data_elt_nb(size_t _nb)
{
	insert_elt_array = new DevVarPipeDataEltArray();
	insert_elt_array->length(_nb);

	for (size_t loop = 0;loop < _nb;loop++)
	{
		(*insert_elt_array)[loop].value.union_no_data(true);
		(*insert_elt_array)[loop].inner_blob.length(0);
	}

	insert_ctr = 0;
	extract_ctr = 0;
	insert_ind = -1;
	extract_ind = -1;
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::set_current_delt_name
//
// description :
//		Set the data element name. This method create the pipe data elt array (insert) if not already done
//
// argument :
//		in:
//			- _na : The data elements name
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::set_current_delt_name(const string &_na)
{
    if (insert_elt_array == Tango_nullptr)
    {
        insert_elt_array = new DevVarPipeDataEltArray(10);
        insert_elt_array->length(1);

        (*insert_elt_array)[0].value.union_no_data(true);
        (*insert_elt_array)[0].inner_blob.length(0);

        insert_ctr = 0;
        extract_ctr = 0;
        insert_ind = -1;
        extract_ind = -1;
    }
    else if (insert_ctr == (int)insert_elt_array->length())
    {
        insert_elt_array->length(insert_elt_array->length() + 1);

        (*insert_elt_array)[insert_ctr].value.union_no_data(true);
        (*insert_elt_array)[insert_ctr].inner_blob.length(0);
    }

    (*insert_elt_array)[insert_ctr].name = CORBA::string_dup(_na.c_str());
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::get_data_elt_nb()
//
// description :
//		Get the data element number in the blob
//
// return :
//		The data element number
//
//-------------------------------------------------------------------------------------------------------------------

size_t DevicePipeBlob::get_data_elt_nb()
{
    size_t ret;

    if (extract_elt_array == Tango_nullptr)
    {
        if (insert_elt_array == Tango_nullptr)
            ret = 0;
        else
            ret = insert_elt_array->length();
    }
    else
        ret = extract_elt_array->length();

    return ret;
}

//******************************************************************************************************************

DevicePipeBlob & DevicePipeBlob::operator<<(DevBoolean &datum)
{
	INSERT_BASIC_TYPE(DevVarBooleanArray,bool_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(short &datum)
{
	INSERT_BASIC_TYPE(DevVarShortArray,short_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevLong &datum)
{
	INSERT_BASIC_TYPE(DevVarLongArray,long_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevLong64 &datum)
{
	INSERT_BASIC_TYPE(DevVarLong64Array,long64_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(float &datum)
{
	INSERT_BASIC_TYPE(DevVarFloatArray,float_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(double &datum)
{
	INSERT_BASIC_TYPE(DevVarDoubleArray,double_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevUChar &datum)
{
	INSERT_BASIC_TYPE(DevVarUCharArray,uchar_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevUShort &datum)
{
	INSERT_BASIC_TYPE(DevVarUShortArray,ushort_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevULong &datum)
{
	INSERT_BASIC_TYPE(DevVarULongArray,ulong_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevULong64 &datum)
{
	INSERT_BASIC_TYPE(DevVarULong64Array,ulong64_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevString &datum)
{
	failed = false;
	ext_state.reset();

	if (insert_elt_array == Tango_nullptr)
		ext_state.set(blobdenamenotset_flag);
	else if (insert_ctr == -1 && insert_ind == -1)
		ext_state.set(mixing_flag);
	else
	{
		size_t nb_insert = insert_elt_array->length();
		if (nb_insert == 0 || insert_ctr > (int)nb_insert - 1)
			ext_state.set(notenoughde_flag);
		else
		{
			DevVarStringArray dvsa;
			dvsa.length(1);
			dvsa[0] = CORBA::string_dup(datum);

			if (insert_ind != -1)
			{
				(*insert_elt_array)[insert_ind].value.string_att_value(dvsa);
                (*insert_elt_array)[insert_ctr].inner_blob_name = CORBA::string_dup("Scalar");
				insert_ind = -1;
			}
			else
			{
				(*insert_elt_array)[insert_ctr].value.string_att_value(dvsa);
                (*insert_elt_array)[insert_ctr].inner_blob_name = CORBA::string_dup("Scalar");
				insert_ctr++;
			}
		}

	}

	if (ext_state.any() == true)
		failed = true;

	if (ext_state.test(blobdenamenotset_flag) == true && exceptions_flags.test(blobdenamenotset_flag) == true)
		throw_name_not_set("operator<<");

	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true)
		throw_too_many("operator<<",false);

	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true)
		throw_mixing("operator>>");

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevState &datum)
{
	INSERT_BASIC_TYPE(DevVarStateArray,state_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevEncoded &datum)
{
	INSERT_BASIC_TYPE(DevVarEncodedArray,encoded_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(const string &datum)
{
	failed = false;
	ext_state.reset();

	if (insert_elt_array == Tango_nullptr)
		ext_state.set(blobdenamenotset_flag);
	else if (insert_ctr == -1 && insert_ind == -1)
		ext_state.set(mixing_flag);
	else
	{
		size_t nb_insert = insert_elt_array->length();
		if (nb_insert == 0 || insert_ctr > (int)nb_insert - 1)
			ext_state.set(notenoughde_flag);
		else
		{
			DevVarStringArray dvsa;
			dvsa.length(1);
			dvsa[0] = CORBA::string_dup(datum.c_str());

			if (insert_ind != -1)
			{
				(*insert_elt_array)[insert_ind].value.string_att_value(dvsa);
                (*insert_elt_array)[insert_ind].inner_blob_name = CORBA::string_dup("Scalar");
				insert_ind = -1;
			}
			else
			{
				(*insert_elt_array)[insert_ctr].value.string_att_value(dvsa);
                (*insert_elt_array)[insert_ctr].inner_blob_name = CORBA::string_dup("Scalar");
				insert_ctr++;
			}
		}
	}

	if (ext_state.any() == true)
		failed = true;

	if (ext_state.test(blobdenamenotset_flag) == true && exceptions_flags.test(blobdenamenotset_flag) == true)
		throw_name_not_set("operator<<");

	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true)
		throw_too_many("operator<<",false);

	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true)
		throw_mixing("operator>>");

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevicePipeBlob &datum)
{
	failed = false;
	ext_state.reset();

	if (insert_elt_array == Tango_nullptr)
		ext_state.set(blobdenamenotset_flag);
	else if (insert_ctr == -1 && insert_ind == -1)
		ext_state.set(mixing_flag);
	else
	{
		size_t nb_insert = insert_elt_array->length();
		if (nb_insert == 0 || insert_ctr > (int)nb_insert - 1)
			ext_state.set(notenoughde_flag);
		else
		{
			DevVarPipeDataEltArray *tmp_ptr = datum.get_insert_data();
			if (tmp_ptr != Tango_nullptr)
			{
				CORBA::ULong max,len;
				max = tmp_ptr->maximum();
				len = tmp_ptr->length();

				if (insert_ind != -1)
				{
					(*insert_elt_array)[insert_ctr].inner_blob.replace(max,len,tmp_ptr->get_buffer((CORBA::Boolean)true),true);
					(*insert_elt_array)[insert_ctr].inner_blob_name = CORBA::string_dup(datum.get_name().c_str());
					insert_ind = -1;
				}
				else
				{
					(*insert_elt_array)[insert_ctr].inner_blob.replace(max,len,tmp_ptr->get_buffer((CORBA::Boolean)true),true);
					(*insert_elt_array)[insert_ctr].inner_blob_name = CORBA::string_dup(datum.get_name().c_str());
					insert_ctr++;
				}

				delete tmp_ptr;
			}
		}
	}

	if (ext_state.any() == true)
		failed = true;

	if (ext_state.test(blobdenamenotset_flag) == true && exceptions_flags.test(blobdenamenotset_flag) == true)
		throw_name_not_set("operator<<");

	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true)
		throw_too_many("operator<<",false);

	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true)
		throw_mixing("operator>>");

	return *this;
}

//---------------------------------------------------------------------------------------------------------------

DevicePipeBlob & DevicePipeBlob::operator<<(vector<DevBoolean> &datum)
{
	failed = false;
	ext_state.reset();

	if (insert_elt_array == Tango_nullptr)
		ext_state.set(blobdenamenotset_flag);
	else if (insert_ctr == -1 && insert_ind == -1)
		ext_state.set(mixing_flag);
	else
	{
		size_t nb_insert = insert_elt_array->length();
		if (nb_insert == 0 || insert_ctr > (int)nb_insert - 1)
			ext_state.set(notenoughde_flag);
		else
		{
			DevVarBooleanArray dvsa;
			if (insert_ind != -1)
			{
				(*insert_elt_array)[insert_ind].value.bool_att_value(dvsa);
				DevVarBooleanArray &dvsb = (*insert_elt_array)[insert_ind].value.bool_att_value();
				dvsb << datum;
				insert_ind = -1;
			}
			else
			{
				(*insert_elt_array)[insert_ctr].value.bool_att_value(dvsa);
				DevVarBooleanArray &dvsb = (*insert_elt_array)[insert_ctr].value.bool_att_value();
				dvsb << datum;
				insert_ctr++;
			}
		}
	}

	if (ext_state.any() == true)
		failed = true;

	if (ext_state.test(blobdenamenotset_flag) == true && exceptions_flags.test(blobdenamenotset_flag) == true)
		throw_name_not_set("operator<<");

	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true)
		throw_mixing("operator>>");

	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true)
		throw_too_many("operator<<",false);

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<DevShort> &datum)
{
	INSERT_VECTOR_TYPE(DevVarShortArray,short_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<DevLong> &datum)
{
	INSERT_VECTOR_TYPE(DevVarLongArray,long_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<DevLong64> &datum)
{
	INSERT_VECTOR_TYPE(DevVarLong64Array,long64_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<float> &datum)
{
	INSERT_VECTOR_TYPE(DevVarFloatArray,float_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<double> &datum)
{
	INSERT_VECTOR_TYPE(DevVarDoubleArray,double_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<DevUChar> &datum)
{
	INSERT_VECTOR_TYPE(DevVarUCharArray,uchar_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<DevUShort> &datum)
{
	INSERT_VECTOR_TYPE(DevVarUShortArray,ushort_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<DevULong> &datum)
{
	INSERT_VECTOR_TYPE(DevVarULongArray,ulong_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<DevULong64> &datum)
{
	INSERT_VECTOR_TYPE(DevVarULong64Array,ulong64_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<DevString> &datum)
{
	INSERT_VECTOR_TYPE(DevVarStringArray,string_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<DevState> &datum)
{
	INSERT_VECTOR_TYPE(DevVarStateArray,state_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<DevEncoded> &datum)
{
	INSERT_VECTOR_TYPE(DevVarEncodedArray,encoded_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(vector<string> &datum)
{
	failed = false;
	ext_state.reset();

	if (insert_elt_array == Tango_nullptr)
		ext_state.set(blobdenamenotset_flag);
	else if (insert_ctr == -1 && insert_ind == -1)
		ext_state.set(mixing_flag);
	else
	{
		size_t nb_insert = insert_elt_array->length();
		if (nb_insert == 0 || insert_ctr > (int)nb_insert - 1)
			ext_state.set(notenoughde_flag);
		else
		{
			size_t nb = datum.size();
			char **strvec = DevVarStringArray::allocbuf(nb);
			for (size_t i = 0;i < nb;i++)
				strvec[i] = CORBA::string_dup(datum[i].c_str());
			DevVarStringArray dvsa;

			if (insert_ind != -1)
			{
				(*insert_elt_array)[insert_ind].value.string_att_value(dvsa);
				DevVarStringArray &dvsb = (*insert_elt_array)[insert_ind].value.string_att_value();
				dvsb.replace(datum.size(),datum.size(),strvec,true);
				insert_ind = -1;
			}
			else
			{
				(*insert_elt_array)[insert_ctr].value.string_att_value(dvsa);
				DevVarStringArray &dvsb = (*insert_elt_array)[insert_ctr].value.string_att_value();
				dvsb.replace(datum.size(),datum.size(),strvec,true);
				insert_ctr++;
			}
		}
	}

	if (ext_state.any() == true)
		failed = true;

	if (ext_state.test(blobdenamenotset_flag) == true && exceptions_flags.test(blobdenamenotset_flag) == true)
		throw_name_not_set("operator<<");

	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true)
		throw_too_many("operator<<",false);

	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true)
		throw_mixing("operator>>");

	return *this;
}

//---------------------------------------------------------------------------------------------------------------

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarBooleanArray &datum)
{
	INSERT_SEQ_TYPE(DevVarBooleanArray,bool_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarShortArray &datum)
{
	INSERT_SEQ_TYPE(DevVarShortArray,short_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarLongArray &datum)
{
	INSERT_SEQ_TYPE(DevVarLongArray,long_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarLong64Array &datum)
{
	INSERT_SEQ_TYPE(DevVarLong64Array,long64_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarFloatArray &datum)
{
	INSERT_SEQ_TYPE(DevVarFloatArray,float_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarDoubleArray &datum)
{
	INSERT_SEQ_TYPE(DevVarDoubleArray,double_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarUCharArray &datum)
{
	INSERT_SEQ_TYPE(DevVarUCharArray,uchar_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarUShortArray &datum)
{
	INSERT_SEQ_TYPE(DevVarUShortArray,ushort_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarULongArray &datum)
{
	INSERT_SEQ_TYPE(DevVarULongArray,ulong_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarULong64Array &datum)
{
	INSERT_SEQ_TYPE(DevVarULong64Array,ulong64_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarStringArray &datum)
{
	INSERT_SEQ_TYPE(DevVarStringArray,string_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarStateArray &datum)
{
	INSERT_SEQ_TYPE(DevVarStateArray,state_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarEncodedArray &datum)
{
	INSERT_SEQ_TYPE(DevVarEncodedArray,encoded_att_value)

	return *this;
}

//---------------------------------------------------------------------------------------------------------------

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarBooleanArray *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarBooleanArray,bool_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarShortArray *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarShortArray,short_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarLongArray *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarLongArray,long_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarLong64Array *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarLong64Array,long64_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarFloatArray *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarFloatArray,float_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarDoubleArray *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarDoubleArray,double_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarUCharArray *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarUCharArray,uchar_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarUShortArray *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarUShortArray,ushort_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarULongArray *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarULongArray,ulong_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarULong64Array *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarULong64Array,ulong64_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarStringArray *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarStringArray,string_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarStateArray *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarStateArray,state_att_value)

	return *this;
}

DevicePipeBlob & DevicePipeBlob::operator<<(DevVarEncodedArray *datum)
{
	INSERT_SEQ_PTR_TYPE(DevVarEncodedArray,encoded_att_value)

	return *this;
}

//******************************************************************************************************************

DevicePipeBlob &DevicePipeBlob::operator >> (DevBoolean &datum)
{
	EXTRACT_BASIC_TYPE(ATT_BOOL,bool_att_value,"DevBoolean")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (short &datum)
{
	EXTRACT_BASIC_TYPE(ATT_SHORT,short_att_value,"DevShort")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevLong &datum)
{
	EXTRACT_BASIC_TYPE(ATT_LONG,long_att_value,"DevLong")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevLong64 &datum)
{
	EXTRACT_BASIC_TYPE(ATT_LONG64,long64_att_value,"DevLong64")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (float &datum)
{
	EXTRACT_BASIC_TYPE(ATT_FLOAT,float_att_value,"DevFloat")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (double &datum)
{
	EXTRACT_BASIC_TYPE(ATT_DOUBLE,double_att_value,"DevDouble")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevUChar &datum)
{
	EXTRACT_BASIC_TYPE(ATT_UCHAR,uchar_att_value,"DevUChar")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevUShort &datum)
{
	EXTRACT_BASIC_TYPE(ATT_USHORT,ushort_att_value,"DevUShort")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevULong &datum)
{
	EXTRACT_BASIC_TYPE(ATT_ULONG,ulong_att_value,"DevULong")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevULong64 &datum)
{
	EXTRACT_BASIC_TYPE(ATT_ULONG64,ulong64_att_value,"DevULong64")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevString &datum)
{
	failed = false;
	ext_state.reset();

	if (extract_ctr > (int)extract_elt_array->length() - 1)
		ext_state.set(notenoughde_flag);
	else if (extract_ctr == -1 && extract_ind == -1)
		ext_state.set(mixing_flag);
	else
	{
		int ind;
		if (extract_ind != -1)
			ind = extract_ind;
		else
			ind = extract_ctr;
		const AttrValUnion *uni_ptr = &((*extract_elt_array)[ind].value);
		AttributeDataType adt = uni_ptr->_d();
		if (adt != ATT_STRING)
		{
			if (adt == ATT_NO_DATA)
			{
				if ((*extract_elt_array)[ind].inner_blob.length() == 0)
					ext_state.set(isempty_flag);
			}
			else
				ext_state.set(wrongtype_flag);
		}
		else
		{
			datum = CORBA::string_dup((uni_ptr->string_att_value())[0].in());
			if (extract_ind != -1)
				extract_ind = -1;
			else
				extract_ctr++;
		}
	}

	if (ext_state.any() == true)
		failed = true;

	if (ext_state.test(isempty_flag) == true && exceptions_flags.test(isempty_flag) == true)
		throw_is_empty("operator>>");

	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true)
		throw_too_many("operator>>",true);

	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true)
		throw_mixing("operator>>");

	if (ext_state.test(wrongtype_flag) == true && exceptions_flags.test(wrongtype_flag) == true)
		throw_type_except("DevString","operator>>");

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevState &datum)
{
	EXTRACT_BASIC_TYPE(ATT_STATE,state_att_value,"DevState")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevEncoded &datum)
{
	EXTRACT_BASIC_TYPE(ATT_ENCODED,encoded_att_value,"DevEncoded")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (string &datum)
{
	EXTRACT_BASIC_TYPE(ATT_STRING,string_att_value,"DevString")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevicePipeBlob &datum)
{
	failed = false;
	ext_state.reset();

	if (extract_ctr > (int)extract_elt_array->length() - 1)
		ext_state.set(notenoughde_flag);
	else if (extract_ctr == -1 && extract_ind == -1)
		ext_state.set(mixing_flag);
	else
	{
		int ind;
		if (extract_ind != -1)
			ind = extract_ind;
		else
			ind = extract_ctr;

		if ((*extract_elt_array)[ind].inner_blob.length() == 0)
		{
			if ((*extract_elt_array)[ind].value._d() == ATT_NO_DATA)
				ext_state.set(isempty_flag);
			else
				ext_state.set(wrongtype_flag);
		}
		else
		{
			datum.set_extract_data(&((*extract_elt_array)[ind].inner_blob));
			string tmp((*extract_elt_array)[ind].inner_blob_name.in());
			datum.set_name(tmp);
			datum.reset_extract_ctr();

			if (extract_ind != -1)
				extract_ind = -1;
			else
				extract_ctr++;
		}
	}

	if (ext_state.any() == true)
		failed = true;

	if (ext_state.test(isempty_flag) == true && exceptions_flags.test(isempty_flag) == true)
		throw_is_empty("operator>>");

	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true)
		throw_too_many("operator>>",true);

	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true)
		throw_mixing("operator>>");

	if (ext_state.test(wrongtype_flag) == true && exceptions_flags.test(wrongtype_flag) == true)
		throw_type_except("DevicePipeBlob","operator>>");

	return *this;
}

//----------------------------------------------------------------------------------------------------------

DevicePipeBlob &DevicePipeBlob::operator >> (vector<DevBoolean> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_BOOL,bool_att_value,DevVarBooleanArray,"DevBoolean")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<short> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_SHORT,short_att_value,DevVarShortArray,"DevShort")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<DevLong> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_LONG,long_att_value,DevVarLongArray,"DevLong")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<DevLong64> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_LONG64,long64_att_value,DevVarLong64Array,"DevLong64")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<float> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_FLOAT,float_att_value,DevVarFloatArray,"DevFloat")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<double> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_DOUBLE,double_att_value,DevVarDoubleArray,"DevDouble")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<DevUChar> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_UCHAR,uchar_att_value,DevVarUCharArray,"DevUChar")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<DevUShort> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_USHORT,ushort_att_value,DevVarUShortArray,"DevUShort")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<DevULong> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_ULONG,ulong_att_value,DevVarULongArray,"DevULong")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<DevULong64> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_ULONG64,ulong64_att_value,DevVarULong64Array,"DevULong64")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<string> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_STRING,string_att_value,DevVarStringArray,"DevString")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (vector<DevState> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_STATE,state_att_value,DevVarStateArray,"DevState")

	return *this;
}

/*DevicePipeBlob &DevicePipeBlob::operator >> (vector<DevEncoded> &datum)
{
	EXTRACT_VECTOR_TYPE(ATT_ENCODED,encoded_att_value,DevVarEncodedArray,"DevEncoded")

	return *this;
}*/

//----------------------------------------------------------------------------------------------------------

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarBooleanArray *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_BOOL,bool_att_value,DevVarBooleanArray,"DevBoolean")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarShortArray *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_SHORT,short_att_value,DevVarShortArray,"DevShort")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarLongArray *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_LONG,long_att_value,DevVarLongArray,"DevLong")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarLong64Array *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_LONG64,long64_att_value,DevVarLong64Array,"DevLong64")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarFloatArray *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_FLOAT,float_att_value,DevVarFloatArray,"DevFloat")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarDoubleArray *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_DOUBLE,double_att_value,DevVarDoubleArray,"DevDouble")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarUCharArray *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_UCHAR,uchar_att_value,DevVarUCharArray,"DevUChar")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarUShortArray *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_USHORT,ushort_att_value,DevVarUShortArray,"DevUShort")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarULongArray *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_ULONG,ulong_att_value,DevVarULongArray,"DevULong")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarULong64Array *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_ULONG64,ulong64_att_value,DevVarULong64Array,"DevULong64")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarStringArray *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_STRING,string_att_value,DevVarStringArray,"DevString")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarStateArray *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_STATE,state_att_value,DevVarStateArray,"DevState")

	return *this;
}

DevicePipeBlob &DevicePipeBlob::operator >> (DevVarEncodedArray *datum)
{
	EXTRACT_SEQ_PTR_TYPE(ATT_ENCODED,encoded_att_value,DevVarEncodedArray,"DevEncoded")

	return *this;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::has_failed
//
// description :
//		Throw exception when wrong data type is used
//
// return:
//		True if previous insertion/extraction has failed for one reason or another
//
//-------------------------------------------------------------------------------------------------------------------

bool DevicePipeBlob::has_failed()
{
	if (failed == true)
	{
		delete insert_elt_array;
		if (extract_delete == true)
		{
			delete extract_elt_array;
			extract_delete = false;
		}
	}
	return failed;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::throw_type_except
//
// description :
//		Throw exception when wrong data type is used
//
// argument:
//		in :
//			- _ty: data type name
//			- _meth : Caller method name
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::throw_type_except(const string &_ty,const string &_meth)
{
	delete insert_elt_array;
	if (extract_delete == true)
	{
		delete extract_elt_array;
		extract_delete = false;
	}

	stringstream ss;

	ss << "Can't get data element " << extract_ctr << " (numbering starting from 0) into a " << _ty << " data type";
	string m_name("DevicePipeBlob::");
	m_name = m_name + _meth;
	ApiDataExcept::throw_exception(API_IncompatibleArgumentType,ss.str(),m_name.c_str());
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::throw_too_many
//
// description :
//		Throw exception when user try to insert/extract too many data from the blob
//
// argument:
//		in :
//			- _meth : Caller method name
//			- _extract : Flag set to true if this method is called during extraction. Otherwise, false
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::throw_too_many(const string &_meth,bool _extract)
{
	stringstream ss;
	ss << "Not enough data element in blob (";
	if (_extract == true)
		ss << get_data_elt_nb();
	else
		ss << insert_elt_array->length();
	ss << " data elt in created/received blob)";
	string m_name("DevicePipeBlob::");
	m_name = m_name + _meth;

	delete insert_elt_array;
	insert_elt_array = Tango_nullptr;
	if (extract_delete == true)
	{
		delete extract_elt_array;
		extract_delete = false;
	}

	ApiDataExcept::throw_exception(API_PipeWrongArg,ss.str(),m_name.c_str());
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::throw_is_empty
//
// description :
//		Throw exception when user try to insert/extract data from one empty data element
//
// argument:
//		in :
//			- _meth : Caller method name
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::throw_is_empty(const string &_meth)
{
	delete insert_elt_array;
	if (extract_delete == true)
	{
		delete extract_elt_array;
		extract_delete = false;
	}

	string m_name("DevicePipeBlob::");
	m_name = m_name + _meth;
	ApiDataExcept::throw_exception(API_EmptyDataElement,"The data element is empty",m_name.c_str());
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::throw_name_not_set
//
// description :
//		Throw exception when user try to insert data into a device blob while the DE name/nb not set
//
// argument:
//		in :
//			- _meth : Caller method name
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::throw_name_not_set(const string &_meth)
{
	delete insert_elt_array;
	if (extract_delete == true)
	{
		delete extract_elt_array;
		extract_delete = false;
	}

	string m_name("DevicePipeBlob::");
	m_name = m_name + _meth;
	ApiDataExcept::throw_exception(API_PipeNoDataElement,"The blob data element number (or name) not set",m_name.c_str());
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevicePipeBlob::throw_mixing
//
// description :
//		Throw exception when user try to mix [] and >> (<<) insertion/extraction method
//
// argument:
//		in :
//			- _meth : Caller method name
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::throw_mixing(const string &_meth)
{
	delete insert_elt_array;
	if (extract_delete == true)
	{
		delete extract_elt_array;
		extract_delete = false;
	}

	string m_name("DevicePipeBlob::");
	m_name = m_name + _meth;
	Except::throw_exception(API_NotSupportedFeature,
								"Not supported to mix extraction type (operator >> (or <<) and operator [])",m_name.c_str());
}

//+------------------------------------------------------------------------------------------------------------------
//
// method
// 		DevicePipeBlob::print
//
// description :
//		Print blob content. Due to recusrsive type, it was not possible to write this feature as a classical
//		<< operator.
//
// argument:
//		in :
//			- o_str : The stream used for printing
//			- indent : Indentation level for printing in case of blob inside blob
//			- insert_extract: Flag set to true is insert data should be printed, else extract data are printed
//
//-------------------------------------------------------------------------------------------------------------------

void DevicePipeBlob::print(ostream &o_str,int indent,bool insert_extract)
{

//
// Print blob name (if any)
//

	if (name.size() != 0 && insert_extract == true)
	{
		int max = indent;
		if (indent > 1)
			max--;
		for (int loop = 0;loop < max;loop++)
			o_str << "\t";
		o_str << "Blob name = " << name << endl;
	}


//
// Data element name/value
//

	const DevVarPipeDataEltArray *dvpdea;
	if (insert_extract == false)
		dvpdea = get_extract_data();
	else
		dvpdea = get_insert_data();

	if (dvpdea != Tango_nullptr)
	{
		for (size_t ctr = 0;ctr < dvpdea->length();ctr++)
		{
			for (int loop = 0;loop < indent;loop++)
				o_str << "\t";
			o_str << "Data element name = " << (*dvpdea)[ctr].name.in() << endl;
			for (int loop = 0;loop < indent;loop++)
				o_str<< "\t";
			o_str << "Data element value:" << endl;

//
// Print indent level
//

			for (int loop = 0;loop < indent;loop++)
				o_str << "\t";

//
// Print data element value
//

			switch ((*dvpdea)[ctr].value._d())
			{
				case ATT_BOOL:
				{
					const DevVarBooleanArray &dvba = (*dvpdea)[ctr].value.bool_att_value();
					o_str << dvba;
				}
				break;

				case ATT_SHORT:
				{
					const DevVarShortArray &dvsa = (*dvpdea)[ctr].value.short_att_value();
					o_str << dvsa;
				}
				break;

				case ATT_LONG:
				{
					const DevVarLongArray &dvla = (*dvpdea)[ctr].value.long_att_value();
					o_str << dvla;
				}
				break;

				case ATT_LONG64:
				{
					const DevVarLong64Array &dvlo64 = (*dvpdea)[ctr].value.long64_att_value();
					o_str << dvlo64;
				}
				break;

				case ATT_FLOAT:
				{
					const DevVarFloatArray &dvfa = (*dvpdea)[ctr].value.float_att_value();
					o_str << dvfa;
				}
				break;

				case ATT_DOUBLE:
				{
					const DevVarDoubleArray &dvda = (*dvpdea)[ctr].value.double_att_value();
					o_str << dvda;
				}
				break;

				case ATT_UCHAR:
				{
					const DevVarCharArray &dvda = (*dvpdea)[ctr].value.uchar_att_value();
					o_str << dvda;
				}
				break;

				case ATT_USHORT:
				{
					const DevVarUShortArray &dvda = (*dvpdea)[ctr].value.ushort_att_value();
					o_str << dvda;
				}
				break;

				case ATT_ULONG:
				{
					const DevVarULongArray &dvda = (*dvpdea)[ctr].value.ulong_att_value();
					o_str << dvda;
				}
				break;

				case ATT_ULONG64:
				{
					const DevVarULong64Array &dvda = (*dvpdea)[ctr].value.ulong64_att_value();
					o_str << dvda;
				}
				break;

				case ATT_STRING:
				{
					const DevVarStringArray &dvda = (*dvpdea)[ctr].value.string_att_value();
					o_str << dvda;
				}
				break;

				case ATT_STATE:
				{
					const DevVarStateArray &dvda = (*dvpdea)[ctr].value.state_att_value();
					o_str << dvda;
				}
				break;

				case ATT_ENCODED:
				{
					const DevVarEncodedArray &dvda = (*dvpdea)[ctr].value.encoded_att_value();
					o_str << dvda;
				}
				break;

				case DEVICE_STATE:
				o_str << "Unsupported data type in data element (ATT_NO_DATA, DEVICE_STATE)!" << endl;
				break;

				default:
				break;
			}

//
// Print inner blob if any
//

			if ((*dvpdea)[ctr].inner_blob.length() != 0)
			{
				DevicePipeBlob dpb((*dvpdea)[ctr].inner_blob_name.in());
				dpb.set_extract_data(&(*dvpdea)[ctr].inner_blob);
				dpb.print(o_str,indent + 1,true);
				dpb.print(o_str,indent + 1,false);
			}

			if (ctr < dvpdea->length() - 1)
				o_str << endl;
		}
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// Function
// 		operator overloading : 	<<
//
// description :
//		Friend function to ease printing instance of the DevicePipe class
//
//-------------------------------------------------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,DevicePipe &dd)
{

//
// Pipe date
//

	if (dd.time.tv_sec != 0)
	{
		time_t tmp_val = dd.time.tv_sec;
		char tmp_date[128];
#ifdef _TG_WINDOWS_
		ctime_s(tmp_date,128,&tmp_val);
#else
		ctime_r(&tmp_val,tmp_date);
#endif
		tmp_date[strlen(tmp_date) - 1] = '\0';
		o_str << tmp_date;
		o_str << " (" << dd.time.tv_sec << "," << dd.time.tv_usec << " sec) : ";
	}

//
// Print pipe name
//

	o_str << dd.name << endl;

//
// Print blob
//

	DevicePipeBlob &dpb = dd.get_root_blob();
	if (dpb.get_insert_data() == Tango_nullptr && dpb.get_extract_data() == Tango_nullptr)
		o_str << "DevicePipe is empty";
	else
	{
		dpb.print(o_str,0,true);
		dpb.print(o_str,0,false);
	}

	return o_str;
}


DevicePipe &operator>>(DevicePipe &_dp,char *&datum)
{
	_dp.get_root_blob().operator>>(datum);
	return _dp;
}



} // End of Tango namepsace
