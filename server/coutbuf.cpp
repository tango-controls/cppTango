static const char *RcsId = "$Header$";

//+=============================================================================
//
// file :               coutbuf.cpp
//
// description :        C++ source for the coutbuf class. This class is used
//                      only for Windows device server for the debug 
//                      outputs
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 2.7  2002/12/16 10:15:36  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:06  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:19  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:53  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:24  taurel
// Miscellaneous small changes/bug fixes for Tango CPP release 2.1.0
//     - classes reference documentation now generated using doxygen instead of doc++
//     - A little file added to the library which summarizes version number.
//       The RCS/CVS "ident" command will now tells you that release library x.y.z is composed
//       by C++ client classes set release a.b and C++ server classes set release c.d
//     - Fix incorrect field setting for DevFailed exception re-thrown from a CORBA exception
//     - It's now not possible to poll the Init command
//     - It's now possible to define a default class doc. per control system
//       instance (using property)
//     - The test done to check if attribute value has been set before it is
//       returned to caller is done only if the attribute quality is set to VALID
//     - The JTCInitialize object is now stored in the Util
//     - Windows specific : The tango.h file now also include winsock.h
//
// Revision 2.2  2002/04/30 10:50:40  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:03  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:09  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:12  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:10  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:13  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:44  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:15  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-=============================================================================

#include <coutbuf.h>
#include <winnt_lib/tango_static/resource.h>
#include <commctrl.h>

namespace Tango
{

static HINSTANCE hAppInst;
static HWND hWndDebug;
static CRITICAL_SECTION cs;
static HWND hWndDebugList = NULL;

//+----------------------------------------------------------------------------
//
// method : 		CoutBuf::CoutBuf()
// 
// description : 	Constructor of the CoutBuf class. It mainly creates
//                      the debug output window
//
// in :			- hInstance : The application instance
//			- nCmdShow : 
//                      - svc_name : The device server name
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

        wc.lpszClassName    = "Debug";
        wc.style            = CS_HREDRAW | CS_VREDRAW;
        wc.hCursor          = ::LoadCursor(NULL, IDC_ARROW);
        wc.hIcon            = ::LoadIcon(NULL,IDI_APPLICATION);
        wc.lpszMenuName     = NULL;
        wc.hbrBackground    = ::CreateSolidBrush(RGB(255,255,255));
        wc.hInstance        = hInstance;
        wc.lpfnWndProc      = DebugWndProc;
        wc.cbClsExtra       = 0;
        wc.cbWndExtra       = 0;
    
        RegisterClass(&wc);

//
// create the debug console
//

	parent_window = parent;
	CreateWin(svc_name);

//
// Create a Windows mutex
//

	InitializeCriticalSection(&cs);
	nb_critical = 0;

//
// Show the main window
//

	DbgWin = hWndDebug;
//	ShowWindow(hWndDebug,nCmdShow);
//	UpdateWindow(hWndDebug);

//
// Set streambuf pointers
//

        setp(buffer,buffer + (bufferSize - 2));

}


CoutBuf::~CoutBuf()
{
        sync();
}

void CoutBuf::CreateWin(LPCSTR svc_name)
{
	char buf[256];
	

	sprintf(buf,"%s - Console",svc_name);
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
                Except::throw_exception((LPCSTR)"API_NtDebugWindowError",
				      (LPCSTR)"Can't create debug window",
                                      (LPCSTR)"CoutBuf::CoutBuf");
	}
	DbgWin = hWndDebug;
}

//+----------------------------------------------------------------------------
//
// method : 		CoutBuf::dbg_out()
// 
// description : 	This method add a string to the end of the debug list
//
// in : 		- buf : The string to be printed
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

        i = (int) SendMessage(hWndDebugList,LB_ADDSTRING,(WPARAM)0,(LPARAM)(LPSTR)buf);
        SendMessage(hWndDebugList,LB_SETCURSEL,(WPARAM)i,(LPARAM)0);

//
// Enable the repaint now
//

        SendMessage(hWndDebugList,WM_SETREDRAW,(WPARAM)TRUE,(LPARAM)0);

        return 0;
}

//+----------------------------------------------------------------------------
//
// method : 		CoutBuf::overflow()
// 
// description : 	Overriding of the streambuf::overflow method
//                      This method prints a single char at a time
//                      This method has been copied from the std C++ library
//                      book from Josuttis (p 674)
//
// in : 		- c : The charater to be printed
//
//-----------------------------------------------------------------------------

streambuf::int_type CoutBuf::overflow(streambuf::int_type c)
{
        if (c != EOF)
        {
		EnterCriticalSection(&cs);
		nb_critical++;
                *pptr() = c;
                pbump(1);
        }

        if (flushBuffer() == EOF)
                return EOF;
                
        return c;
}

streamsize CoutBuf::xsputn(const char_type *s,streamsize n)
{

//
// Enter the critical section and count how many times we enter it
//

	EnterCriticalSection(&cs);
	nb_critical++;

//
// Count hhow many characters we can copy into the stream buffer
//

	int nb_place = epptr() - pptr();
	int start = pptr() - pbase();
	int nb_to_write,i;
	
	if (n <= nb_place)
		nb_to_write = n;
	else
		nb_to_write = nb_place;

//
// Copy data into stream buffer
//

	for (i = 0;i < nb_to_write;i++)
	{
		buffer[start] = s[i];
		start++;
	}
	pbump(nb_to_write);
	return nb_to_write;			
}

//+----------------------------------------------------------------------------
//
// method : 		CoutBuf::flushBuffer()
// 
// description : 	This method flushs (send data to the output window)
//                      characters buffer
//                      This method has been copied from the std C++ library
//                      book from Josuttis (p 674)
//
//-----------------------------------------------------------------------------

int CoutBuf::flushBuffer()
{

//
// Replace the last windows chracter (0xa) by the famous 0
//

        int num = pptr() - pbase();
	if (buffer[num - 1] == 0xa)
		buffer[num - 1] = '\0';
	else
        	buffer[num] = '\0';

//
// Send buffer to output window
//

	if (DbgWin != NULL)
	{
		if (dbg_out((LPCSTR)buffer) == -1)
			return EOF;
	}

        pbump(-num);

//
// Leave ctitical section as often as we enter it
//

	long loop = nb_critical;
	nb_critical = 0;
	for (int i = 0;i < loop;i++)
		LeaveCriticalSection(&cs);

        return num;
}


//+----------------------------------------------------------------------------
//
// method : 		CoutBuf::sync()
// 
// description : 	Overriding of the streambuf::sync method
//                      This method synchronizes data with the output window
//                      This method has been copied from the std C++ library
//                      book from Josuttis (p 674)
//
//-----------------------------------------------------------------------------

int CoutBuf::sync()
{
        if (flushBuffer() == EOF)
                return -1;

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
                                       WS_CHILD | WS_VISIBLE | WS_VSCROLL
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
        HDC hDC;


        hDC = GetDC(hWnd);
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
//		SetBkMode(hDC,TRANSPARENT);    
                SendMessage(lpDI->hwndItem,LB_GETTEXT,lpDI->itemID,(LPARAM)(LPSTR)buf);

                ExtTextOut(hDC,rc.left+2, rc.top,ETO_CLIPPED,&rc,buf,lstrlen(buf),NULL);
    
                break;
    
        }
}

} // End of Tango namespace
