#include "stdafx.h"
#include "LayeredDialog.h"
#include "LayeredButtonDlg.h"

CLayeredButtonDlg::CLayeredButtonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLayeredButtonDlg::IDD, pParent)
{
	m_pBackSkin	= NULL;	
}

void CLayeredButtonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
}

BEGIN_MESSAGE_MAP(CLayeredButtonDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CLayeredButtonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW | WS_EX_LAYERED);

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

void CLayeredButtonDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CLayeredButtonDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CLayeredButtonDlg::OnMouseMove(UINT nFlags, CPoint point)
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

BOOL CLayeredButtonDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CLayeredButtonDlg::DrawSkin(CDC* pDC)
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


	if(m_btnClose.IsAvailableDraw())
		m_btnClose.OnDrawLayerdWindow(gps);


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

BOOL CLayeredButtonDlg::LoadSkin()
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

void CLayeredButtonDlg::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

void CLayeredButtonDlg::InitControl()
{
	CRect rClient;
	GetClientRect(&rClient);

	CString strImgPath = "";

	if(!PathFileExists(strImgPath))
		m_btnClose.SetImage(MAKEINTRESOURCE(IDB_PNG_CLOSE), "PNG", AfxGetApp()->m_hInstance, rClient.right - 30, rClient.top + 5, 1);
	else
		m_btnClose.SetImagePath(strImgPath, rClient.right - 30, rClient.top + 5, 1);   
}

