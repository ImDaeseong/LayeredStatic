#include "stdafx.h"
#include "LayeredDialog.h"
#include "LayeredStaticDlg.h"

CLayeredStaticDlg::CLayeredStaticDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLayeredStaticDlg::IDD, pParent)
{
	m_pBackSkin	= NULL;	

	m_strTitle = "";
	m_strContents = "";
	m_strTitleAlign = "";
	m_strImgPath = "";
}

void CLayeredStaticDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLayeredStaticDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CLayeredStaticDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW | WS_EX_LAYERED);

	if(!LoadSkin())
	{
		CDialog::OnCancel();
		return FALSE;
	}

	InitControl();

	CDC* pDC = GetDC();
	DrawSkin(pDC);
	ReleaseDC(pDC);
	
	return TRUE;  
}

void CLayeredStaticDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CLayeredStaticDlg::OnDestroy()
{
	CDialog::OnDestroy();

	KillTimer(1);

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CLayeredStaticDlg::OnMouseMove(UINT nFlags, CPoint point)
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

BOOL CLayeredStaticDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CLayeredStaticDlg::DrawSkin(CDC* pDC)
{
	CDC memDC;
	CBitmap bmp;
	CBitmap* bmp_old;

	CRect rect;
	GetWindowRect(&rect);

	int cx = rect.Width();
	int cy = rect.Height();

	memDC.CreateCompatibleDC(pDC);

	BITMAPINFO bmInfo;
	memset(&bmInfo, 0x00, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = cx;
	bmInfo.bmiHeader.biHeight = cy;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biSizeImage = 0;
	bmInfo.bmiHeader.biClrUsed = 0;
	bmInfo.bmiHeader.biClrImportant = 0;

	LPBYTE pbmpBits = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(pDC->GetSafeHdc(), (LPBITMAPINFO)&bmInfo, DIB_RGB_COLORS, (void **)&pbmpBits, NULL, 0);
	if(hBitmap == NULL)
		bmp.CreateCompatibleBitmap(pDC, cx, cy);
	else
		bmp.Attach(hBitmap);

	bmp_old = memDC.SelectObject(&bmp);
	Graphics gps(memDC.GetSafeHdc());

	gps.DrawImage(
		m_pBackSkin->m_pBitmap, 
		Rect(0, 0, m_pBackSkin->m_pBitmap->GetWidth(), m_pBackSkin->m_pBitmap->GetHeight()), 
		0,
		0, 
		m_pBackSkin->m_pBitmap->GetWidth(), 
		m_pBackSkin->m_pBitmap->GetHeight(),
		UnitPixel);

	DisplayStaticText(gps);

	CPoint ptWindowScreenPosition(rect.TopLeft());
	CSize szWindow(cx, cy);

	BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	CPoint ptSrc(0,0);

	HDC hdc = gps.GetHDC();

	BOOL bRet= ::UpdateLayeredWindow(GetSafeHwnd(), 
		pDC->GetSafeHdc(),
		&ptWindowScreenPosition,
		&szWindow,
		hdc,
		&ptSrc,
		0, 
		&blendPixelFunction,
		ULW_ALPHA);

	gps.ReleaseHDC(hdc);

	memDC.SelectObject(bmp_old);
	bmp.DeleteObject();
	memDC.DeleteDC();
}

BOOL CLayeredStaticDlg::LoadSkin()
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

void CLayeredStaticDlg::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

void CLayeredStaticDlg::InitControl()
{
	CRect rClient;
	GetClientRect(&rClient);

	m_strTitle = "제목";
	m_strContents = "내용 텍스트 내용 텍스트\r\n내용 텍스트 내용 텍스트";
	m_strTitleAlign = "제목 정렬";
	m_strImgPath = "C:\\1.png";

	SetTimer(1, 2000, NULL);
}

void CLayeredStaticDlg::SetTextLabel(Graphics& gps) 
{
	USES_CONVERSION;

	Gdiplus::SolidBrush  brushText(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::FontFamily  fontFamily(L"굴림");
	Gdiplus::Font        font(&fontFamily, 17, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::PointF      pointF(15.0f, 15.0f);
	gps.DrawString(A2W(m_strTitle), -1, &font, pointF, &brushText);
}

void CLayeredStaticDlg::SetTextContents(Graphics& gps) 
{	
	USES_CONVERSION;

	Gdiplus::SolidBrush  brushBackGround(Gdiplus::Color(255, 41, 128, 185));
	gps.FillRectangle(&brushBackGround, 20, 33, 227, 40);

	Gdiplus::FontFamily  fontFamily(L"굴림");

	Gdiplus::SolidBrush  brushText(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::Font        font(&fontFamily, 15, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::PointF      pointF(20.0f, 37.0f);
	gps.DrawString(A2W(m_strContents), -1, &font, pointF, &brushText);
}

void CLayeredStaticDlg::SetBackgroundImage(Graphics& gps) 
{
	USES_CONVERSION;

	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(A2W(m_strImgPath), false);
	gps.DrawImage(bitmap, 150, 80, 100, 50);
}

void CLayeredStaticDlg::SetTextAlignLabel(Graphics& gps, int nAlign) 
{
	USES_CONVERSION;

	Gdiplus::SolidBrush  brushText(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::FontFamily  fontFamily(L"굴림");
	Gdiplus::Font        font(&fontFamily, 17, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	
	Gdiplus::StringFormat format;

	if(nAlign == 1)
	{
		format.SetAlignment( Gdiplus::StringAlignmentNear );
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	}
	else if(nAlign == 2)
	{
		format.SetAlignment( Gdiplus::StringAlignmentFar );
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	}
	else 
	{
		format.SetAlignment( Gdiplus::StringAlignmentCenter );
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	}

	Gdiplus::SolidBrush  brushBackGround(Gdiplus::Color(255, 41, 128, 185));
	gps.FillRectangle(&brushBackGround, 19, 80, 130, 50);
	

	RectF rectF(19, 80, 130, 50);
	gps.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	gps.DrawString(A2W(m_strTitleAlign), -1, &font, rectF, &format, &brushText);	
}

void CLayeredStaticDlg::DisplayStaticText(Graphics& gps)
{
	CRect rect;
    GetClientRect(&rect);

	SetTextLabel(gps);

	SetTextContents(gps);

	SetBackgroundImage(gps);

	SetTextAlignLabel(gps);
}

void CLayeredStaticDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		KillTimer(1);

		m_strTitle = "제목1";
		m_strContents = "내용 텍스트 내용 텍스트1\r\n내용 텍스트 내용 텍스트1";
		m_strTitleAlign = "제목 정렬1";
		m_strImgPath = "C:\\2.png";
		Invalidate();
	}

	CDialog::OnTimer(nIDEvent);
}
