//
// Created by ingvord on 10/30/16.
//

#include <algorithm>
#include <tango.h>
#include <tango/server/tango_const.h>
#include <tango/server/except.h>

using namespace std;
using namespace Tango;

PollObjType Tango::PollObjType_from_string(string &&value) {
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    if (value == PollCommand) {
        return POLL_CMD;
    } else if (value == PollAttribute) {
        return POLL_ATTR;
    } else {
        Except::throw_exception(API_NotSupported, "Object type " + value + " not supported", "PollObjType_from_string");
    }
}

