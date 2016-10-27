//
// Created by ingvord on 10/25/16.
//

#include <tango/backend/orb.hxx>
#include <tango/frontend/object.hxx>

#include <omniORB4/CORBA.h>
#include <signal.h>
#include <tango/frontend/orb_const.hxx>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace Tango;

backend::ORB::~ORB() {
    CORBA::release(this->orb_ptr_);
}

bool backend::ORB::poll_next_response() {
    return this->orb_ptr_->poll_next_response();
}

void backend::ORB::get_next_response(TangoRequest_ptr req) {
    return this->orb_ptr_->get_next_response(req);
}

TangoObject_var backend::ORB::string_to_object(std::string str) {
    return this->orb_ptr_->string_to_object(str.c_str());
}

std::string backend::ORB::object_to_string(TangoObject_var& obj) {
    return std::string(this->orb_ptr_->object_to_string(obj));
}

void backend::ORB::shutdown(bool wait_for_completion){
    this->orb_ptr_->shutdown(wait_for_completion);
}

void backend::ORB::destroy() {
    this->orb_ptr_->destroy();
}

backend::ORB::ORB(CORBA::ORB *orb_ptr_) : orb_ptr_(orb_ptr_) {}

void backend::ORB::send_multiple_requests_deferred(frontend::ORB::RequestSeq seq) {
    //TODO transform seq to CORBA::ORB::RequestSeq
    CORBA::ORB::RequestSeq corba_seq;
    corba_seq.length(seq.size());
    for(size_t i = 0 ; i < seq.size(); ++i)
        corba_seq[i] = seq[i];
    this->orb_ptr_->send_multiple_requests_deferred(corba_seq);
}

TangoObject_var backend::ORB::resolve_initial_references(std::string string) {
    return this->orb_ptr_->resolve_initial_references(string.c_str());
}

bool backend::ORB::work_pending() {
    return this->orb_ptr_->work_pending();
}

void backend::ORB::perform_work() {
    this->orb_ptr_->perform_work();
}

void backend::ORB::run() {
    this->orb_ptr_->run();
}


auto backend::ORB_init()
    -> ORB_ptr
{
    int _argc;
    char **_argv;

//
// pass dummy arguments to init() because we don't have access to argc and argv
//

    _argc = 1;
    _argv = (char **) malloc(sizeof(char *));
    _argv[0] = (char *) "dummy";

    //
// Init the ORB
// Starting with omniORB 4.2, we need to add the throwTransientOnTimeout option for compatibility
//

    map<string,string> options_map;

    options_map.emplace("clientCallTimeOutPeriod",    CLNT_TIMEOUT_STR);
    options_map.emplace("verifyObjectExistsAndType",  "0");
    options_map.emplace("maxGIOPConnectionPerServer", MAX_GIOP_PER_SERVER);
    options_map.emplace("giopMaxMsgSize",             MAX_TRANSFER_SIZE);
    options_map.emplace("throwTransientOnTimeOut",    "1");

    auto orb = ORB_init(_argc, _argv, options_map);

    free(_argv);

    return orb;
}

auto backend::ORB_init(int &argc, char **argv, map<string,string> options)
    -> backend::ORB_ptr
{


//
// Get user signal handler for SIGPIPE (ORB_init call install a SIG_IGN for SIGPIPE. This could be annoying in case
// the user uses SIGPIPE)
//

#ifndef _TG_WINDOWS_
    struct sigaction sa;
    sa.sa_handler = NULL;

    if (sigaction(SIGPIPE, NULL, &sa) == -1)
        sa.sa_handler = NULL;
#endif

    bool omni_42_compat = false;
    auto omni_vers_hex = omniORB::versionHex();
    if (omni_vers_hex > 0x04020000)
        omni_42_compat = true;


    //TODO calculate total size
    char* omni_options[options.size() + 1][2];


    //TODO strcpy
//    auto i = 0;
//    for_each(options.begin(), options.end(), [&i, &omni_options](pair<string, string>& pair){
//        omni_options[i++] = {pair.first.c_str(), pair.second.c_str()};
//    });
//    omni_options[i] = {NULL, NULL};


    if (omni_42_compat == false) {
        int nb_opt = sizeof(omni_options) / sizeof(char *[2]);
        omni_options[nb_opt - 2][0] = NULL;
        omni_options[nb_opt - 2][1] = NULL;
    }

    auto _orb = CORBA::ORB_init(argc, argv, "omniORB4", omni_options);

//
// Restore SIGPIPE handler
//

#ifndef _TG_WINDOWS_
    if (sa.sa_handler != NULL) {
        struct sigaction sb;

        sb = sa;

        if (sigaction(SIGPIPE, &sb, NULL) == -1) {
            std::cerr << "Can not re-install user signal handler for SIGPIPE!" << std::endl;
        }
    }
#endif
    return std::unique_ptr<frontend::ORB>{new backend::ORB{_orb}};
}


