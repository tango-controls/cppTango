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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
//
//=============================================================================

#ifndef _COUTBUF_H
#define _COUTBUF_H

#include <tango.h>

namespace Tango
{

//
//   Some defines
//

#define IDC_LIST      	-777
#define MAXLISTLINES  	200

class CoutBuf
{
public:
  	CoutBuf(HINSTANCE,int,HWND,LPCSTR);
  	virtual ~CoutBuf();
  	HWND get_debug_window() {return DbgWin;}
  	void clear_debug_window() {DbgWin = NULL;}
  	void CreateWin(LPCSTR);

  	int dbg_out (LPCSTR);

protected:

  	HWND DbgWin;
  	HWND parent_window;

};

//
//  Some functions for windows window management!!!
//

LRESULT CALLBACK DebugWndProc(HWND, UINT, WPARAM, LPARAM );
void DrawDebugItem(HWND, LPDRAWITEMSTRUCT );
void MeasureDebugItem(HWND, LPMEASUREITEMSTRUCT);

} // End of Tango namespace

#endif /* _COUTBUF_H */
