// Asi.h : main header file for the ASI application

// Rename to Truscan.h for UT/Utron/Truscope Truscan project
//	5/30/00  jeh
//

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
#define MEM_MAX_CHANNEL		100
#define MAX_SHOES	10
#define MAX_GATE	2
#define XDCR_TYPE	6
/////////////////////////////////////////////////////////////////////////////
// CTscanApp:
// See Asi.cpp for the implementation of this class
//

class CTscanApp : public CWinApp
{
public:
	CTscanApp();
	OnIdle(LONG lCount);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTscanApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTscanApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRUSCOPE2_H__E654F6C5_C864_11D3_9B48_00A0C91FC401__INCLUDED_)
