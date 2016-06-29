// Pulser.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
// include only the parent dialog header asidlg.h for all child dialogs
#include "TscanDlg.h"
#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
bool bPulsr;
/////////////////////////////////////////////////////////////////////////////
// CPulser dialog
short *pval, pmin, pmax, pdelta;
extern char *st_sysOn,*st_sysOff,*st_chnOn,*st_chnOff;
extern bool bDUpdate;		//update dialog only
/*	*st_sysOn = 		"SysOn";
char *st_sysOff = 		"SysOff";
char *st_chnOn = "Ch#/On";
char *st_chnOff = "Ch#/Off";
*/
char xchk = XMT_BITS;	// 0x22;
char rchk = 0; //RCV_BITS;	// 0x11;	
						// default to pulse on/off operation on both xmit and receive
						// not saved in config file
char OnMask = 0x33;	// assume sequence length is 2.  If any rcvr or xmit is on
					// then the channel can be turned on.  Else can not be turned
					// on

void set_sync_register1()
	{
//	int tmpchnl;
	/* One sync register per instrument.  Controls slave trigger bus and
		bus terminations.
	*/

	pCTscanDlg->SendMsg(LOCAL_PRF_CONTROL);		//local_prf();		// Set local prf info
	pCTscanDlg->SendMsg(PULSER_PRF)	;			//pulser_prf();		// set pulser mode and rate for this instrument

#ifndef  TWO_CHANNEL_4_GATE_BOARD

		{
		pCTscanDlg->SendMsg(CHANNEL_SELECT);
		pCTscanDlg->SendMsg(SET_SYNC_REG);
		}
		pCTscanDlg->SendMsg(CHANNEL_SELECT);

#endif
	};

void SetSyncRegisters ( void)
	{
	int i;
	WORD sync_mask;

	// Initial mode is slave in, pass thru or end of chain for all
	// lines and boxes
	sync_mask = 0x6cc;

	if ( (ConfigRec.pulser.mode == 0) &&		// prf mode
		 (ConfigRec.pulser.LocalPRF[gChannel/10]) )		// for testing, internal prf
		 sync_mask = 0x4cc;

	for ( i = 0; i < 4; i++) ConfigRec.UtRec.SyncControl[i] = sync_mask;
	}

CPulser::CPulser(CWnd* pParent /*=NULL*/)
	: CDialog(CPulser::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CPulser)
	m_Swdth = _T("");
	m_Spp = _T("");
	m_ss = _T("");
	//}}AFX_DATA_INIT
	bPulsr=TRUE;

	m_pUndo = new CONFIG_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
	}

void CPulser::Idle(long time)
	{
	m_ptheApp->OnIdle(time);
	}

void CPulser::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPulser)
	DDX_Control(pDX, IDC_BTNSyOn, m_BTNSOn);
	DDX_Control(pDX, IDC_BTNCHOn, m_OnOff);
	DDX_Control(pDX, IDC_SBWdth, m_SBwdth);
	DDX_Control(pDX, IDC_SB_PP, m_SBpp);
	DDX_Text(pDX, IDC_Wdth, m_Swdth);
	DDX_Text(pDX, IDC_PP, m_Spp);
	DDV_MaxChars(pDX, m_Spp, 12);
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CPulser, CDialog)
	//{{AFX_MSG_MAP(CPulser)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTNCHOn, OnBTNCHOn)
	ON_BN_CLICKED(IDC_RDPls, OnRDPls)
	ON_BN_CLICKED(IDC_RDPrf, OnRDPrf)
	ON_BN_CLICKED(IDC_RDPrfI, OnRDPrfI)
	ON_BN_CLICKED(IDC_RDPrfx, OnRDPrfx)
	ON_BN_CLICKED(IDC_CkRcv, OnCkRcv)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_BN_CLICKED(IDC_CkPls, OnCkPls)
	ON_BN_CLICKED(IDC_BTNSyOn, OnBTNSyOn)
	ON_BN_CLICKED(IDC_RD_EXTTTL, OnRdExtTTL)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPulser::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Tscandlg window.
//	if (bDUpdate) return FALSE;		//do not send if upadte dialog only
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}


void CPulser::OnOK() 
	{
	// TODO: Add extra validation here
	if ( !IsSysFireSeqOn() )
		SetSysPulserOnOff(TRUE);
	if ( !IsSysRcvrSeqOn() )
		SetSysRcvrOnOff(TRUE);
	Save_Pos();	
	SendMsg(PULSER_ONOFF);
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

void CPulser::OnCancel() 
	{
	// TODO: Add extra cleanup here
	if ( !IsSysFireSeqOn() )
		SetSysPulserOnOff(TRUE);
	if ( !IsSysRcvrSeqOn() )
		SetSysRcvrOnOff(TRUE);
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CPulser::KillMe()
	{
	// Public access to OnCancel
	// Add to every modeless dialog
	OnCancel();
	}


void CPulser::OnUndo() 
{
	// TODO: Add your control notification handler code here
	if (MessageBox(	_T("Undo all pulser changes since last SAVE operation?"),
			_T("Restore Configuration"),MB_YESNO)== IDYES)
	{
		//memcpy ( (void *) &ConfigRec, (void *) m_pUndo, sizeof(CONFIG_REC));
		UndoPulserSettings();
	}

	Save_Pos();
	OnInitDialog();	
}


void CPulser::UndoPulserSettings()
{
	int iChannel;
	int is, ic;
	iChannel = gChannel;

	ConfigRec.pulser.prf = m_pUndo->pulser.prf ;
	SendMsg(PULSER_PRF);

	for (is=0; is<MAX_SHOES; is++)  //instrument loop
	{
		ConfigRec.pulser.LocalPRF[is] = m_pUndo->pulser.LocalPRF[is];
		m_pTcpThreadRxList->SetPrfIntExt(is);
	}

	for (ic=0; ic<MAX_CHANNEL; ic++)  //channel loop
	{
		gChannel = ic;
		SendMsg(CHANNEL_SELECT);

		if ( ConfigRec.pulser.pulse_width[ic] != m_pUndo->pulser.pulse_width[ic] )
		{
			ConfigRec.pulser.pulse_width[ic] = m_pUndo->pulser.pulse_width[ic];
			SendMsg(PULSER_WIDTH);
		}
	}

	gChannel = iChannel;
	SendMsg(CHANNEL_SELECT);
}


void CPulser::UpdateUndoBuffer()
{
	int iChannel;
	int is, ic;
	iChannel = gChannel;

	m_pUndo->pulser.prf = ConfigRec.pulser.prf;

	for (is=0; is<MAX_SHOES; is++)  //instrument loop
	{
		m_pUndo->pulser.LocalPRF[is] = ConfigRec.pulser.LocalPRF[is];
	}

	for (ic=0; ic<MAX_CHANNEL; ic++)  //channel loop
	{
		m_pUndo->pulser.pulse_width[ic] = ConfigRec.pulser.pulse_width[ic];
	}

	UpdateDlg();
}


CPulser::~CPulser()
	{
	delete m_pUndo;
	}

/////////////////////////////////////////////////////////////////////////////
// CPulser message handlers


void CPulser::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;

	}

BOOL CPulser::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	CString s;
	// TODO: Add extra initialization here
	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	if ( bPulsr )
		{
		bPulsr = FALSE;
		GetWindowPlacement(&wp);
		dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		pCTscanDlg->GetWindowLastPosition(_T("PULSER"), &rect);
		if ( ( (rect.right - rect.left) >= dx ) &&
			 ( (rect.bottom - rect.top) >= dy ))
			{
			wp.rcNormalPosition = rect;
			SetWindowPlacement(&wp);
			}
		}

	if (ConfigRec.pulser.mode == 0)
	{	// absolute prf mode
	//pval = (short *) &ConfigRec.pulser.prf;
	pmin = PRFMIN;
	pmax = PRFMAX;
//			pdelta = PRFMAX/10;
	
	CheckRadioButton(IDC_RDPrf,IDC_RDPls,IDC_RDPrf);
	m_SBpp.SetScrollRange(PRFMIN,PRFMAX,TRUE);
	if(ConfigRec.pulser.prf < PRFMIN)
		ConfigRec.pulser.prf = PRFMIN;
	if(ConfigRec.pulser.prf > PRFMAX)
		ConfigRec.pulser.prf = PRFMAX;
	m_SBpp.EnableScrollBar(ESB_ENABLE_BOTH);

	m_SBpp.SetScrollPos(ConfigRec.pulser.prf,TRUE);
/*			sprintf(tBuf,
			szPuls_Hz,
			ConfigRec.pulser.prf*10);
	// add comma for thousands
*/
	m_Spp.Format(_T("%7d Hz"),ConfigRec.pulser.prf*10);
	if(m_Spp[3] != ' ')
		{
		s=m_Spp.Left(4);
		s += _T(',');		//add a coma
		s += m_Spp.Mid(4);	// move 1st 3 char's left
		m_Spp = s;
		}
	}	// absolute prf mode

	else
	{	// pulse density mode
		ConfigRec.pulser.mode = 1;
		//pval = (short *) &ConfigRec.pulser.density;
		pmin = PDMIN;
		pmax = PDMAX;
	//			pdelta = 20;	// 0.020"
		
		CheckRadioButton(IDC_RDPrf,IDC_RDPls,IDC_RDPls);
		m_SBpp.SetScrollRange(PDMIN,PDMAX,TRUE);

		if(ConfigRec.pulser.prf < PDMIN)
			ConfigRec.pulser.prf = PDMIN;
		if(ConfigRec.pulser.prf > PRFMAX)
			ConfigRec.pulser.prf = PRFMAX;
		m_SBpp.EnableScrollBar(ESB_ENABLE_BOTH);

		m_SBpp.SetScrollPos(ConfigRec.pulser.density,TRUE);
//			sprintf(tBuf,"0.%03d inch",ConfigRec.pulser.density);
		if ( ConfigRec.bEnglishMetric == ENGLISH)
			{
	//		f = ConfigRec.pulser.density;
//			m_Spp.Format(_T("0.%03d inch"), ConfigRec.pulser.density );
			m_Spp.Format(_T("0.040 inch"));
			}
		else
			{
	//		f =  25.4f * ConfigRec.pulser.density;
//			m_Spp.Format(_T("0.%03f mm"), 2.54f * ConfigRec.pulser.density );
			m_Spp.Format(_T("1.02 mm"));
			}
	}	// pulse density mode
		m_SBwdth.SetScrollRange(PFWIDMIN,PFWIDMAX,TRUE);
		m_SBwdth.EnableScrollBar(ESB_ENABLE_BOTH);
		if (ConfigRec.pulser.pulse_width[gChannel] > PFWIDMAX)
			ConfigRec.pulser.pulse_width[gChannel] = PFWIDMAX;
		if (ConfigRec.pulser.pulse_width[gChannel] < PFWIDMIN)
			ConfigRec.pulser.pulse_width[gChannel] = PFWIDMIN;

		m_SBwdth.SetScrollPos(ConfigRec.pulser.pulse_width[gChannel],TRUE);
		m_Swdth.Format(_T("%7d ns"),ConfigRec.pulser.pulse_width[gChannel]*10);
			if(m_Swdth[3] != ' ')
				{
				s=m_Swdth.Left(4);
				s += _T(',');		//add a coma
				s += m_Swdth.Mid(4);	// move 1st 3 char's left
				m_Swdth = s;
			}

		/********** Pulser on/off ... usually for testing ************/

		if (ConfigRec.UtRec.sequence_length == 2)
			{	// look at both xmit and rcv points to determine if chananel
				// can be turned on
			OnMask = 0x33; 
			}
		else OnMask = 0x3;	// only 1st point considered for on/off

		ConfigRec.pulser.pulse_on &= (XMT_BITS | RCV_BITS | PLS_ON_BIT);
		ConfigRec.pulser.pulse_on |= (rchk | xchk);

		if ( (ConfigRec.pulser.pulse_on & PLS_ON_BIT) ||
			 ( (ConfigRec.pulser.pulse_on & ( XMT_BITS | RCV_BITS)) == 0) )
			{
			s = st_sysOn;
			ConfigRec.pulser.pulse_on |= PLS_ON_BIT;
			SetDlgItemText(IDC_BTNSyOn,s);
			}
		else
			{
			s = st_sysOff;
			SetDlgItemText(IDC_BTNSyOn,(s));
			}


		ConfigRec.pulser.pulse_chn_on[gChannel] &= (XMT_BITS | RCV_BITS | PLS_ON_BIT);
		ConfigRec.pulser.pulse_chn_on[gChannel] |= (rchk | xchk);
		if (ConfigRec.pulser.pulse_chn_on[gChannel] & PLS_ON_BIT)
			{	// This button push RESTORES configuration set in udpconfig
			if ( (ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].udp_mult & OnMask) )
				{
				s = st_chnOn;
				SetDlgItemText(IDC_BTNCHOn,s);
				}

			else	// no rcvr or xmiter is on... can not override
				{
				ConfigRec.pulser.pulse_chn_on[gChannel] |= ~PLS_ON_BIT;
				s = st_chnOff;
				SetDlgItemText(IDC_BTNCHOn,s);
				}
			}

		else
			{	// this button push should take us to off
			if (ConfigRec.pulser.pulse_chn_on[gChannel] & ( XMT_BITS | RCV_BITS))
				{
				s = st_chnOff;
				SetDlgItemText(IDC_BTNCHOn,s);
				}
			else
				// not operating on any bits
				{
				if ( (ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].udp_mult & OnMask) )
					{
					s = st_chnOn;
					SetDlgItemText(IDC_BTNCHOn,s);
					ConfigRec.pulser.pulse_chn_on[gChannel] |= PLS_ON_BIT;
					}
				else
					{
					s = st_chnOff;
					SetDlgItemText(IDC_BTNCHOn,s);
					}
				}
			}

		SetSysOnOffBtn();
		SetChnlOnOffBtn(gChannel);

		// set pulser on/off check boxes for receiver/xmitter options
		if (xchk != 0)
			CheckDlgButton(IDC_CkPls,1);
		else
			CheckDlgButton(IDC_CkPls,0);
		
		if (rchk != 0)
			CheckDlgButton(IDC_CkRcv,1);
		else
			CheckDlgButton(IDC_CkRcv,0);


		// Set PRF internal/external radio buttons
		//ConfigRec.pulser.LocalPRF[0] = 1;
		if (ConfigRec.pulser.LocalPRF[gChannel/10] == 1)
			CheckRadioButton(IDC_RDPrfI,IDC_RD_EXTTTL,IDC_RDPrfI);
		else if (ConfigRec.pulser.LocalPRF[gChannel/10] == 0)
			CheckRadioButton(IDC_RDPrfI,IDC_RD_EXTTTL,IDC_RDPrfx);
		else if (ConfigRec.pulser.LocalPRF[gChannel/10] == 3)
			CheckRadioButton(IDC_RDPrfI,IDC_RD_EXTTTL,IDC_RD_EXTTTL);

		SendMsg(PULSER_ONOFF);
//			ProcPtrArray[PULSER_ONOFF]();
		UpdateData(FALSE);
		m_ptheApp = (CTscanApp *) AfxGetApp();
		
		SetDefID(-1);

		return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPulser::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	// CODE IS DIRECTION OF SCROLL MOTION
	int nDelta;		// where the bar started and amount of change
	int nMax = 10;
	int nMin = 0;
	int nCtlId;
	int dummy = 5;
	CString s;
	short *nStart;
//	float fStart;			// floating scroll bar start point
	nStart = (short *) &dummy;

	nCtlId = pScrollBar->GetDlgCtrlID();

	switch (nCtlId)
		{
	case IDC_SB_PP:
		nMin=pmin;
		nMax=pmax;
		if (ConfigRec.pulser.mode == 0)
		{
			pdelta = PRFMAX/10;

		} else {
			pdelta = 20;
		}

		break;
	case IDC_SBWdth:
		nMin=PFWIDMIN;
		nMax=PFWIDMAX;
		pdelta = 10;
		break;

	default:
//		nStart = (short*)&dummy;
		break;
		}

	*nStart = pScrollBar->GetScrollPos();
//	pScrollBar->GetScrollRange(&nMin, &nMax);
//	pScrollBar->SetScrollRange(nMin, nMax);

	switch ( nSBCode)
		{	// switch on type of motion
	case SB_LINEUP:
		nDelta = -1;
		break;

	case SB_LINEDOWN:
		nDelta = 1;
		break;

	case SB_PAGEUP:
		nDelta = -pdelta;	//-(nMax - nMin)/10;
		break;

	case SB_PAGEDOWN:
		nDelta = pdelta;	//(nMax - nMin)/10;
		break;

	case SB_THUMBTRACK:
		return;
		nDelta = (int)nPos - *nStart;
		break;

	case SB_THUMBPOSITION:
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

//	m_sDelay.Format ("%.3f   in", *nStart/1000.0);

//	UpdateData(TRUE);

	switch (nCtlId)
		{
	case IDC_SB_PP:
	if (ConfigRec.pulser.mode == 0)
	{
		ConfigRec.pulser.prf=*nStart;
			m_Spp.Format(_T("%7d Hz"),ConfigRec.pulser.prf*10);
			if(m_Spp[3] != ' ')
				{
				s=m_Spp.Left(4);
				s += _T(',');		//add a coma
				s += m_Spp.Mid(4);	// move 1st 3 char's left
				m_Spp = s;
				}

//			SendMsg(PULSER_PRF);			// Set gate delay
	}
	else
	{
//		ConfigRec.pulser.density=*nStart;
//			m_Spp.Format(_T("0.%03d inch", ConfigRec.pulser.density );
		if ( ConfigRec.bEnglishMetric == ENGLISH)
			{
	//		f = ConfigRec.pulser.density;
//			m_Spp.Format(_T("0.%03d inch", ConfigRec.pulser.density );
			m_Spp.Format(_T("0.040 inch"));
			}
		else
			{
	//		f =  25.4f * ConfigRec.pulser.density;
//			m_Spp.Format(_T("0.%03f mm", 2.54f * ConfigRec.pulser.density );
			m_Spp.Format(_T("1.02 mm"));
			}
/*		ftmp = (float) ConfigRec.pipeinfo.Diameter;
		fcir = 3.1416f*ftmp;
		fden = (float) ConfigRec.pulser.density;
		fden /= 1000.0f;
		ConfigRec.pulser.PulsePerRev = (WORD) ((fcir/fden)+0.5);
*/

//	SendMsg(SET_SYNC_REG);
	}
	SetSyncRegisters();
	set_sync_register1();
		break;
	case IDC_SBWdth:
		// Send gate delay to instrument
		ConfigRec.pulser.pulse_width[gChannel]=*nStart;
		m_Swdth.Format(_T("%7d ns"),ConfigRec.pulser.pulse_width[gChannel]*10);
			if(m_Swdth[3] != ' ')
				{
				s=m_Swdth.Left(4);
				s += _T(',');		//add a coma
				s += m_Swdth.Mid(4);	// move 1st 3 char's left
				m_Swdth = s;
			}
	SetSyncRegisters();
	set_sync_register1();

	SendMsg(PULSER_WIDTH);			// Set gate delay
	default:
		break;
		}
	UpdateData(FALSE);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPulser::OnBTNCHOn() 
{
	// TODO: Add your control notification handler code here
//	if (rchk || xchk)
	{
	// system global pulser on/off  PUSH BUTTONI

		ConfigRec.pulser.pulse_chn_on[gChannel] ^= PLS_ON_BIT;	// toggle button
		ConfigRec.pulser.pulse_chn_on[gChannel] &= ~(XMT_BITS | RCV_BITS);
			// remove old xmt/rcv setting
		ConfigRec.pulser.pulse_chn_on[gChannel] |= (rchk | xchk);
			// latch in new xmt/rcv settings
		OnInitDialog();	
//		SendMessage(hDlg,WM_INITDIALOG,0,0L);
		SendMsg(PULSER_ONOFF);
	}

	if (xchk)
	{
		if ( IsFireSequenceOn(gChannel/10, gChannel%10) )
			SetChnlPulserOnOff(gChannel,FALSE);
		else
			SetChnlPulserOnOff(gChannel,TRUE);
	}

	if (rchk)
	{
		if ( IsRcvrSequenceOn(gChannel/10, gChannel%10) )
			SetChnlRcvrOnOff(gChannel,FALSE);
		else
			SetChnlRcvrOnOff(gChannel,TRUE);
	}

	//SetChnlOnOffBtn(gChannel);
	UpdateDlg();
}

void CPulser::OnRDPls() 
	{
	// TODO: Add your control notification handler code here
	ConfigRec.pulser.mode = 1;
//	CheckRadioButton(IDC_RDPrf,IDC_RDPls,IDC_RDPrf);
	SetSyncRegisters();
	set_sync_register1();
//	SendMsg(SET_SYNC_REG);
	SendMsg(PULSER_ONOFF);
	pCTscanDlg->UpdateChnlSelected();
	}

void CPulser::OnRDPrf() 
	{
	// TODO: Add your control notification handler code here
	ConfigRec.pulser.mode = 0;	
//	CheckRadioButton(IDC_RDPrf,IDC_RDPls,IDC_RDPls);
	SetSyncRegisters();
	set_sync_register1();
//	SendMsg(SET_SYNC_REG);
	SendMsg(PULSER_ONOFF);
	pCTscanDlg->UpdateChnlSelected();
	}

void CPulser::OnRDPrfI() 
	{
	// TODO: Add your control notification handler code here
#if 0
	if (gChannel < 10)
	{
		CheckRadioButton(IDC_RDPrfI,IDC_RDPrfx,IDC_RDPrfI);
		return;
	}
#endif
	ConfigRec.pulser.LocalPRF[gChannel/10] = 1;
//	CheckRadioButton(IDC_RDPrfI,IDC_RDPrfx,IDC_RDPrfI);
//			ProcPtrArray[SET_SYNC_REG]();
	SetSyncRegisters();
	//set_sync_register1();
//	SendMsg(SET_SYNC_REG);
	SendMsg(PULSER_ONOFF);
	pCTscanDlg->UpdateChnlSelected();
	}

void CPulser::OnRDPrfx() 
	{
	// TODO: Add your control notification handler code here
	if (1 /*gChannel >= 10*/)
	{
		ConfigRec.pulser.LocalPRF[gChannel/10] = 0;
	//	CheckRadioButton(IDC_RDPrfI,IDC_RDPrfx,IDC_RDPrfx);
	//			ProcPtrArray[SET_SYNC_REG]();
		SetSyncRegisters();
		//set_sync_register1();
	//	SendMsg(SET_SYNC_REG);
		SendMsg(PULSER_ONOFF);
	}

	pCTscanDlg->UpdateChnlSelected();
	}

void CPulser::OnCkRcv() 
	{
	// TODO: Add your control notification handler code here
	rchk ^= RCV_BITS;
	if (rchk != 0)
		CheckDlgButton(IDC_CkRcv,1);
	else
		CheckDlgButton(IDC_CkRcv,0);

	if (rchk !=0)
		xchk = 0;
	if (xchk != 0)
		CheckDlgButton(IDC_CkPls,1);
	else
		CheckDlgButton(IDC_CkPls,0);

	SetSysOnOffBtn();
	SetChnlOnOffBtn(gChannel);
	}

void CPulser::OnCkPls() 
	{
	// TODO: Add your control notification handler code here
	xchk ^= XMT_BITS;
	if (xchk != 0)
		CheckDlgButton(IDC_CkPls,1);
	else
		CheckDlgButton(IDC_CkPls,0);
	
	if (xchk != 0)
		rchk = 0;
	if (rchk != 0)
		CheckDlgButton(IDC_CkRcv,1);
	else
		CheckDlgButton(IDC_CkRcv,0);

	SetSysOnOffBtn();
	SetChnlOnOffBtn(gChannel);
	}

void CPulser::OnBTNSyOn() 
	{
	// TODO: Add your control notification handler code here
//	if (rchk || xchk)
	{
	// system global pulser on/off  PUSH BUTTONI
		ConfigRec.pulser.pulse_on ^= PLS_ON_BIT;
		ConfigRec.pulser.pulse_on &= ~(XMT_BITS | RCV_BITS);
			// remove old xmt/rcv setting
		ConfigRec.pulser.pulse_on |= (rchk | xchk);
		// latch in new xmt/rcv settings
		memset( &ConfigRec.pulser.pulse_chn_on,
				ConfigRec.pulser.pulse_on, MAX_CHANNEL);
		SendMsg(PULSER_ALL_ONOFF);
		OnInitDialog();
	}
	
	if (xchk)
	{
		if ( IsSysFireSeqOn() )
			SetSysPulserOnOff(FALSE);
		else
			SetSysPulserOnOff(TRUE);
	}

	if (rchk)
	{
		if ( IsSysRcvrSeqOn() )
			SetSysRcvrOnOff(FALSE);
		else
			SetSysRcvrOnOff(TRUE);
	}

	//SetSysOnOffBtn();
	UpdateDlg();
	}

void CPulser::Save_Pos()
	{
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition(_T("PULSER"), &wp);
	}

 
void CPulser::OnMove(int x, int y) 
	{
	CDialog::OnMove(x, y);
	// TODO: Add your message handler code here
//	if ( !bPulsr ) Save_Pos();
	}


BOOL CPulser::IsFireSequenceOn(int nShoe, int nChannel)
{
	BYTE nSeqLen = ConfigRec.UtRec.Shoe[nShoe].sequence_length%17;
	WORD nFireSeq = ConfigRec.UtRec.Shoe[nShoe].Ch[nChannel].Fire_Sequence;
	WORD nSeqMask = 0x0;

	for (int i=0; i<nSeqLen; i++)
	{
		nSeqMask |= ( (WORD) 0x1 << i );
	}

	if ( (nFireSeq & nSeqMask) == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CPulser::IsRcvrSequenceOn(int nShoe, int nChannel)
{
	BYTE nSeqLen = ConfigRec.UtRec.Shoe[nShoe].sequence_length%17;
	WORD nRcvrSeq = ConfigRec.UtRec.Shoe[nShoe].Ch[nChannel].Rcvr_Sequence;
	WORD nSeqMask = 0x0;

	for (int i=0; i<nSeqLen; i++)
	{
		nSeqMask |= ( (WORD) 0x1 << i );
	}

	if ( (nRcvrSeq & nSeqMask) == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


void CPulser::SetChnlOnOffBtn(int nChannel)
{
CString s;
	if ( xchk && rchk)
	{
		if ( IsFireSequenceOn(nChannel/10, nChannel%10) && IsRcvrSequenceOn(nChannel/10, nChannel%10) )
		{
			s = st_chnOn;
			SetDlgItemText(IDC_BTNCHOn,s);
		}
		else
		{
			s = st_chnOff;
			SetDlgItemText(IDC_BTNCHOn,s);
		}
	}
	else if ( xchk )
	{
		if ( IsFireSequenceOn(nChannel/10, nChannel%10) )
		{
			s = st_chnOn;
			SetDlgItemText(IDC_BTNCHOn,s);
		}
		else
		{
			s = st_chnOff;
			SetDlgItemText(IDC_BTNCHOn,s);
		}
	}
	else if ( rchk )
	{
		if ( IsRcvrSequenceOn(nChannel/10, nChannel%10) )
		{
			s = st_chnOn;
			SetDlgItemText(IDC_BTNCHOn,s);
		}
		else
		{	
			s = st_chnOff;
			SetDlgItemText(IDC_BTNCHOn,s);
		}
	}
	else
	{
		SetDlgItemText(IDC_BTNCHOn,_T("CH#/On/Off"));
	}
}


void CPulser::SetChnlPulserOnOff(int nChannel, BOOL bOn)
{
	if (bOn)
		ConfigRec.UtRec.Shoe[nChannel/10].Ch[nChannel%10].Fire_Sequence = m_pUndo->UtRec.Shoe[nChannel/10].Ch[nChannel%10].Fire_Sequence;
	else
		ConfigRec.UtRec.Shoe[nChannel/10].Ch[nChannel%10].Fire_Sequence = 0;

	m_pTcpThreadRxList->EnableFireSequence(nChannel, TRUE);
}


void CPulser::SetChnlRcvrOnOff(int nChannel, BOOL bOn)
{
	if (bOn)
		ConfigRec.UtRec.Shoe[nChannel/10].Ch[nChannel%10].Rcvr_Sequence = m_pUndo->UtRec.Shoe[nChannel/10].Ch[nChannel%10].Rcvr_Sequence;
	else
		ConfigRec.UtRec.Shoe[nChannel/10].Ch[nChannel%10].Rcvr_Sequence = 0;

	m_pTcpThreadRxList->EnableFireSequence(nChannel, TRUE);
}


BOOL CPulser::IsSysFireSeqOn()
{
	BYTE nSeqLen;
	WORD nFireSeq;
	WORD nSeqMask;
	int i;

	for (int nShoe=0; nShoe<MAX_SHOES; nShoe++)
	{
		nSeqLen = ConfigRec.UtRec.Shoe[nShoe].sequence_length%17;

		nSeqMask = 0x0;
		for (i=0; i<nSeqLen; i++)
		{
			nSeqMask |= ( (WORD) 0x1 << i );
		}

		for (int nChannel=0; nChannel<MAX_CHANNEL_PER_INSTRUMENT; nChannel++)
		{
			nFireSeq = ConfigRec.UtRec.Shoe[nShoe].Ch[nChannel].Fire_Sequence;

			if ( (nFireSeq & nSeqMask) != 0 )
			{
				return TRUE;
			}
		}
	}

	return FALSE;

}


BOOL CPulser::IsSysRcvrSeqOn()
{
	BYTE nSeqLen;
	WORD nRcvrSeq;
	WORD nSeqMask;
	int i;

	for (int nShoe=0; nShoe<MAX_SHOES; nShoe++)
	{
		nSeqLen = ConfigRec.UtRec.Shoe[nShoe].sequence_length%17;

		nSeqMask = 0x0;
		for (i=0; i<nSeqLen; i++)
		{
			nSeqMask |= ( (WORD) 0x1 << i );
		}

		for (int nChannel=0; nChannel<MAX_CHANNEL_PER_INSTRUMENT; nChannel++)
		{
			nRcvrSeq = ConfigRec.UtRec.Shoe[nShoe].Ch[nChannel].Rcvr_Sequence;

			if ( (nRcvrSeq & nSeqMask) != 0 )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


void CPulser::SetSysPulserOnOff(BOOL bOn)
{
	for (int nShoe=0; nShoe<MAX_SHOES; nShoe++)
	{
		for (int nChannel=0; nChannel<MAX_CHANNEL_PER_INSTRUMENT; nChannel++)
		{
			if (bOn)
				ConfigRec.UtRec.Shoe[nShoe].Ch[nChannel].Fire_Sequence = m_pUndo->UtRec.Shoe[nShoe].Ch[nChannel].Fire_Sequence;
			else
				ConfigRec.UtRec.Shoe[nShoe].Ch[nChannel].Fire_Sequence = 0;

			m_pTcpThreadRxList->EnableFireSequence(nShoe*10+nChannel, TRUE);
		}
	}
}


void CPulser::SetSysRcvrOnOff(BOOL bOn)
{
	for (int nShoe=0; nShoe<MAX_SHOES; nShoe++)
	{
		for (int nChannel=0; nChannel<MAX_CHANNEL_PER_INSTRUMENT; nChannel++)
		{
			if (bOn)
				ConfigRec.UtRec.Shoe[nShoe].Ch[nChannel].Rcvr_Sequence = m_pUndo->UtRec.Shoe[nShoe].Ch[nChannel].Rcvr_Sequence;
			else
				ConfigRec.UtRec.Shoe[nShoe].Ch[nChannel].Rcvr_Sequence = 0;

			m_pTcpThreadRxList->EnableFireSequence(nShoe*10+nChannel, TRUE);
		}
	}
}


void CPulser::SetSysOnOffBtn()
{
	CString sn, sf;
	sn = st_sysOn;
	sf = st_sysOff;
	if ( xchk && rchk)
	{
		if ( IsSysFireSeqOn() && IsSysRcvrSeqOn() )
		{
			SetDlgItemText(IDC_BTNSyOn,sn);
		}
		else
		{	
			SetDlgItemText(IDC_BTNSyOn,sf);
		}
	}
	else if ( xchk )
	{
		if ( IsSysFireSeqOn() )
		{
			SetDlgItemText(IDC_BTNSyOn,sn);
		}
		else
		{	
			SetDlgItemText(IDC_BTNSyOn,sf);
		}
	}
	else if ( rchk )
	{
		if ( IsSysRcvrSeqOn() )
		{
			SetDlgItemText(IDC_BTNSyOn,sn);
		}
		else
		{	
			SetDlgItemText(IDC_BTNSyOn,sf);
		}
	}
	else
	{
		SetDlgItemText(IDC_BTNSyOn,_T("SysOnOff"));
	}
}

void CPulser::OnRdExtTTL() 
{
	// TODO: Add your control notification handler code here
	if (1 /*gChannel < 10*/)
	{
		ConfigRec.pulser.LocalPRF[gChannel/10] = 3;
		SetSyncRegisters();
		SendMsg(PULSER_ONOFF);
	}

	pCTscanDlg->UpdateChnlSelected();
}


void CPulser::UpdateDlg()
{
	SetSysOnOffBtn();
	SetChnlOnOffBtn(gChannel);
}