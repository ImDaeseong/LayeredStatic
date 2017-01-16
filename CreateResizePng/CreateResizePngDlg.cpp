#include "stdafx.h"
#include "CreateResizePng.h"
#include "CreateResizePngDlg.h"

CCreateResizePngDlg::CCreateResizePngDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateResizePngDlg::IDD, pParent)
{
	m_pBackSkin = NULL;
}

void CCreateResizePngDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCreateResizePngDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CCreateResizePngDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW, 0);

	if(!LoadSkin())
	{
		CDialog::OnCancel();
		return FALSE;
	}

	DragAcceptFiles(TRUE);

	CDC* pDC = GetDC();
	if(pDC)
	{
		DrawSkin(pDC);
		ReleaseDC(pDC);
	}
		 
	InitControl();

	return TRUE;  
}

void CCreateResizePngDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CCreateResizePngDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CCreateResizePngDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags & MK_LBUTTON)
	{	
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE,  0,0);
	}
	else
	{
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE,  0,0);
	}	

	CDialog::OnMouseMove(nFlags, point);
}

BOOL CCreateResizePngDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CCreateResizePngDlg::DrawSkin(CDC* pDC)
{
	if (m_pBackSkin->m_pBitmap == NULL)	
		return;

	Graphics gps(pDC->GetSafeHdc());
	int x = m_pBackSkin->m_pBitmap->GetWidth();
	int y = m_pBackSkin->m_pBitmap->GetHeight();
	gps.DrawImage(m_pBackSkin->m_pBitmap, Rect(0, 0, x, y) , 0, 0, x, y, UnitPixel);
}

BOOL CCreateResizePngDlg::LoadSkin()
{	
	m_pBackSkin = new CGdiPlusBitmapResource;
	if(!m_pBackSkin->LoadPath(""))
	{
		if(!m_pBackSkin->Load(MAKEINTRESOURCE(IDB_PNG_BG), _T("PNG"), AfxGetApp()->m_hInstance))
		{
			delete m_pBackSkin;
			m_pBackSkin = NULL;
			return FALSE;
		}
	}

	MoveLocationDialog();

	return TRUE;
}

void CCreateResizePngDlg::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

void CCreateResizePngDlg::InitControl()
{

}

CString CCreateResizePngDlg::GetFileExtName(CString strFilename)
{
	int nPos = strFilename.ReverseFind('.');
	if(nPos > 0)
		return strFilename.Mid(nPos + 1);
	return strFilename;
}

void CCreateResizePngDlg::OnDropFiles(HDROP hDropInfo)
{	
	CStringArray aryIcolst;
	
	TCHAR szFilePathName[MAX_PATH];
	UINT uFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);  

	for (UINT count = 0; count < uFileCount; count++)
	{	
		DragQueryFile(hDropInfo, count, szFilePathName, _MAX_PATH); 

		if( lstrcmpi(GetFileExtName(szFilePathName), "png") == 0 )
			aryIcolst.Add(szFilePathName);
	}
	

	CString strWidth, strHeight;
	GetDlgItemText(IDC_EDIT_WIDTH, strWidth);
	GetDlgItemText(IDC_EDIT_HEIGHT, strHeight);	

	int nResizeWidth = 0;
	if(strWidth != "")
		nResizeWidth = atoi(strWidth);

	int nResizeHeight = 0;
	if(strHeight != "")
		nResizeHeight = atoi(strHeight);

    CResizePng mPng;
	CString strPngName = "";
	for(int i=0; i < aryIcolst.GetCount(); i++)
	{		
		strPngName = aryIcolst.GetAt(i);	
		mPng.CreateResizePng(strPngName, nResizeWidth, nResizeHeight);
	}
	aryIcolst.RemoveAll();
	

	::ShellExecute(NULL, "open", "", "", NULL, SW_SHOWNORMAL);
	Sleep(1000);
	CDialog::OnOK();


	CDialog::OnDropFiles(hDropInfo);
}