
// PO1Dlg.h : header file
//

#pragma once
#include "ImgWnd.h"
#include "afxwin.h"



// CPODlg dialog
class CPODlg : public CDialogEx
{
// Construction
public:
	CPODlg(CWnd* pParent = NULL);	// standard constructor
	CString filePath;
// Dialog Data
	enum { IDD = IDD_PO1_DIALOG };
	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CImgWnd m_imgIN;
	CImgWnd m_imgOUT;



	int parameter = 1; // parameter equals to 1 by default
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonProcess();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonParams();
	CSliderCtrl slider; // kontrolka slidero
	CComboBox m_combo1;// combobox
	afx_msg void OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchangeCombo1();
};
