// SetUp.cpp : implementation file
//
/*

06/16/00	ADD xdcr selection/assignment to setup.

*/
#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"
#include "SequenceDlg.h"
#include "AdiSetupDlg.h"
#include "PasswordDlg.h"
#include "FolderDlg.h"

#include "Extern.h"
#include "string.h"
//#define MAX_CHANNEL 60
// #include "SetUp.h  in asidlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define SEQ2			(WORD) 0x100
#define XMIT2			(WORD) 0x20
#define XMIT1			(WORD) 0x2
#define RCV2			(WORD) 0x10
#define RCV1			(WORD) 1
#define Dlog_Config_ConfigUDP		12L
extern char *st_On, *st_Off ,*st_ON, *st_OFF ;
//extern char tBuf[80];
char tBuf[80];


int lbi;		// channel type list box index
int last_lbi;	// what item was previously selected
WORD onoff_tmplate, last_onoff_tmplate, onoff_selected;
// on/off template switch and previous selection

int lbi2;		// xmit seq list box index
int last_lbi2;	// what item was previously selected

/////////////////////////////////////////////////////////////////////////////
// CSetUp dialog
void SetActiveChannel (void);

void SetActiveChannel (void)
	{
	// Using the ubiqutious 'channel' variable, make sure the channel
	// selection becomes effective in the working memory, scope mux
	// board and the instrument.
	if (pCTscanDlg == NULL) return ;
	
	else 
		{ 
		pCTscanDlg->SendMsg(CHAN_GATE_CONT);
		pCTscanDlg->SendMsg(CHANNEL_SELECT);
		pCTscanDlg->SendMsg(SCOPE_TRACE1);
		pCTscanDlg->SendMsg(SCOPE_TRACE2);
		}
	}

CSetUp::CSetUp(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUp::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CSetUp)
	m_nXdcrSel = -1;
	m_nXOffset = 0;
	m_fWOffset = 0.0f;
	//}}AFX_DATA_INIT

	m_pUndo = new CONFIG_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));

	}


void CSetUp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetUp)
	DDX_Control(pDX, IDC_SB_XOFFSET, m_sbXOffset);
	DDX_Control(pDX, IDC_SB_WOFFSET, m_sbWOffset);
	DDX_Control(pDX, IDC_CBSlvB, m_CBSlvB);
	DDX_Control(pDX, IDC_CBSlvA, m_CBSlvA);
	DDX_Control(pDX, IDC_LB_XDCR, m_lbXdcr);
	DDX_Radio(pDX, IDC_RB_NONE, m_nXdcrSel);
	DDX_Text(pDX, IDC_EN_XOFFSET, m_nXOffset);
	DDV_MinMaxInt(pDX, m_nXOffset, -48, 48);
	DDX_Check(pDX, IDC_LOCK_CHNL, m_nLock);
	DDX_Text(pDX, IDC_EN_WOFFSET, m_fWOffset);
	DDV_MinMaxFloat(pDX, m_fWOffset, -3.f, 3.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetUp, CDialog)
	//{{AFX_MSG_MAP(CSetUp)
	ON_BN_CLICKED(IDC_CFG_BT, OnCfgBt)
	ON_BN_CLICKED(IDC_DATA_BT, OnDataBt)
	ON_LBN_SELCHANGE(IDC_LB_XDCR, OnSelchangeLbXdcr)
	ON_WM_HSCROLL()
	ON_WM_HELPINFO()
	ON_CBN_SELCHANGE(IDC_CBSlvA, OnSelchangeCBSlvA)
	ON_CBN_SELCHANGE(IDC_CBSlvB, OnSelchangeCBSlvB)
	ON_BN_CLICKED(IDC_RDDst, OnRDDst)
	ON_BN_CLICKED(IDC_RdEng, OnRdEng)
	ON_BN_CLICKED(IDC_RDLen1, OnRDLen1)
	ON_BN_CLICKED(IDC_RDLen2, OnRDLen2)
	ON_BN_CLICKED(IDC_RdMtr, OnRdMtr)
	ON_BN_CLICKED(IDC_RDTme, OnRDTme)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_SETUP_UNDO, OnSetupUndo)
	ON_BN_CLICKED(IDC_SETUP_SAVE, OnSetupSave)
	ON_BN_CLICKED(IDC_LOCK_CHNL, OnLockChnl)
	ON_BN_CLICKED(IDC_SETUP_XDFLT, OnSetupXdflt)
	ON_BN_CLICKED(IDC_SETUP_WDFLT, OnSetupWdflt)
	ON_BN_CLICKED(IDC_BTN_SEQUENCE, OnBtnSequence)
	ON_BN_CLICKED(IDC_BTN_ADISETUP, OnBtnAdiSetup)
	//}}AFX_MSG_MAP

	ON_CONTROL_RANGE(BN_CLICKED, IDC_RB_NONE, IDC_RB_LAMN, DoXdcrTypeRadioButton)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetUp message handlers

void CSetUp::OnCancel() 
{
	// TODO: Add extra cleanup here
	UndoSetupChanges(m_pUndo);

	CDialog::OnCancel();
}

void CSetUp::OnOK() 
{
	// TODO: Add extra validation here
	if (!m_CfgDirName.IsEmpty() )
		pCTscanDlg->m_szDefCfgDir	= m_CfgDirName;
	if (!m_DataDirName.IsEmpty() )
		pCTscanDlg->m_szDefDataDir	= m_DataDirName;

	pCTscanDlg->SetDefaultDirectories();

	UpdateData(TRUE);	// Copy screen TO variables 
	//ConfigRec.UtRec.nDropOutMs = m_nDropOut;
//	SendMsg(SET_DROPOUT);

	SendMsg(CHANNEL_CONFIG_MSG);

	pCTscanDlg->UpdateDlgs();

	CDialog::OnOK();
	// Send Msg to instrument

}

void CSetUp::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	}
//void SendFiringSeq(int chnl);		// in instcmd.c

void display_firing_sequence(int chnl)
	{
	// Build the ASCII string in tBuf

	int tmp;
	CString s, t, sn, sf;
	sn = st_ON;
	sf = st_OFF;

	t.Format(_T("%3d  "),chnl+1);
	//wsprintf(tBuf,_T("%3d  "),chnl+1);
	tmp = ConfigRec.UtRec.Shoe[chnl/10].Ch[chnl%10].udp_mult;
	if( (tmp & XMIT1) == XMIT1)	t += sn;
		//strcat(tBuf,st_ON);
	else						t += sf;
		//lstrcat(tBuf,st_OFF);

	if( (tmp & RCV1) == RCV1)	t += sn;
		//strcat(tBuf,st_ON);
	else						t += sf;
		//strcat(tBuf,st_OFF);

	t += _T("      ");
	//strcat(tBuf,"      ");	// space between 1st group and 2nd

	if (ConfigRec.UtRec.sequence_length == 2)
		{ 
		ConfigRec.UtRec.Shoe[chnl/10].Ch[chnl%10].udp_mult |= SEQ2;
		if( (tmp & XMIT2) == XMIT2)	t += sn;
			//strcat(tBuf,st_ON);
		else						t += sf;
			//strcat(tBuf,st_OFF);
            	
		if( (tmp & RCV2) == RCV2)	t += sn;
			//strcat(tBuf,st_ON);
		else						t += sf;
			//strcat(tBuf,st_OFF);
		}
	else
		{	// seq len = 1
		ConfigRec.UtRec.Shoe[chnl/10].Ch[chnl%10].udp_mult &= ~SEQ2;
		t += _T("  --       --");
		strcat(tBuf,"  --       --");
		}	// seq len = 1

	}

void SendFiringSeq(int chnl);		// in instcmd.c


BOOL CSetUp::OnInitDialog() 
	{
	CString s;
	int si, ci, i, j;
	int tmp;
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	s = m_CfgDirName = pCTscanDlg->m_szDefCfgDir;
	GetDlgItem(IDC_CFG_EN)->SetWindowText(s);
	s = m_DataDirName = pCTscanDlg->m_szDefDataDir;
	GetDlgItem(IDC_DATA_EN)->SetWindowText(s);

	m_nXdcrSel = 0;	// none
	m_nLastIndex  = 200;
	m_nLock = TRUE;

	m_lbXdcr.ResetContent();
	j = 0;
	for ( si = 0; si < MAX_CHANNEL/10 ; si++)
		{
		for ( ci = 0; ci < 10; ci++)
			{
			switch (ConfigRec.UtRec.Shoe[si].Ch[ci].Type)
				{
				case IS_NOTHING:
				default:			i = 0;				break;

				case IS_WALL:		i = 1;				break;

				case IS_LONG:		i = 2;				break;

				case IS_TRAN:		i = 3;				break;

				case IS_OBQ1:		i = 4;				break;

				case IS_OBQ2:		i = 5;				break;
				case IS_OBQ3:		i = 6;				break;
				case IS_LAM:		i = 7;				break;
				}


			if ( i > XDCR_TYPE) i = 0;	// only 6 valid types of xdcr

			s.Format(_T("S%1d:Ch%1d  %s"), si+1, ci+1, XdcrText[i]);
			m_lbXdcr.AddString(s);
			m_MapIndexToChnl[j++] = 10*si + ci;	// index j pts to chnl 10si+ci
			}	// end of channel loop

		if ( si < (MAX_CHANNEL/10-1))
			{
			s = _T("===========");
			m_lbXdcr.AddString(s);
			m_MapIndexToChnl[j++] = 0xff;	// index j pts separator line
			}
		}	//of for

//	m_nDropOut = ConfigRec.UtRec.nDropOutMs;

		//Xmit/Recv Set up
		// Seq len 1 xmit and receive indicators

		tmp = ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].udp_mult;

		onoff_tmplate = last_onoff_tmplate = ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].udp_mult & 0x133;


	m_CBSlvA.ResetContent();
		for(tmp=0;tmp<MAX_CHANNEL;tmp++)
			{
			//wsprintf(tBuf,_T("%02d   "),tmp+1);
			s.Format(_T("%02d   "),tmp+1);
			m_CBSlvA.AddString(s);
			}
		m_CBSlvA.SetCurSel(ConfigRec.UtRec.trigger_a);

	m_CBSlvB.ResetContent();
		for(tmp=0;tmp<MAX_CHANNEL;tmp++)
			{
			s.Format(_T("%02d   "),tmp+1);
			i=m_CBSlvB.AddString(s);
			}
		m_CBSlvB.SetCurSel(ConfigRec.UtRec.trigger_b);


		/** Unit of Measure **/

		if (ConfigRec.bEnglishMetric == ENGLISH)
			CheckRadioButton(IDC_RdEng, IDC_RdMtr, IDC_RdEng);
		else
			CheckRadioButton(IDC_RdEng, IDC_RdMtr, IDC_RdMtr);

		if ( (ConfigRec.bMotionTime & 0x80) == 0 )
			CheckRadioButton(IDC_RDTme, IDC_RDDst, IDC_RDTme);
		else
			CheckRadioButton(IDC_RDTme, IDC_RDDst, IDC_RDDst);

	// SCROLL BARS

	m_sbXOffset.SetScrollRange(-48,48,TRUE);	
	m_sbXOffset.EnableScrollBar(ESB_ENABLE_BOTH);
	m_nXOffset = ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].cXOffset;
	m_sbXOffset.SetScrollPos(m_nXOffset, TRUE);

	m_sbWOffset.SetScrollRange(-15,15,TRUE);	
	m_sbWOffset.EnableScrollBar(ESB_ENABLE_BOTH);
	m_nWOffsetCnt = ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].cWOffset;
	m_sbWOffset.SetScrollPos(m_nWOffsetCnt, TRUE);
	m_fWOffset = m_nWOffsetCnt * 0.2f;

	SetDlgItemText(IDC_RB_OBQ1, ConfigRec.JobRec.Oblq1Name);
	SetDlgItemText(IDC_RB_OBQ2, ConfigRec.JobRec.Oblq2Name);
	SetDlgItemText(IDC_RB_OBQ3, ConfigRec.JobRec.Oblq3Name);

	UpdateData(FALSE);	// Copy variables to screen
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CSetUp::OnCfgBt() 
{ 
    CString strFolderPath( _T( "c:\\" ) ); // Just for sample
    CString strDisplayName( _T( "" ) );
	
	if (pCTscanDlg->m_szDefCfgDir.GetLength()) 
		strFolderPath = pCTscanDlg->m_szDefCfgDir;

    CFolderDialog dlg(  _T( "Select Default CFG Directory" ), strFolderPath, this );
    if( dlg.DoModal() == IDOK  )
    {	
        pCTscanDlg->m_szDefCfgDir=m_CfgDirName= strFolderPath  = dlg.GetFolderPath();
		GetDlgItem(IDC_CFG_EN)->SetWindowText(m_CfgDirName);
    }	
}

void CSetUp::OnDataBt() 
{ 
    CString strFolderPath( _T( "c:\\" ) ); // Just for sample
    CString strDisplayName( _T( "" ) );
	
	if (pCTscanDlg->m_szDefDataDir.GetLength()) 
		strFolderPath = pCTscanDlg->m_szDefDataDir;

    CFolderDialog dlg(  _T( "Select Default DATA Directory" ), strFolderPath, this );
    if( dlg.DoModal() == IDOK  )
    {	
        pCTscanDlg->m_szDefDataDir=m_DataDirName= strFolderPath  = dlg.GetFolderPath();
		GetDlgItem(IDC_DATA_EN)->SetWindowText(m_DataDirName);
    }	
}

void CSetUp::OnSelchangeLbXdcr() 
	{
	// TODO: Add your control notification handler code here
	CString s;
	int si, ci;
	int nXdcrUsed;		// index of type actuall used
	int i = m_lbXdcr.GetCurSel();
	si = ci = 0;

	// 04/25/02 expand use as method of selecting chnl
	// for use in setting x and w offsets
	// if chnls locked, don't change chnl assignment

	if (m_nLock)
		{
		// Only use is to set active channel
		if ( m_MapIndexToChnl[i] != 0xff)
			{	// pointing to a chnl instead of the separator
			gChannel = m_MapIndexToChnl[i];
			gShoe = gChannel/10;
			gShoeChnl = gChannel % 10;
			pCTscanDlg->ChangeActiveChannel();
			}
		UpdateOffsets();
		return;	// exit w/o changing chnl type
		}

	if ( m_nLastIndex == i)
		{	// 2nd click on same item, restore previous value
		for (nXdcrUsed = 0; nXdcrUsed < XDCR_TYPE; nXdcrUsed++)
			{// previous type index
			s = XdcrText[nXdcrUsed];
			if (m_szLast.Find(s) > 0)
				{
				m_nLastIndex = 10;	// out of bounds
				break;
				}
			}
		}
	else
		{
		m_lbXdcr.GetText(i, m_szLast);
		nXdcrUsed = m_nXdcrSel;
		m_nLastIndex = i;
		}

	if ( m_MapIndexToChnl[i] != 0xff)
		{	// pointing to a chnl instead of the separator
		gChannel = m_MapIndexToChnl[i];
		si = m_MapIndexToChnl[i]/10;
		ci = m_MapIndexToChnl[i] % 10;
		s.Format(_T("S%1d:Ch%1d  %s"), si+1, ci+1, XdcrText[nXdcrUsed]);
		m_lbXdcr.DeleteString(i);
		m_lbXdcr.InsertString(i,s);
		m_lbXdcr.SetCurSel(i);
		ConfigRec.UtRec.Shoe[si].Ch[ci].Type = XdcrType[nXdcrUsed];
		}
//	gChannel = i;
	gShoe = si;
	gShoeChnl = ci;
	pCTscanDlg->ChangeActiveChannel();
	UpdateOffsets();
	}

void CSetUp::DoXdcrTypeRadioButton(UINT nID)
	{
	int i;

	i = nID - IDC_RB_NONE;
	ASSERT( i >= 0 && i <8);
	m_nXdcrSel = i;
	}

BOOL CSetUp::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Asidlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

CSetUp::~CSetUp()
	{
	delete m_pUndo;
	}

void CSetUp::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	// TODO: Add your message handler code here and/or call default
	int nDelta;		// where the bar started and amount of change
	int nMax, nMin;
	int nStart,nCtlId;
	CString s;

	nCtlId = pScrollBar->GetDlgCtrlID();

	nStart = pScrollBar->GetScrollPos();
	pScrollBar->GetScrollRange(&nMin, &nMax);

	switch ( nSBCode)
		{	// switch on type of motion
	case SB_LINELEFT:
	case SB_PAGELEFT:
		nDelta = -1;
		break;

	case SB_LINERIGHT:
	case SB_PAGERIGHT:
		nDelta = 1;
		break;

	case SB_THUMBTRACK:
		nDelta = (int)nPos - nStart;
		break;


	default:
		nDelta = 0;		//need a value 8-27-2001
		return;
		break;

		}	// switch on type of motion

	nStart += nDelta;
	if ( nStart > nMax) nStart = nMax;
	if ( nStart < nMin) nStart = nMin;
	pScrollBar->SetScrollPos(nStart, TRUE);

	switch (nCtlId)
		{
	case IDC_SB_XOFFSET:
		ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].cXOffset = m_nXOffset =nStart;
		TRACE("gShoe=%d, gShoeChnl=%d, nStart=%d\n", gShoe,gShoeChnl, nStart);
		break;

	case IDC_SB_WOFFSET:
		ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].cWOffset = m_nWOffsetCnt = nStart;
		m_fWOffset = m_nWOffsetCnt * 0.2f;
		break;

	default:
		break;
		}
	UpdateData(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

BOOL CSetUp::OnHelpInfo(HELPINFO* pHelpInfo) 
	{
	// TODO: Add your message handler code here and/or call default
	
	return CDialog::OnHelpInfo(pHelpInfo);
	}



void CSetUp::OnSelchangeCBSlvA() 
	{
	// TODO: Add your control notification handler code here
	ConfigRec.UtRec.trigger_a = m_CBSlvA.GetCurSel();
	SendMsg(SLAVE_TRIG_RESET);
	SendMsg(SLAVE_TRIGGER);
//	SetSyncRegisters();		// in this module
	SendMsg(SET_SYNC_REG);
	
	}

void CSetUp::OnSelchangeCBSlvB() 
	{
	// TODO: Add your control notification handler code here
	ConfigRec.UtRec.trigger_b = m_CBSlvB.GetCurSel();
	SendMsg(SLAVE_TRIG_RESET);
	SendMsg(SLAVE_TRIGGER);
//	SetSyncRegisters();		// in this module
	SendMsg(SET_SYNC_REG);
	
	}

void CSetUp::OnRDDst() 
	{
	// TODO: Add your control notification handler code here
    ConfigRec.bMotionTime |= 0x80;
	}

void CSetUp::OnRdEng() 
	{
	// TODO: Add your control notification handler code here
	ConfigRec.bEnglishMetric = ENGLISH;
	
	}

void CSetUp::OnRDLen1() 
	{
	// TODO: Add your control notification handler code here
	ConfigRec.UtRec.sequence_length = 1;	
	}

void CSetUp::OnRDLen2() 
	{
	// TODO: Add your control notification handler code here
	ConfigRec.UtRec.sequence_length = 2;	
	}

void CSetUp::OnRdMtr() 
	{
	// TODO: Add your control notification handler code here
	ConfigRec.bEnglishMetric = !ENGLISH;
	
	}

void CSetUp::OnRDTme() 
	{
	// TODO: Add your control notification handler code here
	ConfigRec.bMotionTime &= 0x7F;	
	}

void CSetUp::OnHelp() 
	{
	// TODO: Add your control notification handler code here
	pCTscanDlg->PublicHelpContents();
#if 0
//char szHelpFileName[80] = "Truscope.hlp";
	GetTuboHelp((HWND)IDD_SETUP, szHelpFileName,
			HELP_CONTEXT, Dlog_Config_ConfigUDP);
#endif
	
	}


void CSetUp::OnSetupUndo() 
{
	// TODO: Add your control notification handler code here
	if (MessageBox(	_T("Undo all transducer assignment and offset changes since last SAVE operation?"),
			_T("Restore Configuration"), MB_YESNO)== IDYES)
	{
		//memcpy ( (void *) &ConfigRec, (void *) pCTscanDlg->GetUndoBuffer(), 
				//sizeof(CONFIG_REC));
		UndoSetupChanges ( pCTscanDlg->GetUndoBuffer() );
		SendMsg(CHANNEL_CONFIG_MSG);
		OnInitDialog();
	}
	// NOW RESEND INFO TO INSTRUMENT
}


void CSetUp::UndoSetupChanges(CONFIG_REC *pUndo)
{
	int si, ci;

	for (si=0; si<MAX_SHOES; si++)
	{
		for (ci=0; ci<MAX_CHANNEL_PER_INSTRUMENT; ci++)
		{
			ConfigRec.UtRec.Shoe[si].Ch[ci].Type = pUndo->UtRec.Shoe[si].Ch[ci].Type;
			ConfigRec.UtRec.Shoe[si].Ch[ci].cXOffset = pUndo->UtRec.Shoe[si].Ch[ci].cXOffset;
			ConfigRec.UtRec.Shoe[si].Ch[ci].cWOffset = pUndo->UtRec.Shoe[si].Ch[ci].cWOffset;
		}
	}
}


void CSetUp::OnSetupSave() 
	{
	// TODO: Add your control notification handler code here
	pCTscanDlg->FileSaveAs();
	}

void CSetUp::OnLockChnl() 
	{
	// Disable the channel type assignment so that chnls can be selected
	// for setting x and w offsets without changing chnl type

	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables 
	
	}

void CSetUp::OnSetupXdflt() 
	{
	// Reset all channels to default locations
	// TODO: Add your control notification handler code here
	int i, ci;

	for ( i = 0; i < 40; i++)
		{
		ci = i%10;
		//ConfigRec.UtRec.Shoe[i/10].Ch[ci].cXOffset =	5 - ci;	// 4/26/02 flip
		}
	UpdateOffsets();
	}

void CSetUp::OnSetupWdflt() 
	{
	// Reset all channels to default locations
	// TODO: Add your control notification handler code here
	int i;

	for ( i = 0; i < 40; i++)
		//ConfigRec.UtRec.Shoe[i/10].Ch[i%10].cWOffset =	0;
	UpdateOffsets();
	}

void CSetUp::UpdateOffsets()
	{
	m_nXOffset = ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].cXOffset;
	m_sbXOffset.SetScrollPos(m_nXOffset, TRUE);

	m_nWOffsetCnt = ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].cWOffset;
	m_sbWOffset.SetScrollPos(m_nWOffsetCnt, TRUE);
	m_fWOffset = m_nWOffsetCnt * 0.2f;

	UpdateData(FALSE);	// Copy variables to screen
	}

void CSetUp::OnBtnSequence() 
	{
	// TODO: Add your control notification handler code here
	// jeh jeh 27-Jul-12
	// based on code anlaysis, change this to a ptr
	//CSequenceDlg dlg;
	CSequenceDlg *pdlg = new CSequenceDlg();
	if (NULL == pdlg)
		{
		TRACE("Failed to create CSequenceDlg in OnBtnSequence()\n");
		return;
		}

	//dlg.m_pTcpThreadRxList = m_pTcpThreadRxList;
	pdlg->m_pTcpThreadRxList = m_pTcpThreadRxList;

	//dlg.DoModal();
	pdlg->DoModal();

	pCTscanDlg->ChangeActiveChannel();
	delete pdlg;
	}

void CSetUp::OnBtnAdiSetup() 
{
	// TODO: Add your control notification handler code here
	CAdiSetupDlg dlg;
	CString sPassword;
	CPasswordDlg dlgPW;

	CWinApp* pApp = AfxGetApp();
	//pApp->WriteProfileString(_T("ADMINISTRATION"), _T("PASSWORD"), _T("udp44"));

	//sPassword.Format(_T("udp44"));
	sPassword = pApp->GetProfileString(_T("ADMINISTRATION"), _T("PASSWORD"));

	dlg.m_pTcpThreadRxList = m_pTcpThreadRxList;

	if (dlgPW.DoModal() == IDOK)
	{
		if (dlgPW.m_sPassword == sPassword)
			dlg.DoModal();
		else
			AfxMessageBox(_T("The password you typed is incorrect!"));
	}
}
