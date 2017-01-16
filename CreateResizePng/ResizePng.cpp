#include "stdafx.h"
#include "ResizePng.h"

CResizePng::CResizePng()
{
}

CResizePng::~CResizePng()
{
}

void CResizePng::CreateResizePng(CString strPngPath, int nResizeWidth, int nResizeHeight)
{
	USES_CONVERSION;

	CString strFolderPath;
	strFolderPath.Format("%s\\ResizePng", GetModulePath());
    if(!::PathFileExists(strFolderPath))
		CreateDirectory(strFolderPath, NULL);

	CString strNewFilePath;
	strNewFilePath.Format("%s\\%s", strFolderPath, GetFileName(strPngPath));

	Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromFile(A2W(strPngPath));
	if (pBitmap)
	{
		pBitmap = GetResizedBitmap(pBitmap, nResizeWidth, nResizeHeight);
	
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


		int nSizeCount = MultiByteToWideChar(CP_ACP, 0, strNewFilePath, -1, NULL, 0);
		WCHAR * pWString = new WCHAR[nSizeCount];
		MultiByteToWideChar(CP_ACP, 0, strNewFilePath, -1, pWString, nSizeCount);

		GetEncoderClsid(L"image/png", &encoderClsid);
		pBitmap->Save(pWString, &encoderClsid, &encoderParameters);

		delete pBitmap;		
	}
}

Bitmap* CResizePng::GetResizedBitmap(Gdiplus::Bitmap* pBitmap, int nResizeWidth, int nResizeHeight)
{
	UINT uHeight = pBitmap->GetHeight();
	UINT uWidth  = pBitmap->GetWidth();
	INT nWidth    = nResizeWidth;
	INT nHeight   = nResizeHeight;

	double dRatio = ((double)uWidth) / ((double)uHeight);
	if (uWidth > uHeight)
	{
		nHeight = static_cast<UINT>(((double)uWidth) / dRatio);
	}
	else 
	{
		nWidth = static_cast<UINT>(nHeight * dRatio);
	}
	
	Bitmap* newpBitmap = new Gdiplus::Bitmap(nWidth, nHeight, pBitmap->GetPixelFormat());
	Graphics graphics(newpBitmap);
	graphics.DrawImage(pBitmap, 0, 0, nWidth, nHeight);
	return newpBitmap;
}

CString CResizePng::GetModulePath()
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

CString CResizePng::GetFileName(CString strFilename)
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

int CResizePng::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
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
