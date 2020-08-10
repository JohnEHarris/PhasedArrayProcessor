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
, m_PulserCmdTxt(_T(""))
, m_SmallTxt(_T(""))
, m_LargeTxt(_T(""))
// Using ini file for dialog location, next items are keys for ini file
	{
	m_DlgLocationKey = _T("NC_NX_PA2");
	m_DlgLocationSection = _T("Dialog Locations");	// Section is always this string for all dlgs
	m_nPopulated = 0;
	}

CNcNx::~CNcNx()
	{
	TRACE( _T( "CNcNx::~CNcNx()\n" ) );
	gDlg.pNcNx = 0;
	//MFC\dlgcore.cpp(137) : AppMsg - 	OnDestroy or PostNcDestroy in derived class will not be called.
	}

void CNcNx::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SP_PAP, m_spPap);
	DDX_Control(pDX, IDC_SP_SEQ, m_spSeq);
	DDX_Control(pDX, IDC_SP_CH, m_spCh);
	DDX_Control(pDX, IDC_SP_GATE, m_spGate);
	DDX_Control(pDX, IDC_SP_PARAM, m_spParam);
	DDX_Control(pDX, IDC_LB_NCNX, m_lbOutput);
	DDX_Control(pDX, IDC_CB_CMDS, m_cbCommand);
	DDX_Control(pDX, IDC_SP_BOARD, m_spBoard);
	DDX_Control(pDX, IDC_BN_DONOTHING, m_bnDoNoting);
	DDX_Control(pDX, IDC_ED_PARAM, m_edParam);
	DDX_Control(pDX, IDC_EDSMAL_CMD, m_SmallCmdTxt);
	DDX_Control(pDX, IDC_EDLARGE_CMD, m_LargeCmdTxt);
	DDX_Text(pDX, IDC_EDSMAL_CMD, m_SmallTxt);
	DDX_Text(pDX, IDC_EDLARGE_CMD, m_LargeTxt);
	DDX_Control(pDX, IDC_CB_RDWHAT, m_cbReadWhat);
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
	ON_EN_CHANGE(IDC_ED_GATE, &CNcNx::OnEnChangeEdGate)
	ON_EN_CHANGE(IDC_ED_CH, &CNcNx::OnEnChangeEdCh)
	ON_EN_CHANGE(IDC_ED_SEQ, &CNcNx::OnEnChangeEdSeq)
	ON_CBN_SELCHANGE(IDC_CB_RDWHAT, &CNcNx::OnCbnSelchangeCbRdwhat)
END_MESSAGE_MAP()


// CNcNx message handlers

void CNcNx::DebugOut(CString s)
	{
	m_lbOutput.AddString( s );
	gDlg.pUIDlg->SaveDebugLog( s );
	}

#define PAP_MAX		7
#define BOARD_MAX	8
#define GATE_MAX	3
#define PARAM_MAX	20000

BOOL CNcNx::OnInitDialog()
	{
	CDialogEx::OnInitDialog();
	CString s;

	// TODO:  Add extra initialization here
	//PositionWindow();
	m_nPAP = m_nBoard = m_nSeq = m_nCh = m_nGate = m_nParam	= 0;
	guAscanMsgCnt = 0;
	guIdataMsgCnt = 0;
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
	m_spParam.SetRange( 0, PARAM_MAX );	// depends of command selected from list box

	m_lbOutput.ResetContent();
	m_cbCommand.ResetContent();
	m_nPAP = m_nBoard = m_nSeq = m_nCh = m_nGate = m_nParam	= 0;
#ifdef I_AM_PAG
	OnBnClickedRbSmallcmd();	// calls 	PopulateCmdComboBox();
	CButton* pButton = (CButton*)GetDlgItem(IDC_RB_SMALLCMD);
	pButton->SetCheck(true);
#endif

	m_cbCommand.SetCurSel ( 2 );	// Gate Delay


	// ReadWhat Combo
	m_cbReadWhat.ResetContent();
	s.Format(_T("GateCmds"));			m_cbReadWhat.AddString(s); // m_RdBkCmd.wReadBackID = 1;
	s.Format(_T("TCGBeamGain"));		m_cbReadWhat.AddString(s); // TCGBeamGain is cmd 0x204  m_RdBkCmd.wReadBackID = 2;
	s.Format(_T("TCGSeqGain"));			m_cbReadWhat.AddString(s); // TCGSeqGain  is cmd 0x205  m_RdBkCmd.wReadBackID = 3;
	m_cbReadWhat.SetCurSel(0);
	m_nReadBackWhat = 1;	// default to gates

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
	guIdataMsgCnt = 0;
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
		s.Format(_T("Gate n Polarity"));	m_cbCommand.AddString(s);	//7
		s.Format(_T("Gate n TOF"));			m_cbCommand.AddString(s);	//8
		s.Format(_T("SetChnlTrigger"));		m_cbCommand.AddString(s);	//9  TCGProbeTrigger T("TCGChnlTrigger")
		s.Format(_T("TCGGainClock"));		m_cbCommand.AddString(s);	//10
		s.Format(_T("TCGChnlGainDelay"));	m_cbCommand.AddString(s);	//11
		s.Format(_T("Blast300"));			m_cbCommand.AddString(s);	//12 Send 300 commands
		s.Format(_T("ProcNull"));			m_cbCommand.AddString(s);	//13 DebugPrint moved to 29 per RAC
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
		s.Format(_T("ASCAN_REP_RATE"));			m_cbCommand.AddString(s);	//27 how often Ascan sent to PAP
		s.Format(_T("WallNx"));					m_cbCommand.AddString(s);	//28
		s.Format(_T("DebugPrint"));				m_cbCommand.AddString(s);	//29 replace TcgBeamGainAll with DebugPrint
		s.Format(_T("ReadBack"));				m_cbCommand.AddString(s);	//30
		s.Format(_T("TcgBeamGainAll"));			m_cbCommand.AddString(s);	//31
		s.Format(_T("InitADC"));				m_cbCommand.AddString(s);	//32
		s.Format(_T("GateBlast"));				m_cbCommand.AddString(s);	//33
		s.Format(_T("Cmd204H_Blast"));			m_cbCommand.AddString(s);	//34
		s.Format(_T("Cmd205H_Blast"));			m_cbCommand.AddString(s);	//35
		s.Format(_T("XLocDivider"));			m_cbCommand.AddString(s);	//36

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
		s.Format(_T("PULSER_OnOff"));			m_cbCommand.AddString(s);	// 7+300h
		s.Format(_T("DEBUG_PRINT"));			m_cbCommand.AddString(s);	// 8+300h
		s.Format(_T("SAM_INIT"));				m_cbCommand.AddString(s);	// 9+300h
		m_cbCommand.SetCurSel(0);
		break;

		}
	m_nPopulated = 1;
	}

void CNcNx::NxTestCases(int nSelect)
	{
	WORD wNx, wMax, wMin, wDrop;
	nSelect &= 3;	//Limit to 1,2,3
	switch (nSelect)
		{
	case 0:
	case 1:
	default:
		wNx = 1;	wMax = 1392;	wMin = 110;	wDrop = 4;		break;
	case 2:
		wNx = 2;	wMax = 700;		wMin = 300;	wDrop = 10;		break;
		break;
	case 3:
		wNx = 3;	wMax = 500;		wMin = 400;	wDrop = 50;		break;
		break;
		}
	// Nx the same for all channels
	// args Nx, Max, Min, DropOut
	// For PAG testing, use only nParam to select test cases
	// WallNxCmd(m_nPAP, m_nBoard, m_nSeq, m_nCmdId, m_nParam);
	MakeWallNxCmd(m_nPAP, m_nBoard, wNx, wMax, wMin, wDrop);
	}

// Selecting a command from this combo box creates the command and sends it.
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
		{	//Pulser commands
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
		}	//Pulser commands

	else if ((0x200 <= (m_nCmdId + nCmdOffset)) && (m_nCmdId < TOTAL_LARGE_COMMANDS))
		{	// Large Commands
		switch (m_nCmdId + nCmdOffset)
			{
		case 0:
#ifdef I_AM_PAG
			// eliminate Debug fifo, use 0 for ProcNull
			DebugFifo(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
#endif

			case 2 + 0x200:
			case 3 + 0x200:							break;
			case 0x204: s = _T("TCG_BEAM_GAIN");
				LargeCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, nCmdLarge, (WORD)m_nParam);
				t = _T("Large Command");
				break;
			case 0x205: s = _T("TCG_SEQ_GAIN");	
				// build command here
				LargeCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, nCmdLarge, (WORD)m_nParam);
				t = _T("Large Command");
				break;
			default:
				break;
			}
		}	// Large Commands

	else
		{	// small commands
		switch (m_nCmdId + nCmdOffset)
			{
			//case 0:	s.Format(_T("null %d"), m_nCmdId);	break;
			case 0:	s = _T("ProcNull");						break;	// replace this with ProcNull, change #13 to something else
			case 1:	s = _T("Fake Data");					break;
			case 2: s.Format(_T("Gate %d Delay %d"), m_nGate, m_nParam); break;
			case 3: s.Format(_T("Gate %d Range %d"), m_nGate, m_nParam); break;
			case 4: s.Format(_T("Gate %d Blank %d"), m_nGate, m_nParam); break;
			case 5: s.Format(_T("Gate %d Thold %d"), m_nGate, m_nParam); break;
			case 6: s.Format(_T("Gate %d Trigger %d"), m_nGate, m_nParam); break;
			case 7: s.Format(_T("Gate %d Polarity %d"), m_nGate, m_nParam); break;
			case 8: s.Format(_T("Gate %d TOF %d"), m_nGate, m_nParam);		break;
			case 9: s.Format(_T("TCG Chnl Trigger Ch%d"), m_nCh, m_nParam);	break;
			case 13: s = _T("ProcNull");								break;
			case 21: s.Format(_T("Ascan Sample Rate = %d"), m_nParam);		break;	// ticks between a/d sampling
			//case 9: s.Format(_T("Nx = %d"), m_nParam);				break;
			case 26: s.Format(_T("AscanGateOutput = 0x%x"), m_nParam);	break;
			case 27: s.Format(_T("AscanRepRate (ms) = %d"), m_nParam);	break;
			case 28: s.Format(_T("Wall Nx = %d"), m_nParam);		break;
			case 30: s.Format(_T("ReadBk SubCmd %d"), m_nParam);	break;
			case 31: s.Format(_T("TcgBeamGainAll %d"), m_nParam);	break;
			case 32: s = _T("ADC Init");							break;
			case 33: s = _T("GateBlast");							break;
			case 36: s = _T("X-LocScale");							break;
			case 0x204: s = _T("TCG_BEAM_GAIN");					break;
			case 0x205: s = _T("TCG_SEQ_GAIN");						break;
			default:	s = t;		break;
			}
		//m_lbOutput.AddString( s );


		if (m_nCmdId + nCmdOffset < TOTAL_COMMANDS)	// limit number has to be adjusted
			{
			switch (m_nCmdId + nCmdOffset)
				{
				case 0:
#ifdef I_AM_PAG
					// Use 0 for procnull
					//DebugFifo(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
					ProcNull(m_nPAP, m_nBoard, m_nPAP, m_nBoard);	break;
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
					GateCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam );
					break;
					// TCG commands
				case 9:
					//TCG trigger cmd
					m_nParam &= 1;	// 0= IP,1=IF
					GateCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
					break;
					// TCG commands
				case 10:
				case 11:
					TcgCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
					break;
				case 12:
					// Blast 300 cmds
					Blast(m_nPAP, m_nBoard);						break;
				case 13:
					ProcNull(m_nPAP, m_nBoard, m_nPAP, m_nBoard);	break;
				case 26:
				case 27:
					GenericSmall(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
					break;
				case 28:
					NxTestCases(m_nParam);					break;
				case 29:
					DebugPrint(m_nPAP, m_nBoard, m_nCmdId, m_nParam);	break;
				case 30:
					ReadBackCmd(m_nPAP, m_nBoard, m_nSeq, m_nCmdId, m_nParam);	break;
				case 32:
					SamInitAdc(m_nPAP, m_nBoard, m_nParam);	break;
				case 33:
					GateBlast(m_nPAP, m_nBoard, m_nSeq);	break;
				case 34:
					Cmd204hBlast(m_nPAP, m_nBoard, m_nSeq);	break;
				case 35:
					Cmd205hBlast(m_nPAP, m_nBoard);			break;
				case 36:
					X_LocScale(m_nPAP, m_nBoard, m_nCmdId, m_nParam);	break;
				default:
					break;
				}
			}
#if 0
		else if ((m_nCmdId + nCmdOffset) < TOTAL_COMMANDS)
			{
			WordCmd(m_nPAP, m_nBoard, m_nSeq, m_nCh, m_nGate, m_nCmdId, m_nParam);
			}
#endif
		else
			{
			TRACE(_T("unknown command\n"));
			}
		}	// small commands


	}	// CNcNx::OnCbnSelchangeCbCmds()

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
	int i;
	if (pMsg->wMsgID > 0x300 + LAST_PULSER_COMMAND)
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
		gDlg.pUIDlg->SendMsgToPAP((int)pSend->bPapNumber, pSend->wMsgID, (void *)pSend);
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
		pSend->bPapNumber = m_nPAP;
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
			s.Format(_T("TCGBeamGain PAP=%d, Board=%d\n"), pSend->bPapNumber, pSend->bBoardNumber);
			TRACE(s);
			break;
		case 0x205:	// SetSeqTCGGain
			s.Format(_T("SetSeqTCGGain PAP=%d, Board=%d\n"), pSend->bPapNumber, pSend->bBoardNumber);
			for (i = 0; i < 128; i++)
				pSend->wCmd[i] = i;
			break;

		default:
			TRACE(_T("Unrecognized Large message .. delete pSend\n"));
			delete pSend;
			return;
			}

		gDlg.pUIDlg->SendMsgToPAP((int)pSend->bPapNumber, pSend->wMsgID, (void *)pSend);
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
			gDlg.pUIDlg->SendMsgToPAP((int)pSend->bPapNumber, pSend->wMsgID, (void *)pSend);
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
	case 9:		sym = _T("TCG Trigger: ");		break;	// 0=IP, 1=IF
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

// A generic small command
// nValue = m_nParam
void CNcNx::GenericSmall(int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, int nValue)
	{
	CString s, t, sym;
	switch (nCmd)
		{
		case SET_ASCAN_GATE_OUTPUT_ID: sym = _T("SetAscanGates: ");		break;	// UUI ASCAN_SCOPE
		case ASCAN_REP_RATE_ID:	sym = _T("AscanRepRate: ");		break;	// UUI ASCAN_SCOPE

		default:	sym = _T("???");		return;
		}

	memset(&m_GenericSmallCmd, 0, sizeof(ST_SMALL_CMD));
	m_GenericSmallCmd.wMsgID = nCmd;
	m_GenericSmallCmd.wByteCount = 32;
	m_GenericSmallCmd.uSync = SYNC;
	//		m_GateCmd.Head.wMsgSeqCnt;	SET BY SENDING ROUTINE
	m_GenericSmallCmd.bPapNumber = nPap;
	m_GenericSmallCmd.bBoardNumber = nBoard;
	m_GenericSmallCmd.wCmd[0] = nValue;	
	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, Value=%5d\n"),
		m_GenericSmallCmd.wMsgID, m_GenericSmallCmd.wByteCount, m_GenericSmallCmd.bPapNumber,
		m_GenericSmallCmd.bBoardNumber, m_GenericSmallCmd.wCmd[0]);
	t = sym + s;
	m_lbOutput.AddString(t);
	SendMsg((GenericPacketHeader*)&m_GenericSmallCmd);
	}

void CNcNx::MakeWallNxCmd(WORD nPap, WORD nBoard, WORD wX, WORD wMax, WORD wMin, WORD wDrop)
	{
	CString s;
	memset(&m_NxCmd, 0, sizeof(ST_NX_CMD));
	m_NxCmd.wMsgID = SET_WALL_NX_CMD_ID;		// 28;
	m_NxCmd.wByteCount = 32;
	m_NxCmd.uSync = SYNC;
	m_NxCmd.bPapNumber = (BYTE)nPap;
	m_NxCmd.bBoardNumber = (BYTE)nBoard;
	m_NxCmd.wNx = wX;
	m_NxCmd.wMax = wMax;
	m_NxCmd.wMin = wMin;
	m_NxCmd.wDropCount = wDrop;
	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, Nx=%d, Max=%d, Min=%d Drop=%5d  Param modifies Max,Min,drop\n"),
		m_NxCmd.wMsgID, m_NxCmd.wByteCount, m_NxCmd.bPapNumber, m_NxCmd.bBoardNumber, m_NxCmd.wNx,
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
	case 31:	sym = _T("TCGBeamGainAll: : "); 		break;
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

ST_SMALL_CMD Cmd;
ST_LARGE_CMD CmdL;

// cmd 12 -- changed to 28 per RAC
// sort of a pseudo command since it generates real instrument commands for the PAG/UUI side
void CNcNx::Blast(int m_nPAP, int m_nBoard)
	{
	int i, j,k;
	ST_WORD_CMD *pCmdW = (ST_WORD_CMD *)&Cmd;
	WORD *pW = (WORD *)&CmdL;
	BYTE *pB = (BYTE *)&Cmd;
	CString s;

	for (i = 0; i < sizeof(ST_LARGE_CMD)/2; i++)
		pW[i] = i;
	for (i = 0; i < sizeof(ST_SMALL_CMD); i++)
		pB[i] = i;

	Cmd.uSync = CmdL.uSync = SYNC;
	Cmd.wByteCount = 32;
	CmdL.wByteCount = 1056;
	Cmd.bPapNumber = CmdL.bPapNumber = m_nPAP;
	Cmd.bBoardNumber = CmdL.bBoardNumber = m_nBoard;

#if 1
	// Now send 7 pulser commands
	int iStart, iStop;
	// set a break point here to manage loop limits:0,7 7,14 14,21 21,28 28,35 35,42  42,39
	iStart = 0;
	iStop = iStart + 14;
	k = 0;
	DebugPrint(m_nPAP, m_nBoard, PULSER_DEBUG_PRINT_CMD_ID, 6);	// turn off debug in pulser and clear counters
	Sleep(40);
	// only prf, shape, and width are affected by changing start/stop limits
	for (i = iStart; i < iStop; i++)
		{
#if 1
		Cmd.wMsgID = 0x300 + 3;	//pulse shape
		Cmd.wCmd[0] = 255;
		for (j = 0; j < 4; j++)
			{
			pCmdW->bSeq = (k++);
			k = k % 10;
			SendMsg((GenericPacketHeader*)&Cmd);
			s.Format(_T("ID=%d, Bytes=%d, PAP=%d, wCmd=%5d\n"),
				Cmd.wMsgID, Cmd.wByteCount, Cmd.bPapNumber, Cmd.wCmd[0]);
			m_lbOutput.AddString(s);
			}
		Sleep(10);
#else
		Cmd.wMsgID = 0x300 + (i % 7);

		switch (i % 7)
			{
			case 0:		
			case 1:		Cmd.wCmd[0] = 8000;	//prf
				Sleep(250);
				break;		// uncertain how and why to set HV - for now will resend prf
			case 2:		Cmd.wCmd[0] = 0; // Polarity
				break;
#if 1
			case 3:		Cmd.wCmd[0] = 255;	break;	// SHAPE
			case 4:		Cmd.wCmd[0] = 6;	break;	// width
			case 5:		Cmd.wCmd[0] = 3;	break;	// seq len
			case 6:		Cmd.wCmd[0] = 4;	break;	// socomate pulse len
#endif
			default:
				break;
			}

		if ((i % 7) != 1)	// skip hv setting for now
			{
			s.Format(_T("ID=%d, Bytes=%d, PAP=%d, wCmd=%5d\n"),
				Cmd.wMsgID, Cmd.wByteCount, Cmd.bPapNumber, Cmd.wCmd[0]);
			m_lbOutput.AddString(s);
			SendMsg((GenericPacketHeader*)&Cmd);
			if ((i % 7) == 0) Sleep(500);	// let prf settle at 320
			else Sleep(10);
			}
#endif

		}	// pulser command loop
#endif

	// After pulser commands sent, drop to 10 Hz prf -- high prf better than low prf!!
	// 12k works very good on the bench
#if 0
	Cmd.wMsgID = 0x300;
	Cmd.wCmd[0] = 10;
	m_lbOutput.AddString(s);
	SendMsg((GenericPacketHeader*)&Cmd);
	Sleep(20);
#endif

	// Next a variable number of ADC commands

	Cmd.uSync = CmdL.uSync = SYNC;
	Cmd.wByteCount = 32;
	CmdL.wByteCount = 1056;
	Cmd.bPapNumber = CmdL.bPapNumber = m_nPAP;
	Cmd.bBoardNumber = CmdL.bBoardNumber = m_nBoard;
//	DebugPrint(m_nPAP, m_nBoard, DEBUG_PRINT_CMD_ID, 6);	// turn off debug in adc and clear counters
//	DebugPrint(m_nPAP, m_nBoard, 0x308, 6);					// turn off debug in pulser and clear counters
	Sleep(40);
#if 1
	for (i = 0; i < 1000; i++ )	// was 5000
		{
		Cmd.wMsgID = 2 + (i % 6);	// gate cmds 2-7
		Cmd.wCmd[0] = i;
		if ((i % 10) == 0)
			{
			// Large command
			CmdL.wMsgID = 516 + (rand() & 1);
			CmdL.wCmd[0] = i;
			CmdL.wCmd[1] = i+1;
			CmdL.wCmd[2] = i+2;
			CmdL.wCmd[3] = i+3;
			if ((i < 10) || (i > 590))
				{
				s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, wCmd[4] = %4d, %4d, %4d, %4d\n"),
					CmdL.wMsgID, CmdL.wByteCount, CmdL.bPapNumber, CmdL.bBoardNumber,
					CmdL.wCmd[0], CmdL.wCmd[1], CmdL.wCmd[2], CmdL.wCmd[3]);
				m_lbOutput.AddString(s);
				}
			SendMsg((GenericPacketHeader*)&CmdL);
			//Sleep(10);
			}
		else
			{
			// small command
			Cmd.wCmd[0] = i;
			pCmdW->bChnl = i & 7;
			pCmdW->bGateNumber = i & 3;
			pCmdW->bSeq = i % 3;
			if ((i < 10) || (i > 990))
				{
				s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, Seq=%d, Ch=%d, Gate=%d, wCmd=%5d\n"),
					Cmd.wMsgID, Cmd.wByteCount, Cmd.bPapNumber, Cmd.bBoardNumber,
					pCmdW->bSeq, pCmdW->bChnl, pCmdW->bGateNumber, Cmd.wCmd[0]);
				m_lbOutput.AddString(s);
				}
			SendMsg((GenericPacketHeader*)&Cmd);
			}
		}	// for (i = 0; i < 300; i++ )
#endif
	
#if 1
	// 500 large cmds
	for (i = 0; i < 800; i++)	// was 500
		{
		CmdL.wMsgID = 516 + (rand() & 1);
		CmdL.wCmd[0] = i;
		CmdL.wCmd[1] = i + 1;
		CmdL.wCmd[2] = i + 2;
		CmdL.wCmd[3] = i + 3;
		if ((i < 10) || (i > 490))
			{
			s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, wCmd[4] = %4d, %4d, %4d, %4d\n"),
				CmdL.wMsgID, CmdL.wByteCount, CmdL.bPapNumber, CmdL.bBoardNumber,
				CmdL.wCmd[0], CmdL.wCmd[1], CmdL.wCmd[2], CmdL.wCmd[3]);
			m_lbOutput.AddString(s);
			}
		SendMsg((GenericPacketHeader*)&CmdL);
		//Sleep(20);	// was 10
		}
#if 1
	// final blast of 50 pulser commands - not prf
	s = _T("Final blast of 50 large commands\n");
	m_lbOutput.AddString(s);
	for (i = 0; i < 100; i++)
		{
		CmdL.wMsgID = 516;
		CmdL.wCmd[0] = i;
		CmdL.wCmd[1] = i + 1;
		CmdL.wCmd[2] = i + 2;
		CmdL.wCmd[3] = i + 3;
		if ((i < 10) || (i > 90))
			{
			s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, wCmd[4] = %4d, %4d, %4d, %4d\n"),
				CmdL.wMsgID, CmdL.wByteCount, CmdL.bPapNumber, CmdL.bBoardNumber,
				CmdL.wCmd[0], CmdL.wCmd[1], CmdL.wCmd[2], CmdL.wCmd[3]);
			m_lbOutput.AddString(s);
			}
		SendMsg((GenericPacketHeader*)&CmdL);
		}

#endif
#endif
	Sleep(20);
	// reset ADC board to initial condition
	//SamInitAdc(m_nPAP, m_nBoard);

#if 0
	// Now send 7 pulser commands
	int iStart, iStop;
	// set a break point here to manage loop limits:0,7 7,14 14,21 21,28 28,35 35,42  42,39
	iStart = 0;
	iStop = iStart + 70;
	DebugPrint(m_nPAP, m_nBoard, PULSER_DEBUG_PRINT_CMD_ID, 2);	// turn off debug in pulser and clear counters
	Sleep(40);
	// only prf, shape, and width are affected by changing start/stop limits
	for (i = iStart; i < iStop; i++)
		{
		Cmd.wMsgID = 0x300 + (i % 7);
		
		switch (i % 7)
			{
			case 0:		Cmd.wCmd[0] = (i * 50) + 1000; break;	//prf
			case 1:		break;		// uncertain how and why to set HV - for now will resend prf
			case 2:		Cmd.wCmd[0] = 0; // Polarity
				break;
#if 1
			case 3:		Cmd.wCmd[0] = 255;	break;	// SHAPE
			case 4:		Cmd.wCmd[0] = 6;	break;	// width
			case 5:		Cmd.wCmd[0] = 3;	break;	// seq len
			case 6:		Cmd.wCmd[0] = 4;	break;	// socomate pulse len
#endif
			default:
				break;
			}
		
		if ((i % 7) != 1)	// skip hv setting for now
			{
			s.Format(_T("ID=%d, Bytes=%d, PAP=%d, wCmd=%5d\n"),
				Cmd.wMsgID, Cmd.wByteCount, Cmd.bPapNumber, Cmd.wCmd[0]);
			m_lbOutput.AddString(s);
			SendMsg((GenericPacketHeader*)&Cmd);
			Sleep(10);
			}

		}	// pulser command loop
	// restore Pulser to initial condition
	//SamInitPulser(m_nPAP, m_nBoard);

	Cmd.wMsgID = 0x300;
	Cmd.wCmd[0] = 12000;	// 12000 works very good in office
	// 3k fails on large commands almost always .. ditto 6k also 5k
	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, wCmd=%5d\n"),
		Cmd.wMsgID, Cmd.wByteCount, Cmd.bPapNumber, Cmd.wCmd[0]);

	m_lbOutput.AddString(s);
	SendMsg((GenericPacketHeader*)&Cmd);
#endif

	Sleep(500);
	}

// Set recognizable pattern for gate cmds to be read back
// Test (somewhat) if command sent shows correctly in ReadBack data 2019-05-03

void CNcNx::GateBlast(int m_nPAP, int m_nBoard, int Seq)
	{
	int is, ic, ig;	// seq/chnl/gate
	int iCmd, nGateVal;
	ST_GATE_DELAY_CMD *pCmdG = (ST_GATE_DELAY_CMD *)&Cmd;
	BYTE *pB = (BYTE *)&Cmd;
	CString s;
	Cmd.uSync = SYNC;
	Cmd.wByteCount = 32;
	Cmd.bPapNumber = m_nPAP;
	Cmd.bBoardNumber = m_nBoard;
	
	is = Seq & 3;	// seq = 0,1,2 only
	if (is == 3) is = 2;
	pCmdG->bSeq = is;
	for (iCmd = 2; iCmd < 9; iCmd++)
		{
		Cmd.wMsgID = iCmd;
		nGateVal = 1;
		for (ic = 0; ic < 8; ic++)
			{
			pCmdG->bChnl = ic;
			for (ig = 0; ig < 4; ig++)
				{
				pCmdG->bGateNumber = ig;
				pCmdG->wDelay =  nGateVal++;
				SendMsg((GenericPacketHeader*)&Cmd);
				s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Seq=%d, Ch=%d, G=%d, wCmd=%5d\n"),
					Cmd.wMsgID, Cmd.wByteCount, Cmd.bPapNumber, is, ic, ig, pCmdG->wDelay);
				m_lbOutput.AddString(s);
				}	// for (ig = 0; ig < 4; ig++)
			}	// for (ic = 0; ic 8; ic++)
		}	// for (iCmd = 2; iCmd < 9; iCmd++)
	}

// Cmd204H-Blast
// Send 1 command for each sequence, like GateBlast
//
void CNcNx::Cmd204hBlast(int m_nPAP, int m_nBoard, int Seq)
	{
	int is, ir, ie;	// seq/row/element  16 per row
	int ic;	// chnl loop
	int nRowStart;
	ST_TCG_BEAM_GAIN *pCmdBG = (ST_TCG_BEAM_GAIN *)&CmdL;	// Beam Gain
	CString s;
	pCmdBG->Head.uSync = SYNC;
	pCmdBG->Head.wByteCount = 288;
	pCmdBG->Head.bPapNumber = m_nPAP;
	pCmdBG->Head.bBoardNumber = m_nBoard;
	pCmdBG->Head.wMsgID = TCG_GAIN_CMD_ID; //Generic header is 12 bytes

	for (is = 0; is < 3; is++)
		{
		pCmdBG->bSeqNumber = is;
		for (ic = 0; ic < 8; ic++)	//chnl loop 8 channels
			{
			pCmdBG->bChnl = ic;
			// 8*16*2 = 256, total size is 12 + 256 = 268
				nRowStart = 0;
			for (ir = 0; ir < 8; ir++)	// 8 rows
				{
				for (ie = 0; ie < 16; ie++)	// 16 elements
					{
					pCmdBG->wGain[ie + nRowStart] = ie + nRowStart;
					}
				nRowStart += 16;
				}
			s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Seq=%d, 1st Byte 0x%02x\n"),
				CmdL.wMsgID, CmdL.wByteCount, CmdL.bPapNumber, is, pCmdBG->wGain[0]);
			m_lbOutput.AddString(s);
			// Send 1 chnl at a time
			SendMsg((GenericPacketHeader*)&CmdL);	// SendMsg deletes argument at end, but we pass a ptr to a static struct.2019-05-23
			}	// chnl loop
		} //for (is = 0; is < 3; is++)
	}

// Cmd205H Blast  sizeof(CMD205H_READBACK);
void CNcNx::Cmd205hBlast(int m_nPAP, int m_nBoard)
	{
	int is, ir, ie;	// seq/row/element  16 per row
	int nRowStart;
	ST_SEQ_TCG_GAIN *pCmdSG = (ST_SEQ_TCG_GAIN *)&CmdL;	// Seq Gain
	CString s;
	pCmdSG->Head.uSync = SYNC;
	pCmdSG->Head.wByteCount = 288;	// 128 values, 256 bytes + header of 32
	pCmdSG->Head.bPapNumber = m_nPAP;
	pCmdSG->Head.bBoardNumber = m_nBoard;
	pCmdSG->Head.wMsgID = SEQ_TCG_GAIN_CMD_ID; //Generic header is 12 bytes

	for (is = 0; is < 3; is++)
		{
		pCmdSG->bSeqNumber = is;

		//pCmdSG->bChnl = ic;
		// 8*16*2 = 256, total size is 12 + 256 = 268
		nRowStart = 0;
		for (ir = 0; ir < 8; ir++)	// 8 rows
			{
			for (ie = 0; ie < 16; ie++)	// 16 elements
				{
				pCmdSG->wGain[ie + nRowStart] = ie + nRowStart;
				}
			nRowStart += 16;
			}
		s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Seq=%d, 1st Byte 0x%02x\n"),
			CmdL.wMsgID, CmdL.wByteCount, CmdL.bPapNumber, is, pCmdSG->wGain[0]);
		m_lbOutput.AddString(s);
		// Send 1 seq at a time
		SendMsg((GenericPacketHeader*)&CmdL);	// SendMsg deletes argument at end, but we pass a ptr to a static struct.2019-05-23
		} //for (is = 0; is < 3; is++)
	}

// cmd 13 ProcNull for ADC   Something else for UUI
void CNcNx::ProcNull(int nPap, int nBoard, int nCmd, int nValue)
	{
	CString s;
	ST_WORD_CMD *pCmdW = (ST_WORD_CMD *)&Cmd;
	memset((void *)pCmdW, 0, sizeof(ST_WORD_CMD));
	pCmdW->Head.wMsgID = 0;
	pCmdW->Head.wByteCount = 32;
	pCmdW->Head.uSync = SYNC;
	pCmdW->Head.bPapNumber = nPap;
	pCmdW->Head.bBoardNumber = nBoard;
	s.Format(_T("ProcNull: ID=%d, Bytes=%d, PAP=%d  Board=%d\n"),
		pCmdW->Head.wMsgID, pCmdW->Head.wByteCount, nPap, nBoard );
	m_lbOutput.AddString(s);
	SendMsg((GenericPacketHeader*)pCmdW);
	}


// cmd 13 -- changed to 29 per RAC
// bit 0 turn on/off printf in NIOS code
// bit 1=1  resets command count
// bit 2=1  reset max quque depth
void CNcNx::DebugPrint(int nPap, int nBoard, int nCmd, int nValue)
	{
	CString s;
	ST_DEBUG_CMD Dbg;
	memset(&Dbg, 0, sizeof(ST_DEBUG_CMD));
	Dbg.wMsgID = nCmd;// DEBUG_PRINT_CMD_ID;
	Dbg.uSync = SYNC;
	Dbg.wByteCount = 32;
	Dbg.bPapNumber = nPap;
	Dbg.bBoardNumber = nBoard;
	Dbg.wDbgFlag = nValue;
	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, PrintFlag=%5d\n"),
		Dbg.wMsgID, Dbg.wByteCount, Dbg.bPapNumber, Dbg.bBoardNumber, Dbg.wDbgFlag);
	m_lbOutput.AddString(s);
	SendMsg((GenericPacketHeader*)&Dbg);
	}

// Runs Sam's init code on adc board
// Probably crashed the NIOS code with unhandled interrupt message.
// bit0=0, ADC board Wiznet ONLY init. bit 0 set, reset ADC BRD also
void CNcNx::SamInitAdc(int nPap, int nBoard, int m_nParam)
	{
#if 0
	CString s;
	ST_SMALL_CMD Init;
	memset(&Init, 0, sizeof(Init));
	Init.uSync = SYNC;
	Init.wByteCount = 32;
	Init.wMsgID = ADC_WIZ_RESET_CMD_ID;
	Init.bPapNumber = nPap;
	Init.bBoardNumber = nBoard;
	Init.wCmd[0] = m_nParam;
	if (m_nParam)
		s.Format(_T("ID=%d, Init ADC Board %d, PAP %d\n"), Init.wMsgID, nBoard, nPap);
	else
		s.Format(_T("ID=%d, Init only Wiznet on ADC Board %d, PAP %d\n"), Init.wMsgID, nBoard, nPap);
	m_lbOutput.AddString(s);
	SendMsg((GenericPacketHeader*)&Init);
#endif
	}

// Probably crashed the NIOS code with unhandled interrupt message.
// bit0=0, pulser board Wiznet ONLY init. bit 0 set, reset PULSER BRD also
void CNcNx::SamInitPulser(int nPap, int nBoard, int nSel)
	{
	CString s;
	ST_SMALL_CMD Init;
	Init.uSync = SYNC;
	Init.wByteCount = 32;
	Init.wMsgID = PULSER_WIZ_RESET_CMD_ID;
	Init.bPapNumber = nPap;
	Init.bBoardNumber = nBoard;
	Init.wCmd[0] = nSel;
	if (nSel)
		s.Format(_T("Init Pulser Board %d, PAP %d\n"), nBoard, nPap);
	else
		s.Format(_T("Init only Wianet on Pulser Board %d, PAP %d\n"), nBoard, nPap);
	m_lbOutput.AddString(s);
	SendMsg((GenericPacketHeader*)&Init);
	}


// Readback has sub commands. Top read back ID is 30
// nRbId is nValue from NcNx parameters
// For Apr 2019 only one read back --- gate data by sequence
//
void CNcNx::ReadBackCmd(int nPap, int nBoard, int nSeq, int nCmd, int nValue)
	{
	CString s, t, sym;
	memset(&m_RdBkCmd, 0, sizeof(ST_SET_TCG_DELAY_CMD));
	m_RdBkCmd.Head.wMsgID = nCmd;	 // 30;
	m_RdBkCmd.Head.wByteCount = 32;
	m_RdBkCmd.Head.uSync = SYNC;
	//		m_TcgCmd.Head.wMsgSeqCnt;	SET BY SENDING ROUTINE
	m_RdBkCmd.Head.bPapNumber = nPap;
	m_RdBkCmd.Head.bBoardNumber = nBoard;
	m_RdBkCmd.bSeq = nSeq;
	m_RdBkCmd.wReadBackID = m_nReadBackWhat;	// nValue;	// this is the nValue parameter-which data to send back
	// wReadBackID = 1 means send back gates data by seq number.

	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, Seq=%d,  RdBkID = %2d\n"),
		m_RdBkCmd.Head.wMsgID, m_RdBkCmd.Head.wByteCount, m_RdBkCmd.Head.bPapNumber,
		m_RdBkCmd.Head.bBoardNumber, nSeq, m_RdBkCmd.wReadBackID);
	t = sym + s;
	m_lbOutput.AddString(t);
	SendMsg((GenericPacketHeader*)&m_RdBkCmd);
	}

// cmd 36 
// set the motion pulse hardware divider in the ADC board
void CNcNx::X_LocScale(int nPap, int nBoard, int nCmd, int nValue)
	{
	CString s;
	ST_X_LOC_SCALE_CMD xScale;
	memset(&xScale, 0, sizeof(ST_X_LOC_SCALE_CMD));
	xScale.Head.wMsgID = nCmd;// DEBUG_PRINT_CMD_ID;
	xScale.Head.uSync = SYNC;
	xScale.Head.wByteCount = 32;
	xScale.Head.bPapNumber = nPap;
	xScale.Head.bBoardNumber = nBoard;
	xScale.wScaleFactor = nValue;
	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, ScaleFactor=%5d\n"),
		xScale.Head.wMsgID, xScale.Head.wByteCount, xScale.Head.bPapNumber, xScale.Head.bBoardNumber, xScale.wScaleFactor);
	m_lbOutput.AddString(s);
	SendMsg((GenericPacketHeader*)&xScale);
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
// Replaced by Blast. Delete DebugFifo sometime
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
		lrg[i].bPapNumber = 0;
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

// Puts wValue into all 512 large command  words wCmd[]
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
			sym = _T("TCG_BEAM_GAIN_CMD ");			break;	//0x204
#if 0
		case SET_ASCAN_BEAMFORM_DELAY_ID:	
			sym = _T("SET_ASCAN_BEAMFORM_DELAY ");	break;	//0x204
#endif
		}
	m_wLargeCmd.wByteCount = sizeof(ST_LARGE_CMD);
	m_wLargeCmd.uSync = SYNC;
	//		m_wLargeCmd.Head.wMsgSeqCnt;	SET BY SENDING ROUTINE
	m_wLargeCmd.bPapNumber = nPap;
	m_wLargeCmd.bBoardNumber = nBoard;
	m_wLargeCmd.bSeqNumber = nSeq & 0x1f;	// starting seq number
	for ( i = 0; i < 512; i++)
		m_wLargeCmd.wCmd[i] = wValue;

	s.Format(_T("ID=%d, Bytes=%d, PAP=%d, Board=%d, (Param)WordCmd[0] = %d\n"),
		m_wLargeCmd.wMsgID, m_wLargeCmd.wByteCount, m_wLargeCmd.bPapNumber,
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
		case 7 + 0x300:			sym = _T("PULSER ON/OFF ");				break;
		case 8 + 0x300:			sym = _T("Debug Print ");				break;
		case 9 + 0x300:			sym = _T("Pulser Init ");				break;
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

// In resource view, add and event handler for edit box change
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


// In resource view, add and event handler for edit box change
void CNcNx::OnEnChangeEdGate()
	{
	if (m_nPopulated)		m_nGate = m_spGate.GetPos();
	}

void CNcNx::OnEnChangeEdCh()	//channel
	{
	if (m_nPopulated)		m_nCh = m_spCh.GetPos();
	}


void CNcNx::OnEnChangeEdSeq()
	{
	if (m_nPopulated) m_nSeq = m_spSeq.GetPos();
	}

void CNcNx::IncrementAscanCnt(void)
	{
	SetDlgItemInt(IDC_EN_ASCANCNT, guAscanMsgCnt, 0);
	}

void CNcNx::ShowLastCmdSeq(void)
	{
#ifdef I_AM_PAG
	SetDlgItemInt(IDC_EN_LAST_MSG_CNT, gwLastAdcCmdMsgCnt, 0);
#endif
	}

void CNcNx::ShowIdataSource(void)
	{
	SetDlgItemInt(IDC_EN_PAP, gLastIdataPap.bPapNumber, 0);
	SetDlgItemInt(IDC_EN_BOARD, gLastIdataPap.bBoardNumber, 0);
	}

void CNcNx::ShowSmallCmds(void)
	{
	SetDlgItemInt(IDC_EDSMAL_CMD, gLastAscanPap.wSmallCmds, 0);
	}
void CNcNx::ShowLargeCmds(void)
	{
	SetDlgItemInt(IDC_EDLARGE_CMD, gLastAscanPap.wLargeCmds, 0);
	}
void CNcNx::ShowPulserCmds(void)
	{
	SetDlgItemInt(IDC_EDPULSER_CMD, gLastAscanPap.wPulserCmds, 0);
	}

void CNcNx::ShowSmallQ(void)
	{
	SetDlgItemInt(IDC_EN_SMALLQ, gLastIdataPap.bCmdQDepthS, 0);
	}
void CNcNx::ShowLargeQ(void)
	{
	SetDlgItemInt(IDC_EN_LARGEQ, gLastIdataPap.bCmdQDepthL, 0);
	}
void CNcNx::ShowPulserQ(void)
	{
	SetDlgItemInt(IDC_EN_PULSERQ, gLastIdataPap.bCmdQDepthP, 0);
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


void CNcNx::OnCbnSelchangeCbRdwhat()
	{
	// TODO: Add your control notification handler code here
	int nSel;
	nSel = m_cbReadWhat.GetCurSel();
	m_nReadBackWhat = nSel + 1;
	}
