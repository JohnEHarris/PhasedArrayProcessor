// TCG.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
// include only the parent dialog header asidlg.h for all child dialogs
#include "TscanDlg.h"
#include "Extern.h"
#include <math.h>
#include "sbctbl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTCG dialog
bool bTCGSet;
extern char *tcg_fn[],*tcg_trigger[],*tcg_step[],*st_sysOff,*st_sysOn,*st_chnOn,*st_chnOff;
CTCG::CTCG(CWnd* pParent /*=NULL*/)
	: CDialog(CTCG::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CTCG)
	m_SAArg = _T("");
	m_SBArg = _T("");
	m_SGdb = _T("");
	m_SGint = _T("");

	//}}AFX_DATA_INIT
	bTCGSet = TRUE;
	m_pUndo = new CONFIG_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
	}


void CTCG::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTCG)
	DDX_Control(pDX, IDC_SBB, m_SBB);
	DDX_Control(pDX, IDC_SBA, m_SBA);
	DDX_Control(pDX, IDC_CB_Trgr, m_CBTrgr);
	DDX_Control(pDX, IDC_CB_Step, m_CBStep);
	DDX_Control(pDX, IDC_CB_TCGGain, m_CBTGain);
	DDX_Text(pDX, IDC_EDAarg, m_SAArg);
	DDV_MaxChars(pDX, m_SAArg, 12);
	DDX_Text(pDX, IDC_EDBArg, m_SBArg);
	DDV_MaxChars(pDX, m_SBArg, 12);
	DDX_Text(pDX, IDC_EDGdb, m_SGdb);
	DDV_MaxChars(pDX, m_SGdb, 12);
	DDX_Text(pDX, IDC_EDGint, m_SGint);
	DDV_MaxChars(pDX, m_SGint, 12);
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CTCG, CDialog)
	//{{AFX_MSG_MAP(CTCG)
	ON_WM_HSCROLL()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDChOn, OnChOn)
	ON_BN_CLICKED(IDCpte, OnCpte)
	ON_BN_CLICKED(IDSysOn, OnSysOn)
	ON_CBN_SELCHANGE(IDC_CB_Step, OnSelchangeCBStep)
	ON_CBN_SELCHANGE(IDC_CB_TCGGain, OnSelchangeCBTCGGain)
	ON_CBN_SELCHANGE(IDC_CB_Trgr, OnSelchangeCBTrgr)
	ON_BN_CLICKED(IDC_CkTCG, OnCkTCG)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTCG message handlers
#define		ARG1_MIN		-15000
#define		ARG1_MAX		 15000

#define		ARG2_MIN		-15000
#define		ARG2_MAX		 15000

#define		PD1				500
#define		PD2				500
SB_CONTROL_TBL_ENTRY tcg_sbt[] =	
{
	// SB ID	variable ptr		max		min pu  	pd  lu  ld  EN ID
	{ (HWND) IDC_SBA,		NULL,	ARG1_MAX, ARG1_MIN,	-PD1, PD1, -1, 1, (HWND) IDC_EDAarg},
	{ (HWND) IDC_SBB,		NULL,	ARG2_MAX, ARG2_MIN,	-PD2, PD2, -1, 1, (HWND) IDC_EDBArg},
	{      0       ,	NULL                ,	30,			0,	-5,		5,	-1,	1}
};
float farg1,farg2;
static float farg1_div, farg2_div;
//extern float gaindb;		// receiver gain in 1/10th db
float gaindb;
char ti;
void CTCG::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;

}

void CTCG::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CString s;
	int *ptrval;	// pointer to variable being scrolled
/*	*nStart = pScrollBar->GetScrollPos();
	pScrollBar->GetScrollRange(&nMin, &nMax);
*/
	// Adjusting the gain turns off the TCG
	int itbl,nCtrlID;		// table index of scroll bars

	nCtrlID = pScrollBar->GetDlgCtrlID();
	switch (nCtrlID)
	{
	case IDC_SBA:
		itbl = 0;
		break;
	case IDC_SBB:
		itbl = 1;
		break;
		default:
		return;

	}

	//ConfigRec.receiver.tcg_enable[gChannel] = 0;
		ptrval = tcg_sbt[itbl].var;

	switch ( nSBCode)
		{	// switch on type of motion
		case SB_PAGEDOWN :
			*ptrval += tcg_sbt[itbl].pddelta;
			break;
		case SB_LINEDOWN :
			*ptrval += tcg_sbt[itbl].lddelta;
			break;
		case SB_PAGEUP :
			*ptrval += tcg_sbt[itbl].pudelta;
			break;
		case SB_LINEUP :
			*ptrval += tcg_sbt[itbl].ludelta;
			break;
		case SB_TOP:
			*ptrval = tcg_sbt[itbl].min;
			break;
		case SB_BOTTOM:
			*ptrval = tcg_sbt[itbl].max;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			*ptrval = (int)nPos;
			break;
		default:
		return;
		break;

		}	// switch on type of motion

		if (*ptrval > tcg_sbt[itbl].max) *ptrval = tcg_sbt[itbl].max;
		if (*ptrval < tcg_sbt[itbl].min) *ptrval = tcg_sbt[itbl].min;

		pScrollBar->SetScrollPos(*ptrval, TRUE);
		farg1 = (float) *ptrval;
		if ( itbl == 0) 	s.Format(_T("%6.3f"), farg1/farg1_div);
		else 	s.Format(_T("%6.3f"), farg1/farg2_div);
//		SetDlgItemText(tcg_sbt[itbl].en,(LPSTR)tBuf);
		SetDlgItemText(IDC_SBA+itbl,s);
		SendMsg(TCG_ARG1);		// send tcg arg 1
		SendMsg(TCG_ARG2);		// send tcg arg 2
	pCTscanDlg->UpdateChnlSelected();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CTCG::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	// TODO: Add your message handler code here
	if ( !bTCGSet) {
		WINDOWPLACEMENT wp;
		// Save closing location of window
		GetWindowPlacement(&wp);
		pCTscanDlg->SaveWindowLastPosition(_T("TCG SET"), &wp);
	}
}

BOOL CTCG::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString s;
	short tmp;
	// TODO: Add extra initialization here
	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	int  i, gate_len;

	float gain_int;			// integer gain as opposed to db
 	if ( bTCGSet )
	{
		bTCGSet = FALSE;
		GetWindowPlacement(&wp);
		dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		pCTscanDlg->GetWindowLastPosition(_T("TCG SET"), &rect);
		if ( ( (rect.right - rect.left) >= dx ) &&
			 ( (rect.bottom - rect.top) >= dy ))
			{
			wp.rcNormalPosition = rect;
			SetWindowPlacement(&wp);
			}
	}
	gChannel %= MAX_CHANNEL;
	gGate &= 1;
	//gShoe &= 10;
	if (gShoe < 0) gShoe = 0;
	if (gShoe > 9) gShoe = 9;
	s.Format(_T("   ch=%d  TCG SET"),gChannel +1);
	SetWindowText(s);

		//tcg_sbt[0].var = (int *) &ConfigRec.receiver.arg1[gChannel];
		//tcg_sbt[1].var = (int *) &ConfigRec.receiver.arg2[gChannel];

		// Init FUNCTION drop down combo box
		m_CBTGain.ResetContent( );
			for(tmp=0; tmp < 4; tmp++)
				{
				s = tcg_fn[tmp];
				m_CBTGain.AddString(s);
				}

		// put the cursor on the last one selected
		//ConfigRec.receiver.tcg_function[gChannel] &= 3;
		//m_CBTGain.SetCurSel(ConfigRec.receiver.tcg_function[gChannel]);

		// Based on the gain function selected, scroll bars have 
		// different ranges
/*
		switch (ConfigRec.receiver.tcg_function[gChannel])
			{
		case 0:
			// parabolic at + btt
			tcg_sbt[0].max = 1000;
			tcg_sbt[0].min = -1000; 
			tcg_sbt[0].pddelta = 50;
			tcg_sbt[0].pudelta = -50;
			tcg_sbt[0].lddelta = 1;
			tcg_sbt[0].ludelta = -1;
			// B variables
			tcg_sbt[1].max = 2000;
			tcg_sbt[1].min = -2000; 
			tcg_sbt[1].pddelta = 50;
			tcg_sbt[1].pudelta = -50;
			tcg_sbt[1].lddelta = 1;
			tcg_sbt[1].ludelta = -1;
			farg1_div = 100.0f;
			farg2_div = 1000.0f;
			break;
			
		case 1:
			// step function for flaw, lower gain on 2nd gate
			tcg_sbt[0].max = 500;
			tcg_sbt[0].min = 0; 
			tcg_sbt[0].pddelta = 20;
			tcg_sbt[0].pudelta = -20;
			tcg_sbt[0].lddelta = 1;
			tcg_sbt[0].ludelta = -1;
			// B variables
			tcg_sbt[1].max = 250;
			tcg_sbt[1].min = -250;
			tcg_sbt[1].pddelta = 60;
			tcg_sbt[1].pudelta = -60;
			tcg_sbt[1].lddelta = 1;
			tcg_sbt[1].ludelta = -1;
			farg1_div = 1.0f;
			farg2_div = 10.0f;
			break;
			
		case 2:
			// g = B(1-exp(-at) )  "RC"
			tcg_sbt[0].max = 1000;
			tcg_sbt[0].min = 0;
			tcg_sbt[0].pddelta = 200;
			tcg_sbt[0].pudelta = -200;
			tcg_sbt[0].lddelta = 1;
			tcg_sbt[0].ludelta = -1;
			// B variables
			tcg_sbt[1].max = 250;
			tcg_sbt[1].min = -250;
			tcg_sbt[1].pddelta = 60;
			tcg_sbt[1].pudelta = -60;
			tcg_sbt[1].lddelta = 1;
			tcg_sbt[1].ludelta = -1;
			farg1_div = 1000.0f;
			farg2_div = 10.0f;
			break;
			
		case 3:
			// g = (B+1)/(1+Bexp(-at) )
			tcg_sbt[0].max = 1000;
			tcg_sbt[0].min = 0;
			tcg_sbt[0].pddelta = 200;
			tcg_sbt[0].pudelta = -200;
			tcg_sbt[0].lddelta = 1;
			tcg_sbt[0].ludelta = -1;
			// B variables
			tcg_sbt[1].max = 250;
			tcg_sbt[1].min = -250; 
			tcg_sbt[1].pddelta = 60;
			tcg_sbt[1].pudelta = -60;
			tcg_sbt[1].lddelta = 1;
			tcg_sbt[1].ludelta = -1;
			farg1_div = 1000.0f;
			farg2_div = 10.0f;
			break;
			
			}
*/
		for ( i = 0; i < 2; i++)
			{
			if ( *tcg_sbt[i].var > tcg_sbt[i].max)
			  *tcg_sbt[i].var = tcg_sbt[i].max;
				
			if ( *tcg_sbt[i].var < tcg_sbt[i].min)
			  *tcg_sbt[i].var = tcg_sbt[i].min;
			}
				
		// Arg1  scroll bar
		m_SBA.SetScrollRange(tcg_sbt[0].min,tcg_sbt[0].max,TRUE);
		m_SBA.SetScrollPos(*tcg_sbt[0].var,TRUE);

		// Display gain numeric value
		farg1 = (float) *tcg_sbt[0].var;
		m_SAArg.Format(_T("%6.3f"), farg1/farg1_div);
		
		// Arg2  scroll bar
		m_SBB.SetScrollRange(tcg_sbt[1].min,tcg_sbt[1].max,TRUE);
		m_SBB.SetScrollPos(*tcg_sbt[1].var,TRUE);
//		SetScrollRange(hCtl,SB_CTL,ARG2_MIN,ARG2_MAX,FALSE);
//		SetScrollPos(hCtl,SB_CTL,ConfigRec.receiver.arg2[gChannel],TRUE);

		// Display gain numeric value
		farg2 = (float) *tcg_sbt[1].var;
//		farg2 = (float) ConfigRec.receiver.arg2[gChannel];
		m_SBArg.Format(_T("%6.3f"), farg2/farg2_div);

		/* Display gain in db */
		// after 9/8/94 gain in 1/10th dbs
		gaindb = (float) ConfigRec.receiver.gain[gChannel];
		gaindb /= 10.0;		// 1/10th db
		m_SGdb.Format(_T("%-7.1f db"),gaindb);

		/* Display gain as int */
		// after 9/8/94 gain in 1/10th dbs
		gain_int = (float)pow(10.0, 
							(double)(ConfigRec.receiver.gain[gChannel])/200.0);
		m_SGint.Format(_T("%7.2f"),gain_int);

		// Init trigger drop down combo box
		m_CBTrgr.ResetContent( );		// Add gate duration for gate 1 and gate 2 to ascii text
		for ( tmp = 0; tmp < 7; tmp++)
			{	// 0=disable,1=IP,2=G1,3=G2,4=I/F,5=SlvA,6=SlvB
			CString s1= tcg_trigger[tmp];
			i = (ConfigRec.receiver.tcg_step[gChannel] & 3) + 1;	// 1-4
			i *= 2;	// i is step time in 0.1 us increments, 2,4,6,8
			switch (tmp)
				{
			default:
				break;

			case 2:
				// Gate 1 trigger
				// range in 0.1 us divided by step size in 0.1 us is 
				// length of gate in step units
				gate_len = ConfigRec.gates.range[gChannel][0] / i;
				s.Format(_T("[%d]"), gate_len);
				s1 += s;
				break;

			case 3:
				// Gate 2 trigger
				gate_len = ConfigRec.gates.range[gChannel][1] / i;
				s.Format(_T("[%d]"), gate_len);
				s1 += s;
				break;
				}
			m_CBTrgr.AddString(s1);
			}

		// High light current selection for tcg trigger
		ConfigRec.receiver.tcg_trigger[gChannel] &= 7;
		m_CBTrgr.SetCurSel(	ConfigRec.receiver.tcg_trigger[gChannel]);


		// Init step drop down combo box
		m_CBStep.ResetContent( );
		tmp = 0;
		while(tmp < 4)
			{
			s = tcg_step[tmp++];
			m_CBStep.AddString(s);
			}

		// High light current selection for tcg step
		ConfigRec.receiver.tcg_step[gChannel] &= 3;
		m_CBStep.SetCurSel(	ConfigRec.receiver.tcg_step[gChannel]);


		// Set button text for system tcg on/off
		ConfigRec.receiver.tcg_system &= 1;
		if (ConfigRec.receiver.tcg_system)
			{
			s = st_sysOn;
			}
		else
			{
			s = st_sysOff;
			}
		SetDlgItemText(IDSysOn,s);


		// Set button text for gChannel tcg on/off
		//ConfigRec.receiver.tcg_enable[gChannel] &= 1;
		//if (ConfigRec.receiver.tcg_enable[gChannel])
			//{
			//SetDlgItemText(IDChOn,(LPSTR)st_chnOn);
			//}
		//else
			//SetDlgItemText(IDChOn,(LPSTR)st_chnOff);

		// show if warning in scoreboard is on
		CheckDlgButton( IDC_CkTCG, ConfigRec.receiver.tcg_warn);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTCG::OnCancel()
	{
	CDialog::OnCancel();
	CDialog::DestroyWindow();

	}

void CTCG::OnOK() 
	{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
	CDialog::DestroyWindow();

	}

BOOL CTCG::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Tscandlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CTCG::OnChOn() 
	{
	// TODO: Add your control notification handler code here
	//ConfigRec.receiver.tcg_enable[gChannel] &= 1;
	//ConfigRec.receiver.tcg_enable[gChannel] ^= 1;
	pCTscanDlg->UpdateChnlSelected();
				
//			SendMessage(hDlg,WM_INITDIALOG,0,0L);
			SendMsg(TCG_ENABLE);

//			if (hDlgDLG_RECEIVER) PostMessage(hDlgDLG_RECEIVER,WM_INITDIALOG,0,0);
	
	}

void CTCG::OnCpte() 
	{
	// TODO: Add your control notification handler code here
	SendMsg(TCG_FN);			// define which function used
	SendMsg(RECEIVER_GAIN);	// set current gain
	SendMsg(TCG_ARG1);		// send tcg arg 1
	SendMsg(TCG_ARG2);		// send tcg arg 2
	SendMsg(TCG_COMPUTE);	// compute new tcg curve
	//ConfigRec.receiver.tcg_enable[gChannel] = 1;
	pCTscanDlg->UpdateChnlSelected();
	
	}

void CTCG::OnSysOn() 
	{
	// TODO: Add your control notification handler code here
			ConfigRec.receiver.tcg_system &= 1;
			ConfigRec.receiver.tcg_system ^= 1;

			//memset( &ConfigRec.receiver.tcg_enable[0],
						//ConfigRec.receiver.tcg_system, MAX_CHANNEL);
	pCTscanDlg->UpdateChnlSelected();
			SendMsg(TCG_SYSTEM);
/*			SendMessage(hDlg,WM_INITDIALOG,0,0L);

			if (hDlgDLG_RECEIVER) PostMessage(hDlgDLG_RECEIVER,WM_INITDIALOG,0,0);
*/	
}

void CTCG::OnSelchangeCBStep() 
{
	// TODO: Add your control notification handler code here
				SendMsg(TCG_STEP);
				ti = m_CBStep.GetCurSel ();
				ConfigRec.receiver.tcg_step[gChannel] = ti & 3;
				SendMsg(TCG_STEP);
	
}

void CTCG::OnSelchangeCBTCGGain() 
	{
	// TODO: Add your control notification handler code here
				SendMsg(TCG_FN);
				ti = m_CBTGain.GetCurSel ();
				//ConfigRec.receiver.tcg_function[gChannel] = ti & 3;
				SendMsg(TCG_FN);
//				PostMessage(hDlgDLG_TCGSET,WM_INITDIALOG,0,0);
	}

void CTCG::OnSelchangeCBTrgr() 
	{
	// TODO: Add your control notification handler code here
				SendMsg(TCG_TRIGGER);
				ti = m_CBTrgr.GetCurSel ();
				ConfigRec.receiver.tcg_trigger[gChannel] = ti & 7;
				SendMsg(TCG_TRIGGER);
	
	}

void CTCG::OnCkTCG() 
	{
	// TODO: Add your control notification handler code here
			ConfigRec.receiver.tcg_warn &= 1;
			ConfigRec.receiver.tcg_warn ^= 1;
			CheckDlgButton(IDC_CkTCG, ConfigRec.receiver.tcg_warn);
	
	}

void CTCG::KillMe()
	{
	// Public access to OnCancel
	// Add to every modeless dialog
	OnCancel();
	}

CTCG::~CTCG()
	{
	delete m_pUndo;
	}

