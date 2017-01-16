#include "stdafx.h"
#include "LayeredDialog.h"
#include "LayeredTraySubDlg.h"

CLayeredTraySubDlg::CLayeredTraySubDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLayeredTraySubDlg::IDD, pParent)
{
	m_pBackSkin = NULL;
}

void CLayeredTraySubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER_TRAY, m_WebBrowser);	
}

BEGIN_MESSAGE_MAP(CLayeredTraySubDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CLayeredTraySubDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW, 0);
	
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

void CLayeredTraySubDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CLayeredTraySubDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

BOOL CLayeredTraySubDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CLayeredTraySubDlg::DrawSkin(CDC* pDC)
{
	if (m_pBackSkin->m_pBitmap == NULL)	
		return;

	Graphics gps(pDC->GetSafeHdc());
	int x = m_pBackSkin->m_pBitmap->GetWidth();
	int y = m_pBackSkin->m_pBitmap->GetHeight();
	gps.DrawImage(m_pBackSkin->m_pBitmap, Rect(0, 0, x, y) , 0, 0, x, y, UnitPixel);
}

BOOL CLayeredTraySubDlg::LoadSkin()
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

void CLayeredTraySubDlg::MoveLocationDialog()
{		
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

void CLayeredTraySubDlg::InitControl()
{	
	CRect rClient;
	GetClientRect(&rClient);
	m_WebBrowser.MoveWindow(rClient.left + 10, rClient.top + 10 , rClient.Width() - 20, rClient.Height() - 20 );
	Navigate("http://www.naver.com");
}

void CLayeredTraySubDlg::Navigate(CString strUrl)
{
    COleVariant vtEmpty;
    m_WebBrowser.Navigate(strUrl, &vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty);
}

void CLayeredTraySubDlg::SetParent(CLayeredTrayDlg* pParent)
{
	m_pParent = pParent;
}

int CLayeredTraySubDlg::GetWidth()
{
	return m_pBackSkin->m_pBitmap->GetWidth();
}

int CLayeredTraySubDlg::GetHeight()
{
	return m_pBackSkin->m_pBitmap->GetHeight();
}
