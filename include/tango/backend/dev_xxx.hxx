//
// Created by ingvord on 10/11/16.
//

#pragma once

#include <cstdint>

namespace Tango {
    struct Any{

    };

    using DevUChar = uint8_t;
    using DevShort = int16_t;
    using DevUShort = uint16_t;
    using DevLong = int32_t;
    using DevULong = uint32_t;
    using DevLong64 = int64_t;
    using DevULong64 = uint64_t;
    using DevBoolean = bool;
    using DevFloat = float;
    using DevDouble = double;
    using DevString = std::string;


    template <typename T>
    void operator<<=(Any, T*);

    template <typename T>
    void operator<<=(Any, T);
}//Tango
