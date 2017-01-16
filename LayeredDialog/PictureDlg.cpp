#include "stdafx.h"
#include "LayeredDialog.h"
#include "PictureDlg.h"

CPictureDlg::CPictureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPictureDlg::IDD, pParent)
{
	m_pBackSkin = NULL;
}

void CPictureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PICTURE, m_SticPicture);
}

BEGIN_MESSAGE_MAP(CPictureDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CPictureDlg::OnInitDialog()
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

void CPictureDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CPictureDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CPictureDlg::OnMouseMove(UINT nFlags, CPoint point)
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

BOOL CPictureDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CPictureDlg::DrawSkin(CDC* pDC)
{
	if (m_pBackSkin->m_pBitmap == NULL)	
		return;

	Graphics gps(pDC->GetSafeHdc());
	int x = m_pBackSkin->m_pBitmap->GetWidth();
	int y = m_pBackSkin->m_pBitmap->GetHeight();
	gps.DrawImage(m_pBackSkin->m_pBitmap, Rect(0, 0, x, y) , 0, 0, x, y, UnitPixel);
}

BOOL CPictureDlg::LoadSkin()
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

void CPictureDlg::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

void CPictureDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		m_SticPicture.SetPictureStaticClick(TRUE);
		m_SticPicture.SetPictureStaticCursor(TRUE);

		KillTimer(1);
	}

	CDialog::OnTimer(nIDEvent);
}

void CPictureDlg::InitControl()
{
	CRect rClient;
	GetClientRect(&rClient);

	m_SticPicture.MoveWindow(rClient.left + 20, rClient.top + 20 , rClient.Width() - 40, rClient.Height() - 40 );
	m_SticPicture.SetPictureStaticClick(FALSE);
	m_SticPicture.SetPictureStaticCursor(FALSE);

	CString strImgPath = "D:\\피카바탕화면런처\\bin\\Release\\Imgs\\item_left1.png";

	if(!PathFileExists(strImgPath))
		m_SticPicture.SetImage(MAKEINTRESOURCE(IDB_PNGPLAY), "PNG", AfxGetApp()->m_hInstance);
	else
		m_SticPicture.SetImagePath(strImgPath);


	SetTimer(1, 2000, NULL);
}

