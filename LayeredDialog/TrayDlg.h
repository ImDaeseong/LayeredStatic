#pragma once
#include "afxwin.h"

#include "webbrowser2.h"
#include <mshtml.h>


class CTrayDlg : public CDialog
{
public:
	CTrayDlg(CWnd* pParent = NULL); 

	enum { IDD = IDD_TRAY_DIALOG };

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
	void MoveLocationDialog(BOOL bShow = false);
	CGdiPlusBitmapResource* m_pBackSkin;	

	void InitControl();

	bool m_bMoveDialog;

	CWebBrowser2 m_WebBrowser;
	void Navigate(CString strUrl);
};