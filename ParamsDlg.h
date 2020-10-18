#pragma once
#include "afxwin.h"


// CParamsDlg dialog

class CParamsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParamsDlg)

public:
	CParamsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParamsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PARAMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CEdit m_edit1;

	DECLARE_MESSAGE_MAP()
public:
	CString m_sParams;
	afx_msg void OnBnClickedOk();
};
