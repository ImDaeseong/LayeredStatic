#include "stdafx.h"
#include "LayeredStatic.h"

CLayeredStatic::CLayeredStatic()
{
	m_pImage = NULL;
	m_pBackImage = NULL;
}

CLayeredStatic::~CLayeredStatic()
{
	FreeImage();
}

BEGIN_MESSAGE_MAP(CLayeredStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CLayeredStatic::FreeImage()
{
	if(m_pImage != NULL)
		delete m_pImage;
	m_pImage = NULL;

	if(m_pBackImage != NULL)
		delete m_pBackImage;
	m_pBackImage = NULL;
}

void CLayeredStatic::SetImagePath(CString strBackPath, CString strPath)
{	
	if(m_pBackImage == NULL)
		m_pBackImage = new CGdiPlusBitmapResource;

	if(!m_pBackImage->LoadPath(strBackPath))
	{
		delete m_pBackImage;
		m_pBackImage = NULL;
		return;
	}

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

void CLayeredStatic::OnPaint() 
{
	CPaintDC dc(this); 

	CDC* pDC = GetDC();

	Unit units;
	CRect rect;
	GetClientRect(&rect);

	CDC m_memDC;
	CBitmap m_memBitmap;

	m_memDC.CreateCompatibleDC(pDC);
	if(!m_memDC.GetSafeHdc())
		return;

	m_memBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	m_memDC.SelectObject(m_memBitmap);

	Graphics gr(m_memDC.m_hDC);
		
	if (m_pBackImage != NULL)
	{
		RectF destRect(REAL(rect.left),	REAL(rect.top),	REAL(rect.Width()),	REAL(rect.Height()));
		RectF srcRect;
		m_pBackImage->m_pBitmap->GetBounds(&srcRect, &units);
		gr.DrawImage(m_pBackImage->m_pBitmap, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel, NULL);
	}

	if (m_pImage != NULL)
	{	
		int nImageWidth = (m_pImage->m_pBitmap->GetWidth()/2);
		int nImageHeight = (m_pImage->m_pBitmap->GetHeight()/2);
		int x = ((rect.Width() - nImageWidth) /2);
		int y = ((rect.Height() - nImageHeight) /2);
		RectF destRect(x, y, nImageWidth, nImageHeight);	

		RectF srcRect;
		m_pImage->m_pBitmap->GetBounds(&srcRect, &units);
		gr.DrawImage(m_pImage->m_pBitmap, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, UnitPixel, NULL);

		BLENDFUNCTION bf;
		bf.BlendOp =AC_SRC_OVER;
		bf.BlendFlags =0;
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;
		
		POINT ptDes= CPoint(0, 0);
		SIZE  szDes={srcRect.Width, srcRect.Height};
		POINT ptSrc={0,0};
		BOOL bet =UpdateLayeredWindow(pDC, &ptDes, &szDes, &m_memDC, &ptSrc, 0, &bf, ULW_ALPHA);
		if(!bet)
			AlphaBlend(pDC->GetSafeHdc(), 0, 0, srcRect.Width, srcRect.Height, m_memDC, 0, 0, srcRect.Width, srcRect.Height, bf);
	}

	gr.ReleaseHDC(m_memDC);
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &m_memDC, 0, 0, SRCCOPY);
	m_memDC.DeleteDC();
	m_memBitmap.DeleteObject();
	ReleaseDC(pDC);
}

BOOL CLayeredStatic::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
