//=============================================================================
//
// file :               coutbuf.h
//
// description :        Include for Windows NT debug output class  
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
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

#ifndef _COUTBUF_H
#define _COUTBUF_H

#include <tango.h>
#include <streambuf>

namespace Tango
{

//
//   Some defines
//

#define IDC_LIST        -777
#define MAXLISTLINES    200
#define bufferSize      512



class CoutBuf: public streambuf
{
public:
        CoutBuf(HINSTANCE,int,HWND,LPCSTR);
        virtual ~CoutBuf();
	HWND get_debug_window() {return DbgWin;}
	void clear_debug_window() {DbgWin = NULL;}
	void CreateWin(LPCSTR);
       
protected:

        char buffer[bufferSize];
	long nb_critical;
	HWND DbgWin;
	HWND parent_window;

        int dbg_out(LPCSTR);
        virtual int_type overflow(int_type);
        int flushBuffer();
        virtual int sync();
	virtual streamsize xsputn(const char_type *,streamsize);
};


//
//  Some functions for windows window management!!!
//

LRESULT CALLBACK DebugWndProc(HWND, UINT, WPARAM, LPARAM );
void DrawDebugItem(HWND, LPDRAWITEMSTRUCT );
void MeasureDebugItem(HWND, LPMEASUREITEMSTRUCT);

} // End of Tango namespace

#endif /* _COUTBUF_H */
