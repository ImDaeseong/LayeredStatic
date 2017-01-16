#pragma once

class CLayeredStatic : public CStatic
{
public:
	CLayeredStatic();
	virtual ~CLayeredStatic();

public:	
	void SetImagePath(CString strBackPath, CString strPath);
	
protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

private:
	void FreeImage();
	void FreeBackImage();
	
	CGdiPlusBitmapResource* m_pImage;
	CGdiPlusBitmapResource* m_pBackImage;
};


