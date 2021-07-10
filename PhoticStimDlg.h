
// PhoticStimDlg.h : header file
//

#pragma once

#include "C:\Program Files (x86)\National Instruments\NI-DAQ\DAQmx ANSI C Dev\include\NIDAQmx.h"		// DAQ card ansi C functions
#include <vector>			//sýralama vektü için
#include <fstream>			//file open için
#include "afxmt.h"			//mutithread için
using namespace std;
// CPhoticStimDlg dialog

const int WM_SYNCGELDI = WM_USER + 100;  //sync geldi için mesaj
const int WM_THREADSON = WM_USER + 101;  //sync geldi için mesaj

class CPhoticStimDlg : public CDialogEx
{
// Construction
public:
	CPhoticStimDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PHOTICSTIM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonload();
	afx_msg void OnBnClickedButtonstart();
	afx_msg LONG OnSyncGeldi(WPARAM wParam, LPARAM lParam);//thread mesajý
	afx_msg LONG OnThreadSon(WPARAM wParam, LPARAM lParam);//thread mesajý
	int m_iEditSync;
	int m_iEditSyncSum;
	int m_iFreq;
	afx_msg void OnEnChangeEditfreq();
};
