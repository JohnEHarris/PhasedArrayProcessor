// Replay.cpp : implementation file
/*

11/29/00 Change structure of info stored in combo box.  Now include
		file position of data for this joint.  Get file position
		by converting from ascii to bin.  Necessary since combo box
		displays data sorted by joint number which may not be order in
		which recorded..  Position index file in order of  recording.

  04/30/02 Continuous replay for otc demo

  */
//



#include "stdafx.h"
#include "Truscan.h"
// include "Replay.h"
#include "TscanDlg.h"

#include "Extern.h"
#include <time.h>

extern CString szFlaw;

#define REPLAY_IS_MODELESS		1

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CString sconfig;
/////////////////////////////////////////////////////////////////////////////
// CReplay dialog


CReplay::CReplay(CWnd* pParent /*=NULL*/)
	: CDialog(CReplay::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReplay)
	m_bWallStat = FALSE;
	//}}AFX_DATA_INIT

}


void CReplay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReplay)
	DDX_Control(pDX, IDC_Rjoint, m_joint);
	DDX_Control(pDX, IDC_RTime, m_time);
	DDX_Control(pDX, IDC_LAST_JNT, m_ceLastJoint);
	DDX_Control(pDX, IDC_LIST_NEXT_JNT, m_lbNextJoint);
	DDX_Check(pDX, IDC_CK_WALL_STAT2, m_bWallStat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReplay, CDialog)
	//{{AFX_MSG_MAP(CReplay)
	ON_BN_CLICKED(IDC_RP_PLAY, OnRpPlay)
	ON_LBN_SELCHANGE(IDC_LIST_NEXT_JNT, OnSelchangeListNextJnt)
	ON_BN_CLICKED(IDC_AUTO_PRINT, OnAutoPrint)
	ON_WM_SIZE()
	ON_LBN_DBLCLK(IDC_LIST_NEXT_JNT, OnDblclkListNextJnt)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CK_WALL_STAT2, OnCkWallStat2)
	ON_BN_CLICKED(IDC_Rjoint, OnRjoint)
	ON_BN_CLICKED(IDC_RP_QUIT, OnRpQuit)
	ON_BN_CLICKED(IDC_RTime, OnRTime)
	//}}AFX_MSG_MAP

	//Manually add message to continuously replay pipe at OTC 4/30/02...JEH
	ON_MESSAGE(WM_USER_CONTINUOUS_REPLAY, VS10_OnRpPlay)
	
END_MESSAGE_MAP()

// Wrappers to get from VC6 to Visual Studio 10
// this foolishness required to go from Visual Studio to Visual NET
afx_msg LRESULT CReplay::VS10_OnRpPlay(WPARAM, LPARAM)
	{	OnRpPlay();	return 0;	}

/////////////////////////////////////////////////////////////////////////////
// CReplay message handlers

void CReplay::OnCancel() 
	{
	// TODO: Add extra cleanup here
#if REPLAY_IS_MODELESS

	m_pDlg = NULL;

	if (pCTscanDlg->m_pInspectDlg1)
	{
//		::PostMessage(pCTscanDlg->m_pInspectDlg1->m_hWnd, WM_USER_INSPECT_STOP,
//					0,0);
		if (pCTscanDlg->m_pInspectDlg1->m_nControlSel != INSPECT_STOP)
		{
			//int tmp =pCTscanDlg->m_pInspectDlg1->m_nID;
			//pCTscanDlg->m_pInspectDlg1->m_nID = -1 ;
			pCTscanDlg->m_pInspectDlg1->OnStop();
			//pCTscanDlg->m_pInspectDlg1->m_nID = tmp;
			//SetDlgItemText (IDC_EDIT_JOINTNUM, NULL);
		}
	}
	pCTscanDlg->RestoreOriginalConfigRec();
	CstringToTChar(sconfig,ConfigRec.JobRec.CfgFile);
	//strcpy (ConfigRec.JobRec.CfgFile, (LPCTSTR) sconfig);
	if (pCTscanDlg->m_pInspectDlg1)
		pCTscanDlg->m_pInspectDlg1->ShowDlgTitle();

#endif

	// Save closing location of window
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition(_T("REPLAY"), &wp);

	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CReplay::OnOK() 
	{
	// TODO: Add extra validation here
	WINDOWPLACEMENT wp;

	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition(_T("REPLAY"), &wp);
	
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

void CReplay::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::PostNcDestroy();


	// Same code for all windows
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
#if REPLAY_IS_MODELESS
	m_pDlg = NULL;
	delete this;
#endif
	}

void CReplay::KillMe()
	{
	// Public access to OnCancel
	//OnCancel();
	OnRpQuit();
	}

void CReplay::OnRpPlay() 
	{
	// TODO: Add your control notification handler code here
	CString s;
	szFlaw = _T("");
	int i;
	i = ReplayRec.LastJoint = ReplayRec.NextJoint;
//	ReplayRec.Pos = m_dwPos.GetAt(i);
	m_lbNextJoint.GetText(i, s);
	ReplayRec.Pos = GetReplayPosition(&s);
//		nLboxCnt = m_lbNextJoint.GetCount();

	if ( ReplayRec.LastJoint < m_lbNextJoint.GetCount())
		{
		m_lbNextJoint.GetText(ReplayRec.LastJoint, s);	// get last selection
		if (m_time.GetCheck() == 1)
		{
			pCTscanDlg->m_pInspectDlg1->m_tTime = s.Left(18);
			s = s.Mid(20,7);
		}
		else
		{
			pCTscanDlg->m_pInspectDlg1->m_tTime = s.Mid(10,18);
			s.SetAt(7,0);	// drop the location from being output
		}
		m_ceLastJoint.SetWindowText(s);
		}

	ReplayRec.NextJoint = 	ReplayRec.LastJoint + 1;;
	pCTscanDlg->m_uJntNum = _tstoi(s);

	// Save screen position of dialog for repositioning on next 
	// time dialog is run  See prosise p835
	if (pCTscanDlg->m_pInspectDlg1) pCTscanDlg->m_pInspectDlg1->PlayOnePipe();
//	OnOK();

//	MessageBox(s);
	i++;
	if ( i <= m_lbNextJoint.GetCount() )
		m_lbNextJoint.SetCurSel(i); 
	OnSelchangeListNextJnt();	
	}

void CReplay::OnRpTholds() 
	{
	// TODO: Add your control notification handler code here
	// Call Tholds dialog as modal instead of modeless
	if ( CTholds::m_pDlg) CTholds::m_pDlg->KillMe();
		// kill the modeless version

	CTholds Tdlg;	// make modal version on stack
	Tdlg.DoModal();
	
	}

#if 0

void CReplay::OnRpOpen() 
	{
	// TODO: Add your control notification handler code here
	MessageBox("Open file");
	OnCancel();
	}
#endif

void CReplay::OnRpQuit() 
	{
	// TODO: Add your control notification handler code here
	m_dwPos.RemoveAll();
	//pCTscanDlg->m_pInspectDlg1->m_pDataFile = NULL;
	OnCancel();
	}


/*****  This procedure runs when play is selected from the inspect dialog and each
		time a pipe ends in replay.
******/

BOOL CReplay::OnInitDialog() 
	{

	int nLboxCnt;		// how many in list box
	CString s;
	struct tm *Time;
	CFile *pIndexFile;
	INDEX_REC iRec;
	int nSize;
	int flag;

	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window

	CDialog::OnInitDialog();
//	sconfig = ConfigRec.JobRec.CfgFile;
	GetWindowPlacement(&wp);
	dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	pCTscanDlg->GetWindowLastPosition(_T("REPLAY"), &rect);
	if ( ( (rect.right - rect.left) >= dx ) &&
		 ( (rect.bottom - rect.top) >= dy ))
		{
		wp.rcNormalPosition = rect;
		SetWindowPlacement(&wp);
		}

	// TODO: Add extra initialization here
	// remember where we were when we terminated by checking the registry
	// See Prosise p 834
	// Set Reg key must be in the app, not in child windows
//	SetRegistryKey(_T("Tuboscope"));	// gen HKEY_CUR_USR\Software\Tuboscope\Asi  key

	// Get a pointer to the application to read/write registry entries
	m_ptheApp = (CTscanApp *) AfxGetApp();
//	m_ptheApp->WriteProfileString(_T("Version 1.0"), _T("Replay"),
//									_T("This is a test"));
	// Restore screen position at closing

	pIndexFile = NULL;

	if (pCTscanDlg->m_pInspectDlg1)
		{
		flag = pCTscanDlg->m_pInspectDlg1->GetAutoPrintFlag();
		CheckDlgButton(IDC_AUTO_PRINT,flag);
	

		// Fill the list box with pipe numbers to select from
		pIndexFile = pCTscanDlg->m_pInspectDlg1->GetIndexFile();
		}
	if (!pIndexFile)
		{// fatal error
		MessageBox(_T("Failed to Open Index File"),_T("Fatal Error"));
		OnCancel();
		return TRUE;
		}

       if (m_joint.GetCheck() == 0) m_time.SetCheck(1);

	pIndexFile->Seek(0, CFile::begin);

	nSize = pIndexFile->Read(&iRec, sizeof(INDEX_REC));
	while (nSize == sizeof(INDEX_REC))
		{
		Time = localtime(&iRec.time);
		if (Time!=NULL)
		{
			if (m_time.GetCheck() == 1)
				s.Format(_T("%04d-%02d-%02d  %02d:%02d   %07u      P=%d"),1900+Time->tm_year,
				Time->tm_mon+1, Time->tm_mday, Time->tm_hour, 
				Time->tm_min, iRec.JointNumber,
				iRec.pos);
			else
				s.Format(_T("%07u   %04d-%02d-%02d  %02d:%02d      P=%d"), iRec.JointNumber,
				1900+Time->tm_year,Time->tm_mon+1, Time->tm_mday, Time->tm_hour, 
				Time->tm_min,
				iRec.pos);
		}
		m_lbNextJoint.AddString(s);
		//.Add(iRec.pos);
		nSize = pIndexFile->Read(&iRec, sizeof(INDEX_REC));
		}

	nLboxCnt = m_lbNextJoint.GetCount();
/*
	if ( ReplayRec.LastJoint < nLboxCnt)
		{
		m_lbNextJoint.GetText(ReplayRec.LastJoint, s);	// get last selection
		s.SetAt(7,0);	// drop the location from being output
		m_ceLastJoint.SetWindowText(s);
		}

	ReplayRec.NextJoint = 	ReplayRec.LastJoint + 1;;
*/	
	//if ( ReplayRec.NextJoint >= nLboxCnt)
	ReplayRec.NextJoint = 0;
	m_lbNextJoint.SetCurSel(ReplayRec.NextJoint);

	m_uIpxTimer = SetTimer(IDT_IPX_TIMER, 150, NULL);
	if (!m_uIpxTimer) MessageBox(_T("Failed to start IPX timer"));
	m_nTick = 0;

	m_bWallStat = ConfigRec.JobRec.WallStatFlag;
//	m_bWallStat = ConfigRec.JobRec.WallStatFlag = 0;

	UpdateData(FALSE);	// Copy variables to screen
	
	pCTscanDlg->m_bInstAutoRun = FALSE;
//	SendMsg(INST_AUTORUN);
	SendMsg(NOP_MODE);

	SetWindowPos (&wndTopMost, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CReplay::OnSelchangeListNextJnt() 
	{
	// TODO: Add your control notification handler code here
	CString s;
	int i = m_lbNextJoint.GetCurSel();
	m_lbNextJoint.GetText(i, s);
	ReplayRec.NextJoint = i++;
//	ReplayRec.Pos = m_dwPos.GetAt(nUnsortedIndex);
	ReplayRec.Pos = GetReplayPosition(&s);

//	MessageBox(s);
	}


void CReplay::OnAutoPrint() 
	{
	// TODO: Add your control notification handler code here
	int flag;
/*	if (pCTscanDlg->m_pInspectDlg1)
		{
		pCTscanDlg->m_pInspectDlg1->OnAutoPrint2();
		flag = pCTscanDlg->m_pInspectDlg1->GetAutoPrintFlag();
		CheckDlgButton(IDC_AUTO_PRINT,flag);
		}
*/
	if (pCTscanDlg->m_pInspectDlg1)
		{
		pCTscanDlg->m_pInspectDlg1->OnINSPreport();
		flag = pCTscanDlg->m_pInspectDlg1->GetAutoPrintFlag();
		CheckDlgButton(IDC_AUTO_PRINT,flag);
		}


	}

void CReplay::OnSize(UINT nType, int cx, int cy) 
	{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
//	if (nType == SIZE_MINIMIZED) pCTscanDlg->m_pInspectDlg1->Refresh();
	}

void CReplay::OnDblclkListNextJnt() 
	{
	// TODO: Add your control notification handler code here
	OnRpPlay();	
	}

void CReplay::OnTimer(UINT nIDEvent) 
	{
	// TODO: Add your message handler code here and/or call default

	if ( m_nTick == 2)
		{
		ShowWindow(SW_SHOW);
		UpdateWindow();
		}
	
	m_nTick++;


#if OTC_CONTINUOUS_LOOP
	// Automatically play the next pipe in the list
	if (m_nTick == 30)
		{	// about 3 seconds after InitDialog runs
		// Send ourselves a request to play the next pipe in the list
		::PostMessage(m_hWnd, WM_USER_CONTINUOUS_REPLAY, 0, 0);
		}


#endif
	CDialog::OnTimer(nIDEvent);
	}

void CReplay::OnCkWallStat2() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	ConfigRec.JobRec.WallStatFlag = m_bWallStat;
//	ConfigRec.JobRec.WallStatFlag = 0;	// until wall opts resolved
	}

DWORD CReplay::GetReplayPosition(CString *s)
	{
	// Replay file position is embedded as ascii in combo box output
	// Extract the position and return actual file loc to caller

	char buf[48], *p;
	DWORD pos;
	CString t;
	t = *s;
	CstringToChar(t,buf);
	//strcpy(buf, *s);
	p = strstr(buf,"P");
	sscanf(&p[2], "%ld", &pos);
	return pos;
	}

BOOL CReplay::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Tscandlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);

	}

void CReplay::OnRjoint() 
{
	// TODO: Add your control notification handler code here
	m_lbNextJoint.ResetContent();
	m_time.SetCheck(0);
	m_joint.SetCheck(1);
	OnInitDialog();	
	
}

void CReplay::OnRTime() 
{
	// TODO: Add your control notification handler code here
	m_lbNextJoint.ResetContent();
	m_time.SetCheck(1);
	m_joint.SetCheck(0);
	OnInitDialog();	
}
