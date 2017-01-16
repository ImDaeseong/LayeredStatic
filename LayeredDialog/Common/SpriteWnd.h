#pragma once

class CSpriteWnd : public CWnd
{
	DECLARE_DYNAMIC(CSpriteWnd)

public:
	CSpriteWnd();
	virtual ~CSpriteWnd();

	BOOL Create(CWnd* pParent);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	void DrawSkin(CDC* pDC);
	BOOL LoadSkin();
	CGdiPlusBitmapResource* m_pBackSkin;	

	void MoveLocationDialog();	
	CString GetModulePath();

	void LoadSpriteImage();
	void DrawSprite(Graphics& gps);
	void Start();
	void Stop();
	int m_curIndex;
	int m_TotalIndex;

	CGdiPlusBitmapResource* m_pSprite;	
};


