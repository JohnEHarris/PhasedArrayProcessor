// IpxIn.cpp : implementation file
// 01/18/01
// Collect all ipx packet handling into a separate thread which
// runs at higher priority than the inspection mmi thread
//

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

//#include "IpxIn.h"	included in tscandlg.h

#include "Extern.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CIpxIn

IMPLEMENT_DYNCREATE(CIpxIn, CWinThread)



CIpxIn::CIpxIn()
	{
	m_nLoop = 0;
	}

CIpxIn::~CIpxIn()
	{
//	if (CIpxDlg::m_pDlg) CIpxDlg::m_pDlg->KillMe();
	m_nLoop = 2;
	}

BOOL CIpxIn::InitInstance()
	{
	// TODO:  perform and per-thread initialization here
	m_nLoop = 0;
	hIpxInThread = (HWND) AfxGetInstanceHandle();
//	hIpxInThread = m_hThread;

	if (CIpxDlg::m_pDlg == NULL)
		{
//		DestroyDataClients();		// Kill all other data consumers
		CIpxDlg::m_pDlg = new CIpxDlg;
		if (CIpxDlg::m_pDlg) CIpxDlg::m_pDlg->Create(IDD_IPX_THREAD_DLG);
		}

	return TRUE;
	}

#if 1
int CIpxIn::ExitInstance()
	{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
	}

#endif

// Wrappers to get from VC6 to Visual Studio 10
// this foolishness required to go from Visual Studio to Visual NET
afx_msg LRESULT CIpxIn::VS10_ExitInstance(WPARAM, LPARAM)
	{	ExitInstance();	return 0;	}


BEGIN_MESSAGE_MAP(CIpxIn, CWinThread)
	//{{AFX_MSG_MAP(CIpxIn)
	//}}AFX_MSG_MAP
	//Manually add message to kill window  .. jeh

//	ON_MESSAGE(WM_USER_ACAL_ERASE, VS10_EraseBars)  // this required to go from Visual Studio 6 to Visual Studio 2010
//  add to header file
//	afx_msg LRESULT VS10_EraseBars(WPARAM, LPARAM);	// this foolishness required to go from Visual Studio to Visual NET
// IN this cpp file call EraseBars from inside VS10_EraseBars

	//ON_MESSAGE(WM_USER_KILL_IPX_THREAD, ExitInstance)

	ON_MESSAGE(WM_USER_KILL_IPX_THREAD,  &VS10_ExitInstance )

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CIpxIn message handlers
#if 0
int CIpxIn::ExitInstance()
	{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
	}
#endif