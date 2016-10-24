//
// Created by ingvord on 10/24/16.
//

#pragma once

#include <omniORB4/CORBA.h>

namespace Tango {
    using TangORB = CORBA::ORB;
    using TangORB_ptr = std::unique_ptr<TangORB>;
    using TangORB_var = std::shared_ptr<TangORB>;

    template<typename OPTIONS>
    inline auto TangORB_init(int& argc, char** argv, char* orb_id, OPTIONS options)
        -> TangORB_var
    {
        auto orb = CORBA::ORB_init(argc, argv, orb_id, options);
        return TangORB_var(orb);
    }

    auto TangORB_init() -> TangORB_var;
}//Tango