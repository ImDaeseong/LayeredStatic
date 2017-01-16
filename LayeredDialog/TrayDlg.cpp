#include "stdafx.h"
#include "LayeredDialog.h"
#include "TrayDlg.h"

CTrayDlg::CTrayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrayDlg::IDD, pParent)
{
	m_pBackSkin = NULL;
	m_bMoveDialog = false;
}

void CTrayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER_TRAY, m_WebBrowser);	
}

BEGIN_MESSAGE_MAP(CTrayDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CTrayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW, 0);

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

	CDC* pDC = GetDC();
	if(pDC)
	{
		DrawSkin(pDC);
		ReleaseDC(pDC);
	}
		 
	InitControl();

	return TRUE;  
}

void CTrayDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CTrayDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CTrayDlg::OnMouseMove(UINT nFlags, CPoint point)
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

BOOL CTrayDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CTrayDlg::DrawSkin(CDC* pDC)
{
	if (m_pBackSkin->m_pBitmap == NULL)	
		return;

	Graphics gps(pDC->GetSafeHdc());
	int x = m_pBackSkin->m_pBitmap->GetWidth();
	int y = m_pBackSkin->m_pBitmap->GetHeight();
	gps.DrawImage(m_pBackSkin->m_pBitmap, Rect(0, 0, x, y) , 0, 0, x, y, UnitPixel);
}

BOOL CTrayDlg::LoadSkin()
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

void CTrayDlg::MoveLocationDialog(BOOL bShow)
{		
	if(bShow)
	{
		int cx = m_pBackSkin->m_pBitmap->GetWidth();
		int cy = m_pBackSkin->m_pBitmap->GetHeight();

		RECT rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
		MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);


		CRect rClient;
		GetClientRect(&rClient);
		m_WebBrowser.MoveWindow(rClient.left + 10, rClient.top + 10 , rClient.Width() - 20, rClient.Height() - 20 );
	}
	else
	{
		MoveWindow(-1000, -1000, 0, 0);
		m_WebBrowser.MoveWindow(-1000, -1000, 0, 0);
	}
}

void CTrayDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		MoveLocationDialog(true);
		::ShowWindowFromTrayA( this->m_hWnd, 1000, TRUE, TRUE);

		KillTimer(1);
		SetTimer(2, 2000, NULL);
	}
	else if(nIDEvent == 2)
	{
		::ShowWindowFromTrayA( this->m_hWnd, 500, FALSE, TRUE);
	
		KillTimer(2);
		CDialog::OnOK();
	}

	CDialog::OnTimer(nIDEvent);
}

void CTrayDlg::InitControl()
{	
	Navigate("http://www.naver.com");

	SetTimer(1, 1000, NULL);
}

void CTrayDlg::Navigate(CString strUrl)
{
    COleVariant vtEmpty;
    m_WebBrowser.Navigate(strUrl, &vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty);
}