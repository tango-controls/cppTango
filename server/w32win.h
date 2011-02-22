//=============================================================================
//
// file :               w32win.h
//
// description :        Include for Windows NT device server output window  
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _W32WIN_H
#define _W32WIN_H

#include <tango.h>

namespace Tango
{

class Util;
class CoutBuf;

class W32Win
{
public:
        W32Win(Util *,int);
        ~W32Win();
	
	HWND get_win() {return win;}
	CoutBuf *get_output_buffer() {return pcb;}

private:
	HWND win;

	CoutBuf *pcb;
	void RegisterTangoClass(HINSTANCE);
	void InitInstance(HINSTANCE,int);
};

//
// Some define for the main window drawing
//

#define		X_FACTOR	2
#define		Y_FACTOR	1.3
#define		TOO_LARGE_TXT	"Your text is too large for this window !!"
#define		MAIN_TXT	"Tango\nDevice\nServer"

//
//  Some functions for windows window management!!!
//

LRESULT CALLBACK TangoWndProc(HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK AboutProc(HWND, UINT, WPARAM, LPARAM);
void SetDebugLevel(int,HMENU);

} // End of Tango namespace

#endif /* _COUTBUF_H */
