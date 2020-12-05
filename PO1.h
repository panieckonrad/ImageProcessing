
// PO1.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <gdiplus.h> 
using namespace Gdiplus;

// CPOApp:
// See PO1.cpp for the implementation of this class
//

class CPOApp : public CWinApp
{

public:
	CPOApp();
	GdiplusStartupInput gdiplusParametry;
	ULONG_PTR gdiplusToken;

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPOApp theApp;