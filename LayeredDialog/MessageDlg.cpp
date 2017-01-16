#include "stdafx.h"
#include "LayeredDialog.h"
#include "MessageDlg.h"

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageDlg::IDD, pParent)
{
	m_pBackSkin = NULL;
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_StcMessage);
}

BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CMessageDlg::OnInitDialog()
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

void CMessageDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CMessageDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CMessageDlg::OnMouseMove(UINT nFlags, CPoint point)
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

BOOL CMessageDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CMessageDlg::DrawSkin(CDC* pDC)
{
	if (m_pBackSkin->m_pBitmap == NULL)	
		return;

	Graphics gps(pDC->GetSafeHdc());
	int x = m_pBackSkin->m_pBitmap->GetWidth();
	int y = m_pBackSkin->m_pBitmap->GetHeight();
	gps.DrawImage(m_pBackSkin->m_pBitmap, Rect(0, 0, x, y) , 0, 0, x, y, UnitPixel);
}

BOOL CMessageDlg::LoadSkin()
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

void CMessageDlg::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

void CMessageDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		m_StcMessage.SetText("테스트 컨트롤");
		m_StcMessage.SetFontSize(20);
		m_StcMessage.SetTextColor(RGB(255, 0, 0));
		m_StcMessage.SetFontBold(false);
		m_StcMessage.Invalidate();

		KillTimer(1);
	}

	CDialog::OnTimer(nIDEvent);
}

void CMessageDlg::InitControl()
{
	CRect rClient;
	GetClientRect(&rClient);

	CString strImgPath = "";

	if(!PathFileExists(strImgPath))
		m_btnClose.SetImage(MAKEINTRESOURCE(IDB_PNG_CLOSE), "PNG", AfxGetApp()->m_hInstance, rClient.right - 30, rClient.top + 5, 1);
	else
		m_btnClose.SetImagePath(strImgPath, rClient.right - 30, rClient.top + 5, 1);
    m_btnClose.SetBkColor(RGB(210, 210, 210));

	m_StcMessage.MoveWindow(rClient.left + 20, rClient.top + 30 , rClient.Width() - 40, rClient.Height() - 60 );
	m_StcMessage.SetFontName("궁서체");
	m_StcMessage.SetFontSize(40);
	m_StcMessage.SetTextColor(RGB(0, 0, 0));
	m_StcMessage.SetBkColor(RGB(199, 199, 199));
	m_StcMessage.SetFontBold(true);
	m_StcMessage.SetStringAlign(1);	
	m_StcMessage.SetText("테스트 컨트롤\r\n테스트 컨트롤\r\n테스트 컨트롤");	

	m_StcMessage.SetGradient(true, RGB(255, 0, 0), RGB(0, 0, 255));

	SetTimer(1, 2000, NULL);
}
