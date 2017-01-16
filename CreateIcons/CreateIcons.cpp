#include "stdafx.h"
#include "CreateIcons.h"
#include "CreateIconsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCreateIconsApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CCreateIconsApp::CCreateIconsApp()
{
	m_gdiplusToken = NULL;
}

CCreateIconsApp theApp;

BOOL CCreateIconsApp::InitInstance()
{	
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, "CreateIconDialogInstance_");
	if (ERROR_ALREADY_EXISTS == ::GetLastError())
		return FALSE;

	CoInitialize(NULL);

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);


	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	SetClassName();

	CCreateIconsDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	return FALSE;
}

int CCreateIconsApp::ExitInstance()
{
	if(m_gdiplusToken != NULL)
	{
		GdiplusShutdown(m_gdiplusToken);
		m_gdiplusToken = NULL;
	}

	CoUninitialize();

	return CWinAppEx::ExitInstance();
}

void CCreateIconsApp::SetClassName()
{
	WNDCLASS wc;
	GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);;

	wc.lpszClassName = _T("CreateDialogModule");
	AfxRegisterClass(&wc);
}