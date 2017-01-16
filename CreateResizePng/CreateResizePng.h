#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"	

class CCreateResizePngApp : public CWinAppEx
{
public:
	CCreateResizePngApp();

	public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

private:
	ULONG_PTR m_gdiplusToken;
	void SetClassName();
};

extern CCreateResizePngApp theApp;