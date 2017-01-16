#include "stdafx.h"
#include "LayeredDialog.h"
#include "MultiMessageDlg.h"

CMultiMessageDlg::CMultiMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMultiMessageDlg::IDD, pParent)
{
	m_pBackSkin = NULL;	
}

void CMultiMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_StcMessage);
}

BEGIN_MESSAGE_MAP(CMultiMessageDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CMultiMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW, 0);

	if(!LoadSkin())
	{
		CDialog::OnCancel();
		return FALSE;
	}

	CDC* pDC = GetDC();
	if(pDC)
	{
		DrawSkin(pDC);
		ReleaseDC(pDC);
	}
		 
	InitControl();

	return TRUE;  
}

void CMultiMessageDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CMultiMessageDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CMultiMessageDlg::OnMouseMove(UINT nFlags, CPoint point)
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

BOOL CMultiMessageDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CMultiMessageDlg::DrawSkin(CDC* pDC)
{
	if (m_pBackSkin->m_pBitmap == NULL)	
		return;

	Graphics gps(pDC->GetSafeHdc());
	int x = m_pBackSkin->m_pBitmap->GetWidth();
	int y = m_pBackSkin->m_pBitmap->GetHeight();
	gps.DrawImage(m_pBackSkin->m_pBitmap, Rect(0, 0, x, y) , 0, 0, x, y, UnitPixel);
}

BOOL CMultiMessageDlg::LoadSkin()
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

void CMultiMessageDlg::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

void CMultiMessageDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		m_StcMessage.SetText1("테스트 멀티라인 컨트롤4", RGB(255, 0, 0));
		m_StcMessage.Invalidate();

		KillTimer(1);
	}

	CDialog::OnTimer(nIDEvent);
}

void CMultiMessageDlg::InitControl()
{
	CRect rClient;
	GetClientRect(&rClient);

	CString strImgPath = "";

	if(!PathFileExists(strImgPath))
		m_btnClose.SetImage(MAKEINTRESOURCE(IDB_PNGPLAY), "PNG", AfxGetApp()->m_hInstance, rClient.right - 60, rClient.top + 20, 4);
	else
		m_btnClose.SetImagePath(strImgPath, rClient.right - 60, rClient.top + 20, 4);
    m_btnClose.SetBkColor(RGB(199, 199, 199));


	m_StcMessage.MoveWindow(rClient.left + 20, rClient.top + 70 , rClient.Width() - 40, rClient.Height() - 90 );
	m_StcMessage.SetFontName("굴림체");
	m_StcMessage.SetFontSize(15);
	m_StcMessage.SetBkColor(RGB(199, 199, 199));
	m_StcMessage.SetFontBold(true);
	m_StcMessage.SetText1("테스트 멀티라인 컨트롤1", RGB(0, 0, 0));
	m_StcMessage.SetText2("테스트 멀티라인 컨트롤2", RGB(100, 0, 0));
	m_StcMessage.SetText3("테스트 멀티라인 컨트롤3", RGB(0, 100, 0));

	SetTimer(1, 2000, NULL);
}
