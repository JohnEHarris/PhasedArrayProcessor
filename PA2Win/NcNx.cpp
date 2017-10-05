// NcNx.cpp : implementation file
//

#include "stdafx.h"
#include "PA2Win.h"
#include "PA2WinDlg.h"
#include "NcNx.h"
#include "afxdialogex.h"
#include "NcNx.h"
#include <stdio.h>
#include <string.h>
// std lib
#include <string>
#include <iostream>
#include <sstream>
using std::string;
using std::stringstream;




// General purpose integer text to integer array converter
void CstringToIntArray(CString s, int *pArray, int nArraySize)
	{
	char txt[4096];
	int i;
	CstringToChar(s,txt);
	string num = txt;
	stringstream stream(num);
	i = 0;
	while (stream)
		{
		stream >> pArray[i++]; // now have array of ints
		if ( i >= nArraySize) return;
		}

	}



// CNcNx dialog

IMPLEMENT_DYNAMIC(CNcNx, CDialogEx)

CNcNx::CNcNx(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_NCNX_PA, pParent)
//	, m_nAscanCnt(0)
	{
	m_DlgLocationKey = _T("NC_NX_PA2");
	m_DlgLocationSection = _T("Dialog Locations");	// Section is always this string for all dlgs
	m_nPopulated = 0;
	}

CNcNx::~CNcNx()
	{
	TRACE( _T( "CNcNx::~CNcNx()\n" ) );
	gDlg.pNcNx = 0;
	}

void CNcNx::DoDataExchange(CDataExchange* pDX)
{
CDialogEx::DoDataExchange( pDX );
DDX_Control( pDX, IDC_SP_PAP, m_spPap );
DDX_Control( pDX, IDC_SP_SEQ, m_spSeq );
DDX_Control( pDX, IDC_SP_CH, m_spCh );
DDX_Control( pDX, IDC_SP_GATE, m_spGate );
DDX_Control( pDX, IDC_SP_PARAM, m_spParam );
DDX_Control( pDX, IDC_LB_NCNX, m_lbOutput );
DDX_Control( pDX, IDC_CB_CMDS, m_cbCommand );
DDX_Control( pDX, IDC_SP_BOARD, m_spBoard );
DDX_Control( pDX, IDC_BN_DONOTHING, m_bnDoNoting );
DDX_Control( pDX, IDC_ED_PARAM, m_edParam );
	}


BEGIN_MESSAGE_MAP(CNcNx, CDialogEx)
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_PAP, &CNcNx::OnDeltaposSpPap )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_BOARD, &CNcNx::OnDeltaposSpBoard )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_SEQ, &CNcNx::OnDeltaposSpSeq )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_CH, &CNcNx::OnDeltaposSpCh )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_GATE, &CNcNx::OnDeltaposSpGate )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_PARAM, &CNcNx::OnDeltaposSpParam )
	ON_BN_CLICKED( IDC_BN_ERASE, &CNcNx::OnBnClickedBnErase )
	ON_CBN_SELCHANGE( IDC_CB_CMDS, &CNcNx::OnCbnSelchangeCbCmds )
	//ON_WM_VSCROLL()
	ON_BN_CLICKED( IDC_BN_DONOTHING, &CNcNx::OnBnClickedBnDonothing )
	ON_EN_CHANGE( IDC_ED_PARAM, &CNcNx::OnChangeEdParam )
	ON_BN_CLICKED(IDC_RB_SMALLCMD, &CNcNx::OnBnClickedRbSmallcmd)
	ON_BN_CLICKED(IDC_RB_LARGECMDS, &CNcNx::OnBnClickedRbLargecmds)
END_MESSAGE_MAP()


// CNcNx message handlers

void CNcNx::DebugOut(CString s)
	{
	m_lbOutput.AddString( s );
	gDlg.pUIDlg->SaveDebugLog( s );
	}

#define PAP_MAX		0
#define BOARD_MAX	8
#define GATE_MAX	3
#define PARAM_MAX	5000

BOOL CNcNx::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	PositionWindow();
	m_nPAP = m_nBoard = m_nSeq = m_nCh = m_nGate = m_nParam	= 0;
	guAscanMsgCnt = 0;
	// if this is the PAG
#ifdef I_AM_PAG
	m_spPap.SetRange( 0, PAP_MAX );	// gnMaxClientsPerServer - 1 );  //how many clients do I have
	m_spBoard.SetRange( 0, BOARD_MAX-1 );	// gnMaxClients - 1 );		// how many clients does my client have
#else
	// I am the PAP
	m_spPap.SetRange( 0, 0 );  //how many clients do I have
	m_spBoard.SetRange( 0, gnMaxClients-1 );		// how many clients does my client have
#endif

	m_spSeq.SetRange( 0, gMaxSeqCount-1 );		// number of sequence in firing scheme
	m_spCh.SetRange( 0, gMaxChnlsPerMainBang-1 );		// number of channel in each sequence
	m_spGate.SetRange( 0, 3 );		// number of gates in each channel
	m_spParam.SetRange( 1, PARAM_MAX );	// depends of command selected from list box

	m_lbOutput.ResetContent();
	m_cbCommand.ResetContent();
	m_nPAP = m_nBoard = m_nSeq = m_nCh = m_nGate = m_nParam	= 0;
	OnBnClickedRbSmallcmd();	// calls 	PopulateCmdComboBox();
	CButton* pButton = (CButton*)GetDlgItem(IDC_RB_SMALLCMD);
	pButton->SetCheck(true);

	m_cbCommand.SetCurSel ( 2 );	// Gate Delay

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
	}

void CNcNx::Save_Pos()
	{
	WINDOWPLACEMENT wp;
	if (gDlg.pTuboIni == NULL) return;
	// Save closing location of window
	GetWindowPlacement(&wp);
	//m_DlgLocationKey is class member. Set in constructor
	gDlg.pTuboIni->SaveWindowLastPosition(m_DlgLocationSection, m_DlgLocationKey, &wp);
	gDlg.pTuboIni->SaveIniFile();
	}

void CNcNx::OnOK()
	{
	// TODO: Add your specialized code here and/or call the base class
	// Using properties in Resource tool, change default button to something other than OK
	// Otherwise casual carriage return closes dialog.
	Save_Pos();
	CDialogEx::OnOK();
	delete this;
	}


void CNcNx::OnCancel()
	{
	// TODO: Add your specialized code here and/or call the base class

	// don't save the final position
	CDialogEx::OnCancel();
	delete this;
	}


void CNcNx::PostNcDestroy()
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::PostNcDestroy();
	}

void CNcNx::OnDeltaposSpBoard( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
#if 0
	m_nBoard = pNMUpDown->iPos + pNMUpDown->iDelta;
	if (m_nBoard < 0)	m_nBoard = 0;
	if (m_nBoard > BOARD_MAX)	m_nBoard = BOARD_MAX;
#endif
	m_nBoard = GetSpinValue( pNMUpDown, &m_spBoard );
	}


void CNcNx::OnDeltaposSpPap( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	// must use vertical scroll to retrieve the new value after the button spins
	*pResult = 0;
#if 0
	m_nPAP = pNMUpDown->iPos + pNMUpDown->iDelta;
	//if (m_nPAP < 0)	
	m_nPAP = 0;
#endif
	m_nPAP = GetSpinValue( pNMUpDown, &m_spPap );
	}


void CNcNx::OnDeltaposSpSeq( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
#if 0
	m_nSeq = pNMUpDown->iPos + pNMUpDown->iDelta;
	if (m_nSeq > gMaxSeqCount - 1)	m_nSeq = gMaxSeqCount - 1;
	if (m_nSeq < 0)		m_nSeq = 0;
#endif
	m_nSeq = GetSpinValue( pNMUpDown, &m_spSeq );
	}


void CNcNx::OnDeltaposSpCh( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
#if 0
	m_nCh = pNMUpDown->iPos + pNMUpDown->iDelta;
	if (m_nCh > gMaxChnlsPerMainBang - 1)	m_nCh =  gMaxChnlsPerMainBang - 1;
	if (m_nCh < 0)		m_nCh = 0;
#endif
	m_nCh = GetSpinValue( pNMUpDown, &m_spCh );
	}


void CNcNx::OnDeltaposSpGate( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
#if 0
	m_nGate = pNMUpDown->iPos + pNMUpDown->iDelta;
	if (m_nGate > 3)		m_nGate = 3;
	if (m_nGate < 0)		m_nGate = 0;
#endif
	m_nGate = GetSpinValue( pNMUpDown, &m_spGate );
	}

// Do limit checking on spin value
// Uses range limit normally set in OnInitDialog
// If you don't set the range this will not work
// Compiler warns not to pass spin control as a reference... says its a delete function
// But this seems to work by passing a pointer.
//
int CNcNx::GetSpinValue( LPNMUPDOWN pNMUpDown, CSpinButtonCtrl *m_spButton )
	{
	int nValue;
	DWORD nPos;
	int min, max;
	nValue = pNMUpDown->iPos + pNMUpDown->iDelta;
	nPos = m_spButton->GetRange();	// GetRange returns a double
	min = nPos >> 16;	max = nPos & 0xffff;
	if (nValue < min)		nValue = min;
	if (nValue > max)		nValue = max;
	return nValue;
	}

// Parameter variable depends on what command is selected
// could be gate delay, or receiver gain, or trigger level of a flaw
void CNcNx::OnDeltaposSpParam( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// Copy this code for all spinners BEGIN
	// TODO: Add your control notification handler code here
	*pResult = 0;
#if 0

	int nValue;
	DWORD nPos;
	int min, max;
	nValue = pNMUpDown->iPos + pNMUpDown->iDelta;
	// Limit checking so we don't get 1 below or 1 above
	// change the SPINNER PARAMETER here when copying code
	nPos = m_spParam.GetRange();	min = nPos >> 16;	max = nPos & 0xffff;
	if (nValue < 0)		nValue = 0;
	if (nValue > PARAM_MAX)	nValue = PARAM_MAX;
	// Copy this code for all spinners END
#endif
	// assuming that you must set the range before first use, why
	// couldn't microsoft do this
	m_nParam = GetSpinValue( pNMUpDown, &m_spParam );
	}


void CNcNx::UpdateTitle(void)
	{
	CString s, t;
	s.Format(_T("Nc Nx Phased Array 2 PAP = %d, Instrument = %2d\r\n"), m_nPam, m_nBoard);
	SetWindowText(s);
	}


//
// Use the ini file to reposition window in same location it was when the window closed.
void CNcNx::PositionWindow()
	{
	if (gDlg.pTuboIni == NULL)	return;

	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	GetWindowPlacement(&wp);
	dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	// m_DlgLocationKey set in constructor
	gDlg.pTuboIni->GetWindowLastPosition(m_DlgLocationSection, m_DlgLocationKey, &rect);

	if (((rect.right - rect.left) >= dx) &&
		((rect.bottom - rect.top) >= dy))
		{
		wp.rcNormalPosition = rect;
		SetWindowPlacement(&wp);
		}
	}



void CNcNx::OnBnClickedBnErase()
	{
	// TODO: Add your control notification handler code here
	m_lbOutput.ResetContent();
	guAscanMsgCnt = 0;
	}

// Fill the combo box with strings whose index in the combobox matches the 
// command ID found in Cmds.h
// NcNx is a test command. Will likely never exist for real system
// Short commands have cmd ID < 0x200
void CNcNx::PopulateCmdComboBox()
	{
	CString s;
	m_cbCommand.ResetContent();
	if (m_nShowSmallCmds)
		{
		s.Format(_T("null 0"));				m_cbCommand.AddString(s);
		s.Format(_T("FakeData"));			m_cbCommand.AddString(s);	//1
		s.Format(_T("Gate n Delay"));		m_cbCommand.AddString(s);	//2
		s.Format(_T("Gate n Range"));		m_cbCommand.AddString(s);	//3
		s.Format(_T("Gate n Blank"));		m_cbCommand.AddString(s);	//4
		s.Format(_T("Gate n Thold"));		m_cbCommand.AddString(s);	//5
		s.Format(_T("Gate n Trigger"));		m_cbCommand.AddString(s);	//6
		s.Format(_T("Gate n Polarty"));		m_cbCommand.AddString(s);	//7
		s.Format(_T("Gate n TOF"));			m_cbCommand.AddString(s);	//8
		s.Format(_T("SetTcgClockRate"));	m_cbCommand.AddString(s);	//9
		s.Format(_T("TCGTriggerDelay"));	m_cbCommand.AddString(s);	//10
		s.Format(_T("TCGGainClock"));		m_cbCommand.AddString(s);	//11
		s.Format(_T("TCGChnlGainDelay"));	m_cbCommand.AddString(s);	//12
		s.Format(_T("SetPRF"));					m_cbCommand.AddString(s);	//13
		s.Format(_T("ASCAN_SCOPE"));			m_cbCommand.AddString(s);	//14
		s.Format(_T("ASCAN_SCOPE_DELAY"));		m_cbCommand.AddString(s);	//15
		s.Format(_T("SET_ASCAN_PEAK_MODE"));	m_cbCommand.AddString(s);	//16
		s.Format(_T("ASCAN_RF_BEAM"));			m_cbCommand.AddString(s);	//17--check case statements below
		}
	else
		{
		// show large commands
		s.Format(_T("=="));		m_cbCommand.AddString(s);	m_cbCommand.AddString(s);	//1st 2 blank
		s.Format(_T("SEQ_TCG_GAIN"));			m_cbCommand.AddString(s);	//0x200 +2
		s.Format(_T("TCG_GAIN_CMD"));			m_cbCommand.AddString(s);	//0x200 +3
		s.Format(_T("ASCAN_BEAMFORM_DELAY"));	m_cbCommand.AddString(s);	//0x200 +4
		}
	m_cbCommand.SetCurSel(2);
	m_nPopulated = 1;
	}

void CNcNx::OnCbnSelchangeCbCmds()
	{
	CString s, t;
	int nCmdOffset;
	int nCmdLarge = 0;
	// TODO: Add your control notification handler code here
	m_nCmdId = m_cbCommand.GetCurSel();
	if (m_nShowSmallCmds)	nCmdOffset = 0;
	else
		{
		nCmdOffset = 0x200;
		nCmdLarge = m_nCmdId + nCmdOffset;
		}
	t.Format(_T("m_nCmdId = %d"), m_nCmdId + nCmdOffset);

				
	switch (m_nCmdId + nCmdOffset)
		{
		case 0:	s.Format( _T( "null %d" ), m_nCmdId );	break;
		case 1:	s =_T( "Fake Data" );					break;
		case 2: s.Format(_T("Gate %d Delay %d"), m_nGate, m_nParam); break;
		case 3: s.Format(_T("Gate %d Range %d"), m_nGate, m_nParam); break;
		case 4: s.Format(_T("Gate %d Blank %d"), m_nGate, m_nParam); break;
		case 5: s.Format(_T("Gate %d Thold %d"), m_nGate, m_nParam); break;
		case 6: s.Format(_T("Gate %d Trigger %d"), m_nGate, m_nParam); break;
		case 7: s.Format(_T("Gate %d Polarity %d"), m_nGate, m_nParam); break;
		case 8: s.Format(_T("Gate %d TOF %d"), m_nGate, m_nParam); break;

		case 0x202: s = _T("SEQ_TCG_GAIN");					break;
		default:	s = t;		break;
		}
	//m_lbOutput.AddString( s );


	switch (m_nCmdId + nCmdOffset)
		{
		case 0:			break;
		case 1:	
			FakeData( m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam );
			break;
		case 2: 
		case 3: 
		case 4: 
		case 5: 
		case 6: 
		case 7: 
		case 8: 
			GateCmd( m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam );
			break;
		// TCG commands
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			TcgCmd( m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam );
			break;
			//AScan control commands
		case 14:
		case 15:
		case 16:
		case 17:
			WordCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
			break;

		case 2 + 0x200:
		case 3 + 0x200:
		case 4 + 0x200:
			// build command here
			LargeCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, nCmdLarge, (WORD) m_nParam);
			t = _T("Large Command");
			break;
		default:	
			break;
		}	
	}

void CNcNx::OnBnClickedBnDonothing()
	{
	// TODO: Add your control notification handler code here
	// Does nothing but take focus off of OK and Cancel
	// In Resource view use Properties to set this as the default button
	int i;
	i = m_nParam;
	}


// Message goes to one specific PAP and one specific board in the PAP
// 2016-06-27 we will assume that messages will be built by the routine which configures the
// parameters of the message. This is a departure from the way we have done this for a couple
// of decades. Allocate memory to the message here to be put into a linked list.  Call the main
// dialog to actually add the message to the linked list so the message sequence number can be attached.
//
void CNcNx::SendMsg(GenericPacketHeader *pMsg)//, int nChTypes)
	{
#ifdef I_AM_PAG
	CString s;
	//int nPam, nInst, nChnl;
	//PAP_INST_CHNL_NCNX * pSendNcNx;
	if (pMsg->wMsgID >= 0x200)
		{
		ST_LARGE_CMD *pSend = new (ST_LARGE_CMD);
		memcpy((void*)pSend, (void*)pMsg, sizeof(ST_LARGE_CMD));
		pSend->bPAPNumber = m_nPAP;
		pSend->bBoardNumber = m_nBoard;

		switch (pMsg->wMsgID)
			{
		case 1+0x200:
			// development code special
			//pSendNcNx = (PAP_INST_CHNL_NCNX*)pSend;
			//memset((void *)pSendNcNx, 0, sizeof(PAP_INST_CHNL_NCNX));
			//memcpy((void *)pSendNcNx, (void *)pMsg, pMsg->wByteCount);	// sizeof(ST_NC_NX) * 72);
			break;
		case SEQ_TCG_GAIN_CMD_ID:
			s.Format(_T("SEQ_TCG_GAIN_CMD PAP=%d, Board=%d\n"), pSend->bPAPNumber, pSend->bBoardNumber);
			TRACE(s);
			break;
		case TCG_GAIN_CMD_ID:
			s.Format(_T("TCG_GAIN_CMD PAP=%d, Board=%d\n"), pSend->bPAPNumber, pSend->bBoardNumber);
			break;
		case SET_ASCAN_BEAMFORM_DELAY_ID:
			s.Format(_T("SET_ASCAN_BEAMFORM_DELAY PAP=%d, Board=%d\n"), pSend->bPAPNumber, pSend->bBoardNumber);
			break;
		default:
			TRACE(_T("Unrecognized message .. delete pSend\n"));
			delete pSend;
			return;
			}

		gDlg.pUIDlg->SendMsgToPAP((int)pSend->bPAPNumber, pSend->wMsgID, (void *)pSend);
		//delete pSend;
		}
	else
		{
		if (pMsg->wByteCount == 32)
			{	// small command format
			ST_SMALL_CMD *pSmall = (ST_SMALL_CMD *)pMsg;
			ST_SMALL_CMD *pSend = new ST_SMALL_CMD;
			memset((void*)pSend, 0, sizeof(ST_SMALL_CMD));
			memcpy((void*)pSend, (void*)pSmall, sizeof(ST_SMALL_CMD));
			gDlg.pUIDlg->SendMsgToPAP((int)pSend->bPAPNumber, pSend->wMsgID, (void *)pSend);
			//delete pSend; auto deleted in sending function
			}
		else
			{	// large command format
			ST_LARGE_CMD *pSend = new ST_LARGE_CMD;
			memset((void*)pSend, 0, sizeof(ST_LARGE_CMD));
			memcpy((void*)pSend, (void*)pMsg, sizeof(ST_LARGE_CMD));
			gDlg.pUIDlg->SendMsgToPAP((int)pMsg->bPapNumber, pMsg->wMsgID, (void *)pSend);
			//delete pSend;
			}
		}
#endif

	}


// All inputs are integers, but transmitted value to instrument may not be.
// Cmds are Delay=2, Range=3, Blank=4, Thold=5, Trigger=6 Polarity=7 TOF=8
// All commands use the gate delay structure.
// Unlike the Enet2 boards of Truscope, no command settings are retained in the
// PAP
//
void CNcNx::GateCmd( int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, int nValue)
	{
	CString s, t, sym;
	switch (nCmd)
		{
	case 2:		sym = _T("Delay: ");		break;
	case 3:		sym = _T("Rng: ");			break;
	case 4:		sym = _T("Blk: ");			break;
	case 5:		sym = _T("Thl: ");			break;
	
	case 6:		// gate trigger source for all 4 gates
		sym = _T("Trg:"); 	
		// high nibble sets triger for all 4 gates, low nibble is enable/disable	
		// bit7,6,5,4:  trigger select (0:mbs, 1:threshold) for all gate 4-1
		// bit 3-0: gate enable
		break;
	case 7:		sym = _T("Pol:"); 		break;	// gate data mode ie signal polarity
	case 8:		sym = _T("Tof:"); 		break;	// gate data mode ie tof
	default:	sym = _T( "???" );		return;
		}

	memset(&m_GateCmd, 0, sizeof(ST_GATE_DELAY_CMD));
	m_GateCmd.Head.wMsgID = nCmd;
	m_GateCmd.Head.wByteCount = 32;
	m_GateCmd.Head.uSync = SYNC;
//		m_GateCmd.Head.wMsgSeqCnt;	SET BY SENDING ROUTINE
	m_GateCmd.Head.bPapNumber = nPap;
	m_GateCmd.Head.bBoardNumber = nBoard;
	m_GateCmd.bSeq = nSeq;
	m_GateCmd.bChnl = nCh;
	m_GateCmd.bGateNumber = nGate;
	m_GateCmd.wDelay = nValue;	// called delay but now can be one of many
	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, Seq=%d, Ch=%d, Gate=%d, Value=%5d\n"),
		m_GateCmd.Head.wMsgID, m_GateCmd.Head.wByteCount, m_GateCmd.Head.bPapNumber,
		m_GateCmd.Head.bBoardNumber, m_GateCmd.bSeq, m_GateCmd.bChnl,
		m_GateCmd.bGateNumber, m_GateCmd.wDelay);
	t = sym + s;
	m_lbOutput.AddString(t);
	SendMsg((GenericPacketHeader*)&m_GateCmd);
	//if (m_RbGates >= 4) break;	// one command sets all gates for a chnl	
	}

void CNcNx::TcgCmd( int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, int nValue )
	{
	CString s, t, sym;
	switch (nCmd)
		{
	default:		sym = _T( "???" );				return;
	case 9:		sym = _T("TcgClockRate: "); 		break;
	case 10:	sym = _T("TCGTriggerDelay: "); 		break;
	case 11:	sym = _T("TCGGainClock: "); 		break;
	case 12:	sym = _T("TCGChnlGainDelay: "); 	break;
	case 13:	sym = _T("SetPRF: "); 				break;
		}

	memset(&m_TcgCmd, 0, sizeof(ST_SET_TCG_DELAY_CMD));
	m_TcgCmd.Head.wMsgID = nCmd;
	m_TcgCmd.Head.wByteCount = 32;
	m_TcgCmd.Head.uSync = SYNC;
//		m_TcgCmd.Head.wMsgSeqCnt;	SET BY SENDING ROUTINE
	m_TcgCmd.Head.bPapNumber = nPap;
	m_TcgCmd.Head.bBoardNumber = nBoard;
	m_TcgCmd.bSeq = nSeq;
	m_TcgCmd.bChnl = nCh;
	m_TcgCmd.bGateNumber = nGate;
	m_TcgCmd.wDelay = nValue;	// called delay but now can be one of many
	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, Seq=%d, Ch=%d, Gate=%d, Value=%5d\n"),
		m_TcgCmd.Head.wMsgID, m_TcgCmd.Head.wByteCount, m_TcgCmd.Head.bPapNumber,
		m_TcgCmd.Head.bBoardNumber, m_TcgCmd.bSeq, m_TcgCmd.bChnl,
		m_TcgCmd.bGateNumber, m_TcgCmd.wDelay);
	t = sym + s;
	m_lbOutput.AddString(t);
	SendMsg((GenericPacketHeader*)&m_TcgCmd);
	}

// Command ID = 1 generates a request for the NIOS instrument to create fake data and
// send to PAP

// For fake data, the sequence number will set the starting sequence point for fake data.
// if sequence == 0 Fake data will continue from where it was.
// Use m_Gate as only the cmd ID and the Seq number matter for fake data.
// Of course the Pap/Board steering variables determine where the command will go.
// nValue will is the modulo of the sequence count
// nValue = 4 -> [0,1,2,3]
// 2017-10-04  Fake data generation will send 1088 bytes but will only send
// multiples of the modulus. If mod 3, then only 30 of the 32 Seq will be valid
// SEQ_DATA Seq[(32 / modulus)*modulo] are valid. For modulo 3:
// (32/3) = 10, 10*3 = 30. only 30 valid sequences in fake data packet
// IDATA_FROM_HW.bSeqPerPacket = 30
void CNcNx::FakeData(int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, int nValue )
	{
	CString s, t, sym;
	sym = _T("Generate Fake Data: ");
	memset(&m_GateCmd, 0, sizeof(ST_GATE_DELAY_CMD));
	m_GateCmd.Head.wMsgID = 1;	// nCmd;	// 1 is assigned to Fake data
	m_GateCmd.Head.wByteCount = 32;
	m_GateCmd.Head.uSync = SYNC;
//		m_GateCmd.Head.wMsgSeqCnt;	SET BY SENDING ROUTINE
	m_GateCmd.Head.bPapNumber = nPap;
	m_GateCmd.Head.bBoardNumber = nBoard;
	m_GateCmd.bSeq = nSeq & 0x1f;	// starting seq number
	m_GateCmd.bChnl = nCh;	// does not matter
	m_GateCmd.bGateNumber = nGate;	// does not matter
	if (nValue > 32)	nValue = 32;
	m_GateCmd.bSpare = nValue; // the last valid seq number before restart + 1.. the modulo
	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, Start_Seq#=%d, (Param)SeqModulo = %d\n"),
		m_GateCmd.Head.wMsgID, m_GateCmd.Head.wByteCount, m_GateCmd.Head.bPapNumber,
		m_GateCmd.Head.bBoardNumber, m_GateCmd.bSeq, m_GateCmd.bSpare);
	t = sym + s;
	m_lbOutput.AddString(t);
	SendMsg((GenericPacketHeader*)&m_GateCmd);
	}

// WordCmd, use nValue to assign to hardware functions
void CNcNx::WordCmd(int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, int nValue)
	{
	CString s, t, sym;
	sym = _T("WORD Cmd: ");
	memset(&m_WordCmd, 0, sizeof(ST_WORD_CMD));
	m_WordCmd.Head.wMsgID = nCmd;
	switch (nCmd)
		{
	case SET_ASCAN_SCOPE_ID:		sym = _T("SET_ASCAN_SCOPE ");		break;	//14
	case SET_ASCAN_SCOPE_DELAY_ID:	sym = _T("SET_ASCAN_SCOPE_DELAY ");	break;	//15
	case SET_ASCAN_PEAK_MODE_ID:	sym = _T("SET_ASCAN_PEAK_MODE ");	break;	//16
	case SET_ASCAN_RF_BEAM_ID:		sym = _T("SET_ASCAN_RF_BEAM ");		break;	//17
		}
	m_WordCmd.Head.wByteCount = 32;
	m_WordCmd.Head.uSync = SYNC;
	//		m_WordCmd.Head.wMsgSeqCnt;	SET BY SENDING ROUTINE
	m_WordCmd.Head.bPapNumber = nPap;
	m_WordCmd.Head.bBoardNumber = nBoard;
	m_WordCmd.bSeq = nSeq & 0x1f;	// starting seq number
	m_WordCmd.bChnl = nCh;	// does not matter
	m_WordCmd.bGateNumber = nGate;	// does not matter
	m_WordCmd.wCmd = nValue;
	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, Start_Seq#=%d, (Param)WordCmd = %d\n"),
		m_WordCmd.Head.wMsgID, m_WordCmd.Head.wByteCount, m_WordCmd.Head.bPapNumber,
		m_WordCmd.Head.bBoardNumber, m_WordCmd.bSeq, m_WordCmd.wCmd);
	t = sym + s;
	m_lbOutput.AddString(t);
	SendMsg((GenericPacketHeader*)&m_WordCmd);
	}

// Putw wValue into all 512 large command  words wCmd[]
void CNcNx::LargeCmd(int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, WORD wValue)
	{
	CString s, t, sym;
	int i;
	sym = _T("LARGE Cmd: ");
	memset(&m_wLargeCmd, 0, sizeof(ST_LARGE_CMD));
	m_wLargeCmd.wMsgID = nCmd;
	switch (nCmd)
		{
		case SEQ_TCG_GAIN_CMD_ID:			
			sym = _T("SEQ_TCG_GAIN_CMD ");			break;	//0x202
		case TCG_GAIN_CMD_ID:				
			sym = _T("TCG_GAIN_CMD ");				break;	//0x203
		case SET_ASCAN_BEAMFORM_DELAY_ID:	
			sym = _T("SET_ASCAN_BEAMFORM_DELAY ");	break;	//0x204
		}
	m_wLargeCmd.wByteCount = sizeof(ST_LARGE_CMD);
	m_wLargeCmd.uSync = SYNC;
	//		m_wLargeCmd.Head.wMsgSeqCnt;	SET BY SENDING ROUTINE
	m_wLargeCmd.bPAPNumber = nPap;
	m_wLargeCmd.bBoardNumber = nBoard;
	m_wLargeCmd.bSeqNumber = nSeq & 0x1f;	// starting seq number
	for ( i = 0; i < 512; i++)
		m_wLargeCmd.wCmd[i] = wValue;

	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, (Param)WordCmd[0] = %d\n"),
		m_wLargeCmd.wMsgID, m_wLargeCmd.wByteCount, m_wLargeCmd.bPAPNumber,
		m_wLargeCmd.bBoardNumber, m_wLargeCmd.wCmd[0]);
	t = sym + s;
	m_lbOutput.AddString(t);
	SendMsg((GenericPacketHeader*)&m_wLargeCmd);
	}

void CNcNx::OnChangeEdParam()
	{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (m_nPopulated)
		m_nParam = m_spParam.GetPos();
	}


void CNcNx::IncrementAscanCnt(void)
	{
	SetDlgItemInt(IDC_EN_ASCANCNT, guAscanMsgCnt, 0);
	}



void CNcNx::OnBnClickedRbSmallcmd()
	{
	// TODO: Add your control notification handler code here
	// fILL COMBO box with small commands-- 32 byte commands
	m_nShowSmallCmds = 1;
	PopulateCmdComboBox();
	}


void CNcNx::OnBnClickedRbLargecmds()
	{
	// Fill COMBO box with large commands-- 1056 byte commands
	m_nShowSmallCmds = 0;
	PopulateCmdComboBox();
	}