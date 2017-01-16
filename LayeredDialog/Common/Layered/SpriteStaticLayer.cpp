#include "stdafx.h"
#include "SpriteStaticLayer.h"

CSpriteStaticLayer::CSpriteStaticLayer()
{
	m_pSprite = NULL;
	m_bClick = FALSE;
	m_bCursor = FALSE;

	m_curIndex = 0;
	m_TotalIndex = 0;
	m_nDelayTime = 40;
	m_nImgWidth = 0;
	m_nImgHeight = 0;

	m_nRow = 0;
	m_nCol = 0;
}

CSpriteStaticLayer::~CSpriteStaticLayer()
{
	if(m_pSprite != NULL)
		delete m_pSprite;
	m_pSprite = NULL;
}

BEGIN_MESSAGE_MAP(CSpriteStaticLayer, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CSpriteStaticLayer::PreSubclassWindow() 
{
	CStatic::PreSubclassWindow();
    
	SetTimer(1, m_nDelayTime, NULL);
}

CSpriteStaticLayer& CSpriteStaticLayer::SetSpriteStaticClick(BOOL bClick)
{
	m_bClick = bClick;

	if (bClick)
		ModifyStyle(0,SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY,0);
	return *this;
}

void CSpriteStaticLayer::SetSpriteStaticCursor(BOOL bCursor)
{
	m_bCursor = bCursor;	
}

BOOL CSpriteStaticLayer::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(m_bCursor)
	{		
		::SetCursor(::LoadCursor(0, MAKEINTRESOURCE(IDC_HAND)));
		return TRUE;
	}
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CSpriteStaticLayer::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bClick)
	{
		::ShellExecute(NULL, "open", "", "", NULL, SW_SHOWNORMAL);
	}
}

void CSpriteStaticLayer::SetImagePath(CString strPath)
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

void CSpriteStaticLayer::SetImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
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

void CSpriteStaticLayer::OnPaint() 
{		
	CPaintDC dc(this); 
}

BOOL CSpriteStaticLayer::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

BOOL CSpriteStaticLayer::IsAvailableDraw()
{
	if(!m_pSprite )
		return FALSE;
	return TRUE;
}

void CSpriteStaticLayer::OnDrawLayerdWindow(Graphics& Gps)
{
	if(m_pSprite)
	{		
		Gps.SetSmoothingMode(SmoothingModeHighQuality);
		Gps.SetInterpolationMode(InterpolationModeHighQualityBicubic);

		CRect rc;
		GetWindowRect(rc);
		GetOwner()->ScreenToClient(rc);
		RectF wndRect(REAL(rc.left), REAL(rc.top),	REAL(rc.Width()), REAL(rc.Height()));
		
		int x = (m_curIndex % m_nCol ) * m_nImgWidth;
		int y = (m_curIndex / m_nRow ) * m_nImgHeight;
		Rect ImgRect( x, y, m_nImgWidth, m_nImgHeight);
		Gps.DrawImage(m_pSprite->m_pBitmap, wndRect, ImgRect.X, ImgRect.Y, ImgRect.Width, ImgRect.Height, UnitPixel, NULL);
	}
}

void CSpriteStaticLayer::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == 1)
	{
		m_curIndex = (m_curIndex + 1) % m_TotalIndex;	

		CRect rect;
		GetWindowRect(&rect);
		GetParent()->ScreenToClient(&rect);
		GetParent()->InvalidateRect(rect, TRUE);

		/*
		if (m_TotalIndex > 0)
		{
			m_curIndex++;
			if (m_curIndex >= m_TotalIndex)
				m_curIndex = 0;

			CRect rect;
			GetWindowRect(&rect);
			GetParent()->ScreenToClient(&rect);
			GetParent()->InvalidateRect(rect, TRUE);
		}
		*/
	}
}

void CSpriteStaticLayer::SetSprite(int nTotalIndex, int nImgWidth, int nImgHeight, int nDelayTime)
{
	m_TotalIndex = nTotalIndex;
	m_nDelayTime = nDelayTime;
	m_nImgWidth = nImgWidth;
	m_nImgHeight = nImgHeight;
}

void CSpriteStaticLayer::SetRowCol(int nRow, int nCol)
{
	m_nRow = nRow;
	m_nCol = nCol;
}