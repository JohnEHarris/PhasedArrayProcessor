// NcNx.cpp : implementation file
//

#include "stdafx.h"


#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

#include "Extern.h"
//#include "NcNx.h" went into Tscandlg.h
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
	: CDialogEx(CNcNx::IDD, pParent)
	{
	m_nPam = 0;
	m_nInst = 0;
	m_nChnlType = m_nChnlRepeat = 0;
	m_nChnl = 1;
	m_nNcId = m_nThldId = m_nMId = 0;
	m_nNcOd = m_nThldOd = m_nMOd = 0;
	m_nNx =  m_nDropCnt = 0;
	m_nMaxWall =  100;
	m_nMinWall = 25;

	// create log file for Release debugging
	CString s;
	char txt[1024];
	time_t Now;
	struct tm when;

// create log file for Release debugging
	DebugFile.Open(_T("NcNxDebug.txt"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);


	DebugFile.SeekToEnd();	// if it already exists, go to end
#ifdef _DEBUG
	s = _T("\r\n***************************************************DEBUG*************************************************************************");
#else
	s = _T("\r\n***************************************************DEBUG RELEASE*****************************************************************");
#endif


	CstringToChar(s,txt);
	strcat(txt,"\r\n");
	DebugFile.Write(txt, strlen(txt));
	time( &Now );
	when = *localtime( &Now );
	s = asctime( &when);
	s.TrimRight();
	//s += _T("\r\n");
	CstringToChar(s,txt);
	strcat(txt,"\r\n");
	DebugFile.Write(txt, strlen(txt));
	}


CNcNx::~CNcNx()
	{
	DebugFile.Close();
	m_pDlg = NULL;
	}

void CNcNx::DoDataExchange(CDataExchange* pDX)
	{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_PAM, m_cbPam);
	DDX_Control(pDX, IDC_LB_NC_NX, m_lbOutput);
	DDX_Control(pDX, IDC_SP_NCID, m_spNcId);
	DDX_Control(pDX, IDC_SP_THLDID, m_spThldId);
	DDX_Control(pDX, IDC_SP_MID, m_spMId);
	DDX_Control(pDX, IDC_SP_NCOD, m_spNcOd);
	DDX_Control(pDX, IDC_SP_THLDOD, m_spThldOd);
	DDX_Control(pDX, IDC_SP_MOD, m_spMOd);
	DDX_Control(pDX, IDC_SP_NX, m_spNx);
	DDX_Control(pDX, IDC_SP_MAXWALL, m_spMaxWall);
	DDX_Control(pDX, IDC_SP_MINWALL, m_spMinWall);
	DDX_Control(pDX, IDC_SP_DROPCNT, m_spDropCnt);
	DDX_Control(pDX, IDC_SP_CHNL, m_spChnl);
	DDX_Control(pDX, IDC_SP_CHNL_TYPES, m_spChTypes);
	DDX_Control(pDX, IDC_SP_CHNL_REPEATS, m_spChRepeat);
	DDX_Control(pDX, IDC_SP_INST, m_spInst);
	}


BEGIN_MESSAGE_MAP(CNcNx, CDialogEx)
	ON_CBN_SELCHANGE(IDC_CB_PAM, &CNcNx::OnCbnSelchangeCbPam)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SP_INST, &CNcNx::OnDeltaposSpInst)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BN_UPDATE, &CNcNx::OnBnClickedBnUpdate)
	ON_BN_CLICKED(IDC_BN_ERASE, &CNcNx::OnBnClickedBnErase)
	ON_BN_CLICKED(IDC_BN_PRINT, &CNcNx::OnBnClickedPrint2File)
	ON_BN_CLICKED(IDC_BN_SEND, &CNcNx::OnBnClickedBnSend)
	ON_BN_CLICKED(IDC_BN_GENERATE, &CNcNx::OnBnClickedBnGenerate)
END_MESSAGE_MAP()


// CNcNx message handlers
void CNcNx::DebugOut(CString s)
	{
	char txt[1024];
	CstringToChar(s,txt);
	strcat(txt,"\r\n");
	DebugFile.Write(txt, strlen(txt));

	}

BOOL CNcNx::OnInitDialog()
	{
	CDialogEx::OnInitDialog();
	WINDOWPLACEMENT wp;
	RECT rect;
	CString s;
	int i;
	int dx, dy;		// width and height of original window

	GetWindowPlacement(&wp);
	dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	pCTscanDlg->GetWindowLastPosition(_T("NC_NX_PA"), &rect);
	if ( ( (rect.right - rect.left) >= dx ) &&
			( (rect.bottom - rect.top) >= dy ))
		{
		wp.rcNormalPosition = rect;
		SetWindowPlacement(&wp);
		}

	//m_pNcNxCfg	= (NcNxPA2REC *) &ConfigRec.NcNx;
	//m_pPamInstChnlInfo		= (PAM_INST_CHNL_INFO *) &m_pNcNxCfg->PaInstChnlInfo[0][0];	// PAM 0, INST 0
	m_nLastPam = m_nPam	= 0;
	m_nLastInst= m_nInst = 0;

	m_cbPam.ResetContent();
	m_cbPam.AddString(_T("0"));
	m_cbPam.SetCurSel(0);


	// set spinners
	m_spInst.SetRange(0,2);

	m_spNcId.SetRange(0,8);
	m_spThldId.SetRange(0,255);
	m_spMId.SetRange(1,8);
	m_spMId.SetPos(1);
	m_nMId = 1;

	m_spNcOd.SetRange(0,8);
	m_spThldOd.SetRange(0,255);
	m_spMOd.SetRange(1,8);
	m_spMOd.SetPos(1);
	m_nMOd = 1;

	m_spNx.SetRange(0,8);
	m_spMaxWall.SetRange(50,1380);
	m_spMaxWall.SetPos(50);
	m_nMaxWall = 50;
	m_spMinWall.SetRange(25,1380);
	m_spMinWall.SetPos(25);
	m_nMinWall = 25;
	m_spDropCnt.SetRange(0,100);

	m_spChTypes.SetRange(1,8);
	m_spChTypes.SetPos(1);
	m_nChnlType = 1;
	m_spChRepeat.SetRange(1,16);
	m_spChRepeat.SetPos(8);
	m_nChnlRepeat = 8;
	m_spChnl.SetRange(1,8);

	m_spChnl.SetRange(1,8);
	m_spChnl.SetPos(1);
	m_nChnl = 1;

	m_lbOutput.ResetContent();
	m_nListBoxUnsentQty = 0;
	//i = sizeof(PAM_INST_CHNL_INFO);

	UpdateTitle();
	//MessageBox(_T("Changes are not registered until the Update button is pressed \n \
 //		Changing the PAM or Instrument clears the list box"), _T("Warning"), MB_OK);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	}


void CNcNx::ChangePamOrInstrument(void)
	{
	int nResult;
	int i = m_nListBoxUnsentQty;
	i = m_nLastPam;
	i = m_nLastInst;

	// skip the message which comes from resetting the pam/inst with cancel below
	if ( (m_nPam == m_nLastPam) && (m_nInst == m_nLastInst)) 
		return;

	if ( m_nListBoxUnsentQty > 0 )
		{
		nResult = MessageBox(_T("Unsent configuration data in list box\n  \
 If you continue it will be sent and list box erased"), _T("Warning"), MB_OKCANCEL);
		if (nResult == IDCANCEL)
			{
			m_nPam = m_nLastPam;
			m_nInst = m_nLastInst;
			m_cbPam.SetCurSel(m_nPam);
			m_spInst.SetPos(m_nInst);
			return;
			}
		else
			{
			CopyListBoxToMessage();
			OnBnClickedPrint2File();
			m_lbOutput.ResetContent();
			}


		}

	m_nLastPam = m_nPam;
	m_nLastInst = m_nInst; 
	}



// uses m_nLastPam and m_nLastInst to steer message to correct PAM/ Inst
// first fill the big structure with new content, then send all messages which have non-zero content
// Assumes that the list box references only one PAM and one Instrument, but multiple channel types
void CNcNx::CopyListBoxToMessage(void)
	{
#if 0
	int i, j, nLast;
	int tmp[16];
	char txt[1024];
	int chnl;
	CString s;
	ST_NC_NX *pStNcNx;

	m_pPamInstChnlInfo		= &m_pNcNxCfg->PaInstChnlInfo[m_nLastPam][m_nLastInst];
//	m_pPamInstChnlInfo->bChnlTypes = (BYTE) m_nChnlType;
//	m_pPamInstChnlInfo->bSeqQty = (BYTE) m_nChnlRepeat;
//	m_pPamInstChnlInfo->bMaxVChnlPerInst = 32; // AN ASSUMPTION AT THIS POITN

	nLast = m_lbOutput.GetCount();

	for ( i = 0; i < nLast; i++) //look at every line in list box
		{
		m_lbOutput.GetText(i,s);
		DebugOut(s);
		CstringToIntArray(s,tmp, 16);
		chnl				= tmp[2] - 1; // Operator 1st chnl = 1, hardware 1st chnl = 0

		pStNcNx = &m_pPamInstChnlInfo->stNcNx[chnl];

		pStNcNx->bNcID		= tmp[3];
		pStNcNx->bTholdID	= tmp[4];
		pStNcNx->bModID		= tmp[5];
		pStNcNx->bNcOD		= tmp[6];
		pStNcNx->bTholdOD	= tmp[7];
		pStNcNx->bModOD		= tmp[8];
		pStNcNx->wNx		= tmp[9];
		pStNcNx->wWallMax	= tmp[10];
		pStNcNx->wWallMin	= tmp[11];
		pStNcNx->wDropOut	= tmp[12];
		
		}
	//SendMsg(m_pPamInstChnlInfo);//, m_pPamInstChnlInfo->bChnlTypes);
#endif
	}

// Message goes to one specific PAM and one specific instrument in the PAM
// 2016-06-27 we will assume that messages will be built by the routine which configures the
// parameters of the message. This is a departure from the way we have done this for a couple
// of decades. Allocate memory to the message here to be put into a linked list.  Call the main
// dialog to actually add the message to the linked list so the message sequence number can be attached.
//
void CNcNx::SendMsg(PAP_INST_CHNL_NCNX *pMsg)//, int nChTypes)
	{
	int nPam, nInst, nChnl;
	PAP_INST_CHNL_NCNX *pSend = new PAP_INST_CHNL_NCNX;
	memset ((void *) pSend,0, sizeof(PAP_INST_CHNL_NCNX));

	pSend->wMsgID			= NC_NX_CMD_ID;
	pSend->bPAPNumber		= m_nLastPam;
	pSend->bInstNumber		= m_nLastInst;
	pSend->uSync			= pMsg->uSync;
	pSend->wByteCount		= pMsg->wByteCount;
//	pSend->bChnlTypes		= m_nChnlType;
//	pSend->bSeqQty			= m_nChnlRepeat;
//	pSend->bMaxVChnlPerInst	= 32;

	memcpy((void *)pSend, (void *)pMsg, pMsg->wByteCount);	// sizeof(ST_NC_NX) * 72);
	pCTscanDlg->SendMsgToPAM(m_nLastPam, NC_NX_CMD_ID, (void *)pSend);

	}



void CNcNx::Save_Pos()
	{
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition(_T("NC_NX_PA"), &wp);
	}

void CNcNx::OnOK()
	{
	// TODO: Add your specialized code here and/or call the base class

	Save_Pos();

	CDialogEx::OnOK();
	delete this;
	m_pDlg = NULL;
	}


void CNcNx::OnCancel()
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnCancel();
	delete this;
	m_pDlg = NULL;

	}


void CNcNx::PostNcDestroy()
	{
	// Let the top level dialog TscanDlg know we are now dead
	CDialogEx::PostNcDestroy();
	CDialogEx::OnDestroy();
	delete this;
	m_pDlg = NULL;
	}


void CNcNx::OnCbnSelchangeCbPam()
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	m_nPam = m_cbPam.GetCurSel();
	ChangePamOrInstrument();
	UpdateTitle();
	}


void CNcNx::OnDeltaposSpInst(NMHDR *pNMHDR, LRESULT *pResult)
	{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	// must use vertical scroll to retrieve the new value after the button spins
	*pResult = 0;
	UpdateData(TRUE);	// Copy screen TO variables
	//m_nInst = m_spInst.GetPos(); always seems to be the last value
	}


void CNcNx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
	// TODO: Add your message handler code here and/or call default
	int nCtrlId = pScrollBar->GetDlgCtrlID();
#if 0
	int m_nChnlType, m_nChnlRepeat, m_nChnl;
	int m_nNcId, m_nThldId, m_nMId;
	int m_nNcOd, m_nThldOd, m_nMOd;
	int m_nNx, m_nMaxWall, m_nMinWall, m_nDropCnt;
#endif
	switch (nCtrlId)
		{
	default:	break;
	case IDC_SP_INST:			m_nInst = nPos;		ChangePamOrInstrument();	UpdateTitle();	break;
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
		}


	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
	}

void CNcNx::UpdateTitle(void)
	{
	CString s, t;
	s.Format(_T("Nc Nx Phased Array 2 PAM = %d, Instrument = %2d\r\n"), m_nPam, m_nInst);
	SetWindowText(s);
	}


// Take the data from the selection boxes and display as a line of text on the list box.
// Copy the local variables into the message structure as well
//
void CNcNx::OnBnClickedBnUpdate()
	{
	// TODO: Add your control notification handler code here
	CString s,t,u;
	int nTab = 15;
	int nErrCnt = 0;
	if ( (m_nChnl < 1) || (m_nChnl > 8) )
		{
		MessageBox(_T("Invalid Channel Number - correct and then resubmit"), _T("Input Error"), MB_OK);
		return;
		}
	if ( (m_nNcId == 0) || (m_nNcOd == 0) )
		MessageBox(_T("Nc = 0 means not a flaw channel"), _T("Input Warning"), MB_OK);

	if (m_nNx == 0) 
		MessageBox(_T("Nx = 0 means not a wall channel"), _T("Input Warning"), MB_OK);

	m_lbOutput.SetTabStops(nTab);

	//t = _T("|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t|\t\t|\t|\t|\t|\t|\t|\t|\t");
	//m_lbOutput.AddString(t);
	s.Format(_T("\t%d\t\t%2d\t\t"), m_nPam, m_nInst);
	t.Format(_T("\t%2d\t\t%d\t\t%3d\t\t\t%d\t\t"), m_nChnl,m_nNcId, m_nThldId, m_nMId);
	s += t;
	t.Format(_T("%d\t\t\t%3d\t\t%d\t\t"),m_nNcOd, m_nThldOd, m_nMOd);
	s += t;
	t.Format(_T("%d\t\t%04d\t\t%04d\t\t\t%03d"), m_nNx, m_nMaxWall, m_nMinWall, m_nDropCnt);
	s += t;

	m_lbOutput.AddString(s);
	m_nListBoxUnsentQty++;
	// build part of a message for the PAM

	}

// Erase the last entry
void CNcNx::OnBnClickedBnErase()
	{
	// TODO: Add your control notification handler code here
	int i,j;
	CString s,t;
	char c[300];
	i = m_lbOutput.GetCount();
	for ( j = 0; j < i; j++)
		{
		m_lbOutput.GetText(j,s);
		t = s;
		CstringToChar(t,c);
		}
	if ( i > 0)
		{
		j = m_lbOutput.DeleteString(i-1);
		}
	}


// Append the info about PAM and instrument, then empty list box to a file called
// NcNxLog.txt
// copied from void CFlashProgrammer::OnBnSave()  NiosADP GUI  project
//
void CNcNx::OnBnClickedPrint2File()
	{
	// TODO: Add your control notification handler code here
	CString s, t;
	time_t Now;
	struct tm when;
	int i;
	char txt[1024];	// 8 bit characters for file write

	s = pCTscanDlg->m_szDefCfgDir;
	s += _T("\\NcNxLog.txt");

		t = s;

	CFile LogFile(s, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);

	LogFile.SeekToEnd();	// if it already exists, go to end
	s = _T("\r\n*********************************************************************************************************************************");
	CstringToChar(s,txt);
	strcat(txt,"\r\n");
	LogFile.Write(txt, strlen(txt));
	time( &Now );
	when = *localtime( &Now );
	s = asctime( &when);
	s.TrimRight();
	//s += _T("\r\n");
	CstringToChar(s,txt);
	strcat(txt,"\r\n");
	LogFile.Write(txt, strlen(txt));
	// Put column header on first line
	s = _T("    PAM    Inst         Ch     NcId     Thld        M     NcOd          Thld    M      Nx     MaxLimit    MinLimit        DropCnt");
		
	CstringToChar(s,txt);
	strcat(txt,"\r\n");
	LogFile.Write(txt, strlen(txt));

	i = 0;
	// iterate thru List Box messages and write to the file
	for ( i = 0; i < m_lbOutput.GetCount(); i++)
		{
		m_lbOutput.GetText(i,s);
		//s += _T("\r\n");
		CstringToChar(s,txt);
		strcat(txt,"\r\n");
		LogFile.Write(txt, strlen(txt));
		//LogFile.Write(s, s.GetLength());
		}

	LogFile.Close();
	s = _T("Log file ") + t + _T(" has been written");
	MessageBox(s, _T("File Save Operation"));
	CopyListBoxToMessage();	// testing

	}


void CNcNx::OnBnClickedBnSend()
	{
	// TODO: Add your control notification handler code here
	CopyListBoxToMessage();
	}


void CNcNx::OnBnClickedBnGenerate()
	{
	// TODO: Add your control notification handler code here
	//Generate canned values for Nc and Nx  Id thold = seq, Od thold=chnl number
	// 10 sequence points, 32 channels per sequence
	int nc, m, id, od;
	int seq,ch, msgcnt;
	CString s;
	int i;

	nc = 2;
	m = 3;
	msgcnt = i = 0;
	memset(&m_NcNxCmd,0,sizeof(m_NcNxCmd));
	m_NcNxCmd.bPAPNumber = m_nLastPam;
	m_NcNxCmd.bInstNumber = m_nLastInst; 
	for ( seq = 0; seq < 10; seq++)
		{
		for ( ch = 0; ch < 32; ch++)
			{
			m_NcNxCmd.stNcNx[msgcnt].bSeqNumber		= seq;
			m_NcNxCmd.stNcNx[msgcnt].bChnlNumber	= ch;
			m_NcNxCmd.stNcNx[msgcnt].bNcID			= m_nNcId;	//enable changes from window
			m_NcNxCmd.stNcNx[msgcnt].bTholdID		= 29;
			m_NcNxCmd.stNcNx[msgcnt].bModID			= m_nMId;

			m_NcNxCmd.stNcNx[msgcnt].bNcOD			= m_nNcId;
			m_NcNxCmd.stNcNx[msgcnt].bTholdOD		= 33;
			m_NcNxCmd.stNcNx[msgcnt].bModOD			= m_nMId;

			m_NcNxCmd.stNcNx[msgcnt].wNx			= 2;
			m_NcNxCmd.stNcNx[msgcnt].wWallMax		= 1377;
			m_NcNxCmd.stNcNx[msgcnt].wWallMin		= 27;
			m_NcNxCmd.stNcNx[msgcnt++].wDropOut		= 20;
			i++;
			if (msgcnt == 72)
				{
				s.Format(_T("1st Seq=%d/Chnl=%d ..Last Seq=%d/Chnl=%d\n"),
					m_NcNxCmd.stNcNx[0].bSeqNumber, m_NcNxCmd.stNcNx[0].bChnlNumber,
					m_NcNxCmd.stNcNx[71].bSeqNumber, m_NcNxCmd.stNcNx[71].bChnlNumber);
				TRACE(s);
				//Send what we have now and start a new message until all sent
				m_NcNxCmd.uSync = SYNC;
				m_NcNxCmd.wByteCount = 1460;
				m_NcNxCmd.wMsgID = 1;
				SendMsg(&m_NcNxCmd);
				memset(&m_NcNxCmd,0,sizeof(m_NcNxCmd));
				m_NcNxCmd.bPAPNumber = m_nLastPam;
				m_NcNxCmd.bInstNumber = m_nLastInst; 
				msgcnt = 0;
				}
			}
		}	// for ( seq = 0; seq < 5; seq++)

	//if msgcnt > 0 have a fragment. Send it and mark 1st nonvalid element with seqNumber = -1
	if (msgcnt)
		{
		s.Format(_T("**1st Seq=%d, Chnl=%d ..Last Seq=%d, Chnl=%d i = %d\n"),
			m_NcNxCmd.stNcNx[0].bSeqNumber, m_NcNxCmd.stNcNx[0].bChnlNumber,
			m_NcNxCmd.stNcNx[msgcnt-1].bSeqNumber, m_NcNxCmd.stNcNx[msgcnt-1].bChnlNumber, i);
		TRACE(s);
		m_NcNxCmd.wMsgID = 1;
		m_NcNxCmd.uSync = SYNC;
		m_NcNxCmd.wByteCount = (msgcnt) * 20 + 20;
		m_NcNxCmd.stNcNx[msgcnt].bSeqNumber		= -1; //terminator for message
		SendMsg(&m_NcNxCmd);
		}
	}
