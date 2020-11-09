
// PO1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "PO1Dlg.h"
#include "ParamsDlg.h"
#include "afxdialogex.h"
#include "HistogramWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IMG_WND_ID_IN	100
#define IMG_WND_ID_OUT	101


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPODlg dialog



CPODlg::CPODlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPODlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo1);
	DDX_Control(pDX, IDC_SLIDER1, slider);
}

BEGIN_MESSAGE_MAP(CPODlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CPODlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CPODlg::OnBnClickedButtonProcess)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CPODlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_PARAMS, &CPODlg::OnBnClickedButtonParams)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CPODlg::OnNMCustomdrawSlider1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CPODlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CPODlg message handlers

BOOL CPODlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect rDlg(7,7,407,407);
	MapDialogRect(rDlg);

	m_imgIN.Create(rDlg, this, IMG_WND_ID_IN);
		
	rDlg = CRect(530, 7, 930, 407);
	MapDialogRect(rDlg);

	m_imgOUT.Create(rDlg, this, IMG_WND_ID_OUT);
	
	m_combo1.AddString(L"lab1_convert to greyscale");

	m_combo1.AddString(L"lab2_change brightness");
	m_combo1.AddString(L"lab2_change kontrast");
	m_combo1.AddString(L"lab2_change potega");
	m_combo1.AddString(L"lab2_negatyw");
	m_combo1.AddString(L"lab2_wyswietlenie histogramu");
	m_combo1.AddString(L"lab2_wyrownanie histogramu");

	m_combo1.AddString(L"lab3_reczne progowanie");
	m_combo1.AddString(L"lab3_iteracyjne progowanie");
	m_combo1.AddString(L"lab3_gradient progowanie");
	m_combo1.AddString(L"lab3_otsu progowanie");

	m_combo1.AddString(L"lab4_filtr srednia 3x3");
	m_combo1.AddString(L"lab4_filtr Gaussa 3x3");
	m_combo1.AddString(L"lab4_filtr SobelaPion 3x3");
	m_combo1.AddString(L"lab4_filtr SobelaPoziom 3x3");
	m_combo1.AddString(L"lab4_filtr Laplasjanu 3x3");
	m_combo1.AddString(L"lab4_filtr Wyostrzajaca 3x3");
	m_combo1.AddString(L"lab4_filtr medianowy 3x3");
	m_combo1.AddString(L"lab4_filtr medianowy 5x5");
	m_combo1.AddString(L"lab4_filtr medianowy krzyz");
	m_combo1.AddString(L"lab4_log");


	m_combo1.SelectString(0, L"lab1_convert to greyscale");
	




	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPODlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPODlg::OnBnClickedButtonLoad()
{
	WCHAR strFilter[] = { L"Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||" };

	CFileDialog FileDlg(TRUE, NULL, NULL, 0, strFilter);

	if (FileDlg.DoModal() == IDOK)
	{
		filePath = FileDlg.GetPathName();
		m_imgIN.Image.LoadDIB(filePath);
		m_imgIN.doPaint = true;
		//MessageBox(L"Now what?", L";-)", MB_OK);
		InvalidateRect(NULL); // onPaint();

	}
	 
	
}


void CPODlg::OnBnClickedButtonProcess()
{
	CString sOption;
	m_combo1.GetLBText(m_combo1.GetCurSel(), sOption);

	m_imgOUT.doPaint = true;
	m_imgOUT.Image.LoadDIB(filePath); // wczytaj to samo zdjecie co m_img_in

	if (sOption == L"lab1_convert to greyscale")
	{
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
	}
	else if (sOption == L"lab2_change brightness") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		m_imgOUT.Image.ChangeBrightness(slider.GetPos());
	}
	else if (sOption == L"lab2_change kontrast") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		m_imgOUT.Image.ChangeContrast(slider.GetPos());
	}
	else if (sOption == L"lab2_change potega") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		m_imgOUT.Image.ChangeExponent(slider.GetPos());
	}
	else if (sOption == L"lab2_negatyw") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		m_imgOUT.Image.Negative();
	}
	else if (sOption == L"lab2_wyswietlenie histogramu") {
		m_imgOUT.Image.CalculateHistogram(0, 0, m_imgOUT.Image.fileWidth, m_imgOUT.Image.fileHeight);
		m_imgOUT.Image.ShowHistogram(-2);
	}
	else if (sOption == L"lab2_wyrownanie histogramu") {
		m_imgOUT.Image.CalculateHistogram(0, 0, m_imgOUT.Image.fileWidth, m_imgOUT.Image.fileHeight);
		m_imgOUT.Image.EqualizeHistogram();
		m_imgOUT.Image.ShowHistogram(-2);
	}
	else if (sOption == L"lab3_reczne progowanie") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		m_imgOUT.Image.CalculateHistogram(0, 0, m_imgOUT.Image.fileWidth, m_imgOUT.Image.fileHeight);
		m_imgOUT.Image.ManualThreshold(slider.GetPos());
		m_imgOUT.Image.ShowHistogram(slider.GetPos());
	}
	else if (sOption == L"lab3_iteracyjne progowanie") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		m_imgOUT.Image.IterativeSegmentation();
	}
	else if (sOption == L"lab3_gradient progowanie") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		m_imgOUT.Image.GradientSegmentation();
	}
	else if (sOption == L"lab3_otsu progowanie") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		int threshold = m_imgOUT.Image.OtsuSegmentation();
		m_imgOUT.Image.ShowHistogram(threshold); // pokazuje rozklad funkcji 1/odchylenieStandardowe zamiast histogramu
	}
	else if (sOption == L"lab4_filtr srednia 3x3") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		int mask[3][3] = { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
		m_imgOUT.Image.LinearFilter(mask, false);
	}
	else if (sOption == L"lab4_filtr Gaussa 3x3") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		int mask[3][3] = { { 1, 4, 1 }, { 4, 12, 4 }, { 1, 4, 1 } };
		m_imgOUT.Image.LinearFilter(mask, false);
	}
	else if (sOption == L"lab4_filtr SobelaPion 3x3") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		int mask[3][3] = { { 1, 2, 1 }, { 0, 0, 0}, { -1, -2, -1 } };
		m_imgOUT.Image.LinearFilter(mask, false);
	}
	else if (sOption == L"lab4_filtr SobelaPoziom 3x3") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		int mask[3][3] = { { -1, 0, 1 }, { -2, 0, 2}, { -1, 0, 1 } };
		m_imgOUT.Image.LinearFilter(mask, false);
	}
	else if (sOption == L"lab4_filtr Laplasjanu 3x3") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		int mask[3][3] = { { -2, 1, -2 }, { 1, 4, 1}, { -2, 1, -2 } };
		m_imgOUT.Image.LinearFilter(mask, false);
	}
	else if (sOption == L"lab4_filtr Wyostrzajaca 3x3") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		int mask[3][3] = { { 0, -1, 0 }, { -1, 5, -1}, { 0, -1, 0 } };
		m_imgOUT.Image.LinearFilter(mask, true);
	}
	else if (sOption == L"lab4_filtr medianowy 3x3") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		m_imgOUT.Image.MedianFilter(3, 0); // medianowy 3x3
	}
	else if (sOption == L"lab4_filtr medianowy 5x5") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		m_imgOUT.Image.MedianFilter(5, 0); // medianowy 5x5
	}
	else if (sOption == L"lab4_filtr medianowy krzyz") { // medianowy krzyz 3x3
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		m_imgOUT.Image.MedianFilter(3, 1); // krzyz 5 elementowy
	}
	else if (sOption == L"lab4_log") {
		m_imgOUT.Image.CreateGreyscaleDIB(NULL, 0, 0);
		//int mask[3][3] = { { -1, -1, -1 }, { -1, 8, -1}, { -1, -1, -1 } };
		//m_imgOUT.Image.LinearFilter(mask, true);
		//m_imgOUT.Image.Negative();
		m_imgOUT.Image.LogFilter(slider.GetPos());
		

	}


	
	InvalidateRect(NULL);
}

// save
void CPODlg::OnBnClickedButtonSave()
{
	WCHAR strFilter[] = { L"Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||" };

	CFileDialog FileDlg(FALSE, NULL, NULL, 0, strFilter);

	if (FileDlg.DoModal() == IDOK)
	{
		//MessageBox(L"Now what?", L";-)");
		CString fileName = FileDlg.GetPathName();
		fileName += ".bmp";
		m_imgOUT.Image.SaveDIB(fileName);
	}
}


void CPODlg::OnBnClickedButtonParams()
{
	CParamsDlg paramsDlg;
	CString s;
	
	if (paramsDlg.DoModal() == IDOK)
	{
		s = paramsDlg.m_sParams;
	}
}


void CPODlg::OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CPODlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	CString sOption;
	m_combo1.GetLBText(m_combo1.GetCurSel(), sOption);
	if (sOption == L"lab2_change brightness") {
		slider.SetRangeMin(-100);
		slider.SetRangeMax(100);
		slider.SetPos(0);
	}
	else if (sOption == L"lab2_change kontrast") {
		slider.SetRangeMin(1);
		slider.SetRangeMax(50);
		slider.SetPos(10);
	}
	else if (sOption == L"lab2_change potega") {
		slider.SetRangeMin(1);
		slider.SetRangeMax(30);
		slider.SetPos(0);
	}
	else if (sOption == L"lab3_reczne progowanie") {
		slider.SetRange(0, 255);
		slider.SetPos(0);
	}
	else if (sOption == L"lab4_log") {
		slider.SetRange(6, 24);
		slider.SetPos(15);
	}
}
