#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
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