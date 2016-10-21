//
// Created by ingvord on 10/17/16.
//

#pragma once

#include <memory>
#include "DevVarArray.hxx"

namespace Tango {
    //forward declaration for backend implementation
    namespace backend {
        class DevVarDoubleArray;
    }

    typedef DevVarArray<double, backend::DevVarDoubleArray> DevVarDoubleArray;

    typedef std::unique_ptr<DevVarDoubleArray> DevVarDoubleArray_var;
}//Tango