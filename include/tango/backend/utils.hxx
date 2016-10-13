//
// Created by ingvord on 10/12/16.
//

#pragma once

#include "dev_xxx.hxx"

namespace Tango {
    template<typename T>
    /*static*/ std::string string_dup(T *c_str) {
            return std::move(std::string(c_str));
    }

    /**
     * Sets client connection timeout
     *
     * @param timeout in millis
     */
    /*static*/void setClientConnectTimeout(DevULong timeout);
}//tango
