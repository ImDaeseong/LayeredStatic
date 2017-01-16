#include "stdafx.h"
#include "LayeredDialog.h"
#include "LayeredDlg.h"
#include "LayeredSubDlg.h"
#include "MessageDlg.h"
#include "MultiMessageDlg.h"
#include "PictureDlg.h"
#include "LayeredPictureDlg.h"
#include "LayeredButtonDlg.h"
#include "TrayDlg.h"
#include "LayeredTrayDlg.h"
#include "ButtononeDlg.h"
#include "LayeredStaticDlg.h"
#include "SpriteDlg.h"
#include "NoLayeredSpriteDlg.h"
#include "SpriteToolDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CLayeredDialogApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CLayeredDialogApp::CLayeredDialogApp()
{
	m_gdiplusToken = NULL;
}

CLayeredDialogApp theApp;


BOOL CLayeredDialogApp::InitInstance()
{
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, "LayeredDialogInstance_");
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

	//CLayeredDlg dlg;
	//CLayeredSubDlg dlg;
	//CMessageDlg dlg;
	//CMultiMessageDlg dlg;
	//CPictureDlg dlg;
	//CLayeredPictureDlg dlg;
	//CLayeredButtonDlg dlg;
	//CTrayDlg dlg;
	//CLayeredTrayDlg dlg;
	//CButtononeDlg dlg;
	//CLayeredStaticDlg dlg;
	CSpriteToolDlg dlg;
	//CSpriteDlg dlg;
	//CNoLayeredSpriteDlg dlg;
	
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

int CLayeredDialogApp::ExitInstance()
{
	if(m_gdiplusToken != NULL)
	{
		GdiplusShutdown(m_gdiplusToken);
		m_gdiplusToken = NULL;
	}

	CoUninitialize();

	return CWinAppEx::ExitInstance();
}

void CLayeredDialogApp::SetClassName()
{
	WNDCLASS wc;
	GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);;

	wc.lpszClassName = _T("LayeredDialogModule");
	AfxRegisterClass(&wc);
}