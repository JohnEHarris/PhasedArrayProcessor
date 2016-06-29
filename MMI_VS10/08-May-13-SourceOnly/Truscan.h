// Asi.h : main header file for the ASI application

// Rename to Truscan.h for UT/Utron/Truscope Truscan project
//	5/30/00  jeh
//
// rah NEATNESS COUNTS
//     _WIN32_WINNT=0x601 is defined in the preprocessor so that Windows 7 headers are used
//     and it gets rid of the annoying compiler warnings about MAX_VER
//
#pragma warning (disable: 4101) // hide harmless but truly annoying compiler warnings


#if !defined(AFX_TRUSCOPE2_H__E654F6C5_C864_11D3_9B48_00A0C91FC401__INCLUDED_)
#define AFX_TRUSCOPE2_H__E654F6C5_C864_11D3_9B48_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "resource.h"		// main symbols
#include "winsock2.h"		// manually by jeh  does not work is
							// winsock support selected when project
							// created!!
// database related
#include "afxdb.h"			// afx database
#define MAX_BLOB_SIZE 8000
#define MEM_MAX_CHANNEL		100
#define MAX_SHOES	10
#define MAX_GATE	4
#define XDCR_TYPE	8
/////////////////////////////////////////////////////////////////////////////
// CTscanApp:
// See Asi.cpp for the implementation of this class
//


class CTscanApp : public CWinApp
{
public:
	CTscanApp();
	BOOL OnIdle(LONG lCount);
	int m_iWinVer;
	HMODULE m_hLanguage;

#ifdef _I_AM_PAG
public:
	// for remote system fuctionality
	BOOL m_hDBAvailable;
	BOOL m_hSysCPAvailable;
	BOOL m_hGDPAvailable;
	BOOL m_hPLCAvailable;

	HINSTANCE m_hMCDBI; // handle to MillConsoleDatabaseInterface DLL instance
	HINSTANCE m_hGDPI;	// handle to MillConsoleGDPInterface DLL instance
#endif

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTscanApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTscanApp)
	afx_msg void OnSetupChangePW();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRUSCOPE2_H__E654F6C5_C864_11D3_9B48_00A0C91FC401__INCLUDED_)
