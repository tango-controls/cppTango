//
// Created by ingvord on 10/24/16.
//

#pragma once

#include <omniORB4/CORBA.h>

namespace Tango {
    inline char *string_dup(char *s) { return CORBA::string_dup(s); }

    inline char *string_dup(const char *s) { return CORBA::string_dup(s); }
}//Tango