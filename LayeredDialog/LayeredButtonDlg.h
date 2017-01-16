#pragma once
#include "afxwin.h"

class CLayeredButtonDlg : public CDialog
{
public:
	CLayeredButtonDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_LAYERED_BUTTON_DIALOG };

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
	CSkinButtonLayer m_btnClose;
};