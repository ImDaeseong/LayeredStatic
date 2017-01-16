#include "stdafx.h"
#include "CreateIcons.h"
#include "CreateIconsDlg.h"

CCreateIconsDlg::CCreateIconsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateIconsDlg::IDD, pParent)
{
	m_pBackSkin	= NULL;		
}

void CCreateIconsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCreateIconsDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CCreateIconsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW | WS_EX_LAYERED);

	if(!LoadSkin())
	{
		CDialog::OnCancel();
		return FALSE;
	}

	DragAcceptFiles(TRUE);

	CDC* pDC = GetDC();
	DrawSkin(pDC);
	ReleaseDC(pDC);

	return TRUE;  
}

void CCreateIconsDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawSkin(&dc);
}

void CCreateIconsDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CCreateIconsDlg::OnMouseMove(UINT nFlags, CPoint point)
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

BOOL CCreateIconsDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CCreateIconsDlg::DrawSkin(CDC* pDC)
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

	DisplayStaticText(gps);

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

void CCreateIconsDlg::DisplayStaticText(Graphics& gps)
{
	Gdiplus::SolidBrush  brushText(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::FontFamily  fontFamily(L"굴림");
	Gdiplus::Font        font(&fontFamily, 17, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::PointF      pointF(15.0f, 65.0f);
	gps.DrawString(L"Png 이미지를 드레그 하세요.", -1, &font, pointF, &brushText);
}

BOOL CCreateIconsDlg::LoadSkin()
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

void CCreateIconsDlg::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

void CCreateIconsDlg::OnDropFiles(HDROP hDropInfo)
{	
	CStringArray aryPnglst;
	
	TCHAR szFilePathName[MAX_PATH];
	UINT uFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);  

	for (UINT count = 0; count < uFileCount; count++)
	{	
		DragQueryFile(hDropInfo, count, szFilePathName, _MAX_PATH); 

		if( lstrcmpi(GetFileExtName(szFilePathName), "png") == 0 )
			aryPnglst.Add(szFilePathName);
	}
		
	CPngIcon mIcon;
	CString strPngName = "";
	for(int i=0; i < aryPnglst.GetCount(); i++)
	{		
		strPngName = aryPnglst.GetAt(i);	
		mIcon.CreateIcon(strPngName);
	}
	aryPnglst.RemoveAll();
	

	::ShellExecute(NULL, "open", "", "", NULL, SW_SHOWNORMAL);
	Sleep(1000);
	CDialog::OnOK();


	CDialog::OnDropFiles(hDropInfo);
}

CString CCreateIconsDlg::GetFileExtName(CString strFilename)
{
	int nPos = strFilename.ReverseFind('.');
	if(nPos > 0)
		return strFilename.Mid(nPos + 1);
	return strFilename;
}
