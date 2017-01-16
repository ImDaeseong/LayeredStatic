#pragma once

class CLabelStatic : public CStatic
{
	DECLARE_DYNAMIC(CLabelStatic)

public:
	CLabelStatic();
	virtual ~CLabelStatic();

	void SetFontName(CString strFont);
	void SetFontSize(int nFontSize);
	void SetTextColor(COLORREF crText);
	void SetBkColor(COLORREF crBG);
	void SetFontBold(BOOL bBold);
	void SetText(CString strMsg);
	void SetStringAlign(int nAlign);
    void SetUnderLine(BOOL bUnderLine);

	CLabelStatic& SetLabelStaticClick(BOOL bClick = FALSE);
	void SetLabelStaticCursor(BOOL bCursor = FALSE);

	void SetGradient(BOOL bGradient, COLORREF crGrBG1, COLORREF crGrBG2);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

private:
	CString    m_strMsg;
	CString	   m_strFont;
	int		   m_nFontSize;	
	COLORREF   m_crText;
	COLORREF   m_crBG;
	FontStyle  m_FontStyle;
	int        m_nAlign;
	
	BOOL       m_bClick;
	BOOL       m_bCursor;
	BOOL       m_bUnderLine;

	BOOL       m_DashStyle;

	BOOL       m_bGradient;
	COLORREF   m_crGrBG1;
	COLORREF   m_crGrBG2;
};


