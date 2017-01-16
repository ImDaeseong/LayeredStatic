#include "stdafx.h"
#include "LayeredDialog.h"
#include "SpriteToolDlg.h"

CSpriteToolDlg::CSpriteToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpriteToolDlg::IDD, pParent)
{
	m_pBackSkin	= NULL;	

	m_strTitle = "";
	m_strImgPath = "";

	mSpritebitmap = NULL;
	mTmpSpritebitmap = NULL;

	m_bMouseDown = FALSE;
	mDrawPen = new Pen(Color::White);
	mDrawBrush = new SolidBrush(Color::White);	
	mMouseMoveRect = Rect(0, 0, 0, 0);
	mMouseSelectRect = Rect(0, 0, 0, 0);
}

void CSpriteToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_LOAD, m_btnLoadPng);
}

BEGIN_MESSAGE_MAP(CSpriteToolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CSpriteToolDlg::OnBnClickedButtonLoad)
END_MESSAGE_MAP()

BOOL CSpriteToolDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) 
		{
			CDialog::OnOK();
			return TRUE;              
		}

		if( pMsg->wParam == VK_SPACE)
		{
			if (OpenClipboard())
			{
				EmptyClipboard();

				HGLOBAL hClipboardData;
				hClipboardData = GlobalAlloc(GMEM_DDESHARE, m_strTitle.GetLength()+1);
				char * pchData;
				pchData = (char*)GlobalLock(hClipboardData);
				  
				strcpy(pchData, LPCSTR(m_strTitle));		  

				GlobalUnlock(hClipboardData);
			
				SetClipboardData(CF_TEXT,hClipboardData);

				CloseClipboard();

				return TRUE;     
			}
		}

		//UINT nID = ::GetDlgCtrlID(pMsg->hwnd);  
		//if(nID == IDC_BUTTON_LOAD) return TRUE;     
	}		
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CSpriteToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW, 0);

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

void CSpriteToolDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CSpriteToolDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(mSpritebitmap != NULL)
		delete mSpritebitmap;
	mSpritebitmap = NULL;	

	if(mTmpSpritebitmap != NULL)
		delete mTmpSpritebitmap;
	mTmpSpritebitmap = NULL;	
	
	
	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

Bitmap* CSpriteToolDlg::ResizeBitmap(Bitmap* pInImg, int nResizeWidth, int nResizeHeight)
{
	int nImgWidth = pInImg->GetWidth();
	int nIngHeight = pInImg->GetHeight();
	float fHor = (float)nResizeWidth / nImgWidth;
	float fVor = (float)nResizeHeight / nIngHeight;
	
	nResizeWidth = (int)(nImgWidth * fHor);
	nResizeHeight = (int)(nIngHeight * fVor);
	Bitmap * pCopyImg = new Bitmap(nResizeWidth, nResizeHeight, PixelFormat32bppARGB);
	Graphics graphic(pCopyImg);
	graphic.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphic.DrawImage(pInImg, Gdiplus::Rect(0, 0, nResizeWidth, nResizeHeight), 0, 0, nImgWidth, nIngHeight, Gdiplus::UnitPixel);
	graphic.Flush();
	return pCopyImg;
}

CString CSpriteToolDlg::GetModulePath()
{
	char cTemp[MAX_PATH];
	char *spzRealDirEnd;
	CString strModulePath;

	GetModuleFileName(NULL, cTemp, MAX_PATH);  
	spzRealDirEnd = strrchr(cTemp, '\\');
	*spzRealDirEnd = '\0';
	strModulePath = (CString)cTemp;
	return strModulePath;
}

void CSpriteToolDlg::DrawSkin(CDC* pDC)
{ 	
	if (m_pBackSkin->m_pBitmap == NULL)	
		return;
	
	Graphics gps(pDC->GetSafeHdc());
	int x = m_pBackSkin->m_pBitmap->GetWidth();
	int y = m_pBackSkin->m_pBitmap->GetHeight();
	gps.DrawImage(m_pBackSkin->m_pBitmap, Rect(0, 0, x, y) , 0, 0, x, y, UnitPixel);

	SetBackgroundImage(gps); 

	gps.DrawRectangle(mDrawPen, mMouseSelectRect);
	gps.DrawRectangle(mDrawPen, mMouseMoveRect);

	SetTextLabel(gps);
}

BOOL CSpriteToolDlg::LoadSkin()
{
	m_pBackSkin = new CGdiPlusBitmapResource;
	if(!m_pBackSkin->LoadPath(""))
	{
		if(!m_pBackSkin->Load(MAKEINTRESOURCE(IDB_PNG_SPRITEBOX), _T("PNG"), AfxGetApp()->m_hInstance))
		{
			delete m_pBackSkin;
			m_pBackSkin = NULL;
			return FALSE;
		}
		
		int nWidth = 1024;
		int nHeight = 768;		
		if( (m_pBackSkin->m_pBitmap->GetWidth() != nWidth) || (m_pBackSkin->m_pBitmap->GetHeight() != nHeight) )
			m_pBackSkin->m_pBitmap = ResizeBitmap(m_pBackSkin->m_pBitmap, nWidth, nHeight);
	}
		
	MoveLocationDialog();
	
	return TRUE;
}

void CSpriteToolDlg::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow((rcWorkArea.left + 50), (rcWorkArea.top + 100), cx, cy);	
}

void CSpriteToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{		

		KillTimer(1);
	}

	CDialog::OnTimer(nIDEvent);
}

void CSpriteToolDlg::OnBnClickedButtonLoad()
{
	TCHAR szFile[MAX_PATH];
    ZeroMemory(szFile, MAX_PATH);
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize	= sizeof(OPENFILENAME);
    ofn.Flags		= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.hwndOwner	= this->m_hWnd;
    ofn.lpstrFilter	= _T("Png Files Types(*.png)\0*.Png;*.*\0\0");
    ofn.lpstrTitle	= _T("Load Png File");
    ofn.lpstrFile	= szFile;
    ofn.nMaxFile	= MAX_PATH;
	if (IDOK == GetOpenFileName(&ofn))
	{
		m_strImgPath = (LPCTSTR)szFile;
	}
	else
		m_strImgPath = "";

	LoadBackgroundImage(); 
}

void CSpriteToolDlg::InitControl()
{
	CRect rClient;
	GetClientRect(&rClient);
	m_btnLoadPng.SetImage(MAKEINTRESOURCE(IDB_PNG_ADD), "PNG", AfxGetApp()->m_hInstance, rClient.right - 520, rClient.bottom - 30);
    m_btnLoadPng.SetBkColor(RGB(180, 180, 180));

	SetTimer(1, 2000, NULL);
}

void CSpriteToolDlg::LoadBackgroundImage() 
{
	USES_CONVERSION;
	
	mSpritebitmap = Gdiplus::Bitmap::FromFile(A2W(m_strImgPath), false);
	mTmpSpritebitmap = new Bitmap(mSpritebitmap->GetWidth(), mSpritebitmap->GetHeight());
	::InvalidateRect(this->m_hWnd, NULL, TRUE);
}

void CSpriteToolDlg::SetBackgroundImage(Graphics& gps) 
{
	if(mSpritebitmap)
	{
		CRect rect;
		GetClientRect(&rect);

		Rect rcWnd(rect.left, rect.top, rect.right, rect.bottom);
		gps.DrawImage(mSpritebitmap, rcWnd, rcWnd.X, rcWnd.Y, rcWnd.Width, rcWnd.Height, UnitPixel);
	}
}

void CSpriteToolDlg::SetTextLabel(Graphics& gps) 
{	
	USES_CONVERSION;

	CRect rect;
	GetClientRect(&rect);
	
	Gdiplus::SolidBrush  brushText(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::FontFamily  fontFamily(L"굴림");
	Gdiplus::Font        font(&fontFamily, 17, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	
	Gdiplus::StringFormat format;
	format.SetAlignment( Gdiplus::StringAlignmentNear );
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	Gdiplus::SolidBrush  brushBackGround(Gdiplus::Color(255, 41, 128, 185));
	gps.FillRectangle(&brushBackGround, (rect.right - 500), (rect.bottom - 30), 500, 30);

	RectF rectF((rect.right - 500), (rect.bottom - 30), 500, 30);
	gps.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	gps.DrawString(A2W(m_strTitle), -1, &font, rectF, &format, &brushText);
}

void CSpriteToolDlg::OnLButtonDown(UINT nFlags, CPoint point)
{	
	Point EndPoint(point.x, point.y);

	m_bMouseDown = true;

	mMouseMoveRect = Rect(EndPoint.X, EndPoint.Y, 1, 1);	
	//::InvalidateRect(this->m_hWnd, NULL, TRUE);

	CDialog::OnLButtonDown(nFlags, point);
}

void CSpriteToolDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	Point EndPoint(point.x, point.y);
	
	mMouseMoveRect = Rect(min(EndPoint.X, mMouseMoveRect.X), min(EndPoint.Y, mMouseMoveRect.Y),	abs(EndPoint.X - mMouseMoveRect.X),	abs(EndPoint.Y - mMouseMoveRect.Y));
	
	m_bMouseDown = false;

	if(mSpritebitmap)
		mMouseSelectRect = FindRectRange(mMouseMoveRect);

	mMouseMoveRect = Rect(0,0,0,0);

	CString strMsg;
	//strMsg.Format("CRect(%d, %d, %d, %d)", mMouseSelectRect.X, mMouseSelectRect.Y, mMouseSelectRect.Width, mMouseSelectRect.Height);	
	strMsg.Format("CRect(%d, %d, %d, %d)  Width:[%d] height:[%d]", mMouseSelectRect.X, mMouseSelectRect.Y, mMouseSelectRect.GetRight(), mMouseSelectRect.GetBottom(), mMouseSelectRect.Width, mMouseSelectRect.Height);
	m_strTitle = strMsg;
	::InvalidateRect(this->m_hWnd, NULL, TRUE);
    
	SaveSplitImage();
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CSpriteToolDlg::OnMouseMove(UINT nFlags, CPoint point)
{	
	if (m_bMouseDown)
	{
		Point EndPoint(point.x, point.y);
		mMouseMoveRect = Rect(min(EndPoint.X, mMouseMoveRect.X), min(EndPoint.Y, mMouseMoveRect.Y),	abs(EndPoint.X - mMouseMoveRect.X),	abs(EndPoint.Y - mMouseMoveRect.Y));
		//::InvalidateRect(this->m_hWnd, NULL, TRUE);
	}
	CDialog::OnMouseMove(nFlags, point);
}

Rect CSpriteToolDlg::FindRectRange( Rect range )
{
	Rect rt;
	if ((range.Width == 1)  && (range.Height == 1))
	{
		rt = FindRect( Point(range.X, range.Y));
	}
	else
	{
		Color bgColor;
		Status status = mSpritebitmap->GetPixel(0, 0, &bgColor);

		Graphics graphic(mTmpSpritebitmap);
		graphic.FillRectangle(mDrawBrush, Rect(0,0,mTmpSpritebitmap->GetWidth(), mTmpSpritebitmap->GetHeight()));

		Rect tr = TrimRect( range );
		RECT rect = {tr.X, tr.Y, tr.X+tr.Width, tr.Y+tr.Height};
		RECT tmp = rect;

		for (int i=rect.left; i < rect.right; ++i)
		{
			for (int k=rect.top; k < rect.bottom; ++k)
			{
				Point pos(i, k);

				Color color, checkColor;
				mSpritebitmap->GetPixel(pos.X, pos.Y, &color);
				mTmpSpritebitmap->GetPixel(pos.X, pos.Y, &checkColor);

				if ((color.GetValue() != bgColor.GetValue()) &&	(checkColor.GetValue() == 0xFFFFFFFF) && (color.GetAlpha() != 0))
				{
					RECT r = FindRectRec(pos, bgColor, false);
					RECT newR;
					newR.left = min(r.left, tmp.left);
					newR.top = min(r.top, tmp.top);
					newR.right = max(r.right, tmp.right);
					newR.bottom = max(r.bottom, tmp.bottom);

					if ((tmp.left != newR.left) || (tmp.top != newR.top) ||	(tmp.right != newR.right) || (tmp.bottom != newR.bottom))
					{
						tmp = newR;
						// 처음 부터 다시 검색한다.
						rect = newR;
						i = newR.left-1;
						k = newR.top - 1;
					}
				}
			}
		}
		rt = Rect(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
	}
	return rt;
}

// 백그라운드 컬러 값을 제외한 최대 렉트크기를 리턴한다.
Rect CSpriteToolDlg::TrimRect( Rect range )
{
	RECT r = {0, 0, 0, 0};
	RECT checkRange = {range.X, range.Y, range.X+range.Width, range.Y+range.Height};

	Color bgColor;
	mSpritebitmap->GetPixel(0, 0, &bgColor);
	
	bool checkLeft = false;
	for (int i=checkRange.left; (!checkLeft) && (i < checkRange.right); ++i)
	{
		for (int k=checkRange.top; k < checkRange.bottom; ++k)
		{
			Point pos(i, k);

			Color color;
			mSpritebitmap->GetPixel(pos.X, pos.Y, &color);
			if ((color.GetValue() != bgColor.GetValue()) &&	(color.GetAlpha() != 0))
			{
				r.left = i;
				checkLeft = true;
				break;
			}
		}
	}

	bool checkTop = false;
	for (int i=checkRange.top; (!checkTop) && (i < checkRange.bottom); ++i)
	{
		for (int k=checkRange.left; k < checkRange.right; ++k)
		{
			Point pos(k, i);

			Color color;
			mSpritebitmap->GetPixel(pos.X, pos.Y, &color);
			if ((color.GetValue() != bgColor.GetValue()) &&	(color.GetAlpha() != 0))
			{
				r.top = i;
				checkTop = true;
				break;
			}
		}
	}

	bool checkRight = false;
	for (int i=checkRange.right; (!checkRight) && (i >= checkRange.left); --i)
	{
		for (int k=checkRange.top; k < checkRange.bottom; ++k)
		{
			Point pos(i, k);

			Color color;
			mSpritebitmap->GetPixel(pos.X, pos.Y, &color);
			if ((color.GetValue() != bgColor.GetValue()) &&	(color.GetAlpha() != 0))
			{
				r.right = i;
				checkRight = true;
				break;
			}
		}
	}

	bool checkBottom = false;
	for (int i=checkRange.bottom; (!checkBottom) && (i >= checkRange.top); --i)
	{
		for (int k=checkRange.left; k < checkRange.right; ++k)
		{
			Point pos(k, i);

			Color color;
			mSpritebitmap->GetPixel(pos.X, pos.Y, &color);
			if ((color.GetValue() != bgColor.GetValue()) &&	(color.GetAlpha() != 0))
			{
				r.bottom = i;
				checkBottom = true;
				break;
			}
		}
	}
	return Rect(r.left,r.top,r.right-r.left,r.bottom-r.top);
}

Rect CSpriteToolDlg::FindRect( Point mousePos )
{
	Graphics graphic(mTmpSpritebitmap);
	graphic.FillRectangle(mDrawBrush, Rect(0,0,mTmpSpritebitmap->GetWidth(), mTmpSpritebitmap->GetHeight()));

	Color color;
	mSpritebitmap->GetPixel(0, 0, &color);
	RECT r = FindRectRec(mousePos, color, true);

	return Rect(r.left, r.top, r.right-r.left, r.bottom-r.top);
}

RECT CSpriteToolDlg::FindRectRec( Point firstPos, Color bgColor, bool checkRange )
{
	vector<Point> q;
	q.reserve(10000);
	q.push_back(firstPos);

	RECT rect = {firstPos.X,firstPos.Y,firstPos.X,firstPos.Y};

	while (!q.empty())
	{
		Point pos = q.back();
		q.pop_back();

		if ((pos.X < 0) || (pos.Y < 0))
			continue;
		if ((pos.X >= (int)mSpritebitmap->GetWidth()) || (pos.Y >= (int)mSpritebitmap->GetHeight()))
			continue;

		Color checkColor;
		mTmpSpritebitmap->GetPixel(pos.X, pos.Y, &checkColor);
		if (checkColor.GetValue() != 0xFFFFFFFF)
			continue;

		mTmpSpritebitmap->SetPixel(pos.X, pos.Y, Color::Black);

		Color color;
		mSpritebitmap->GetPixel(pos.X, pos.Y, &color);
		if ((color.GetValue() == bgColor.GetValue()) ||	(color.GetAlpha() == 0))
			continue;

		q.push_back(Point(pos.X-1, pos.Y));
		q.push_back(Point(pos.X, pos.Y-1));
		q.push_back(Point(pos.X+1, pos.Y));
		q.push_back(Point(pos.X, pos.Y+1));

		rect.left = min(pos.X, rect.left);
		rect.top = min(pos.Y, rect.top);
		rect.right = max(pos.X, rect.right);
		rect.bottom = max(pos.Y, rect.bottom);
	}

	RECT tmp = rect;
	return tmp;
}

Bitmap* CSpriteToolDlg::CropImage(Bitmap* pSource, CRect cropRect)
{
	Bitmap* cropBitMap = new Bitmap(cropRect.Width(), cropRect.Height());
	if(!cropBitMap) return NULL;

	Graphics* pGraphics = Gdiplus::Graphics::FromImage(cropBitMap);

	pGraphics->DrawImage(pSource, Rect(0, 0, cropBitMap->GetWidth(), cropBitMap->GetHeight()), cropRect.left, cropRect.top, cropRect.Width(), cropRect.Height(), UnitPixel);

	return cropBitMap;
}

int CSpriteToolDlg::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
	UINT num = 0;
	UINT size = 0;
	ImageCodecInfo * pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo *)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return -1;
}

CString CSpriteToolDlg::GetRandomString() 
{
	srand ((unsigned)time(NULL));

	int nHigh = 10;
	int nLow = 0;
	CString strRandom;
	strRandom.Format("%d", rand() % (nHigh - nLow) + nLow);
	return strRandom;
}

void CSpriteToolDlg::SaveSplitImage()
{
	CRect rc = CRect(mMouseSelectRect.X, mMouseSelectRect.Y, mMouseSelectRect.GetRight(), mMouseSelectRect.GetBottom());
	Gdiplus::Bitmap* pBitmap = CropImage(mSpritebitmap, rc);
	
	CLSID				encoderClsid;
	EncoderParameters	encoderParameters;
	ULONG				quality;

	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;

	// 이미지 png 저장시 Quality level 100.
	quality = 100;
	encoderParameters.Parameter[0].Value = &quality;
		
	CString strRealPath;
	strRealPath.Format("%s\\Image_%s.png",GetModulePath(), GetRandomString() );

	int nSizeCount = MultiByteToWideChar(CP_ACP, 0, strRealPath, -1, NULL, 0);
	WCHAR * pWString = new WCHAR[nSizeCount];
	MultiByteToWideChar(CP_ACP, 0, strRealPath, -1, pWString, nSizeCount);

	GetEncoderClsid(L"image/png", &encoderClsid);
	pBitmap->Save(pWString, &encoderClsid, &encoderParameters);
}