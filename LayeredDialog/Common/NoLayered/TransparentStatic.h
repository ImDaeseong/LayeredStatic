#pragma once

class CTransparentStatic : public CStatic
{
	DECLARE_DYNAMIC(CTransparentStatic)

public:
	CTransparentStatic();
	virtual ~CTransparentStatic();

	void SetFontName(CString strFont);
	void SetFontSize(int nFontSize);
	void SetTextColor(COLORREF crText);
	void SetFontBold(BOOL bBold);
	void SetText(CString strMsg);
	void SetStringAlign(int nAlign);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
	CString    m_strMsg;
	CString	   m_strFont;
	int		   m_nFontSize;
	COLORREF   m_crText;
	FontStyle  m_FontStyle;
	int        m_nAlign;
};


