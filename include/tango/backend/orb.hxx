//
// Created by ingvord on 10/25/16.
//

#pragma once

#include <tango/frontend/orb.hxx>
#include <omniORB4/CORBA.h>


namespace Tango {
    namespace backend {
        class ORB : public frontend::ORB {
            CORBA::ORB_ptr orb_ptr_;
        public:
            ORB(CORBA::ORB *orb_ptr_);
            ~ORB();
        public:
            bool poll_next_response() override;
            void get_next_response(TangoRequest_ptr) override;

            virtual void send_multiple_requests_deferred(RequestSeq seq) override;

            TangoObject_var string_to_object(std::string) override ;

            std::string object_to_string(TangoObject_var&) override ;

            virtual TangoObject_var resolve_initial_references(std::string string) override;

            virtual bool work_pending() override;

            virtual void perform_work() override;

            virtual void run() override;

            void shutdown(bool) override;

            void destroy() override;
        };

        using ORB_ptr = std::unique_ptr<frontend::ORB>;

        auto ORB_init()
        -> ORB_ptr;

        auto ORB_init(int& argc, char** argv, char* orb_id, const char *options[][2])
        -> ORB_ptr;
    }//backend
}//Tango