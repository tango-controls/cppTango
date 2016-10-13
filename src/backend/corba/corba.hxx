//
// Created by ingvord on 10/10/16.
//
#pragma once

//moved from tango_config.h
//
// Is it a 32 or 64 bits computer
//
#ifndef _TG_WINDOWS_
#include <omniORB4/acconfig.h>

#ifdef PACKAGE_BUGREPORT
#undef PACKAGE_BUGREPORT
#endif

#ifdef PACKAGE_NAME
#undef PACKAGE_NAME
#endif

#ifdef PACKAGE_STRING
#undef PACKAGE_STRING
#endif

#ifdef PACKAGE_TARNAME
#undef PACKAGE_TARNAME
#endif

#ifdef PACKAGE_VERSION
#undef PACKAGE_VERSION
#endif

#if SIZEOF_LONG == 8
#define TANGO_LONG64
#else
#define TANGO_LONG32
#endif
#else
#define TANGO_LONG32
#endif