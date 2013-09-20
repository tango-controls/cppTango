//+=================================================================================================================
//
// file :               templ_inst.cpp
//
// description :        C++ source code to instantiate template methods in order to have explicit instanciation
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
// These methods are in a sepearte files because I did not find a way to explicitely instanciate specialized
// templates. Therefore, for template management we have three files:
// 1 - A file with explicit template instanciation (templ_inst.cpp)
// 2 - A file with template method definition (attribute.tpp for Attribute class)
// 3 - A file with template specialization defnition (attribute_spec.tpp (this file))
//
// We did this in order to have explicit instanciation of templates method except for specialized templates
// for which we have instanciation following the inclusion model
// See C++ template book chapter 6
//

#include <tango.h>
#include <attribute.tpp>

namespace Tango
{

//
// The DevEncoded and DevString data types are not there. DevString is handle by 2 (for const) classic methods
// forwarding the call to the same method using string data type as parameter. DevEncoded and string are
// managed as specialized templates
//

#define TEMPL_EXPL_INST_CONST(METH) \
template void Attribute::METH(const DevBoolean &); \
template void Attribute::METH(const DevUChar &); \
template void Attribute::METH(const DevShort &); \
template void Attribute::METH(const DevUShort &); \
template void Attribute::METH(const DevLong &); \
template void Attribute::METH(const DevULong &); \
template void Attribute::METH(const DevLong64 &); \
template void Attribute::METH(const DevULong64 &); \
template void Attribute::METH(const DevFloat &); \
template void Attribute::METH(const DevDouble &); \
template void Attribute::METH(const DevState &);


#define TEMPL_EXPL_INST(METH) \
template void Attribute::METH(DevBoolean &); \
template void Attribute::METH(DevUChar &); \
template void Attribute::METH(DevShort &); \
template void Attribute::METH(DevUShort &); \
template void Attribute::METH(DevLong &); \
template void Attribute::METH(DevULong &); \
template void Attribute::METH(DevLong64 &); \
template void Attribute::METH(DevULong64 &); \
template void Attribute::METH(DevFloat &); \
template void Attribute::METH(DevDouble &); \
template void Attribute::METH(DevState &); \
template void Attribute::METH(DevString &); \
template void Attribute::METH(DevEncoded &);

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

//
// Instanciate Attribute::xxx methods
//

TEMPL_EXPL_INST(get_min_alarm)
TEMPL_EXPL_INST(get_max_alarm)
TEMPL_EXPL_INST(get_min_warning)
TEMPL_EXPL_INST(get_max_warning)

TEMPL_EXPL_INST_CONST(set_min_alarm)
TEMPL_EXPL_INST_CONST(set_max_alarm)
TEMPL_EXPL_INST_CONST(set_min_warning)
TEMPL_EXPL_INST_CONST(set_max_warning)

template void Attribute::check_hard_coded_properties(const AttributeConfig &);
template void Attribute::check_hard_coded_properties(const AttributeConfig_3 &);

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

} // End of Tango namespace

