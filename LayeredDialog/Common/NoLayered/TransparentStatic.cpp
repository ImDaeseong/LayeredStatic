#include "stdafx.h"
#include "TransparentStatic.h"

IMPLEMENT_DYNAMIC(CTransparentStatic, CStatic)

CTransparentStatic::CTransparentStatic()
{
	m_strMsg = "";
	m_strFont = "돋움";
	m_nFontSize = 12;
	m_crText = RGB(0, 0, 0);
	m_FontStyle = FontStyle::FontStyleRegular;
	m_nAlign = 0;
}

CTransparentStatic::~CTransparentStatic()
{	
}

BEGIN_MESSAGE_MAP(CTransparentStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CTransparentStatic::OnEraseBkgnd(CDC* pDC)
{	
	return TRUE;
}

void CTransparentStatic::OnPaint()
{
	USES_CONVERSION;

	CPaintDC dc(this); 

	Gdiplus::Graphics graphics(dc.GetSafeHdc());

	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	CRect rect;
	GetClientRect(&rect);

	FontFamily fontFamily(A2W(m_strFont));
	Gdiplus::Font font(&fontFamily, m_nFontSize, m_FontStyle, UnitPixel);
	
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, GetRValue(m_crText), GetGValue(m_crText), GetBValue(m_crText)));

	StringFormat stringFormat(StringFormatFlagsDisplayFormatControl);
	if(m_nAlign == 1)
	{
		stringFormat.SetAlignment(StringAlignmentCenter);
		stringFormat.SetLineAlignment(StringAlignmentCenter);
	}
	else if(m_nAlign == 2)
	{
		stringFormat.SetAlignment(StringAlignmentFar);
		stringFormat.SetLineAlignment(StringAlignmentFar);
	}
	else
	{
		stringFormat.SetAlignment(StringAlignmentNear);
		stringFormat.SetLineAlignment(StringAlignmentNear);
	}

	RectF rectF(0, 0, (float)rect.Width(), (float)rect.Height());
	graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	graphics.DrawString(A2W(m_strMsg), -1, &font, rectF, &stringFormat, &brush);

	//check size
	//SolidBrush bgbrush(Color(112, 0, 0));
	//graphics.FillRectangle(&bgbrush, rectF);
}

void CTransparentStatic::SetFontName(CString strFont)
{	
	m_strFont = strFont;
}

void CTransparentStatic::SetFontSize(int nFontSize)
{
	m_nFontSize = nFontSize;
}

void CTransparentStatic::SetTextColor(COLORREF crText)
{
	m_crText = crText;
}

void CTransparentStatic::SetFontBold(BOOL bBold)
{	
	if(bBold)
		m_FontStyle = FontStyle::FontStyleBold;
	else
		m_FontStyle = FontStyle::FontStyleRegular;
}

void CTransparentStatic::SetStringAlign(int nAlign)
{
	m_nAlign = nAlign;
}

void CTransparentStatic::SetText(CString strMsg)
{
	m_strMsg = strMsg;
	//RedrawWindow();
	
	//깜박임 증상으로 사용중지
	CRect rect;
	GetWindowRect(&rect);
	GetParent()->ScreenToClient(&rect);
	GetParent()->InvalidateRect(rect, TRUE);
	Invalidate(FALSE);
}