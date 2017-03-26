//
// Created by ingvord on 3/26/17.
//

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

    CommandInfo newCommandInfo(DevCmdInfo_var);
    CommandInfo newCommandInfo(DevCmdInfo_2_var);
    CommandInfo newCommandInfo(DevCmdInfo_3_var);

//TODO template
    void fill_vector(vector<string> &, DevVarStringArray &);

/**
 * A vector of CommandInfo structure
 *
 * @headerfile tango.h
 * @ingroup Client
 */
    typedef vector<CommandInfo> CommandInfoList;
}