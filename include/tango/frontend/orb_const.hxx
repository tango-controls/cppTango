//
// Created by ingvord on 10/25/16.
//

#pragma once

//
// Client timeout as defined by omniORB4.0.0
//

const char* const CLNT_TIMEOUT_STR         = "3000";
const int   CLNT_TIMEOUT                   = 3000;
const int   NARROW_CLNT_TIMEOUT            = 100;

//
// Max transfer size 256 MBytes (in byte). Needed by omniORB
//

const char* const MAX_TRANSFER_SIZE        = "268435456";

//
// Max GIOP connection per server . Needed by omniORB
//

const char* const MAX_GIOP_PER_SERVER      = "128";
