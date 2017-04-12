//===================================================================================================================
//
// file :		device_command_info.hxx
//
// project :		tango
//
// author(s) :		ingvord
//
// Copyright (C) :      2004-2017
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
//===================================================================================================================

#pragma once


namespace Tango {
/**
 * Base structure for command information
 *
 * @headerfile tango.h
 */
#ifdef GEN_DOC
    typedef struct DevCommandInfo
#else
    typedef struct _DevCommandInfo
#endif
    {
        string cmd_name;           ///< The command name
        long cmd_tag;            ///< The command tag
        long in_type;            ///< Input parameter data type
        long out_type;           ///< Output parameter data type
        string in_type_desc;       ///< Input parameter description
        string out_type_desc;      ///< Ouptput parameter description
///@privatesection
        bool operator==(const _DevCommandInfo &);
    } DevCommandInfo;

/**
 * Command information data extension
 *
 * @headerfile tango.h
 * @ingroup Client
 */
#ifdef GEN_DOC
    typedef struct CommandInfo : public DevCommandInfo
#else
    typedef struct _CommandInfo : public DevCommandInfo
#endif
    {
        Tango::DispLevel disp_level;    ///< The command display level

        vector<string> in_enum_labels; ///< Labels for enum arg or empty
        vector<string> out_enum_labels; ///< Labels for enum result or empty

        _CommandInfo() : DevCommandInfo(),
                         in_enum_labels(),
                         out_enum_labels() {}

/// @privatesection
        friend ostream &operator<<(ostream &, _CommandInfo &);

        bool operator==(const _CommandInfo &);
    } CommandInfo;

    CommandInfo newCommandInfo(const DevCmdInfo&);
    CommandInfo newCommandInfo(const DevCmdInfo_2&);
    CommandInfo newCommandInfo(const DevCmdInfo_3&);

//TODO template
    void fill_vector(vector<string> &, const DevVarStringArray &);

/**
 * A vector of CommandInfo structure
 *
 * @headerfile tango.h
 * @ingroup Client
 */
    typedef vector<CommandInfo> CommandInfoList;

    template<typename DevCommandInfoList>
    CommandInfoList* newCommandInfoList(DevCommandInfoList devCmdInfoList_var){
        auto result = new CommandInfoList(devCmdInfoList_var->length());

        for (size_t i=0, size = devCmdInfoList_var->length(); i < size; ++i) {
            (*result)[i] = newCommandInfo(devCmdInfoList_var[i]);
        }

        return(result);
    }
}