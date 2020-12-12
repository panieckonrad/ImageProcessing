// ImgWnd.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "ImgWnd.h"
#include "PO1Dlg.h"


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
	ON_CBN_SELCHANGE(IDC_COMBO1, &CPODlg::OnCbnSelchangeCombo1)
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
	memBM.CreateCompatibleBitmap(pDC, rect.right - rect.left, rect.bottom - rect.top);  // szerokosc i wysokosc okna imgWnd to 600 x 650
	dcMemory.SelectObject(memBM);
	for (int i = 0; i < (rect.right - rect.left); i++) {
		for (int j = 0; j < (rect.bottom - rect.top); j++) {
			dcMemory.SetPixel(i, j, 1);
		}
	}

	//lab9
	parent = (CPODlg*)pParentWnd;

	ReleaseDC(pDC);

	return bRes;
}


void CImgWnd::OnPaint()
{


	CPaintDC dc(this); // device context for painting
	//
	//// Do not call CWnd::OnPaint() for painting messages
	if (!doPaint) {
		return;
	}
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

		Gdiplus::Rect rectangle(topX, topY, abs(width), abs(height));
		SolidBrush solidBrush(color);
		g.FillRectangle(&solidBrush, rectangle);
		firstClickR = true;


	}
}

void CImgWnd::FourierClickL(UINT nFlags, CPoint point) {
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

void CImgWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	//FourierClickL(nFlags, point); // lab8, gryzie sie z lab9
	MomentyClickL(nFlags, point);
}

void CImgWnd::MomentyClickL(UINT nFlags, CPoint point) {
	if (doPaint) { // jesli juz wczytalismy obraz
		CRect r;
		GetClientRect(&r);
		int x = 0, y = 0; // pozycja x i y w oknie
		int xp = 0, yp = 0; // pozycja x i y w prawidlowym obrazie
		int szerokosc;
		int wysokosc;
		int fileWidth = Image.fileWidth;
		int fileHeight = Image.fileHeight;

		//skalowanie z paintdib(), potrzebny nam offset
		if (fileWidth > fileHeight) { // jesli szerokosc wieksza niz wysokosc to skalujemy wysokosc
			szerokosc = r.Width();
			wysokosc = (float)fileHeight / (float)fileWidth * (float)(r.Height());
			y = (r.Height() - wysokosc) / 2;
		}
		else { // w przeciwnym wypadku szerokosc
			wysokosc = r.Height();
			szerokosc = (float)fileWidth / (float)fileHeight * (float)(r.Width());
			x = (r.Width() - szerokosc) / 2;
		}

		xp = point.x - x; // przesuwamy aby trafic w prawdziwy obraz
		yp = point.y - y;
		xp = xp * fileWidth / szerokosc; // dostosowujemy indeks z imgWnd do prawidlowego indeksu w obrazie
		yp = yp * fileHeight / wysokosc;

		std::vector<double> momenty;
		
		if ((xp >= 0 && xp < fileWidth) && (yp >= 0 && yp <= fileHeight)) { // na wszelki wypadek
			momenty = Image.Momenty(xp,yp);
		}
		if (!momenty.empty()){
			CString strBuffer;
			strBuffer.Format(_T("u00 = %f \n u10 = %f \n u01 = %f \n u20 = %f \n u02 = %f \n u11 = %f \n deg = %f"), 
				momenty[0], momenty[1], momenty[2], momenty[3], momenty[4], momenty[5], momenty[6]);
			MessageBox(strBuffer);
		}
		InvalidateRect(NULL);
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
