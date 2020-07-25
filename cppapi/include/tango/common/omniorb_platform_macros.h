#ifndef TANGO_OMNIORB_PLATFORM_MACROS_H
#define TANGO_OMNIORB_PLATFORM_MACROS_H

// NOTE: This file must be included before including any other omniORB headers.
// It must also be included before including the `idl/tango.h` header file.

// omniORB requires below definitions to be set on Windows platform.
// See: http://omniorb.sourceforge.net/omni41/omniORB/omniORB001.html#toc3

#ifdef _WIN32

#ifndef __WIN32__
#define __WIN32__
#endif

#ifndef __x86__
#define __x86__
#endif

#ifndef __NT__
#define __NT__
#endif

#ifndef __OSVERSION
#define __OSVERSION 4
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#endif // _WIN32

#endif // TANGO_OMNIORB_PLATFORM_MACROS_H
