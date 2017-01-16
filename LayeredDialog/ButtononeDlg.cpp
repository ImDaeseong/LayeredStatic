#include "stdafx.h"
#include "LayeredDialog.h"
#include "ButtononeDlg.h"

CButtononeDlg::CButtononeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CButtononeDlg::IDD, pParent)
{
	m_pBackSkin = NULL;
}

void CButtononeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_ALPHA, m_btnAlpha);
}

BEGIN_MESSAGE_MAP(CButtononeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CButtononeDlg::OnInitDialog()
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

void CButtononeDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CButtononeDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CButtononeDlg::OnMouseMove(UINT nFlags, CPoint point)
{
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

BOOL CButtononeDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CButtononeDlg::DrawSkin(CDC* pDC)
{
	if (m_pBackSkin->m_pBitmap == NULL)	
		return;

	Graphics gps(pDC->GetSafeHdc());
	int x = m_pBackSkin->m_pBitmap->GetWidth();
	int y = m_pBackSkin->m_pBitmap->GetHeight();
	gps.DrawImage(m_pBackSkin->m_pBitmap, Rect(0, 0, x, y) , 0, 0, x, y, UnitPixel);
}

BOOL CButtononeDlg::LoadSkin()
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

void CButtononeDlg::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

void CButtononeDlg::InitControl()
{
	CRect rClient;
	GetClientRect(&rClient);

	CString strImgPath = "";

	if(!PathFileExists(strImgPath))
		m_btnClose.SetImage(MAKEINTRESOURCE(IDB_PNG_CLOSE), "PNG", AfxGetApp()->m_hInstance, rClient.right - 30, rClient.top + 5);
	else
		m_btnClose.SetImagePath(strImgPath, rClient.right - 30, rClient.top + 5);
    m_btnClose.SetBkColor(RGB(210, 210, 210));


	if(!PathFileExists(strImgPath))
		m_btnAlpha.SetImage(MAKEINTRESOURCE(IDB_PNGPLAY), "PNG", AfxGetApp()->m_hInstance, rClient.left + 20, rClient.top + 50);
	else
		m_btnAlpha.SetImagePath(strImgPath, rClient.left + 20, rClient.top + 50);
    m_btnAlpha.SetBkColor(RGB(199, 199, 199));
	
}
