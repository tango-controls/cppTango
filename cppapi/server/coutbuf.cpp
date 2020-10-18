static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :         coutbuf.cpp
//
// description :  C++ source for the coutbuf class. This class is used
//          only for Windows device server for the debug
//          outputs
//
// project :      TANGO
//
// author(s) :    A.Gotz + E.Taurel
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
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <coutbuf.h>
#include <resource.h>
#include <commctrl.h>

namespace Tango
{

static HINSTANCE hAppInst;
static HWND hWndDebug;
static HWND hWndDebugList = NULL;

//+----------------------------------------------------------------------------
//
// method :       CoutBuf::CoutBuf()
//
// description :  Constructor of the CoutBuf class. It mainly creates
//          	  the debug output window
//
// in :     - hInstance : The application instance
//          - nCmdShow :
//          - svc_name : The device server name
//
//-----------------------------------------------------------------------------


CoutBuf::CoutBuf(HINSTANCE hInstance,int nCmdShow,HWND parent,LPCSTR svc_name)
{

  	hAppInst = hInstance;

//
// Init common controls library
//

  	InitCommonControls();

//
// Register out window class
//

  	WNDCLASS wc;

  	wc.lpszClassName  = "Debug";
  	wc.style          = CS_HREDRAW | CS_VREDRAW;
  	wc.hCursor        = ::LoadCursor(NULL, IDC_ARROW);
  	wc.hIcon          = ::LoadIcon(NULL,IDI_APPLICATION);
  	wc.lpszMenuName   = NULL;
  	wc.hbrBackground  = ::CreateSolidBrush(RGB(255,255,255));
  	wc.hInstance      = hInstance;
  	wc.lpfnWndProc    = DebugWndProc;
  	wc.cbClsExtra     = 0;
  	wc.cbWndExtra     = 0;

  	RegisterClass(&wc);

//
// create the debug console
//

  	parent_window = parent;
  	CreateWin(svc_name);


//
// Show the main window
//

  	DbgWin = hWndDebug;
//	ShowWindow(hWndDebug,nCmdShow);
//	UpdateWindow(hWndDebug);

}

CoutBuf::~CoutBuf()
{
}

void CoutBuf::CreateWin(LPCSTR svc_name)
{
  	char buf[256];

  	std::snprintf(buf, sizeof(buf),"%s - Console",svc_name);
 	hWndDebug = CreateWindow("Debug",buf,
                          WS_OVERLAPPED | WS_CAPTION | WS_BORDER | WS_THICKFRAME,
                          GetSystemMetrics(SM_CXSCREEN) / 2-100,
                          GetSystemMetrics(SM_CYSCREEN) * 3 / 4-75,
                          GetSystemMetrics(SM_CXSCREEN) / 2,
                          GetSystemMetrics(SM_CYSCREEN) / 4,
                          parent_window,
                          0,hAppInst,NULL);

  	if (!hWndDebug)
  	{
    		Except::throw_exception((LPCSTR)API_NtDebugWindowError,
              				(LPCSTR)"Can't create debug window",
              				(LPCSTR)"CoutBuf::CoutBuf");
  	}
  	DbgWin = hWndDebug;
}

//+----------------------------------------------------------------------------
//
// method :       CoutBuf::dbg_out()
//
// description :  This method add a string to the end of the debug list
//
// in :     - buf : The string to be printed
//
//-----------------------------------------------------------------------------

int CoutBuf::dbg_out(LPCSTR buf)
{

//
// Stop the listbox repaints while we mess with it
//

  	SendMessage(hWndDebugList,WM_SETREDRAW,(WPARAM)FALSE,(LPARAM)0);

//
// Get the item count
//

  	int i = (int)SendMessage(hWndDebugList,LB_GETCOUNT,(WPARAM)0,(LPARAM)0);
  	if (i == LB_ERR)
    		i = 0;

//
// Scrub a few if we have too many
//

  	while (i >= MAXLISTLINES)
  	{
    		SendMessage(hWndDebugList,LB_DELETESTRING,(WPARAM)0,(LPARAM)0);
    		i--;
  	}

//
// Add the new one on at the end and scroll it into view
//

  	i = (int) SendMessage(hWndDebugList,LB_ADDSTRING,(WPARAM)0,(LPARAM)buf); //MODIF-NL
  	SendMessage(hWndDebugList,LB_SETCURSEL,(WPARAM)i,(LPARAM)0);

//
// Enable the repaint now
//

  	SendMessage(hWndDebugList,WM_SETREDRAW,(WPARAM)TRUE,(LPARAM)0);

  	return 0;
}



//
// Window procedure for debug window
//

LRESULT CALLBACK DebugWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  	PAINTSTRUCT ps;
  	HWND hWndList = NULL;
  	HWND hMainWnd;
  	HMENU hMenu;
  	BOOL bo;

  	switch(msg)
  	{
  	case WM_CREATE:

//
// Create the listbox
//

    		hWndDebugList = CreateWindow("Listbox","",
                                    WS_CHILD
                                  | WS_VISIBLE
                                  | WS_VSCROLL
                                  | LBS_DISABLENOSCROLL
                                  | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED
                                  | LBS_NOINTEGRALHEIGHT,
                                  0,0,0,0,hWnd,
                                  (HMENU)IDC_LIST,hAppInst,
                                  (LPSTR)NULL);
    		break;

  	case WM_SIZE:
    		SetWindowPos(hWndDebugList,NULL,0,0,LOWORD(lParam),HIWORD(lParam),SWP_NOZORDER);
    		break;

  	case WM_SETFOCUS:
    		SetFocus(hWndDebugList);
    		break;

  	case WM_MEASUREITEM:
    		MeasureDebugItem(hWnd,(LPMEASUREITEMSTRUCT)lParam);
    		return (LRESULT) TRUE;

  	case WM_DRAWITEM:
    		DrawDebugItem(hWnd,(LPDRAWITEMSTRUCT)lParam);
    		break;

  	case WM_PAINT:
    		BeginPaint(hWnd, &ps);
    		EndPaint(hWnd, &ps);
    		break;

  	case WM_DESTROY:
    		hWndDebug = NULL;
    		Tango::Util::instance()->get_debug_object()->clear_debug_window();

    		hMainWnd = Tango::Util::instance()->get_ds_main_window();
    		hMenu = GetMenu(hMainWnd);
    		MENUITEMINFO mii;

    		mii.fMask = MIIM_STATE;
    		mii.cbSize = sizeof(mii);
    		bo = GetMenuItemInfo(hMenu,ID_VIEW_CONSOLE,FALSE,&mii);
    		mii.fState ^= MFS_CHECKED;
    		bo = SetMenuItemInfo(hMenu,ID_VIEW_CONSOLE,FALSE,&mii);
    		break;

  	default:
    		return DefWindowProc(hWnd, msg, wParam, lParam);
    		break;
 	}
  	return 0;
}

//
// Measure an item in our debug listbox
//

void MeasureDebugItem(HWND hWnd, LPMEASUREITEMSTRUCT lpMIS)
{
  	TEXTMETRIC tm;
  	HDC hDC = GetDC(hWnd);
  	GetTextMetrics(hDC, &tm);
  	ReleaseDC(hWnd, hDC);
  	lpMIS->itemHeight = tm.tmHeight;
}

//
// Display an item in our debug listbox
//

void DrawDebugItem(HWND hWnd, LPDRAWITEMSTRUCT lpDI)
{
  	HBRUSH hbrBkGnd;
  	RECT rc;
  	HDC hDC;
  	char buf[256];

  	hDC = lpDI->hDC;
  	rc = lpDI->rcItem;

  	switch (lpDI->itemAction)
  	{
  	case ODA_SELECT:
  	case ODA_DRAWENTIRE:

//
// Erase the rectangle
//
    		hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    		FillRect(hDC, &rc, hbrBkGnd);
    		DeleteObject(hbrBkGnd);

//
// Show the text in our standard font
//

    		SetBkMode(hDC,(int)1);
//        SetBkMode(hDC,TRANSPARENT);
    		SendMessage(lpDI->hwndItem,LB_GETTEXT,lpDI->itemID,(LPARAM)(LPSTR)buf);

    		ExtTextOut(hDC,rc.left+2, rc.top,ETO_CLIPPED,&rc,buf,lstrlen(buf),NULL);

    		break;

  	}
}

} // End of Tango namespace
