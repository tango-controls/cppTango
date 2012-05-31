static const char *RcsId = "$Id$\n$Name$";
static const char *Vers = "$Revision$";

//+=============================================================================
//
// file :               w32win.cpp
//
// description :        C++ source for the main Tango device server output
//			window uses with Windows.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
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
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <w32win.h>
#include <resource.h>

namespace Tango
{

static HINSTANCE hInst;
static TCHAR szWindowClass[40] = "TangoDS_Window_Class";
static int Main_Win_width;
static int Main_Win_height;
static HDC Main_Win_MemDC;
static LOGFONT lf;
static LOGFONT user_font;

//+----------------------------------------------------------------------------
//
// method : 		W32Win::W32Win()
//
// description : 	Constructor of the W32Win class. It mainly creates
//                      the server output main window
//
//-----------------------------------------------------------------------------


W32Win::W32Win(Util *ptr,int nCmd)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	hInst = hInstance;

	RegisterTangoClass(hInstance);

	InitInstance(hInstance,nCmd);

	pcb = new CoutBuf(hInstance,nCmd,win,ptr->get_ds_name().c_str());
}


W32Win::~W32Win()
{
}

void W32Win::RegisterTangoClass(HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)TangoWndProc;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	= hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(TANGO_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = (LPCSTR)TANGO_MENU1;
	wc.lpszClassName = szWindowClass;

	ATOM at = RegisterClass(&wc);
	if (at == 0)
	{
                Except::throw_exception((LPCSTR)"API_NtDebugWindowError",
				      (LPCSTR)"Can't register class for server main output window",
                                      (LPCSTR)"W32Win::RegisterTangoClass");
	}
}

void W32Win::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

//
// Build window title
//

	TCHAR szTitle[80];
	strcpy(szTitle,"Tango device server : ");
	Tango::Util *tg = Tango::Util::instance();
	strcat(szTitle,tg->get_ds_name().c_str());

//
// Create window
//

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
			    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL,
			    hInstance, NULL);

	if (!hWnd)
	{
                Except::throw_exception((LPCSTR)"API_NtDebugWindowError",
				      (LPCSTR)"Can't create main device server window",
                                      (LPCSTR)"W32Win::InitInstance");

	}
	win = hWnd;

//
// Set background bitmap
//

	HBITMAP dancers = LoadBitmap(hInst,MAKEINTRESOURCE(TANGO_BITMAP1));
	BITMAP dancers_bm;
	GetObject(dancers,sizeof(BITMAP),&dancers_bm);

	HDC dc = GetDC(hWnd);
	HDC mem_dc = CreateCompatibleDC(dc);
	SelectObject(mem_dc,dancers);

	RECT wnd,client,calc;
	GetWindowRect(hWnd,&wnd);
	GetClientRect(hWnd,&client);
	calc.right = wnd.right - wnd.left - client.right + dancers_bm.bmWidth;
	calc.bottom = wnd.bottom - wnd.top - client.bottom + dancers_bm.bmHeight;

//
// Set window size
//

	SetWindowPos(hWnd,HWND_TOP,0,0,(int)(calc.right*X_FACTOR),
		     (int)(calc.bottom*Y_FACTOR),SWP_NOMOVE | SWP_NOZORDER);

//	SetBkMode(mem_dc,TRANSPARENT);
	SetBkMode(mem_dc,(int)1);

	Main_Win_width = dancers_bm.bmWidth;
	Main_Win_height = dancers_bm.bmHeight;
	Main_Win_MemDC = mem_dc;

//
// Release display context
//

	ReleaseDC(hWnd,dc);

//
// Display window
//

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
}

//
// Window procedure for debug window
//

LRESULT CALLBACK TangoWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	PAINTSTRUCT ps;
	HDC hdc;
	BOOL bo;
	HMENU hMenu;
	LogLevel log_level;
	RECT re;
	int calc_hei;
	bool too_large = false;
	Tango::Util *tg = Tango::Util::instance();

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
			// Parse the menu selections:
		switch (wmId)
		{
		case TANGO_APP_ABOUT:
			DialogBox(hInst,(LPCTSTR)TANGO_ABOUT1,hWnd,(DLGPROC)AboutProc);
			break;

		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;

		case ID_VIEW_CONSOLE:
			hMenu = GetMenu(hWnd);
			if (tg->get_console_window() == NULL)
			{
				tg->get_debug_object()->CreateWin(tg->get_ds_name().c_str());
			}

			MENUITEMINFO mii;

			mii.fMask = MIIM_STATE;
			mii.cbSize = sizeof(mii);
			bo = GetMenuItemInfo(hMenu,ID_VIEW_CONSOLE,FALSE,&mii);
			if (mii.fState == 0x0)
			{
				ShowWindow(tg->get_console_window(),SW_SHOW);
			}
			else
			{
				ShowWindow(tg->get_console_window(),SW_HIDE);
			}

			mii.fState ^= MFS_CHECKED;
			bo = SetMenuItemInfo(hMenu,ID_VIEW_CONSOLE,FALSE,&mii);
			break;

		case ID_DEBUG_LEVEL0:
			SetDebugLevel(LOG_OFF,GetMenu(hWnd),true);
			tg->set_trace_level(0);
			Logging::get_core_logger()->set_level(log4tango::Level::OFF);
			break;

		case ID_DEBUG_LEVEL1:
			SetDebugLevel(LOG_FATAL,GetMenu(hWnd),true);
			tg->set_trace_level(1);
			Logging::get_core_logger()->set_level(log4tango::Level::FATAL);
			add_cout_appender(tg);
			break;

		case ID_DEBUG_LEVEL2:
			SetDebugLevel(LOG_ERROR,GetMenu(hWnd),true);
			tg->set_trace_level(2);
			Logging::get_core_logger()->set_level(log4tango::Level::ERROR);
			add_cout_appender(tg);
			break;

		case ID_DEBUG_LEVEL3:
			SetDebugLevel(LOG_WARN,GetMenu(hWnd),true);
			tg->set_trace_level(3);
			Logging::get_core_logger()->set_level(log4tango::Level::WARN);
			add_cout_appender(tg);
			break;

		case ID_DEBUG_LEVEL4:
			SetDebugLevel(LOG_INFO,GetMenu(hWnd),true);
			tg->set_trace_level(4);
			Logging::get_core_logger()->set_level(log4tango::Level::INFO);
			add_cout_appender(tg);
			break;

		case ID_DEBUG_LEVEL5:
			SetDebugLevel(LOG_DEBUG,GetMenu(hWnd),true);
			tg->set_trace_level(4);
			Logging::get_core_logger()->set_level(log4tango::Level::DEBUG);
			add_cout_appender(tg);
			break;

		case ID_DEV_DEBUG_LEVEL0:
			SetDebugLevel(LOG_OFF,GetMenu(hWnd),false);
			all_dev(tg,LOG_OFF);
			break;

		case ID_DEV_DEBUG_LEVEL1:
			SetDebugLevel(LOG_FATAL,GetMenu(hWnd),false);
			all_dev(tg,LOG_FATAL);
			all_dev_add_cout(tg);
			break;

		case ID_DEV_DEBUG_LEVEL2:
			SetDebugLevel(LOG_ERROR,GetMenu(hWnd),false);
			all_dev(tg,LOG_ERROR);
			all_dev_add_cout(tg);
			break;

		case ID_DEV_DEBUG_LEVEL3:
			SetDebugLevel(LOG_WARN,GetMenu(hWnd),false);
			all_dev(tg,LOG_WARN);
			all_dev_add_cout(tg);
			break;

		case ID_DEV_DEBUG_LEVEL4:
			SetDebugLevel(LOG_INFO,GetMenu(hWnd),false);
			all_dev(tg,LOG_INFO);
			all_dev_add_cout(tg);
			break;

		case ID_DEV_DEBUG_LEVEL5:
			SetDebugLevel(LOG_DEBUG,GetMenu(hWnd),false);
			all_dev(tg,LOG_DEBUG);
			all_dev_add_cout(tg);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc,0,0,Main_Win_width,Main_Win_height,Main_Win_MemDC,0,0,SRCCOPY);

//
// Set font for the main text
//

		strcpy(lf.lfFaceName,"French Script MT");
		lf.lfWeight = FW_BOLD;
		lf.lfHeight = -40;
		lf.lfCharSet = DEFAULT_CHARSET;
		SelectObject(hdc,CreateFontIndirect(&lf));

//
// Display text (in red)
//

		SetTextColor(hdc,RGB(255,0,0));
		re.left = Main_Win_width;
		re.top = 0;
		re.right = Main_Win_width * 2;
		re.bottom = Main_Win_height;
		calc_hei = DrawText(hdc,TEXT(MAIN_TXT),-1,&re,
			            DT_CALCRECT | DT_CENTER);
		re.top = (Main_Win_height - calc_hei) / 2;
		re.bottom = (Main_Win_height + calc_hei) / 2;
		re.right = Main_Win_width * 2;
		re.left = Main_Win_width;
		DrawText(hdc,TEXT(MAIN_TXT),-1,&re,DT_CENTER);

//
// Change font to a more classical one
//

		user_font.lfCharSet = DEFAULT_CHARSET;
		SelectObject(hdc,CreateFontIndirect(&user_font));

//
// Display user text. If the text is too large to fit into the foreseen place
// replace it with a predefined message
//

		re.top = Main_Win_height;
		re.left = 0;
		re.right = (int)(Main_Win_width * X_FACTOR);
		re.bottom = (int)(Main_Win_height * Y_FACTOR);
		SetTextColor(hdc,RGB(0,0,255));
		calc_hei = DrawText(hdc,tg->get_main_window_text().c_str(),-1,&re,
				    DT_CALCRECT | DT_CENTER);
		if (calc_hei > ((Y_FACTOR - 1) * Main_Win_height))
		{
			too_large = true;
			re.top = Main_Win_height;
			re.bottom = (int)(Main_Win_height * Y_FACTOR);
		}
		else
		{
			re.top = (int)(Main_Win_height + (Main_Win_height * ((Y_FACTOR - 1) / 2)) - (calc_hei / 2));
			re.bottom = (int)(Main_Win_height + (Main_Win_height * ((Y_FACTOR - 1) / 2)) + (calc_hei / 2));
		}
		re.left = 0;
		re.right = Main_Win_width * X_FACTOR;

		if (too_large == true)
			DrawText(hdc,TOO_LARGE_TXT,-1,&re,
				 DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		else
			DrawText(hdc,tg->get_main_window_text().c_str(),-1,&re,DT_CENTER);

        	log_level = Logging::log4tango_to_tango_level(Logging::get_core_logger()->get_level());
		SetDebugLevel(log_level,GetMenu(hWnd),true);
		if (get_all_dev_log_level(tg,log_level) == 0)
			SetDebugLevel(log_level,GetMenu(hWnd),false);
		else
			NoDebugLevel(GetMenu(hWnd),false);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
//		tg->get_dserver_device()->DServer::~DServer();
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
        return 0;
}

void SetDebugLevel(LogLevel level,HMENU menu,bool admin_dev)
{
	MENUITEMINFO mii;
	BOOL bo;

//
// Reset all items
//

	mii.fMask = MIIM_STATE;
	mii.fState = 0;
	mii.cbSize = sizeof(mii);

	if (admin_dev == true)
	{
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL0,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL1,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL2,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL3,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL4,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL5,FALSE,&mii);
	}
	else
	{
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL0,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL1,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL2,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL3,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL4,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL5,FALSE,&mii);
	}


//
// Set the chosen level
//

	mii.fState = MFS_CHECKED;
	switch (level)
	{
	case LOG_OFF:
		if (admin_dev == true)
			bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL0,FALSE,&mii);
		else
			bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL0,FALSE,&mii);
		break;

	case LOG_FATAL:
		if (admin_dev == true)
			bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL1,FALSE,&mii);
		else
			bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL1,FALSE,&mii);
		break;

	case LOG_ERROR:
		if (admin_dev == true)
			bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL2,FALSE,&mii);
		else
			bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL2,FALSE,&mii);
		break;

	case LOG_WARN:
		if (admin_dev == true)
			bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL3,FALSE,&mii);
		else
			bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL3,FALSE,&mii);
		break;

	case LOG_INFO:
		if (admin_dev == true)
			bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL4,FALSE,&mii);
		else
			bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL4,FALSE,&mii);
		break;

	case LOG_DEBUG:
		if (admin_dev == true)
			bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL5,FALSE,&mii);
		else
			bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL5,FALSE,&mii);
		break;
	}
}

void NoDebugLevel(HMENU menu,bool admin_dev)
{
	MENUITEMINFO mii;
	BOOL bo;

//
// Reset all items
//

	mii.fMask = MIIM_STATE;
	mii.fState = 0;
	mii.cbSize = sizeof(mii);

	if (admin_dev == true)
	{
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL0,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL1,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL2,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL3,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL4,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEBUG_LEVEL5,FALSE,&mii);
	}
	else
	{
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL0,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL1,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL2,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL3,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL4,FALSE,&mii);
		bo = SetMenuItemInfo(menu,ID_DEV_DEBUG_LEVEL5,FALSE,&mii);
	}
}

LRESULT CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	Util *tg = Util::instance();
	TCHAR str[160];
	string vers;
	string full_vers;
	string::size_type nb_char;

	switch (message)
	{
	case WM_INITDIALOG:
		strcpy(str,"Device server : ");
		strcat(str,tg->get_ds_name().c_str());
		SetDlgItemText(hDlg,TANGO_ABOUT_TEXT2,(LPCSTR)str);

		strcpy(str,"TANGO release : ");
		full_vers = RcsId;
		string::size_type pos,pos2;
		if ((pos = full_vers.rfind(' ')) != string::npos)
		{
			pos2 = full_vers.rfind(' ',pos - 1);
			if (pos2 != string::npos)
			{
				nb_char = pos - pos2;
				vers = full_vers.substr(pos2,nb_char);
				strcat(str,vers.c_str());
			}
			else
			{
				strcat(str,"x.y");
			}
		}
		else
		{
			strcat(str,"x.y");
		}
		SetDlgItemText(hDlg,TANGO_ABOUT_TEXT3,(LPCSTR)str);

		strcpy(str,"TANGO IDL definition release : ");
		strcat(str,tg->get_version_str().c_str());
		SetDlgItemText(hDlg,TANGO_ABOUT_TEXT4,(LPCSTR)str);

		strcpy(str,"Server release : ");
		strcat(str,tg->get_server_version().c_str());
		SetDlgItemText(hDlg,TANGO_ABOUT_TEXT5,(LPCSTR)str);
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void all_dev(Util *tg,LogLevel level)
{
	vector<DeviceImpl *> dev_list;

	dev_list = tg->get_device_list("*");
	for (unsigned int i = 0;i < dev_list.size();i++)
	{
		string dev_name = dev_list[i]->get_name();
		if (dev_name.size() > 7)
		{
			string dev_name_begin = dev_name.substr(0,7);
			transform(dev_name_begin.begin(),dev_name_begin.end(),dev_name_begin.begin(),::tolower);
			if (dev_name_begin == "dserver")
				continue;
		}
        log4tango::Level::Value log4tango_level = Logging::tango_to_log4tango_level(level);
	dev_list[i]->get_logger()->set_level(log4tango_level);
	}
}

int get_all_dev_log_level(Util *tg,LogLevel &lev)
{
	vector<DeviceImpl *> dev_list;
	LogLevel ini_log_level = LOG_WARN;
	LogLevel log_level;
	int i,j;

	dev_list = tg->get_device_list("*");
	int nb_dev = dev_list.size();

//
// If not fully initialized yet
//
	if (nb_dev == 0)
		return -1;

//
// A loop on each device but forget admin device
//

	for (i = 0,j = 0;i < nb_dev;i++)
	{
		string dev_name = dev_list[i]->get_name();
		if (dev_name.size() > 7)
		{
			string dev_name_begin = dev_name.substr(0,7);
			transform(dev_name_begin.begin(),dev_name_begin.end(),dev_name_begin.begin(),::tolower);
			if (dev_name_begin == "dserver")
				continue;
		}
		j++;
		if (j == 1)
        		ini_log_level = Logging::log4tango_to_tango_level(dev_list[i]->get_logger()->get_level());
		else
		{
        		log_level = Logging::log4tango_to_tango_level(dev_list[i]->get_logger()->get_level());
			if (log_level != ini_log_level)
				return -1;
		}
	}

//
// If only one device (the admin one)
//

	if ((i == 1) && (j == 0))
		return -1;

	lev = ini_log_level;
	return 0;
}

void add_cout_appender(Util *tg)
{
	int i,nb_appenders;
	Tango::DevVarStringArray *log_target;

	try
	{
		log_target = Logging::get_logging_target(tg->get_dserver_device()->get_name());
		nb_appenders = log_target->length();
		if (nb_appenders == 0)
			Logging::add_logging_target(Logging::get_core_logger(),"console::cout",false);
		else
		{
			for (i = 0;i < nb_appenders;i++)
			{
				if (::strcmp((*log_target)[i].in(),"console::cout") == 0)
					break;
			}
			if (i == nb_appenders)
				Logging::add_logging_target(Logging::get_core_logger(),"console::cout",false);
		}
		delete log_target;
	}
	catch(...) {}
}


void all_dev_add_cout(Util *tg)
{
	vector<DeviceImpl *> dev_list;
	int j,nb_appenders;
	Tango::DevVarStringArray *log_target;

	dev_list = tg->get_device_list("*");
	for (unsigned int i = 0;i < dev_list.size();i++)
	{
		string dev_name = dev_list[i]->get_name();
		if (dev_name.size() > 7)
		{
			string dev_name_begin = dev_name.substr(0,7);
			transform(dev_name_begin.begin(),dev_name_begin.end(),dev_name_begin.begin(),::tolower);
			if (dev_name_begin == "dserver")
				continue;
		}
		try
		{
			log_target = Logging::get_logging_target(dev_list[i]->get_name());
			nb_appenders = log_target->length();
			if (nb_appenders == 0)
				Logging::add_logging_target(dev_list[i]->get_logger(),"console::cout",false);
			else
			{
				for (j = 0;j < nb_appenders;j++)
				{
					if (::strcmp((*log_target)[j].in(),"console::cout") == 0)
						break;
				}
				if (j == nb_appenders)
					Logging::add_logging_target(dev_list[i]->get_logger(),"console::cout",false);
			}
			delete log_target;
		}
		catch(...) {}
	}
}

} // End of Tango namespace
