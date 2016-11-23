//
// Created by ingvord on 10/24/16.
//

#pragma once

#include <memory>
#include <omniORB4/CORBA.h>
#include <tango/frontend/request.hxx>
#include <tango/frontend/object.hxx>
#include <vector>
#include <map>

namespace Tango {
    namespace frontend {
        struct ORB {
            using RequestSeq = std::vector<TangoRequest_ptr>;
            virtual ~ORB() = 0;

            virtual bool poll_next_response() = 0;

            virtual void get_next_response(TangoRequest_ptr) = 0;

            virtual void send_multiple_requests_deferred(RequestSeq) = 0;

            virtual TangoObject_var string_to_object(std::string) = 0;

            virtual std::string object_to_string(TangoObject_var&) = 0;

            virtual TangoObject_var resolve_initial_references(std::string) = 0;

            virtual bool work_pending() = 0;

            virtual void perform_work() = 0;

            virtual void run() = 0;

            virtual void shutdown(bool) = 0;

            virtual void destroy() = 0;
        };
    }

    using TangORB = frontend::ORB;

    using TangORB_ptr = std::unique_ptr<TangORB>;
    using TangORB_var = std::shared_ptr<TangORB>;

    auto TangORB_init(int&, char**, std::map<std::string, std::string>)-> TangORB_ptr;

    auto TangORB_init() -> TangORB_ptr;
}//Tango