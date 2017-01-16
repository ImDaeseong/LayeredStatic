#include "stdafx.h"
#include "SpriteStatic.h"

CSpriteStatic::CSpriteStatic()
{
	m_pSprite = NULL;
	m_bClick = FALSE;
	m_bCursor = FALSE;

	m_curIndex = 0;
	m_TotalIndex = 0;
	m_nDelayTime = 40;
	m_nImgWidth = 0;
	m_nImgHeight = 0;

	m_crBG = RGB(255, 255, 255);

	m_nRow = 0;
	m_nCol = 0;
}

CSpriteStatic::~CSpriteStatic()
{
	if(m_pSprite != NULL)
		delete m_pSprite;
	m_pSprite = NULL;
}

BEGIN_MESSAGE_MAP(CSpriteStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CSpriteStatic::PreSubclassWindow() 
{
	CStatic::PreSubclassWindow();
    
	SetTimer(1, m_nDelayTime, NULL);
}

CSpriteStatic& CSpriteStatic::SetSpriteStaticClick(BOOL bClick)
{
	m_bClick = bClick;

	if (bClick)
		ModifyStyle(0,SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY,0);
	return *this;
}

void CSpriteStatic::SetSpriteStaticCursor(BOOL bCursor)
{
	m_bCursor = bCursor;	
}

BOOL CSpriteStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(m_bCursor)
	{		
		::SetCursor(::LoadCursor(0, MAKEINTRESOURCE(IDC_HAND)));
		return TRUE;
	}
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CSpriteStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bClick)
	{
		::ShellExecute(NULL, "open", "", "", NULL, SW_SHOWNORMAL);
	}
}

void CSpriteStatic::SetImagePath(CString strPath)
{
	if(m_pSprite == NULL)
		m_pSprite = new CGdiPlusBitmapResource;
	
	if(!m_pSprite->LoadPath(strPath))
	{
		delete m_pSprite;
		m_pSprite = NULL;
		return;
	}	
	InvalidateRect(NULL);
}

void CSpriteStatic::SetImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
{
	if(m_pSprite == NULL)
		m_pSprite = new CGdiPlusBitmapResource;
	
	if (!m_pSprite->Load(pName, pType, hInst))
	{
		delete m_pSprite;
		m_pSprite = NULL;
		return;
	}
	InvalidateRect(NULL);
}

void CSpriteStatic::OnPaint() 
{		
	CPaintDC dc(this); 

	Graphics graphics(dc.GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	Color bgColor(Gdiplus::Color(255, GetRValue(m_crBG), GetGValue(m_crBG), GetBValue(m_crBG)));
	graphics.Clear(bgColor);

	CRect rect;
    GetClientRect(&rect);
	RectF wndRect(REAL(rect.left),	REAL(rect.top),	REAL(rect.Width()),	REAL(rect.Height()));
	
	if(m_pSprite)
	{	
		int x = (m_curIndex % m_nCol ) * m_nImgWidth;
		int y = (m_curIndex / m_nRow ) * m_nImgHeight;
        Rect ImgRect( x, y, m_nImgWidth, m_nImgHeight);
		graphics.DrawImage(m_pSprite->m_pBitmap, wndRect, ImgRect.X, ImgRect.Y, ImgRect.Width, ImgRect.Height, UnitPixel, NULL);
	}
}

BOOL CSpriteStatic::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CSpriteStatic::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == 1)
	{
		m_curIndex = (m_curIndex + 1) % m_TotalIndex;	
		InvalidateRect(NULL);

		/*
		if (m_TotalIndex > 0)
		{
			m_curIndex++;
			if (m_curIndex >= m_TotalIndex)
				m_curIndex = 0;	
			
			InvalidateRect(NULL);
		}
		*/
	}
}

void CSpriteStatic::SetSprite(int nTotalIndex, int nImgWidth, int nImgHeight, int nDelayTime)
{
	m_TotalIndex = nTotalIndex;
	m_nDelayTime = nDelayTime;
	m_nImgWidth = nImgWidth;
	m_nImgHeight = nImgHeight;
}

void CSpriteStatic::SetBkColor(COLORREF crBG)
{
	m_crBG = crBG;
}

void CSpriteStatic::SetRowCol(int nRow, int nCol)
{
	m_nRow = nRow;
	m_nCol = nCol;
}