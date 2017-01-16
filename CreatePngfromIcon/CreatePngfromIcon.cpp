#include "stdafx.h"
#include "CreatePngfromIcon.h"
#include "CreatePngfromIconDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCreatePngfromIconApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CCreatePngfromIconApp::CCreatePngfromIconApp()
{
	m_gdiplusToken = NULL;
}

CCreatePngfromIconApp theApp;

BOOL CCreatePngfromIconApp::InitInstance()
{
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, "CreatePngfromIconDialogInstance_");
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

	CCreatePngfromIconDlg dlg;
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

int CCreatePngfromIconApp::ExitInstance()
{
	if(m_gdiplusToken != NULL)
	{
		GdiplusShutdown(m_gdiplusToken);
		m_gdiplusToken = NULL;
	}

	CoUninitialize();

	return CWinAppEx::ExitInstance();
}

void CCreatePngfromIconApp::SetClassName()
{
	WNDCLASS wc;
	GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);;

	wc.lpszClassName = _T("CreatePngfromIconDialogModule");
	AfxRegisterClass(&wc);
}