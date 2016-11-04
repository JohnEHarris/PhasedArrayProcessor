// GateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
//#include "GatesDlg.h"
// include only the parent dialog header asidlg.h for all child dialogs
#include "TscanDlg.h"
#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//CONFIG_REC * m_pUndo;
static short *nStart;
static char i,j;
static BOOL bGate;
/////////////////////////////////////////////////////////////////////////////
// CGateDlg dialog
char gate_txt[120], nChannel ;
	int button;
	CString s;
char *gate_trigger[] = {"Disable","Initial Pulse","Interface Echo","Slave A", "Slave B"};
char *gate_lam[] = {"Disable","Enable"};

CGateDlg::CGateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGateDlg)
	m_sBlanking = _T("0.0   us");
	m_sLevel = _T("   0  ");
	m_sRange = _T("0.0  us");
    m_bGate1Inv = FALSE;
	//}}AFX_DATA_INIT
	bGate=TRUE;

	m_pUndo = new CONFIG_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));

	if ( ( ConfigRec.UtRec.CompVelocity < 0.1) ||
		 ( ConfigRec.UtRec.CompVelocity > 0.6) ) 
		ConfigRec.UtRec.CompVelocity = 0.23f; // mild steel

	if ( ( ConfigRec.UtRec.ShearVelocity < 0.04) ||
		 ( ConfigRec.UtRec.ShearVelocity > 0.4) ) 
		 ConfigRec.UtRec.ShearVelocity = 0.13f; // mild steel

}

void Tprintf ( CString &s, float f)
	{
	/* convert the floating value to ascii and store in s.  Consider
		the flags governing engineering units display to transform
		the floating value f to the correct units of measure.
	*/

	float velocity;

		if ( (ConfigRec.bMotionTime & 0x80) == 0 )
		{
		s.Format(_T("%-7.1f us"), f);
		return;
		}

	if (ConfigRec.UtRec.Shoe[gChannel/10].Ch[gChannel%10].Type == IS_WALL)
		velocity = ConfigRec.UtRec.CompVelocity/2.0f;
	else
		velocity = ConfigRec.UtRec.ShearVelocity/2.0f;
	if ( ConfigRec.bEnglishMetric == ENGLISH)
		{
		f *= velocity;
		s.Format(_T("%-7.3f in"),f);
		return;
		}
	else
		{
		f = f * 25.4f * velocity;
		s.Format(_T("%-7.2f mm"),f);
		}
	}

void Tprintf ( char *s, float f)
	{
	/* convert the floating value to ascii and store in s.  Consider
		the flags governing engineering units display to transform
		the floating value f to the correct units of measure.
	*/

	float velocity;

		if ( (ConfigRec.bMotionTime & 0x80) == 0 )
		{
		sprintf(s,"%-7.1f us", f);
		return;
		}

	if (ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].Type != IS_WALL)
		velocity = ConfigRec.UtRec.CompVelocity/2.0f;
	else
		velocity = ConfigRec.UtRec.ShearVelocity/2.0f;
	if ( ConfigRec.bEnglishMetric == ENGLISH)
		{
		f *= velocity;
		sprintf(s,"%-7.3f in",f);
		return;
		}
	else
		{
		f = f * 25.4f * velocity;
		sprintf(s,"%-7.2f mm",f);
		}
	}


void CGateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGateDlg)
	DDX_Control(pDX, IDC_COMBOLam, m_CMlam);
	DDX_Control(pDX, IDC_COMBOTrigger, m_CMTrigger);
	DDX_Control(pDX, IDC_GATES_BLANKING_SB, m_sbGATEblanking);
	DDX_Control(pDX, IDC_GATES_RANGE_SB, m_sbGATErange);
	DDX_Control(pDX, IDC_GATES_LEVEL_SB, m_sbGATElevel);
	DDX_Control(pDX, IDC_GATES_DELAY_SB, m_sbGATEdelay);
	DDX_Text(pDX, IDC_delay, m_sDelay);
	DDV_MaxChars(pDX, m_sDelay, 12);
	DDX_Text(pDX, IDC_blanking, m_sBlanking);
	DDV_MaxChars(pDX, m_sBlanking, 12);
	DDX_Text(pDX, IDC_level, m_sLevel);
	DDV_MaxChars(pDX, m_sLevel, 12);
	DDX_Text(pDX, IDC_range, m_sRange);
	DDV_MaxChars(pDX, m_sRange, 12);
    DDX_Check(pDX, IDC_CHECK1, m_bGate1Inv);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGateDlg, CDialog)
	//{{AFX_MSG_MAP(CGateDlg)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(ID_GATES_UNDO, OnGatesUndo)
	ON_BN_CLICKED(IDC_gate1, Ongate1)
	ON_BN_CLICKED(IDC_gate2, Ongate2)
	ON_CBN_SELCHANGE(IDC_COMBOTrigger, OnSelchangeCOMBOTrigger)
    ON_BN_CLICKED(IDC_CHECK1, OnG1Invrt)
	ON_CBN_SELCHANGE(IDC_COMBOLam, OnSelchangeCOMBOLam)
	ON_BN_CLICKED(IDC_gate3, Ongate3)
	ON_BN_CLICKED(IDC_gate4, Ongate4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGateDlg message handlers

void CGateDlg::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}

BOOL CGateDlg::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	WINDOWPLACEMENT wp;
	RECT rect;
	CString s;
	int dx, dy;		// width and height of original window
	if ( bGate )
	{
		bGate = FALSE;
		GetWindowPlacement(&wp);
		dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		pCTscanDlg->GetWindowLastPosition(_T("GATES"), &rect);
		if ( ( (rect.right - rect.left) >= dx ) &&
			 ( (rect.bottom - rect.top) >= dy ))
			{
			wp.rcNormalPosition = rect;
			SetWindowPlacement(&wp);
			}
	}
	
	// TODO: Add extra initialization here
	gChannel %= MAX_CHANNEL;
	gGate &= 3;
	//gShoe &= 10;
	if (gShoe < 0) gShoe = 0;
	if (gShoe > 9) gShoe = 9;
	switch (gGate)
	{
	case 0:
	default:
		button = IDC_gate1;
		break;
	case 1:
		button = IDC_gate2;
		break;
	case 2:
		button = IDC_gate3;
		break;
	case 3:
		button = IDC_gate4;
		break;
/*	default:
		button = IDC_gate1;
		break;
*/	}

	CheckRadioButton( IDC_gate1, IDC_gate4, button );
	nChannel = gChannel % 10;
    s.Format(_T("   ch=%d   Gates   g=%d"),gChannel +1,gGate+1);
	SetWindowText(s);
	ConfigRec.gates.g1_invrt[gChannel] &= 1;	// limit to 0/1
	if (ConfigRec.gates.g1_invrt[gChannel] ==1) m_bGate1Inv = TRUE;
	else m_bGate1Inv = FALSE;
	
	if (ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].Type != IS_WALL)
		ConfigRec.gates.triger[gChannel] = 0;
	else 
		ConfigRec.gates.triger[gChannel] = 1;

	m_sbGATEdelay.SetScrollRange(DELAYMIN,DELAYMAX,TRUE);
	m_sbGATEdelay.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbGATElevel.SetScrollRange(LEVEL1MIN,LEVEL1MAX,TRUE);
	m_sbGATElevel.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbGATErange.SetScrollRange(RANGEMIN,RANGEMAX,TRUE);
	m_sbGATErange.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbGATEblanking.SetScrollRange(BLANKMIN,BLANKMAX,TRUE);
	m_sbGATEblanking.EnableScrollBar(ESB_ENABLE_BOTH);
//	if (bInit) {
		nStart = &ConfigRec.gates.delay[gChannel][gGate];
		m_sbGATEdelay.SetScrollPos(*nStart, TRUE);
		//TRACE ("gChannel = %d\n", gChannel);
		//TRACE ("gGate = %d\n", gGate);
		//TRACE ("GateDlg: OnInitDlg: Gate Delay = %d\n", *nStart);
		Tprintf( gate_txt, *nStart/10.0f);
		m_sDelay = gate_txt;
		nStart = &ConfigRec.gates.level[gChannel][gGate];
		m_sbGATElevel.SetScrollPos(*nStart, TRUE);

		m_sLevel.Format (_T("   %2d  %%"), *nStart);

		nStart = &ConfigRec.gates.range[gChannel][gGate];
		Tprintf( gate_txt, *nStart/10.0f);
		m_sRange = gate_txt;

		m_sbGATErange.SetScrollPos(*nStart, TRUE);

		nStart = &ConfigRec.gates.blank[gChannel][gGate];
		Tprintf( gate_txt, *nStart/10.0f);
		m_sBlanking = gate_txt;
		m_sbGATEblanking.SetScrollPos(*nStart, TRUE);
		m_CMTrigger.ResetContent( );
		for ( i=0; i< 5;i++)
			{
			s = gate_trigger[i];
			m_CMTrigger.AddString(s);
			}
		m_CMlam.ResetContent( );
		for (i=0; i< 2;i++)
			{
			s = gate_lam[i];
			m_CMlam.AddString(s);
			}

		m_CMTrigger.SetCurSel( ConfigRec.gates.trg_option[gChannel][gGate] );
		m_CMlam.SetCurSel( ConfigRec.gates.triger[gChannel] );
		UpdateData(FALSE);
//	}
//	bInit=TRUE;

	SetDefID(-1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CGateDlg::OnCancel() 
	{
	// TODO: Add extra cleanup here
	
	// Need this to get to PostNcDestroy which nulls ptr to the routine
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CGateDlg::OnOK() 
	{
	// TODO: Add extra validation here
	// Need this to get to PostNcDestroy which nulls ptr to the routine

	Save_Pos();
	SendMsg(GATE_SELECT);
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

void CGateDlg::KillMe()
	{
	// Public access to OnCancel
	// Add to every modeless dialog
	OnCancel();
	}

void CGateDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	// TODO: Add your message handler code here and/or call default
	

	// CODE IS DIRECTION OF SCROLL MOTION
	int nDelta;		// where the bar started and amount of change
	int nMax = 10;
	int nMin = 0;
	int nCtlId;
	int dummy = 5;
	int nStart;
	CString s;

//	float fStart;			// floating scroll bar start point

	nCtlId = pScrollBar->GetDlgCtrlID();

	switch (nCtlId)
		{
	case IDC_GATES_DELAY_SB:
		nMin=DELAYMIN;
		nMax=DELAYMAX;
		break;

	case IDC_GATES_LEVEL_SB:
		nMin=LEVEL1MIN;
		nMax=LEVEL1MAX;
		break;

	case IDC_GATES_RANGE_SB:
		nMin=RANGEMIN;
		nMax=RANGEMAX;
		break;

	case IDC_GATES_BLANKING_SB:
		nMin=BLANKMIN;
		nMax=BLANKMAX;
		break;

#if 0
	case IDC_SB_WALL_MAX:
		nStart = &m_nWallMax;
		break;

	case IDC_SB_LID:
		nStart = &m_nLid;
		break;

	case IDC_SB_LOD:
		nStart = &m_nLod;
		break;

	case IDC_SB_TID:
		nStart = &m_nTid;
		break;

	case IDC_SB_TOD:
		nStart = &m_nTod;
		break;

		//oblq1
	case IDC_SB_Q1ID:
		nStart = &m_nQ1id;
		break;

	case IDC_SB_Q1OD:
		nStart = &m_nQ1od;
		break;


		// oblq2
	case IDC_SB_Q2ID:
		nStart = &m_nQ2id;
		break;

	case IDC_SB_Q2OD:
		nStart = &m_nQ2od;
		break;

		
		// oblq3
	case IDC_SB_Q3ID:
		nStart = &m_nQ3id;
		break;

	case IDC_SB_Q3OD:
		nStart = &m_nQ3od;
		break;

	default:
		nStart = (short*)&dummy;
		break;
#endif	

		}

	nStart = pScrollBar->GetScrollPos();
//	pScrollBar->GetScrollRange(&nMin, &nMax);

	switch ( nSBCode)
		{	// switch on type of motion
	case SB_LINELEFT:
		nDelta = -1;
		break;

	case SB_LINERIGHT:
		nDelta = 1;
		break;

	case SB_PAGELEFT:
		nDelta = -10;	//-(nMax - nMin)/10;
		break;

	case SB_PAGERIGHT:
		nDelta = 10;	//(nMax - nMin)/10;
		break;
#if 1
	case SB_THUMBTRACK:
		nDelta = (int)nPos;
		nStart = 0;
		break;
#endif

	default:
		nDelta = 0;		//need a value 8-27-2001
		return;
		break;

		}	// switch on type of motion

	nStart += nDelta;
	if ( nStart > nMax) nStart = nMax;
	if ( nStart < nMin) nStart = nMin;


	pScrollBar->SetScrollPos(nStart, TRUE);

//	m_sDelay.Format ("%.3f   in", *nStart/1000.0);

//	UpdateData(TRUE);

	switch (nCtlId)
		{
	case IDC_GATES_DELAY_SB:
		// Send gate delay to instrument
		Tprintf( gate_txt, nStart/10.0f);
		m_sDelay = gate_txt;
		ConfigRec.gates.delay[gChannel][gGate] = nStart;
		TRACE ("nStart = %d\n", nStart);
		//	delay = 16383 - ConfigRec.gates.delay[gChannel][gGate];

		SendMsg(GATES_DELAY);			// Set gate delay

		if (ConfigRec.receiver.tcg_trigger[gChannel] != 0)
			if (pCTscanDlg->m_pTcgSetupDlg)
				(pCTscanDlg->m_pTcgSetupDlg)->UpdateDlg();

		break;
	case IDC_GATES_LEVEL_SB:
		// Send gate delay to instrument
		m_sLevel.Format (_T("   %2d  %%"), nStart);
		ConfigRec.gates.level[gChannel][gGate] = nStart;
		SendMsg(GATES_LEVEL);			// Set gate delay
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);			// Set gate delay
		TRACE("Level = %d\n",nStart);
		break;
	case IDC_GATES_RANGE_SB:
		// Send gate delay to instrument
		Tprintf( gate_txt, nStart/10.0f);
		m_sRange = gate_txt;
		ConfigRec.gates.range[gChannel][gGate] = nStart;
		SendMsg(GATES_RANGE);			// Set gate delay

		if (ConfigRec.receiver.tcg_trigger[gChannel] != 0)
			if (pCTscanDlg->m_pTcgSetupDlg)
				(pCTscanDlg->m_pTcgSetupDlg)->UpdateDlg();

		TRACE("Range = %d\n",nStart);
		break;
	case IDC_GATES_BLANKING_SB:
		// Send gate delay to instrument
		Tprintf( gate_txt, nStart/10.0f);
		m_sBlanking = gate_txt;
	ConfigRec.gates.blank[gChannel][gGate] = nStart;
	SendMsg(GATES_BLANK);			// Set gate delay

		break;

		// all thresholds in per cent
#if 0
	case IDC_SB_WALL_MIN:
	case IDC_SB_WALL_MAX:
		CONFIG_REC.UtRec.TholdWall[0] = (char) (m_nWallMin & 0xff);
		CONFIG_REC.UtRec.TholdWall[1] = m_nWallMax;
		DrawThold(0);	// wall
		break;

	case IDC_SB_LID:
	case IDC_SB_LOD:
		CONFIG_REC.UtRec.TholdLong[0] = m_nLid;
		CONFIG_REC.UtRec.TholdLong[1] = m_nLod;
		DrawThold(1);	//long
		break;

	case IDC_SB_TID:
	case IDC_SB_TOD:
		CONFIG_REC.UtRec.TholdTran[0] = m_nTid;
		CONFIG_REC.UtRec.TholdTran[1] = m_nTod;
		DrawThold(2);
		break;
		
		// oblq1
	case IDC_SB_Q1ID:
	case IDC_SB_Q1OD:
		CONFIG_REC.UtRec.TholdOblq1[0] = m_nQ1id;
		CONFIG_REC.UtRec.TholdOblq1[1] = m_nQ1od;
		DrawThold(3);	
		break;

		// oblq2
	case IDC_SB_Q2ID:
	case IDC_SB_Q2OD:
		CONFIG_REC.UtRec.TholdOblq2[0] = m_nQ2id;
		CONFIG_REC.UtRec.TholdOblq2[1] = m_nQ2od;
		DrawThold(4);	
		break;

		// oblq3
	case IDC_SB_Q3ID:
	case IDC_SB_Q3OD:
		CONFIG_REC.UtRec.TholdOblq3[0] = m_nQ3id;
		CONFIG_REC.UtRec.TholdOblq3[1] = m_nQ3od;
		DrawThold(5);	
		break;

#endif
	default:
		break;
		}
	UpdateData(FALSE);
		
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

BOOL CGateDlg::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Tscandlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CGateDlg::OnGatesUndo() 
	{
	// TODO: Add your control notification handler code here
	if (MessageBox(	_T("Undo all gate changes since last SAVE operation?"),
			_T("Restore Configuration"),MB_YESNO)== IDYES)
		{
		//memcpy ( (void *) &ConfigRec, (void *) m_pUndo, sizeof(CONFIG_REC));
		UndoGateSettings();
		}
	OnInitDialog();
	}


void CGateDlg::UndoGateSettings()
{
	int iChannel, iGate;
	int ic, ig;
	iChannel = gChannel;
	iGate = gGate;
	for (ic=0; ic<MAX_CHANNEL; ic++)  //channel loop
	{
		gChannel = ic;
		SendMsg(CHANNEL_SELECT);
		for (ig=0; ig<MAX_GATE; ig++)  //gate loop
		{
			gGate = ig;
			pCTscanDlg->SendMsg(GATE_SELECT);

			ConfigRec.gates.delay[ic][ig] = m_pUndo->gates.delay[ic][ig] ;
			SendMsg(GATES_DELAY);

			ConfigRec.gates.level[ic][ig] = m_pUndo->gates.level[ic][ig] ;
			SendMsg(GATES_LEVEL);

			ConfigRec.gates.range[ic][ig] = m_pUndo->gates.range[ic][ig] ;
			SendMsg(GATES_RANGE);

			ConfigRec.gates.blank[ic][ig] = m_pUndo->gates.blank[ic][ig] ;
			SendMsg(GATES_BLANK);

			ConfigRec.gates.trg_option[ic][ig] = m_pUndo->gates.trg_option[ic][ig] ;
			SendMsg(GATES_TRIGGER);
		}
	}

	gChannel = iChannel;
	SendMsg(CHANNEL_SELECT);

	gGate = iGate;
	SendMsg(GATE_SELECT);
}

CGateDlg::~CGateDlg()
	{

	delete m_pUndo;
	}

void CGateDlg::Ongate1() 
	{
	// TODO: Add your control notification handler code here
	CheckRadioButton( IDC_gate1, IDC_gate4, IDC_gate1 );
	gGate=0;
	SendMsg(GATE_SELECT);
	pCTscanDlg->UpdateChnlSelected();

	if (pCTscanDlg->m_pAscanDlg)
		(pCTscanDlg->m_pAscanDlg)->SetRfFullWaveMode();
	}

void CGateDlg::Ongate2() 
	{
	// TODO: Add your control notification handler code here
	CheckRadioButton( IDC_gate1, IDC_gate4, IDC_gate2 );
	gGate=1;
//	::CheckRadioButton(hTscanDlg, IDC_RADIO1, IDC_RADIO2, IDC_RADIO2 );	
	SendMsg(GATE_SELECT);
	pCTscanDlg->UpdateChnlSelected();

	if (pCTscanDlg->m_pAscanDlg)
		(pCTscanDlg->m_pAscanDlg)->SetRfFullWaveMode();
	}

void CGateDlg::Ongate3() 
{
	CheckRadioButton( IDC_gate1, IDC_gate4, IDC_gate3 );
	gGate=2;
//	::CheckRadioButton(hTscanDlg, IDC_RADIO1, IDC_RADIO2, IDC_RADIO2 );	
	SendMsg(GATE_SELECT);
	pCTscanDlg->UpdateChnlSelected();

	if (pCTscanDlg->m_pAscanDlg)
		(pCTscanDlg->m_pAscanDlg)->SetRfFullWaveMode();
}

void CGateDlg::Ongate4() 
{
	// TODO: Add your control notification handler code here
	CheckRadioButton( IDC_gate1, IDC_gate4, IDC_gate4 );
	gGate=3;
//	::CheckRadioButton(hTscanDlg, IDC_RADIO1, IDC_RADIO2, IDC_RADIO2 );	
	SendMsg(GATE_SELECT);
	pCTscanDlg->UpdateChnlSelected();

	if (pCTscanDlg->m_pAscanDlg)
		(pCTscanDlg->m_pAscanDlg)->SetRfFullWaveMode();
}

void CGateDlg::OnSelchangeCOMBOTrigger() 
	{
	// TODO: Add your control notification handler code here
	SendMsg(GATES_TRIGGER);
	ConfigRec.gates.trg_option[gChannel][gGate] = m_CMTrigger.GetCurSel( );
	SendMsg(GATES_TRIGGER);

	}

void CGateDlg::OnG1Invrt() 
	{
	// TODO: Add your control notification handler code here
	m_bGate1Inv ^= 1;
	ConfigRec.gates.g1_invrt[gChannel] = m_bGate1Inv;
	}

void CGateDlg::OnSelchangeCOMBOLam() 
	{
	// TODO: Add your control notification handler code here
//	ConfigRec.gates.triger[gChannel] =	m_CMlam.GetCurSel ();
	SendMsg(GATES_LAMIN);
	ConfigRec.gates.triger[gChannel] =	m_CMlam.GetCurSel ();
	SendMsg(GATES_LAMIN);
	
	}
void CGateDlg::Save_Pos()
	{
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition(_T("GATES"), &wp);
//	bCtof = FALSE;

	}



void CGateDlg::DrawThold(int nGraph)
	{
	if (pCTscanDlg->m_pInspectDlg1)
		(pCTscanDlg->m_pInspectDlg1)->DrawThold(nGraph);

	}
