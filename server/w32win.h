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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
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
void SetDebugLevel(LogLevel,HMENU,bool);
void all_dev(Util *,LogLevel);
int get_all_dev_log_level(Util *,LogLevel &);
void NoDebugLevel(HMENU,bool);
void add_cout_appender(Util *);
void all_dev_add_cout(Util *);

} // End of Tango namespace

#endif /* _COUTBUF_H */
