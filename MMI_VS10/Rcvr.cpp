// Rcvr.cpp : implementation file
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
bool bRcvr;
/////////////////////////////////////////////////////////////////////////////
// CRcvr dialog
short gaindb;		// receiver gain in 1/10th db
//char *recv_filter[] = {"Wide Band","2.25 MHz","3.50 MHz","5.00 MHz"};
//char f_option;
//bool rf,plus,chon=0;
char *ch_on="Ch#/On";
char *ch_off="Ch#/Off";
extern char *recv_filter[];
CRcvr::CRcvr(CWnd* pParent /*=NULL*/)
	: CDialog(CRcvr::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CRcvr)
	m_sGain = _T("");
	//}}AFX_DATA_INIT
	bRcvr=TRUE;
	m_pUndo = new CONFIG_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
	}


void CRcvr::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRcvr)
	DDX_Control(pDX, IDC_RCVR_AREACOEF_SB, m_sbAreaCoef);
	DDX_Control(pDX, IDC_SB_OFFSET, m_sbOffset);
	DDX_Control(pDX, IDC_CB_fltr, m_CBfltr);
	DDX_Control(pDX, IDC_RCVR_Gain_SB, m_SBGain);
	DDX_Text(pDX, IDC_Gain, m_sGain);
	DDV_MaxChars(pDX, m_sGain, 12);
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CRcvr, CDialog)
	//{{AFX_MSG_MAP(CRcvr)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_Bn_ChOff, OnBnChOff)
	ON_BN_CLICKED(IDC_Bn_Compu, OnBnCompu)
	ON_BN_CLICKED(IDC_Rd_Mns, OnRdMns)
	ON_BN_CLICKED(IDC_Rd_Pls, OnRdPls)
	ON_CBN_SELCHANGE(IDC_CB_fltr, OnSelchangeCBfltr)
	ON_BN_CLICKED(ID_RCVR_UNDO, OnRcvrUndo)
	ON_BN_CLICKED(IDC_Rd_FW, OnRdFW)
	ON_BN_CLICKED(IDC_Rd_RF, OnRdRF)
	ON_BN_CLICKED(IDC_RD_GATE3PEAK, OnRdGate3peak)
	ON_BN_CLICKED(IDC_RD_GATE3AREA, OnRdGate3area)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRcvr message handlers
BOOL CRcvr::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Tscandlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CRcvr::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
}

void CRcvr::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	// TODO: Add your message handler code here and/or call default
	// CODE IS DIRECTION OF SCROLL MOTION
	int nDelta=0;		// where the bar started and amount of change
	int nMax, nMin;
	int dummy = 5;
	CString s;
	CString sn = ch_on;
	CString sf = ch_off;
	short *nStart;
//	float fStart;			// floating scroll bar start point
	nStart = (short*)&dummy;
	nMax=GAINMAX;
	nMin=GAINMIN;

	int nCtlId = pScrollBar->GetDlgCtrlID();
	int nPageStep;

	switch (nCtlId)
	{
	case IDC_RCVR_Gain_SB:
		nMax=GAINMAX;
		nMin=GAINMIN;
		nPageStep = GAINMAX/20;
		break;

	case IDC_SB_OFFSET:
		nMax=25;
		nMin=-25;
		nPageStep = 5;
		break;

	case IDC_RCVR_AREACOEF_SB:
		nMax=255;
		nMin=0;
		nPageStep = 5;
		break;

	default:
		break;
	}


	*nStart = pScrollBar->GetScrollPos();
	pScrollBar->GetScrollRange(&nMin, &nMax);

	// Adjusting the gain turns off the TCG
	//ConfigRec.receiver.tcg_enable[gChannel] = 0;

	switch ( nSBCode)
		{	// switch on type of motion
	case SB_LINELEFT:
		nDelta = -1;
		break;

	case SB_LINERIGHT:
		nDelta = 1;
		break;

	case SB_PAGELEFT:
		nDelta -= nPageStep;
		break;

	case SB_PAGERIGHT:
		nDelta += nPageStep;
		break;

	case SB_THUMBTRACK:
		nDelta = (int)nPos - *nStart;
		break;

	default:
		nDelta = 0;		//need a value 8-27-2001
		return;
		break;

		}	// switch on type of motion

	*nStart += nDelta;
	if ( *nStart > nMax) *nStart = nMax;
	if ( *nStart < nMin) *nStart = nMin;

	pScrollBar->SetScrollPos(*nStart, TRUE);

	switch (nCtlId)
	{
	case IDC_RCVR_Gain_SB:
		gaindb = *nStart;
		ConfigRec.receiver.gain[gChannel] = gaindb;
	//	if (gaindb > GAINMAX) gaindb=GAINMAX;
	//	if (gaindb < GAINMIN) gaindb=GAINMIN;
	//	gaindb /= 10.0;		// 1/10th db
        m_sGain.Format(_T("%-7.1f"),(float) gaindb/10);
		UpdateData(FALSE);
		//ConfigRec.receiver.tcg_enable[gChannel]=0;
		SetDlgItemText(IDC_Bn_ChOff,sf);
		pCTscanDlg->UpdateChnlSelected();
		SendMsg(RECEIVER_GAIN);
		break;

	case IDC_SB_OFFSET:
		ConfigRec.receiver.offset[gChannel] = *nStart;
		SetDlgItemInt(IDC_EDIT_OFFSET, *nStart, TRUE);
		SendMsg(RECEIVER_FCNT);
		break;

	case IDC_RCVR_AREACOEF_SB:
		ConfigRec.receiver.AreaCoef[gChannel] = (BYTE) *nStart;
		SetDlgItemInt(IDC_RCVR_AREACOEF, *nStart, TRUE);
		SendMsg(RECEIVER_PROCESS);
		break;

	default:
		break;
	}

//	m_SBGain.SetScrollPos(ConfigRec.receiver.gain[gChannel],TRUE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

BOOL CRcvr::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	CString s;
	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	if ( bRcvr )
		{
		bRcvr = FALSE;
		GetWindowPlacement(&wp);
		dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		pCTscanDlg->GetWindowLastPosition(_T("RECEIVER"), &rect);
		if ( ( (rect.right - rect.left) >= dx ) &&
			 ( (rect.bottom - rect.top) >= dy ))
			{
			wp.rcNormalPosition = rect;
			SetWindowPlacement(&wp);
			}
		}
	
	// TODO: Add extra initialization here
    s.Format(_T("    ch=%2d  Receiver"),gChannel +1);
	SetWindowText(s);
	m_SBGain.SetScrollRange(GAINMIN,GAINMAX,TRUE);
	m_SBGain.EnableScrollBar(ESB_ENABLE_BOTH);
//	m_SBGain.SetScrollPos(ConfigRec.receiver.gain[gChannel],TRUE);
	gaindb = ConfigRec.receiver.gain[gChannel];
	if (gaindb > GAINMAX) gaindb=GAINMAX;
	if (gaindb < GAINMIN) gaindb=GAINMIN;
	m_SBGain.SetScrollPos(gaindb,TRUE);
	//	gaindb /= 10.0;		// 1/10th db
//    m_sGain.Format(_T("%-7.1f db",(float) gaindb/10);
    m_sGain.Format(_T("%-7.1f"),(float) gaindb/10);
	// Set polarity radio button
	if(ConfigRec.gates.polarity[gChannel][gGate])
		CheckRadioButton(IDC_Rd_Pls,IDC_Rd_Mns ,
						IDC_Rd_Mns);
	else
		CheckRadioButton(IDC_Rd_Pls,IDC_Rd_Mns ,
						IDC_Rd_Pls);

	// Set detector radio buttons
	if ( ConfigRec.gates.det_option[gChannel][gGate])
		CheckRadioButton(IDC_Rd_RF, IDC_Rd_FW, IDC_Rd_FW);
	else
		CheckRadioButton(IDC_Rd_RF, IDC_Rd_FW, IDC_Rd_RF);

	// Set Gate 3 process radio buttons
	if ( ConfigRec.receiver.process[gChannel] == 0)
		CheckRadioButton(IDC_RD_GATE3PEAK, IDC_RD_GATE3AREA, IDC_RD_GATE3PEAK);
	else
		CheckRadioButton(IDC_RD_GATE3PEAK, IDC_RD_GATE3AREA, IDC_RD_GATE3AREA);

	// area coef scroll bar initialization
	m_sbAreaCoef.SetScrollRange(0,255,TRUE);
	m_sbAreaCoef.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbAreaCoef.SetScrollPos(ConfigRec.receiver.AreaCoef[gChannel],TRUE);
	SetDlgItemInt(IDC_RCVR_AREACOEF, ConfigRec.receiver.AreaCoef[gChannel], TRUE);

	// Init Filter combo box text
	m_CBfltr.ResetContent( );
	for ( short i=0; i< 4;i++){
		s = recv_filter[i];
		m_CBfltr.AddString(s);
	}
	if (ConfigRec.receiver.fil_option[gChannel] <0) ConfigRec.receiver.fil_option[gChannel]=0;
	m_CBfltr.SetCurSel(ConfigRec.receiver.fil_option[gChannel]%4);
//	chon=ConfigRec.receiver.tcg_enable[gChannel]?TRUE:FALSE;
	//if ( ConfigRec.receiver.tcg_enable[gChannel] ) s=ch_on;
		//else s=ch_off;
	SetDlgItemText(IDC_Bn_ChOff,s);

	// receiver offset scroll bar initialization
	m_sbOffset.SetScrollRange(-25,25,TRUE);
	m_sbOffset.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbOffset.SetScrollPos(ConfigRec.receiver.offset[gChannel],TRUE);
	SetDlgItemInt(IDC_EDIT_OFFSET, ConfigRec.receiver.offset[gChannel], TRUE);

	UpdateData(FALSE);

	SetDefID(-1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CRcvr::OnBnChOff() 
{
	// TODO: Add your control notification handler code here

	CString s;
	//ConfigRec.receiver.tcg_enable[gChannel] ^= 1;
	//if ( ConfigRec.receiver.tcg_enable[gChannel] ) s=ch_on;
		//else s=ch_off;
	SetDlgItemText(IDC_Bn_ChOff,s);
	SendMsg(TCG_ENABLE);
	pCTscanDlg->UpdateChnlSelected();

}

void CRcvr::OnBnCompu() 
{
	// TODO: Add your control notification handler code here
	//ConfigRec.receiver.tcg_enable[gChannel]=1;
//	SetDlgItemText(IDC_Bn_ChOff,ch_on);
	SendMsg(RECEIVER_GAIN);
	SendMsg(TCG_ARG1);
	SendMsg(TCG_ARG2);
	SendMsg(TCG_COMPUTE);
	pCTscanDlg->UpdateChnlSelected();
}

void CRcvr::OnRdFW() 
	{
	// TODO: Add your control notification handler code here
	int nRfFw = GetCheckedRadioButton(IDC_Rd_RF, IDC_Rd_FW) - IDC_Rd_RF;
	if ( nRfFw != 1) return;

	// TODO: Add your control notification handler code here
	//ConfigRec.receiver.det_option[gChannel]=0;
	ConfigRec.gates.det_option[gChannel][gGate]=1;
	SendMsg(RECEIVER_RF_VIDEO);
	CheckRadioButton(IDC_Rd_RF,IDC_Rd_FW,IDC_Rd_FW);
	UpdateData(FALSE);

	if (pCTscanDlg->m_pAscanDlg)
		(pCTscanDlg->m_pAscanDlg)->SetRfFullWaveMode();
	}

void CRcvr::OnRdMns() 
{
	int nPM = GetCheckedRadioButton(IDC_Rd_Pls, IDC_Rd_Mns) - IDC_Rd_Pls;
	if ( nPM != 1) return;

	// TODO: Add your control notification handler code here
	ConfigRec.receiver.polarity[gChannel]=0;
	ConfigRec.gates.polarity[gChannel][gGate] = 1;
	SendMsg(RECEIVER_POLT);
}

void CRcvr::OnRdPls() 
{
	int nPM = GetCheckedRadioButton(IDC_Rd_Pls, IDC_Rd_Mns) - IDC_Rd_Pls;
	if ( nPM != 0) return;

	// TODO: Add your control notification handler code here
	ConfigRec.receiver.polarity[gChannel]=1;
	ConfigRec.gates.polarity[gChannel][gGate] = 0;
	SendMsg(RECEIVER_POLT);
}

void CRcvr::OnRdRF() 
	{
	// TODO: Add your control notification handler code here
	int nRfFw = GetCheckedRadioButton(IDC_Rd_RF, IDC_Rd_FW) - IDC_Rd_RF;
	if ( nRfFw != 0) return;

	// TODO: Add your control notification handler code here
	//ConfigRec.receiver.det_option[gChannel]=1;
	ConfigRec.gates.det_option[gChannel][gGate]=0;
	SendMsg(RECEIVER_RF_VIDEO);
	CheckRadioButton(IDC_Rd_RF,IDC_Rd_FW,IDC_Rd_RF);
	UpdateData(FALSE);

	if (pCTscanDlg->m_pAscanDlg)
		(pCTscanDlg->m_pAscanDlg)->SetRfFullWaveMode();
	}

void CRcvr::OnOK() 
{
	// TODO: Add extra validation here
//	ConfigRec.receiver.gain[gChannel]=gaindb;
//	ConfigRec.receiver.fil_option[gChannel]	= f_option;
//	ConfigRec.receiver.polarity[gChannel]=plus;	
//	ConfigRec.receiver.det_option[gChannel]=rf;
//	ConfigRec.receiver.tcg_enable[gChannel]=chon;
	Save_Pos();
	CDialog::OnOK();
	CDialog::DestroyWindow();

}

void CRcvr::OnCancel() 
	{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CRcvr::KillMe()
	{
	// Public access to OnCancel
	// Add to every modeless dialog
	OnCancel();
	}

void CRcvr::OnSelchangeCBfltr() 
	{
	// TODO: Add your control notification handler code here
	SendMsg(RECEIVER_FILT);
	ConfigRec.receiver.fil_option[gChannel] = m_CBfltr.GetCurSel();
	SendMsg(RECEIVER_FILT);
	}

void CRcvr::Save_Pos()
	{
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition(_T("RECEIVER"), &wp);
//	bCtof = FALSE;

}


CRcvr::~CRcvr()
{

	delete m_pUndo;
}

void CRcvr::OnRcvrUndo() 
{
	// TODO: Add your control notification handler code here
	if (MessageBox(	_T("Undo all receiver changes since last SAVE operation?"),
			_T("Restore Configuration"),MB_YESNO)== IDYES)
	{
			//memcpy ( (void *) &ConfigRec, (void *) m_pUndo, sizeof(CONFIG_REC));
			UndoRcvrSettings();
	}

	OnInitDialog();	
}


void CRcvr::UndoRcvrSettings()
{
	int iChannel, iGate;
	int ic, ig;
	iChannel = gChannel;
	iGate = gGate;
	for (ic=0; ic<MAX_CHANNEL; ic++)  //channel loop
	{
		gChannel = ic;
		SendMsg(CHANNEL_SELECT);

		ConfigRec.receiver.gain[ic] = m_pUndo->receiver.gain[ic] ;
		SendMsg(RECEIVER_GAIN);

		ConfigRec.receiver.fil_option[ic] = m_pUndo->receiver.fil_option[ic];
		SendMsg(RECEIVER_FILT);

		ConfigRec.receiver.offset[ic] = m_pUndo->receiver.offset[ic];
		SendMsg(RECEIVER_FCNT);

		for (ig=0; ig<MAX_GATE; ig++)  //gate loop
		{
			gGate = ig;
			pCTscanDlg->SendMsg(GATE_SELECT);

			ConfigRec.gates.det_option[ic][ig] = m_pUndo->gates.det_option[ic][ig] ;
			SendMsg(RECEIVER_RF_VIDEO);

			ConfigRec.gates.polarity[ic][ig] = m_pUndo->gates.polarity[ic][ig] ;
			SendMsg(RECEIVER_POLT);
		}
	}

	gChannel = iChannel;
	SendMsg(CHANNEL_SELECT);

	gGate = iGate;
	SendMsg(GATE_SELECT);
}

void CRcvr::OnRdGate3peak() 
{
	// TODO: Add your control notification handler code here
	int nProc = GetCheckedRadioButton(IDC_RD_GATE3PEAK, IDC_RD_GATE3AREA) - IDC_RD_GATE3PEAK;
	if ( nProc != 0) return;

	// TODO: Add your control notification handler code here
	ConfigRec.receiver.process[gChannel] = 0;
	SendMsg(RECEIVER_PROCESS);
	CheckRadioButton(IDC_RD_GATE3PEAK, IDC_RD_GATE3AREA, IDC_RD_GATE3PEAK);
	UpdateData(FALSE);
}

void CRcvr::OnRdGate3area() 
{
	// TODO: Add your control notification handler code here
	int nProc = GetCheckedRadioButton(IDC_RD_GATE3PEAK, IDC_RD_GATE3AREA) - IDC_RD_GATE3PEAK;
	if ( nProc != 1) return;

	// TODO: Add your control notification handler code here
	ConfigRec.receiver.process[gChannel] = 1;
	SendMsg(RECEIVER_PROCESS);
	CheckRadioButton(IDC_RD_GATE3PEAK, IDC_RD_GATE3AREA, IDC_RD_GATE3AREA);
	UpdateData(FALSE);
}
