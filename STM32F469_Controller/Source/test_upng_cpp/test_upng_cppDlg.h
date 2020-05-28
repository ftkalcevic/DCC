
// test_upng_cppDlg.h : header file
//

#pragma once

#include <stdint.h>

// CtestupngcppDlg dialog
class CtestupngcppDlg : public CDialogEx
{
// Construction
public:
	CtestupngcppDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_UPNG_CPP_DIALOG };
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
	void ProcessPng();
	void LoadPng();

	uint32_t bufferLen;
	uint8_t* buffer;
	int bpp;
};
