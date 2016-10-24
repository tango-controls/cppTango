//
// Created by ingvord on 10/24/16.
//

#pragma once

namespace Tango {
    using TangORB = CORBA::ORB;
    using TangORB_ptr = CORBA::ORB_ptr;
    using TangORB_var = CORBA::ORB_var;

    template<typename OPTIONS>
    inline TangORB_ptr TangORB_init(int& argc, char** argv, char* orb_id, OPTIONS options) { return CORBA::ORB_init(argc, argv, orb_id, options);}
}//Tango