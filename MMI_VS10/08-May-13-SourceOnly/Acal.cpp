// Acal.cpp : implementation file
//
/*
Revised:	5/25/01 Change Id gain control to Od gain control.
			My mistake.

*/

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"
#include "calicopy.h"
#include "caldata.h"
// include "Acal.h"
#include "..\include\lang.h"

#include "Extern.h"
#include <malloc.h>
#include <stdlib.h>
#include <math.h>

#include "QStringParser.h"

extern CPtrList g_pTcpListUtData;			/* linked list holding the IData */
extern CCriticalSection g_CriticalSection;  /* Critical section for mutual exclusion access of g_pTcpListUtData */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*	BOOL	m_bLong;
	BOOL	m_bCkTran;
	BOOL	m_bCkQ1;
	BOOL	m_bCkQ2;
	BOOL	m_bCkQ3;
	BOOL	m_bCkLamn;
*/	int		m_nChnlSel,btn;
#define FAKE_DATA		0
	#define	BLACK	RGB(0,0,0)
	#define	BLACK2	RGB(100,100,100)
	#define	BLACK3	RGB(180,180,180)
	#define	GREEN	RGB(0,255,0)
	#define	DBLUE	RGB(0,0,128)
	#define	BLUE	RGB(0,0,192)
	#define	RED		RGB(255,0,0)
	#define	DRED	RGB(192,0,0)
	#define	YELLOW	RGB(255,255,0)
	#define	CYAN	RGB(0,255,255)
	#define	WHITE	RGB(255,255,255)
	#define MAGENTA RGB(255, 0, 255)
	#define ORANGE RGB(255, 128, 0)
	#define BROWN RGB(128, 64, 0)
	#define TOF RGB(255,125,0)
	#define AMP RGB(255, 0, 255)

// Od is on top or first in displays in this dialog.  therefore,
// od = 0, id = 1.  
#define	ACAL_OD			0
#define	ACAL_ID			1
#define YLEN			100
BOOL	bCalpos;
BOOL bCMD ;
char *wgate[]={"ID","OD"};
	short m_bGain[102][MAX_GATE];		// byte gain values for display
	short m_bSig[102][MAX_GATE];		// sig level for 100 channels
	short m_bSigOD[102][MAX_GATE];
	static BYTE m_pBarID[102][MAX_GATE],xtype;		//,  pointer to raw data array
//	BYTE m_pBarOD[102][MAX_GATE];		//, *m_pBarOD;	// pointer to byte array
//	BYTE m_pBarII[102][MAX_GATE];		//, *m_pBarID;	// pointer to byte array
	short m_bOldBarID[102][MAX_GATE];	// Last drawn bar, whatever the selection
	BYTE *m_pTextID, *m_pTextOD;	// pointer to byte array
	BYTE m_MapBytetoPcnt[256];

/////////////////////////////////////////////////////////////////////////////
// CAcal dialog
// Steal code from inspect.cpp 02/02/00

CAcal::CAcal(CWnd* pParent /*=NULL*/)
	: CDialog(CAcal::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CAcal)
	m_bCkOd = FALSE;
	m_bCkId = TRUE;
	m_nTholdLvlSel = 1;
	m_nIdGain = 100;
	m_nOdGain = 0;
	m_nRbG1 = -1;
	m_nCalLvlEn = 0;
	m_bAllflaw2 = FALSE;
	m_rGain = _T("");
	m_bLong = FALSE;
	m_bO1 = FALSE;
	m_bO2 = FALSE;
	m_bO3 = FALSE;
	m_bLam = FALSE;
	m_bTran = FALSE;
	m_bChnlRange = FALSE;
	bCalpos = TRUE;
	m_nChnlSel = 0;
	m_nGainSigSel = 1;
	m_bAutoCalOd = TRUE;
	m_bAutoCalId = TRUE;
	//}}AFX_DATA_INIT
	m_bPeakHoldFlag = 0;	// not pk hold
    m_bBeginCal = FALSE;
//	m_nGainSigBarSel = 1;	//  bar is  the gain
//	m_nGainSigTextSel = 0;	//  text is gain

	penWhite2.CreatePen(PS_SOLID,2, RGB(255,255,255));
	penRed1.CreatePen(PS_DOT,1, RGB(128,0,0));
	penBlue1.CreatePen(PS_DOT,1, RGB(0,0,128));

	m_pUndo = new CONFIG_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
	memset((void *) m_bGain,0,sizeof( m_bGain));
	memset ((void *) m_bSig, 0, sizeof( m_bSig));
	bCMD = FALSE;
	m_pTcpThreadRxList = NULL;
	}


void CAcal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAcal)
	DDX_Control(pDX, IDC_EDIT_CHNLRANGE, m_editChnlRange);
	DDX_Control(pDX, IDC_TS_CAL9, m_dc9);
	DDX_Control(pDX, IDC_TS_CAL8, m_dc8);
	DDX_Control(pDX, IDC_TS_CAL7, m_dc7);
	DDX_Control(pDX, IDC_TS_CAL6, m_dc6);
	DDX_Control(pDX, IDC_TS_CAL5, m_dc5);
	DDX_Control(pDX, IDC_TS_CAL4, m_dc4);
	DDX_Control(pDX, IDC_TS_CAL3, m_dc3);
	DDX_Control(pDX, IDC_TS_CAL2, m_dc2);
	DDX_Control(pDX, IDC_TS_CAL10, m_dc0);
	DDX_Control(pDX, IDC_TS_CAL1, m_dc1);
	DDX_Control(pDX, IDC_SB_RGain, m_sbRGain);
	DDX_Control(pDX, IDC_ACAL_LVL_ID_SB, m_sbCalLvlId);
	DDX_Text(pDX, IDC_ACAL_ID_LVL, m_nCalLvlEn);
	DDV_MinMaxInt(pDX, m_nCalLvlEn, 0, 100);
	DDX_Text(pDX, IDC_ERGain, m_rGain);
	DDV_MaxChars(pDX, m_rGain, 8);
	DDX_Check(pDX, IDC_Slong, m_bLong);
	DDX_Check(pDX, IDC_So1, m_bO1);
	DDX_Check(pDX, IDC_So2, m_bO2);
	DDX_Check(pDX, IDC_So3, m_bO3);
	DDX_Check(pDX, IDC_Slamn, m_bLam);
	DDX_Check(pDX, IDC_Stran, m_bTran);
	DDX_Check(pDX, IDC_CHK_CHNLRANGE, m_bChnlRange);
	DDX_Check(pDX, IDC_CHECK_CALOD, m_bAutoCalOd);
	DDX_Check(pDX, IDC_CHECK_CALID, m_bAutoCalId);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAcal, CDialog)
	//{{AFX_MSG_MAP(CAcal)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(ID_ACAL_CAL_BEGIN, OnAcalCalBegin)
	ON_BN_CLICKED(ID_ACAL_PEAK_HOLD, OnAcalPeakHold)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID__FILTER, OnFilter)
	ON_COMMAND(ID__CHNL_ON, OnChnlOn)
	ON_BN_CLICKED(ID_ACAL_ERASE, OnAcalErase)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CK_LONG, OnCkLong)
	ON_BN_CLICKED(IDC_CK_TRAN, OnCkTran)
	ON_BN_CLICKED(IDC_RB_CAL, OnRbCal)
	ON_BN_CLICKED(IDC_RB_INSP, OnRbInsp)
	ON_BN_CLICKED(IDC_CK_OBLQ1, OnCkOblq1)
	ON_BN_CLICKED(IDC_CK_OBLQ2, OnCkOblq2)
	ON_BN_CLICKED(IDC_CK_OBLQ3, OnCkOblq3)
	ON_BN_CLICKED(ID_ACAL_GAIN_SIG, OnAcalGainSig)
	ON_CBN_SELCHANGE(IDC_CBgate, OnSelchangeCBgate)
	ON_BN_CLICKED(IDC_All_Flaws2, OnAllFlaws2)
	ON_BN_CLICKED(ID_ACAL_END, OnAcalCmpute)
	ON_BN_CLICKED(IDC_BtID1, OnBtID)
	ON_BN_CLICKED(IDC_BtOD1, OnBtOD)
	ON_BN_CLICKED(ID_ACAL_PEAK_HOLD2, OnAcalPeakHold2)
	ON_BN_CLICKED(ID_ACAL_GAIN_SIG2, OnAcalGainSig2)
	ON_BN_CLICKED(IDC_Slong, OnSlong)
	ON_BN_CLICKED(IDC_Slamn, OnSlamn)
	ON_BN_CLICKED(IDC_So1, OnSo1)
	ON_BN_CLICKED(IDC_So2, OnSo2)
	ON_BN_CLICKED(IDC_So3, OnSo3)
	ON_BN_CLICKED(IDC_Stran, OnStran)
	ON_BN_CLICKED(ID_ACAL_CAL_copy, OnACALCALcopy)
	ON_BN_CLICKED(ID_ACAL_END2, OnAcalSave)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_DATA, OnData)
	ON_BN_CLICKED(IDC_Print, OnPrint)
	ON_BN_CLICKED(IDC_CHK_CHNLRANGE, OnChkChnlrange)
	ON_BN_CLICKED(ID_ACAL_CMPUTE, OnAcalCmpute)
	ON_BN_CLICKED(ID_ACAL_SAVE, OnAcalSave)
	ON_BN_CLICKED(IDC_CHECK_CALID, OnCheckCalOnId)
	ON_BN_CLICKED(IDC_CHECK_CALOD, OnCheckCalOnOd)
	//}}AFX_MSG_MAP
        ON_BN_CLICKED(IDC_DATA, OnData)
       ON_BN_CLICKED(IDC_Print, OnPrint)

	// manually added by jeh 02-02-00
//	ON_MESSAGE(WM_USER_ACAL_ERASE, VS10_EraseBars)  // this required to go from Visual Studio 6 to Visual Studio 2010
//  add to header file
//	afx_msg LRESULT VS10_EraseBars(WPARAM, LPARAM);	// this foolishness required to go from Visual Studio to Visual NET
// IN this cpp file call EraseBars from inside VS10_EraseBars


//	ON_MESSAGE(WM_USER_ACAL_ERASE, EraseBars)
//	ON_MESSAGE(WM_USER_UPDATE_ACAL_SCROLLBARS, UpdateScrollInfo)
	ON_MESSAGE(WM_USER_ACAL_ERASE, VS10_EraseBars)
	ON_MESSAGE(WM_USER_UPDATE_ACAL_SCROLLBARS, VS10_UpdateScrollInfo)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_ACAL_CH1, IDC_ACAL_CH100, DoS1RadioButton)
	
END_MESSAGE_MAP()

// Wrappers to get from VC6 to Visual Studio 10
// this foolishness required to go from Visual Studio to Visual NET
afx_msg LRESULT CAcal::VS10_EraseBars(WPARAM, LPARAM)
	{	EraseBars();	return 0;	}

afx_msg LRESULT CAcal::VS10_UpdateScrollInfo(WPARAM, LPARAM)
	{	UpdateScrollInfo();	return 0;	}

/////////////////////////////////////////////////////////////////////////////
// CAcal message handlers


void CAcal::OnCancel() 
	{
	// TODO: Add extra cleanup here
	// Need this to get to PostNcDestroy which nulls ptr to the routine
	m_pTcpThreadRxList->SendSlaveMsg(NOP_MODE,0,0,0,0,0,0);
	StopTimer();
	m_nTholdLvlSel = 1;	// restore inspection level tholds
	//SetCalTholds();
	Save_Pos();	
	
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CAcal::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}

BOOL CAcal::OnInitDialog() 
	{
	CDialog::OnInitDialog();

	CString s;
	int i;

	i = gChannel + IDC_ACAL_CH1;
	CheckRadioButton( IDC_ACAL_CH1, IDC_ACAL_CH100, i );

	if (bCalpos)
	{
		WINDOWPLACEMENT wp;
		RECT rect;
		int dx, dy;		// width and height of original window

		GetWindowPlacement(&wp);
		dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		pCTscanDlg->GetWindowLastPosition(_T("CALIB"), &rect);
		if ( ( (rect.right - rect.left) >= dx ) &&
			 ( (rect.bottom - rect.top) >= dy ))
			{
			wp.rcNormalPosition = rect;
			SetWindowPlacement(&wp);
			}
		bCalpos = FALSE;
	}

	// Kill thold and gain windows if they are being displayed
	if (CTholds::m_pDlg) CTholds::m_pDlg->KillMe();
	if (CGainDlg::m_pDlg) CGainDlg::m_pDlg->KillMe();

//	CheckHeap(1);
	// TODO: Add extra initialization here

//	m_nAmaSonoSel = ConfigRec.AmaSonoSel;
	for ( i = 0; i < 256; i++) 
		m_MapBytetoPcnt[i] = (BYTE)( (float)(i) * 100.0/255.0 + 0.5);

	memset ( (void *) m_bIdMask, 0xff, sizeof(m_bIdMask));

	// Start timer
	// which will process ipx data msg's collected from OnIpxRXRDY
	// procedure in TscanDlg

	gChannel %= MAX_CHANNEL;
/*		m_CBgate.ResetContent( );
		for ( i=0; i< 2;i++){
			m_CBgate.AddString(wgate[i]);
		}
*/	gGate %= MAX_GATE;
#if 0
	switch (gGate)
	{
	case 0:
	default:
		btn = IDC_BtID1;
		break;
	case 2:
		btn = IDC_BtOD1;
		break;
/*	case 2:
		btn = IDC_gate3;
		break;
	case 3:
		btn = IDC_gate4;
		break;
*/
	}
#endif

	if (m_bCkId)
		btn = IDC_BtID1;
	else
		btn = IDC_BtOD1;
	CheckRadioButton( IDC_BtID1, IDC_BtOD1, btn );
//		m_CBgate.SetCurSel(gGate);
	xtype=GetChnlTypeIndex(gChannel);
	m_uIpxTimer = 0;
//	s = _T("PkOff");
//	GetDlgItem(ID_ACAL_PEAK_HOLD)->SetWindowText(s);
	StartTimer();
//	for ( i=0;i<MAX_GATE;i++) m_bOldBarID[i] = m_pBarID[i] =  &m_bSig[0][i];
//	memcpy ((void *) m_pBarID, (void *) m_bSig, sizeof( m_bSig));

	memcpy ((void *) m_bOldBarID, (void *) m_bSig, sizeof( m_bSig));
//	m_pBarOD = m_pTextOD = &m_bSigOD[0][0];
//	m_pBarID = &m_bSig[0];
//	m_pBarOD = &m_bSigOD[0];
//	memset( (void *) m_bOldBarID, 0, sizeof(m_bOldBarID));
//	memset( (void *) m_bOldBarOD, 0, sizeof(m_bOldBarOD));

	LoadConfigRec();

	ShowChnlInfo();
	//memcpy(&m_nCalLvlId,&ConfigRec.UtRec.TholdLong, MAX_GATE*XDCR_TYPE);
	for (i=0; i<6; i++)
	{
		m_nCalLvlId[i+2][0] = ConfigRec.UtRec.bCalLvlL[i][0];
		m_nCalLvlId[i+2][1] = ConfigRec.UtRec.bCalLvlL[i][1];
	}

	// Show signal or gain display mode
	if (m_nGainSigSel)
		{	// show sig levels
//		s = _T("Signal");
                memcpy ((void *) m_pBarII, (void *) m_bSig, sizeof( m_bSig));

/*		m_pBarID =  &m_bSig[0][gGate];
		m_pBarOD = &m_bSigOD[0][gGate];
*/		}
	else
		{	// show gains
//		s = _T("Gain");
		m_sbRGain.SetScrollRange(1,100,TRUE);
		m_sbRGain.EnableScrollBar(ESB_ENABLE_BOTH);
//m_szRGain
//		for ( i=0;i<MAX_GATE;i++) m_pBarID[i] =  &m_bGain[i];
		
		memcpy ((void *) m_pBarII, (void *) m_bGain, sizeof( m_bGain));
//		m_pBarID[0] =  &m_bGain[0][0];
		m_sbRGain.SetScrollPos(m_bGain[gChannel][0]);
		}
//	GetDlgItem(ID_ACAL_GAIN_SIG)->SetWindowText(s);

	// STD LEVEL works in per cent
#if 0
	m_sbCalLvlOd.SetScrollRange(1,100,TRUE);
	m_sbCalLvlOd.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbCalLvlOd.SetScrollPos(m_nCalLvlOd);

	m_sbOdGain.SetScrollRange(30,300,TRUE);
	m_sbOdGain.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbOdGain.SetScrollPos(m_nOdGain);
#endif

//	SetCalLvlPtrs();	// also sets scroll pos of id/od lvls

	SendMsg(CAL_MODE);

	// Make sure in Calibration mode and not inspect mode
	// Give some time to display before erasing screen.. Buy time with
	// windows post message operation.

//	m_nTholdLvlSel = 1;	// use thresholds for cal mode
	CheckRadioButton(IDC_RB_CAL,IDC_RB_INSP,IDC_RB_CAL+m_nTholdLvlSel);
	m_bAllflaw2 = ConfigRec.cal.AllFlawsFlag;
	m_sbCalLvlId.SetScrollRange(1,100,TRUE);
	m_sbCalLvlId.EnableScrollBar(ESB_ENABLE_BOTH);
	if (m_bCkId)
	{
		m_sbCalLvlId.SetScrollPos(m_nCalLvlId[xtype][0]);
		m_nCalLvlEn=m_nCalLvlId[xtype][0];
	}
	else
	{
		m_sbCalLvlId.SetScrollPos(m_nCalLvlId[xtype][1]);
		m_nCalLvlEn=m_nCalLvlId[xtype][1];
	}
	
	m_sbRGain.SetScrollRange(-200,600,TRUE);
	m_sbRGain.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbRGain.SetScrollPos(ConfigRec.receiver.gain[m_nChnlSel]);
//	i = -1;
//	EraseBars();

	if (m_nGainSigSel)
		CheckRadioButton(ID_ACAL_GAIN_SIG,ID_ACAL_GAIN_SIG2,ID_ACAL_GAIN_SIG);
		else 
		CheckRadioButton(ID_ACAL_GAIN_SIG,ID_ACAL_GAIN_SIG2,ID_ACAL_GAIN_SIG2);
	CheckRadioButton(ID_ACAL_PEAK_HOLD,ID_ACAL_PEAK_HOLD2,1-m_bPeakHoldFlag+ID_ACAL_PEAK_HOLD);
	UpdateData(FALSE);	// Copy variables to screen
//	ShowDlgTitle();
//	CheckHeap(6);
//       m_bBeginCal = TRUE;  moved later per cl code 071403
//      m_bPeakHoldFlag = 1;
//      CheckRadioButton(ID_ACAL_GAIN_SIG,ID_ACAL_GAIN_SIG2,ID_ACAL_GAIN_SIG);
//      CheckRadioButton(ID_ACAL_PEAK_HOLD,ID_ACAL_PEAK_HOLD2,ID_ACAL_PEAK_HOLD);
        //memset( (void *) &m_bSig, 0, sizeof(m_bSig) );
  	
	s = ConfigRec.JobRec.Oblq1Name;
	SetDlgItemText(IDC_So1,s );
	s = ConfigRec.JobRec.Oblq2Name;
	SetDlgItemText(IDC_So2, s);
	s = ConfigRec.JobRec.Oblq3Name;
	SetDlgItemText(IDC_So3, s);

	SetWindowText(SYSTEM_NAME);

	SetDefID(-1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CAcal::OnTimer(UINT nIDEvent) 
	{
	// TODO: Add your message handler code here and/or call default

	I_MSG_CAL	*pImsg02;
	int i;

	// copy calibration data from g_pListUtData to plistUtdata
	I_MSG_RUN *pImsgRun;
	g_CriticalSection.Lock();
	while (!g_pTcpListUtData.IsEmpty() )
	{
		pImsgRun = (I_MSG_RUN *) g_pTcpListUtData.RemoveHead();
		plistUtData.AddTail((void *) pImsgRun);
	}
	g_CriticalSection.Unlock();
						

	m_uTimerTick++;
	// Every 8th timer tick, capture the system time
	if ( (m_uTimerTick & 0x7 ) == 0)
	// Every 2th timer tick, capture the system time
	//if ( (m_uTimerTick & 0x1 ) == 0)
	{
		time(&m_tTimeNow);
//		EraseBars();
		pCTscanDlg->UpdateTimeDate(&m_tTimeNow);
		pCTscanDlg->UpdateRpm();	// also displays joint length
		if ( m_bBeginCal) ShowDlgTitle();
		if (m_bPeakHoldFlag==0)
		{
		memset( (void *) &m_bSigOD, 0, sizeof(m_bSigOD) );
//		memset( (void *) &m_bSig, 0, sizeof(m_bSig) );
		}
	}

#if !FAKE_DATA
	if (plistUtData.IsEmpty() && m_nGainSigSel )
		{	// nothing to do here
		CDialog::OnTimer(nIDEvent);
		i = (int) m_uTimerTick;	// kill compiler warning
		return;
		}

#else
	pImsg02 = new I_MSG_CAL;
	memset ( (void *) pImsg02, 0, sizeof(I_MSG_CAL));
	i = (m_uTimerTick >> 3) & 0x1f;	// chnl indx

/*
	pImsg02->UtSample.Sh[i/10].Ch[i%10].g[0] = i+20;
	pImsg02->UtSample.Sh[i/10].Ch[i%10].g[1] = i+25;
*/
//	if ( pImsg02->MsgID != 2) return;
//	int is, ic, ib;

//	pImsg02->Shoe[i/10].Amp[i%10][0]= i+20;
//	pImsg02->Shoe[i/10].Amp[i%10][1]= i+25;
	plistUtData.AddTail( (void *)pImsg02);

#endif
//	EraseBars();
//		DrawBars();

//	ShowText();

	// Now that all samples in list have been drawn on all 
	// display areas, empty list.

	DrawBars();

	while (!plistUtData.IsEmpty() )
		{
		//DrawBars();
		pImsg02 = (I_MSG_CAL *)plistUtData.RemoveHead();
		delete pImsg02;
		}
	
	CDialog::OnTimer(nIDEvent);
	}

void CAcal::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{

	// TODO: Add your message handler code here and/or call default
	int nStart, nDelta;		// where the bar started and amount of change
	int nResult;			// normally start + delta
	int nMax, nMin;
	int nCtlId;

	nStart = pScrollBar->GetScrollPos();
	pScrollBar->GetScrollRange(&nMin, &nMax);

	switch ( nSBCode)
		{	// switch on type of motion
	case SB_LINELEFT:
		nDelta = -1;
		break;

	case SB_LINERIGHT:
		nDelta = 1;
		break;

	case SB_PAGELEFT:
		nDelta = -10;
		break;

	case SB_PAGERIGHT:
		nDelta = 10;
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

//			m_nOdLvlLast[i] = m_nCalLvlOd[i];


	switch(nCtlId)
		{
		// Abs controls
	case IDC_ACAL_LVL_OD_SB:
		//if ( !m_bCkOd) goto DefOp;	// no action unless ck box set
//		m_nCalLvlOd = nResult;
		break;
	
	case IDC_ACAL_LVL_ID_SB:
		m_nCalLvlEn = nResult;
/*		if ( gGate == 0) m_nCalLvlOd = nResult;
	// no action unless ck box set
		//m_nCalLvlEn = m_nCalLvlOd = 
		else if ( gGate == 1) m_nCalLvlId = nResult;
*/
//		switch (gGate){
//		case 0:
		if (m_bCkId)
		{
			m_nCalLvlId[xtype][0] = nResult;
			if ( (xtype >= 2) && (xtype <= 7) )
				ConfigRec.UtRec.bCalLvlL[xtype-2][0] = nResult;
		}
		if (m_bCkOd)
		{
			m_nCalLvlId[xtype][1] = nResult;
			if ( (xtype >= 2) && (xtype <= 7) )
				ConfigRec.UtRec.bCalLvlL[xtype-2][1] = nResult;
		}
		ShowChnlInfo();

//		DrawBars();
//			break;
/*		case 1:
			m_nCalLvlId[i] = nResult;
			break;

		}
*/		break;
	
	case IDC_SB_RGain:
        m_bGain[m_nChnlSel][0]=ConfigRec.receiver.gain[m_nChnlSel] = nResult;
//		UpdateConfigRec();
//		ShowChnlInfo();
		SendMsg(RECEIVER_GAIN);
		//TurnOffGroupTcg();
		pCTscanDlg->UpdateChnlSelected();
		break;

#if 0	
	case IDC_SB_RGain:
		m_nGain[m_nChnlSel] = m_nOdGain = nResult;
//		UpdateConfigRec();
		SendMsg( SET_ALL_GAINS);
		break;
#endif
	
	default:
		break;
		}

	pScrollBar->SetScrollPos(nResult, TRUE);
		DrawBars();

	UpdateData(FALSE);	// copy data to screen
	UpdateConfigRec();	// copy data to config rec structure
//	DrawBars();
//	DrawTholds();
#if 0
	UpdateScrollInfo();

#endif
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

void CAcal::OnAcalCalBegin() 
	{
	// TODO: Add your control notification handler code here
	CString s;
	m_bBeginCal = TRUE;
	m_bPeakHoldFlag = 1;
	CheckRadioButton(ID_ACAL_GAIN_SIG,ID_ACAL_GAIN_SIG2,ID_ACAL_GAIN_SIG);
	CheckRadioButton(ID_ACAL_PEAK_HOLD,ID_ACAL_PEAK_HOLD2,ID_ACAL_PEAK_HOLD);
	memset( (void *) &m_bSig, 0, sizeof(m_bSig) );
	OnAcalGainSig();	
	}


void CAcal::AutoAdjustTcgGain(char type, int nChannel)
{
	float tmpID, tmpOD, tlogID, tlogOD;
	float Gate3End;
	float OldIdGain = ConfigRec.receiver.TcgRec[nChannel].IdGain;
	float OldOdGain = ConfigRec.receiver.TcgRec[nChannel].OdGain;
	BOOL  bWriteTrigStep = FALSE;

	Gate3End = m_pTcpThreadRxList->GetGateDelay(nChannel, 2) + m_pTcpThreadRxList->GetGateRange(nChannel, 2);
	if (Gate3End < 150.0f)
		Gate3End = 150.0f;

	/* Adjust ID gain */
	if ( m_bAutoCalId && m_pBarII[nChannel][0] > 10 )
	{
		/* If ID is between 10 and 100 */
		if ( m_pBarII[nChannel][0] > 10 && m_pBarII[nChannel][0] < 100 )
		{
			tmpID = (float) ( (m_nCalLvlId[type][0]) / (double) m_pBarII[nChannel][0] );
			tlogID = (float) ( 20.0f*log10(tmpID) );
			tmpID = OldIdGain + tlogID;
		}
		/* If ID saturate, lower gain 6 dB */
		else if ( m_pBarII[nChannel][0] >= 100 )
		{
			tmpID = OldIdGain - 6.0f;
		}

		tmpID = tmpID * m_pTcpThreadRxList->m_TruscopGainScaling + m_pTcpThreadRxList->m_TruscopGainOffset;

		if (tmpID < GAINMIN)
			tmpID = GAINMIN;
		if (tmpID > GAINMAX)
			tmpID = GAINMAX;
		ConfigRec.receiver.gain[nChannel] = (short) tmpID;
		m_pTcpThreadRxList->ReSetTcgGainTable(nChannel, FALSE);
	}

	// turn on TCG and/or change step
	if ( m_pTcpThreadRxList->GetTcgTrigSel(nChannel) != 1)
	{
		ConfigRec.receiver.tcg_trigger[nChannel] = 1;
		bWriteTrigStep = TRUE;
	}
	if ( Gate3End < 102.0f )
	{
		if ( ConfigRec.receiver.tcg_step[nChannel] != 1 )
		{
			ConfigRec.receiver.tcg_step[nChannel] = 1;
			bWriteTrigStep = TRUE;
		}
	}
	else if ( Gate3End < 153.0f )
	{
		if ( ConfigRec.receiver.tcg_step[nChannel] != 2 )
		{
			ConfigRec.receiver.tcg_step[nChannel] = 2;
			bWriteTrigStep = TRUE;
		}
	}
	else
	{
		if ( ConfigRec.receiver.tcg_step[nChannel] != 3 )
		{
			ConfigRec.receiver.tcg_step[nChannel] = 3;
			bWriteTrigStep = TRUE;
		}
	}

	if ( bWriteTrigStep )
		m_pTcpThreadRxList->WriteTcgStepTrigSel(nChannel);


	/* Adjust OD gain */
	/* If OD is between 10 and 100 */
	if (m_bAutoCalOd && m_pBarII[nChannel][1] > 10  && m_pBarII[nChannel][1] < 100)
	{
		tmpOD = (float) ( (m_nCalLvlId[type][1]) / (double) m_pBarII[nChannel][1] );
		tlogOD = (float) ( 20.0f*log10(tmpOD) );
		tmpOD = OldOdGain + tlogOD;
		m_pTcpThreadRxList->SetTcgGainTable(0, nChannel, 2, tmpOD, 2);
	}
	/* If OD saturate, lower gain 6 dB */
	else if (m_bAutoCalOd && m_pBarII[nChannel][1] >= 100)  
	{
		m_pTcpThreadRxList->SetTcgGainTable(0, nChannel, 2, OldOdGain-6.0f, 2);
	}
	else if ( m_bAutoCalId && m_pBarII[nChannel][0] > 10 )
	{
		m_pTcpThreadRxList->SetTcgGainTable(0, nChannel, 2, OldOdGain, 2);
	}
}


void CAcal::AutoAdjustLamGain(char type, int nChannel)
{
	float tmpID, tlogID;
	float OldIdGain = ConfigRec.receiver.TcgRec[nChannel].IdGain;

	/* Adjust ID gain (Gate 2) */
	if ( m_bAutoCalId && m_pBarII[nChannel][0] > 10 )
	{
		/* If ID is between 10 and 100 */
		if ( m_pBarII[nChannel][0] > 10 && m_pBarII[nChannel][0] < 100 )
		{
			tmpID = (float) ( (m_nCalLvlId[type][0]) / (double)m_pBarII[nChannel][0] );
			tlogID = (float) ( 20.0f*log10(tmpID) );
			tmpID = OldIdGain + tlogID;
		}
		/* If ID saturate, lower gain 6 dB */
		else if ( m_pBarII[nChannel][0] >= 100 )
		{
			tmpID = OldIdGain - 6.0f;
		}

		if (tmpID < 0.0f)
			tmpID = 0.0f;
		if (tmpID > 79.9f)
			tmpID = 79.9f;
		if ( m_pTcpThreadRxList->GetTcgTrigSel(nChannel) != 0)
			m_pTcpThreadRxList->SetTcgGainTable(0, nChannel, 1, tmpID, 2);
	}
}


void CAcal::OnAcalCmpute() 
{
	// TODO: Add your control notification handler code here
	if (!m_bLong && !m_bTran && !m_bO1 && !m_bO2 && !m_bO3 && !m_bChnlRange && !m_bLam)
	{
			MessageBox(_T("No action was taken because\nno channel range was specified."), _T("Auto Calibration"), MB_OK);
			return;
	}

	if (!m_bAutoCalId && !m_bAutoCalOd)
	{
			MessageBox(_T("No action was taken because\nboth ID and OD were unchecked."), _T("Auto Calibration"), MB_OK);
			return;
	}

	BOOL bValidRange = TRUE;
    m_bBeginCal = TRUE;  //added from cl code 071403
	if (m_bChnlRange)
		bValidRange = GetChannelRange();

	int i;
	CString s;
	SetCalTholds();	
	//m_bPeakHoldFlag = 1;
	for ( i=0; i<MEM_MAX_CHANNEL;i++)
	{
		if ( (m_bAutoCalId && (m_pBarII[i][0] > 10)) || (m_bAutoCalOd && (m_pBarII[i][1] > 10)) ) 
		{
			char type = GetChnlTypeIndex(i);
			switch(type)
			{
			case 2:	//IS_LONG:
				if (m_bLong || (m_bChnlRange && m_bChnlSelect[i]) )
				{
					AutoAdjustTcgGain(type, i);
				}
				break;
			case 3: //IS_TRAN:
				if (m_bTran || (m_bChnlRange && m_bChnlSelect[i]) ) 
				{
					AutoAdjustTcgGain(type, i);
				}
				break;
			case 4:	//IS_OBQ1:
				if 	(m_bO1 || (m_bChnlRange && m_bChnlSelect[i]) )
				{
					AutoAdjustTcgGain(type, i);
				}
				break;
			case 5: //IS_OBQ2:
				if (m_bO2 || (m_bChnlRange && m_bChnlSelect[i]) )
				{
					AutoAdjustTcgGain(type, i);
				}
				break;
			case 6: //IS_OBQ3:
				if (m_bO3 || (m_bChnlRange && m_bChnlSelect[i]) )
				{
					AutoAdjustTcgGain(type, i);
				}
				break;
			case 7: //IS_LAM:
				if (m_bLam || (m_bChnlRange && m_bChnlSelect[i]) )
				{
					AutoAdjustLamGain(type, i);
				}
				break;
			default:
				if (m_bChnlRange && m_bChnlSelect[i])
				{
					AutoAdjustTcgGain(type, i);
				}
				break;
			}
		}
	}
//	CheckRadioButton(ID_ACAL_GAIN_SIG,ID_ACAL_GAIN_SIG2,ID_ACAL_GAIN_SIG2);
	//OnAcalGainSig();
	m_bPeakHoldFlag = 0;
	CheckRadioButton(ID_ACAL_PEAK_HOLD,ID_ACAL_PEAK_HOLD2,ID_ACAL_PEAK_HOLD2);
/*	for (i=0 ; i<MEM_MAX_CHANNEL ; i++)
		m_sbRGain.SetScrollPos(m_bGain[i][0]);
*/

	OnInitDialog();

	if (pCTscanDlg->m_pTcgSetupDlg)
		(pCTscanDlg->m_pTcgSetupDlg)->UpdateDlg();
	if (pCTscanDlg->m_pAscanDlg)
		(pCTscanDlg->m_pAscanDlg)->UpdateDlg();

    m_bBeginCal = FALSE;
	if (bValidRange)
		AfxMessageBox(_T("Auto Calibration completed normally."));
	else
		AfxMessageBox(_T("Auto Calibration completed.\nHowever, the channel range you entered is not valid and was ignored."));
}


void CAcal::OnAcalPeakHold() 
{
	// TODO: Add your control notification handler code here
	CString s;


	if (m_bChnlRange)
		if (!GetChannelRange() )
		{
			AfxMessageBox(_T("The channel range you entered is not valid.\nPlease enter channel numbers and/or ranges separated by commas, e.g., 1,3,5-12.\nDO NOT leave any space between characters."));
			m_bPeakHoldFlag = 0;
			CheckRadioButton(ID_ACAL_PEAK_HOLD,ID_ACAL_PEAK_HOLD2,ID_ACAL_PEAK_HOLD2);
			return;
		}

	m_bPeakHoldFlag=1;	
}


/*
void CAcal::OnGainSigSel() 
	{	// Graphical display of info
	// TODO: Add your control notification handler code here

	CString s;

	m_nGainSigBarSel ^= 1;	// toggle
	m_nGainSigBarSel &= 1;
	if (m_nGainSigBarSel == 0)
		{
		s = _T("Gain");
		for (int i=0;i<MAX_CHANNEL;i++) m_pBarID[i] =  &m_bGain[i];
		}
	else
		{
		s = _T("Signal");
		for (int i=0;i<MAX_CHANNEL;i++) m_pBarID[i] =  &m_bSig[i][gGate];
//		m_pBarOD = &m_bSigOD[0][0];
		}
	GetDlgItem(ID_ACAL_GAIN_SIG)->SetWindowText(s);
//	GetDlgItem(IDC_GAIN_SIG_SEL2)->SetWindowText(s);


	}
*/
void CAcal::KillMe()					// another way to die
	{
	// Public access to OnCancel
	OnCancel();
	}


void CAcal::StopTimer(void)
	{
	if (m_uIpxTimer)
		{
		KillTimer(m_uIpxTimer);
		m_uIpxTimer = 0;
		}
	}

void CAcal::StartTimer(void)
	{
	if (m_uIpxTimer)	return;	// already running

	m_uIpxTimer = SetTimer(IDT_IPX_TIMER, 500, NULL);
//	m_uIpxTimer = SetTimer(IDT_IPX_TIMER, 150, NULL);
	if (!m_uIpxTimer) MessageBox(_T("Failed to start IPX timer"));
	}



void CAcal::EraseBars(void)
	{
return;
	CRect CalRect[MAX_SHOES];

	CClientDC dc1(&m_long);	// get drawing context shoe 1
	CClientDC dc2(&m_tran);	// get drawing context shoe 1
	CClientDC dc3(&m_obq1);	// get drawing context shoe 1
	CClientDC dc4(&m_obq2);	// get drawing context shoe 1
	CClientDC dc5(&m_obq3);	// get drawing context shoe 1
	CClientDC dc6(&m_lamn);	// get drawing context shoe 1
	CClientDC *pdc[MAX_SHOES];
	CString s,t;

	pdc[0] = &dc1;
	pdc[1] = &dc2;
	pdc[2] = &dc3;
	pdc[3] = &dc4;
	pdc[4] = &dc5;
	pdc[5] = &dc6;

	// Get the client rectangle boundaries for the Image Map graph (Button)
	m_long.GetClientRect(&CalRect[0]);
	m_tran.GetClientRect(&CalRect[1]);
	m_obq1.GetClientRect(&CalRect[2]);
	m_obq2.GetClientRect(&CalRect[3]);
	m_obq3.GetClientRect(&CalRect[4]);
	m_lamn.GetClientRect(&CalRect[5]);
	int		i;

	for ( i = 0; i < MAX_SHOES; i++)
		{
		CalRect[i].NormalizeRect();
		// Move the coordinate origin to the upper left corner
		pdc[i]->SetViewportOrg( 0, 0);	// CalRect[i].Height());
		// Set mapping mode to AnIsotropic, deal in logical units rather than pixels
		pdc[i]->SetMapMode(MM_ANISOTROPIC);
		// 10 chnls per shoe, but two lines (ID/OD) per channel = 20 slots
		// MAKE EACH CHNL 3 SLOTS WIDE , blue, red, space and offset

		pdc[i]->SetWindowExt(100,YLEN);	// define size of window in logical units
		// max amp (x) is 100, 24 lines stacked vertically
		// tell windows how big this box is physically in pixels
		pdc[i]->SetViewportExt( CalRect[i].Width(), CalRect[i].Height());
			pdc[i]->FillSolidRect(0,0,100,YLEN, RGB(255,255,255));
		}

	// Erase both shoe displays
/*	for ( i = 0; i < MAX_SHOES; i++)
			{
			pdc[i]->FillSolidRect(0,0,100,YLEN, RGB(255,255,255));
			}
*/
//	memset( (void *) m_bOldBarID, 0, sizeof(m_bOldBarID));
//	memset( (void *) m_bOldBarOD, 0, sizeof(m_bOldBarOD));
//	if (i == -1)
	{
	for ( i = 0; i < MAX_SHOES; i++)
			{
			pdc[i]->FillSolidRect(0,0,100,YLEN, RGB(255,255,255));
			}
	}
/*	else 
		if (nXdtype < MAX_SHOES)
			pdc[nXdtype]->FillSolidRect(0,0,100,YLEN, RGB(255,255,255));
*/
//	DrawTholds();
//	UpdateScrollInfo();
	}


void CAcal::DrawBars(void)
{

	CRect CalRect[MAX_SHOES];

	CClientDC dc1(&m_dc1);	// get drawing context shoe 1
	CClientDC dc2(&m_dc2);	// get drawing context shoe 1
	CClientDC dc3(&m_dc3);	// get drawing context shoe 1
	CClientDC dc4(&m_dc4);	// get drawing context shoe 1
	CClientDC dc5(&m_dc5);	// get drawing context shoe 1
	CClientDC dc6(&m_dc6);	// get drawing context shoe 1
	CClientDC dc7(&m_dc7);	// get drawing context shoe 1
	CClientDC dc8(&m_dc8);	// get drawing context shoe 1
	CClientDC dc9(&m_dc9);	// get drawing context shoe 1
	CClientDC dc0(&m_dc0);	// get drawing context shoe 1
	CClientDC *pdc[MAX_SHOES];

	
	CString s,t;
	I_MSG_CAL *pImsg02;

	pdc[0] = &dc1;
	pdc[1] = &dc2;
	pdc[2] = &dc3;
	pdc[3] = &dc4;
	pdc[4] = &dc5;
	pdc[5] = &dc6;
	pdc[6] = &dc7;
	pdc[7] = &dc8;
	pdc[8] = &dc9;
	pdc[9] = &dc0;

	// Get the client rectangle boundaries for the Image Map graph (Button)
	m_dc1.GetClientRect(&CalRect[0]);
	m_dc2.GetClientRect(&CalRect[1]);
	m_dc3.GetClientRect(&CalRect[2]);
	m_dc4.GetClientRect(&CalRect[3]);
	m_dc5.GetClientRect(&CalRect[4]);
	m_dc6.GetClientRect(&CalRect[5]);
	m_dc7.GetClientRect(&CalRect[6]);
	m_dc8.GetClientRect(&CalRect[7]);
	m_dc9.GetClientRect(&CalRect[8]);
	m_dc0.GetClientRect(&CalRect[9]);
	int	ylen=20*(MAX_GATE+3)+10;
	if (!plistUtData.IsEmpty() )
	{
		POSITION rPosition = plistUtData.GetHeadPosition();
		pImsg02 = (I_MSG_CAL *) plistUtData.GetHead();

	// Moved from FillBarArray	
	if (m_bPeakHoldFlag==0)
		{
		//memset( (void *) &m_bSigOD, 0, sizeof(m_bSigOD) );
		memset( (void *) &m_bSig, 0, sizeof(m_bSig) );
		}


		while (1)
		{
			pImsg02 = (I_MSG_CAL *) plistUtData.GetNext(rPosition);
	#if TRACK_ID
			FillBarArray(pImsg02);	//Put max of real data into bar array
			// MOVE notch  tracking after fill bar 04/26/00
			TrackIDNotch(pImsg02);	// only has effect for amalog id cal
	#else
			TrackIDNotch(pImsg02);	// only has effect for amalog id cal
			FillBarArray(pImsg02);	//Put max of real data into bar array

	#endif
			if (rPosition == NULL) break;	// exit while 1
		}
	}		

	FillGainArray();	// get gains from Config record
	//j=GetChnlTypeIndex(gChannel);	
//	i = gChannel/10;

	for ( int i = 0; i < MAX_SHOES; i++)
		{
		CalRect[i].NormalizeRect();
		// Move the coordinate origin to the upper left corner
		pdc[i]->SetViewportOrg( 0, 0);	// CalRect[i].Height());
		// Set mapping mode to AnIsotropic, deal in logical units rather than pixels
		pdc[i]->SetMapMode(MM_ANISOTROPIC);
		// 10 chnls per shoe, but two lines (ID/OD) per channel = 20 slots
		// MAKE EACH CHNL 3 SLOTS WIDE , blue, red, space and offset

		pdc[i]->SetWindowExt(100,ylen);	// define size of window in logical units
		// max amp (x) is 100, 50 lines stacked vertically
		// tell windows how big this box is physically in pixels
		pdc[i]->SetViewportExt( CalRect[i].Width(), CalRect[i].Height());
		pdc[i]->FillSolidRect(0,0,100,ylen, RGB(255,255,255));	//thold
		pdc[i]->SelectObject(&penBlue1);
			pdc[i]->MoveTo(m_nCalLvlId[xtype][1], 1);
			pdc[i]->LineTo(m_nCalLvlId[xtype][1], ylen);
		pdc[i]->SelectObject(&penRed1);
			pdc[i]->MoveTo(m_nCalLvlId[xtype][0], 1);
			pdc[i]->LineTo(m_nCalLvlId[xtype][0], ylen);
			DrawIDBar(i, pdc[i]);
//			DrawODBar(i, pdc[i]);
//			m_nIdLvlLast[i] = m_nCalLvlId[i];

		}
	// Display info for selected chnl
	ShowChnlInfo();

}

void CAcal::DoS1RadioButton(UINT nID) 
	{
	// For TruScan, all radio buttons work single control
	int i;
	if (bCMD) i = gChannel ;
			else i = nID - IDC_ACAL_CH1;
	ASSERT( i >= 0 && i < MEM_MAX_CHANNEL);
	ConfigRec.UtRec.CurrentChnl = m_nChnlSel = i;	// new chnl selected
	//CheckRadioButton( IDC_ACAL_CH1, IDC_ACAL_CH100, nID );

	ConfigRec.UtRec.CurrentChnl = m_nChnlSel = GetCheckedRadioButton(IDC_ACAL_CH1, IDC_ACAL_CH100) - IDC_ACAL_CH1;

	UpdateData(FALSE);	// Copy variables to screen

#if 0
	m_nS1ChGain = GetChnlGain(m_nS1Band, 0, m_nChnlSel);// new chnl gain
	UpdateScrollInfo();	// show on scroll bars and edit boxes
	s.Format(_T("CH=%02d"), i+1);
	GetDlgItem(IDC_STATIC_CH)->SetWindowText(s);
#endif
	m_sbRGain.SetScrollPos(ConfigRec.receiver.gain[m_nChnlSel]);
	ShowChnlInfo();
	gChannel = m_nChnlSel;
	pCTscanDlg->ChangeActiveChannel();
//	CheckRadioButton( IDC_ACAL_CH1, IDC_ACAL_CH100, i );
	DrawBars();
	}

/***************************************************************/
/*
GetxxxGain commands vary only by frequency.  High pass (OD)
	is a single stage gain.  Low Pass (ID) is cascaded and is the
	product of abs gain and differential gain.

SetxxxGain commands vary only by abs/proportional.  Differential
	and absolute gain operations have different commands.  The 
	user can not set the differential gain directly except with
	pre setgain.  The cal operation takes care of differential
	gain adjustment.

*/
/***************************************************************/

BOOL CAcal::SetChnlGain(int band, int shoe, int chnl, int gain)
	{
#if 0
	// Set is always absolute mode
	band &= 1;
	shoe &= 1;
	chnl &= 0xf;	// up to 16 chnls per shoe

	if ( gSetChnlGain(m_nAmaSonoSel, band, shoe, chnl, gain) )
		{
		if (!m_nAmaSonoSel )
			{
			ConfigRec.AmaRec.CurrentShoe = shoe;
			ConfigRec.AmaRec.CurrentShoeChnl[shoe] = chnl;
			ConfigRec.AmaRec.CurrentShoeBand[shoe] = band;
			SendMsg( SET_ACHNL_GAIN);
			}
		else 
			{
			shoe = 0;
			ConfigRec.SonoRec.CurrentShoe = shoe;
			ConfigRec.SonoRec.CurrentShoeChnl[shoe] = chnl;
			ConfigRec.SonoRec.CurrentShoeBand[shoe] = band;
			SendMsg( SET_SCHNL_GAIN);
			}
		return TRUE;
		}
#endif
	return FALSE;

	}

int CAcal::GetChnlGain(int band, int shoe, int ch)
	{
#if 0
	band &= 1;
	shoe &= 1;
	ch &= 0xf;	// up to 16 chnls per shoe

	return gGetChnlGain(m_nAmaSonoSel, band, shoe, ch);

#endif
	return 0;
	}

void CAcal::FillBarArray(I_MSG_CAL *pMsg)
	{
	/* Data from Utron/Truscan sampled every 50 ms and sent to
		the MMI.  For the human eye, we
	   can find the max value and display it.
	*/
#if 1
	int si, ci, ib;
	BYTE ct;

	if ( pMsg->MstrHdr.MsgId != CAL_MODE) return;

	for (si = 0; si < MAX_SHOES; si++)
	{	// shoe loop
		ib = 10*si;	// bar index = 10*shoe index
		for ( ci = 0; ci < 10; ci++)
		{	// chnl loop
			ct = ConfigRec.UtRec.Shoe[si].Ch[ci].Type;

			switch (ct)
			{	// switch on type
			case IS_NOTHING:
				m_bSig[ib][0] = 0;
				m_bSig[ib][1] = 0;
				break;

			case IS_WALL:
				if (m_bPeakHoldFlag)
				{
					if (m_bSig[ib][0] < (BYTE) pMsg->Shoe[si].Amp[ci][1])
						m_bSig[ib][0] = (BYTE) pMsg->Shoe[si].Amp[ci][1];
					if (m_bSig[ib][1] < (BYTE) pMsg->Shoe[si].Amp[ci][0])
						m_bSig[ib][1] = (BYTE) pMsg->Shoe[si].Amp[ci][0];

					if (m_bSig[ib][2] > pMsg->Shoe[si].MinWall[ci])
						m_bSig[ib][2] = pMsg->Shoe[si].MinWall[ci];
					if (m_bSig[ib][3] < pMsg->Shoe[si].MaxWall[ci])
						m_bSig[ib][3] = pMsg->Shoe[si].MaxWall[ci];
				}
				else
				{
					if (m_bSig[ib][0] < (BYTE) pMsg->Shoe[si].Amp[ci][1])
						m_bSig[ib][0] = (BYTE) pMsg->Shoe[si].Amp[ci][1];
					if (m_bSig[ib][1] < (BYTE) pMsg->Shoe[si].Amp[ci][0])
						m_bSig[ib][1] = (BYTE) pMsg->Shoe[si].Amp[ci][0];

					m_bSig[ib][2] = pMsg->Shoe[si].MinWall[ci];
					m_bSig[ib][3] = pMsg->Shoe[si].MaxWall[ci];
				}

				// Prevent over drawing someone else's area
				m_bSig[ib][0] = __min(m_bSig[ib][0], 100);
				m_bSig[ib][1] = __min(m_bSig[ib][1], 100);
				break;

			default:
				if ( ((ct==IS_LONG) && m_bLong) || ((ct==IS_TRAN) && m_bTran) ||
					 ((ct==IS_OBQ1) && m_bO1) || ((ct==IS_OBQ2) && m_bO2) ||
					 ((ct==IS_OBQ3) && m_bO3) || ((ct==IS_WALL) && m_bLam) ||
					 m_bChnlRange )
				{
					for (int i=0; i< MAX_GATE ; i++)
					{
							if (1/*m_bPeakHoldFlag*/)
							{
								if (m_bSig[ib][i] < (BYTE) pMsg->Shoe[si].Amp[ci][i])
									m_bSig[ib][i] = (BYTE) pMsg->Shoe[si].Amp[ci][i];
							}
							else
							{
								m_bSig[ib][i] = (BYTE) pMsg->Shoe[si].Amp[ci][i];
							}

							// Prevent over drawing someone else's area
							m_bSig[ib][i] = __min(m_bSig[ib][i], 100);
					//		m_bSigOD[ib][i] = __min(m_bSigOD[ib][i], 100);

					}
				}
				else
				{
					for (int i=0; i< MAX_GATE ; i++)
						m_bSig[ib][i] = 0;
				}
				break;

			}	// switch on type

			ib++;
		}	// chnl loop
	}

	if (m_nGainSigSel)
	{	// show sig levels
		//s = _T("Signal");
		memcpy ((void *) m_pBarII, (void *) m_bSig, sizeof( m_bSig));
    }        
	else
		memcpy ((void *) m_pBarII, (void *) m_bGain, sizeof( m_bGain));

#endif
}

void CAcal::FillGainArray()
	{
	int si, ci, ib;


	for (si = 0; si < MAX_SHOES; si++)
		{	// shoe loop
		ib = 10*si;	// bar index = 10*shoe index
		for ( ci = 0; ci < 10; ci++)
			{	// chnl loop
				//m_bGain[ib+ci][0] = RcvrGainToPct( ConfigRec.receiver.gain[ib+ci] );
				m_bGain[ib+ci][0] = (BYTE) (m_pTcpThreadRxList->GetIdTcgGain(ib+ci)*1.25f);
				m_bGain[ib+ci][1] = (BYTE) (m_pTcpThreadRxList->GetOdTcgGain(ib+ci)*1.25f);
			}	// chnl loop
		}	// shoe loop

	}

int color[MAX_GATE]={RED,BLUE,ORANGE,GREEN};

/*int color[XDCR_TYPE+2]={WHITE,BROWN,ORANGE,GREEN,BLACK,
		BLACK2,BLACK3,YELLOW};
*/
void CAcal::DrawODBar(int shoe, CClientDC *pdc)
	{
/*	int j, chnl_base;

	chnl_base = shoe*10;
	for (j = 0; j < 10; j++)
		{
		pdc->FillSolidRect(1,(6*j+1+gGate),m_pBarOD[chnl_base+j],1,
			color[GetChnlTypeIndex(chnl_base+j)] );
		// if the bar got shorter, white out the end
		if (m_pBarOD[chnl_base+j] < m_bOldBarOD[chnl_base+j])
			pdc->FillSolidRect(m_pBarOD[chnl_base+j]+1,1,
				(m_bOldBarOD[chnl_base+j] - m_pBarOD[chnl_base+j]),
				1, RGB(255,255,255));
		m_bOldBarOD[chnl_base+j] = m_pBarOD[chnl_base+j];
		}
*/	}

void CAcal::DrawIDBar(int shoe, CClientDC *pdc)
{
/*
	penBlack(0,0,0));
	cyan - 0,255,255
	yellow	-	255,255,0
	
	  WALL-BROWN for gain only. NO SIGNAL

	XDCR_TYPE - ORANGE ,GREEN,	BLACK,	BLACK2,	BLACK3,	YELLOW
				LONG,	TRAN,	OBQ1,	OBQ2,	OBQ3,	LAMN

*/
	
	int j, chnl_base;
	BYTE ct;
	CString sWall;
	// 
	chnl_base = shoe*10;
	for (j = 0; j < 10; j++)
	{
		ct = ConfigRec.UtRec.Shoe[shoe].Ch[j].Type;

		if ( (ct != IS_WALL) || m_bLam )
		{
			for (int i=0; i< 2/*MAX_GATE*/ ;i++)
			{
							pdc->FillSolidRect(1,(4+j/2+14*j+i*6),m_pBarII[chnl_base+j][i],4,
									color[i] );
							/*
							// if the bar got shorter, white out the end
							if (m_pBarII[chnl_base+j][i] < m_bOldBarID[chnl_base+j][i])
									pdc->FillSolidRect(m_pBarII[chnl_base+j][i]+1,1,
											(m_bOldBarID[chnl_base+j][i] - m_pBarII[chnl_base+j][i]),
											1, RGB(255,255,255));
							*/
							m_bOldBarID[chnl_base+j][i] = m_pBarII[chnl_base+j][i];
			}
		}
		else
		{
			sWall.Format(_T("%d   %d"), m_bSig[chnl_base+j][2], m_bSig[chnl_base+j][3]);
			pdc->SetBkColor(RGB(255,255,255));
			pdc->TextOut(1,(4+j/2+14*j), sWall);
		}
	}
}

void CAcal::UpdateData(BOOL flag)
	{	// our virtual function to update data and write to/read from
		// config record.

	if (flag)
		{	// copy class variables to config record variables
		CDialog::UpdateData(flag);	// screen to class members
//		UpdateConfigRec();			// class members to config rec
		}
	else
		{	// get variable values from config record and put into
			// class member variables
//		LoadConfigRec();		// config rec elements -> class members
		CDialog::UpdateData(flag);	// class members to screen
		}

	}

void CAcal::LoadConfigRec()
	{
	int i;
	gChannel %= MAX_CHANNEL;
	m_nChnlSel = gChannel;
//	m_nChnlSel = ConfigRec.UtRec.CurrentChnl % MAX_CHANNEL;
/*	m_nCalLvlOd = (BYTE) (ConfigRec.UtRec.bCalLvlOd % 100);
	m_nCalLvlEn = m_nCalLvlId = (BYTE) (ConfigRec.UtRec.bCalLvlId % 100);
	m_nRbG1 = ConfigRec.UtRec.bOdCalChk & 1;	// 0 = id, 1 = od
	m_bCkOd = m_bCkId = 1;
//	m_bCkId = ConfigRec.UtRec.bIdCalChk & 1;
*/
	if (ConfigRec.UtRec.bCalTypes & IS_LONG) m_bLong = TRUE;
	else m_bCkLong = FALSE;
	if (ConfigRec.UtRec.bCalTypes & IS_TRAN) m_bTran = TRUE;
	else m_bCkTran = FALSE;

	if (ConfigRec.UtRec.bCalTypes & IS_OBQ1) m_bO1 = TRUE;
	else m_bCkQ1 = FALSE;
	if (ConfigRec.UtRec.bCalTypes & IS_OBQ2) m_bO2 = TRUE;
	else m_bCkQ2 = FALSE;
	if (ConfigRec.UtRec.bCalTypes & IS_OBQ3) m_bO3 = TRUE;
	else m_bCkQ3 = FALSE;



	for ( i = 0; i < MAX_CHANNEL; i++)
		{
		if (ConfigRec.receiver.gain[i]  > 600) ConfigRec.receiver.gain[i] = 600;
		if (ConfigRec.receiver.gain[i]  < -200) ConfigRec.receiver.gain[i] = -200;
		//m_bGain[i][0] = RcvrGainToPct( ConfigRec.receiver.gain[i] );
		m_bGain[i][0] = (BYTE) (m_pTcpThreadRxList->GetIdTcgGain(i)*1.25f);
		m_bGain[i][1] = (BYTE) (m_pTcpThreadRxList->GetOdTcgGain(i)*1.25f);
		}

	m_nOdGain = m_bGain[m_nChnlSel][0];
	}

void CAcal::UpdateConfigRec()
	{
//	int i;


	if ( m_bLong) ConfigRec.UtRec.bCalTypes |= IS_LONG;
	else ConfigRec.UtRec.bCalTypes &= ~IS_LONG;

	if ( m_bTran) ConfigRec.UtRec.bCalTypes |= IS_TRAN;
	else ConfigRec.UtRec.bCalTypes &= ~IS_TRAN;

	if ( m_bO1) ConfigRec.UtRec.bCalTypes |= IS_OBQ1;
	else ConfigRec.UtRec.bCalTypes &= ~IS_OBQ1;


	if ( m_bO2) ConfigRec.UtRec.bCalTypes |= IS_OBQ2;
	else ConfigRec.UtRec.bCalTypes &= ~IS_OBQ2;


	if ( m_bO3) ConfigRec.UtRec.bCalTypes |= IS_OBQ3;
	else ConfigRec.UtRec.bCalTypes &= ~IS_OBQ3;

	//ConfigRec.UtRec.CurrentChnl = m_nChnlSel;
/*	ConfigRec.UtRec.bCalLvlOd = m_nCalLvlOd;
	ConfigRec.UtRec.bCalLvlId = m_nCalLvlId;
	ConfigRec.UtRec.bOdCalChk = m_nRbG1;
	ConfigRec.UtRec.bIdCalChk = m_nRbG1;
*/
	//memcpy(&ConfigRec.UtRec.TholdLong,&m_nCalLvlId, MAX_GATE*XDCR_TYPE);

  	for (int i = 0; i < MAX_CHANNEL; i++)
		ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Gain[1]= m_bGain[i][0];
	}

CAcal::~CAcal()
	{
	delete m_pUndo;
	}

void CAcal::OnSave() 
	{
	// TODO: Add your control notification handler code here
	// Save config rec to hard disk
	// Make sure we are saving the inspection operational thresholds
//	m_nTholdLvlSel = 1;	// restore inspection level tholds
//	SetCalTholds();
	pCTscanDlg->FileSaveAs();
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
	ShowDlgTitle();
	UpdateData(FALSE);	// Copy variables to screen
	}

void CAcal::UpdateScrollInfo()
	{
#if 0

	m_nS1ChGain = GetChnlGain(m_nS1Band,0,m_nChnlSel);
	m_sbS1ChGain.SetScrollPos(m_nS1ChGain);
	SetDlgItemInt(IDC_EDIT_S1_CH, m_nS1ChGain, FALSE);

#endif
	}


void CAcal::DrawTholds()
	{
#if 0
	CRect CalRect[4];

	CClientDC dc1(&m_bAcal1);	// get drawing context shoe 1
	CClientDC dc2(&m_bAcal2);	// get drawing context shoe 2
	CClientDC dc3(&m_bAcal3);	// get drawing context shoe 2
	CClientDC dc4(&m_bAcal4);	// get drawing context shoe 2
	CClientDC *pdc[4];

	int i;

	
	pdc[0] = &dc1;
	pdc[1] = &dc2;
	pdc[2] = &dc3;
	pdc[3] = &dc4;

	// Get the client rectangle boundaries for the Image Map graph (Button)
	m_bAcal1.GetClientRect(&CalRect[0]);
	m_bAcal2.GetClientRect(&CalRect[1]);
	m_bAcal3.GetClientRect(&CalRect[2]);
	m_bAcal4.GetClientRect(&CalRect[3]);
	

	for ( i = 0; i < 4; i++)
		{
		CalRect[i].NormalizeRect();
		// Move the coordinate origin to the upper left corner
		pdc[i]->SetViewportOrg( 0, 0);	// CalRect[i].Height());
		// Set mapping mode to AnIsotropic, deal in logical units rather than pixels
		pdc[i]->SetMapMode(MM_ANISOTROPIC);

		pdc[i]->SetWindowExt(100,100);	// define size of window in logical units
		// tell windows how big this box is physically in pixels
		pdc[i]->SetViewportExt( CalRect[i].Width(), CalRect[i].Height());
		}

	// Erase previous tholds if they exist
	if ( m_nOdLvlLast > 0)
		for ( i = 0; i < 4; i++)
			{
			pdc[i]->SelectObject(&penWhite2);
			pdc[i]->MoveTo(m_nOdLvlLast, 1);
			pdc[i]->LineTo(m_nOdLvlLast, 99);
			}
	m_nOdLvlLast = m_nCalLvlOd;

	if ( m_nIdLvlLast > 0)
		for ( i = 0; i < 4; i++)
			{
			pdc[i]->MoveTo(m_nIdLvlLast, 1);
			pdc[i]->LineTo(m_nIdLvlLast, 99);
			}
	m_nIdLvlLast = m_nCalLvlId;


	// Draw OD std level in blue

	if (m_nRbG1 == 1) for ( i = 0; i < 4; i++)
		{
		pdc[i]->SelectObject(&penBlue1);
		pdc[i]->MoveTo(m_nCalLvlOd, 1);
		pdc[i]->LineTo(m_nCalLvlOd, 99);
		}

	// Draw ID std level in red
	if (m_nRbG1 == 0) for ( i = 0; i < 4; i++)
		{
		pdc[i]->SelectObject(&penRed1);
		pdc[i]->MoveTo(m_nCalLvlId, 1);
		pdc[i]->LineTo(m_nCalLvlId, 99);
		}
#endif
	}

void CAcal::SetCalLvlPtrs()
	{
#if 0
	CString s;

	if ( m_nAmaSonoSel == 0)
		{	// amalog
		m_pIdLast = &m_nAidLast;
		m_pOdLast = &m_nAodLast;
		m_pIdLvl  = m_pAid;
		m_pOdLvl  = m_pAod;
		s = _T("AMALOG");
		}
	else
		{
		m_pIdLast = &m_nSidLast;
		m_pOdLast = &m_nSodLast;
		m_pIdLvl  = m_pSid;
		m_pOdLvl  = m_pSod;
		s	= _T("SONOSCOPE");
		}
	GetDlgItem(ID_AMA_SONO_SEL)->SetWindowText(s);
	m_sbAcalLvlOD.SetScrollPos((int) m_nCalLvlOd);
	m_sbAcalLvlID.SetScrollPos((int) m_nCalLvlId);
#endif
	}

void CAcal::ShowDlgTitle()
	{
	// show the config file name and TruImage moniker
	CString s,t,u;
	static int toggle;


	//s = _T("TruScan Flaw Calibration   Cfg file = ");
	s.Format(_T("%s Flaw Calibration    Cfg File = "), SYSTEM_NAME);
	u = ConfigRec.JobRec.CfgFile;
	if ( u.GetLength() < 50)
		{
		t = ConfigRec.JobRec.CfgFile;
		s += t;
		}

	if ( m_bBeginCal)
		{
		toggle &= 1;
		toggle ^= 1;
		if ( toggle)
			{
            s = _T("Calibration in Progress        ");
			}
		}
	SetWindowText(s);

	}

BOOL CAcal::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Asidlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CAcal::OnContextMenu(CWnd* pWnd, CPoint point) 
	{
	// TODO: Add your message handler code here
	// copy code from inspect.cpp.  Change menu resource to
	// add a 2nd null menu item with a submenu.  The 1st submenu
	// is used by inspect and has chnlon/gain/filter/noise/thold
	// the 2nd submenu is used here and has only chnlon/filter
	CMenu Menu;
	CPoint pt;
	CRect rect;

	// p 117, Teach Yourself VC++ 6 in 21 days & p238
	// p238 Prosise

//	CSetChGn dlg;
//	int nResponse = dlg.DoModal();
	LoadConfigRec();
//	m_sbIdGain.SetScrollPos(m_nIdGain);
//	m_sbOdGain.SetScrollPos(m_nOdGain);
//	SendMsg( SET_ALL_GAINS);
	UpdateData(FALSE);	// Copy variables to screen

#if 0
	// Preserve the system menu properties
	pt = point;
	GetClientRect(&rect);
	ScreenToClient(&pt);

	// Was the mouse clicked on the client area or system area?
	if ( rect.PtInRect (pt) )
		{	// client area
		Menu.LoadMenu(IDR_MENU_INSPECT);

//		CMenu *pContextMenu = Menu.GetSubMenu(0);	// used in inspect.cpp
		CMenu *pContextMenu = Menu.GetSubMenu(1);
		pContextMenu->TrackPopupMenu( TPM_LEFTALIGN |
						TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
						point.x, point.y, this, NULL);
		}
	else CWnd::OnContextMenu(pWnd,point);	// system area
#endif

	CWnd::OnContextMenu(pWnd,point);
	}

void CAcal::OnFilter() 
	{
#if 0
	// TODO: Add your command handler code here
	// copy code from inspect.cpp

	if (CFilter::m_pDlg == NULL)
		{
		CFilter::m_pDlg = new CFilter;
		if (CFilter::m_pDlg)
			{
//			CFilter::m_pDlg->m_nAmaSonoSel = m_nAmaSonoSel;
			CFilter::m_pDlg->Create(IDD_FILTER_NOISE);
			}
		}

	// If this window gets covered up by inspect, kill it
	// then the next attempt to show it will succeed.
	// Always on top may not be good since we would be tempted to 
	// do that with all the context menu selections which could
	// not have all of them on top.

	else CFilter::m_pDlg->KillMe();	// kill the window
	
#endif
	}

void CAcal::OnChnlOn() 
	{
	// TODO: Add your command handler code here
	// copied from OnFilter above

	if (CChnlOn::m_pDlg == NULL)
		{
		CChnlOn::m_pDlg = new CChnlOn;
		if (CChnlOn::m_pDlg)
			{
//			CChnlOn::m_pDlg->m_nAmaSonoSel = m_nAmaSonoSel;	// which machine
			CChnlOn::m_pDlg->Create(IDD_CHNLON);
			}
		}

	// If this window gets covered up by inspect, kill it
	// then the next attempt to show it will succeed.
	// Always on top may not be good since we would be tempted to 
	// do that with all the context menu selections which could
	// not have all of them on top.

	else CChnlOn::m_pDlg->KillMe();	// kill the window
	
	}

void CAcal::OnAcalErase() 
	{
	// TODO: Add your control notification handler code here
	EraseBars();
	UpdateScrollInfo();
	}

//03/29/00
// If gains can not be adjusted to bring all signals to reference
// level, do not change any gain and report False for Adjust effort
// If gains can be changed, change all and report True for Adjust effort
BOOL CAcal::AdjustAmaHp()
	{	// Adapted from gsetbandgain
#if 0
	int ic, is;
	float fgtmp[2][16];		// attempt to set gain on temp array 1st
	float fratio;			// new/old ratio
	float fsig, fref;
	int band = 0;	//od or hp
	int nreturn;

	// copy new cal adjusted gains to fgtmp if we can

	fref = (float) (m_nCalLvlOd) * 2.55f;	// percent to 8 bit lvl

	for ( is = 0; is < 2; is++)
		{	// shoe iteration
		for ( ic = 0; ic < 16; ic++)
			{
			if (!gIsChnlOn(0,is,ic) )	continue;	// this chnl off, skip it
			// chnl is on
			fsig = (float) m_bSigOD[16*is + ic];
			if ( fsig > 1.0) fratio = fref / fsig;
			else fratio = 1.0f;

			fgtmp[is][ic] = 
				fratio * ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].fGain;
			if ( fgtmp[is][ic] < 2.0f )
				{
				nreturn = 0;
				return nreturn;	// nope, can't do this
				}
			if ( fgtmp[is][ic] > 254.5f)
				{
				nreturn = 0;
				return nreturn;	// nope, can't do this
				}
			}
		}	// shoe iteration

	// Success
	for ( is = 0; is < 2; is++)
		{
		for ( ic = 0; ic < 16; ic++)
			{
			if (!gIsChnlOn(0,is,ic) )	continue;
			ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].fGain = fgtmp[is][ic];
			ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].Gain = 
						(BYTE) (fgtmp[is][ic] + 0.5f);
			}
		}
#endif			
	return TRUE;
	}



BOOL CAcal::AdjustAmaLp()
	{
#if 0
	// Absolute chnl gains set to 1.00 (100%).  All adjustments made 
	// to differential channels
	int ic, is;
	float fgtmp[2][16];		// attempt to set gain on temp array 1st
	float fratio;			// new/old ratio
	float fsig, fref;
	int band = 1;	//id or lp
	int nreturn;

	// copy new cal adjusted gains to fgtmp if we can

	fref = (float) (m_nCalLvlId) * 2.55f;	// percent to 8 bit lvl

	for ( is = 0; is < 2; is++)
		{	// shoe iteration
		for ( ic = 0; ic < 16; ic++)
			{
			if (!gIsChnlOn(0,is,ic) )	continue;	// this chnl off, skip it
			// chnl is on
			fsig = (float) m_bSig[16*is + ic][gGate];
			if ( fsig > 1.0) fratio = fref / fsig;
			else fratio = 1.0f;

			fgtmp[is][ic] = 
				fratio * ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].fDifGain;
			if ( fgtmp[is][ic] < 2.0f )
				{
				nreturn = 0;
				return nreturn;	// nope, can't do this
				}
			if ( fgtmp[is][ic] > 254.5f)
				{
				nreturn = 0;
				return nreturn;	// nope, can't do this
				}
			}
		}	// shoe iteration

	// Success
	for ( is = 0; is < 2; is++)
		{
		for ( ic = 0; ic < 16; ic++)
			{
			if (!gIsChnlOn(0,is,ic) )	continue;
			ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].fDifGain = fgtmp[is][ic];
			ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].DifGain = 
						(BYTE) (fgtmp[is][ic] + 0.5f);

//			ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].fGain = (float) NOM_DIF_GAIN;
//			ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].Gain = NOM_DIF_GAIN;

			}
		}
#endif			
	return TRUE;
	

	}

void CAcal::TrackIDNotch(I_MSG_CAL *pMsg)
	{
#if 0
	// IF in cal mode and amalog id is being cal'ed,
	// turn off downstream chnls before peak holding
	AMA_TIME_SAMPLE *pASample;
	int imax;	// chnl location of largers OD signal
	int i, j, maxsig;
	int imax1, imax2;		// max od chnl for shoe 1/2
	int msig1, msig2;		// od sig level

	if ( m_nAmaSonoSel || (m_nCalBand != 1) || (m_bBeginCal != TRUE))
		{
		memset( (void *) m_bIdMask , 0xff , sizeof (m_bIdMask));
		return;	// Sono or od
		}

	// Here we have AmalogLp signals
	// Track notch location with od chnls

	pASample = &pMsg->AmaTime.sample[0];

	maxsig = 1;
	imax = 0xff;	// no max found
	msig1 = msig2 = 10;
	imax1 = imax2 = 0xff;

#if TRACK_ID
	// One caveat, track notch must be called after FillBarArray.
	for ( i = 0; i< 16; i++)
		{
		// Aid in %, need raw a/d counts for 2/3 of thold
		// 255/100*2/3 ~ 5/3
		if ((pASample->bS1IdCh[i] > *m_pAid*5/3)  &&
			(pASample->bS1IdCh[i] > maxsig) )
			{
			maxsig = pASample->bS1IdCh[i];
			imax = i;
			}
		}

	if ( (imax < 16) && (imax > m_nNotchLocation) )
		{
		j = max(0, imax - 2);
		for ( i = 0; i <= j; i++) 
			{	// turn off trailing channel gain
			m_bIdMask[i] = m_bIdMask[16 + i] = 0;
			}
		if ( imax + 2 < 16)
			{	// turn on forward channel gain
			m_bIdMask[imax + 2] = m_bIdMask[16 + imax + 2] = 0xff;
			}
		m_nNotchLocation = imax;
		}
#else
	// Track  with od notches.  Turn ON od coil , +1 & -1 away
	// One caveat, track notch must be called before FillBarArray.

	for ( i = 0; i< 16; i++)
		{
		if (pASample->bS1OdCh[i] > 30)
			{
			if (msig1 < pASample->bS1OdCh[i])
				{
				msig1 = pASample->bS1OdCh[i];
				imax1 = i;
				}
			}

		// SHOE 2
		if (pASample->bS2OdCh[i] > 30)
			{
			if (msig2 < pASample->bS2OdCh[i])
				{
				msig2 = pASample->bS2OdCh[i];
				imax2 = i;
				}
			}

		}

	if ( (imax1 >= 0) && (imax1 < 16) )
		{
		memset ( (void *) m_bIdMask, 0, 16);
		m_bIdMask[imax1] = 0xff;	// od peak chnl
		j = max(0, imax1-1);
		m_bIdMask[j] = 0xff;		// behind od peak chnl
		j = min(15, imax1+1);
		m_bIdMask[j] = 0xff;		// ahead of od peak chnl
		}

	if ( (imax2 >= 0) && (imax2 < 16) )
		{
		memset ( (void *) &m_bIdMask[16], 0, 16);
		m_bIdMask[imax2+16] = 0xff;	// od peak chnl
		j = max(0, imax2-1);
		m_bIdMask[j+16] = 0xff;		// behind od peak chnl
		j = min(15, imax2+1);
		m_bIdMask[j+16] = 0xff;		// ahead of od peak chnl
		}

#endif
#endif
	}

HBRUSH CAcal::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
	{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here
	// copy from inspect.cpp code
	// See Prosise p 376

	if ( nCtlColor == CTLCOLOR_STATIC)
	{
//        if (pWnd->m_hWnd == GetDlgItem(IDC_Swall)->m_hWnd)
//                        pDC->SetTextColor(BROWN);

		if (pWnd->m_hWnd == GetDlgItem(IDC_Slong)->m_hWnd)
						pDC->SetTextColor(ORANGE);	
		if (pWnd->m_hWnd == GetDlgItem(IDC_Stran)->m_hWnd)
						pDC->SetTextColor(GREEN);	
		if (pWnd->m_hWnd == GetDlgItem(IDC_So1)->m_hWnd)
						pDC->SetTextColor(BLACK);
		if (pWnd->m_hWnd == GetDlgItem(IDC_So2)->m_hWnd)
						pDC->SetTextColor(BLACK2);	
		if (pWnd->m_hWnd == GetDlgItem(IDC_So3)->m_hWnd)
						pDC->SetTextColor(BLACK3);	
		if (pWnd->m_hWnd == GetDlgItem(IDC_Slamn)->m_hWnd)
						pDC->SetTextColor(YELLOW);
//			CheckRadioButton( IDC_BtID, IDC_BtOD, IDC_BtID );
		if (pWnd->m_hWnd == GetDlgItem(IDC_BtID1)->m_hWnd)
						pDC->SetTextColor(RED);
		if (pWnd->m_hWnd == GetDlgItem(IDC_BtOD1)->m_hWnd)
						pDC->SetTextColor(BLUE);

	for (int i=0; i<MEM_MAX_CHANNEL;i++)
	{
		int j = i+IDC_ACAL_CH1;
		if  (pWnd->m_hWnd == GetDlgItem(j)->m_hWnd)
			//&& 	(i<MEM_MAX_CHANNEL)  )
		{
		char type = GetChnlTypeIndex(i);
			switch (type)
			{
			case 0:		//IS_NOTHING:
			default:
				pDC->SetTextColor(WHITE);
				break;
			case 1:		//IS_WALL:
				pDC->SetTextColor(BROWN);
				break;

			case 2:		//IS_LONG:
				pDC->SetTextColor(ORANGE);
				break;
			case 3:		//IS_TRAN:
				pDC->SetTextColor(GREEN);
				break;

			case 4:		//IS_OBQ1:
				pDC->SetTextColor(BLACK);	
				break;
			case 5:		//IS_OBQ2:
				pDC->SetTextColor(BLACK2);	
				break;

			case 6:		//IS_OBQ3:
				pDC->SetTextColor(BLACK3);	
				break;
			case 7:		//IS_LAM:
				pDC->SetTextColor(YELLOW);
				break;
		
			}
		}
	}

		if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_XDCR)->m_hWnd)
		{
			switch(xtype)
			{
			case 0:		//IS_NOTHING:
			default:
				pDC->SetTextColor(WHITE);
				break;
			case 1:		//IS_WALL:
				pDC->SetTextColor(BROWN);
				break;

			case 2:		//IS_LONG:
				pDC->SetTextColor(ORANGE);
				break;
			case 3:		//IS_TRAN:
				pDC->SetTextColor(GREEN);
				break;

			case 4:		//IS_OBQ1:
				pDC->SetTextColor(BLACK);	
				break;
			case 5:		//IS_OBQ2:
				pDC->SetTextColor(BLACK2);	
				break;

			case 6:		//IS_OBQ3:
				pDC->SetTextColor(BLACK3);	
				break;
			case 7:		//IS_LAM:
				pDC->SetTextColor(YELLOW);
				break;
		
			}
		}
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
	}

void CAcal::CheckHeap(int n)
	{
	// Debug version works.  Release version crashes when
	// we try to run this module.    Memory at 0531 could not
	// be read is the message.  Is it a heap problem in 
	// release mode

	CString s;
	return;

	if (_heapchk() != _HEAPOK)
		{
		s.Format(_T("CheckHeap failed at number %d"),n);
//		MessageBox(s);
		}
	else
		{
		s.Format(_T("Ok at %d"),n);
		}
	MessageBox(s);

	}

void CAcal::OnCkLong() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	UpdateConfigRec();
	}
void CAcal::OnCkOblq1() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	UpdateConfigRec();
	}

void CAcal::OnCkOblq2() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	UpdateConfigRec();
	}

void CAcal::OnCkOblq3() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	UpdateConfigRec();
	}


void CAcal::OnCkTran() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables 
	UpdateConfigRec();
	}


int CAcal::GetChnlTypeIndex(int Chnl)
{
	// Given the chnl number selected, return the index of the chnl type
	BYTE bType;

	bType = ConfigRec.UtRec.Shoe[Chnl/10].Ch[Chnl % 10].Type;
	switch (bType)
	{
	case IS_NOTHING:
	default:
		return 0;
	case IS_WALL:
		if (m_pTcpThreadRxList->GetGateTrigMode(Chnl, 1) != 0)
			return 7;
		else
			return 1;
	case IS_LONG:
		return 2;

	case IS_TRAN:
		return 3;

	case IS_OBQ1:
		return 4;

	case IS_OBQ2:
		return 5;

	case IS_OBQ3:
		return 6;
	case IS_LAM:
		return 7;

	}

}


void CAcal::ShowChnlInfo()
	{	// Based on chnl selected and xdcr type assigned
		// Update info in chnl group box
	int is, ic;
	CString s;

	is = m_nChnlSel/10;
	ic = m_nChnlSel % 10;
	s.Format(_T("S%d:Ch%d"), is+1,ic+1);
	GetDlgItem(IDC_STATIC_CH)->SetWindowText(s);
	s = XdcrText[xtype];
	GetDlgItem(IDC_STATIC_XDCR)->SetWindowText(s);
	s.Format(_T("ID = %d"), m_pBarII[m_nChnlSel][0]);
	s += _T("%");
	GetDlgItem(IDC_STATIC_ID_SIG)->SetWindowText(s);
	s.Format(_T("OD = %d"), m_pBarII[m_nChnlSel][1]);
	s += _T("%");
	GetDlgItem(IDC_STATIC_OD_SIG)->SetWindowText(s);

	s.Format(_T("%5.1f dB"), ((float) ConfigRec.receiver.gain[m_nChnlSel])/10.0f);
	m_rGain = s;
	UpdateData(FALSE);
	}

void CAcal::OnRbCal() 
	{
	// TODO: Add your control notification handler code here
	// Set thresholds in instrument to 3% to allow almost all signals thru
//	UpdateData(TRUE);	// Copy screen TO variables 
//	m_nTholdLvlSel=0;
//	SetCalTholds();
	
	}

void CAcal::OnRbInsp() 
	{
	// TODO: Add your control notification handler code here
	// Set thresholds to operational inspection levels
//	UpdateData(TRUE);	// Copy screen TO variables 
//	m_nTholdLvlSel=1;
//	SetCalTholds();
	
	}

void CAcal::SetCalTholds()
	{
	// Set the instrument threshold level for signal reject.
	// This is not the calibration level.  It allows or blocks
	// the signal from the dsp from getting to the instrument
	// processor.
	// Selection of thold set made on basis of m_nTholdLvlSel
	// Tholds are byte variables for long/tran/oblq in per cent
/*
	if ( m_nTholdLvlSel)
		{	// set tholds to inspection levels
		memcpy( (void *) &ConfigRec.UtRec.TholdLong,
				(void *) &m_pUndo->UtRec.TholdLong, MAX_GATE*XDCR_TYPE); 
		}
	else
		{	// set tholds to 3 % for all channels
		memset( (void *) &ConfigRec.UtRec.TholdLong, 3, MAX_GATE*XDCR_TYPE);
		}
//	memcpy(&m_nCalLvlId,&ConfigRec.UtRec.TholdLong, MAX_GATE*MAX_SHOES);
	SendMsg(SET_ALL_THOLDS);	// all tholds
*/	}



void CAcal::OnAcalGainSig() 
	{
	// TODO: Add your control notification handler code here
	CString s;

/*
	m_nGainSigSel ^= 1;
	m_nGainSigSel &= 1;
	if (m_nGainSigSel)
		{	// show sig levels
//		s = _T("Signal");
       memcpy ((void *) m_pBarID, (void *) m_bSig, sizeof( m_bSig));


//		for (int i=0;i<MAX_GATE;i++) m_pBarID[i] =  &m_bSig[0][i];
/*		m_pBarID =  &m_bSig[0][gGate];
        m_pBarOD = &m_bSigOD[0][gGate];
		}
/*	else
		{	// show gains
//		s = _T("Gain");
//		for (int i=0;i<MAX_GATE;i++) m_pBarID[i] =  &m_bGain[i][0];
	memcpy ((void *) m_pBarID, (void *) m_bGain, sizeof( m_bGain));
//		m_pBarID[0] = &m_bGain[0][0];
		}
	GetDlgItem(ID_ACAL_GAIN_SIG)->SetWindowText(s);
*/
	EraseBars();
	m_nGainSigSel = 1;
	GetDlgItem(ID_ACAL_PEAK_HOLD)->EnableWindow(TRUE);
	GetDlgItem(ID_ACAL_PEAK_HOLD2)->EnableWindow(TRUE);
	memcpy ((void *) m_pBarII, (void *) m_bSig, sizeof( m_bSig));
	
	}

BYTE CAcal::RcvrGainToPct(int gain)
	{
	// Convert receiver gain (-200 to +600 range. -20db to +60 db
	// to value from 0 to 100

	return (BYTE) ((gain + 200)/8);

	}

BYTE CAcal::PctToRcvrGain(int pcent)
	{
	// Convert receiver gain (-200 to +600 range. -20db to +60 db
	// to value from 0 to 100
	return (BYTE) ((pcent*8) -200);

	}


void CAcal::TurnOnAll(int ChnlType)
	{
	// Turn ON all channels of type ChnlType (IS_WALL, IS_LONG, etc )
	int si, ci, i;

	for ( si = 0; si < MAX_SHOES; si++)
		for ( ci = 0; ci < 10; ci++)
			if (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == ChnlType)
				ConfigRec.UtRec.Shoe[si].Ch[ci].On = 1;

	// Record in config file what is being displayed on this trace
	switch (ChnlType)
		{
	case IS_NOTHING:
	default:			i = 11;				break;

	case IS_WALL:		i = 0;				break;
	case IS_LONG:		i = 1;				break;
	case IS_TRAN:		i = 2;				break;
	case IS_OBQ1:		i = 3;				break;
	case IS_OBQ2:		i = 4;				break;
	case IS_OBQ3:		i = 5;				break;		
	case IS_LAM:		i = 6;				break;		
		}

	ConfigRec.UtRec.DisplayChannel[i] = 0xff;		// all for this type
	}

void CAcal::TurnOffAll(int ChnlType)
	{
	// Turn OFF all channels of type ChnlType (IS_WALL, IS_LONG, etc )
	int si, ci;

	for ( si = 0; si < MAX_SHOES; si++)
		for ( ci = 0; ci < 10; ci++)
			if (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == ChnlType)
				ConfigRec.UtRec.Shoe[si].Ch[ci].On = 0;

	}

int CAcal::GetChnlNumber(char *s)
	{
	// pick the chan number of the the text string of a combo box
	// Combo string is 1-40 based, storage is 0-39 based

	char buf[24], *p;
	int i;

	strcpy(buf, s);
	p = strstr(buf,"Ch");
	sscanf(&p[2], "%d", &i);	// human form 1-40
	if ( i ) 	return i-1;
	return 0;
	}

void CAcal::OnSelchangeCBgate() 
{
	// TODO: Add your control notification handler code here
//	gGate=m_CBgate.GetCurSel ();
//	OnInitDialog();
	if (Ctof::m_pDlg)
		Ctof::m_pDlg->OnInitDialog();
	OnInitDialog();
	
}


void CAcal::OnAllFlaws2() 
{
	// TODO: Add your control notification handler code here
	ConfigRec.cal.AllFlawsFlag ^= 1;
	m_bAllflaw2 = ConfigRec.cal.AllFlawsFlag;

//	ConfigRec.cal.FlawDepthFlag=0;
//	CheckRadioButton( IDC_AllFlaws,IDC_FlawDpth,IDC_AllFlaws);
	OnInitDialog();
	SendMsg(SEND_ALL_FLAWS);
	
}



void CAcal::OnBtID() 
{
	// TODO: Add your control notification handler code here
	CheckRadioButton( IDC_BtID1, IDC_BtOD1, IDC_BtID1 );
	if (Ctof::m_pDlg)
		Ctof::m_pDlg->OnInitDialog();
	m_bCkId = TRUE;
	m_bCkOd = FALSE;
	if (m_bCkId)
	{
		m_sbCalLvlId.SetScrollPos(m_nCalLvlId[xtype][0]);
		m_nCalLvlEn=m_nCalLvlId[xtype][0];
	}
	else
	{
		m_sbCalLvlId.SetScrollPos(m_nCalLvlId[xtype][1]);
		m_nCalLvlEn=m_nCalLvlId[xtype][1];
	}
	UpdateData(FALSE);
}

void CAcal::OnBtOD() 
{
	// TODO: Add your control notification handler code here
	CheckRadioButton( IDC_BtID1, IDC_BtOD1, IDC_BtOD1 );
	if (Ctof::m_pDlg)
		Ctof::m_pDlg->OnInitDialog();
	m_bCkOd = TRUE;
	m_bCkId = FALSE;
	if (m_bCkId)
	{
		m_sbCalLvlId.SetScrollPos(m_nCalLvlId[xtype][0]);
		m_nCalLvlEn=m_nCalLvlId[xtype][0];
	}
	else
	{
		m_sbCalLvlId.SetScrollPos(m_nCalLvlId[xtype][1]);
		m_nCalLvlEn=m_nCalLvlId[xtype][1];
	}
	UpdateData(FALSE);
}

void CAcal::OnAcalPeakHold2() 
{
	m_bPeakHoldFlag = 0;	
}

void CAcal::OnAcalGainSig2() 
{
	// TODO: Add your control notification handler code here
	memcpy ((void *) m_pBarII, (void *) m_bGain, sizeof( m_bGain));
	m_nGainSigSel = 0;	
	GetDlgItem(ID_ACAL_PEAK_HOLD)->EnableWindow(FALSE);
	GetDlgItem(ID_ACAL_PEAK_HOLD2)->EnableWindow(FALSE);
}

void CAcal::OnSlong() 
{
	// TODO: Add your control notification handler code here
	m_bLong ^= 1;	
	UpdateConfigRec();
	
}

void CAcal::OnSlamn() 
{
	// TODO: Add your control notification handler code here
	m_bLam ^= 1;	
	UpdateConfigRec();
	
}

void CAcal::OnSo1() 
{
	// TODO: Add your control notification handler code here
	m_bO1 ^= 1;	
	UpdateConfigRec();
	
}

void CAcal::OnSo2() 
{
	// TODO: Add your control notification handler code here
	m_bO2 ^= 1;	
	UpdateConfigRec();
	
}

void CAcal::OnSo3() 
{
	// TODO: Add your control notification handler code here
	m_bO3 ^= 1;	
	UpdateConfigRec();
	
}

void CAcal::OnStran() 
{
	// TODO: Add your control notification handler code here
	m_bTran ^= 1;
	UpdateConfigRec();

}

void CAcal::OnACALCALcopy() 
{
	// TODO: Add your control notification handler code here
/*	int nSrcType, nDestType;


	nSrcType = ChnlDisplay(gChannel);
	nDestType = ChnlDisplay(gCopyTo);
*/
		{	// all of active chnl type to all other chnls of same type
		calicopy dlg;
		dlg.m_pTcpThreadRxList = m_pTcpThreadRxList;
		if ( dlg.DoModal() == IDOK)
			{
			}
		}
	
}

void CAcal::OnAcalSave() 
{
	// TODO: Add your control notification handler code here
	m_pTcpThreadRxList->SendSlaveMsg(NOP_MODE,0,0,0,0,0,0);
	pCTscanDlg->FileSave();
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
	ShowDlgTitle();
	UpdateData(FALSE);	// Copy variables to screen
	m_pTcpThreadRxList->SendSlaveMsg(CAL_MODE,0,0,0,0,0,0);
}

void CAcal::StartAcquireData()
{
	for (int i=0; i<MEM_MAX_CHANNEL; i++)
	{
		m_pTcpThreadRxList->m_fOdTcgGain[i] = ConfigRec.receiver.TcgRec[i].GateGain[2];
		//m_pTcpThreadRxList->ReSetTcgGainTable(0, FALSE);
	}

	m_pTcpThreadRxList->SendSlaveMsg(CAL_MODE,0,0,0,0,0,0);
}


void CAcal::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}


void CAcal::TurnOffGroupTcg()
{
	char type;

	for ( int i=0; i<MEM_MAX_CHANNEL;i++)
	{
		type = GetChnlTypeIndex(i);
		switch(type)
		{
		case 2:	//IS_LONG:
			if (m_bLong)
			{
				TurnOffChnlTcg(i);
			}
			break;
		case 3: //IS_TRAN:
			if (m_bTran) 
			{
				TurnOffChnlTcg(i);
			}
			break;
		case 4:	//IS_OBQ1:
			if 	(m_bO1 )
			{
				TurnOffChnlTcg(i);
			}
			break;
		case 5: //IS_OBQ2:
			if (m_bO2 )
			{
				TurnOffChnlTcg(i);
			}
			break;
		case 6: //IS_OBQ3:
			if (m_bO3 )
			{
				TurnOffChnlTcg(i);
			}
			break;
		case 7: //IS_LAM:
			if (m_bLam )
			{
				TurnOffChnlTcg(i);
			}
			break;
		default:
			break;
		}
	}

}


void CAcal::TurnOffChnlTcg(int nChannel)
{
	m_pTcpThreadRxList->m_fOdTcgGain[nChannel] = m_pTcpThreadRxList->GetRcvrGain(nChannel);

	if ( m_pTcpThreadRxList->GetTcgTrigSel(nChannel) != 0)
	{
		ConfigRec.receiver.tcg_trigger[nChannel] = 0;
		m_pTcpThreadRxList->WriteTcgStepTrigSel(nChannel);
	}
}

void CAcal::OnData() 
{
	// TODO: Add your control notification handler code here
	caldata dlg;
	dlg.DoModal();
	
}

void CAcal::OnPrint() 
{
	// TODO: Add your control notification handler code here
	pCTscanDlg->OnFilePrintscreen();
	
}

void CAcal::OnChkChnlrange() 
{
	// TODO: Add your control notification handler code here
	m_bChnlRange ^= 1;
}


BOOL CAcal::GetChannelRange() 
{
	// TODO: Add your control notification handler code here
	ResetChannelRange();

	CString sChnlRange;
	m_editChnlRange.GetWindowText(sChnlRange);
	CQStringParser sParser(sChnlRange, ',','\"');
	int i, j;

	int nCount = sParser.GetCount();
	if (nCount > 0)
	{
		CString str = "";
		char ch[128];
		CQStringParser sTemp(sChnlRange, ',','\"');
		int nCnt;
		int nStart;
		int nStop;

		for (i=1; i<=nCount; i++)
		{
			//str += (sParser.GetField(i) + CString("\n"));
			str = sParser.GetField(i);
			if (str.Find('-') >= 0)
			{
				//AfxMessageBox(str);
				sTemp.ResetOriginalString(str,'-','\"');
				//AfxMessageBox(sTemp.GetField(1));
				nCnt = sTemp.GetCount();
				if (nCnt != 2)
				{
					ResetChannelRange();
					return FALSE;
				}
				else
				{
					// range start
					str = sTemp.GetField(1);
					if (!IsStringNumber(str))
					{
						ResetChannelRange();
						return FALSE;
					}
					else
					{
						CstringToChar(str,ch);
						sscanf(ch, "%d", &nStart);
						if ( (nStart < 1) || (nStart > 100) )
						{
							ResetChannelRange();
							return FALSE;
						}
					}
					// range stop
					str = sTemp.GetField(2);
					if (!IsStringNumber(str))
					{
						ResetChannelRange();
						return FALSE;
					}
					else
					{
						CstringToChar(str,ch);
						sscanf(ch, "%d", &nStop);
						if ( (nStop < 1) || (nStop > 100) )
						{
							ResetChannelRange();
							return FALSE;
						}
					}
					
					if (nStop < nStart)
					{
						ResetChannelRange();
						return FALSE;
					}

					for (j=nStart; j<=nStop; j++)
						m_bChnlSelect[j-1] = TRUE;
				}
			}
			else
			{
				if (!IsStringNumber(str))
				{
					ResetChannelRange();
					return FALSE;
				}
				else
				{
					CstringToChar(str,ch);
					sscanf(ch, "%d", &nStart);
					if ( (nStart < 1) || (nStart > 100) )
					{
						ResetChannelRange();
						return FALSE;
					}
					else
					{
						m_bChnlSelect[nStart-1] = TRUE;
					}
				}
			}
		}
	}
	else
	{
		ResetChannelRange();
		return FALSE;
	}

#if 0  //debug
	CString s = "";
	CString sTmp;

	for (i=0; i<MEM_MAX_CHANNEL; i++)
	{
		if (m_bChnlSelect[i])
		{
			sTmp.Format(_T("%d, "), i+1);
			s += sTmp;
		}
	}

	AfxMessageBox(s);
#endif

	return TRUE;
}


void CAcal::ResetChannelRange()
{
	for (int i=0; i<MEM_MAX_CHANNEL; i++)
		m_bChnlSelect[i] = FALSE;
}


BOOL CAcal::IsStringNumber(CString str)
{
	int nLength = str.GetLength();
	TCHAR c;

	if (nLength < 1)
		return FALSE;

	for (int i=0; i<nLength; i++)
	{
		c = str.GetAt(i);

		if (i==0)
			if (c == _T('0'))
				return FALSE;

		switch(c)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			break;

		default:
			return FALSE;
			break;
		}
	}

	return TRUE;
}


void CAcal::Save_Pos()
	{
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition(_T("CALIB"), &wp);
//	bCtof = FALSE;

	}

BOOL CAcal::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
/*
	bCMD = TRUE;
	DoS1RadioButton(nID);
	bCMD = FALSE;
*/	
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
void CAcal::DoS1RadioButton1(UINT nID)
{
	bCMD = FALSE;
	DoS1RadioButton(nID);
}


void CAcal::OnCheckCalOnId() 
{
	// TODO: Add your control notification handler code here
	m_bAutoCalId ^= 1;
}

void CAcal::OnCheckCalOnOd() 
{
	// TODO: Add your control notification handler code here
	m_bAutoCalOd ^= 1;
}