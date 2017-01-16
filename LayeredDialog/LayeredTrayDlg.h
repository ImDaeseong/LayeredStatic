#pragma once
#include "afxwin.h"

class CLayeredTraySubDlg;
class CLayeredTrayDlg : public CDialog
{
public:
	CLayeredTrayDlg(CWnd* pParent = NULL);	

	CLayeredTraySubDlg* m_pLayeredTraySubDlg;
	void CreateLayeredTraySubDlg();
	void DestroyLayeredTraySubDlg();
	void SetChildPos();

	enum { IDD = IDD_LAYERED_TRAY_DIALOG };

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
	afx_msg void OnMove(int x, int y);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);

private:
	void DrawSkin(CDC* pDC);
	BOOL LoadSkin();
	void MoveLocationDialog(BOOL bShow = false);
	CGdiPlusBitmapResource* m_pBackSkin;	

	void InitControl();
	bool m_bMoveDialog;	
};
