#include "stdafx.h"
#include "LayeredDialog.h"
#include "SpriteDlg.h"

CSpriteDlg::CSpriteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpriteDlg::IDD, pParent)
{
	m_pBackSkin	= NULL;	

	m_pLayeredWnd = NULL;

	m_pLayeredWndEx = NULL;

	m_pSpriteLayeredWnd = NULL;
}

void CSpriteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SPRITE, m_StaticSprite);
}

BEGIN_MESSAGE_MAP(CSpriteDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CSpriteDlg::OnInitDialog()
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

void CSpriteDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CSpriteDlg::OnDestroy()
{
	CDialog::OnDestroy();

	KillTimer(1);

	DestoryLayerWnd();
	DestoryLayerWndEx();
	DestorySpriteLayerWnd();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CSpriteDlg::OnMouseMove(UINT nFlags, CPoint point)
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

BOOL CSpriteDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CSpriteDlg::DrawSkin(CDC* pDC)
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

	if(m_StaticSprite.IsAvailableDraw())
		m_StaticSprite.OnDrawLayerdWindow(gps);


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

BOOL CSpriteDlg::LoadSkin()
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

void CSpriteDlg::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

void CSpriteDlg::InitControl()
{		
	SetTimer(1, 100, NULL);

	m_StaticSprite.SetImagePath("sprite.png");
	m_StaticSprite.SetSprite(14, 110, 101);
	m_StaticSprite.SetRowCol(5, 5);
	m_StaticSprite.SetSpriteStaticClick(true);
	m_StaticSprite.SetSpriteStaticCursor(true);
}

void CSpriteDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		KillTimer(1);	
		CreateLayerWnd();
		CreateLayerWndEx();
		CreateSpriteLayerWnd();
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CSpriteDlg::CreateLayerWnd()
{
	if( m_pLayeredWnd == NULL )
	{
		m_pLayeredWnd = new CLayeredwnd();
		if(!m_pLayeredWnd->Create(this))
		{
			delete m_pLayeredWnd;
			m_pLayeredWnd = NULL;
			return FALSE;
		}

		CRect rWnd;
		GetWindowRect(&rWnd);
		m_pLayeredWnd->MoveWindow(200, 20, rWnd.Width(), rWnd.Height());//m_pLayeredWnd->MoveWindow(&rWnd);
	}
	return TRUE;
}

void CSpriteDlg::DestoryLayerWnd()
{
	if(m_pLayeredWnd != NULL)
		delete m_pLayeredWnd;
	m_pLayeredWnd = NULL;
}

BOOL CSpriteDlg::CreateLayerWndEx()
{
	if( m_pLayeredWndEx == NULL )
	{
		m_pLayeredWndEx = new CLayeredwndEx();
		if(!m_pLayeredWndEx->Create(this))
		{
			delete m_pLayeredWndEx;
			m_pLayeredWndEx = NULL;
			return FALSE;
		}

		CRect rWnd;
		GetWindowRect(&rWnd);
		m_pLayeredWndEx->MoveWindow(500, 20, rWnd.Width(), rWnd.Height());//m_pLayeredWndEx->MoveWindow(&rWnd);
	}
	return TRUE;
}

void CSpriteDlg::DestoryLayerWndEx()
{
	if(m_pLayeredWndEx != NULL)
		delete m_pLayeredWndEx;
	m_pLayeredWndEx = NULL;
}

BOOL CSpriteDlg::CreateSpriteLayerWnd()
{
	if( m_pSpriteLayeredWnd == NULL )
	{
		m_pSpriteLayeredWnd = new CSpriteWnd();
		if(!m_pSpriteLayeredWnd->Create(this))
		{
			delete m_pSpriteLayeredWnd;
			m_pSpriteLayeredWnd = NULL;
			return FALSE;
		}

		CRect rWnd;
		GetWindowRect(&rWnd);
		m_pSpriteLayeredWnd->MoveWindow(800, 20, rWnd.Width(), rWnd.Height());//m_pSpriteLayeredWnd->MoveWindow(&rWnd);
	}
	return TRUE;
}

void CSpriteDlg::DestorySpriteLayerWnd()
{
	if(m_pSpriteLayeredWnd != NULL)
		delete m_pSpriteLayeredWnd;
	m_pSpriteLayeredWnd = NULL;
}