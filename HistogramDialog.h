#pragma once


// HistogramDialog dialog

class HistogramDialog : public CDialogEx
{
	DECLARE_DYNAMIC(HistogramDialog)

public:
	HistogramDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~HistogramDialog();
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
