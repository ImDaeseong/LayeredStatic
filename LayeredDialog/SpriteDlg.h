#pragma once
#include "afxwin.h"

class CSpriteDlg : public CDialog
{
public:
	CSpriteDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_SPRITE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	void DrawSkin(CDC* pDC);
	BOOL LoadSkin();
	void MoveLocationDialog();
	CGdiPlusBitmapResource* m_pBackSkin;	
			
	CLayeredwnd* m_pLayeredWnd;
	BOOL CreateLayerWnd();
	void DestoryLayerWnd();
	
    CLayeredwndEx* m_pLayeredWndEx;
	BOOL CreateLayerWndEx();
	void DestoryLayerWndEx();

	CSpriteWnd* m_pSpriteLayeredWnd;
	BOOL CreateSpriteLayerWnd();
	void DestorySpriteLayerWnd();

	void InitControl();
	CSpriteStaticLayer m_StaticSprite;
};