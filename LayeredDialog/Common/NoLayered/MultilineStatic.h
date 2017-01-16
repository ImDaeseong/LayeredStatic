#pragma once

class CMultilineStatic : public CStatic
{
	DECLARE_DYNAMIC(CMultilineStatic)

public:
	CMultilineStatic();
	virtual ~CMultilineStatic();

	void SetBkColor(COLORREF crBG);
	void SetFontName(CString strFont);
	void SetFontSize(int nFontSize);
	void SetFontBold(BOOL bBold);
	
	void SetText1(CString strMsg, COLORREF crText);
	void SetText2(CString strMsg, COLORREF crText);
	void SetText3(CString strMsg, COLORREF crText);

private:
	void DrawLabel1(CRect rRect);
	void DrawLabel2(CRect rRect);
	void DrawLabel3(CRect rRect);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
	Gdiplus::Graphics *m_graphics;

	CString	   m_strFont;
	int		   m_nFontSize;
	FontStyle  m_FontStyle;
	COLORREF   m_crBG;

	CString    m_strMsg1;
	CString    m_strMsg2;
	CString    m_strMsg3;

	COLORREF   m_crText1;
	COLORREF   m_crText2;
	COLORREF   m_crText3;

	int        m_nOffset; 
};


