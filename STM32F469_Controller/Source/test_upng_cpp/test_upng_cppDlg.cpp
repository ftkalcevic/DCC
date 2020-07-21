
// test_upng_cppDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "test_upng_cpp.h"
#include "test_upng_cppDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <assert.h>
#include "../STM32F469_Disco_DCC/upng.h"

// CAboutDlg dialog used for App About

//#define INPUTFILE "..\\Images\\simple.png"
//#define INPUTFILE "..\\Images\\test.png"
//#define INPUTFILE "..\\Images\\Lines.png"
//#define INPUTFILE "..\\Images\\Splash.png"
//#define INPUTFILE "..\\Images\\background.png"
#define INPUTFILE "..\\sdmedia\\Images\\Marklin8840_small.png"

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


// CtestupngcppDlg dialog



CtestupngcppDlg::CtestupngcppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TEST_UPNG_CPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	bufferLen = 0;
	buffer = NULL;
}

void CtestupngcppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtestupngcppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CtestupngcppDlg message handlers

BOOL CtestupngcppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	LoadPng();
	ProcessPng();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CtestupngcppDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CtestupngcppDlg::OnPaint()
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
		//CDialogEx::OnPaint();

		CPaintDC dc(this); // device context for painting

		CImage pngImage;
		CBitmap pngBmp;
		CDC bmDC;
		CBitmap* pOldbmp;
		BITMAP  bi;

		pngImage.Load(_T(INPUTFILE));
		// new code

		pngBmp.Attach(pngImage.Detach());

		bmDC.CreateCompatibleDC(&dc);

		pOldbmp = bmDC.SelectObject(&pngBmp);
		pngBmp.GetBitmap(&bi);
		dc.BitBlt(0, 0, bi.bmWidth, bi.bmHeight, &bmDC, 0, 0, SRCCOPY);
		bmDC.SelectObject(pOldbmp);		

		int xPos, yPos;
		if (bi.bmWidth > bi.bmHeight)
		{
			xPos = 0;
			yPos = bi.bmHeight;
		}
		else
		{
			xPos = bi.bmWidth;
			yPos = 0;
		}
		CBitmap pngBmp2;
		pngBmp2.CreateCompatibleBitmap(&dc, bi.bmWidth, bi.bmHeight);
		//pngBmp2.CreateBitmap(bi.bmWidth, bi.bmHeight, 1, 24, buffer);
		//pngBmp2.SetBitmapBits(bi.bmWidth * bi.bmHeight * 3, buffer);
		pOldbmp = bmDC.SelectObject(&pngBmp2);
		uint8_t* ptr = buffer;
		for (int y = 0; y < bi.bmHeight; y++)
			for (int x = 0; x < bi.bmWidth; x++)
			{
				uint8_t r = *(ptr+0);
				uint8_t g = *(ptr+1);
				uint8_t b = *(ptr+2);
				ptr += (bpp + 7) / 8;
				bmDC.SetPixel(x, y, RGB(r, g, b));
			}
		dc.BitBlt(xPos, yPos, bi.bmWidth, bi.bmHeight, &bmDC, 0, 0, SRCCOPY);
		bmDC.SelectObject(pOldbmp);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CtestupngcppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


class MyFileStream : public Stream
{
	int fd;
public:
	MyFileStream(const char* file)
	{
		fd = -1;
		_sopen_s(&fd, file, O_RDONLY | O_BINARY, _SH_DENYWR, _S_IREAD);
	}
	~MyFileStream()
	{
		if ( fd >= 0 )
			_close(fd);
	}

	// Inherited via Stream
	virtual uint16_t ReadBytes(uint8_t* buffer, uint16_t len, uint16_t& bytesread) override
	{
		bytesread = _read(fd, buffer, len);
		return 0;
	}
	virtual uint32_t GetSize() override
	{
		long pos = _lseek(fd, 0, SEEK_CUR);
		long end = _lseek(fd, 0, SEEK_END);
		_lseek(fd, pos, SEEK_SET);
		return end;
	}
	virtual void Seek(uint32_t offset, uint8_t whence) override
	{
		_lseek(fd, offset, whence);
	}
};

void CtestupngcppDlg::ProcessPng()
{
	MyFileStream stream(INPUTFILE);
	upng upng(&stream);
	upng.header();

	bpp = upng.get_bpp();
	bufferLen = upng.get_width() * upng.get_height() * ((upng.get_bpp()+7)/8) + upng.get_height();	// We need 1 extra byte per row during processing
	buffer = new uint8_t [bufferLen+2*4];
	buffer += 4;
	*(uint32_t*)(buffer-4) = 0x0BADFEED;
	*(uint32_t*)(buffer+4+bufferLen) = 0x0BADFEED;
	upng.decode(buffer, bufferLen);
	assert(*(uint32_t*)(buffer-4) == 0x0BADFEED);
	assert(*(uint32_t*)(buffer + 4 + bufferLen) == 0x0BADFEED );

	int x = upng.get_width(), y = upng.get_height();
	if (x > y)
		y *= 2;
	else
		x *= 2;
	y += 50;
	x += 20;
	SetWindowPos(NULL, 0, 0, x, y, SWP_NOZORDER | SWP_NOMOVE);
}

void CtestupngcppDlg::LoadPng()
{
}

