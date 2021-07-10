
// PhoticStimDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhoticStim.h"
#include "PhoticStimDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

double glob_iFreq;
vector<int> vecBlock;
UINT ThreadProcdaq(LPVOID param);

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPhoticStimDlg dialog



CPhoticStimDlg::CPhoticStimDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PHOTICSTIM_DIALOG, pParent)
	, m_iEditSync(0)
	, m_iEditSyncSum(0)
	, m_iFreq(46)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPhoticStimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITSync, m_iEditSync);
	DDX_Text(pDX, IDC_EDITfreq, m_iFreq);
	DDV_MinMaxInt(pDX, m_iFreq, 0, 99);
	DDX_Text(pDX, IDC_EDITsyncSum, m_iEditSyncSum);
}

BEGIN_MESSAGE_MAP(CPhoticStimDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SYNCGELDI, OnSyncGeldi)
	ON_MESSAGE(WM_THREADSON, OnThreadSon)
	ON_BN_CLICKED(IDC_BUTTONLOAD, &CPhoticStimDlg::OnBnClickedButtonload)
	ON_BN_CLICKED(IDC_BUTTONSTART, &CPhoticStimDlg::OnBnClickedButtonstart)
	ON_EN_CHANGE(IDC_EDITfreq, &CPhoticStimDlg::OnEnChangeEditfreq)
END_MESSAGE_MAP()


// CPhoticStimDlg message handlers

BOOL CPhoticStimDlg::OnInitDialog()
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
	GetDlgItem(IDC_BUTTONSTART)->EnableWindow(FALSE);
	//GetDlgItem(IDOK)->EnableWindow(FALSE);
	glob_iFreq = 46;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPhoticStimDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPhoticStimDlg::OnPaint()
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
HCURSOR CPhoticStimDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPhoticStimDlg::OnBnClickedButtonload()
{
	bool parantez = FALSE;
	int iokunan, dummy;
	CString strBlok;
	TCHAR strFilter[] = _T("Settings file (*.txt)|*.txt|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T(".txt"), NULL, 0, strFilter);
	if (fileDlg.DoModal() == IDOK)
	{
		ifstream file_op(fileDlg.GetPathName());
		if (!file_op.good() )
		{
			MessageBox(_T("File Could Not Open"), _T("Error"), MB_OK);
			return;
		}
		vecBlock.clear();
		strBlok.Format(_T("Blocks:"));
		while (!file_op.eof())
		{
			file_op >> iokunan;
			if (parantez = !parantez)
				strBlok.Format(strBlok + _T(" %d"), iokunan);
			else
				strBlok.Format(strBlok + _T(" (%d)"), iokunan);
			vecBlock.push_back(iokunan);
		}
		GetDlgItem(IDC_STATICBLOK)->SetWindowTextW(strBlok);
		file_op.close();
		GetDlgItem(IDC_BUTTONSTART)->EnableWindow(TRUE);
		m_iEditSyncSum = 0;
		for (dummy = 0; dummy < int(vecBlock.size()); ++dummy) {
			m_iEditSyncSum += vecBlock[dummy];
		}
		UpdateData(FALSE);
	}
}

UINT ThreadProcdaq(LPVOID param)
{
	int         error = 0;
	TaskHandle  taskHandleEEGmarker = 0;
	TaskHandle  taskHandleFlash = 0;
	float64     data[2] = { 5.0, 0.0 };//EEG üzerine iþaret koy
	char        errBuff[2048] = { '\0' };
	CString		CStrError;
	int32   	written;
	float64		freq = glob_iFreq; //flashing light freq.
	int			blokind, bloksyncind;
	int			bloklen = static_cast<int>(vecBlock.size());
	int			blokaktif = 0; //ilk blok pasif
	//*********************************************
	// DAQmx Configure Code (EEG marker)
	//*********************************************
	DAQmxErrChk(DAQmxCreateTask("", &taskHandleEEGmarker));
	DAQmxErrChk(DAQmxCreateAOVoltageChan(taskHandleEEGmarker, "Dev1/ao0", "", 0.0, 5.0, DAQmx_Val_Volts, NULL));
	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandleEEGmarker, "", 5000.0, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, 2));
	DAQmxErrChk(DAQmxCfgDigEdgeStartTrig(taskHandleEEGmarker, "/Dev1/PFI0", DAQmx_Val_Rising));
	//*********************************************
	// DAQmx Write Code
	//*********************************************
	DAQmxErrChk(DAQmxWriteAnalogF64(taskHandleEEGmarker, 2, 0, 100.0, DAQmx_Val_GroupByChannel, data, &written, NULL));
	//*********************************************
	// DAQmx Configure Code (flashing light)
	//*********************************************
	DAQmxErrChk(DAQmxCreateTask("", &taskHandleFlash));
	DAQmxErrChk(DAQmxCreateCOPulseChanFreq(taskHandleFlash, "Dev1/ctr0", "", DAQmx_Val_Hz, DAQmx_Val_Low, 0.0, freq, 0.50));
	DAQmxErrChk(DAQmxCfgImplicitTiming(taskHandleFlash, DAQmx_Val_ContSamps, 5000));
	//*********************************************
	// DAQmx Start Code
	//*********************************************
	for (blokind = 0; blokind < bloklen; ++blokind)
	{
		blokaktif = blokind % 2; //tek bloklar aktif 1,3,5,7...
		for (bloksyncind = 0; bloksyncind < vecBlock[blokind]; ++bloksyncind)
		{
			//DAQmxErrChk(DAQmxStartTask(taskHandleEEGmarker));
			//sync awaiting(120sn), in order to send EEGmarker signal
			//DAQmxErrChk(DAQmxWaitUntilTaskDone(taskHandleEEGmarker, 120.0));
			// for testing, use sleep instead of MR sync signal. 
			Sleep(1000);

			if (bloksyncind == 0)
			{
				if (blokaktif)//blok aktif bloksa
				{
					DAQmxErrChk(DAQmxStartTask(taskHandleFlash));
				}
				else// if( blokind !=0 )//hiç baþlamamýþý durdurmamak için ama gerek yokmuþ
				{
					DAQmxErrChk(DAQmxStopTask(taskHandleFlash));
				}
			}
			::PostMessage((HWND)param, WM_SYNCGELDI, 0, 0);
			//DAQmxErrChk(DAQmxStopTask(taskHandleEEGmarker));
		}
	}
Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
		CStrError.Format(_T("DAQmx Error: %s"), errBuff);
		::MessageBox((HWND)param, CStrError, _T("Error"), MB_OK);//
	}
	//*********************************************
	// DAQmx Stop Code
	//*********************************************
	if (taskHandleEEGmarker != 0)
	{
		DAQmxStopTask(taskHandleEEGmarker);
		DAQmxClearTask(taskHandleEEGmarker);
	}
	if (taskHandleFlash != 0)
	{
		DAQmxStopTask(taskHandleFlash);
		DAQmxClearTask(taskHandleFlash);
	}
	::PostMessage((HWND)param, WM_THREADSON, 0, 0);
	return 0;
}

void CPhoticStimDlg::OnBnClickedButtonstart()
{
	HWND hWnd = GetSafeHwnd();

	AfxBeginThread(ThreadProcdaq, hWnd, THREAD_PRIORITY_NORMAL);
	GetDlgItem(IDC_BUTTONSTART)->EnableWindow(FALSE);
	//GetDlgItem(IDOK)->EnableWindow(FALSE);
}

LONG CPhoticStimDlg::OnSyncGeldi(WPARAM wParam, LPARAM lParam)
{
	++m_iEditSync;
	UpdateData(FALSE);
	return 0;
}

LONG CPhoticStimDlg::OnThreadSon(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_BUTTONSTART)->EnableWindow(TRUE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	m_iEditSync = 0;
	UpdateData(FALSE);
	return 0;
}


void CPhoticStimDlg::OnEnChangeEditfreq()
{
	UpdateData(TRUE);
	glob_iFreq = double(m_iFreq);
}

