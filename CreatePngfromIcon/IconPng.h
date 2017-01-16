#pragma once

class CIconPng 
{
public:
	CIconPng();
	virtual ~CIconPng();

	void CreatePngFromIcon(CString strIcoPath);

private:
	CString GetModulePath();
	int GetEncoderClsid(const WCHAR *format, CLSID *pClsid);
	CString GetFileName(CString strFilename);
	Gdiplus::Bitmap* IconToBitmap(HICON hIcon);
	HICON LoadIcon(CString strPath);
};