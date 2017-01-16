#include "stdafx.h"
#include "IconPng.h"

CIconPng::CIconPng()
{
}

CIconPng::~CIconPng()
{
}

void CIconPng::CreatePngFromIcon(CString strIcoPath)
{
	CString strFolderPath;
	strFolderPath.Format("%s\\IconsPng", GetModulePath());
    if(!::PathFileExists(strFolderPath))
		CreateDirectory(strFolderPath, NULL);

	CString strNewFilePath;
	strNewFilePath.Format("%s\\%s", strFolderPath, GetFileName(strIcoPath));

	HICON hico = LoadIcon(strIcoPath);
	if(hico)
	{
		Gdiplus::Bitmap* pBitmap = IconToBitmap(hico);
		if (pBitmap)
		{
			TCHAR szOutputIconPath[MAX_PATH];
			lstrcpy(szOutputIconPath, strNewFilePath);
			PathRemoveExtension(szOutputIconPath);
			lstrcat(szOutputIconPath, TEXT(".png"));
						

			CLSID				encoderClsid;
			EncoderParameters	encoderParameters;
			ULONG				quality;

			encoderParameters.Count = 1;
			encoderParameters.Parameter[0].Guid = EncoderQuality;
			encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
			encoderParameters.Parameter[0].NumberOfValues = 1;

			// 이미지 png 저장시 Quality level 100.
			quality = 100;
			encoderParameters.Parameter[0].Value = &quality;


			int nSizeCount = MultiByteToWideChar(CP_ACP, 0, szOutputIconPath, -1, NULL, 0);
			WCHAR * pWString = new WCHAR[nSizeCount];
			MultiByteToWideChar(CP_ACP, 0, szOutputIconPath, -1, pWString, nSizeCount);

			GetEncoderClsid(L"image/png", &encoderClsid);
			pBitmap->Save(pWString, &encoderClsid, &encoderParameters);

			delete pBitmap;		
		}
	}
}

CString CIconPng::GetModulePath()
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

int CIconPng::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
	UINT num = 0;
	UINT size = 0;
	ImageCodecInfo * pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo *)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return -1;
}

CString CIconPng::GetFileName(CString strFilename)
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

Gdiplus::Bitmap* CIconPng::IconToBitmap(HICON hIcon)
{	
	// Get the icon info
	ICONINFO iconInfo = { 0 };
	GetIconInfo(hIcon, &iconInfo);

	// Get the screen DC
	HDC dc = GetDC(NULL);

	// Get icon size info
	BITMAP bm = { 0 };
	GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);

	// Set up BITMAPINFO
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bm.bmWidth;
	bmi.bmiHeader.biHeight = -bm.bmHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	// Extract the color bitmap
	int nBits = bm.bmWidth * bm.bmHeight;
	INT32* colorBits = new INT32[nBits];
	GetDIBits(dc, iconInfo.hbmColor, 0, bm.bmHeight, colorBits, &bmi, DIB_RGB_COLORS);

	// Check whether the color bitmap has an alpha channel.
	// (On my Windows 7, all file icons I tried have an alpha channel.)
	BOOL hasAlpha = FALSE;
	for (int i = 0; i < nBits; i++)
	{
		if ((colorBits[i] & 0xff000000) != 0) 
		{
			hasAlpha = TRUE;
			break;
		}
	}

	// If no alpha values available, apply the mask bitmap
	if (!hasAlpha)
	{
		// Extract the mask bitmap
		INT32* maskBits = new INT32[nBits];
		GetDIBits(dc, iconInfo.hbmMask, 0, bm.bmHeight, maskBits, &bmi, DIB_RGB_COLORS);
		// Copy the mask alphas into the color bits
		for (int i = 0; i < nBits; i++) 
		{
			if (maskBits[i] == 0)
			{
				colorBits[i] |= 0xff000000;
			}
		}
		delete[] maskBits;
	}

	// Release DC and GDI bitmaps
	ReleaseDC(NULL, dc);
	::DeleteObject(iconInfo.hbmColor);
	::DeleteObject(iconInfo.hbmMask);

	// Create GDI+ Bitmap
	Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4, PixelFormat32bppARGB, (BYTE*)colorBits);

	return bmp;
}

HICON CIconPng::LoadIcon(CString strPath)
{
	HICON hIcon = NULL;
	SHFILEINFO sfi;
	sfi.hIcon = NULL;
	
	if(SHGetFileInfo(strPath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_LARGEICON) == 0)
		return NULL;
		
	hIcon = sfi.hIcon;
	return hIcon;	
}