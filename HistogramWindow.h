#pragma once


// HistogramWindow dialog

class HistogramWindow : public CDialogEx
{
	DECLARE_DYNAMIC(HistogramWindow)

public:
	HistogramWindow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~HistogramWindow();
	int threshold = -2;
	float* values;


// Dialog Data 
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HISTOGRAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
};
