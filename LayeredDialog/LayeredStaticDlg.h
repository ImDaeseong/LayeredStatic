#pragma once
#include "afxwin.h"

class CLayeredStaticDlg : public CDialog
{
public:
	CLayeredStaticDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_LAYERED_STATIC_DIALOG };

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
	void DisplayStaticText(Graphics& gps);	

	void SetTextLabel(Graphics& gps);
	void SetTextContents(Graphics& gps);
	void SetBackgroundImage(Graphics& gps);
	void SetTextAlignLabel(Graphics& gps, int nAlign = 0);
	CString m_strTitle;
	CString m_strContents;
	CString m_strTitleAlign;
	CString m_strImgPath;
};