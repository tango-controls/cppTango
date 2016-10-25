//
// Created by ingvord on 10/25/16.
//

#include <tango/frontend/tango_orb.hxx>
#include <omniORB4/CORBA.h>
#include <signal.h>
#include <tango/frontend/tango_orb_const.hxx>
#include <iostream>

Tango::TangORB_var Tango::TangORB_init() {
    int _argc;
    char **_argv;

//
// pass dummy arguments to init() because we don't have access to argc and argv
//

    _argc = 1;
    _argv = (char **) malloc(sizeof(char *));
    _argv[0] = (char *) "dummy";

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

//
// Init the ORB
// Starting with omniORB 4.2, we need to add the throwTransientOnTimeout option for compatibility
//

    bool omni_42_compat = false;
    auto omni_vers_hex = omniORB::versionHex();
    if (omni_vers_hex > 0x04020000)
        omni_42_compat = true;

    const char *options[][2] = {
            {"clientCallTimeOutPeriod",    CLNT_TIMEOUT_STR},
            {"verifyObjectExistsAndType",  "0"},
            {"maxGIOPConnectionPerServer", MAX_GIOP_PER_SERVER},
            {"giopMaxMsgSize",             MAX_TRANSFER_SIZE},
            {"throwTransientOnTimeOut",    "1"},
            {0,                            0}};

    if (omni_42_compat == false) {
        int nb_opt = sizeof(options) / sizeof(char *[2]);
        options[nb_opt - 2][0] = NULL;
        options[nb_opt - 2][1] = NULL;
    }

    auto _orb = TangORB_init(_argc, _argv, "omniORB4", options);

    free(_argv);

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
    return _orb;
}
