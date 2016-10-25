//
// Created by ingvord on 10/25/16.
//

#include <tango/frontend/orb.hxx>
#include <tango/backend/orb.hxx>

Tango::frontend::ORB::~ORB(){}

auto Tango::TangORB_init(int &argc, char **argv, char *orb_id, const char *options[][2]) -> decltype(Tango::backend::ORB_init(argc, argv, orb_id, options)) {
    return Tango::backend::ORB_init(argc, argv, orb_id, options);
}

auto Tango::TangORB_init() -> decltype(Tango::backend::ORB_init()) {
    return Tango::backend::ORB_init();
}

