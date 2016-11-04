// DmxAdj.cpp : implementation file
//

// This file/class should be called DMXACAL for demux auto cal


/*
Date:	6/3/02
Purpose:	Step the operator thru the adjustment/setup of the scope demux board
			Save setup info in binary file named Scopedmx.cal
			Top window contains ouput from MMI and/or Master UDP.
			Bottom window is instructions on how to setup demux.
			Program advances as operator clicks on the NEXT button.
			Must operate for both full wave and rf modes.
Author:	JEH



*/
#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

// #include "DmxAdj.h"

#include "Extern.h"

extern char xchk ;		//= XMT_BITS;	// 0x22;
extern char rchk ;		//= RCV_BITS;	// 0x11;	

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDmxAdj dialog


CDmxAdj::CDmxAdj(CWnd* pParent /*=NULL*/)
	: CDialog(CDmxAdj::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CDmxAdj)
	m_nRf = -1;
	m_nChnl = 1;
	m_nDac = 0;
	m_szLabel = _T("");
	//}}AFX_DATA_INIT


	m_pUndo = new CONFIG_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));

	}

CDmxAdj::~CDmxAdj()
	{
	if ( m_pUndo) delete m_pUndo;
	}


void CDmxAdj::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDmxAdj)
	DDX_Control(pDX, IDC_EN_INSTRUCTIONS, m_ceInstruct);
	DDX_Control(pDX, IDC_EN_DMX_OUT, m_ceDmxOut);
	DDX_Control(pDX, IDC_SB_DMX_DAC, m_sbDac);
	DDX_Control(pDX, IDC_SB_CHNL, m_sbChnl);
	DDX_Radio(pDX, IDC_RB_RF, m_nRf);
	DDX_Text(pDX, IDC_EN_CHNL, m_nChnl);
	DDV_MinMaxInt(pDX, m_nChnl, 1, 40);
	DDX_Text(pDX, IDC_EN_DMX_DAC, m_nDac);
	DDV_MinMaxInt(pDX, m_nDac, 0, 4095);
	DDX_Text(pDX, IDC_ST_SB_SEL, m_szLabel);
	DDV_MaxChars(pDX, m_szLabel, 14);
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CDmxAdj, CDialog)
	//{{AFX_MSG_MAP(CDmxAdj)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_RB_RF, OnRbRf)
	ON_BN_CLICKED(IDC_RB_FW, OnRbFw)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BN_BACK, OnBnBack)
	ON_BN_CLICKED(IDC_BN_FINISH, OnBnFinish)
	ON_BN_CLICKED(IDC_BN_NEXT, OnBnNext)
	ON_BN_CLICKED(IDC_BN_OPTIONS, OnBnOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDmxAdj message handlers

void CDmxAdj::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}

void CDmxAdj::OnCancel() 
	{
	// TODO: Add extra cleanup here

	if ( m_uIpxTimer ) KillTimer(m_uIpxTimer);
	Save_Pos();
	DmxCalRec.nCmd = 6;	// kill scope dmx poll loop

	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
	
	/* cmd = 1, initial download of structure from mmi to master udp */
	/* cmd = 2, begin autocal operation */
	/* cmd = 3, adj in progress,  */
	/* cmd = 4, final upload of completed adjustment from udp */
	/* cmd = 5, compute linear equation for gate 1 & 2 coeff */
	/* cmd = 6, terminate autocal operation in udp */


	if ( m_pUndo)
		{
		memcpy( (void *) &ConfigRec, (void *) m_pUndo, sizeof (CONFIG_REC));
		pCTscanDlg->UpdateChnlSelected();
		}
	
//	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

BOOL CDmxAdj::OnInitDialog() 
	{
	
	CDialog::OnInitDialog();
	
	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	TCHAR BackFile[300], *pName;


	GetWindowPlacement(&wp);
	dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	pCTscanDlg->GetWindowLastPosition(_T("SCOPE_DMX_ADJ"), &rect);
	if ( ( (rect.right - rect.left) >= dx ) &&
		 ( (rect.bottom - rect.top) >= dy ))
		{
		wp.rcNormalPosition = rect;
		SetWindowPlacement(&wp);
		}


	// Download the setup structure
	DmxCalRec.nCmd = 1;		/* cmd = 1, initial download of structure from mmi to master udp */
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);


	// Create a backup file of the existing scopedmx.cal file
	_tcscpy(BackFile, gszScopeDmxCalFile);
	pName = _tcsstr(BackFile,_T("ScopeDmx."));
	pName[9] = 0;	// stop string after dmx.
	_tcscat(pName, _T("bak"));

	try
		{
		CFile file (BackFile, CFile::modeCreate | 
				CFile::modeReadWrite);	// create if not existing
																// else open existing
		file.Write(&DmxCalRec, sizeof(DmxCalRec));
		file.Close();
		}
	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}

//	LoadDmxCalRecord(&DmxCalMsg);	// from cal file into local msg struct
	m_pHSDmxVariable = NULL;
	// Always default to full wave setup
	m_nRf = DmxCalRec.nRfFw = 1;	// start in fw = 1
	m_nChnl = gChannel+1;
	m_nStateRf = m_nStateFw = 0;	// initial state of autocal steps
	// Next State Displays text and inputs data for the process
	m_nStateFw = NextState(m_nStateFw);	// move to next state value

	m_sbDac.SetScrollRange(0,4095,TRUE);
	m_sbDac.SetScrollPos(m_nDac);
	m_sbDac.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbChnl.SetScrollRange(1,40,TRUE);
	m_sbChnl.SetScrollPos(m_nChnl);
	m_sbChnl.EnableScrollBar(ESB_ENABLE_BOTH);

	UpdateData(FALSE);	// Copy variables to screen

	m_uIpxTimer = SetTimer(IDT_IPX_TIMER, 100, NULL);
	if (!m_uIpxTimer) MessageBox(_T("Failed to start IPX timer"));

	// Send init command to master udp to create scopedmxloop task
	DmxCalRec.nCmd = 2;
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
	
	/* cmd = 1, initial download of structure from mmi to master udp */
	/* cmd = 2, begin autocal operation */
	/* cmd = 3, adj in progress,  */
	/* cmd = 4, final upload of completed adjustment from udp */
	/* cmd = 5, compute linear equation for gate 1 & 2 coeff */
	/* cmd = 6, terminate autocal operation in udp */

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CDmxAdj::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	// TODO: Add your message handler code here and/or call default
	int nStart, nDelta;		// where the bar started and amount of change
	int nResult;			// normally start + delta
	int nMax, nMin;
	int nCtlId;
	int nSpan;



	nStart = pScrollBar->GetScrollPos();
	pScrollBar->GetScrollRange(&nMin, &nMax);
	nSpan = nMax - nMin;
	if ( nSpan < 100)	nSpan = 5;	// channel selection
	else nSpan = 200;	// 12 bit dac

	switch ( nSBCode)
		{	// switch on type of motion
	case SB_LINELEFT:
		nDelta = -1;
		break;

	case SB_LINERIGHT:
		nDelta = 1;
		break;

	case SB_PAGELEFT:
		nDelta = -nSpan;
		break;

	case SB_PAGERIGHT:
		nDelta = nSpan;
		break;

	case SB_THUMBTRACK:
		nDelta = (int)nPos - nStart;
		break;


	default:
		return;
		break;

		}	// switch on type of motion

	nResult = nStart;
	nResult += nDelta;
	if ( nResult > nMax) nResult = nMax;
	if ( nResult < nMin) nResult = nMin;


	// Now determine which scroll bar was used
	nCtlId = pScrollBar->GetDlgCtrlID();



	switch(nCtlId)
		{
	case IDC_SB_DMX_DAC:
		m_nDac = nResult;
		break;

	case IDC_SB_CHNL:		
		m_nChnl = nResult;
		gChannel = m_nChnl - 1;
		pCTscanDlg->ChangeActiveChannel();
		DmxCalRec.nChnl = gChannel;
//		SetWaveMode();	// change rcvr, scope, gates for new chnl
		// If chnl changed while setting trim gains,
		if ( DmxCalRec.nState == 10)
			{
			m_pHSDmxVariable = &DmxCalRec.nVgain[gChannel][m_nRf];
			m_nDac = *m_pHSDmxVariable;
			m_sbDac.SetScrollPos(m_nDac, TRUE);
			}

		if ( DmxCalRec.nState == 11)
			{
			m_pHSDmxVariable = &DmxCalRec.nChNull[gChannel][m_nRf];
			m_nDac = *m_pHSDmxVariable;
			m_sbDac.SetScrollPos(m_nDac, TRUE);
			}
//		SendMsg(CHANNEL_SELECT);
//		SetWaveMode();

		break;

		
	default:
		break;
		}

		
	pScrollBar->SetScrollPos(nResult, TRUE);
	
	UpdateData(FALSE);	// Copy variables to screen
	if (m_pHSDmxVariable != NULL)
		{
		*m_pHSDmxVariable = m_nDac;
		DmxCalRec.nRfFw = m_nRf;
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

void CDmxAdj::OnRbRf() 
	{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);	// Copy screen to variables
	DmxCalRec.nRfFw = m_nRf;
	if(m_nRf)	ExecuteState(m_nStateFw);	
	else		ExecuteState(m_nStateRf);
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
	}

void CDmxAdj::OnRbFw() 
	{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);	// Copy screen to variables
	DmxCalRec.nRfFw = m_nRf;
	if(m_nRf)	ExecuteState(m_nStateFw);	
	else		ExecuteState(m_nStateRf);	
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);	
	}



void CDmxAdj::PrintDmxCal(I_MSG_DMX_CAL *pImsg)
	{	// The top half of the structure is filled by the operator
		// The bottom half is filled by the master udp
		// Display the structure while the cal is operating
//	CFONT hOldFont;
	CString s, t;
	int i, j;


//	hOldFont = m_ceDmxOut.GetFont();
//	hOldFont = GetDlgItem(IDC_EN_DMX_OUT)->GetFont();
	s.Format(_T("Shunt = [%d,%d] T1Null = [%d,%d] Rej = [%d,%d]\r\nADC = %d   MsgNum = %d case = %d  ADCgnd = %d\r\n"),
		pImsg->DmxCal.nShunt[0],pImsg->DmxCal.nShunt[1],
		pImsg->DmxCal.nScope1Null[0],pImsg->DmxCal.nScope1Null[1],
		pImsg->DmxCal.nReject[0],pImsg->DmxCal.nReject[1],
		pImsg->DmxCal.nAdcReading, pImsg->MstrHdr.MsgNum, pImsg->DmxCal.nState, pImsg->DmxCal.nAdcGnd );


	switch (pImsg->DmxCal.nState)
		{	// switch on state or step number 

	case 7:
	case 8:
	case 9:
		t.Format(_T("G1Ref[2][2] = [%4d, %4d], [%4d, %4d]\r\n"),
			pImsg->DmxCal.nG1Ref[0][0], pImsg->DmxCal.nG1Ref[0][1],
			pImsg->DmxCal.nG1Ref[1][0], pImsg->DmxCal.nG1Ref[1][1]	);
		s += t;
		t.Format(_T("G2Ref[2][2] = [%4d, %4d], [%4d, %4d]\r\n"),
			pImsg->DmxCal.nG2Ref[0][0], pImsg->DmxCal.nG2Ref[0][1],
			pImsg->DmxCal.nG2Ref[1][0], pImsg->DmxCal.nG2Ref[1][1]	);
		s += t;
		t.Format(_T("Ag1 = [%7.3f,%7.3f], Bg1 = [%7.3f,%7.3f]\r\nAg2 = [%7.3f,%7.3f], Bg2 = [%7.3f,%7.3f]\r\n"),
			pImsg->DmxCal.fAg1[0],pImsg->DmxCal.fAg1[1],
			pImsg->DmxCal.fBg1[0],pImsg->DmxCal.fBg1[1],
			pImsg->DmxCal.fAg2[0],pImsg->DmxCal.fAg2[1],
			pImsg->DmxCal.fBg2[0],pImsg->DmxCal.fBg2[1]);
		s += t;

		break;

	case 10:
		if ( m_nRf) s += _T("Trim gains for FW\r\n");
		else		s += _T("Trim gains for RF\r\n");
		for ( j = 0; j < 4; j++)
			{
			for ( i = 0; i < 10; i++)
				{
				t.Format(_T("%4d,"), pImsg->DmxCal.nVgain[i +10*j][m_nRf]);
				s += t;
				}
			s += ("\r\n");
			}
		break;

	case 11:
	case 12:
		if ( m_nRf) s += _T("Chnl Nulls for FW  ");
		else		s += _T("Chnl Nulls for RF  ");
		t.Format(_T("[Chnl = %2d]\r\n"), pImsg->DmxCal.nChnl+1);
		s += t;

		for ( j = 0; j < 4; j++)
			{
			for ( i = 0; i < 10; i++)
				{
				t.Format(_T("%4d,"), pImsg->DmxCal.nChNull[i +10*j][m_nRf]);
				s += t;
				}
			s += ("\r\n");
			}
		break;


	case 13:
	case 14:
	case 15:
		if (m_nRf == 0) break;	// no reject level for rf mode
		t.Format(_T("fArej = %7.3f, fBrej = %7.3f\r\n"), pImsg->DmxCal.fArej, pImsg->DmxCal.fBrej);
		s += t;
		break;


		}	// switch on state or step number 


	m_ceDmxOut.SetWindowText(s);

	}

void CDmxAdj::OnTimer(UINT nIDEvent) 
	{
	// TODO: Add your message handler code here and/or call default
	
	CString s;

#if 1
	I_MSG_DMX_CAL	*pImsg;

	while (plistUtData.GetCount() > 4)
		{
		pImsg = (I_MSG_DMX_CAL *)plistUtData.RemoveHead();
		delete pImsg;
		}

	while (!plistUtData.IsEmpty() )
		{

		pImsg = (I_MSG_DMX_CAL *)plistUtData.RemoveHead();
	
		switch (pImsg->MstrHdr.MsgId )
			{	// switch on  Msg id
		case RUN_MODE:
//			PrintMsgRun(  pImsg);
			break;

		case CAL_MODE:
//			PrintMsg02((I_MSG_CAL *)pImsg);
			break;

		default:
		case PKT_MODE:
//			PrintMsg03( (I_MSG_PKT *) pImsg);
			break;

		case SCOPE_DMX_CAL_MODE:
			memcpy ( (void *) &DmxCalRec, (void *) &pImsg->DmxCal, sizeof(SCOPE_DMX_CAL));
			PrintDmxCal(pImsg);
			break;


			}	// switch on  msg id
		
		delete pImsg;

		}

#endif

#if 0
	//dEBUG FOR UDP MSGS
	if ( strlen(UdpMsg99.txt) )
		{
		s.Format(_T("\r\nUdpMsg:[%d] %s",UdpMsg99.Seq, UdpMsg99.txt);
		UdpMsg99.txt[0] = 0;	// clear txt msg
		m_ceStdOut.SetWindowText(s);
		}
#endif
	
	CDialog::OnTimer(nIDEvent);
	}

void CDmxAdj::KillMe()
	{
	// Public access to OnCancel
	// Add to every modeless dialog
	OnCancel();
	}

BOOL CDmxAdj::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in TscanDlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CDmxAdj::Save_Pos()
	{
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition(_T("SCOPE_DMX_ADJ"), &wp);

	}

void CDmxAdj::SetAllChnlsRfFw(int nRf)
	{
	// Set every channel mode to either rf or full wave.  Set the scope selection also.
	// Modify the existing ConfigRec.  Assumes that ConfigRec is reasonable.
	// On dialog exit, advise operator to reload config file

	int i;

	nRf &= 1;

	for ( i = 0; i < 40; i++)
		{
		ConfigRec.receiver.det_option[i] = nRf;	// rf = 0, fw = 1
		//ConfigRec.OscopeRec.trace1[i] = nRf ^ 1;	// rf/fw flipped for scope
		ConfigRec.receiver.fil_option[i] = 1;		// 2.25 Mhz
		}

	pCTscanDlg->SystemInit();	// Send config file to instruments
	pCTscanDlg->UpdateChnlSelected();

	}


short CDmxAdj::ExecuteState(short nState)
	{
	// provide execution of state cases for both next and previous state functions
	// localState+1 is the next state

	CString s;
	short nStep;
	int chnltmp;

	DmxCalRec.nRfFw = m_nRf;
	if (nState < 20) nStep = nState;
	else nStep = nState - 20;

	s.Format(_T("[Step %2d]\r\n"), nStep);

	switch (nState)
		{
	default:
	case 0:
	case 20:
		// no operation, start of sequence
		m_pHSDmxVariable = NULL;
		s = _T("Invalid state transition");
		m_ceInstruct.SetWindowText(s);
		return 0;

	case 1:
	case 21:
//		ImportantScreen( &s);
		ImportantScreen2(s);	// argument is a reference
//		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 2:
		RfShuntSet(s);
		SetAllChnlsRfFw(m_nRf);
		// Update state case in master UDP
		// result in forcing reject ckt ON
//		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 22:
		FwShuntSet(s);
		SetAllChnlsRfFw(m_nRf);
		// Update state case in master UDP
		// result in forcing reject ckt ON
//		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;


	case 3:
		RfShuntAdj(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 23:
		FwShuntAdj(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;


		/********** Gate 1 Adjustments to linearize output amplitude of gate ***********/

	case 4:
		SetWaveMode();
		RfG1_0_Set(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 24:
		SetWaveMode();
		FwG1_0_Set(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 5:
		SetWaveMode();
		RfG1_Fs_Set(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 25:
		SetWaveMode();
		FwG1_Fs_Set(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;



		/********** Gate 2 Adjustments to linearize output amplitude of gate ***********/


	case 6:
		SetWaveMode();
		RfG2_0_Set(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 26:
		SetWaveMode();
		FwG2_0_Set(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 7:
		SetWaveMode();
		RfG2_Fs_Set(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 27:
		SetWaveMode();
		FwG2_Fs_Set(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;



		/********** Trace 1 Scope Null   ***********/


	case 8:
		SetWaveMode();
		RfTrace1Null(s);
		// Update state case in master UDP
		// result in forcing computation of fa, fb coefficients
//		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 28:
		SetWaveMode();
		FwTrace1Null(s);
		// Update state case in master UDP
		// result in forcing computation of fa, fb coefficients
//		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;


	case 9:
		SetWaveMode();
		RfTrace1NullAdj(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 29:
		SetWaveMode();
		FwTrace1NullAdj(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 10:
		SetWaveMode();
#if 0
		gChannel = m_nChnl - 1;
		SendMsg(CHANNEL_SELECT);
#endif
		RfTrimGainAdj(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 30:
		SetWaveMode();
		FwTrimGainAdj(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 11:
		// set receiver gains to -20 db
		chnltmp = gChannel;
		for ( gChannel = 0; gChannel < 40; gChannel++)
			{
			ConfigRec.receiver.gain[gChannel] = -200;
			//ConfigRec.receiver.tcg_enable[gChannel]=0;
			SendMsg(CHANNEL_SELECT);
			SendMsg(RECEIVER_GAIN);
			}
#if 0
		gChannel = chnltmp;
		SendMsg(CHANNEL_SELECT);
#endif

		SetWaveMode();
		RfChnlNull(s);	// manual gain adjustment
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 31:
		// set receiver gains to -20 db
		chnltmp = gChannel;
		for ( gChannel = 0; gChannel < 40; gChannel++)
			{
			ConfigRec.receiver.gain[gChannel] = -200;
			//ConfigRec.receiver.tcg_enable[gChannel]=0;
			SendMsg(CHANNEL_SELECT);
			SendMsg(RECEIVER_GAIN);
			}
#if 0
		gChannel = chnltmp;
		SendMsg(CHANNEL_SELECT);
#endif

		SetWaveMode();
		FwChnlNull(s);
		if (m_pHSDmxVariable != NULL)		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;


	case 12:
		m_nChnl = 1;
		m_sbDac.SetScrollPos(m_nChnl, TRUE);
		SetWaveMode();	// change rcvr, scope, gates for new chnl

		RfAutoChnlNull(s);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 32:
		m_nChnl = 1;
		m_sbDac.SetScrollPos(m_nChnl, TRUE);
		SetWaveMode();	// change rcvr, scope, gates for new chnl

		FwAutoChnlNull(s);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 13:
		RfEnd(s);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;


		/********** Reject Level Adjustments to linearize output amplitude of reject ***********/
		/**********  Full Wave Mode ONLY   *****************************************************/
	case 33:
		FwRej10(s);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 34:
		FwRej50(s);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

	case 35:
		FwEnd(s);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		break;

		}

	m_ceInstruct.SetWindowText(s);
	return 1;	// success

	}

short CDmxAdj::NextState(short CurrentState)
	{
	// States 0 - 19 are rf mode states
	// States 20-39 are fw mode states

	short localState;

	if (CurrentState > 19) CurrentState = 0;
	if (CurrentState < 0) CurrentState = 0;
	
	localState = CurrentState + 20*m_nRf;

	if (ExecuteState(localState+1)) 	return CurrentState+1;
	else return 0;
	}

short CDmxAdj::PrevState(short CurrentState)
	{
	// States 0 - 19 are rf mode states
	// States 20-39 are fw mode states
	short localState;

	if (CurrentState > 19) CurrentState = 2;
	if (CurrentState < 1) CurrentState = 2;

	localState = CurrentState + 20*m_nRf;

	if (ExecuteState(localState-1)) 	return CurrentState-1;
	else	return 0;

	}

void CDmxAdj::OnBnBack() 
	{
	// TODO: Add your control notification handler code here
	if(m_nRf)
		{
		m_nStateFw = PrevState(m_nStateFw);	// move to next state value
		}

	else
		{
		m_nStateRf = PrevState(m_nStateRf);
		}
	
	}

#if 0
void CDmxAdj::OnBnExecute() 
	{
	// TODO: Add your control notification handler code here
	if (m_nRf)
		switch(m_nStateFw)
			{
		default:
			break;

		case 2:
			// do something
			// Send Udp msg to compute
			break;
			}

	else
		switch (m_nStateRf)
		{
		default:
			break;

		case 2:
			// do something
			// Send Udp msg to compute
			break;
		}

	
	}
#endif

void CDmxAdj::OnBnOptions() 
	{
	// TODO: Add your control notification handler code here
	// Pop up window to allow operator to select default configuration or
	// Use trim gain from 1 channel on a) all chnls in same shoe or
	// b) all chnls in system
	// This is a modal dialog
	

	CScpDmx2 dlg;
	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
		{
//		SendMsg(JOINT_NUMBER_MSG);	// Set pipe number in instrument
		}
	else if (nResponse == IDCANCEL)
		{
		}

	}

void CDmxAdj::OnBnFinish() 
	{
	// TODO: Add your control notification handler code here
	// Write the demux structure back to the 

	CString s;

	DmxCalRec.nCmd = 5;	// compute linear equation for gate 1 & 2 coeff 
	try
		{
		CFile file (gszScopeDmxCalFile, CFile::modeCreate | 
				CFile::modeNoTruncate |  CFile::modeReadWrite);	// create if not existing
																// else open existing
		file.Write(&DmxCalRec, sizeof(DmxCalRec));
		// when this procedure closes, it will close the open file
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		}
	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}

	s.Format(_T("Scope Demux Cal file %s has been updated\r\n"), gszScopeDmxCalFile);
	MessageBox(s);
	
	}

void CDmxAdj::OnBnNext() 
	{
	// TODO: Add your control notification handler code here
	if(m_nRf)
		{
		m_nStateFw = NextState(m_nStateFw);	// move to next state value
		}

	else
		{
		m_nStateRf = NextState(m_nStateRf);
		}
	
	}


void CDmxAdj::SetWaveMode()
	{
	// Set the wave mode fw/rf based on m_nRf and update the config record and 
	// oscope and receiver windows to match.  Send the info to the instrument.
	gChannel = m_nChnl - 1;
	if (m_nRf)
		{	// fw mode = 1 in mmi
		//ConfigRec.OscopeRec.trace1[gChannel] = 0;	// I know, its backward
		ConfigRec.receiver.det_option[gChannel]=1;
		}

	else
		{	// rf
		//ConfigRec.OscopeRec.trace1[gChannel] = 1;	// I know, its backward
		ConfigRec.receiver.det_option[gChannel]=0;
		}

	// Mode independent

	//ConfigRec.OscopeRec.trace2[gChannel] = 3;	// gates 1 & 2
	//ConfigRec.OscopeRec.trigger_index[gChannel] = 0;	// IP
	ConfigRec.OscopeRec.trigger[gChannel] = 0;	// set lower nibble
	pCTscanDlg->UpdateChnlSelected();
	DmxCalRec.nChnl = gChannel;
	SendMsg(RECEIVER_RF_VIDEO);
	SendMsg(SCOPE_TRACE1);
	SendMsg(SCOPE_TRACE2);
	pCTscanDlg->ChangeActiveChannel();
	// SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
	}


void CDmxAdj::ImportantScreen(CString *s)
	{	// case 1 pointer to c string
	*s += _T("        IMPORTANT !\r\n");
	*s += _T("Set Oscilloscope sensitivity to 0.5 volts/division\r\n\r\n");
	*s += _T("\r\nPress NEXT to continue.");
	DmxCalRec.nState = 1;
	DmxCalRec.nCmd = 3;	// update
	}

	/**************** case 1 ***************************/

void CDmxAdj::ImportantScreen2(CString &s)
	{	// case 1 reference to c string
	m_pHSDmxVariable = NULL;
	DmxCalRec.nState = 1;
	DmxCalRec.nCmd = 3;	// update
	ConfigRec.pulser.pulse_on &= ~(RCV_BITS);
	ConfigRec.pulser.pulse_on |= (XMT_BITS) ;
	xchk |= XMT_BITS;
	rchk &= ~(RCV_BITS);
//CPulser::m_pDlg->CheckDlgButton(IDC_CkPls,1);
//CPulser::m_pDlg->CheckDlgButton(IDC_CkRcv,0);
ConfigRec.pulser.pulse_on |= PLS_ON_BIT;
CPulser::m_pDlg->OnBTNSyOn();
	s += _T("        IMPORTANT !        IMPORTANT !        IMPORTANT !\r\n");
	s += _T("Set Oscilloscope sensitivity to 0.5 volts/division\r\n");
	s += _T("Make sure both gates are being triggered and appear on TRACE 2\r\n");
	s += _T("In the PULSER dialog    Check the PLS box, UnCheck the RCV box\r\n");
	s += _T("and switch the System to OFF\r\n");
	s += _T("\r\nPress NEXT to continue.");
	}

	/**************** case 2 ***************************/

void CDmxAdj::RfShuntSet(CString &s)
	{	// case 2
	m_pHSDmxVariable = NULL;
	DmxCalRec.nState = 2;
	DmxCalRec.nCmd = 3;	// update

	s += _T("Ground the oscope TRACE 2 input and adjust the oscope offset to position the trace\r\n");
	s += _T("down 2 1/2 divisions below the center horizontal graticule.\r\n\r\n");  
	s += _T("Once you have done this\r\n");
	s += _T("\r\nPress NEXT to continue.");
	}

void CDmxAdj::FwShuntSet(CString &s)
	{	// case 2
	m_pHSDmxVariable = NULL;
	DmxCalRec.nState = 2;
	DmxCalRec.nCmd = 3;	// update

	s += _T("Ground the oscope TRACE 2 input and adjust the oscope offset to position the trace\r\n");
	s += _T("down 2 1/2 divisions below the center horizontal graticule.\r\n\r\n");  
	s += _T("Once you have done this\r\n");
	s += _T("\r\nPress NEXT to continue.");
	}

	/**************** case 3 ***************************/

void CDmxAdj::RfShuntAdj(CString &s)
	{	// case 3
	// Set scroll bar and display value to existing Shunt value
	m_pHSDmxVariable = &DmxCalRec.nShunt[0];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 3;
	DmxCalRec.nCmd = 3;	// update

	m_szLabel = _T("RF Shunt");
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("Set the TRACE 2 input mode to DC.\r\n");
	s += _T("Use the scroll bar on the left to adjust the offset to the center horizontal graticule.\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");
	}

void CDmxAdj::FwShuntAdj(CString &s)
	{	// case 3
	// Set scroll bar and display value to existing Shunt value
	m_pHSDmxVariable = &DmxCalRec.nShunt[1];	// full wave shunt value
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 3;
	DmxCalRec.nCmd = 3;	// update
	m_szLabel = _T("FW Shunt");
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("Set the TRACE 2 input mode to DC.\r\n");
	s += _T("Use the scroll bar on the left to re-adjust the offset of the base line trace.\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");
	}


	/**************** case 4 ***************************/

void CDmxAdj::RfG1_0_Set(CString &s)
	{	// case 4
	m_pHSDmxVariable = &DmxCalRec.nG1Ref[m_nRf][0];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 4;
	DmxCalRec.nCmd = 3;	// update
	m_szLabel = _T("RF Gate 1 Zero");
	UpdateData(FALSE);	// Copy variables to screen
#if 0
	// SendMsg to uncheck recvr and do system off
	ConfigRec.pulser.pulse_on = XMT_BITS & 3;	// pulser off but receiver on
	memset( &ConfigRec.pulser.pulse_chn_on,
			ConfigRec.pulser.pulse_on, MAX_CHANNEL);
	SendMsg(PULSER_ALL_ONOFF);
	pCTscanDlg->UpdateChnlSelected();

	s = _T("This program has turned off the PULSERS for all channels.\r\n");
#endif
	s += _T("Verify that the pulsers are OFF from the PULSER dialog\r\n\r\n");
	s += _T("Use the scroll bar on the left to re-adjust the GATE 1 zero\r\n");
	s += _T("so that it lies on the center horizontal graticule.\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");

	}

void CDmxAdj::FwG1_0_Set(CString &s)
	{	// case 4
	m_pHSDmxVariable = &DmxCalRec.nG1Ref[m_nRf][0];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 4;
	DmxCalRec.nCmd = 3;	// update
	m_szLabel = _T("FW Gate 1 Zero");
	UpdateData(FALSE);	// Copy variables to screen


	s += _T("Verify that the pulsers are OFF from the PULSER dialog\r\n\r\n");
	s += _T("Use the scroll bar on the left to re-adjust the GATE 1 zero\r\n");
	s += _T("so that it lies 2-1/2 divisions below the center graticule.\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");
	}


	/**************** case 5 ***************************/


void CDmxAdj::RfG1_Fs_Set(CString &s)
	{	// case 5
	m_pHSDmxVariable = &DmxCalRec.nG1Ref[m_nRf][1];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 5;
	DmxCalRec.nCmd = 3;	// update
	m_szLabel = _T("RF Gate 1 Full Scale");
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("Use the scroll bar on the left to adjust the GATE 1 Full Scale offset\r\n");
	s += _T("so that it lies 2-1/2 divisions above the base line trace (center graticule).\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");
	}


void CDmxAdj::FwG1_Fs_Set(CString &s)
	{	// case 5
	m_pHSDmxVariable = &DmxCalRec.nG1Ref[m_nRf][1];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 5;
	DmxCalRec.nCmd = 3;	// update
	m_szLabel = _T("FW Gate 1 Full Scale");
	UpdateData(FALSE);	// Copy variables to screen


	s += _T("Use the scroll bar on the left to adjust the GATE 1 Full Scale offset\r\n");
	s += _T("so that it lies 2-1/2 divisions above the center graticule.\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");


	}


	/**************** case 6 ***************************/

void CDmxAdj::RfG2_0_Set(CString &s)
	{	// case 6
	m_pHSDmxVariable = &DmxCalRec.nG2Ref[m_nRf][0];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 6;
	DmxCalRec.nCmd = 3;	// update
	m_szLabel = _T("RF Gate 2 Zero");
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("Verify that the pulsers are OFF from the PULSER dialog\r\n\r\n");
	s += _T("Use the scroll bar on the left to re-adjust the GATE 2 zero\r\n");
	s += _T("so that it lies on the base line trace.\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");

	}

void CDmxAdj::FwG2_0_Set(CString &s)
	{	// case 6
	m_pHSDmxVariable = &DmxCalRec.nG2Ref[m_nRf][0];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 6;
	DmxCalRec.nCmd = 3;	// update
	m_szLabel = _T("FW Gate 2 Zero");
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("Verify that the pulsers are OFF from the PULSER dialog\r\n\r\n");
	s += _T("Use the scroll bar on the left to re-adjust the GATE 2 zero\r\n");
	s += _T("so that it lies 2-1/2 divisions below the center graticule.\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");


	}


	/**************** case 7 ***************************/


void CDmxAdj::RfG2_Fs_Set(CString &s)
	{	// case 7
	m_pHSDmxVariable = &DmxCalRec.nG2Ref[m_nRf][1];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 7;
	DmxCalRec.nCmd = 3;	// update
	m_szLabel = _T("RF Gate 2 Full Scale");
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("Use the scroll bar on the left to adjust the GATE 2 Full Scale offset\r\n");
	s += _T("so that it lies 2-1/2 divisions above the base line trace (center graticule).\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");
	}


void CDmxAdj::FwG2_Fs_Set(CString &s)
	{	// case 7
	m_pHSDmxVariable = &DmxCalRec.nG2Ref[m_nRf][1];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 7;
	DmxCalRec.nCmd = 3;	// update
	m_szLabel = _T("FW Gate 2 Full Scale");
	UpdateData(FALSE);	// Copy variables to screen


	s += _T("Use the scroll bar on the left to adjust the GATE 2 Full Scale offset\r\n");
	s += _T("so that it lies 2-1/2 divisions above the center graticule.\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");
	}

	/**************** case 8 ***************************/


void CDmxAdj::RfTrace1Null(CString &s)
	{	// case 8
	m_pHSDmxVariable = NULL;
	s += _T("Ground the oscope TRACE 1 input and adjust the oscope offset to position the trace\r\n");
	s += _T("down 2 1/2 divisions below the center horizontal graticule.\r\n\r\n");  
	s += _T("Once you have done this\r\n");
	s += _T("\r\nPress NEXT to continue.");
	DmxCalRec.nState = 8;
	DmxCalRec.nCmd = 3;	// update

	}


void CDmxAdj::FwTrace1Null(CString &s)
	{	// case 8
	m_pHSDmxVariable = NULL;
	s += _T("Ground the oscope TRACE 1 input and adjust the oscope offset to position the trace\r\n");
	s += _T("down 2 1/2 divisions below the center horizontal graticule.\r\n\r\n");  
	s += _T("Once you have done this\r\n");
	s += _T("\r\nPress NEXT to continue.");
	DmxCalRec.nState = 8;
	DmxCalRec.nCmd = 3;	// update
	}

	/**************** case 9 ***************************/


void CDmxAdj::RfTrace1NullAdj(CString &s)
	{	// case 9
	// Set scroll bar and display value to existing Shunt value
	m_pHSDmxVariable = &DmxCalRec.nScope1Null[m_nRf];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);

	m_szLabel = _T("RF Trace 1 Null");
	UpdateData(FALSE);	// Copy variables to screen


	s += _T("Set the TRACE 1 input mode to DC.\r\n");
	s += _T("Use the scroll bar on the left to adjust the offset to the center horizontal graticule.\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");
	DmxCalRec.nState = 9;
	DmxCalRec.nCmd = 3;	// update
	// force ckt into reject mode to read the ground level.. not actually used in rf operation
	DmxCalRec.nReject[m_nRf] = 0xb35;
	}


void CDmxAdj::FwTrace1NullAdj(CString &s)
	{	// case 9
	// Set scroll bar and display value to existing Shunt value
	m_pHSDmxVariable = &DmxCalRec.nScope1Null[m_nRf];	// full wave shunt value
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	m_szLabel = _T("FW Trace 1 Null");
	UpdateData(FALSE);	// Copy variables to screen


	s += _T("Set the TRACE 1 input mode to DC.\r\n");
	s += _T("Use the scroll bar on the left to re-adjust the offset of the base line trace.\r\n");
	s += _T("Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");
	DmxCalRec.nState = 9;
	DmxCalRec.nCmd = 3;	// update
	// force ckt into reject mode to read the ground level
	DmxCalRec.nReject[m_nRf] = 0xb35;
	}


	/**************** case 10 ***************************/


void CDmxAdj::RfTrimGainAdj(CString &s)
	{	// case 10
	// Set the trim gain for all 40 channels

	m_pHSDmxVariable = &DmxCalRec.nVgain[gChannel][m_nRf];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	m_szLabel = _T("RF Trim Gain");
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("Inject a signal of 80% amplitude on the selected channel.\r\n");
	s += _T("Use the receiver gain and scope output 'Blanking Gate' to achieve\r\n");
	s += _T("the correct output from the channel.  Then use the scroll bar on the left\r\n");
	s += _T("to adjust the peak signal on the oscilloscope to be at the 80% level\r\n");
	s += _T("Repeat this procedure for all 40 channels.  Once this is finished\r\n");
	s += _T("\r\nPress NEXT to continue.");

	DmxCalRec.nState = 10;
	DmxCalRec.nCmd = 3;	// update

	}

void CDmxAdj::FwTrimGainAdj(CString &s)
	{	// case 10
	// Set the trim gain for all 40 channels

	m_pHSDmxVariable = &DmxCalRec.nVgain[gChannel][m_nRf];	// full trim gain value
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	m_szLabel = _T("FW Trim Gain");
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("Inject a signal of 90% amplitude on the selected channel.\r\n");
	s += _T("Use the receiver gain and scope output 'Blanking Gate' to achieve\r\n");
	s += _T("The correct output from the channel.  Then use the scroll bar on the left\r\n");
	s += _T("to adjust the peak signal on the oscilloscope to be at the 90% level\r\n");
	s += _T("Repeat this procedure for all 40 channels.  Once this is finished\r\n");
	s += _T("\r\nPress NEXT to continue.");

	DmxCalRec.nState = 10;
	DmxCalRec.nCmd = 3;	// update
	}

void CDmxAdj::RfChnlNull(CString &s)
	{	// case 11
		// for initial testing, set nulls from mmi

	m_pHSDmxVariable = &DmxCalRec.nChNull[gChannel][m_nRf];	// full trim gain value
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	m_szLabel = _T("RF Chnl Null");
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("Verify that the pulsers are OFF from the PULSER dialog\r\n");
	s += _T("bandwidth is 2.25 Mhz and receiver gain is -20.0 db in the RECEIVER dialog\r\n\r\n");
	s += _T("This step is a MANUAL adjustment.  The next screen will do an AUTOMATIC adjustment.\r\n");
	s += _T("Use the scroll bar on the left to adjust the Channel Null so that\r\n");
	s += _T("the ADC reading shown in the upper screen equals the value it had\r\n");
	s += _T("in Step 1, typically between +/- 3 counts.\r\n");
	s += _T("Repeat this procedure for all 40 channels.  Once this is finished\r\n");
	s += _T("\r\nPress NEXT to continue.");

	DmxCalRec.nState = 11;
	DmxCalRec.nCmd = 3;	// update
	}

void CDmxAdj::FwChnlNull(CString &s)
	{	// case 11
		// for initial testing, set nulls from mmi

	m_pHSDmxVariable = &DmxCalRec.nChNull[gChannel][m_nRf];	// full trim gain value
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	m_szLabel = _T("FW Chnl Null");
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("Verify that the pulsers are OFF from the PULSER dialog\r\n");
	s += _T("bandwidth is 2.25 Mhz and receiver gain is -20.0 db in the RECEIVER dialog\r\n\r\n");
	s += _T("This step is a MANUAL adjustment.  The next screen will do an AUTOMATIC adjustment.\r\n");
	s += _T("Use the scroll bar on the left to adjust the Channel Null so that\r\n");
	s += _T("the ADC reading shown in the upper screen equals the value it had\r\n");
	s += _T("in Step 1, typically between +/- 3 counts.\r\n");
	s += _T("Repeat this procedure for all 40 channels.  Once this is finished\r\n");
	s += _T("\r\nPress NEXT to continue.");

	DmxCalRec.nState = 11;
	DmxCalRec.nCmd = 3;	// update
	}

void CDmxAdj::RfAutoChnlNull(CString &s)
	{	// case 12

	m_pHSDmxVariable = NULL;
	m_szLabel = _T("RF Auto Chnl Null");
	m_nDac = 0;
	m_sbDac.SetScrollPos(m_nDac);
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("The UDP will attempt to null all 40 RF channels.  This operation will be\r\n");
	s += _T("complete when the chnl number shown above reaches 40 and the readings for\r\n");
	s += _T("ADC stabilize.  If this operation is not satisfactory, go back to step 11 and\r\n");
	s += _T("manually adjust the channel nulls.\r\n");
	s += _T("THIS OPERATION WILL START IN 3 SECONDS\r\n");

	DmxCalRec.nState = 12;
	DmxCalRec.nCmd = 3;	// update
	}

void CDmxAdj::FwAutoChnlNull(CString &s)
	{	// case 12

	m_pHSDmxVariable = NULL;
	m_szLabel = _T("FW Auto Chnl Null");
	m_nDac = 0;
	m_sbDac.SetScrollPos(m_nDac);
	UpdateData(FALSE);	// Copy variables to screen

	s += _T("The UDP will attempt to null all 40 FW channels.  This operation will be\r\n");
	s += _T("complete when the chnl number shown above reaches 40 and the readings for\r\n");
	s += _T("ADC stabilize.  If this operation is not satisfactory, go back to step 11 and\r\n");
	s += _T("manually adjust the channel nulls.\r\n");
	s += _T("THIS OPERATION WILL START IN 3 SECONDS\r\n");

	DmxCalRec.nState = 12;
	DmxCalRec.nCmd = 3;	// update
	}

void CDmxAdj::RfEnd(CString &s)
	{	// case 13

	s += _T("RF Scope Demux Board Cal is complete.  Be sure to save the file ('finish button')\r\n");
	s += _T("before exiting the dialog.  If you have not calibrated the FW mode, do so now\r\n");

	DmxCalRec.nState = 13;
	DmxCalRec.nCmd = 3;	// update
	}

void CDmxAdj::FwRej10(CString &s)
	{	// case 13
	m_pHSDmxVariable = &DmxCalRec.nReject[0];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 13;
	DmxCalRec.nCmd = 3;	// update
	m_szLabel = _T("FW Reject 10%");
	UpdateData(FALSE);	// Copy variables to screen


	s += _T("Make sure the pulsers are on and adjust the full-wave Trace 1 A-scan\r\n");
	s += _T("to a signal level of 100% on the scope\r\n");
	s += _T("Use the scroll bar on the left to adjust the Reject 10% level so that the \r\n");
	s += _T("'knee' of the reject area is at 10%.  Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");

	}

void CDmxAdj::FwRej50(CString &s)
	{	// case 14
	m_pHSDmxVariable = &DmxCalRec.nReject[1];
	m_nDac = *m_pHSDmxVariable;
	m_sbDac.SetScrollPos(m_nDac);
	DmxCalRec.nState = 14;
	DmxCalRec.nCmd = 3;	// update
	m_szLabel = _T("FW Reject 50%");
	UpdateData(FALSE);	// Copy variables to screen


	s += _T("Use the scroll bar on the left to adjust the Reject 50% level so that the\r\n");
	s += _T("'knee' of the reject area is at 50%.  Once this step is completed\r\n");
	s += _T("\r\nPress NEXT to continue.");

	}

void CDmxAdj::FwEnd(CString &s)
	{	// case 15

	// Reject coefficients generated here

	s += _T("FW Scope Demux Board Cal is complete.  Be sure to save the file ('finish button')\r\n");
	s += _T("before exiting the dialog.  If you have not calibrated the RF mode, do so now\r\n");

	DmxCalRec.nState = 15;
	DmxCalRec.nCmd = 3;	// update

	}
