#pragma once
#include "afxwin.h"

#include "webbrowser2.h"
#include <mshtml.h>
#include "LayeredTrayDlg.h"

class CLayeredTraySubDlg : public CDialog
{
public:
	CLayeredTraySubDlg(CWnd* pParent = NULL); 
	
	void SetParent(CLayeredTrayDlg* pParent);
	int  GetWidth();
	int  GetHeight();

	enum { IDD = IDD_LAYERED_TRAY_SUBDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnDestroy();

private:
	void DrawSkin(CDC* pDC);
	BOOL LoadSkin();
	void MoveLocationDialog();
	CGdiPlusBitmapResource* m_pBackSkin;	

	void InitControl();

	CWebBrowser2 m_WebBrowser;
	void Navigate(CString strUrl);

	CLayeredTrayDlg* m_pParent;
};