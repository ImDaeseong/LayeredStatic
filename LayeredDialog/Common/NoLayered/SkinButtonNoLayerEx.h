#pragma once

class CSkinButtonNoLayerEx : public CButton
{
	DECLARE_DYNAMIC(CSkinButtonNoLayerEx)

public:
	CSkinButtonNoLayerEx();
	virtual ~CSkinButtonNoLayerEx();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	
	BOOL SetImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst, int nStartX, int nStartY);
	BOOL SetImg(LPCTSTR pName, int nStartX, int nStartY);
	BOOL SetImagePath(CString strPath, int nStartX, int nStartY);
    void Free();
	void Disable(BOOL Disabled = FALSE);
	BOOL Disabled();

	void SetBkColor(COLORREF crBG);

private:	
	CGdiPlusBitmapResource * m_pBGImage;

	BOOL m_Disabled;
	BOOL m_Tracking;
	BOOL m_Hovering;

	int m_nStartX;
	int m_nStartY;
	int m_Status;

	COLORREF   m_crBG;
};
