#pragma once
#include "myImage.h"

// CImgWnd
class CImgWnd : public CWnd
{
	DECLARE_DYNAMIC(CImgWnd)

public:
	CImgWnd();
	virtual ~CImgWnd();

	BOOL Create(const RECT& rect, CWnd*  pParentWnd, UINT nID);
	myImage Image;
	CDC dcMemory;
	bool doPaint = false;

	int rx1 = 0;
	int ry1 = 0;
	int rx2 = 0;
	int ry2 = 0;
	int lx1 = 0;
	int ly1 = 0;
	int lx2 = 0;
	int ly2 = 0;
	bool firstClickR = true;
	bool firstClickL = true;

	//lab9
	void *parent;
	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);

	void FourierClickL(UINT nFlags, CPoint point);
	void MomentyClickL(UINT nFlags, CPoint point);
};


