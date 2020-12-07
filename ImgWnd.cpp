// ImgWnd.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "ImgWnd.h"


// CImgWnd

IMPLEMENT_DYNAMIC(CImgWnd, CWnd)

CImgWnd::CImgWnd()
{

}

CImgWnd::~CImgWnd()
{
}


BEGIN_MESSAGE_MAP(CImgWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()



// CImgWnd message handlers

BOOL CImgWnd::Create(const RECT& rect, CWnd*  pParentWnd, UINT nID)
{
	BOOL bRes;

	bRes = CWnd::Create(NULL, NULL,
		WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, pParentWnd, nID, NULL);

	ShowScrollBar(SB_BOTH, FALSE);

	CDC* pDC = GetDC();
	dcMemory.CreateCompatibleDC(pDC); //dcMemory to pole typu CDC 
	CBitmap memBM;
	memBM.CreateCompatibleBitmap(pDC, rect.right - rect.left, rect.bottom - rect.top);  // szerokosc i wysokosc okna imgWnd to 400
	dcMemory.SelectObject(memBM);

	ReleaseDC(pDC);

	return bRes;
}


void CImgWnd::OnPaint()
{
	if (!doPaint) { 
		return;
	}

	CPaintDC dc(this); // device context for painting
	//
	//// Do not call CWnd::OnPaint() for painting messages
	HDC kontekst = dc.GetSafeHdc();
	CRect r;
	GetClientRect(r);
	Image.PaintDIB(kontekst, r);



}

void CImgWnd::OnRButtonDown(UINT nFlags, CPoint point){

	if (firstClickR) {
		rx1 = point.x;
		ry1 = point.y;
		firstClickR = false;
	}
	else {
		rx2 = point.x;
		ry2 = point.y;
		int width = rx2 - rx1;
		int height = ry2 - ry1;
		int topX = rx1;
		int topY = ry1;
		if (width < 0) {
			topX = rx1 + width;
		}
		if (height < 0) {
			topY = ry1 + height;
		}
		CClientDC dc(this);
		Color color;
		if ((nFlags & MK_SHIFT) == MK_SHIFT) { // if shift pressed
			color = Color(70, 255, 0, 0); //red

			for (int y = topY; y < abs(height); y++) {
				for (int x = topX; x < abs(width); x++) {
					dcMemory.SetPixel(x, y, 0);
				}
			}
		}
		else {
			color = Color(70, 0, 255, 0); // if shift not pressed green

			for (int y = topY; y < abs(height); y++) {
				for (int x = topX; x < abs(width); x++) {
					dcMemory.SetPixel(x, y, 1);
				}
			}
		}
		Gdiplus::Graphics g(dc.GetSafeHdc());
		
		Gdiplus::Rect rectangle(topX,topY,abs(width), abs(height));
		SolidBrush solidBrush(color);
		g.FillRectangle(&solidBrush, rectangle);
		firstClickR = true;


	}
}


void CImgWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (firstClickL) {
		lx1 = point.x;
		ly1 = point.y;
		firstClickL = false;
	}
	else {
		lx2 = point.x;
		ly2 = point.y;
		int width = lx2 - lx1;
		int height = ly2 - ly1;
		int topX = lx1;
		int topY = ly1;
		if (width < 0) {
			topX = lx1 + width;
		}
		if (height < 0) {
			topY = ly1 + height;
		}
		CClientDC dc(this);
		Color color;
		if ((nFlags & MK_SHIFT) == MK_SHIFT) { // if shift pressed
			color = Color(70, 255, 0, 0); //red

			for (int y = topY; y < abs(height); y++) {
				for (int x = topX; x < abs(width); x++) {
					dcMemory.SetPixel(x, y, 0);
				}
			}
		}
		else { // if shift not pressed
			color = Color(70, 0, 255, 0);// green

			for (int y = topY; y < abs(height); y++) {
				for (int x = topX; x < abs(width); x++) {
					dcMemory.SetPixel(x, y, 1);
				}
			}
		}
		Gdiplus::Graphics g(dc.GetSafeHdc());
		
		Gdiplus::Rect rectangle(topX, topY, abs(width), abs(height));
		SolidBrush solidBrush(color);
		g.FillEllipse(&solidBrush, rectangle);
		firstClickL = true;


	}
}


void CImgWnd::OnLButtonDblClk(UINT nFlags, CPoint point) // wysczysc wszystkie obszary i ustaw ze bierzemy wszystko
{
	CClientDC dc(this);
	CRect r;
	GetClientRect(r);
	InvalidateRect(NULL);

	for (int x = 0; x < r.Width(); x++) {
		for (int y = 0; y < r.Height();y++) {
			dcMemory.SetPixel(x, y, 1); // wyczysc wszystkie obszary, wiec wszystko bierzemy
		}
	}
	firstClickL = true;
}


void CImgWnd::OnRButtonDblClk(UINT nFlags, CPoint point) // wyklucz wszystkie obszary
{
	CClientDC dc(this);
	Gdiplus::Graphics g(dc.GetSafeHdc());
	CRect r;
	GetClientRect(r);
	Gdiplus::Rect rect(r.left, r.top, r.Width(), r.Height());
	Color color(70, 255, 0, 0);
	SolidBrush solidBrush(color);
	g.FillRectangle(&solidBrush, rect);


	for (int x = 0; x < r.Width(); x++) {
		for (int y = 0; y < r.Height();y++) {
			dcMemory.SetPixel(x, y, 0); // wyzeruj cala maske
		}
	}
	firstClickR = true;
}
