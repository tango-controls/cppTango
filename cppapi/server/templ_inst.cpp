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

template void Attribute::check_hard_coded_properties(const AttributeConfig &);
template void Attribute::check_hard_coded_properties(const AttributeConfig_3 &);

template void Attribute::set_hard_coded_properties(const AttributeConfig &);
template void Attribute::set_hard_coded_properties(const AttributeConfig_3 &);

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

} // End of Tango namespace

