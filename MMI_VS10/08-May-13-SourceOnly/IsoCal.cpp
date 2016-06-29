// IsoCal.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

#include "Extern.h"

// #include "IsoCal.h"	included in asidlg.h


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCriticalSection g_CriticalSection;  /* Critical section for mutual exclusion access of plistUtData */


#define FAKE_DATA		0


/////////////////////////////////////////////////////////////////////////////
// CIsoCal dialog


CIsoCal::CIsoCal(CWnd* pParent /*=NULL*/)
	: CDialog(CIsoCal::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIsoCal)
	//}}AFX_DATA_INIT
}


void CIsoCal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIsoCal)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIsoCal, CDialog)
	//{{AFX_MSG_MAP(CIsoCal)
	ON_BN_CLICKED(IDC_CHORD_MIN, OnChordMin)
	ON_BN_CLICKED(IDC_CHORD_NOM, OnChordNom)
	ON_BN_CLICKED(IDC_THRU_MIN, OnThruMin)
	ON_BN_CLICKED(IDC_THRU_NOM, OnThruNom)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ISO_PKHOLD, OnIsoPkhold)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIsoCal message handlers

void CIsoCal::OnCancel() 
	{
	// TODO: Add extra cleanup here
	// Need this to get to PostNcDestroy which nulls ptr to the routine
	StopTimer();
	
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	
	}

void CIsoCal::OnOK() 
	{
	// TODO: Add extra validation here
	CString t,c;	// text caption

	c = _T("Calibration Error");
	t = _T("You must sample all 4 readings at least once");
	
	StopTimer();
	if ( !(bCmin & bCmax & bTmin & bTmax) )
		{
		MessageBox(t,c);
		StartTimer();
		return;
		}

	//Save a/d readings for +/- 10% and projected FS
#if 0
	ConfigRec.IsoRec.ChordMin = bChordMin;
	ConfigRec.IsoRec.ChordMax = bChordMax;
	ConfigRec.IsoRec.ThruMin = bThruMin;
	ConfigRec.IsoRec.ThruMax = bThruMax;

	ConfigRec.IsoRec.FS_ChordMin = 
		(BYTE)( (int)bChordMin * (int)MAX_ISO_THIN_PCT_DISPLAY /10);
	ConfigRec.IsoRec.FS_ChordMax = 
		(BYTE)( (int)bChordMax * (int)MAX_ISO_THICK_PCT_DISPLAY /10);
	ConfigRec.IsoRec.FS_ThruMin = 
		(BYTE)( (int)bThruMin * (int)MAX_ISO_THIN_PCT_DISPLAY /10);
	ConfigRec.IsoRec.FS_ThruMax = 
		(BYTE)( (int)bThruMax * (int)MAX_ISO_THICK_PCT_DISPLAY /10);

	c = _T("Full Scale Levels for Instrument");
	t.Format("TX=%3d, TN=%3d, CX=%3d, CN=%3d",ConfigRec.IsoRec.FS_ThruMax,
		ConfigRec.IsoRec.FS_ThruMin, ConfigRec.IsoRec.FS_ChordMax,
		ConfigRec.IsoRec.FS_ChordMin);

	MessageBox(t,c);

	SendMsg(SET_ALL_THOLDS);	// for instrument to know paint levels

#endif
	CDialog::OnOK();
	CDialog::DestroyWindow();
	
	}

void CIsoCal::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}

BOOL CIsoCal::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	CString s;
	
	// TODO: Add extra initialization here
	m_uIpxTimer = 0;
	StartTimer();
	SendMsg(SET_CAL_MODE);
	bCmin = bCmax = bTmin = bTmax = 0;	// flags for button press
	m_bPkHold = 0;
	s = _T("PkOff");
#if 0
	bChordMin = ConfigRec.IsoRec.ChordMin;
	bChordMax = ConfigRec.IsoRec.ChordMax;
	bThruMin = ConfigRec.IsoRec.ThruMin;
	bThruMax = ConfigRec.IsoRec.ThruMax;
	ShowMaxReadings();

#endif
	GetDlgItem(IDC_ISO_PKHOLD)->SetWindowText(s);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CIsoCal::StopTimer(void)
	{	// helper function to stop timer with one call
	if (m_uIpxTimer)
		{
		KillTimer(m_uIpxTimer);
		m_uIpxTimer = 0;
		}
	}

void CIsoCal::StartTimer(void)
	{	// helper function to start timer with one call
	if (m_uIpxTimer)	return;	// already running

	m_uIpxTimer = SetTimer(IDT_IPX_TIMER, 150, NULL);
	if (!m_uIpxTimer) MessageBox("Failed to start IPX timer");
	}

void CIsoCal::KillMe()
	{
	// Public access to OnCancel
	OnCancel();
	}

void CIsoCal::OnChordMin() 
	{
	// TODO: Add your control notification handler code here
	bCmin = 1;	// button pressed at least once
	bChordMin = bMinChord;
	ShowMaxReadings();
	}

void CIsoCal::OnChordNom() 
	{
	// TODO: Add your control notification handler code here
	bCmax = 1;
	bChordMax = bNomChord;
	ShowMaxReadings();
	}

void CIsoCal::OnThruMin() 
	{
	// TODO: Add your control notification handler code here
	bTmin = 1;
	bThruMin = bMinThru;
	ShowMaxReadings();
	}

void CIsoCal::OnThruNom() 
	{
	// TODO: Add your control notification handler code here
	bTmax = 1;
	bThruMax = bNomThru;
	ShowMaxReadings();
	}



void CIsoCal::OnTimer(UINT nIDEvent) 
	{
	// TODO: Add your message handler code here and/or call default
	// IPX timer to empty data messages from data msg queue


	I_MSG_CAL	*pImsg02;
	BYTE bNewData;
//	BYTE negChord, negThru;	// complemented values
	CString s;


	m_uTimerTick++;
	// Every 8th timer tick, capture the system time
	if ( (m_uTimerTick & 0x7 ) == 0)
		{
		time(&m_tTimeNow);
		pCTscanDlg->UpdateTimeDate(&m_tTimeNow);
		pCTscanDlg->UpdateRpm();	// also displays joint length
		}


#if FAKE_DATA
	int i;

	pImsg02 = new I_MSG_CAL;
	i = (m_uTimerTick >> 2) & 0xff;	// chnl indx

	memset ( (void *) pImsg02, 0, sizeof(I_MSG_CAL));
	pImsg02->IsoTime.sample[0].bChordMin = i;
	pImsg02->IsoTime.sample[0].bChordMax = (i+5) & 0xff;

	pImsg02->IsoTime.sample[0].bThruMin = i;
	pImsg02->IsoTime.sample[0].bThruMax = (i+5) & 0xff;

	plistUtData.AddTail( (void *)pImsg02);


#endif

	if (!m_bPkHold)
		bMinChord = bNomChord = bMinThru = bNomThru = bNewData = 0;

	while (!plistUtData.IsEmpty() )
		{
		pImsg02 = (I_MSG_CAL *)plistUtData.RemoveHead();
#if 0
		bNewData = 1;
		if (bMinChord < pImsg02->IsoTime.sample[0].bChordMin)
			bMinChord = pImsg02->IsoTime.sample[0].bChordMin;

		if (bNomChord < pImsg02->IsoTime.sample[0].bChordMax)
			bNomChord = pImsg02->IsoTime.sample[0].bChordMax;

		if (bMinThru < pImsg02->IsoTime.sample[0].bThruMin)
			bMinThru = pImsg02->IsoTime.sample[0].bThruMin;

		if (bNomThru < pImsg02->IsoTime.sample[0].bThruMax)
			bNomThru = pImsg02->IsoTime.sample[0].bThruMax;

#endif		
		delete pImsg02;
		}

	if (bNewData)
		{
#if 0
		negThru = bMinThru;
		s.Format("%3.0f", (float)(negThru * MAX_ISO_THIN_PCT_DISPLAY) / 50.f + .5f);
		//s.Format("%3d", negThru);
#endif
		s.Format("%3d", bMinThru);
		GetDlgItem(IDC_THRU_MIN_EN)->SetWindowText(s);

#if 0

		s.Format("%3.0f", (float)(bNomThru * MAX_ISO_THICK_PCT_DISPLAY) / 50.f + .5f);
		//s.Format("%3d", bNomThru);
#endif
		s.Format("%3d", bNomThru);
		GetDlgItem(IDC_THRU_NOM_EN)->SetWindowText(s);
#if 0

		negChord = ~bMinChord;
		s.Format("%3.0f", (float)(negChord * MAX_ISO_THIN_PCT_DISPLAY) / 50.f + .5f);
		//s.Format("%3d", negChord);
#endif
		s.Format("%3d", bMinChord);
		GetDlgItem(IDC_CHORD_MIN_EN)->SetWindowText(s);

#if 0
		s.Format("%3.0f", (float)(bNomChord * MAX_ISO_THICK_PCT_DISPLAY) / 50.f + .5f);
		//s.Format("%3d", bNomChord);
#endif
		s.Format("%3d", bNomChord);
		GetDlgItem(IDC_CHORD_NOM_EN)->SetWindowText(s);
		}
	

	
	CDialog::OnTimer(nIDEvent);
	}

BOOL CIsoCal::SolveForAB(float *a, float *b, BYTE xmax, BYTE xmin, int ymax, int ymin)
	{
	// a = (y1-y2)/(x1-x2)
	// b = y1 - ax1
	int dx, dy;

	dx = int(xmax - xmin);
	if ( dx == 0) return FALSE;
	dy = ymax - ymin;

	*a = ((float) dy) / ( (float) dx);
	*b = (float) ymax - *a * (float) xmax;

	return TRUE;
	}

BOOL CIsoCal::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Asidlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CIsoCal::ShowMaxReadings()
	{
	// Display in static text field the peak reading assumed from
	// config fiel or the peak reading set by the operator

	CString s;

	s.Format("%3d", bThruMax);
	GetDlgItem(IDC_STATIC_TX)->SetWindowText(s);

	s.Format("%3d", bThruMin);
	GetDlgItem(IDC_STATIC_TN)->SetWindowText(s);

	s.Format("%3d", bChordMax);
	GetDlgItem(IDC_STATIC_CX)->SetWindowText(s);

	s.Format("%3d", bChordMin);
	GetDlgItem(IDC_STATIC_CN)->SetWindowText(s);

	}

void CIsoCal::OnIsoPkhold() 
	{
	// TODO: Add your control notification handler code here
	CString s;

	m_bPkHold ^= 1;
	m_bPkHold &= 1;
	if (m_bPkHold)
		{
		s = _T("PkHold");
		}
	else
		{
		s = _T("PkOff");
		}
	GetDlgItem(IDC_ISO_PKHOLD)->SetWindowText(s);

	
	}
