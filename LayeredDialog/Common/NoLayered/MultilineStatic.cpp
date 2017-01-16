#include "stdafx.h"
#include "MultilineStatic.h"

IMPLEMENT_DYNAMIC(CMultilineStatic, CStatic)

CMultilineStatic::CMultilineStatic()
{
	m_graphics = NULL;

	m_crBG = RGB(255, 255, 255);
	
	m_strFont = "µ¸¿ò";
	m_nFontSize = 12;
    m_FontStyle = FontStyle::FontStyleRegular;

	m_strMsg1 = "";
	m_strMsg2 = "";
	m_strMsg3 = "";

	m_crText1 = RGB(255, 255, 255);
	m_crText2 = RGB(255, 255, 255);
	m_crText3 = RGB(255, 255, 255);

	m_nOffset = 4;
}

CMultilineStatic::~CMultilineStatic()
{	
	if (m_graphics) 
	{
		delete m_graphics;
		m_graphics = NULL;
	}
}

BEGIN_MESSAGE_MAP(CMultilineStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CMultilineStatic::OnEraseBkgnd(CDC* pDC)
{	
	return TRUE;
}

void CMultilineStatic::OnPaint()
{		
	CPaintDC dc(this); 

	CRect rect;
	GetClientRect(&rect);

	Gdiplus::Graphics pGraphics(dc.m_hDC);
	Gdiplus::Bitmap pBitmap(rect.Width(), rect.Height());

	m_graphics = Gdiplus::Graphics::FromImage(&pBitmap);
	m_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	
	SolidBrush bgColor(Gdiplus::Color(255, GetRValue(m_crBG), GetGValue(m_crBG), GetBValue(m_crBG)));
	Gdiplus::Rect BgRect(0, 0, rect.Width(), rect.Height());
	m_graphics->FillRectangle(&bgColor, BgRect);

	DrawLabel1(rect);
	DrawLabel2(rect);
	DrawLabel3(rect);

	pGraphics.DrawImage(&pBitmap, rect.left, rect.top);

	if (m_graphics) 
	{
		delete m_graphics;
		m_graphics = NULL;
	}
}

void CMultilineStatic::SetBkColor(COLORREF crBG)
{
	m_crBG = crBG;
}

void CMultilineStatic::SetFontName(CString strFont)
{	
	m_strFont = strFont;
}

void CMultilineStatic::SetFontSize(int nFontSize)
{
	m_nFontSize = nFontSize;
}

void CMultilineStatic::SetFontBold(BOOL bBold)
{	
	if(bBold)
		m_FontStyle = FontStyle::FontStyleBold;
	else
		m_FontStyle = FontStyle::FontStyleRegular;
}

void CMultilineStatic::SetText1(CString strMsg, COLORREF crText)
{
	if(strMsg == "") return;

	m_strMsg1 = strMsg;
	m_crText1 = crText;
	RedrawWindow();
}

void CMultilineStatic::SetText2(CString strMsg, COLORREF crText)
{
	if(strMsg == "") return;

	m_strMsg2 = strMsg;
	m_crText2 = crText;
	RedrawWindow();
}

void CMultilineStatic::SetText3(CString strMsg, COLORREF crText)
{
	if(strMsg == "") return;

	m_strMsg3 = strMsg;
	m_crText3 = crText;
	RedrawWindow();
}

void CMultilineStatic::DrawLabel1(CRect rRect) 
{
	USES_CONVERSION;

	int nHeight = (rRect.Height()/3);

	Gdiplus::FontFamily	 fontFamily(A2W(m_strFont));
	Gdiplus::Font        font(&fontFamily, m_nFontSize, m_FontStyle, UnitPixel);
	Gdiplus::RectF		 rectF(0, m_nOffset, rRect.Width(), nHeight);
		
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, GetRValue(m_crText1), GetGValue(m_crText1), GetBValue(m_crText1)));

	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentNear);
	m_graphics->DrawString(A2W(m_strMsg1), -1, &font, rectF, &stringFormat, &brush);

	//RectF boundRect;
	//m_graphics->MeasureString(A2W(m_strMsg1), -1, &font, rectF, &stringFormat, &boundRect);
	//return boundRect.Height;
}

void CMultilineStatic::DrawLabel2(CRect rRect) 
{
	USES_CONVERSION;

	int nHeight = (rRect.Height()/3);

	Gdiplus::FontFamily	 fontFamily(A2W(m_strFont));
	Gdiplus::Font        font(&fontFamily, m_nFontSize, m_FontStyle, UnitPixel);
	Gdiplus::RectF		 rectF(0, (nHeight + m_nOffset), rRect.Width(), nHeight);
	
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, GetRValue(m_crText2), GetGValue(m_crText2), GetBValue(m_crText2)));

	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentNear);
	m_graphics->DrawString(A2W(m_strMsg2), -1, &font, rectF, &stringFormat, &brush);

	//RectF boundRect;
	//m_graphics->MeasureString(A2W(m_strMsg2), -1, &font, rectF, &stringFormat, &boundRect);
	//return boundRect.Height;
}

void CMultilineStatic::DrawLabel3(CRect rRect) 
{
	USES_CONVERSION;

	int nHeight = (rRect.Height()/3);

	Gdiplus::FontFamily	 fontFamily(A2W(m_strFont));
	Gdiplus::Font        font(&fontFamily, m_nFontSize, m_FontStyle, UnitPixel);
	Gdiplus::RectF		 rectF(0, (nHeight * 2) + m_nOffset, rRect.Width(), nHeight);
	
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, GetRValue(m_crText3), GetGValue(m_crText3), GetBValue(m_crText3)));

	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentNear);
	m_graphics->DrawString(A2W(m_strMsg3), -1, &font, rectF, &stringFormat, &brush);

	//RectF boundRect;
	//m_graphics->MeasureString(A2W(m_strMsg3), -1, &font, rectF, &stringFormat, &boundRect);
	//return boundRect.Height;
}