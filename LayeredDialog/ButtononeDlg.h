#pragma once
#include "afxwin.h"

class CButtononeDlg : public CDialog
{
public:
	CButtononeDlg(CWnd* pParent = NULL); 

	enum { IDD = IDD_BUTTONONE_DIALOG };

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

private:
	void DrawSkin(CDC* pDC);
	BOOL LoadSkin();
	void MoveLocationDialog();
	CGdiPlusBitmapResource* m_pBackSkin;	

	void InitControl();
	CSkinButtonNoLayerEx m_btnClose;
	CSkinButtonAlphaNoLayer m_btnAlpha;
};