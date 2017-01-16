#include "stdafx.h"
#include "SpriteWnd.h"

IMPLEMENT_DYNAMIC(CSpriteWnd, CWnd)

CSpriteWnd::CSpriteWnd()
{
	m_pBackSkin	= NULL;	

	m_pSprite = NULL;

	m_curIndex = 0;
	m_TotalIndex = 0;
}

CSpriteWnd::~CSpriteWnd()
{
	if(m_pSprite != NULL)
		delete m_pSprite;
	m_pSprite = NULL;	

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

BEGIN_MESSAGE_MAP(CSpriteWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CSpriteWnd::OnEraseBkgnd(CDC* pDC)
{	
	return TRUE;
}

BOOL CSpriteWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}

void CSpriteWnd::OnMouseMove(UINT nFlags, CPoint point)
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

void CSpriteWnd::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CSpriteWnd::OnTimer(UINT_PTR nIDEvent)
{	
	if(nIDEvent == 1)
	{
		if (m_TotalIndex > 0)
		{
			m_curIndex++;
			if (m_curIndex >= m_TotalIndex)
				m_curIndex = 0;
			Invalidate(FALSE);
		}
	}

	CWnd::OnTimer(nIDEvent);
}

BOOL CSpriteWnd::Create(CWnd* pParent)
{
	if (GetSafeHwnd())
		return FALSE;
	
	if (!LoadSkin())	
		return FALSE;
	
	LPCTSTR pszClassName = AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	DWORD dwStyleEx = WS_EX_TOOLWINDOW | WS_EX_LAYERED;	
	
	CRect Rect(0, 0, m_pBackSkin->m_pBitmap->GetWidth(), m_pBackSkin->m_pBitmap->GetHeight());
	
	if (!CreateEx(dwStyleEx, pszClassName, NULL, WS_VISIBLE | WS_POPUP, Rect, pParent, 0))
		return FALSE;

	CDC* pDC = GetDC();
	DrawSkin(pDC);
	ReleaseDC(pDC);

	Start();

	return TRUE;
}

void CSpriteWnd::DrawSkin(CDC* pDC)
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

	DrawSprite(gps);

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

BOOL CSpriteWnd::LoadSkin()
{	
	m_pBackSkin = new CGdiPlusBitmapResource;
	if(!m_pBackSkin->LoadPath(""))
	{
		if(!m_pBackSkin->Load(MAKEINTRESOURCE(IDB_PNG_SPRITEBOX), _T("PNG"), AfxGetApp()->m_hInstance))
		{
			delete m_pBackSkin;
			m_pBackSkin = NULL;
			return FALSE;
		}

		LoadSpriteImage(); 
	}
		
	//MoveLocationDialog();
	
	return TRUE;
}

void CSpriteWnd::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

CString CSpriteWnd::GetModulePath()
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

void CSpriteWnd::Start()
{
	Stop();
	
	if (m_TotalIndex > 0)
		m_curIndex = 0;
	
	SetTimer(1, 40, NULL);
}

void CSpriteWnd::Stop()
{	
	KillTimer(1);
	m_curIndex = 0;
	Invalidate(FALSE);
}

void CSpriteWnd::LoadSpriteImage() 
{
	USES_CONVERSION;

	CString strImgPath;
	strImgPath.Format("%s\\Images\\sprite.png", GetModulePath());
	//strImgPath.Format("%s\\Images\\explode.png", GetModulePath());
	
	if(m_pSprite == NULL)
		m_pSprite = new CGdiPlusBitmapResource;
	m_pSprite->LoadPath(strImgPath);
			
	m_TotalIndex = 14;		
	//m_TotalIndex = 16;	
}

void CSpriteWnd::DrawSprite(Graphics& gps)
{
	if(m_pSprite)
	{	
		int nImgWidth = 110;
		int nImgHeight = 101;

		int x = (m_curIndex % 5 ) * nImgWidth;
		int y = (m_curIndex / 5 ) * nImgHeight;

		//window size
		CRect rc;
		GetClientRect(&rc);
		Rect wndRect(((rc.right - nImgWidth)/2), ((rc.bottom - nImgHeight)/2),	nImgWidth, nImgHeight);

		//image size
		Rect ImgRect( x, y, nImgWidth, nImgWidth);
		gps.DrawImage(m_pSprite->m_pBitmap, wndRect, ImgRect.X, ImgRect.Y, ImgRect.Width, ImgRect.Height, UnitPixel);
	}
	/*
	if(m_pSprite)
	{	
		int nImgWidth = 64;
		int nImgHeight = 64;

		int x = (m_curIndex % 4 ) * nImgWidth;
		int y = (m_curIndex / 4 ) * nImgHeight;

		//window size
		CRect rc;
		GetClientRect(&rc);
		Rect wndRect(((rc.right - nImgWidth)/2), ((rc.bottom - nImgHeight)/2),	nImgWidth, nImgHeight);

		//image size
		Rect ImgRect( x, y, nImgWidth, nImgWidth);
		gps.DrawImage(m_pSprite->m_pBitmap, wndRect, ImgRect.X, ImgRect.Y, ImgRect.Width, ImgRect.Height, UnitPixel);
	}
	*/	
}