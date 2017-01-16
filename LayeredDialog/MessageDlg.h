#pragma once
#include "afxwin.h"

class CMessageDlg : public CDialog
{
public:
	CMessageDlg(CWnd* pParent = NULL); 

	enum { IDD = IDD_MESSAGE_DIALOG };

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

	void InitControl();
	CSkinButtonNoLayer m_btnClose;
	CLabelStatic m_StcMessage;
};
