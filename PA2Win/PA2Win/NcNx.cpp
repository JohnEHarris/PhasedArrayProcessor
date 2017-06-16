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
	{
	m_DlgLocationKey = _T("NC_NX_PA2");
	m_DlgLocationSection = _T("Dialog Locations");	// Section is always this string for all dlgs
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
END_MESSAGE_MAP()


// CNcNx message handlers

void CNcNx::DebugOut(CString s)
	{
	m_lbOutput.AddString( s );
#if 0
	char txt[1024];
	CstringToChar(s,txt);
	strcat(txt,"\r\n");
	DebugFile.Write(txt, strlen(txt));
#endif

	}

#define PAP_MAX		0
#define BOARD_MAX	2
#define GATE_MAX	3
#define PARAM_MAX	5000

BOOL CNcNx::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	PositionWindow();
	m_nPAP = m_nBoard = m_nSeq = m_nCh = m_nGate = m_nParam	= 0;
	// if this is the PAG
#ifdef I_AM_PAG
	m_spPap.SetRange( 0, PAP_MAX );	// gnMaxClientsPerServer - 1 );  //how many clients do I have
	m_spBoard.SetRange( 0, BOARD_MAX );	// gnMaxClients - 1 );		// how many clients does my client have
	m_spSeq.SetRange( 0, gMaxSeqCount-1 );		// number of sequence in firing scheme
	m_spCh.SetRange( 0, gMaxChnlsPerMainBang-1 );		// number of channel in each sequence
	m_spGate.SetRange( 0, 3 );		// number of gates in each channel
	m_spParam.SetRange( 1, PARAM_MAX );	// depends of command selected from list box
#else
	// I am the PAP
	m_spPap.SetRange( 0, 0 );  //how many clients do I have
	m_spInst.SetRange( 0, gnMaxClients-1 );		// how many clients does my client have
	m_spSeq.SetRange( 0, gMaxSeqCount-1 );		// number of sequence in firing scheme
	m_spCh.SetRange( 0, gMaxChnlsPerMainBang-1 );		// number of channel in each sequence
	m_spGate.SetRange( 0, 3 );	

#endif
	m_lbOutput.ResetContent();
	m_cbCommand.ResetContent();
	m_nPAP = m_nBoard = m_nSeq = m_nCh = m_nGate = m_nParam	= 0;

	PopulateCmdComboBox();
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
	}

// Fill the combo box with strings whose index in the combobox matches the 
// command ID found in Cmds.h
// NcNx is a test command. Will likely never exist for real system
// Short commands have cmd ID < 0x200
void CNcNx::PopulateCmdComboBox()
	{
	CString s;
	m_cbCommand.ResetContent();

	s.Format( _T( "null 0" ) );				m_cbCommand.AddString( s );
	s.Format( _T( "null 1" ) );				m_cbCommand.AddString( s );
	s.Format( _T( "Gate n Delay" ) );		m_cbCommand.AddString( s );
	s.Format( _T( "Gate n Range" ) );		m_cbCommand.AddString( s );
	s.Format( _T( "Gate n Blank" ) );		m_cbCommand.AddString( s );
	s.Format( _T( "Gate n Thold" ) );		m_cbCommand.AddString( s );
	s.Format( _T( "Gate n Trigger" ) );		m_cbCommand.AddString( s );
	s.Format( _T( "Gate n Polarty" ) );		m_cbCommand.AddString( s );
	s.Format( _T( "Gate n TOF" ) );			m_cbCommand.AddString( s );

	}

void CNcNx::OnCbnSelchangeCbCmds()
	{
	CString s, t;
	// TODO: Add your control notification handler code here
	m_nCmdId = m_cbCommand.GetCurSel();
	t.Format( _T( "m_nCmdId = %d" ), m_nCmdId );
	switch (m_nCmdId)
		{
		case 0:
		case 1:	s.Format( _T( "null %d" ), m_nCmdId );	break;
		case 2: s.Format(_T("Gate %d Delay %d"), m_nGate, m_nParam); break;
		case 3: s.Format(_T("Gate %d Range %d"), m_nGate, m_nParam); break;
		case 4: s.Format(_T("Gate %d Blank %d"), m_nGate, m_nParam); break;
		case 5: s.Format(_T("Gate %d Thold %d"), m_nGate, m_nParam); break;
		case 6: s.Format(_T("Gate %d Trigger %d"), m_nGate, m_nParam); break;
		case 7: s.Format(_T("Gate %d Polarity %d"), m_nGate, m_nParam); break;
		case 8: s.Format(_T("Gate %d TOF %d"), m_nGate, m_nParam); break;
		default:	s = t;		break;
		}
	//m_lbOutput.AddString( s );


	switch (m_nCmdId)
		{
		case 0:
		case 1:	
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
		default:	
			break;
		}	
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
	case 2:
		sym = _T("Delay: ");		break;
	case 3:
		sym = _T("Rng: ");			break;
	case 4:
		sym = _T("Blk: ");			break;
	case 5:
		sym = _T("Thl: ");			break;
	
	case 6:		// gate trigger source for all 4 gates
		sym = _T("Trg:"); 	
		// high nibble sets triger for all 4 gates, low nibble is enable/disable	
		// bit7,6,5,4:  trigger select (0:mbs, 1:threshold) for all gate 4-1
		// bit 3-0: gate enable
		break;
	case 7:		// gate data mode ie signal polarity
		sym = _T("Pol:"); 		break;
	case 8:		// gate data mode ie tof
		sym = _T("Tof:"); 		break;
	default:
		sym = _T( "???" );		return;
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


void CNcNx::OnBnClickedBnDonothing()
	{
	// TODO: Add your control notification handler code here
	// Does nothing but take focus off of OK and Cancel
	// In Resource view use Properties to set this as the default button
	}


// Message goes to one specific PAM and one specific instrument in the PAM
// 2016-06-27 we will assume that messages will be built by the routine which configures the
// parameters of the message. This is a departure from the way we have done this for a couple
// of decades. Allocate memory to the message here to be put into a linked list.  Call the main
// dialog to actually add the message to the linked list so the message sequence number can be attached.
//
void CNcNx::SendMsg(GenericPacketHeader *pMsg)//, int nChTypes)
	{
	//int nPam, nInst, nChnl;
	ST_LARGE_CMD *pSend;
	PAP_INST_CHNL_NCNX * pSendNcNx;
	if (pMsg->wMsgID >= 0x200)
		{
		switch (pMsg->wMsgID)
			{
		case 1+0x200:
			// development code special
			pSendNcNx = new PAP_INST_CHNL_NCNX;
			memset((void *)pSendNcNx, 0, sizeof(PAP_INST_CHNL_NCNX));
			memcpy((void *)pSendNcNx, (void *)pMsg, pMsg->wByteCount);	// sizeof(ST_NC_NX) * 72);
			pSendNcNx->bPAPNumber = m_nPAP;
			pSendNcNx->bBoardNumber = m_nBoard;
			pSend = (ST_LARGE_CMD *)pSendNcNx;
			gDlg.pUIDlg->SendMsgToPAP((int)pMsg->bPapNumber, pMsg->wMsgID, (void *)pSend);
			break;
		default:
			TRACE(_T("Unrecognized message in NcNx\n"));
			return;
			}
		}
	else
		{
		if (pMsg->wByteCount == 32)
			{	// small command format
			ST_SMALL_CMD *pSend = new ST_SMALL_CMD;
			memset((void*)pSend, 0, sizeof(ST_SMALL_CMD));
			memcpy((void*)pSend, (void*)pMsg, sizeof(ST_SMALL_CMD));
			gDlg.pUIDlg->SendMsgToPAP((int)pMsg->bPapNumber, pMsg->wMsgID, (void *)pSend);
			}
		else
			{	// large command format
			ST_LARGE_CMD *pSend = new ST_LARGE_CMD;
			memset((void*)pSend, 0, sizeof(ST_LARGE_CMD));
			memcpy((void*)pSend, (void*)pMsg, sizeof(ST_LARGE_CMD));
			gDlg.pUIDlg->SendMsgToPAP((int)pMsg->bPapNumber, pMsg->wMsgID, (void *)pSend);
			}
		}


	}
