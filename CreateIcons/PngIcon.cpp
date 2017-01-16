/*
CreateIconLib
Copyright (C) 2016
https://github.com/kenjinote/CreateIconFromImage/blob/master/Source.cpp
*/

#include "stdafx.h"
#include "PngIcon.h"

CPngIcon::CPngIcon()
{
}

CPngIcon::~CPngIcon()
{
}

void CPngIcon::CreateIcon(CString strImgPath)
{
	USES_CONVERSION;
	
	CString strFolderPath;
	strFolderPath.Format("%s\\Icons", GetModulePath());
    if(!::PathFileExists(strFolderPath))
		CreateDirectory(strFolderPath, NULL);

	CString strNewFilePath;
	strNewFilePath.Format("%s\\%s", strFolderPath, GetFileName(strImgPath));
	
	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(A2W(strImgPath));
	if (bitmap)
	{	
		HICON hIcon[7] = { 0 };
		int nTotalCount = 0;
		int listSize[7] = { 16, 32, 48, 64, 96, 128, 256 };
		for (int i = 0; i < 7; ++i)
		{
			if(i == 5)//128bit¸¸ »ý¼º
			{
				hIcon[nTotalCount] = CreateAlphaIcon(bitmap, listSize[i]);
				++nTotalCount;
			}
		}
		TCHAR szOutputIconPath[MAX_PATH];
		lstrcpy(szOutputIconPath, strNewFilePath);
		PathRemoveExtension(szOutputIconPath);
		lstrcat(szOutputIconPath, TEXT(".ico"));
		SaveIcon(szOutputIconPath, hIcon, nTotalCount);
		for (int i = 0; i < nTotalCount; ++i)
		{
			DestroyIcon(hIcon[i]);
		}
		delete bitmap;
	}
}

CString CPngIcon::GetFileName(CString strFilename)
{
	CString strFullName = strFilename;
	int nFindExt=0;
	int i=0;
	for(i=0; i < strFullName.GetLength(); )
	{
		nFindExt = strFullName.Find('\\', nFindExt);
		if(nFindExt == -1) break;
		nFindExt++;
		i = nFindExt;
	}

	if(i > 0)
		return strFullName.Mid(i, strFullName.GetLength());
	else 
		return strFullName;
}

CString CPngIcon::GetModulePath()
{
	char cTemp[MAX_PATH];
	char *spzRealDirEnd;
	CString strModulePath;

	GetModuleFileName(NULL, cTemp, MAX_PATH);  
	spzRealDirEnd = strrchr(cTemp, '\\');
	*spzRealDirEnd = '\0';
	strModulePath = (CString)cTemp;
	return strModulePath;
}

UINT CPngIcon::WriteIconHeader(HANDLE hFile, int nImages)
{
	ICONHEADER iconheader = { 0 };
	DWORD nWritten;
	iconheader.idReserved = 0;
	iconheader.idType = 1;
	iconheader.idCount = nImages;
	WriteFile(hFile, &iconheader, sizeof(iconheader), &nWritten, 0);
	return nWritten;
}

UINT CPngIcon::NumBitmapBytes(BITMAP *pBitmap)
{
	int nWidthBytes = pBitmap->bmWidthBytes;
	if (nWidthBytes & 3)
		nWidthBytes = (nWidthBytes + 4) & ~3;
	return nWidthBytes * pBitmap->bmHeight;
}

UINT CPngIcon::WriteIconImageHeader(HANDLE hFile, BITMAP *pbmpColor, BITMAP *pbmpMask)
{
	BITMAPINFOHEADER biHeader = { 0 };
	DWORD nWritten;
	const UINT nImageBytes = NumBitmapBytes(pbmpColor) + NumBitmapBytes(pbmpMask);
	ZeroMemory(&biHeader, sizeof(biHeader));
	biHeader.biSize = sizeof(biHeader);
	biHeader.biWidth = pbmpColor->bmWidth;
	biHeader.biHeight = pbmpColor->bmHeight * 2;
	biHeader.biPlanes = pbmpColor->bmPlanes;
	biHeader.biBitCount = pbmpColor->bmBitsPixel;
	biHeader.biSizeImage = nImageBytes;
	WriteFile(hFile, &biHeader, sizeof(biHeader), &nWritten, 0);
	return nWritten;
}

BOOL CPngIcon::GetIconBitmapInfo(HICON hIcon, ICONINFO *pIconInfo, BITMAP *pbmpColor, BITMAP *pbmpMask)
{
	if (!GetIconInfo(hIcon, pIconInfo))
		return FALSE;
	if (!GetObject(pIconInfo->hbmColor, sizeof(BITMAP), pbmpColor))
		return FALSE;
	if (!GetObject(pIconInfo->hbmMask, sizeof(BITMAP), pbmpMask))
		return FALSE;
	return TRUE;
}

UINT CPngIcon::WriteIconDirectoryEntry(HANDLE hFile, int nIdx, HICON hIcon, UINT nImageOffset)
{
	ICONINFO iconInfo = { 0 };
	ICONDIR iconDir = { 0 };
	BITMAP bmpColor = { 0 };
	BITMAP bmpMask = { 0 };
	DWORD nWritten;
	UINT nColorCount;
	GetIconBitmapInfo(hIcon, &iconInfo, &bmpColor, &bmpMask);
	const UINT nImageBytes = NumBitmapBytes(&bmpColor) + NumBitmapBytes(&bmpMask);
	if (bmpColor.bmBitsPixel >= 8)
		nColorCount = 0;
	else
		nColorCount = 1 << (bmpColor.bmBitsPixel * bmpColor.bmPlanes);
	iconDir.bWidth = (BYTE)bmpColor.bmWidth;
	iconDir.bHeight = (BYTE)bmpColor.bmHeight;
	iconDir.bColorCount = nColorCount;
	iconDir.bReserved = 0;
	iconDir.wPlanes = bmpColor.bmPlanes;
	iconDir.wBitCount = bmpColor.bmBitsPixel;
	iconDir.dwBytesInRes = sizeof(BITMAPINFOHEADER) + nImageBytes;
	iconDir.dwImageOffset = nImageOffset;
	WriteFile(hFile, &iconDir, sizeof(iconDir), &nWritten, 0);
	DeleteObject(iconInfo.hbmColor);
	DeleteObject(iconInfo.hbmMask);
	return nWritten;
}

UINT CPngIcon::WriteIconData(HANDLE hFile, HBITMAP hBitmap)
{
	BITMAP bmp = { 0 };
	DWORD nWritten;
	GetObject(hBitmap, sizeof(BITMAP), &bmp);
	UINT nBitmapBytes = NumBitmapBytes(&bmp);
	BYTE* pIconData = (BYTE*)malloc(nBitmapBytes);
	GetBitmapBits(hBitmap, nBitmapBytes, pIconData);
	for (int i = bmp.bmHeight - 1; i >= 0; --i)
	{
		WriteFile(
			hFile,
			pIconData + (i * bmp.bmWidthBytes),
			bmp.bmWidthBytes,
			&nWritten,
			0);
		if (bmp.bmWidthBytes & 3)
		{
			DWORD padding = 0;
			WriteFile(hFile, &padding, 4 - (bmp.bmWidthBytes & 3), &nWritten, 0);
		}
	}
	free(pIconData);
	return nBitmapBytes;
}

BOOL CPngIcon::SaveIcon(TCHAR *szIconFile, HICON hIcon[], int nNumIcons)
{
	if (hIcon == 0 || nNumIcons < 1)
		return FALSE;
	HANDLE hFile = CreateFile(szIconFile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	WriteIconHeader(hFile, nNumIcons);
	SetFilePointer(hFile, sizeof(ICONDIR) * nNumIcons, 0, FILE_CURRENT);
	int* pImageOffset = (int*)malloc(nNumIcons * sizeof(int));
	for (int i = 0; i < nNumIcons; ++i)
	{
		ICONINFO iconInfo = { 0 };
		BITMAP bmpColor = { 0 }, bmpMask = { 0 };
		GetIconBitmapInfo(hIcon[i], &iconInfo, &bmpColor, &bmpMask);
		pImageOffset[i] = SetFilePointer(hFile, 0, 0, FILE_CURRENT);
		WriteIconImageHeader(hFile, &bmpColor, &bmpMask);
		WriteIconData(hFile, iconInfo.hbmColor);
		WriteIconData(hFile, iconInfo.hbmMask);
		DeleteObject(iconInfo.hbmColor);
		DeleteObject(iconInfo.hbmMask);
	}
	SetFilePointer(hFile, sizeof(ICONHEADER), 0, FILE_BEGIN);
	for (int i = 0; i < nNumIcons; ++i)
	{
		WriteIconDirectoryEntry(hFile, i, hIcon[i], pImageOffset[i]);
	}
	free(pImageOffset);
	CloseHandle(hFile);
	return TRUE;
}

HICON CPngIcon::CreateAlphaIcon(Gdiplus::Bitmap *pImg, DWORD dwSize)
{
	BITMAPV5HEADER bi = { 0 };
	void *lpBits;
	HICON hAlphaIcon = NULL;
	ZeroMemory(&bi, sizeof(BITMAPV5HEADER));
	bi.bV5Size = sizeof(BITMAPV5HEADER);
	bi.bV5Width = dwSize;
	bi.bV5Height = dwSize;
	bi.bV5Planes = 1;
	bi.bV5BitCount = 32;
	bi.bV5Compression = BI_BITFIELDS;
	bi.bV5RedMask = 0x00FF0000;
	bi.bV5GreenMask = 0x0000FF00;
	bi.bV5BlueMask = 0x000000FF;
	bi.bV5AlphaMask = 0xFF000000;
	bi.bV5CSType = LCS_WINDOWS_COLOR_SPACE;
	bi.bV5Endpoints.ciexyzRed.ciexyzX =
		bi.bV5Endpoints.ciexyzGreen.ciexyzX =
		bi.bV5Endpoints.ciexyzBlue.ciexyzX = 0;
	bi.bV5Endpoints.ciexyzRed.ciexyzY =
		bi.bV5Endpoints.ciexyzGreen.ciexyzY =
		bi.bV5Endpoints.ciexyzBlue.ciexyzY = 0;
	bi.bV5Endpoints.ciexyzRed.ciexyzZ =
		bi.bV5Endpoints.ciexyzGreen.ciexyzZ =
		bi.bV5Endpoints.ciexyzBlue.ciexyzZ = 0;
	bi.bV5GammaRed = 0;
	bi.bV5GammaGreen = 0;
	bi.bV5GammaBlue = 0;
	bi.bV5Intent = LCS_GM_IMAGES;
	bi.bV5ProfileData = 0;
	bi.bV5ProfileSize = 0;
	bi.bV5Reserved = 0;

	const HDC hdc = GetDC(NULL);//CDC* pDC = GetDC();
	const HBITMAP hBitmap = CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO *>(&bi),//const HBITMAP hBitmap = CreateDIBSection(pDC->GetSafeHdc(), reinterpret_cast<BITMAPINFO *>(&bi),
		DIB_RGB_COLORS,
		&lpBits, NULL, DWORD(0));
	const HDC hMemDC = CreateCompatibleDC(hdc);//const HDC hMemDC = CreateCompatibleDC(pDC->GetSafeHdc());
	ReleaseDC(NULL, hdc);//ReleaseDC(pDC);
	
	const HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hMemDC, hBitmap));
	{	
		Gdiplus::Bitmap *bitmap = new Gdiplus::Bitmap(dwSize, dwSize, PixelFormat32bppARGB);
		{
			Gdiplus::Graphics * g = Gdiplus::Graphics::FromImage(bitmap);
			g->Clear(Gdiplus::Color(0, 255, 255, 255));
			g->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
			g->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
			g->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
			const float w = (float)dwSize * min((float)pImg->GetWidth() / (float)pImg->GetHeight(), 1.0f);
			const float h = (float)dwSize * min((float)pImg->GetHeight() / (float)pImg->GetWidth(), 1.0f);
			const float x = (dwSize - w) / 2.0f;
			const float y = (dwSize - h) / 2.0f;
			g->DrawImage(pImg, x, y, w, h);
			delete g;
		}
		Gdiplus::BitmapData bitmapdata;
		const Gdiplus::Rect rect(0, 0, dwSize, dwSize);
		bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapdata);
		const int stride = bitmapdata.Stride;
		const UINT* pixel = (UINT*)bitmapdata.Scan0;
		const byte* p = (byte*)pixel;
		{
			DWORD* lpdwPixel = (DWORD*)lpBits;
			for (int y = dwSize - 1; y >= 0; --y)
			{
				for (int x = 0; x < (int)dwSize; ++x)
				{
					for (int i = 0; i < 4; ++i)
					{
						*lpdwPixel |= (p[4 * x + i + y*stride] << (8*i));
					}
					lpdwPixel++;
				}
			}
		}
		bitmap->UnlockBits(&bitmapdata);
		delete bitmap;
	}
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
	HBITMAP hMonoBitmap = CreateBitmap(dwSize, dwSize, 1, 1, NULL);
	ICONINFO ii = { 0 };
	ii.fIcon = TRUE;
	ii.xHotspot = 0;
	ii.yHotspot = 0;
	ii.hbmMask = hMonoBitmap;
	ii.hbmColor = hBitmap;
	hAlphaIcon = CreateIconIndirect(&ii);
	DeleteObject(hBitmap);
	DeleteObject(hMonoBitmap);
	return hAlphaIcon;
}