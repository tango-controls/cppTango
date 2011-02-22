/*
 * DllMain.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifdef LOG4TANGO_SUPPLY_DLLMAIN

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
  switch (ul_reason_for_call)
  {
      case DLL_PROCESS_ATTACH:
      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
      case DLL_PROCESS_DETACH:
          break;
  }
  return TRUE;
}

#endif // LOG4TANGO_SUPPLY_DLLMAIN
