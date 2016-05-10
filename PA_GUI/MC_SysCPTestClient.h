// MC_SysCPTestClient.h : main header file for the MC_SysCPTESTCLIENT application
//
#if 0


#if !defined(AFX_MC_SysCPTESTCLIENT_H__2B160C3D_8018_4D2C_A7FC_AB4B6EBA01F8__INCLUDED_)
#define AFX_MC_SysCPTESTCLIENT_H__2B160C3D_8018_4D2C_A7FC_AB4B6EBA01F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#pragma warning(disable: 4100)

// manually added by jeh 21-Jun-12

#define WM_USER_KILL_COM_DLG					WM_USER+0x200
#define WM_USER_KILL_ADP_CONNECTION					WM_USER+0x201
#define WM_USER_RESTART_ADP_CONNECTION				WM_USER+0x202
#define WM_USER_INIT_TCP_THREAD						WM_USER+0x207
#define WM_USER_RESTART_TCP_COM_DLG					WM_USER+0x208
#define WM_USER_CLOSE_TCPIP_CONNECTION				WM_USER+0x210
#define WM_USER_SEND_TCPIP_PACKET					WM_USER+0x211
#define WM_USER_CLIENT_PKT_RECEIVED					WM_USER+0x212


#define IDT_ADP_RESTART_TIMER						109

/////////////////////////////////////////////////////////////////////////////
// CMC_SysCPTestClientApp:
// See MC_SysCPTestClient.cpp for the implementation of this class
//

class CMC_SysCPTestClientApp : public CWinApp
{
public:
	CMC_SysCPTestClientApp();
	int m_iWinVer;
	HMODULE m_hLanguage;	// added 13-Jul-12 from YC by jeh
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMC_SysCPTestClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMC_SysCPTestClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MC_SysCPTESTCLIENT_H__2B160C3D_8018_4D2C_A7FC_AB4B6EBA01F8__INCLUDED_)

#endif
