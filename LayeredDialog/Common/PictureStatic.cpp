#include "stdafx.h"
#include "PictureStatic.h"

CPictureStatic::CPictureStatic()
{
	m_pImage = NULL;
	m_bClick = FALSE;
	m_bCursor = FALSE;
}

CPictureStatic::~CPictureStatic()
{
	if(m_pImage != NULL)
		delete m_pImage;
	m_pImage = NULL;
}

BEGIN_MESSAGE_MAP(CPictureStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

CPictureStatic& CPictureStatic::SetPictureStaticClick(BOOL bClick)
{
	m_bClick = bClick;

	if (bClick)
		ModifyStyle(0,SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY,0);
	return *this;
}

void CPictureStatic::SetPictureStaticCursor(BOOL bCursor)
{
	m_bCursor = bCursor;	
}

BOOL CPictureStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(m_bCursor)
	{		
		::SetCursor(::LoadCursor(0, MAKEINTRESOURCE(IDC_HAND)));
		return TRUE;
	}
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CPictureStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bClick)
	{
		::ShellExecute(NULL, "open", "", "", NULL, SW_SHOWNORMAL);
	}
}

void CPictureStatic::SetImagePath(CString strPath)
{
	if(m_pImage == NULL)
		m_pImage = new CGdiPlusBitmapResource;
	
	if(!m_pImage->LoadPath(strPath))
	{
		delete m_pImage;
		m_pImage = NULL;
		return;
	}	
	InvalidateRect(NULL);
}

void CPictureStatic::SetImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
{
	if(m_pImage == NULL)
		m_pImage = new CGdiPlusBitmapResource;
	
	if (!m_pImage->Load(pName, pType, hInst))
	{
		delete m_pImage;
		m_pImage = NULL;
		return;
	}
	InvalidateRect(NULL);
}

void CPictureStatic::OnPaint() 
{		
	CPaintDC dc(this); 

	Graphics graphics(dc.GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	CRect rect;
    GetClientRect(&rect);
	RectF destRect(REAL(rect.left),	REAL(rect.top),	REAL(rect.Width()),	REAL(rect.Height()));
	
	if(m_pImage)
	{
		Unit units;
		RectF srcRect;
		m_pImage->m_pBitmap->GetBounds(&srcRect, &units);
		graphics.DrawImage(m_pImage->m_pBitmap, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel, NULL);
	}
}

BOOL CPictureStatic::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

BOOL CPictureStatic::IsAvailableDraw()
{
	if(!m_pImage )
		return FALSE;
	return TRUE;
}

void CPictureStatic::OnDrawLayerdWindow(Graphics& Gps)
{
	if(m_pImage)
	{		
		Gps.SetSmoothingMode(SmoothingModeHighQuality);
		Gps.SetInterpolationMode(InterpolationModeHighQualityBicubic);

		CRect rc;
		GetWindowRect(rc);
		GetOwner()->ScreenToClient(rc);
		RectF destRect(REAL(rc.left), REAL(rc.top),	REAL(rc.Width()), REAL(rc.Height()));

		Unit units;
		RectF srcRect;
		m_pImage->m_pBitmap->GetBounds(&srcRect, &units);
		Gps.DrawImage(m_pImage->m_pBitmap, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel, NULL);
	}
}
