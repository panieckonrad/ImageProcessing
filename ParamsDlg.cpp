// ParamsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "ParamsDlg.h"
#include "afxdialogex.h"


// CParamsDlg dialog

IMPLEMENT_DYNAMIC(CParamsDlg, CDialogEx)

CParamsDlg::CParamsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CParamsDlg::IDD, pParent)
{

}

CParamsDlg::~CParamsDlg()
{
}

void CParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit1);
}


BEGIN_MESSAGE_MAP(CParamsDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CParamsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CParamsDlg message handlers


void CParamsDlg::OnBnClickedOk()
{
	m_edit1.GetWindowTextW(m_sParams);
	CDialogEx::OnOK();
}
