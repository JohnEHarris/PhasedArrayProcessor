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
DDX_Control( pDX, IDC_SP_INST, m_spInst );
DDX_Control( pDX, IDC_SP_PAP, m_spPap );
DDX_Control( pDX, IDC_SP_SEQ, m_spSeq );
DDX_Control( pDX, IDC_SP_CH, m_spCh );
DDX_Control( pDX, IDC_SP_GATE, m_spGate );
DDX_Control( pDX, IDC_SP_PARAM, m_spParam );
DDX_Control( pDX, IDC_LB_NCNX, m_lbOutput );
	}


BEGIN_MESSAGE_MAP(CNcNx, CDialogEx)
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_INST, &CNcNx::OnDeltaposSpInst )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_PAP, &CNcNx::OnDeltaposSpPap )
	ON_WM_VSCROLL()
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_SEQ, &CNcNx::OnDeltaposSpSeq )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_CH, &CNcNx::OnDeltaposSpCh )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_GATE, &CNcNx::OnDeltaposSpGate )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SP_PARAM, &CNcNx::OnDeltaposSpParam )
	ON_BN_CLICKED( IDC_BUTTON1, &CNcNx::OnBnClickedErase )
END_MESSAGE_MAP()


// CNcNx message handlers
#if 0
void CNcNx::DebugOut(CString s)
	{
	char txt[1024];
	CstringToChar(s,txt);
	strcat(txt,"\r\n");
	DebugFile.Write(txt, strlen(txt));
	}
#endif


BOOL CNcNx::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	PositionWindow();
	// if this is the PAG
#ifdef I_AM_PAG
	m_spPap.SetRange( 0, 0 );	// gnMaxClientsPerServer - 1 );  //how many clients do I have
	m_spInst.SetRange( 0, 2 );	// gnMaxClients - 1 );		// how many clients does my client have
	m_spSeq.SetRange( 0, gMaxSeqCount-1 );		// number of sequence in firing scheme
	m_spCh.SetRange( 0, gMaxChnlsPerMainBang-1 );		// number of channel in each sequence
	m_spGate.SetRange( 0, 3 );		// number of gates in each channel
	m_spParam.SetRange( 0, 100 );	// depends of command selected from list box
#else
	// I am the PAP
	m_spPap.SetRange( 0, 0 );  //how many clients do I have
	m_spInst.SetRange( 0, gnMaxClients-1 );		// how many clients does my client have
	m_spSeq.SetRange( 0, gMaxSeqCount-1 );		// number of sequence in firing scheme
	m_spCh.SetRange( 0, gMaxChnlsPerMainBang-1 );		// number of channel in each sequence
	m_spGate.SetRange( 0, 3 );	

#endif
	m_lbOutput.ResetContent();

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


void CNcNx::OnDeltaposSpInst( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_nInst = pNMUpDown->iPos + pNMUpDown->iDelta;
	}


void CNcNx::OnDeltaposSpPap( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	// must use vertical scroll to retrieve the new value after the button spins
	*pResult = 0;
	m_nPAP = pNMUpDown->iPos + pNMUpDown->iDelta;
	}


void CNcNx::OnDeltaposSpSeq( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_nSeq = pNMUpDown->iPos + pNMUpDown->iDelta;
	}


void CNcNx::OnDeltaposSpCh( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_nSeq = pNMUpDown->iPos + pNMUpDown->iDelta;
	}


void CNcNx::OnDeltaposSpGate( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_nGate = pNMUpDown->iPos + pNMUpDown->iDelta;
	}


//Parameter variable depends on what command is selected
// could be gate delay, or receiver gain, or trigger level of a flaw
void CNcNx::OnDeltaposSpParam( NMHDR *pNMHDR, LRESULT *pResult )
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_nParam = pNMUpDown->iPos + pNMUpDown->iDelta;
	}


#if 0
void CNcNx::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
	{
	// TODO: Add your message handler code here and/or call default
	int nCtrlId = pScrollBar->GetDlgCtrlID();
#if 0
	int m_nChnlType, m_nChnlRepeat, m_nChnl;
	int m_nNcId, m_nThldId, m_nMId;
	int m_nNcOd, m_nThldOd, m_nMOd;
	int m_nNx, m_nMaxWall, m_nMinWall, m_nDropCnt;
	switch (nCtrlId)
		{
		default:	break;
		case IDC_SP_INST:
			m_nInst = nPos;
			//ChangePamOrInstrument();
			m_spInst.SetPos( m_nInst );
			UpdateTitle();
			break;

		case IDC_SP_CHNL_TYPES:		m_nChnlType = nPos;		break;
		case IDC_SP_CHNL_REPEATS:	m_nChnlRepeat = nPos;	break;
		case IDC_SP_CHNL:			m_nChnl = nPos;			break;

		case IDC_SP_NCID:
			m_nNcId = nPos;		break;
		case IDC_SP_THLDID:		m_nThldId = nPos;	break;
		case IDC_SP_MID:
			m_nMId = nPos;		break;
		case IDC_SP_NCOD:		m_nNcOd = nPos;		break;
		case IDC_SP_THLDOD:		m_nThldOd = nPos;	break;
		case IDC_SP_MOD:		m_nMOd = nPos;		break;

		case IDC_SP_NX:			m_nNx = nPos;		break;
		case IDC_SP_MAXWALL:	m_nMaxWall = nPos;	break;
		case IDC_SP_MINWALL:	m_nMinWall = nPos;	break;
		case IDC_SP_DROPCNT:	m_nDropCnt = nPos;	break;
#endif
		//}
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
	}
#endif

void CNcNx::UpdateTitle(void)
	{
	CString s, t;
	s.Format(_T("Nc Nx Phased Array 2 PAP = %d, Instrument = %2d\r\n"), m_nPam, m_nInst);
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




void CNcNx::OnBnClickedErase()
	{
	// TODO: Add your control notification handler code here
	m_lbOutput.ResetContent();
	}
