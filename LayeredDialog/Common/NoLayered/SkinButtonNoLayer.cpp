#include "stdafx.h"
#include "SkinButtonNoLayer.h"

IMPLEMENT_DYNAMIC(CSkinButtonNoLayer, CButton)

CSkinButtonNoLayer::CSkinButtonNoLayer()
{
	m_pBGImage = NULL;
	m_Tracking = FALSE;
	m_Hovering = FALSE;
	m_Disabled = FALSE;

	m_nStartX = 0;
	m_nStartY = 0;
	m_nDivide = 4;
	m_Status  = 0;

	m_crBG = RGB(255, 255, 255);
}

CSkinButtonNoLayer::~CSkinButtonNoLayer()
{
	Free();
}

BEGIN_MESSAGE_MAP(CSkinButtonNoLayer, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

void CSkinButtonNoLayer::Free()
{
	if(m_pBGImage != NULL)
		delete m_pBGImage;
	m_pBGImage = NULL;
}

void CSkinButtonNoLayer::Disable(BOOL Disabled)
{
	m_Disabled = Disabled;
	RedrawWindow();
}

BOOL CSkinButtonNoLayer::Disabled()
{
	return m_Disabled;
}

void CSkinButtonNoLayer::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);
	CButton::PreSubclassWindow();
}

BOOL CSkinButtonNoLayer::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CButton::PreTranslateMessage(pMsg);
}

BOOL CSkinButtonNoLayer::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CSkinButtonNoLayer::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_Disabled) return;
	if (!m_Tracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_Tracking = _TrackMouseEvent(&tme);
		Invalidate(FALSE);
	}

	if(m_Hovering)
		::SetCursor(::LoadCursor(0, MAKEINTRESOURCE(IDC_HAND)));

	CButton::OnMouseMove(nFlags, point);
}

LRESULT CSkinButtonNoLayer::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	if (m_Disabled) return NULL;
	m_Hovering = TRUE;
	Invalidate(FALSE);
	return NULL;
}

LRESULT CSkinButtonNoLayer::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_Disabled) return NULL;
	m_Tracking = FALSE;
	m_Hovering = FALSE;
	Invalidate(FALSE);
	return NULL;
}

void CSkinButtonNoLayer::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_Disabled) return;
	CButton::OnLButtonDown(nFlags, point);
}

void CSkinButtonNoLayer::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_Disabled) return;
	CButton::OnLButtonUp(nFlags, point);
}

void CSkinButtonNoLayer::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		
	Gdiplus::Graphics gps(pDC->m_hDC);
	
	Color bgColor(Gdiplus::Color(255, GetRValue(m_crBG), GetGValue(m_crBG), GetBValue(m_crBG)));
	gps.Clear(bgColor);

	gps.SetSmoothingMode(SmoothingModeAntiAlias);
   
	UINT state = lpDrawItemStruct->itemState;
	if ((state & ODS_SELECTED))
	{	
		m_Status  = 2;
	}
	else if (m_Disabled)
	{
		m_Status  = 3;
	}
	else if (m_Hovering)
	{
		m_Status  = 1;
	}
	else
	{	
		m_Status  = 0;
	}		

	if(m_pBGImage != NULL)
	{
		int nX = m_pBGImage->m_pBitmap->GetWidth() / m_nDivide;
		int nY = m_pBGImage->m_pBitmap->GetHeight();

		if(m_nDivide == 1)
			gps.DrawImage(m_pBGImage->m_pBitmap, Rect(0, 0, nX, nY), 0, 0, nX, nY, UnitPixel);
		else
			gps.DrawImage(m_pBGImage->m_pBitmap, Rect(0, 0, nX, nY), nX * m_Status, 0, nX, nY, UnitPixel);
	}

	gps.ReleaseHDC(pDC->m_hDC);  
}

BOOL CSkinButtonNoLayer::SetImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst, int nStartX, int nStartY, int nDivide)
{
	if (!m_pBGImage)
	{
		m_pBGImage = new CGdiPlusBitmapResource;
	}

	if (!m_pBGImage->Load(pName, pType, hInst))
	{
		Free();
		return FALSE;
	}

	m_nDivide = nDivide;
	m_nStartX = nStartX;
	m_nStartY = nStartY;

	int nX = m_pBGImage->m_pBitmap->GetWidth() / m_nDivide;
	int nY = m_pBGImage->m_pBitmap->GetHeight();
	MoveWindow(nStartX, nStartY, nX, nY);	
	return TRUE;
}

BOOL CSkinButtonNoLayer::SetImg(LPCTSTR pName, int nStartX, int nStartY, int nDivide)
{
	if(!m_pBGImage)
	{
		m_pBGImage = new CGdiPlusBitmapResource;
		if(!m_pBGImage->Load(pName))
		{
			Free();
			return FALSE;
		}
	}	
	
	m_nDivide = nDivide;
	m_nStartX = nStartX;
	m_nStartY = nStartY;

	int nX = m_pBGImage->m_pBitmap->GetWidth() / m_nDivide;
	int nY = m_pBGImage->m_pBitmap->GetHeight();
	MoveWindow(m_nStartX, nStartY, nX, nY);	
	return TRUE;
}

BOOL CSkinButtonNoLayer::SetImagePath(CString strPath, int nStartX, int nStartY, int nDivide)
{
	if(!m_pBGImage)
	{
		m_pBGImage = new CGdiPlusBitmapResource;
		if(!m_pBGImage->LoadPath(strPath))
		{
			Free();
			return FALSE;
		}
	}	

	m_nDivide = nDivide;
	m_nStartX = nStartX;
	m_nStartY = nStartY;

	int nX = m_pBGImage->m_pBitmap->GetWidth() / m_nDivide;
	int nY = m_pBGImage->m_pBitmap->GetHeight();
	MoveWindow(m_nStartX, nStartY, nX, nY);	
	return TRUE;
}

void CSkinButtonNoLayer::SetBkColor(COLORREF crBG)
{
	m_crBG = crBG;
}