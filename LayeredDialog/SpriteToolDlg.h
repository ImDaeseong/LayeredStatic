#pragma once
#include "afxwin.h"

class CSpriteToolDlg : public CDialog
{
public:
	CSpriteToolDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_SPRITETOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonLoad();

private:
	void DrawSkin(CDC* pDC);
	BOOL LoadSkin();
	void MoveLocationDialog();
	CGdiPlusBitmapResource* m_pBackSkin;	

	Bitmap* ResizeBitmap(Bitmap* pInImg, int nResizeWidth, int nResizeHeight);
	CString GetModulePath();

	void InitControl();
	void DisplayImages(Graphics& gps);
	void LoadBackgroundImage();
	void SetBackgroundImage(Graphics& gps);
	void SetTextLabel(Graphics& gps); 
	CString m_strTitle;
	CString m_strImgPath;


	BOOL m_bLoadImage; 
	Gdiplus::Bitmap* mSpritebitmap;
	Gdiplus::Bitmap* mTmpSpritebitmap;

	Rect FindRectRange( Rect range );
	Rect FindRect( Point mousePos );
	Rect TrimRect( Rect range );
	RECT FindRectRec( Point pos, Color bgColor, bool checkRange );

	bool m_bMouseDown;
	Gdiplus::Pen * mDrawPen;
	Gdiplus::Brush * mDrawBrush;
	Gdiplus::Rect mMouseMoveRect;
	Gdiplus::Rect mMouseSelectRect;
	
	CSkinButtonNoLayerEx m_btnLoadPng;

	Bitmap* CropImage(Bitmap* pSource, CRect cropRect);
	int GetEncoderClsid(const WCHAR *format, CLSID *pClsid);
	CString GetRandomString();
	void SaveSplitImage();


	// 더블버퍼링을 위한 변수
	CBitmap bmpOffScreen;			// 더블버퍼링을 위한 비트맵 객체를 만든다.
	CBitmap bmpOfTimeLine;			// 타임라인을 위한 비트맵 객체를 만든다.
	CBitmap* bmpOfTimeLineMask;		// 타임라인 마스크
	CBitmap bMaskedTimeLine;
};