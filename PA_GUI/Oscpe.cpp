// Oscpe.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
// include only the parent dialog header asidlg.h for all child dialogs
#include "TscanDlg.h"
#include "Extern.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//CRcvr *CRcvr::m_pDlg;	// 09/18/01


/////////////////////////////////////////////////////////////////////////////
// COscpe dialog
extern char szHelpFileName[];

COscpe::COscpe(CWnd* pParent /*=NULL*/)
	: CDialog(COscpe::IDD, pParent)
	{
	//{{AFX_DATA_INIT(COscpe)
	m_SLvl = _T("");
	m_nT2gatesync = -1;
	m_nT2CmdIndx = 0;
	m_nSeqPt = -1;
	m_n104Debug = FALSE;
	//}}AFX_DATA_INIT


	m_pUndo = new OSCOPE_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec.OscopeRec, sizeof(OSCOPE_REC));

	}


void COscpe::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COscpe)
	DDX_Control(pDX, IDC_CB_Trc1, m_CBTrc1);
	DDX_Control(pDX, IDC_SBLvl, m_SBLvl);
	DDX_Control(pDX, IDC_CB_Trgr, m_CBTrgr);
	DDX_Control(pDX, IDC_CB_Trc2, m_CBTrc2);
	DDX_Text(pDX, IDC_EDLvl, m_SLvl);
	DDV_MaxChars(pDX, m_SLvl, 20);
	DDX_Radio(pDX, IDC_RB_T2GATE, m_nT2gatesync);
	DDX_Text(pDX, IDC_T2CMD_INDX, m_nT2CmdIndx);
	DDV_MinMaxInt(pDX, m_nT2CmdIndx, 0, 14);
	DDX_Radio(pDX, IDC_RB_SEQ1, m_nSeqPt);
	DDX_Check(pDX, IDC_104_DEBUG, m_n104Debug);
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(COscpe, CDialog)
	//{{AFX_MSG_MAP(COscpe)
	ON_WM_HELPINFO()
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_CB_Trc2, OnSelchangeCBTrc2)
	ON_CBN_SELCHANGE(IDC_CB_Trgr, OnSelchangeCBTrgr)
	ON_BN_CLICKED(IDC_RB_T2GATE, OnRbT2gate)
	ON_BN_CLICKED(IDC_RB_T2SYNC, OnRbT2sync)
	ON_BN_CLICKED(IDC_RB_SEQ1, OnRbSeq1)
	ON_BN_CLICKED(IDC_RB_SEQ2, OnRbSeq2)
	ON_BN_CLICKED(IDC_OSCOPE_UNDO, OnOscopeUndo)
	ON_BN_CLICKED(IDC_104_DEBUG, On104Debug)
	ON_BN_CLICKED(IDC_OSCOPE_HELP, OnOscopeHelp)
	ON_CBN_SELCHANGE(IDC_CB_Trc1, OnSelchangeCBTrc1)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COscpe message handlers

void COscpe::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;

	}

BOOL COscpe::OnHelpInfo(HELPINFO* pHelpInfo) 
	{
	// TODO: Add your message handler code here and/or call default
	// Does nothing in current code
	return CDialog::OnHelpInfo(pHelpInfo);
	}

void COscpe::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	char sb_delta;
	switch(nSBCode)
			{
		case SB_PAGEDOWN :
			sb_delta = 10;
			break;
		case SB_LINEDOWN :
			sb_delta = 1;
			break;
		case SB_PAGEUP :
			sb_delta = -10;
			break;
		case SB_LINEUP :
			sb_delta = -1;
			break;


		default:
			sb_delta = 0;
			}

		ConfigRec.OscopeRec.reject_pcnt += sb_delta;
		if (ConfigRec.OscopeRec.reject_pcnt > 70) ConfigRec.OscopeRec.reject_pcnt = 70;
		if (ConfigRec.OscopeRec.reject_pcnt <  0) ConfigRec.OscopeRec.reject_pcnt = 0;

		m_SBLvl.SetScrollPos(ConfigRec.OscopeRec.reject_pcnt,TRUE);

		m_SLvl.Format(_T("%3d %%"), ConfigRec.OscopeRec.reject_pcnt);
		UpdateData(FALSE);		// Copy variables TO screen 

//		write_to_scope_demux();
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}



#define T2_GATES		1
#define T2_DIAG		2
#define T2_ALWAYS		0

	//	T2_GATES means this trace 2 selection available in "gates" mode
	//  T2_DIAG means this trace 2 selection available in debug mode
	//  defines also in tscandlg.cpp

BOOL COscpe::OnInitDialog() 
	{

	CDialog::OnInitDialog();
	CString s, t;
	int i, j, k;

	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window

	GetWindowPlacement(&wp);
	dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	pCTscanDlg->GetWindowLastPosition(_T("OSCILLOSCOPE"), &rect);
	if ( ( (rect.right - rect.left) >= dx ) &&
		 ( (rect.bottom - rect.top) >= dy ))
		{
		wp.rcNormalPosition = rect;
		SetWindowPlacement(&wp);
		}

	//m_nT2gatesync = ConfigRec.OscopeRec.T2GateOrSync[gChannel];
				// gate = 0, sync = 1
	m_nSeqPt = ConfigRec.OscopeRec.SeqPtTrigger &1;
	ConfigRec.OscopeRec.pc104_debug = m_n104Debug; // default to off

	gChannel %= MAX_CHANNEL;
	gGate &= 3;
	//gShoe &= 3;
	if (gShoe < 0) gShoe = 0;
	if (gShoe > 9) gShoe = 9;



	if ( ConfigRec.OscopeRec.NormalOrDiagnostic)
		{
		s.Format(_T("Diagnostic OSCOPE: Ch=%d"),gChannel +1);
		SetWindowText(s);
		}

	else
		{
		s.Format(_T(" OSCOPE: Ch=%d"),gChannel +1);
		SetWindowText(s);
		}


	m_CBTrc1.ResetContent( );

	for ( i = 0; i < OscopeTrace1.Entries; i++)
		{
		switch (OscopeTrace1.se[i].bSelect)
			{
		case T2_ALWAYS:
			s = OscopeTrace1.se[i].Txt;
			m_CBTrc1.AddString(s);
			break;

		case T2_DIAG:
			if ( ConfigRec.OscopeRec.NormalOrDiagnostic)
				{
				s = OscopeTrace1.se[i].Txt;
				m_CBTrc1.AddString(s);
				}
			break;
			}
		}

	//if ( ConfigRec.OscopeRec.NormalOrDiagnostic == 0)	// normal
		//ConfigRec.OscopeRec.trace1[gChannel] &= 1;

	//m_CBTrc1.SetCurSel(ConfigRec.OscopeRec.trace1[gChannel]);

	// Scope Trace II

	m_CBTrc2.ResetContent( );
	memset( (void *) &m_CmdTbl, 3, sizeof(m_CmdTbl));
	// default cmd index to gates 1&2

	j = k = 0;		// index of selection stored in config file

	// Refill the combo box based on buttons set on dialog
	// Save scope command index associated with dialog box indx in cmdtbl

	for ( i = 0; i < OscopeTrace2.Entries; i++)
		{
		switch (OscopeTrace2.se[i].bSelect)
			{
		case T2_ALWAYS:
			s = OscopeTrace2.se[i].Txt;
			m_CBTrc2.AddString(s);
			//if (ConfigRec.OscopeRec.trace2[gChannel] ==
				//OscopeTrace2.se[i].bIndx)  j = k;
			m_CmdTbl[k++] = OscopeTrace2.se[i].bIndx;	// remember cmd
			break;

		case T2_DIAG:
			if ( ConfigRec.OscopeRec.NormalOrDiagnostic)
				{
				s =  OscopeTrace2.se[i].Txt;
				m_CBTrc2.AddString(s);
				//if (ConfigRec.OscopeRec.trace2[gChannel] ==
					//OscopeTrace2.se[i].bIndx)  j = k;
				m_CmdTbl[k++] = OscopeTrace2.se[i].bIndx;	// remember cmd
				}
			break;

		case T2_GATES+T2_DIAG:
			if ((ConfigRec.OscopeRec.NormalOrDiagnostic) &&
				(m_nT2gatesync == 0) )
				{
				s = OscopeTrace2.se[i].Txt;
				m_CBTrc2.AddString(s);
				//if (ConfigRec.OscopeRec.trace2[gChannel] ==
					//OscopeTrace2.se[i].bIndx)  j = k;
				m_CmdTbl[k++] = OscopeTrace2.se[i].bIndx;	// remember cmd
				}
			break;

		default:
			// i increments here but k does not
			// i counts all possible entries, k counts ones actaully added

			break;
			}
		}


//		i = ConfigRec.OscopeRec.trace2[gChannel];
	
	if ( j == 0)	// no valid cmd found
		{
		// default to gates 1&2
		for ( j = 0; j < OscopeTrace2.Entries; j++)
			{
			if ( m_CmdTbl[j] == 3)
				break;	// found it
			}
		}
	//m_nT2CmdIndx = ConfigRec.OscopeRec.trace2[gChannel] = m_CmdTbl[j];
	m_CBTrc2.SetCurSel(j);

	// Scope Trigger
	m_CBTrgr.ResetContent( );
	for ( i = 0; i < OscopeTrigger.Entries; i++)
		{
		s = OscopeTrigger.se[i].Txt;
		m_CBTrgr.AddString (s);
		}


	//m_CBTrgr.SetCurSel(ConfigRec.OscopeRec.trigger_index[gChannel]);

			
		// Set sequence trigger point to config file value

		// set scroll bar for reject level... affect only scope output
		m_SBLvl.SetScrollRange(0, 100, TRUE);
		m_SBLvl.SetScrollPos(ConfigRec.OscopeRec.reject_pcnt,TRUE);
		if (ConfigRec.OscopeRec.reject_pcnt > 70) ConfigRec.OscopeRec.reject_pcnt=70;
		if (ConfigRec.OscopeRec.reject_pcnt < 0) ConfigRec.OscopeRec.reject_pcnt=0;

		m_SLvl.Format(_T("%3d %%"), ConfigRec.OscopeRec.reject_pcnt);
		UpdateData(FALSE);	// Copy variables TO screen 
		SendMsg(SCOPE_TRACE1);
		SendMsg(SCOPE_TRACE2);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
		SendMsg(DEBUG_CONTROL);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

BOOL COscpe::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Tscandlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}
void COscpe::OnCancel() 
	{
	// TODO: Add extra cleanup here
	
	// Need this to get to PostNcDestroy which nulls ptr to the routine
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void COscpe::OnOK() 
	{
	// TODO: Add extra validation here
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition(_T("OSCILLOSCOPE"), &wp);

	SendMsg(SCOPE_TRACE1);
	SendMsg(SCOPE_TRACE2);
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
	
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

COscpe::~COscpe()
	{
	delete m_pUndo;
	}

void COscpe::OnSelchangeCBTrc1() 
	{
	// TODO: Add your control notification handler code here
	int i = m_CBTrc1.GetCurSel();
	if (CRcvr::m_pDlg) {
		if (i==0) CRcvr::m_pDlg->OnRdFW() ;
		if (i==1) CRcvr::m_pDlg->OnRdRF() ;
	}
	//ConfigRec.OscopeRec.trace1[gChannel] = i;
	UpdateData(FALSE);		// Copy variables TO screen 
	SendMsg(SCOPE_TRACE1);
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
	}



void COscpe::OnSelchangeCBTrc2() 
	{
	// TODO: Add your control notification handler code here
	// Examine text returned by selection to determine command index

	int i;
	CString s;
	i = m_CBTrc2.GetCurSel();
//	m_CBTrc2.GetLBText(i, s);
	// Determine which text string was selected and hence which
	// command index
#if 0
	j = 0;
	for ( i = 0; i < OscopeTrace2.Entries; i++)
		{
		if ( s == OscopeTrace2.se[i].Txt)
			{
			j = OscopeTrace2.se[i].bIndx;
			break;
			}
		}
#endif	
	//ConfigRec.OscopeRec.trace2[gChannel] = m_nT2CmdIndx = m_CmdTbl[i];
	UpdateData(FALSE);		// Copy variables TO screen 
	SendMsg(SCOPE_TRACE2);
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
	}

void COscpe::OnSelchangeCBTrgr()
	{
	// TODO: Add your control notification handler code here
	int i = m_CBTrgr.GetCurSel();
	//ConfigRec.OscopeRec.trigger_index[gChannel] = i;
	ConfigRec.OscopeRec.trigger[gChannel] = i;	// set lower nibble
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
	}



void COscpe::OnRbT2gate() 
	{
	// TODO: Add your control notification handler code here
	// Trace 2 is displaying from inst gates... different selections 
	// for combo box.
	UpdateData(TRUE);	// Copy screen TO variables 
	//ConfigRec.OscopeRec.T2GateOrSync[gChannel] = m_nT2gatesync;
	OnInitDialog();
	}

void COscpe::OnRbT2sync() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables 
	//ConfigRec.OscopeRec.T2GateOrSync[gChannel] = m_nT2gatesync;
	OnInitDialog();
	}

void COscpe::OnRbSeq1() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables 
	ConfigRec.OscopeRec.SeqPtTrigger = 0;
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
	// Write to scope demux
	}

void COscpe::OnRbSeq2() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables 
	if (ConfigRec.UtRec.sequence_length == 2)
		{	// 2 sequence pt defined for pulse/listen
		ConfigRec.OscopeRec.SeqPtTrigger = 1;
		}
	else
		{	// only one seq pt, this selection not allowed
		ConfigRec.OscopeRec.SeqPtTrigger = 0;
		}
	m_nSeqPt = ConfigRec.OscopeRec.SeqPtTrigger;
	UpdateData(FALSE);	// Copy variables TO screen  
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
	// Write to scope demux
	}

void COscpe::OnOscopeUndo() 
	{
	// TODO: Add your control notification handler code here
	if (MessageBox(	_T("UnDo all changes since last SAVE operation?"),
			_T("Restore Configuration"), MB_YESNO)== IDYES)
		{
		memcpy ( (void *) &ConfigRec.OscopeRec, (void *) m_pUndo, sizeof(OSCOPE_REC));
		SendMsg(SCOPE_TRACE1);
		SendMsg(SCOPE_TRACE2);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
		}
	OnInitDialog();
	}

void COscpe::On104Debug() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables 
	ConfigRec.OscopeRec.pc104_debug = m_n104Debug;
	SendMsg(DEBUG_CONTROL);
	}

void COscpe::OnOscopeHelp() 
	{
	// TODO: Add your control notification handler code here
	CString s = gszHelpFile;
	::WinHelp(m_hWnd, s,	HELP_INDEX,0);
	
	}

