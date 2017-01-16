#include "stdafx.h"
#include "LabelStatic.h"

IMPLEMENT_DYNAMIC(CLabelStatic, CStatic)

CLabelStatic::CLabelStatic()
{
	m_strMsg = "";
	m_strFont = "µ¸¿ò";
	m_nFontSize = 12;
	m_crText = RGB(0, 0, 0);
	m_crBG = RGB(255, 255, 255);
	m_FontStyle = FontStyle::FontStyleRegular;
	m_nAlign = 0;

	m_bClick = FALSE;
	m_bCursor = FALSE;
	m_bUnderLine = FALSE;

	m_DashStyle = FALSE;

	m_bGradient = FALSE;
	m_crGrBG1 = RGB(255, 255, 255);
	m_crGrBG2 = RGB(255, 255, 255);
}

CLabelStatic::~CLabelStatic()
{	
}

BEGIN_MESSAGE_MAP(CLabelStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CLabelStatic::OnEraseBkgnd(CDC* pDC)
{	
	return TRUE;
}

void CLabelStatic::OnPaint()
{
	USES_CONVERSION;

	CPaintDC dc(this); 

	Gdiplus::Graphics graphics(dc.GetSafeHdc());

	CRect rect;
	GetClientRect(&rect);
	
	if(m_bGradient)
	{
		//Vertical
		Bitmap bm(rect.Width(), rect.Height(), &graphics);
		Graphics temp(&bm);
	    LinearGradientBrush	brush(Rect(0, 0, rect.Width(), rect.Height()), Color(255, GetRValue(m_crGrBG1), GetGValue(m_crGrBG1), GetBValue(m_crGrBG1)), Color(255, GetRValue(m_crGrBG2), GetGValue(m_crGrBG2), GetBValue(m_crGrBG2)), LinearGradientModeVertical);
		temp.FillRectangle(&brush, Rect(0, 0,  rect.Width(), rect.Height()));
		graphics.DrawImage(&bm, rect.left, rect.top);

		//Horizontal
		/*
		Bitmap bm(rect.Width(), rect.Height(), &graphics);
		Graphics temp(&bm);
	    LinearGradientBrush	brush(Rect(0, 0, rect.Width(), rect.Height()), Color(255, GetRValue(m_crGrBG1), GetGValue(m_crGrBG1), GetBValue(m_crGrBG1)), Color(255, GetRValue(m_crGrBG2), GetGValue(m_crGrBG2), GetBValue(m_crGrBG2)), LinearGradientModeHorizontal);
		temp.FillRectangle(&brush, Rect(0, 0,  rect.Width(), rect.Height()));
		graphics.DrawImage(&bm, rect.left, rect.top);
		*/		
	}
	else
	{
		Color bgColor(Gdiplus::Color(255, GetRValue(m_crBG), GetGValue(m_crBG), GetBValue(m_crBG)));
		graphics.Clear(bgColor);
	}


	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	
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
	
	if(m_bUnderLine)
	{
		RectF rectMeasure;
		graphics.MeasureString(A2W(m_strMsg), -1, &font, RectF((float)rect.left, (float)rect.top, (float)rect.Width(), (float)rect.Height()), &rectMeasure);

        /*		
		Rect rectUnderLine;
		rectUnderLine.X = (int)(rect.left);
		rectUnderLine.Y = (int)(rectMeasure.GetBottom() + 3.f);
		rectUnderLine.Width = rect.Width();
		rectUnderLine.Height = 1;
		graphics.FillRectangle( &SolidBrush( Color(GetRValue(m_crText), GetGValue(m_crText), GetBValue(m_crText)) ), rectUnderLine);
       	*/

		if(m_DashStyle)
		{
			Pen pen(Color(55, GetRValue(m_crText), GetGValue(m_crText), GetBValue(m_crText)), 1);
			pen.SetDashStyle(Gdiplus::DashStyle::DashStyleDash);
			graphics.DrawLine(&pen, (int)(rect.left), (int)(rectMeasure.GetBottom() + 3.f), rect.Width(), (int)(rectMeasure.Y + rectMeasure.GetBottom() + 3.f) );
		}
		else
		{
			Gdiplus::Pen pen(Color(GetRValue(m_crText), GetGValue(m_crText), GetBValue(m_crText)));
			graphics.DrawLine(&pen, (int)(rect.left), (int)(rectMeasure.GetBottom() + 3.f), rect.Width(), (int)(rectMeasure.Y + rectMeasure.GetBottom() + 3.f) );
		}
	}

	//check size
	//SolidBrush bgbrush(Color(112, 0, 0));
	//graphics.FillRectangle(&bgbrush, rectF);
}

void CLabelStatic::SetFontName(CString strFont)
{	
	m_strFont = strFont;
}

void CLabelStatic::SetFontSize(int nFontSize)
{
	m_nFontSize = nFontSize;
}

void CLabelStatic::SetTextColor(COLORREF crText)
{
	m_crText = crText;
}

void CLabelStatic::SetBkColor(COLORREF crBG)
{
	m_crBG = crBG;
}

void CLabelStatic::SetFontBold(BOOL bBold)
{	
	if(bBold)
		m_FontStyle = FontStyle::FontStyleBold;
	else
		m_FontStyle = FontStyle::FontStyleRegular;
}

void CLabelStatic::SetStringAlign(int nAlign)
{
	m_nAlign = nAlign;
}

void CLabelStatic::SetUnderLine(BOOL bUnderLine)
{
	m_bUnderLine = bUnderLine;
}

void CLabelStatic::SetText(CString strMsg)
{	
	m_strMsg = strMsg;
	RedrawWindow();
}

CLabelStatic& CLabelStatic::SetLabelStaticClick(BOOL bClick)
{
	m_bClick = bClick;

	if (bClick)
		ModifyStyle(0,SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY,0);
	return *this;
}

void CLabelStatic::SetLabelStaticCursor(BOOL bCursor)
{
	m_bCursor = bCursor;	
}

void CLabelStatic::SetGradient(BOOL bGradient, COLORREF crGrBG1, COLORREF crGrBG2)
{
	m_bGradient = bGradient;
	m_crGrBG1 = crGrBG1;
	m_crGrBG2 = crGrBG2;
}

void CLabelStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bClick)
	{
		::ShellExecute(NULL, "open", "", "", NULL, SW_SHOWNORMAL);
	}	
}

BOOL CLabelStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(m_bCursor)
	{		
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		return TRUE;
	}
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}
