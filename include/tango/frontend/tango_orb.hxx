//
// Created by ingvord on 10/24/16.
//

#pragma once

namespace Tango {
    using TangORB = CORBA::ORB;
    using TangORB_ptr = CORBA::ORB_ptr;
    using TangORB_var = std::shared_ptr<TangORB>;

    template<typename OPTIONS>
    inline auto TangORB_init(int& argc, char** argv, char* orb_id, OPTIONS options)
        -> TangORB_var
    {
        auto orb = CORBA::ORB_init(argc, argv, orb_id, options);
        return TangORB_var(orb);
    }
}//Tango