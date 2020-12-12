// HistogramWindow.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "HistogramWindow.h"
#include "afxdialogex.h"


// HistogramWindow dialog

IMPLEMENT_DYNAMIC(HistogramWindow, CDialogEx)

HistogramWindow::HistogramWindow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HISTOGRAM, pParent)
{

}

HistogramWindow::~HistogramWindow()
{
}

void HistogramWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(HistogramWindow, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// HistogramWindow message handlers


void HistogramWindow::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialogEx::OnPaint() for painting messages
	CRect r;
	GetClientRect(r);
	float max = 0;
	for (int i = 0; i < 256; i++) {
		if (values[i] > max)
			max = values[i];
	}

	for (int i = 0; i < 256; i++)
	{
		dc.MoveTo(10 + 5 * i, r.Height() - 10);
		float newValue = (values[i] / max ) * 500;
		dc.LineTo(10 + 5 * i, r.Height() - 10 - newValue);
	}

	// rysowanie progu
	if (threshold != -2) {
		dc.MoveTo(10 + 5 * threshold, r.Height() - 10);
		int nsave = dc.SaveDC();
		CPen penRed(PS_SOLID, 1, RGB(255, 0, 0));
		dc.SelectObject(&penRed);
		//RYSOWANIE
		dc.LineTo(10 + 5 * threshold, r.Height() - 10 - 700);
		////////
		dc.RestoreDC(nsave);
	}
}


BOOL HistogramWindow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here 
	int szerokosc = 40 + 256 * 5;
	int wysokosc = 560;
	SetWindowPos(NULL, 0, 0, szerokosc, wysokosc, SWP_NOMOVE | SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
