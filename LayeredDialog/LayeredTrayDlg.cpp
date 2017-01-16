#include "stdafx.h"
#include "LayeredDialog.h"
#include "LayeredTrayDlg.h"
#include "LayeredTraySubDlg.h"

CLayeredTrayDlg::CLayeredTrayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLayeredTrayDlg::IDD, pParent)
{
	m_pBackSkin	= NULL;	
	m_bMoveDialog = false;

	m_pLayeredTraySubDlg = NULL;
}

void CLayeredTrayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLayeredTrayDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_MOVE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
END_MESSAGE_MAP()

BOOL CLayeredTrayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW | WS_EX_LAYERED);

	if(::GetForegroundWindow() != m_hWnd)
	{ 
		HWND h_active_wnd = ::GetForegroundWindow(); 
		if(h_active_wnd != NULL)
		{ 
			DWORD thread_id = GetWindowThreadProcessId(h_active_wnd, NULL); 
			DWORD current_thread_id = GetCurrentThreadId(); 
			if(current_thread_id != thread_id)
			{ 
				if(AttachThreadInput(current_thread_id, thread_id, TRUE))
				{ 
					BringWindowToTop(); 
					AttachThreadInput(current_thread_id, thread_id, FALSE); 
				} 
			} 
		} 
	} 

	if(!LoadSkin())
	{
		CDialog::OnCancel();
		return FALSE;
	}

	InitControl();

	CDC* pDC = GetDC();
	DrawSkin(pDC);
	ReleaseDC(pDC);
	
	return TRUE;  
}

void CLayeredTrayDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CLayeredTrayDlg::OnDestroy()
{
	CDialog::OnDestroy();

	DestroyLayeredTraySubDlg();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CLayeredTrayDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bMoveDialog) return;

	if (nFlags & MK_LBUTTON)
	{	
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE,  0,0);
	}
	else
	{
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE,  0,0);
	}	

	CDialog::OnMouseMove(nFlags, point);
}

BOOL CLayeredTrayDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CLayeredTrayDlg::DrawSkin(CDC* pDC)
{
	CDC memDC;
	CBitmap bmp;
	CBitmap* bmp_old;

	CRect rect;
	GetWindowRect(&rect);

	int cx = rect.Width();
	int cy = rect.Height();

	memDC.CreateCompatibleDC(pDC);

	BITMAPINFO bmInfo;
	memset(&bmInfo, 0x00, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = cx;
	bmInfo.bmiHeader.biHeight = cy;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biSizeImage = 0;
	bmInfo.bmiHeader.biClrUsed = 0;
	bmInfo.bmiHeader.biClrImportant = 0;

	LPBYTE pbmpBits = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(pDC->GetSafeHdc(), (LPBITMAPINFO)&bmInfo, DIB_RGB_COLORS, (void **)&pbmpBits, NULL, 0);
	if(hBitmap == NULL)
		bmp.CreateCompatibleBitmap(pDC, cx, cy);
	else
		bmp.Attach(hBitmap);

	bmp_old = memDC.SelectObject(&bmp);
	Graphics gps(memDC.GetSafeHdc());

	gps.DrawImage(
		m_pBackSkin->m_pBitmap, 
		Rect(0, 0, m_pBackSkin->m_pBitmap->GetWidth(), m_pBackSkin->m_pBitmap->GetHeight()), 
		0,
		0, 
		m_pBackSkin->m_pBitmap->GetWidth(), 
		m_pBackSkin->m_pBitmap->GetHeight(),
		UnitPixel);

	CPoint ptWindowScreenPosition(rect.TopLeft());
	CSize szWindow(cx, cy);

	BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	CPoint ptSrc(0,0);

	HDC hdc = gps.GetHDC();

	BOOL bRet= ::UpdateLayeredWindow(GetSafeHwnd(), 
		pDC->GetSafeHdc(),
		&ptWindowScreenPosition,
		&szWindow,
		hdc,
		&ptSrc,
		0, 
		&blendPixelFunction,
		ULW_ALPHA);

	gps.ReleaseHDC(hdc);

	memDC.SelectObject(bmp_old);
	bmp.DeleteObject();
	memDC.DeleteDC();
}

BOOL CLayeredTrayDlg::LoadSkin()
{
	m_pBackSkin = new CGdiPlusBitmapResource;
	if(!m_pBackSkin->LoadPath(""))
	{
		if(!m_pBackSkin->Load(MAKEINTRESOURCE(IDB_PNG_BG), _T("PNG"), AfxGetApp()->m_hInstance))
		{
			delete m_pBackSkin;
			m_pBackSkin = NULL;
			return FALSE;
		}
	}
		
	MoveLocationDialog();
	
	return TRUE;
}

void CLayeredTrayDlg::MoveLocationDialog(BOOL bShow)
{		
	if(bShow)
	{
		int cx = m_pBackSkin->m_pBitmap->GetWidth();
		int cy = m_pBackSkin->m_pBitmap->GetHeight();

		RECT rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
		MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
	}
	else
	{
		MoveWindow(-1000, -1000, 0, 0);
	}
}

void CLayeredTrayDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		MoveLocationDialog(true);
		
		::ShowWindowFromTrayA(this->m_hWnd, 1000, TRUE, TRUE);

		CreateLayeredTraySubDlg();
		
		KillTimer(1);
		SetTimer(2, 2000, NULL);
	}
	else if(nIDEvent == 2)
	{
		m_pLayeredTraySubDlg->ShowWindow(SW_HIDE);//::ShowWindowFromTrayA( m_pLayeredTraySubDlg->m_hWnd, 500, FALSE, TRUE);
		::ShowWindowFromTrayA( this->m_hWnd, 500, FALSE, TRUE);
	
		KillTimer(2);
		CDialog::OnOK();
	}

	CDialog::OnTimer(nIDEvent);
}

void CLayeredTrayDlg::InitControl()
{	
	SetTimer(1, 1000, NULL);
}

void CLayeredTrayDlg::CreateLayeredTraySubDlg()
{	
	if (!m_pLayeredTraySubDlg)
	{
		m_pLayeredTraySubDlg = new CLayeredTraySubDlg();
		m_pLayeredTraySubDlg->SetParent(this);	
		m_pLayeredTraySubDlg->Create(IDD_LAYERED_TRAY_SUBDIALOG, this);	
		SetChildPos();
		m_pLayeredTraySubDlg->ShowWindow(SW_SHOW);
	}
}

void CLayeredTrayDlg::DestroyLayeredTraySubDlg()
{
	if(m_pLayeredTraySubDlg)
	{
		m_pLayeredTraySubDlg->DestroyWindow();
		delete m_pLayeredTraySubDlg;
		m_pLayeredTraySubDlg = NULL;
	}
}

void CLayeredTrayDlg::SetChildPos()
{
	if(m_pLayeredTraySubDlg)
	{		
		CRect rInfoWnd;
		GetWindowRect(rInfoWnd);				
		m_pLayeredTraySubDlg->MoveWindow(rInfoWnd.left, rInfoWnd.top, m_pLayeredTraySubDlg->GetWidth(), m_pLayeredTraySubDlg->GetHeight() );
	}
}

LRESULT CLayeredTrayDlg::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	SetChildPos();
	return 0;
}

void CLayeredTrayDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	SetChildPos();	
}