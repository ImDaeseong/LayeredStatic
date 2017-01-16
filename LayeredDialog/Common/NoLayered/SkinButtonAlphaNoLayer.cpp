#include "stdafx.h"
#include "SkinButtonAlphaNoLayer.h"

IMPLEMENT_DYNAMIC(CSkinButtonAlphaNoLayer, CButton)

CSkinButtonAlphaNoLayer::CSkinButtonAlphaNoLayer()
{
	m_pBGImage = NULL;
	m_Tracking = FALSE;
	m_Hovering = FALSE;
	m_Disabled = FALSE;

	m_nStartX = 0;
	m_nStartY = 0;
	m_Status  = 0;

	m_crBG = RGB(255, 255, 255);
}

CSkinButtonAlphaNoLayer::~CSkinButtonAlphaNoLayer()
{
	Free();
}

BEGIN_MESSAGE_MAP(CSkinButtonAlphaNoLayer, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

void CSkinButtonAlphaNoLayer::Free()
{
	if(m_pBGImage != NULL)
		delete m_pBGImage;
	m_pBGImage = NULL;
}

void CSkinButtonAlphaNoLayer::Disable(BOOL Disabled)
{
	m_Disabled = Disabled;
	RedrawWindow();
}

BOOL CSkinButtonAlphaNoLayer::Disabled()
{
	return m_Disabled;
}

void CSkinButtonAlphaNoLayer::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);
	CButton::PreSubclassWindow();
}

BOOL CSkinButtonAlphaNoLayer::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CButton::PreTranslateMessage(pMsg);
}

BOOL CSkinButtonAlphaNoLayer::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CSkinButtonAlphaNoLayer::OnMouseMove(UINT nFlags, CPoint point)
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

LRESULT CSkinButtonAlphaNoLayer::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	if (m_Disabled) return NULL;
	m_Hovering = TRUE;
	Invalidate(FALSE);
	return NULL;
}

LRESULT CSkinButtonAlphaNoLayer::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_Disabled) return NULL;
	m_Tracking = FALSE;
	m_Hovering = FALSE;
	Invalidate(FALSE);
	return NULL;
}

void CSkinButtonAlphaNoLayer::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_Disabled) return;
	CButton::OnLButtonDown(nFlags, point);
}

void CSkinButtonAlphaNoLayer::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_Disabled) return;
	CButton::OnLButtonUp(nFlags, point);
}

void CSkinButtonAlphaNoLayer::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
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
		int nX = m_pBGImage->m_pBitmap->GetWidth();
		int nY = m_pBGImage->m_pBitmap->GetHeight();

		if(m_Status == 0)
		{
			gps.DrawImage(m_pBGImage->m_pBitmap, Rect(0, 0, nX, nY), 0, 0, nX, nY, UnitPixel);
		}
		else if(m_Status == 1)
		{	
			int nAlpha = 200;
			ColorMatrix light = {
				1, 0, 0, 0, 0,
				0, 1, 0, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 0, nAlpha / 255.0, 0,
				0, 0, 0, 0, 1};
			ImageAttributes ia;
			ia.SetColorMatrix(&light);
			gps.DrawImage(m_pBGImage->m_pBitmap, Rect(0, 0, nX, nY), 0, 0, nX, nY, UnitPixel, &ia);
		}
		else if(m_Status == 2)
		{	
			int nAlpha = 200;
			ColorMatrix light = {
				1, 0, 0, 0, 0,
				0, 1, 0, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 0, nAlpha / 255.0, 0,
				0, 0, 0, 0, 1};
			ImageAttributes ia;
			ia.SetColorMatrix(&light);
			gps.DrawImage(m_pBGImage->m_pBitmap, Rect(0, 0, nX, nY), 0, 0, nX, nY, UnitPixel, &ia);
		}
		else if(m_Status == 3)
		{		
			int nAlpha = 100;
			ColorMatrix Gray = {
				1, 0, 0, 0, 0,
				0, 1, 0, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 0, nAlpha / 255.0, 0,
				0, 0, 0, 0, 1};
			ImageAttributes ia;
			ia.SetColorMatrix(&Gray);
			gps.DrawImage(m_pBGImage->m_pBitmap, Rect(0, 0, nX, nY), 0, 0, nX, nY, UnitPixel, &ia);
		}
	}

	gps.ReleaseHDC(pDC->m_hDC);  
}

BOOL CSkinButtonAlphaNoLayer::SetImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst, int nStartX, int nStartY)
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
	
	m_nStartX = nStartX;
	m_nStartY = nStartY;

	int nX = m_pBGImage->m_pBitmap->GetWidth();
	int nY = m_pBGImage->m_pBitmap->GetHeight();
	MoveWindow(nStartX, nStartY, nX, nY);	
	return TRUE;
}

BOOL CSkinButtonAlphaNoLayer::SetImg(LPCTSTR pName, int nStartX, int nStartY)
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
	
	m_nStartX = nStartX;
	m_nStartY = nStartY;

	int nX = m_pBGImage->m_pBitmap->GetWidth();
	int nY = m_pBGImage->m_pBitmap->GetHeight();
	MoveWindow(m_nStartX, nStartY, nX, nY);	
	return TRUE;
}

BOOL CSkinButtonAlphaNoLayer::SetImagePath(CString strPath, int nStartX, int nStartY)
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

	m_nStartX = nStartX;
	m_nStartY = nStartY;

	int nX = m_pBGImage->m_pBitmap->GetWidth();
	int nY = m_pBGImage->m_pBitmap->GetHeight();
	MoveWindow(m_nStartX, nStartY, nX, nY);	
	return TRUE;
}

void CSkinButtonAlphaNoLayer::SetBkColor(COLORREF crBG)
{
	m_crBG = crBG;
}