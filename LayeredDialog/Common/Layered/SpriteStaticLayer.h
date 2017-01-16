#pragma once

class CSpriteStaticLayer : public CStatic
{
public:
	CSpriteStaticLayer();
	virtual ~CSpriteStaticLayer();

public:
	void SetImagePath(CString strPath);
	void SetImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);
		
	BOOL IsAvailableDraw();
	void OnDrawLayerdWindow(Graphics& Gps);

	CSpriteStaticLayer& SetSpriteStaticClick(BOOL bClick = FALSE);
	void SetSpriteStaticCursor(BOOL bCursor = FALSE);
		
	void SetSprite(int nTotalIndex, int nImgWidth, int nImgHeight, int nDelayTime = 40);
	void SetRowCol(int nRow, int nCol);

protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual void PreSubclassWindow();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

private:	
	CGdiPlusBitmapResource* m_pSprite;
	BOOL  m_bClick;
	BOOL  m_bCursor;

	int m_curIndex;
	int m_TotalIndex;
	int m_nDelayTime;
	int m_nImgWidth;
	int m_nImgHeight;

	int m_nRow;
	int m_nCol;
};