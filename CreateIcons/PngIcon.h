/*
CreateIconLib
Copyright (C) 2016
https://github.com/kenjinote/CreateIconFromImage/blob/master/Source.cpp
*/

#pragma once

typedef struct
{	
	WORD idReserved;
	WORD idType;
	WORD idCount;
} ICONHEADER;

typedef struct
{
	BYTE bWidth;
	BYTE bHeight;
	BYTE bColorCount;
	BYTE bReserved;
	WORD wPlanes;
	WORD wBitCount;
	DWORD dwBytesInRes;
	DWORD dwImageOffset;
} ICONDIR;

typedef struct
{
	BITMAPINFOHEADER biHeader;
} ICONIMAGE;

class CPngIcon 
{
public:
	CPngIcon();
	virtual ~CPngIcon();

	void CreateIcon(CString strImgPath);	
	CString GetFileName(CString strFilename);
	CString GetModulePath();

	UINT WriteIconHeader(HANDLE hFile, int nImages);
	UINT NumBitmapBytes(BITMAP *pBitmap);
	UINT WriteIconImageHeader(HANDLE hFile, BITMAP *pbmpColor, BITMAP *pbmpMask);
	BOOL GetIconBitmapInfo(HICON hIcon, ICONINFO *pIconInfo, BITMAP *pbmpColor, BITMAP *pbmpMask);
	UINT WriteIconDirectoryEntry(HANDLE hFile, int nIdx, HICON hIcon, UINT nImageOffset);
	UINT WriteIconData(HANDLE hFile, HBITMAP hBitmap);
	BOOL SaveIcon(TCHAR *szIconFile, HICON hIcon[], int nNumIcons);
	HICON CreateAlphaIcon(Bitmap *pImg, DWORD dwSize);
};


