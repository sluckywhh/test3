#include "stdafx.h"
#include "ImageButtonEx.h"

CImageButtonEx::CImageButtonEx()
{
	m_bIsTracking = FALSE;
	m_bIsHovering = FALSE;
	m_bHaveBitmaps = FALSE;

	m_pCurBtnDC = NULL;
	m_pStdImage = NULL;
	m_pStdPImage = NULL;
	m_pStdDImage = NULL;

	m_strBtnText = _T("");
	m_rcText.SetRectEmpty();
	m_crTextColor = GetSysColor(COLOR_WINDOW);
}

CImageButtonEx::~CImageButtonEx()
{
	m_Font.DeleteObject();

	if (m_pStdImage)  delete m_pStdImage;
	if (m_pStdPImage) delete m_pStdPImage;
	if (m_pStdDImage) delete m_pStdDImage;
}

BEGIN_MESSAGE_MAP(CImageButtonEx, CButton)
	ON_WM_DRAWITEM()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()

	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//
void CImageButtonEx::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW, SWP_FRAMECHANGED);

	CButton::PreSubclassWindow();
}

BOOL CImageButtonEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
	{
		pMsg->message = WM_LBUTTONDOWN;
	}
	
	return CButton::PreTranslateMessage(pMsg);
}

HBRUSH CImageButtonEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!m_bHaveBitmaps)
	{
		GetParentWndBk();
		SetImageWndBk(pDC);

		m_bHaveBitmaps = TRUE;
	}
	
	return (HBRUSH)::GetStockObject(GRAY_BRUSH);
}

void CImageButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	CDC *pDC = CDC::FromHandle(lpDIS->hDC);
	if (pDC == NULL)
	{
		return;
	}
	
	CDC* pCurBtnDC = NULL;
	if (lpDIS->itemState & ODS_DISABLED)
	{//����
		pCurBtnDC = &m_dcStandD;
	}
	else
	{
		if (lpDIS->itemState & ODS_SELECTED)
		{//ѡ��
			pCurBtnDC = &m_dcStandP;
		}
		else
		{
			pCurBtnDC = &m_dcStand;
		}
	}

	DrawImageWndBk(pDC, pCurBtnDC);
}

BOOL CImageButtonEx::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

LRESULT CImageButtonEx::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	m_bIsTracking = FALSE;
	m_bIsHovering = TRUE;
	
	//Invalidate();
	return 0;
}

LRESULT CImageButtonEx::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bIsTracking = FALSE;
	m_bIsHovering = FALSE;
	
	//Invalidate();
	return 0;
}

void CImageButtonEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bIsTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = this->GetSafeHwnd();
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 1;

		m_bIsTracking = ::_TrackMouseEvent(&tme);
	}
	
	CButton::OnMouseMove(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////
//
void CImageButtonEx::GetParentWndBk()
{
	if (m_dcWndBkDC.m_hDC == NULL)
	{
		CClientDC clDC(GetParent());
		SetBkGnd(&clDC);

#if 0
		Bitmap*	pWndBkImage = ::new Bitmap((HBITMAP)::GetCurrentObject(m_dcWndBkDC, OBJ_BITMAP),NULL);
		if (pWndBkImage != NULL)
		{
			CLSID pngClsid;
			GetEncoderClsid(L"image/bmp", &pngClsid);

			pWndBkImage->Save(L"C:\\11.bmp", &pngClsid, NULL);
		}
#endif
	}
}

void CImageButtonEx::SetImageWndBk(CDC *pDC)
{
	if (pDC == NULL)
	{
		return;
	}
	
	CRect rect;
	GetClientRect(&rect);
	
	CMemDC MemDC(pDC, rect);

	//����״̬
	if (m_dcStand.m_hDC == NULL)
	{
		DrawStdWndBk(&MemDC, &m_dcWndBkDC);
	}

	//����״̬
	if (m_dcStandP.m_hDC == NULL)
	{
		DrawStdPWndBk(&MemDC, &m_dcWndBkDC);
	}

	//����״̬
	if (m_dcStandD.m_hDC == NULL)
	{
		DrawStdDWndBk(&MemDC, &m_dcWndBkDC);
	}

	if (m_pCurBtnDC == NULL)
	{
		m_pCurBtnDC = &m_dcStand;
	}
}

void CImageButtonEx::DrawImageWndBk(CDC *pDC, CDC *pCurBtnDC)
{
	CRect rcText;
	CStringW strText;
	StringFormat stringformat1;

	USES_CONVERSION;
	if (pDC == NULL || pCurBtnDC == NULL)
	{
		return;
	}

	CRect rect;
	GetClientRect(rect);

	CMemDC MemDC(pDC, rect);
	Graphics graphics(MemDC.GetSafeHdc());
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias); 

	MemDC->BitBlt(0, 0, rect.Width(), rect.Height(), pCurBtnDC, 0, 0, SRCCOPY);

	if (m_strBtnText != _T(""))
	{
		if (m_rcText != CRect(0, 0, 0, 0))
		{
			rcText = m_rcText;
		}
		else
		{
			rcText = rect;
		}

		strText = A2CW(m_strBtnText);
		stringformat1.SetAlignment(StringAlignmentCenter);
		stringformat1.SetLineAlignment(StringAlignmentCenter);

		//FontFamily fontfamily(L"΢���ź�");
		//Font font1(&fontfamily, 26, FontStyleRegular, UnitPixel);
		Gdiplus::Font font1(MemDC->GetSafeHdc(), &m_lfFont);

		graphics.DrawString(strText.GetBuffer(0), strText.GetLength(), &font1, RectF(rcText.left, rcText.top, rcText.Width(), rcText.Height()), &stringformat1, &SolidBrush(Color::White));
	}

	m_pCurBtnDC = pCurBtnDC;
}

BOOL CImageButtonEx::LoadStdImage(LPCTSTR lpszImage)
{
	if (lpszImage == NULL)
	{
		return FALSE;
	}
	
	m_pStdImage = new CGdiPlusBitmap;
	if ( m_pStdImage != NULL )
	{
		USES_CONVERSION;
		m_pStdImage->Load(A2CW(lpszImage));
	}
	
	return TRUE;
}

BOOL CImageButtonEx::LoadStdPImage(LPCTSTR lpszImage)
{
	if (lpszImage == NULL)
	{
		return FALSE;
	}
	
	m_pStdPImage = new CGdiPlusBitmap;
	if ( m_pStdPImage != NULL )
	{
		USES_CONVERSION;
		m_pStdPImage->Load(A2CW(lpszImage));
	}
	
	return TRUE;
}

BOOL CImageButtonEx::LoadStdDImage(LPCTSTR lpszImage)
{
	if (lpszImage == NULL)
	{
		return FALSE;
	}
	
	m_pStdDImage = new CGdiPlusBitmap;
	if ( m_pStdDImage != NULL )
	{
		USES_CONVERSION;
		m_pStdDImage->Load(A2CW(lpszImage));
	}
	
	return TRUE;
}

void CImageButtonEx::DrawStdWndBk(CDC *pDC, CDC *pWndBkDC)
{
	CRect rect;
	GetClientRect(&rect);
	
	//�������ڱ���
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), pWndBkDC, 0, 0, SRCCOPY);
	
	//��ͼƬ��Ϣ
	Graphics graphics(pDC->GetSafeHdc());
	graphics.DrawImage(*m_pStdImage, 0, 0, m_pStdImage->m_pBitmap->GetWidth(), m_pStdImage->m_pBitmap->GetHeight());
	
	m_dcStand.CreateCompatibleDC(pDC);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap *pOldBitmap = m_dcStand.SelectObject(&bitmap);
	
	m_dcStand.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
	bitmap.DeleteObject();
}

void CImageButtonEx::DrawStdPWndBk(CDC *pDC, CDC *pWndBkDC)
{
	CRect rect;
	GetClientRect(&rect);
	
	//�������ڱ���
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), pWndBkDC, 0, 0, SRCCOPY);
	
	//��ͼƬ��Ϣ
	Graphics graphics(pDC->GetSafeHdc());
	if (m_pStdPImage != NULL)
	{
		graphics.DrawImage(*m_pStdPImage, 0, 0, m_pStdPImage->m_pBitmap->GetWidth(), m_pStdPImage->m_pBitmap->GetHeight());
	}
	else
	{
		graphics.DrawImage(*m_pStdImage, 1, 1, m_pStdImage->m_pBitmap->GetWidth(), m_pStdImage->m_pBitmap->GetHeight());
	}
	
	m_dcStandP.CreateCompatibleDC(pDC);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap *pOldBitmap = m_dcStandP.SelectObject(&bitmap);
	
	m_dcStandP.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
	bitmap.DeleteObject();
}

void CImageButtonEx::DrawStdDWndBk(CDC *pDC, CDC *pWndBkDC)
{
	CRect rect;
	GetClientRect(&rect);
	
	//�������ڱ���
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), pWndBkDC, 0, 0, SRCCOPY);
	
	//��ͼƬ��Ϣ
	Graphics graphics(pDC->GetSafeHdc());
	if (m_pStdDImage != NULL)
	{
		graphics.DrawImage(*m_pStdDImage, 0, 0, m_pStdDImage->m_pBitmap->GetWidth(), m_pStdDImage->m_pBitmap->GetHeight());
	}
	else
	{
		ColorMatrix GrayMat = {	0.30f, 0.30f, 0.30f, 0.00f, 0.00f,
								0.59f, 0.59f, 0.59f, 0.00f, 0.00f,
								0.11f, 0.11f, 0.11f, 0.00f, 0.00f,
								0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
								0.00f, 0.00f, 0.00f, 0.00f, 1.00f	};
								
		ImageAttributes ia;
		ia.SetColorMatrix(&GrayMat);

		float width = (float)m_pStdImage->m_pBitmap->GetWidth();
		float height = (float)m_pStdImage->m_pBitmap->GetHeight();

		RectF grect; 
		grect.X=0, grect.Y=0; 
		grect.Width = width; 
		grect.Height = height;
		
		graphics.DrawImage(*m_pStdImage, grect, 0, 0, width, height, UnitPixel, &ia);
	}
	
	m_dcStandD.CreateCompatibleDC(pDC);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap *pOldBitmap = m_dcStandD.SelectObject(&bitmap);
	
	m_dcStandD.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
	bitmap.DeleteObject();
}

int CImageButtonEx::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

/////////////////////////////////////////////////////////////////////////////
//
void CImageButtonEx::SetBkGnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);
	
	CRect rcParentRect;
	GetWindowRect(rcParentRect);
	GetParent()->ScreenToClient(rcParentRect);
	
	m_dcWndBkDC.DeleteDC();
	m_dcWndBkDC.CreateCompatibleDC(pDC);
	
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap *pOldBitmap = m_dcWndBkDC.SelectObject(&bitmap);
	
	m_dcWndBkDC.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, rcParentRect.left, rcParentRect.top, SRCCOPY);
	bitmap.DeleteObject();
}

void CImageButtonEx::SetImageEx(LPCTSTR lpszStdImage, LPCTSTR lpszStdPImage, LPCTSTR lpszStdDImage)
{
	if (!LoadStdImage(lpszStdImage))
	{
		return;
	}
	
	LoadStdPImage(lpszStdPImage);
	LoadStdDImage(lpszStdDImage);
	
	int iWidth = m_pStdImage->m_pBitmap->GetWidth();
	int iHeight = m_pStdImage->m_pBitmap->GetHeight();
	MoveWindow(0, 0, iWidth, iHeight);
}

void CImageButtonEx::SetTextEx(UINT uiFontSize, UINT uiWeight, LPCTSTR lpszBtnText, LPCTSTR lpszFaceName, COLORREF crTextColor, CRect rcText)
{
	if (lpszBtnText == NULL)
	{
		return;
	}

	memset(&m_lfFont, 0x0, sizeof(LOGFONT));
	m_lfFont.lfHeight = uiFontSize;
	m_lfFont.lfWeight = uiWeight;
	m_lfFont.lfCharSet=GB2312_CHARSET;
	_tcscpy_s(m_lfFont.lfFaceName, lpszFaceName);

	m_strBtnText = lpszBtnText;
	m_crTextColor = crTextColor;
	m_rcText.CopyRect(rcText);
	
	Invalidate(TRUE);
}