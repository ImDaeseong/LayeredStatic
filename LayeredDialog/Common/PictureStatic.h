#pragma once

class CPictureStatic : public CStatic
{
public:
	CPictureStatic();
	virtual ~CPictureStatic();

public:
	void SetImagePath(CString strPath);
	void SetImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);
		
	BOOL IsAvailableDraw();
	void OnDrawLayerdWindow(Graphics& Gps);

	CPictureStatic& SetPictureStaticClick(BOOL bClick = FALSE);
	void SetPictureStaticCursor(BOOL bCursor = FALSE);
	
protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()

private:	
	CGdiPlusBitmapResource* m_pImage;
	BOOL  m_bClick;
	BOOL  m_bCursor;
};


