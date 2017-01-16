#include "stdafx.h"
#include "Layeredwnd.h"

CLayeredwnd::CLayeredwnd()
{
	m_pBackSkin	= NULL;	

	m_pSprite = NULL;

	m_nIndex = 0;
}

CLayeredwnd:: ~CLayeredwnd()
{
	if(m_pSprite != NULL)
		delete m_pSprite;
	m_pSprite = NULL;	

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

BEGIN_MESSAGE_MAP(CLayeredwnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL CLayeredwnd::OnEraseBkgnd(CDC* pDC)
{	
	return TRUE;
}

BOOL CLayeredwnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}

void CLayeredwnd::OnMouseMove(UINT nFlags, CPoint point)
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

BOOL CLayeredwnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	SetCursor (LoadCursor (NULL, IDC_ARROW));
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CLayeredwnd::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CLayeredwnd::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		LoadImages();
	}

	CWnd::OnTimer(nIDEvent);
}

BOOL CLayeredwnd::Create(CWnd* pParent)
{
	if (GetSafeHwnd())
		return FALSE;
	
	if (!LoadSkin())	
		return FALSE;
	
	LPCTSTR pszClassName = AfxRegisterWndClass(0, 0, (HBRUSH)GetStockObject(BLACK_BRUSH), 0);//AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	DWORD dwStyleEx = WS_EX_TOOLWINDOW | WS_EX_LAYERED;	
	
	CRect Rect(0, 0, m_pBackSkin->m_pBitmap->GetWidth(), m_pBackSkin->m_pBitmap->GetHeight());
	
	if (!CreateEx(dwStyleEx, pszClassName, NULL, WS_VISIBLE | WS_POPUP, Rect, pParent, 0))
		return FALSE;

	m_pParent = pParent;
	
	return TRUE;
}

void CLayeredwnd::SendMessageToParent()
{
	#define  WM_Layered_CREATE  WM_USER + 11000

	if(m_pParent)
	{
		DWORD dwResult;
		::SendMessageTimeout(m_pParent->m_hWnd, WM_Layered_CREATE, 0, 0, SMTO_NORMAL, 3000, &dwResult );
	}
}

int CLayeredwnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDC* pDC = GetDC();
	DrawSkin(pDC);
	ReleaseDC(pDC);

	//MoveLocationDialog();
    
	LoadImages();

	CWnd::SetTimer(1, 100, NULL);

	return 0;
}

void CLayeredwnd::DrawSkin(CDC* pDC)
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

BOOL CLayeredwnd::LoadSkin()
{	
	LoadImages(TRUE);
	
	m_pBackSkin = new CGdiPlusBitmapResource;
	if(!m_pBackSkin->LoadPath(""))
	{
		if(!m_pBackSkin->Load(MAKEINTRESOURCE(IDB_PNG_SPRITEBOX), _T("PNG"), AfxGetApp()->m_hInstance))
		{
			delete m_pBackSkin;
			m_pBackSkin = NULL;
			return FALSE;
		}
	}	
	
	return TRUE;
}

void CLayeredwnd::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

CString CLayeredwnd::GetModulePath()
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

void CLayeredwnd::LoadImages(BOOL bLoad)
{
	if(bLoad)
	{
		m_SpriteInfo.clear();

		CString strImgPath;
		Sprite spInfo;
		for(int i=1; i < 10; i++)
		{
			strImgPath.Format("%s\\Images\\%d.gif", GetModulePath(), i);
			spInfo.strImgPath = strImgPath;
			m_SpriteInfo.push_back(spInfo);	
		}
	}
	else
	{
		if(m_SpriteInfo.size() == 0) return;

		m_nIndex ++;
		if(m_nIndex > (m_SpriteInfo.size()-1) )
			m_nIndex = 0;

		
		if(m_pSprite == NULL)
			m_pSprite = new CGdiPlusBitmapResource;
		m_pSprite->LoadPath(m_SpriteInfo[m_nIndex].strImgPath);

		Invalidate();
	}
}

void CLayeredwnd::DisplayImg(Graphics& gps)
{
	if(m_pSprite)
	{		
		CRect rc;
		GetClientRect(&rc);

		gps.SetSmoothingMode(SmoothingModeHighQuality);
		gps.SetInterpolationMode(InterpolationModeHighQualityBicubic);
				
		int cx = m_pSprite->m_pBitmap->GetWidth();
		int cy = m_pSprite->m_pBitmap->GetHeight();

		RectF destRect(REAL( (rc.right - cx)/2), REAL((rc.bottom - cy)/2),	REAL(cx), REAL(cy));

		Unit units;
		RectF srcRect;
		m_pSprite->m_pBitmap->GetBounds(&srcRect, &units);
		gps.DrawImage(m_pSprite->m_pBitmap, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel, NULL);
	}

	/*
	USES_CONVERSION;

	CRect rc;
    GetClientRect(&rc);

	gps.SetSmoothingMode(SmoothingModeHighQuality);
	gps.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	
	Image* bitmap = new Image(A2W(m_SpriteInfo[m_nIndex].strImgPath));

	int cx = bitmap->GetWidth();
	int cy = bitmap->GetHeight();

	RectF destRect(REAL( (rc.right - cx)/2), REAL((rc.bottom - cy)/2),	REAL(cx), REAL(cy));

	Unit units;
	RectF srcRect;
	bitmap->GetBounds(&srcRect, &units);
	gps.DrawImage(bitmap, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel, NULL);
	*/
}
