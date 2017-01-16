#pragma once
#include "afxwin.h"

class CCreateIconsDlg : public CDialog
{
public:
	CCreateIconsDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_CREATEICONS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	

protected:	
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
	    
private:
	void DrawSkin(CDC* pDC);
	BOOL LoadSkin();
	void MoveLocationDialog();
	CGdiPlusBitmapResource* m_pBackSkin;	
	void DisplayStaticText(Graphics& gps);
	CString GetFileExtName(CString strFilename);
};
