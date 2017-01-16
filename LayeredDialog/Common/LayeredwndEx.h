#pragma once

class CLayeredwndEx : public CWnd
{
public:
	CLayeredwndEx(); 
	virtual ~CLayeredwndEx();

public:
	BOOL Create(CWnd* pParent);

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	void DrawSkin(CDC* pDC);
	BOOL LoadSkin();
	CGdiPlusBitmapResource* m_pBackSkin;	

	void MoveLocationDialog();	
	CString GetModulePath();

	void LoadImages(BOOL bLoad = false);
	void DisplayImg(Graphics& gps);
	CStringArray m_SpriteInfo;
	int m_nIndex;	

	CGdiPlusBitmapResource* m_pSprite;

};

