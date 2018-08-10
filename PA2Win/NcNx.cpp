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
, m_nRecordLabel(0)
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
#ifdef I_AM_PAG
	ON_BN_CLICKED(IDC_RB_SMALLCMD, &CNcNx::OnBnClickedRbSmallcmd)
	ON_BN_CLICKED(IDC_RB_LARGECMDS, &CNcNx::OnBnClickedRbLargecmds)
	ON_BN_CLICKED(IDC_BN_RECORD, &CNcNx::OnBnClickedBnRecord)
#endif
	ON_BN_CLICKED(IDC_RB_PULSERCMD, &CNcNx::OnBnClickedRbPulsercmd)
END_MESSAGE_MAP()


// CNcNx message handlers

void CNcNx::DebugOut(CString s)
	{
	m_lbOutput.AddString( s );
	gDlg.pUIDlg->SaveDebugLog( s );
	}

#define PAP_MAX		3
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
	m_nRecordState = 0;
#ifdef I_AM_PAG
	SetDlgItemText(IDC_BN_RECORD, _T("RECORD"));
	// if this is the PAG
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
#ifdef I_AM_PAG
	OnBnClickedRbSmallcmd();	// calls 	PopulateCmdComboBox();
	CButton* pButton = (CButton*)GetDlgItem(IDC_RB_SMALLCMD);
	pButton->SetCheck(true);
#endif

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
	m_nBoard = GetSpinValue( pNMUpDown, &m_spBoard );
	}


void CNcNx::OnDeltaposSpPap( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	// must use vertical scroll to retrieve the new value after the button spins
	*pResult = 0;
	m_nPAP = GetSpinValue( pNMUpDown, &m_spPap );
	}


void CNcNx::OnDeltaposSpSeq( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_nSeq = GetSpinValue( pNMUpDown, &m_spSeq );
	}


void CNcNx::OnDeltaposSpCh( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_nCh = GetSpinValue( pNMUpDown, &m_spCh );
	}


void CNcNx::OnDeltaposSpGate( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
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
	switch (m_nWhichTypeCmd)
		{
	case 0:	// large
	default:
		// show large commands
		s.Format(_T("TCG_BEAM_GAIN"));			m_cbCommand.AddString(s);	//0x200 +4
		s.Format(_T("TCG_GAIN_CMD"));			m_cbCommand.AddString(s);	//0x200 +5
		m_cbCommand.SetCurSel(0);
		break;
	case 1:	// small
			//s.Format(_T("null 0"));				m_cbCommand.AddString(s);
		s.Format(_T("Debug FIFO"));			m_cbCommand.AddString(s);	//0
		s.Format(_T("FakeData"));			m_cbCommand.AddString(s);	//1
		s.Format(_T("Gate n Delay"));		m_cbCommand.AddString(s);	//2
		s.Format(_T("Gate n Range"));		m_cbCommand.AddString(s);	//3
		s.Format(_T("Gate n Blank"));		m_cbCommand.AddString(s);	//4
		s.Format(_T("Gate n Thold"));		m_cbCommand.AddString(s);	//5
		s.Format(_T("Gate n Trigger"));		m_cbCommand.AddString(s);	//6
		s.Format(_T("Gate n Polarty"));		m_cbCommand.AddString(s);	//7
		s.Format(_T("Gate n TOF"));			m_cbCommand.AddString(s);	//8
		s.Format(_T("null TCGChnlTrigger"));m_cbCommand.AddString(s);	//9  TCGProbeTrigger T("TCGChnlTrigger")
		s.Format(_T("TCGGainClock"));		m_cbCommand.AddString(s);	//10
		s.Format(_T("TCGChnlGainDelay"));	m_cbCommand.AddString(s);	//11
		s.Format(_T("ProcNull"));			m_cbCommand.AddString(s);	//12
		s.Format(_T("null ReadBack"));		m_cbCommand.AddString(s);	//13
		s.Format(_T("SetTcgClockRate"));	m_cbCommand.AddString(s);	//14
		s.Format(_T("TCGTriggerDelay"));	m_cbCommand.AddString(s);	//15
		s.Format(_T("Powr2Gain"));			m_cbCommand.AddString(s);	//16
		s.Format(_T("ProcNull"));			m_cbCommand.AddString(s);	//17
		s.Format(_T("ProcNull"));			m_cbCommand.AddString(s);	//18
		s.Format(_T("ProcNull"));			m_cbCommand.AddString(s);	//19
		s.Format(_T("ProcNull"));			m_cbCommand.AddString(s);	//20

		s.Format(_T("ASCAN_SAMPLE_RATE"));		m_cbCommand.AddString(s);	//21 How wide the scope trace
		s.Format(_T("ASCAN_SCOPE_DELAY"));		m_cbCommand.AddString(s);	//22 delays trigger
		s.Format(_T("SET_ASCAN_WaveForm"));		m_cbCommand.AddString(s);	//23  data [0..4]
		s.Format(_T("ASCAN_RF_BEAM_SELECT"));	m_cbCommand.AddString(s);	//24--check case statements below
		s.Format(_T("ASCAN_BEAM_SEQ"));			m_cbCommand.AddString(s);	//25 SetAscanSeqBeamReg
		s.Format(_T("ASCAN_GATE_OUTPUT"));		m_cbCommand.AddString(s);	//26
		s.Format(_T("ASCAN_REP_RATE"));			m_cbCommand.AddString(s);	//27
		s.Format(_T("WallNx"));					m_cbCommand.AddString(s);	//28
		s.Format(_T("TcgBeamGainAll"));			m_cbCommand.AddString(s);	//29
		s.Format(_T("ReadBack"));				m_cbCommand.AddString(s);	//30

		m_cbCommand.SetCurSel(2);
		break;
	case 2:	// pulser
		s.Format(_T("PULSER_PRF"));				m_cbCommand.AddString(s);	// 0+300h
		s.Format(_T("HV_ON_OFF"));				m_cbCommand.AddString(s);	// 1+300h
		s.Format(_T("PULSE_POLARITY"));			m_cbCommand.AddString(s);	// 2+300h
		s.Format(_T("PULSE_SHAPE"));			m_cbCommand.AddString(s);	// 3+300h
		s.Format(_T("PULSE_WIDTH"));			m_cbCommand.AddString(s);	// 4+300h
		s.Format(_T("SEQUENCE_LEN"));			m_cbCommand.AddString(s);	// 5+300h
		s.Format(_T("SOCOMATE_SYNC"));			m_cbCommand.AddString(s);	// 6+300h
		m_cbCommand.SetCurSel(0);
		break;

		}
	m_nPopulated = 1;
	}

void CNcNx::OnCbnSelchangeCbCmds()
	{
	CString s, t;
	int nCmdOffset;
	int nCmdLarge = 0;
	// TODO: Add your control notification handler code here
	m_nCmdId = m_cbCommand.GetCurSel();
	switch (m_nWhichTypeCmd)
		{
	case 0:	// large
		nCmdOffset = 0x200;
		m_nCmdId += 4;
		nCmdLarge = m_nCmdId + nCmdOffset;	// 1st 4 large are proc null
		break;
	case 1:	// small
		nCmdOffset = 0;
		break;
	case 2:	// pulser
		nCmdOffset = 0x300;
		break;
		}

	
	t.Format(_T("m_nCmdId = %d"), m_nCmdId + nCmdOffset);

	if ((0x300 <= (m_nCmdId + nCmdOffset)) && (m_nCmdId < TOTAL_PULSER_COMMANDS))
		{
		switch (m_nCmdId + nCmdOffset)
			{
			case 0 + 0x300:
			case 1 + 0x300:
				PulserCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, (m_nCmdId + nCmdOffset), (WORD)m_nParam);
				break;
			default:
				PulserCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, (m_nCmdId + nCmdOffset), (WORD)m_nParam);
				break;
			}
		}

	else if ((0x200 <= (m_nCmdId + nCmdOffset)) && (m_nCmdId < TOTAL_LARGE_COMMANDS))
		{
		switch (m_nCmdId + nCmdOffset)
			{
		case 0:
#ifdef I_AM_PAG
			DebugFifo(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
#endif

			case 2 + 0x200:
			case 3 + 0x200:
			case 4 + 0x200:
			case 5 + 0x200:
				// build command here
				LargeCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, nCmdLarge, (WORD)m_nParam);
				t = _T("Large Command");
				break;
			default:
				break;
			}
		}

	else
		{
		switch (m_nCmdId + nCmdOffset)
			{
			//case 0:	s.Format(_T("null %d"), m_nCmdId);	break;
			case 0:	s.Format(_T("Debug FIFO"));	break;
			case 1:	s = _T("Fake Data");					break;
			case 2: s.Format(_T("Gate %d Delay %d"), m_nGate, m_nParam); break;
			case 3: s.Format(_T("Gate %d Range %d"), m_nGate, m_nParam); break;
			case 4: s.Format(_T("Gate %d Blank %d"), m_nGate, m_nParam); break;
			case 5: s.Format(_T("Gate %d Thold %d"), m_nGate, m_nParam); break;
			case 6: s.Format(_T("Gate %d Trigger %d"), m_nGate, m_nParam); break;
			case 7: s.Format(_T("Gate %d Polarity %d"), m_nGate, m_nParam); break;
			case 8: s.Format(_T("Gate %d TOF %d"), m_nGate, m_nParam); break;
			case 9: s.Format(_T("Nx = %d"), m_nParam);				break;
			case 13: s.Format(_T("ReadBk SubCmd %d"), m_nParam);	break;
			case 0x204: s = _T("TCG_BEAM_GAIN");					break;
			case 0x205: s = _T("TCG_SEQ_GAIN");						break;
			default:	s = t;		break;
			}
		//m_lbOutput.AddString( s );


		if (m_nCmdId + nCmdOffset < 14)
			{
			switch (m_nCmdId + nCmdOffset)
				{
				case 0:
#ifdef I_AM_PAG
					DebugFifo(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
#endif

					break;
				case 1:
					FakeData(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
					break;
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					GateCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
					break;
					// TCG commands
				case 9:
					WallNxCmd(m_nPAP, m_nBoard, m_nSeq, m_nCmdId, m_nParam);
					break;
					// TCG commands
				case 10:
				case 11:
				case 12:
					TcgCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
					break;
				case 13:
					ReadBackCmd(m_nPAP, m_nBoard, m_nCmdId, m_nParam);
				default:
					break;
				}
			}

		else if (m_nCmdId + nCmdOffset < TOTAL_COMMANDS)
			{
			WordCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
			}
		else
			{
			TRACE(_T("unknown command\n"));
			}
		}


	}

void CNcNx::OnBnClickedBnDonothing()
	{
	// TODO: Add your control notification handler code here
	// Does nothing but take focus off of OK and Cancel
	// In Resource view use Properties to set this as the default button
	int i;
	// Break here and uses watches to view Idata and Ascan data from PAP
	i = m_nParam;
	}


// Message goes to one specific PAP and one specific board in the PAP
// 2016-06-27 we will assume that messages will be built by the routine which configures the
// parameters of the message. This is a departure from the way we have done this for a couple
// of decades. Allocate memory to the message here to be put into a linked list.  Call the main
// dialog to actually add the message to the linked list so the message sequence number can be attached.
//
// All messages go to PAP client 0. Not how originally planned, but useful for this All Wall
// phased array machine
void CNcNx::SendMsg(GenericPacketHeader *pMsg)//, int nChTypes)
	{
#ifdef I_AM_PAG
	CString s;
	if (pMsg->wMsgID >= 0x300 + LAST_PULSER_COMMAND)
		{
		s.Format(_T("Pulser command 0x%0x is invalid... deleting\n"), pMsg->wMsgID);
		TRACE(s);
		delete pMsg;
		return;
		}

	if (pMsg->wMsgID >= 0x300)
		{ 
		ST_SMALL_CMD *pSmall = (ST_SMALL_CMD *)pMsg;
		ST_SMALL_CMD *pSend = new ST_SMALL_CMD;
		if (pMsg->wByteCount > 32) pMsg->wByteCount = 32;
		memset((void*)pSend, 0, sizeof(ST_SMALL_CMD));
		memcpy((void*)pSend, (void*)pSmall, pMsg->wByteCount);
		gDlg.pUIDlg->SendMsgToPAP((int)pSend->bPAPNumber, pSend->wMsgID, (void *)pSend);
		return;
		}

	if (pMsg->wMsgID >= 0x200 + LAST_LARGE_COMMAND)
		{
		s.Format(_T("Large ADC command 0x%0x is invalid... deleting\n"), pMsg->wMsgID);
		TRACE(s);
		delete pMsg;
		return;
		}
	if (pMsg->wMsgID >= 0x200)
		{
		ST_LARGE_CMD *pSend = new (ST_LARGE_CMD);
		if (pMsg->wByteCount > sizeof(ST_LARGE_CMD)) 
			pMsg->wByteCount = sizeof(ST_LARGE_CMD);
		memset((void*)pSend, 0, sizeof(ST_LARGE_CMD));
		memcpy((void*)pSend, (void*)pMsg, pMsg->wByteCount);
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
		case 0x204:	// TCGBeamGain
			s.Format(_T("TCGBeamGain PAP=%d, Board=%d\n"), pSend->bPAPNumber, pSend->bBoardNumber);
			TRACE(s);
			break;
		case 0x205:	// SetSeqTCGGain
			s.Format(_T("SetSeqTCGGain PAP=%d, Board=%d\n"), pSend->bPAPNumber, pSend->bBoardNumber);
			break;

		default:
			TRACE(_T("Unrecognized Large message .. delete pSend\n"));
			delete pSend;
			return;
			}

		gDlg.pUIDlg->SendMsgToPAP((int)pSend->bPAPNumber, pSend->wMsgID, (void *)pSend);
		//delete pSend; auto deleted in sending function
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
			{	// largish command format
			ST_LARGE_CMD *pSend = new ST_LARGE_CMD;
			memset((void*)pSend, 0, sizeof(ST_LARGE_CMD));
			memcpy((void*)pSend, (void*)pMsg, pMsg->wByteCount);
			gDlg.pUIDlg->SendMsgToPAP((int)pMsg->bPapNumber, pMsg->wMsgID, (void *)pSend);
			//delete pSend; auto deleted in sending function
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

void CNcNx::WallNxCmd(int nPap, int nBoard, int nSeq, int nCmd, int nValue)
	{
	CString s;
	memset(&m_NxCmd, 0, sizeof(ST_NX_CMD));
	m_NxCmd.wMsgID = SET_WALL_NX_CMD_ID;		// 9;
	m_NxCmd.wByteCount = 32;
	m_NxCmd.uSync = SYNC;
	m_NxCmd.bPAPNumber = nPap;
	m_NxCmd.bBoardNumber = nBoard;
	// change nValue assignment from wall thick to Nx
	nValue = nValue % 10;
	if (nValue == 0) nValue = 1;
	m_NxCmd.wNx = nValue;
	m_NxCmd.wMax = 1200 + nValue;
	m_NxCmd.wMin = 200 + nValue;
	m_NxCmd.wDropCount = 10 + nValue;
	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, Nx=%d, Max=%d, Min=%d Drop=%5d  nValue modifies Max,Min,drop\n"),
		m_NxCmd.wMsgID, m_NxCmd.wByteCount, m_NxCmd.bPAPNumber, m_NxCmd.bBoardNumber, nValue,
		m_NxCmd.wMax, m_NxCmd.wMin, m_NxCmd.wDropCount);
	m_lbOutput.AddString(s);
	SendMsg((GenericPacketHeader*)&m_NxCmd);
	}


void CNcNx::TcgCmd( int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, int nValue )
	{
	CString s, t, sym;
	switch (nCmd)
		{
	default:		sym = _T( "???" );				return;
	case 10:		sym = _T("TcgClockRate: "); 		break;
	case 11:	sym = _T("TCGBeamGainDelay: "); 		break;
	case 12:	sym = _T("TCGBeamGainAll: : "); 		break;
	case 14:	sym = _T("TCGClockRate: "); 			break;
	case 15:	sym = _T("TCGTriggerDelay: "); 				break;
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

// Readback has sub commands. Top read back ID is 13
// nRbId is nValue from NcNx parameters
void CNcNx::ReadBackCmd(int nPap, int nBoard, int nCmd, int nValue)
	{
	CString s, t, sym;
	memset(&m_RdBkCmd, 0, sizeof(ST_SET_TCG_DELAY_CMD));
	m_RdBkCmd.Head.wMsgID = nCmd;	 // 13;
	m_RdBkCmd.Head.wByteCount = 32;
	m_RdBkCmd.Head.uSync = SYNC;
	//		m_TcgCmd.Head.wMsgSeqCnt;	SET BY SENDING ROUTINE
	m_RdBkCmd.Head.bPapNumber = nPap;
	m_RdBkCmd.Head.bBoardNumber = nBoard;
	m_RdBkCmd.wReadBackID = nValue;	// this is the nValue parameter

	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, RdBkCmd = 13, RdBkID = %2d\n"),
		m_RdBkCmd.Head.wMsgID, m_RdBkCmd.Head.wByteCount, m_RdBkCmd.Head.bPapNumber,
		m_RdBkCmd.Head.bBoardNumber, m_RdBkCmd.wReadBackID);
	t = sym + s;
	m_lbOutput.AddString(t);
	SendMsg((GenericPacketHeader*)&m_RdBkCmd);
	}


// Command ID = 1 generates a request for the NIOS instrument to create fake data and
// send to PAP

// For fake data, the sequence number will set the starting sequence point for fake data.
// if sequence == 0 Fake data will continue from where it was.
// Use m_Gate as only the cmd ID and the Seq number matter for fake data.
// Of course the Pap/Board steering variables determine where the command will go.
// nValue will is the modulo of the sequence count
// nValue = 4 -> [0,1,2,3]
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


// Debugging PAP Fifo operation when multiple commands sent in a few packets
// Seem to repeat a command.. probably doesn't miss command
// cmd sequence 25x8, 24, 23, 22, 21, 12*3, 0x205x3, 0x204x2  25 sent 8 times in one packet
// from Robert, results in about 5 total TCPIP packets
// Add AscanRepRate to end of command
#ifdef I_AM_PAG
void CNcNx::DebugFifo(int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, WORD wValue)
	{
	ST_GATE_DELAY_CMD sml[16];
	ST_LARGE_CMD lrg[6];
	ST_ASCAN_PERIOD_CMD *pCmd;
	int i, j, k;
	for (i = 0; i < 16; i++)
		{
		sml[i].Head.bPapNumber = 0;	// always for testing
		sml[i].Head.bBoardNumber = nBoard;
		sml[i].Head.wByteCount = 32;
		sml[i].Head.uSync = SYNC;
		sml[i].bSeq = nSeq;
		sml[i].bChnl = nCh;
		sml[i].bGateNumber = nGate;
		sml[i].wDelay = wValue;
		switch (i)
			{
		default:
			sml[i].Head.wMsgID = 25;		break;	// 1st 8 commands
			case 8:	 sml[i].Head.wMsgID = 24;	break;
			case 9:	 sml[i].Head.wMsgID = 23;	break;
			case 10: sml[i].Head.wMsgID = 22;	break;
			case 11: sml[i].Head.wMsgID = 21;	break;
			case 12:
			case 13:
			case 14: sml[i].Head.wMsgID = 12;	break;	// 3 12's
			case 15: 
				sml[i].Head.wMsgID = 27;		//AScan Rep Rate
				pCmd = (ST_ASCAN_PERIOD_CMD *)&sml[i];
				pCmd->wPeriod = 40;	// 40 ms
				break;
			}
		}	// for (i = 0; i < 16; i++)

	for (i = 0; i < 5; i++)
		{
		lrg[i].wByteCount = sizeof(ST_LARGE_CMD);
		lrg[i].uSync = SYNC;
		lrg[i].bPAPNumber = 0;
		lrg[i].bBoardNumber = nBoard;
		lrg[i].bSeqNumber = nSeq;

		if (i < 3)
			{
			lrg[i].wMsgID = 0x205;
			for (j = 0; j < 128; j++)
					lrg[i].wCmd[j] = wValue;	// 128 words
		}
		else
			{
			lrg[i].wMsgID = 0x204;
			for (j = 0; j < 128; j++)
				lrg[i].wCmd[j] = wValue/2;	// 128 words
			}
		}

	// send small commands
	// 03/05/18 Send th block 8 times in a row
	for (k = 0; k < 8; k++)
		{
		for (i = 0; i < 16; i++)
			{
			SendMsg((GenericPacketHeader *)&sml[i]);
			}

		// send small commands
		for (i = 0; i < 5; i++)
			{
			SendMsg((GenericPacketHeader *)&lrg[i]);
			}
		}
	}

#endif


// WordCmd, use nValue to assign to hardware functions,, nValue is usually m_nParam
void CNcNx::WordCmd(int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, int nValue)
	{
	CString s, t, sym;
	sym = _T("WORD Cmd: ");
	memset(&m_WordCmd, 0, sizeof(ST_WORD_CMD));
	m_WordCmd.Head.wMsgID = nCmd;
	switch (nCmd)
		{
	case 21:			sym = _T("ASCAN_SAMPLE_RATE ");			break;	//14
	case 22:			sym = _T("ASCAN_SCOPE_DELAY ");			break;	//15
	case 23:			sym = _T("SelectAscanWaveForm,	 ");	break;	//16
	case 24:			sym = _T("AscanRfBeamSelect,	 ");	break;	//17
	case 25:			sym = _T("AscanSeqBeamReg,	 ");		break;	//18
	case 26:			sym = _T("ASCAN_GATE_OUTPUTS ");		break;	//19
	case 27:			sym = _T("ASCAN_REP_RATE ");			break;
	//case NIOS_SCOPE_CMD_ID:			sym = _T("NIOS_SCOPE_CMD ");		break;	//20
		
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
			sym = _T("TCG_SEQ_GAIN_CMD ");			break;	//0x205
		case TCG_GAIN_CMD_ID:				
			sym = _T("TCG_BEAM_GAIN_CMD ");				break;	//0x204
#if 0
		case SET_ASCAN_BEAMFORM_DELAY_ID:	
			sym = _T("SET_ASCAN_BEAMFORM_DELAY ");	break;	//0x204
#endif
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

// Pulser Command
void CNcNx::PulserCmd(int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, WORD wValue)
	{
	CString s, t, sym;
	sym = _T("Pulser Cmd: ");
	memset(&m_WordCmd, 0, sizeof(ST_WORD_CMD));
	m_WordCmd.Head.wMsgID = nCmd;
	switch (nCmd)
		{
		case 0+0x300:			sym = _T("PULSER PRF ");				break;	
		case 1+0x300:			sym = _T("HV_ON_OF ");					break;
		case 2+0x300:			sym = _T("PULSE_POLARITY ");			break;
		case 3+0x300:			sym = _T("PULSE_SHAPE ");				break;
		case 4 + 0x300:			sym = _T("PULSE_WIDTH ");				break;
		case 5 + 0x300:			sym = _T("SEQUENCE_LEN ");				break;
		case 6 + 0x300:			sym = _T("SOCOMATE_SYNC ");				break;
		default:				sym = _T("UNKNOWN CMD ");				break;

		}
	m_WordCmd.Head.wByteCount = 32;
	m_WordCmd.Head.uSync = SYNC;
	//		m_WordCmd.Head.wMsgSeqCnt;	SET BY SENDING ROUTINE
	m_WordCmd.Head.bPapNumber = nPap;
	m_WordCmd.Head.bBoardNumber = nBoard;
	m_WordCmd.bSeq = nSeq & 0x1f;	// starting seq number
	m_WordCmd.bChnl = nCh;	// does not matter
	m_WordCmd.bGateNumber = nGate;	// does not matter
	m_WordCmd.wCmd = wValue;
	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, Start_Seq#=%d, (Param)WordCmd = %d\n"),
		m_WordCmd.Head.wMsgID, m_WordCmd.Head.wByteCount, m_WordCmd.Head.bPapNumber,
		m_WordCmd.Head.bBoardNumber, m_WordCmd.bSeq, m_WordCmd.wCmd);
	t = sym + s;
	m_lbOutput.AddString(t);

	SendMsg((GenericPacketHeader*)&m_WordCmd);
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
	m_nWhichTypeCmd = 1;
	PopulateCmdComboBox();
	}


void CNcNx::OnBnClickedRbLargecmds()
	{
	// Fill COMBO box with large commands-- 1056 byte commands
	m_nWhichTypeCmd = 0;
	PopulateCmdComboBox();
	}



void CNcNx::OnBnClickedRbPulsercmd()
	{
	m_nWhichTypeCmd = 2;
	PopulateCmdComboBox();
	}

#ifdef I_AM_PAG
void CNcNx::OnBnClickedBnRecord()
	{
	// TODO: Add your control notification handler code here
	TCHAR szFilter[] = _T("AllWall Files(*.awd) | *.awd||");
	CString s;
	char t[32];

	CFileDialog dlg(TRUE, _T("cfg"), _T("*.cfg"), OFN_OVERWRITEPROMPT,
		szFilter);

	//	if (strlen(m_szDefCfgDir)) dlg.m_ofn.lpstrInitialDir = m_szDefCfgDir;
	//if (m_szDefCfgDir.GetLength()) dlg.m_ofn.lpstrInitialDir = m_szDefCfgDir;
	if (m_nRecordState == 0)
		{
		if (dlg.DoModal() == IDOK)
			{
			s = dlg.GetPathName();
			try
				{
				m_AllWallFile.Open(s, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone);
				// when this procedure closes, it will close the open file
				m_AllWallFileName = s;
				m_AllWallFile.SeekToBegin();
				m_nRecordState = 1;
				SetDlgItemText(IDC_BN_RECORD, _T("SAVE & CLOSE"));
				}
			catch (CFileException* e)
				{
				e->ReportError();
				e->Delete();
				}

			}
		}
	else
		{
		if (m_AllWallFile.GetLength() == 0)
			{
			strcpy(t, "End");
			m_AllWallFile.Write(t, strlen(t));
			}
		m_AllWallFile.Flush();
		m_AllWallFile.Close();
			// save the file and close
		SetDlgItemText(IDC_BN_RECORD, _T("RECORD"));
		m_nRecordState = 0;
		}

	}
#endif



