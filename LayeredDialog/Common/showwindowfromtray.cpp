#include "stdafx.h"
#include "ShowWindowFromTray.h"
#include "Shellapi.h"

#pragma warning( disable: 4706 )
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>

enum TaskBarPosition { knTBPNone = 0, knTBPBottom, knTBPTop, knTBPLeft, knTBPRight, knTBPMax };

bool GetDisplayWorkArea( const RECT &rcSource, RECT &rc )
{
	HMONITOR hMon = ::MonitorFromRect( &rcSource, MONITOR_DEFAULTTONULL );
	if( hMon )
	{
		MONITORINFO m = { sizeof( m ) };
		if( ::GetMonitorInfo( hMon, &m ) )
		{
			rc = m.rcWork;
			return true;
		}
	}
	return false;
}

static BOOL GetTaskBarPositionAndMetrics( TaskBarPosition &position, RECT &rcWorkArea )
{
	BOOL bRetVal = FALSE;

	position = knTBPNone;

	APPBARDATA abd ={ sizeof( APPBARDATA ) };
	if( ::SHAppBarMessage( ABM_GETTASKBARPOS, &abd ) )
	{
		const RECT &rcTaskBar = abd.rc;

		if( ::GetDisplayWorkArea( rcTaskBar, rcWorkArea ) )
		{
			const UINT uTaskBarHeight = rcTaskBar.bottom - rcTaskBar.top;
			const UINT uTaskBarWidth = rcTaskBar.right - rcTaskBar.left;

			bRetVal = TRUE;

			if( uTaskBarHeight > uTaskBarWidth )
			{
				if( rcTaskBar.right > rcWorkArea.right )
				{
					position = knTBPRight;
				}
				else
				{
					position = knTBPLeft;
				}
			}
			else
			{
				if( rcTaskBar.bottom > rcWorkArea.bottom )
				{
					position = knTBPBottom;
				}
				else
				{
					position = knTBPTop;
				}
			}
		}
	}
	return bRetVal;
}

void TaskBarSize(int &nLeft, int &nTop) 
{ 
	CRect rc;

	HWND hwndTaskBar = (HWND)FindWindow(_T("Shell_TrayWnd"), NULL); 
	if (hwndTaskBar)
	{
		HWND hWndTray = (HWND)FindWindowEx(hwndTaskBar, NULL, _T("TrayNotifyWnd"), NULL);
		if (hWndTray)
			::GetWindowRect(hWndTray,rc);	
		
		nLeft = rc.bottom - rc.top;
		nTop = rc.right - rc.left ;
 	}
 	else
 	{
 		nLeft = 0;
		nTop = 0;
 	}
}

BOOL ShowWindowFromTrayA( HWND hwnd, UINT uAnimationTimeMS, BOOL bShow, BOOL bRestrictSize)
{
	if( IsWindow( hwnd ) )
	{
		RECT rcWorkArea;
		TaskBarPosition pos;
	
		if( !GetTaskBarPositionAndMetrics( pos, rcWorkArea ) )
		{
			if( bShow )
			{
				::ShowWindow( hwnd, SW_SHOW );
			}
			else
			{
				::ShowWindow( hwnd, SW_HIDE );
			}
			return TRUE;			
		}

		RECT rcWindow;
		POINT ptNewWindowPos = {0};
		VERIFY( ::GetWindowRect( hwnd, &rcWindow ) );
		if( bRestrictSize )
		{
			if( rcWindow.bottom - rcWindow.top > rcWorkArea.bottom - rcWorkArea.top )
			{
				rcWindow.bottom = rcWindow.top + (rcWorkArea.bottom - rcWorkArea.top);
			}
		}
	
		const UINT uWindowWidth = rcWindow.right - rcWindow.left;
		const UINT uWindowHeight = rcWindow.bottom - rcWindow.top;


		int nScreenSizeX = GetSystemMetrics(SM_CXSCREEN);
		int nScreenSizeY = GetSystemMetrics(SM_CYSCREEN);

		int START_Y = nScreenSizeY - uWindowHeight;
		int START_X = nScreenSizeX - uWindowWidth;

		RECT rcWorkAreaA;
		SystemParametersInfo (SPI_GETWORKAREA, 0, &rcWorkAreaA, 0);


		int nType = -1;
		switch( pos )
		{
		case knTBPBottom:
			nType = 0;
			break;

		case knTBPTop:
			nType = 1;
			break;

		case knTBPLeft:
			nType = 2;
			break;

		case knTBPRight:
			nType = 3;
			break;
		}

		if( bShow )
		{
			int nOffSet = 5;

			int nLeft, nTop;
			TaskBarSize(nLeft, nTop); 

			int nX = 0;
			int nY = 0;
		
			if(nType == 0)
			{
				nX = rcWorkAreaA.left + nScreenSizeX -uWindowWidth;
				nY = rcWorkAreaA.top + START_Y - nLeft - nOffSet;
			}
			else if(nType == 1)
			{	
				nX = rcWorkAreaA.left + nScreenSizeX -uWindowWidth;
				nY = rcWorkAreaA.top + START_Y - nLeft - nOffSet;
			}
			else if(nType == 2)
			{
				nX = rcWorkAreaA.left + nScreenSizeX -uWindowWidth - nTop -nOffSet;
				nY = rcWorkAreaA.top + START_Y;			
			}
			else if(nType == 3)
			{
				nX = rcWorkAreaA.left + nScreenSizeX -uWindowWidth - nTop - nOffSet;
				nY = rcWorkAreaA.top + START_Y;				
			}
			
			VERIFY( ::SetWindowPos( hwnd, NULL, nX, nY, uWindowWidth, uWindowHeight, SWP_NOZORDER | SWP_NOACTIVATE ) );
	
		}

		UINT uAnimateFlags = 0;
		if( bShow )
		{
			uAnimateFlags = AW_SLIDE | AW_VER_NEGATIVE;
		}
		else
		{
			uAnimateFlags = AW_SLIDE | AW_HIDE | AW_VER_POSITIVE;
		}

		if( uAnimateFlags )
		{
			if( ::AnimateWindow( hwnd, uAnimationTimeMS, uAnimateFlags ) )
			{
				RedrawWindow( hwnd, NULL, NULL, RDW_ALLCHILDREN | RDW_FRAME | RDW_UPDATENOW | RDW_INVALIDATE ); 
				return TRUE;
			}
		}
		else
		{
			if( bShow )
			{
				::ShowWindow( hwnd, SW_SHOW );
			}
			else
			{
				::ShowWindow( hwnd, SW_HIDE );
			}
			return TRUE;
		}

	}
	else
	{
		//::SetLastErrorEx( ERROR_INVALID_WINDOW_HANDLE, SLE_ERROR );
	}
	return FALSE;
}
