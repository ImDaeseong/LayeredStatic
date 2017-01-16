#include "stdafx.h"
#include "SkinButtonLayerEx.h"

IMPLEMENT_DYNAMIC(CSkinButtonLayerEx, CButton)

CSkinButtonLayerEx::CSkinButtonLayerEx()
{
	m_pBGImage = NULL;
	m_Tracking = FALSE;
	m_Hovering = FALSE;
	m_Disabled = FALSE;

	m_nStartX = 0;
	m_nStartY = 0;
	m_Status  = 0;
}

CSkinButtonLayerEx::~CSkinButtonLayerEx()
{
	Free();
}

BEGIN_MESSAGE_MAP(CSkinButtonLayerEx, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

void CSkinButtonLayerEx::Free()
{
	if(m_pBGImage != NULL)
		delete m_pBGImage;
	m_pBGImage = NULL;
}

void CSkinButtonLayerEx::Disable(BOOL Disabled)
{
	m_Disabled = Disabled;
	RedrawWindow();
}

BOOL CSkinButtonLayerEx::Disabled()
{
	return m_Disabled;
}

void CSkinButtonLayerEx::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);
	CButton::PreSubclassWindow();
}

BOOL CSkinButtonLayerEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CButton::PreTranslateMessage(pMsg);
}

BOOL CSkinButtonLayerEx::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CSkinButtonLayerEx::OnMouseMove(UINT nFlags, CPoint point)
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

LRESULT CSkinButtonLayerEx::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	if (m_Disabled) return NULL;
	m_Hovering = TRUE;
	Invalidate(FALSE);
	return NULL;
}

LRESULT CSkinButtonLayerEx::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_Disabled) return NULL;
	m_Tracking = FALSE;
	m_Hovering = FALSE;
	Invalidate(FALSE);
	return NULL;
}

void CSkinButtonLayerEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_Disabled) return;
	CButton::OnLButtonDown(nFlags, point);
}

void CSkinButtonLayerEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_Disabled) return;
	CButton::OnLButtonUp(nFlags, point);
}

void CSkinButtonLayerEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
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
	GetOwner()->SendMessage(WM_PAINT, 0, 0);	
}

BOOL CSkinButtonLayerEx::SetImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst, int nStartX, int nStartY)
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

BOOL CSkinButtonLayerEx::SetImg(LPCTSTR pName, int nStartX, int nStartY)
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

BOOL CSkinButtonLayerEx::SetImagePath(CString strPath, int nStartX, int nStartY)
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

BOOL CSkinButtonLayerEx::IsAvailableDraw()
{
	if(!m_pBGImage)
		return FALSE;
	return TRUE;
}

void CSkinButtonLayerEx::OnDrawLayerdWindow(Graphics& Gps)
{		
	CRect rc;
	GetWindowRect(rc);
	GetOwner()->ScreenToClient(rc);	

	CSize szFrame = (GetStyle() & WS_THICKFRAME) ? CSize(GetSystemMetrics(SM_CXSIZEFRAME),  GetSystemMetrics(SM_CYSIZEFRAME)) : CSize(GetSystemMetrics(SM_CXFIXEDFRAME), GetSystemMetrics(SM_CYFIXEDFRAME));
	rc.OffsetRect(szFrame.cy, szFrame.cy);

	int nX = m_pBGImage->m_pBitmap->GetWidth();
	int nY = m_pBGImage->m_pBitmap->GetHeight();
    
	//RectF destRect(REAL(rc.left), REAL(rc.top), REAL(rc.Width()), REAL(rc.Height()));

	if(m_Status == 0)
	{
		Gps.DrawImage(m_pBGImage->m_pBitmap, Rect(rc.left, rc.top, nX, nY), 0, 0, nX, nY, UnitPixel);
	}
	else if(m_Status == 1)
	{	
		ColorMatrix light = {
			1.05f, 0.00f, 0.00f, 0.00f, 0.00f,
			0.00f, 1.05f, 0.00f, 0.00f, 0.00f,
			0.00f, 0.00f, 1.05f, 0.00f, 0.00f,
			0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
			0.05f, 0.05f, 0.05f, 0.00f, 1.00f};
		ImageAttributes ia;
		ia.SetColorMatrix(&light);

		//destRect.X += 1;
		//destRect.Y += 1;
		//destRect.Width -= 1;
		//destRect.Height -= 1;
		//Gps.DrawImage(m_pBGImage->m_pBitmap, destRect, 0, 0, nX, nY, UnitPixel,&ia);
		Gps.DrawImage(m_pBGImage->m_pBitmap, Rect(rc.left, rc.top, nX, nY), 0, 0, nX, nY, UnitPixel, &ia);
	}
	else if(m_Status == 2)
	{	
		ColorMatrix light = {
			1.05f, 0.00f, 0.00f, 0.00f, 0.00f,
			0.00f, 1.05f, 0.00f, 0.00f, 0.00f,
			0.00f, 0.00f, 1.05f, 0.00f, 0.00f,
			0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
			0.05f, 0.05f, 0.05f, 0.00f, 1.00f};
		ImageAttributes ia;
		ia.SetColorMatrix(&light);

		//destRect.X -= 1;
		//destRect.Y -= 1;
		//destRect.Width += 1;
		//destRect.Height += 1;
		//Gps.DrawImage(m_pBGImage->m_pBitmap, destRect, 0, 0, nX, nY, UnitPixel,&ia);
		Gps.DrawImage(m_pBGImage->m_pBitmap, Rect(rc.left, rc.top, nX, nY), 0, 0, nX, nY, UnitPixel, &ia);
	}
	else if(m_Status == 3) 
	{		
		ColorMatrix Gray = {
			0.30f, 0.30f, 0.30f, 0.00f, 0.00f,
			0.59f, 0.59f, 0.59f, 0.00f, 0.00f,
			0.11f, 0.11f, 0.11f, 0.00f, 0.00f,
			0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
			0.00f, 0.00f, 0.00f, 0.00f, 1.00f};

		ImageAttributes ia;
		ia.SetColorMatrix(&Gray);
		//Gps.DrawImage(m_pBGImage->m_pBitmap, destRect, 0, 0, nX, nY, UnitPixel,&ia);
		Gps.DrawImage(m_pBGImage->m_pBitmap, Rect(rc.left, rc.top, nX, nY), 0, 0, nX, nY, UnitPixel, &ia);
	}
}

