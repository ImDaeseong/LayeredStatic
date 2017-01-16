#pragma once

class CResizePng
{
public:
	CResizePng();
	virtual ~CResizePng();

	void CreateResizePng(CString strPngPath, int nResizeWidth, int nResizeHeight);

private:	
	Bitmap* GetResizedBitmap(Gdiplus::Bitmap* pBitmap, int nResizeWidth, int nResizeHeight);
	CString GetModulePath();
	CString GetFileName(CString strFilename);
	int GetEncoderClsid(const WCHAR *format, CLSID *pClsid);
};


