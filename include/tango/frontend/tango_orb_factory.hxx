//
// Created by ingvord on 10/24/16.
//

#pragma once

#include <tango/frontend/tango_orb.hxx>

namespace Tango {
    struct TangORBFactory {
        virtual auto create() -> TangORB_var = 0;
        virtual ~TangORBFactory(){};
    };

    using TangORBFactory_ptr = std::unique_ptr<TangORBFactory>;
}//Tango