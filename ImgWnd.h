#pragma once
#include "Image.h"

// CImgWnd

class CImgWnd : public CWnd
{
	DECLARE_DYNAMIC(CImgWnd)

public:
	CImgWnd();
	virtual ~CImgWnd();

	BOOL Create(const RECT& rect, CWnd*  pParentWnd, UINT nID);
	Image Image;
	bool doPaint = false;
protected:
	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


