#include "stdafx.h"
#include "Layeredwnd.h"

CLayeredwndEx::CLayeredwndEx()
{
	m_pBackSkin	= NULL;	

	m_pSprite = NULL;

	m_nIndex = 0;
}

CLayeredwndEx:: ~CLayeredwndEx()
{
	if(m_pSprite != NULL)
		delete m_pSprite;
	m_pSprite = NULL;	

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

BEGIN_MESSAGE_MAP(CLayeredwndEx, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL CLayeredwndEx::OnEraseBkgnd(CDC* pDC)
{	
	return TRUE;
}

BOOL CLayeredwndEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}

void CLayeredwndEx::OnMouseMove(UINT nFlags, CPoint point)
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
	CWnd::OnMouseMove(nFlags, point);
}

BOOL CLayeredwndEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	SetCursor (LoadCursor (NULL, IDC_ARROW));
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CLayeredwndEx::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CLayeredwndEx::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		LoadImages();
	}

	CWnd::OnTimer(nIDEvent);
}

BOOL CLayeredwndEx::Create(CWnd* pParent)
{
	if (GetSafeHwnd())
		return FALSE;
	
	if (!LoadSkin())	
		return FALSE;
	
	LPCTSTR pszClassName = AfxRegisterWndClass(0, 0, (HBRUSH)GetStockObject(BLACK_BRUSH), 0);
	DWORD dwStyleEx = WS_EX_TOOLWINDOW | WS_EX_LAYERED;	
	
	CRect Rect(0, 0, m_pBackSkin->m_pBitmap->GetWidth(), m_pBackSkin->m_pBitmap->GetHeight());
	
	if (!CreateEx(dwStyleEx, pszClassName, NULL, WS_VISIBLE | WS_POPUP, Rect, pParent, 0))
		return FALSE;

	return TRUE;
}

int CLayeredwndEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDC* pDC = GetDC();
	DrawSkin(pDC);
	ReleaseDC(pDC);

	//MoveLocationDialog();

	LoadImages();

	SetTimer(1, 100, NULL);

	return 0;
}

void CLayeredwndEx::DrawSkin(CDC* pDC)
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

	DisplayImg(gps);

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

BOOL CLayeredwndEx::LoadSkin()
{	
	LoadImages(TRUE);
		
	return TRUE;
}

void CLayeredwndEx::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

CString CLayeredwndEx::GetModulePath()
{
	char cTemp[MAX_PATH];
	char *spzRealDirEnd;
	CString strModulePath;

	GetModuleFileName(NULL, cTemp, MAX_PATH);  
	spzRealDirEnd = strrchr(cTemp, '\\');
	*spzRealDirEnd = '\0';
	strModulePath = (CString)cTemp;
	return strModulePath;
}

void CLayeredwndEx::LoadImages(BOOL bLoad)
{
	if(bLoad)
	{
		m_SpriteInfo.RemoveAll();

		CString strImgPath;
		for(int i=1; i < 10; i++)
		{
			strImgPath.Format("%s\\Images\\%d.gif", GetModulePath(), i);
			m_SpriteInfo.Add(strImgPath);	
		}

		if(m_pBackSkin == NULL)
			m_pBackSkin = new CGdiPlusBitmapResource;
		m_pBackSkin->LoadPath(strImgPath);
	}
	else
	{
		if(m_SpriteInfo.GetCount() == 0) return;

		m_nIndex ++;
		if(m_nIndex > (m_SpriteInfo.GetCount()-1) )
			m_nIndex = 0;

		if(m_pBackSkin == NULL)
			m_pBackSkin = new CGdiPlusBitmapResource;
		m_pBackSkin->LoadPath(m_SpriteInfo.GetAt(m_nIndex));


		if(m_pSprite == NULL)
			m_pSprite = new CGdiPlusBitmapResource;
		m_pSprite->LoadPath(m_SpriteInfo.GetAt(m_nIndex));
		
		Invalidate();
	}
}

void CLayeredwndEx::DisplayImg(Graphics& gps)
{
	if(m_pSprite)
	{		
		CRect rc;
		GetClientRect(&rc);

		gps.SetSmoothingMode(SmoothingModeHighQuality);
		gps.SetInterpolationMode(InterpolationModeHighQualityBicubic);
				
		int cx = m_pSprite->m_pBitmap->GetWidth();
		int cy = m_pSprite->m_pBitmap->GetHeight();

		RectF destRect(REAL((rc.right - cx)), REAL((rc.bottom - cy)), REAL(cx/3), REAL(cy/3));

		Unit units;
		RectF srcRect;
		m_pSprite->m_pBitmap->GetBounds(&srcRect, &units);
		gps.DrawImage(m_pSprite->m_pBitmap, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel, NULL);
	}
}