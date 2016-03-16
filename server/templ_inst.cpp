//+=================================================================================================================
//
// file :               templ_inst.cpp
//
// description :        C++ source code to instantiate template methods/class in order to have explicit instanciation
//						This makes user life easier in term of getting bug fixes (no re-compilation needed)
//
// project :            TANGO
//
// author(s) :          E.Taurel
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


//
// For template management we have several files:
// - A file with explicit template instanciation. This file (templ_inst.cpp)
// For class with templates:
// - A file with template methods/class definition (attribute.tpp for Attribute class)
// - A file with template methods/class specialization definition (attribute_spec.tpp for Attribute class)
//
// We did this in order to have explicit instanciation of templates method except for specialized templates
// for which we have instanciation following the inclusion model
// See C++ template book chapter 6
//

#include <tango.h>

#include <attribute.tpp>
#include <pollext.tpp>
#include <utils.tpp>
#include <w_attribute.tpp>
#include <devapi_pipe.tpp>
#include <pipe.tpp>
#include <w_pipe.tpp>

namespace Tango
{

//
// The DevEncoded and DevString data types are not there. DevString is handle by 2 (for const) classic methods
// forwarding the call to the same method using string data type as parameter. DevEncoded and string are
// managed as specialized templates
//

#define TEMPL_EXPL_METH_INST_CONST(CLASS,METH) \
template void CLASS::METH(const DevBoolean &); \
template void CLASS::METH(const DevUChar &); \
template void CLASS::METH(const DevShort &); \
template void CLASS::METH(const DevUShort &); \
template void CLASS::METH(const DevLong &); \
template void CLASS::METH(const DevULong &); \
template void CLASS::METH(const DevLong64 &); \
template void CLASS::METH(const DevULong64 &); \
template void CLASS::METH(const DevFloat &); \
template void CLASS::METH(const DevDouble &); \
template void CLASS::METH(const DevState &);


#define TEMPL_EXPL_METH_INST(CLASS,METH) \
template void CLASS::METH(DevBoolean &); \
template void CLASS::METH(DevUChar &); \
template void CLASS::METH(DevShort &); \
template void CLASS::METH(DevUShort &); \
template void CLASS::METH(DevLong &); \
template void CLASS::METH(DevULong &); \
template void CLASS::METH(DevLong64 &); \
template void CLASS::METH(DevULong64 &); \
template void CLASS::METH(DevFloat &); \
template void CLASS::METH(DevDouble &); \
template void CLASS::METH(DevState &); \
template void CLASS::METH(DevString &); \
template void CLASS::METH(DevEncoded &);

#define TEMPL_EXPL_CLASS_INST(CLASS) \
template class CLASS<DevBoolean>; \
template class CLASS<DevUChar>; \
template class CLASS<DevShort>; \
template class CLASS<DevUShort>; \
template class CLASS<DevLong>; \
template class CLASS<DevULong>; \
template class CLASS<DevLong64>; \
template class CLASS<DevULong64>; \
template class CLASS<DevFloat>; \
template class CLASS<DevDouble>; \
template class CLASS<DevState>;

//template class CLASS<DevString>;
//template class CLASS<DevEncoded>;

#define TEMPL_EXPL_CMD_CLASS_INST(CLASS) \
template class CLASS<DevBoolean>; \
template class CLASS<DevUChar>; \
template class CLASS<DevShort>; \
template class CLASS<DevUShort>; \
template class CLASS<DevLong>; \
template class CLASS<DevULong>; \
template class CLASS<DevLong64>; \
template class CLASS<DevULong64>; \
template class CLASS<DevFloat>; \
template class CLASS<DevDouble>; \
template class CLASS<DevState>; \
template class CLASS<DevString>; \
template class CLASS<DevEncoded>; \
template class CLASS<DevVarBooleanArray>; \
template class CLASS<DevVarCharArray>; \
template class CLASS<DevVarShortArray>; \
template class CLASS<DevVarUShortArray>; \
template class CLASS<DevVarLongArray>; \
template class CLASS<DevVarULongArray>; \
template class CLASS<DevVarLong64Array>; \
template class CLASS<DevVarULong64Array>; \
template class CLASS<DevVarFloatArray>; \
template class CLASS<DevVarDoubleArray>; \
template class CLASS<DevVarStringArray>; \
template class CLASS<DevVarLongStringArray>; \
template class CLASS<DevVarDoubleStringArray>;


RANGES_TYPE2CONST(Tango::DevDouble,DEV_DOUBLE)
RANGES_TYPE2CONST(Tango::DevFloat,DEV_FLOAT)
RANGES_TYPE2CONST(Tango::DevLong,DEV_LONG)
RANGES_TYPE2CONST(Tango::DevLong64,DEV_LONG64)
RANGES_TYPE2CONST(Tango::DevShort,DEV_SHORT)
RANGES_TYPE2CONST(Tango::DevUChar,DEV_UCHAR)
RANGES_TYPE2CONST(Tango::DevULong,DEV_ULONG)
RANGES_TYPE2CONST(Tango::DevULong64,DEV_ULONG64)
RANGES_TYPE2CONST(Tango::DevUShort,DEV_USHORT)
RANGES_TYPE2CONST(Tango::DevEncoded,DEV_ENCODED)

RANGES_TYPE2CONST(Tango::DevBoolean,DEV_BOOLEAN)
RANGES_TYPE2CONST(Tango::DevString,DEV_STRING)
RANGES_TYPE2CONST(Tango::DevState,DEV_STATE)

//+----------------------------------------------------------------------------
//
// Instanciate Attribute::XXX methods and associate template classes
//
//-----------------------------------------------------------------------------


TEMPL_EXPL_METH_INST(Attribute,get_min_alarm)
TEMPL_EXPL_METH_INST(Attribute,get_max_alarm)
TEMPL_EXPL_METH_INST(Attribute,get_min_warning)
TEMPL_EXPL_METH_INST(Attribute,get_max_warning)

TEMPL_EXPL_METH_INST_CONST(Attribute,set_min_alarm)
TEMPL_EXPL_METH_INST_CONST(Attribute,set_max_alarm)
TEMPL_EXPL_METH_INST_CONST(Attribute,set_min_warning)
TEMPL_EXPL_METH_INST_CONST(Attribute,set_max_warning)

TEMPL_EXPL_CLASS_INST(AttrProp)
TEMPL_EXPL_CLASS_INST(DoubleAttrProp)
TEMPL_EXPL_CLASS_INST(MultiAttrProp)

template void Attribute::get_properties(MultiAttrProp<DevBoolean> &);
template void Attribute::get_properties(MultiAttrProp<DevUChar> &);
template void Attribute::get_properties(MultiAttrProp<DevShort> &);
template void Attribute::get_properties(MultiAttrProp<DevUShort> &);
template void Attribute::get_properties(MultiAttrProp<DevLong> &);
template void Attribute::get_properties(MultiAttrProp<DevULong> &);
template void Attribute::get_properties(MultiAttrProp<DevLong64> &);
template void Attribute::get_properties(MultiAttrProp<DevULong64> &);
template void Attribute::get_properties(MultiAttrProp<DevFloat> &);
template void Attribute::get_properties(MultiAttrProp<DevDouble> &);
template void Attribute::get_properties(MultiAttrProp<DevState> &);
template void Attribute::get_properties(MultiAttrProp<DevEncoded> &);
template void Attribute::get_properties(MultiAttrProp<DevString> &);


template void Attribute::set_properties(MultiAttrProp<DevBoolean> &);
template void Attribute::set_properties(MultiAttrProp<DevUChar> &);
template void Attribute::set_properties(MultiAttrProp<DevShort> &);
template void Attribute::set_properties(MultiAttrProp<DevUShort> &);
template void Attribute::set_properties(MultiAttrProp<DevLong> &);
template void Attribute::set_properties(MultiAttrProp<DevULong> &);
template void Attribute::set_properties(MultiAttrProp<DevLong64> &);
template void Attribute::set_properties(MultiAttrProp<DevULong64> &);
template void Attribute::set_properties(MultiAttrProp<DevFloat> &);
template void Attribute::set_properties(MultiAttrProp<DevDouble> &);
template void Attribute::set_properties(MultiAttrProp<DevState> &);
template void Attribute::set_properties(MultiAttrProp<DevEncoded> &);
template void Attribute::set_properties(MultiAttrProp<DevString> &);

template void Attribute::check_hard_coded_properties(const AttributeConfig &);
template void Attribute::check_hard_coded_properties(const AttributeConfig_3 &);

template void Attribute::set_hard_coded_properties(const AttributeConfig &);
template void Attribute::set_hard_coded_properties(const AttributeConfig_3 &);

template void Attribute::set_upd_properties(const AttributeConfig &,string &,bool);
template void Attribute::set_upd_properties(const AttributeConfig_3 &,string &,bool);
template void Attribute::set_upd_properties(const AttributeConfig_5 &,string &,bool);

template void Attribute::Attribute_2_AttributeValue_base(AttributeValue_4 *,Tango::DeviceImpl *);
template void Attribute::Attribute_2_AttributeValue_base(AttributeValue_5 *,Tango::DeviceImpl *);

template void Attribute::AttrValUnion_fake_copy(const AttributeValue_5 *,AttributeValue_4 *);
template void Attribute::AttrValUnion_fake_copy(const AttributeValue_4 *,AttributeValue_5 *);

template void Attribute::AttrValUnion_2_Any(const AttributeValue_4 *,CORBA::Any &);
template void Attribute::AttrValUnion_2_Any(const AttributeValue_5 *,CORBA::Any &);

//+----------------------------------------------------------------------------
//
// Instanciate Util::fill_XXX_polling buffers methods and associate template classes
//
//-----------------------------------------------------------------------------

TEMPL_EXPL_CLASS_INST(AttrData)
TEMPL_EXPL_CLASS_INST(TimedAttrData)
TEMPL_EXPL_CLASS_INST(AttrHistoryStack)

template class AttrData<DevString>;
template class AttrData<DevEncoded>;

template class TimedAttrData<DevString>;
template class TimedAttrData<DevEncoded>;

template class AttrHistoryStack<DevString>;
template class AttrHistoryStack<DevEncoded>;


template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevBoolean> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevUChar> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevShort> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevUShort> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevLong> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevULong> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevLong64> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevULong64> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevFloat> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevDouble> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevState> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevString> &);
template void Util::fill_attr_polling_buffer(DeviceImpl *,string &,AttrHistoryStack<DevEncoded> &);


TEMPL_EXPL_CMD_CLASS_INST(TimedCmdData)
TEMPL_EXPL_CMD_CLASS_INST(CmdHistoryStack)

template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevShort> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevUShort> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevLong> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevULong> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevLong64> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevULong64> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevFloat> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevDouble> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevState> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevString> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevEncoded> &);

template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarBooleanArray> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarCharArray> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarShortArray> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarUShortArray> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarLongArray> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarULongArray> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarLong64Array> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarULong64Array> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarFloatArray> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarDoubleArray> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarStringArray> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarLongStringArray> &);
template void Util::fill_cmd_polling_buffer(DeviceImpl *,string &,CmdHistoryStack<DevVarDoubleStringArray> &);

//+----------------------------------------------------------------------------
//
// Instanciate WAttribute::XXX methods
//
//-----------------------------------------------------------------------------


TEMPL_EXPL_METH_INST(WAttribute,get_min_value)
TEMPL_EXPL_METH_INST(WAttribute,get_max_value)

TEMPL_EXPL_METH_INST_CONST(WAttribute,set_min_value)
TEMPL_EXPL_METH_INST_CONST(WAttribute,set_max_value)

template void WAttribute::check_min_max(const unsigned int,const DevVarLongArray &,const DevLong &,const DevLong &);
template void WAttribute::check_min_max(const unsigned int,const DevVarShortArray &,const short &,const short &);
template void WAttribute::check_min_max(const unsigned int,const DevVarLong64Array &,const DevLong64 &,const DevLong64 &);
template void WAttribute::check_min_max(const unsigned int,const DevVarUShortArray &,const unsigned short &,const unsigned short &);
template void WAttribute::check_min_max(const unsigned int,const DevVarCharArray &,const unsigned char &,const unsigned char &);
template void WAttribute::check_min_max(const unsigned int,const DevVarULongArray &,const DevULong &,const DevULong &);
template void WAttribute::check_min_max(const unsigned int,const DevVarULong64Array &,const DevULong64 &,const DevULong64 &);
template void WAttribute::check_min_max(const unsigned int,const DevVarStateArray &,const DevState &,const DevState &);

//+----------------------------------------------------------------------------
//
// Instanciate DevicePipe helper functions
//
//-----------------------------------------------------------------------------

template DevicePipe &operator<<(DevicePipe &,DevBoolean &);
template DevicePipe &operator<<(DevicePipe &,short &);
template DevicePipe &operator<<(DevicePipe &,DevLong &);
template DevicePipe &operator<<(DevicePipe &,DevLong64 &);
template DevicePipe &operator<<(DevicePipe &,float &);
template DevicePipe &operator<<(DevicePipe &,double &);
template DevicePipe &operator<<(DevicePipe &,DevUChar &);
template DevicePipe &operator<<(DevicePipe &,DevUShort &);
template DevicePipe &operator<<(DevicePipe &,DevULong &);
template DevicePipe &operator<<(DevicePipe &,DevULong64 &);
template DevicePipe &operator<<(DevicePipe &,DevString &);
template DevicePipe &operator<<(DevicePipe &,DevState &);
template DevicePipe &operator<<(DevicePipe &,DevEncoded &);
template DevicePipe &operator<<(DevicePipe &,string &);

template DevicePipe &operator<<(DevicePipe &,DevicePipeBlob &);

template DevicePipe &operator<<(DevicePipe &,DataElement<DevBoolean> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<short> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevLong> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevLong64> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<float> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<double> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevUChar> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevUShort> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevULong> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevULong64> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevString> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevState> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevEncoded> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<const string> &);

template DevicePipe &operator<<(DevicePipe &,DataElement<DevicePipeBlob> &);


template DevicePipe &operator<<(DevicePipe &,vector<DevBoolean> &);
template DevicePipe &operator<<(DevicePipe &,vector<short> &);
template DevicePipe &operator<<(DevicePipe &,vector<DevLong> &);
template DevicePipe &operator<<(DevicePipe &,vector<DevLong64> &);
template DevicePipe &operator<<(DevicePipe &,vector<float> &);
template DevicePipe &operator<<(DevicePipe &,vector<double> &);
template DevicePipe &operator<<(DevicePipe &,vector<DevUChar> &);
template DevicePipe &operator<<(DevicePipe &,vector<DevUShort> &);
template DevicePipe &operator<<(DevicePipe &,vector<DevULong> &);
template DevicePipe &operator<<(DevicePipe &,vector<DevULong64> &);
template DevicePipe &operator<<(DevicePipe &,vector<DevString> &);
template DevicePipe &operator<<(DevicePipe &,vector<DevState> &);
template DevicePipe &operator<<(DevicePipe &,vector<string> &);

template DevicePipe &operator<<(DevicePipe &,DataElement<vector<DevBoolean> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<short> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<DevLong> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<DevLong64> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<float> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<double> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<DevUChar> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<DevUShort> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<DevULong> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<DevULong64> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<DevString> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<DevState> > &);
template DevicePipe &operator<<(DevicePipe &,DataElement<vector<string> > &);

template DevicePipe &operator<<(DevicePipe &,DevVarBooleanArray &);
template DevicePipe &operator<<(DevicePipe &,DevVarShortArray &);
template DevicePipe &operator<<(DevicePipe &,DevVarLongArray &);
template DevicePipe &operator<<(DevicePipe &,DevVarLong64Array &);
template DevicePipe &operator<<(DevicePipe &,DevVarFloatArray &);
template DevicePipe &operator<<(DevicePipe &,DevVarDoubleArray &);
template DevicePipe &operator<<(DevicePipe &,DevVarUCharArray &);
template DevicePipe &operator<<(DevicePipe &,DevVarUShortArray &);
template DevicePipe &operator<<(DevicePipe &,DevVarULongArray &);
template DevicePipe &operator<<(DevicePipe &,DevVarULong64Array &);
template DevicePipe &operator<<(DevicePipe &,DevVarStringArray &);
template DevicePipe &operator<<(DevicePipe &,DevVarStateArray &);

template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarBooleanArray &> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarShortArray &> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarLongArray &> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarLong64Array &> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarFloatArray &> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarDoubleArray &> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarUCharArray &> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarUShortArray &> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarULongArray &> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarULong64Array &> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarStringArray &> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarStateArray &> &);

template DevicePipe &operator<<(DevicePipe &,DevVarBooleanArray *);
template DevicePipe &operator<<(DevicePipe &,DevVarShortArray *);
template DevicePipe &operator<<(DevicePipe &,DevVarLongArray *);
template DevicePipe &operator<<(DevicePipe &,DevVarLong64Array *);
template DevicePipe &operator<<(DevicePipe &,DevVarFloatArray *);
template DevicePipe &operator<<(DevicePipe &,DevVarDoubleArray *);
template DevicePipe &operator<<(DevicePipe &,DevVarUCharArray *);
template DevicePipe &operator<<(DevicePipe &,DevVarUShortArray *);
template DevicePipe &operator<<(DevicePipe &,DevVarULongArray *);
template DevicePipe &operator<<(DevicePipe &,DevVarULong64Array *);
template DevicePipe &operator<<(DevicePipe &,DevVarStringArray *);
template DevicePipe &operator<<(DevicePipe &,DevVarStateArray *);

template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarBooleanArray *> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarShortArray *> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarLongArray *> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarLong64Array *> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarFloatArray *> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarDoubleArray *> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarUCharArray *> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarUShortArray *> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarULongArray *> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarULong64Array *> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarStringArray *> &);
template DevicePipe &operator<<(DevicePipe &,DataElement<DevVarStateArray *> &);

//-----------------------------------------------------------------------------------------------------

//
// Note that there is no instanciation for type DevString. For this type, a  non template method has been written
// Otherwise, due the partial specialization for pointer types, it was not possible to use extracttion into
// DevString type
//

template DevicePipe &operator>>(DevicePipe &,DevBoolean &);
template DevicePipe &operator>>(DevicePipe &,short &);
template DevicePipe &operator>>(DevicePipe &,DevLong &);
template DevicePipe &operator>>(DevicePipe &,DevLong64 &);
template DevicePipe &operator>>(DevicePipe &,float &);
template DevicePipe &operator>>(DevicePipe &,double &);
template DevicePipe &operator>>(DevicePipe &,DevUChar &);
template DevicePipe &operator>>(DevicePipe &,DevUShort &);
template DevicePipe &operator>>(DevicePipe &,DevULong &);
template DevicePipe &operator>>(DevicePipe &,DevULong64 &);
// Missing DevString (see above)
template DevicePipe &operator>>(DevicePipe &,DevState &);
template DevicePipe &operator>>(DevicePipe &,DevEncoded &);
template DevicePipe &operator>>(DevicePipe &,string &);

template DevicePipe &operator>>(DevicePipe &,DevicePipeBlob &);

template DevicePipe &operator>>(DevicePipe &,DataElement<DevBoolean> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<short> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevLong> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevLong64> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<float> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<double> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevUChar> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevUShort> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevULong> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevULong64> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevString> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevState> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevEncoded> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<string> &);

template DevicePipe &operator>>(DevicePipe &,DataElement<DevicePipeBlob> &);

template DevicePipe &operator>>(DevicePipe &,vector<DevBoolean> &);
template DevicePipe &operator>>(DevicePipe &,vector<short> &);
template DevicePipe &operator>>(DevicePipe &,vector<DevLong> &);
template DevicePipe &operator>>(DevicePipe &,vector<DevLong64> &);
template DevicePipe &operator>>(DevicePipe &,vector<float> &);
template DevicePipe &operator>>(DevicePipe &,vector<double> &);
template DevicePipe &operator>>(DevicePipe &,vector<DevUChar> &);
template DevicePipe &operator>>(DevicePipe &,vector<DevUShort> &);
template DevicePipe &operator>>(DevicePipe &,vector<DevULong> &);
template DevicePipe &operator>>(DevicePipe &,vector<DevULong64> &);
template DevicePipe &operator>>(DevicePipe &,vector<string> &);
template DevicePipe &operator>>(DevicePipe &,vector<DevState> &);
//template DevicePipe &operator>>(DevicePipe &,vector<DevEncoded> &);

template DevicePipe &operator>>(DevicePipe &,DataElement<vector<DevBoolean> > &);
template DevicePipe &operator>>(DevicePipe &,DataElement<vector<short> > &);
template DevicePipe &operator>>(DevicePipe &,DataElement<vector<DevLong> > &);
template DevicePipe &operator>>(DevicePipe &,DataElement<vector<DevLong64> > &);
template DevicePipe &operator>>(DevicePipe &,DataElement<vector<float> > &);
template DevicePipe &operator>>(DevicePipe &,DataElement<vector<double> > &);
template DevicePipe &operator>>(DevicePipe &,DataElement<vector<DevUChar> > &);
template DevicePipe &operator>>(DevicePipe &,DataElement<vector<DevUShort> > &);
template DevicePipe &operator>>(DevicePipe &,DataElement<vector<DevULong> > &);
template DevicePipe &operator>>(DevicePipe &,DataElement<vector<DevULong64> > &);
template DevicePipe &operator>>(DevicePipe &,DataElement<vector<string> > &);
template DevicePipe &operator>>(DevicePipe &,DataElement<vector<DevState> > &);
//template DevicePipe &operator>>(DevicePipe &,DataElement<vector<DevEncoded> > &);

template DevicePipe &operator>>(DevicePipe &,DevVarBooleanArray *);
template DevicePipe &operator>>(DevicePipe &,DevVarShortArray *);
template DevicePipe &operator>>(DevicePipe &,DevVarLongArray *);
template DevicePipe &operator>>(DevicePipe &,DevVarLong64Array *);
template DevicePipe &operator>>(DevicePipe &,DevVarFloatArray *);
template DevicePipe &operator>>(DevicePipe &,DevVarDoubleArray *);
template DevicePipe &operator>>(DevicePipe &,DevVarUCharArray *);
template DevicePipe &operator>>(DevicePipe &,DevVarUShortArray *);
template DevicePipe &operator>>(DevicePipe &,DevVarULongArray *);
template DevicePipe &operator>>(DevicePipe &,DevVarULong64Array *);
template DevicePipe &operator>>(DevicePipe &,DevVarStringArray *);
template DevicePipe &operator>>(DevicePipe &,DevVarStateArray *);
//template DevicePipe &operator>>(DevicePipe &,DevVarEncodedArray *);

template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarBooleanArray *> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarShortArray *> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarLongArray *> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarLong64Array *> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarFloatArray *> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarDoubleArray *> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarUCharArray *> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarUShortArray *> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarULongArray *> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarULong64Array *> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarStringArray *> &);
template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarStateArray *> &);
//template DevicePipe &operator>>(DevicePipe &,DataElement<DevVarEncodedArray *> &);

//+----------------------------------------------------------------------------
//
// Instanciate DevicePipeBlob helper functions
//
//-----------------------------------------------------------------------------

template DevicePipeBlob &operator<<(DevicePipeBlob &,DevBoolean &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,short &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevLong &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevLong64 &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,float &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,double &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevUChar &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevUShort &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevULong &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevULong64 &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevString &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevState &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevEncoded &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,const string &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,string &);

template DevicePipeBlob &operator<<(DevicePipeBlob &,DevicePipeBlob &);

template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevBoolean> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<short> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevLong> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevLong64> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<float> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<double> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevUChar> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevUShort> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevULong> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevULong64> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevString> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevState> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevEncoded> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<const string> &);

template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevicePipeBlob> &);

template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<DevBoolean> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<short> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<DevLong> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<DevLong64> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<float> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<double> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<DevUChar> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<DevUShort> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<DevULong> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<DevULong64> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<DevString> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<DevState> &);
//template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<DevEncoded> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,vector<string> &);


template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<DevBoolean> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<short> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<DevLong> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<DevLong64> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<float> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<double> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<DevUChar> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<DevUShort> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<DevULong> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<DevULong64> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<DevString> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<DevState> > &);
//template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<DevEncoded> > &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<vector<string> > &);

template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarBooleanArray &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarShortArray &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarLongArray &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarLong64Array &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarFloatArray &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarDoubleArray &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarUCharArray &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarUShortArray &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarULongArray &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarULong64Array &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarStringArray &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarStateArray &);
//template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarEncodedArray &);

template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarBooleanArray &> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarShortArray &> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarLongArray &> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarLong64Array &> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarFloatArray &> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarDoubleArray &> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarUCharArray &> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarUShortArray &> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarULongArray &> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarULong64Array &> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarStringArray &> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarStateArray &> &);
//template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarEncodedArray &> &);

template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarBooleanArray *);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarShortArray *);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarLongArray *);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarLong64Array *);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarFloatArray *);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarDoubleArray *);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarUCharArray *);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarUShortArray *);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarULongArray *);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarULong64Array *);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarStringArray *);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarStateArray *);
//template DevicePipeBlob &operator<<(DevicePipeBlob &,DevVarEncodedArray *);

template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarBooleanArray *> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarShortArray *> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarLongArray *> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarLong64Array *> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarFloatArray *> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarDoubleArray *> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarUCharArray *> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarUShortArray *> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarULongArray *> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarULong64Array *> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarStringArray *> &);
template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarStateArray *> &);
//template DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<DevVarEncodedArray *> &);

//-------------------------------------------------------------------------------------------

template DevicePipeBlob &operator>>(DevicePipeBlob &,DevBoolean &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,short &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevLong &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevLong64 &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,float &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,double &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevUChar &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevUShort &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevULong &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevULong64 &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevString &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevState &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevEncoded &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,string &);

template DevicePipeBlob &operator>>(DevicePipeBlob &,DevicePipeBlob &);

template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevBoolean> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<short> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevLong> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevLong64> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<float> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<double> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevUChar> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevUShort> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevULong> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevULong64> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevString> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevState> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevEncoded> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<string> &);

template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevicePipeBlob> &);

template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<DevBoolean> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<short> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<DevLong> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<DevLong64> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<float> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<double> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<DevUChar> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<DevUShort> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<DevULong> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<DevULong64> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<string> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<DevState> &);
//template DevicePipeBlob &operator>>(DevicePipeBlob &,vector<DevEncoded> &);

template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<DevBoolean> > &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<short> > &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<DevLong> > &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<DevLong64> > &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<float> > &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<double> > &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<DevUChar> > &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<DevUShort> > &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<DevULong> > &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<DevULong64> > &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<string> > &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<DevState> > &);
//template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<vector<DevEncoded> > &);

template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarBooleanArray *);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarShortArray *);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarLongArray *);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarLong64Array *);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarFloatArray *);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarDoubleArray *);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarUCharArray *);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarUShortArray *);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarULongArray *);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarULong64Array *);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarStringArray *);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarStateArray *);
//template DevicePipeBlob &operator>>(DevicePipeBlob &,DevVarEncodedArray *);

template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarBooleanArray *> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarShortArray *> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarLongArray *> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarLong64Array *> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarFloatArray *> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarDoubleArray *> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarUCharArray *> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarUShortArray *> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarULongArray *> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarULong64Array *> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarStringArray *> &);
template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarStateArray *> &);
//template DevicePipeBlob &operator>>(DevicePipeBlob &,DataElement<DevVarEncodedArray *> &);

//+----------------------------------------------------------------------------
//
// Instanciate WPipe helper functions
//
//-----------------------------------------------------------------------------

template WPipe &operator>>(WPipe &,DevBoolean &);
template WPipe &operator>>(WPipe &,short &);
template WPipe &operator>>(WPipe &,DevLong &);
template WPipe &operator>>(WPipe &,DevLong64 &);
template WPipe &operator>>(WPipe &,float &);
template WPipe &operator>>(WPipe &,double &);
template WPipe &operator>>(WPipe &,DevUChar &);
template WPipe &operator>>(WPipe &,DevUShort &);
template WPipe &operator>>(WPipe &,DevULong &);
template WPipe &operator>>(WPipe &,DevULong64 &);
template WPipe &operator>>(WPipe &,DevString &);
template WPipe &operator>>(WPipe &,DevState &);
template WPipe &operator>>(WPipe &,DevEncoded &);
template WPipe &operator>>(WPipe &,string &);

template WPipe &operator>>(WPipe &,DevicePipeBlob &);

template WPipe &operator>>(WPipe &,DataElement<DevBoolean> &);
template WPipe &operator>>(WPipe &,DataElement<short> &);
template WPipe &operator>>(WPipe &,DataElement<DevLong> &);
template WPipe &operator>>(WPipe &,DataElement<DevLong64> &);
template WPipe &operator>>(WPipe &,DataElement<float> &);
template WPipe &operator>>(WPipe &,DataElement<double> &);
template WPipe &operator>>(WPipe &,DataElement<DevUChar> &);
template WPipe &operator>>(WPipe &,DataElement<DevUShort> &);
template WPipe &operator>>(WPipe &,DataElement<DevULong> &);
template WPipe &operator>>(WPipe &,DataElement<DevULong64> &);
template WPipe &operator>>(WPipe &,DataElement<DevString> &);
template WPipe &operator>>(WPipe &,DataElement<DevState> &);
template WPipe &operator>>(WPipe &,DataElement<DevEncoded> &);
template WPipe &operator>>(WPipe &,DataElement<string> &);

template WPipe &operator>>(WPipe &,DataElement<DevicePipeBlob> &);

template WPipe &operator>>(WPipe &,vector<DevBoolean> &);
template WPipe &operator>>(WPipe &,vector<short> &);
template WPipe &operator>>(WPipe &,vector<DevLong> &);
template WPipe &operator>>(WPipe &,vector<DevLong64> &);
template WPipe &operator>>(WPipe &,vector<float> &);
template WPipe &operator>>(WPipe &,vector<double> &);
template WPipe &operator>>(WPipe &,vector<DevUChar> &);
template WPipe &operator>>(WPipe &,vector<DevUShort> &);
template WPipe &operator>>(WPipe &,vector<DevULong> &);
template WPipe &operator>>(WPipe &,vector<DevULong64> &);
template WPipe &operator>>(WPipe &,vector<string> &);
template WPipe &operator>>(WPipe &,vector<DevState> &);
//template WPipe &operator>>(WPipe &,vector<DevEncoded> &);

template WPipe &operator>>(WPipe &,DataElement<vector<DevBoolean> > &);
template WPipe &operator>>(WPipe &,DataElement<vector<short> > &);
template WPipe &operator>>(WPipe &,DataElement<vector<DevLong> > &);
template WPipe &operator>>(WPipe &,DataElement<vector<DevLong64> > &);
template WPipe &operator>>(WPipe &,DataElement<vector<float> > &);
template WPipe &operator>>(WPipe &,DataElement<vector<double> > &);
template WPipe &operator>>(WPipe &,DataElement<vector<DevUChar> > &);
template WPipe &operator>>(WPipe &,DataElement<vector<DevUShort> > &);
template WPipe &operator>>(WPipe &,DataElement<vector<DevULong> > &);
template WPipe &operator>>(WPipe &,DataElement<vector<DevULong64> > &);
template WPipe &operator>>(WPipe &,DataElement<vector<string> > &);
template WPipe &operator>>(WPipe &,DataElement<vector<DevState> > &);
//template WPipe &operator>>(WPipe &,DataElement<vector<DevEncoded> > &);

template WPipe &operator>>(WPipe &,DevVarBooleanArray *);
template WPipe &operator>>(WPipe &,DevVarShortArray *);
template WPipe &operator>>(WPipe &,DevVarLongArray *);
template WPipe &operator>>(WPipe &,DevVarLong64Array *);
template WPipe &operator>>(WPipe &,DevVarFloatArray *);
template WPipe &operator>>(WPipe &,DevVarDoubleArray *);
template WPipe &operator>>(WPipe &,DevVarUCharArray *);
template WPipe &operator>>(WPipe &,DevVarUShortArray *);
template WPipe &operator>>(WPipe &,DevVarULongArray *);
template WPipe &operator>>(WPipe &,DevVarULong64Array *);
template WPipe &operator>>(WPipe &,DevVarStringArray *);
template WPipe &operator>>(WPipe &,DevVarStateArray *);
//template WPipe &operator>>(WPipe &,DevVarEncodedArray *);

template WPipe &operator>>(WPipe &,DataElement<DevVarBooleanArray *> &);
template WPipe &operator>>(WPipe &,DataElement<DevVarShortArray *> &);
template WPipe &operator>>(WPipe &,DataElement<DevVarLongArray *> &);
template WPipe &operator>>(WPipe &,DataElement<DevVarLong64Array *> &);
template WPipe &operator>>(WPipe &,DataElement<DevVarFloatArray *> &);
template WPipe &operator>>(WPipe &,DataElement<DevVarDoubleArray *> &);
template WPipe &operator>>(WPipe &,DataElement<DevVarUCharArray *> &);
template WPipe &operator>>(WPipe &,DataElement<DevVarUShortArray *> &);
template WPipe &operator>>(WPipe &,DataElement<DevVarULongArray *> &);
template WPipe &operator>>(WPipe &,DataElement<DevVarULong64Array *> &);
template WPipe &operator>>(WPipe &,DataElement<DevVarStringArray *> &);
template WPipe &operator>>(WPipe &,DataElement<DevVarStateArray *> &);
//template WPipe &operator>>(WPipe &,DataElement<DevVarEncodedArray *> &);

//+----------------------------------------------------------------------------
//
// Instanciate Pipe helper functions
//
//-----------------------------------------------------------------------------


template Pipe &operator<<(Pipe &,DevBoolean &);
template Pipe &operator<<(Pipe &,short &);
template Pipe &operator<<(Pipe &,DevLong &);
template Pipe &operator<<(Pipe &,DevLong64 &);
template Pipe &operator<<(Pipe &,float &);
template Pipe &operator<<(Pipe &,double &);
template Pipe &operator<<(Pipe &,DevUChar &);
template Pipe &operator<<(Pipe &,DevUShort &);
template Pipe &operator<<(Pipe &,DevULong &);
template Pipe &operator<<(Pipe &,DevULong64 &);
template Pipe &operator<<(Pipe &,DevString &);
template Pipe &operator<<(Pipe &,DevState &);
template Pipe &operator<<(Pipe &,DevEncoded &);
template Pipe &operator<<(Pipe &,const string &);

template Pipe &operator<<(Pipe &,DevicePipeBlob &);

template Pipe &operator<<(Pipe &,DataElement<DevBoolean> &);
template Pipe &operator<<(Pipe &,DataElement<short> &);
template Pipe &operator<<(Pipe &,DataElement<DevLong> &);
template Pipe &operator<<(Pipe &,DataElement<DevLong64> &);
template Pipe &operator<<(Pipe &,DataElement<float> &);
template Pipe &operator<<(Pipe &,DataElement<double> &);
template Pipe &operator<<(Pipe &,DataElement<DevUChar> &);
template Pipe &operator<<(Pipe &,DataElement<DevUShort> &);
template Pipe &operator<<(Pipe &,DataElement<DevULong> &);
template Pipe &operator<<(Pipe &,DataElement<DevULong64> &);
template Pipe &operator<<(Pipe &,DataElement<DevString> &);
template Pipe &operator<<(Pipe &,DataElement<DevState> &);
template Pipe &operator<<(Pipe &,DataElement<DevEncoded> &);
template Pipe &operator<<(Pipe &,DataElement<const string> &);

template Pipe &operator<<(Pipe &,DataElement<DevicePipeBlob> &);


template Pipe &operator<<(Pipe &,vector<DevBoolean> &);
template Pipe &operator<<(Pipe &,vector<short> &);
template Pipe &operator<<(Pipe &,vector<DevLong> &);
template Pipe &operator<<(Pipe &,vector<DevLong64> &);
template Pipe &operator<<(Pipe &,vector<float> &);
template Pipe &operator<<(Pipe &,vector<double> &);
template Pipe &operator<<(Pipe &,vector<DevUChar> &);
template Pipe &operator<<(Pipe &,vector<DevUShort> &);
template Pipe &operator<<(Pipe &,vector<DevULong> &);
template Pipe &operator<<(Pipe &,vector<DevULong64> &);
template Pipe &operator<<(Pipe &,vector<DevString> &);
template Pipe &operator<<(Pipe &,vector<DevState> &);
//template Pipe &operator<<(Pipe &,vector<DevEncoded> &);
template Pipe &operator<<(Pipe &,vector<string> &);

template Pipe &operator<<(Pipe &,DataElement<vector<DevBoolean> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<short> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<DevLong> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<DevLong64> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<float> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<double> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<DevUChar> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<DevUShort> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<DevULong> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<DevULong64> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<DevString> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<DevState> > &);
//template Pipe &operator<<(Pipe &,DataElement<vector<DevEncoded> > &);
template Pipe &operator<<(Pipe &,DataElement<vector<string> > &);

template Pipe &operator<<(Pipe &,DevVarBooleanArray &);
template Pipe &operator<<(Pipe &,DevVarShortArray &);
template Pipe &operator<<(Pipe &,DevVarLongArray &);
template Pipe &operator<<(Pipe &,DevVarLong64Array &);
template Pipe &operator<<(Pipe &,DevVarFloatArray &);
template Pipe &operator<<(Pipe &,DevVarDoubleArray &);
template Pipe &operator<<(Pipe &,DevVarUCharArray &);
template Pipe &operator<<(Pipe &,DevVarUShortArray &);
template Pipe &operator<<(Pipe &,DevVarULongArray &);
template Pipe &operator<<(Pipe &,DevVarULong64Array &);
template Pipe &operator<<(Pipe &,DevVarStringArray &);
template Pipe &operator<<(Pipe &,DevVarStateArray &);
//template Pipe &operator<<(Pipe &,DevVarEncodedArray &);

template Pipe &operator<<(Pipe &,DataElement<DevVarBooleanArray &> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarShortArray &> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarLongArray &> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarLong64Array &> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarFloatArray &> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarDoubleArray &> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarUCharArray &> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarUShortArray &> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarULongArray &> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarULong64Array &> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarStringArray &> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarStateArray &> &);
//template Pipe &operator<<(Pipe &,DataElement<DevVarEncodedArray &> &);

template Pipe &operator<<(Pipe &,DevVarBooleanArray *);
template Pipe &operator<<(Pipe &,DevVarShortArray *);
template Pipe &operator<<(Pipe &,DevVarLongArray *);
template Pipe &operator<<(Pipe &,DevVarLong64Array *);
template Pipe &operator<<(Pipe &,DevVarFloatArray *);
template Pipe &operator<<(Pipe &,DevVarDoubleArray *);
template Pipe &operator<<(Pipe &,DevVarUCharArray *);
template Pipe &operator<<(Pipe &,DevVarUShortArray *);
template Pipe &operator<<(Pipe &,DevVarULongArray *);
template Pipe &operator<<(Pipe &,DevVarULong64Array *);
template Pipe &operator<<(Pipe &,DevVarStringArray *);
template Pipe &operator<<(Pipe &,DevVarStateArray *);
//template Pipe &operator<<(Pipe &,DevVarEncodedArray *);

template Pipe &operator<<(Pipe &,DataElement<DevVarBooleanArray *> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarShortArray *> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarLongArray *> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarLong64Array *> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarFloatArray *> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarDoubleArray *> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarUCharArray *> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarUShortArray *> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarULongArray *> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarULong64Array *> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarStringArray *> &);
template Pipe &operator<<(Pipe &,DataElement<DevVarStateArray *> &);
//template Pipe &operator<<(Pipe &,DataElement<DevVarEncodedArray *> &);

//+----------------------------------------------------------------------------
//
// Instanciate DataElement helper functions
//
//-----------------------------------------------------------------------------


template ostream &operator<<(ostream &,DataElement<DevBoolean> &);
template ostream &operator<<(ostream &,DataElement<short> &);
template ostream &operator<<(ostream &,DataElement<DevLong> &);
template ostream &operator<<(ostream &,DataElement<DevLong64> &);
template ostream &operator<<(ostream &,DataElement<float> &);
template ostream &operator<<(ostream &,DataElement<double> &);
template ostream &operator<<(ostream &,DataElement<DevUChar> &);
template ostream &operator<<(ostream &,DataElement<DevUShort> &);
template ostream &operator<<(ostream &,DataElement<DevULong> &);
template ostream &operator<<(ostream &,DataElement<DevULong64> &);
template ostream &operator<<(ostream &,DataElement<DevState> &);
//template ostream &operator<<(ostream &,DataElement<DevEncoded> &);
template ostream &operator<<(ostream &,DataElement<const string> &);

#ifndef _TG_WINDOWS_
template ostream &operator<<(ostream &,DataElement<DevString> &);
template ostream &operator<<(ostream &,DataElement<DevicePipeBlob> &);
#endif

template ostream &operator<<(ostream &,DataElement<vector<DevBoolean> > &);
template ostream &operator<<(ostream &,DataElement<vector<short> > &);
template ostream &operator<<(ostream &,DataElement<vector<DevLong> > &);
template ostream &operator<<(ostream &,DataElement<vector<DevLong64> > &);
template ostream &operator<<(ostream &,DataElement<vector<float> > &);
template ostream &operator<<(ostream &,DataElement<vector<double> > &);
template ostream &operator<<(ostream &,DataElement<vector<DevUChar> > &);
template ostream &operator<<(ostream &,DataElement<vector<DevUShort> > &);
template ostream &operator<<(ostream &,DataElement<vector<DevULong> > &);
template ostream &operator<<(ostream &,DataElement<vector<DevULong64> > &);
template ostream &operator<<(ostream &,DataElement<vector<DevString> > &);
template ostream &operator<<(ostream &,DataElement<vector<DevState> > &);
//template ostream &operator<<(ostream &,DataElement<vector<DevEncoded> > &);
template ostream &operator<<(ostream &,DataElement<vector<string> > &);

template ostream &operator<<(ostream &,DataElement<DevVarBooleanArray *> &);
template ostream &operator<<(ostream &,DataElement<DevVarShortArray *> &);
template ostream &operator<<(ostream &,DataElement<DevVarLongArray *> &);
template ostream &operator<<(ostream &,DataElement<DevVarLong64Array *> &);
template ostream &operator<<(ostream &,DataElement<DevVarFloatArray *> &);
template ostream &operator<<(ostream &,DataElement<DevVarDoubleArray *> &);
template ostream &operator<<(ostream &,DataElement<DevVarUCharArray *> &);
template ostream &operator<<(ostream &,DataElement<DevVarUShortArray *> &);
template ostream &operator<<(ostream &,DataElement<DevVarULongArray *> &);
template ostream &operator<<(ostream &,DataElement<DevVarULong64Array *> &);
template ostream &operator<<(ostream &,DataElement<DevVarStringArray *> &);
template ostream &operator<<(ostream &,DataElement<DevVarStateArray *> &);
//template ostream &operator<<(ostream &,DataElement<DevVarEncodedArray *> &);


} // End of Tango namespace

