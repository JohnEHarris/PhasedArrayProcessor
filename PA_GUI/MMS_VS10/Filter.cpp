// Filter.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"

#include "TscanDlg.h"

//include "Filter.h"	included in asidlg.h

#include "Extern.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilter dialog


CFilter::CFilter(CWnd* pParent /*=NULL*/)
	: CDialog(CFilter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilter)
	m_nIDFilter = 0;
	m_nODFilter = 0;
	m_ckNoise = FALSE;
	m_nNoise = 0;
	m_nHiQFilter = 0;
	//}}AFX_DATA_INIT
}


void CFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilter)
	DDX_Control(pDX, IDC_FILT_HIQ_SB, m_sbHiQ);
	DDX_Control(pDX, IDC_FILT_NOISE_SB, m_sbNoise);
	DDX_Control(pDX, IDC_FILT_OD_SB, m_sbOD);
	DDX_Control(pDX, IDC_FILT_ID_SB, m_sbID);
	DDX_Text(pDX, IDC_FILT_ID_ED, m_nIDFilter);
	DDV_MinMaxInt(pDX, m_nIDFilter, 1, 1000);
	DDX_Text(pDX, IDC_FILT_OD_ED, m_nODFilter);
	DDV_MinMaxInt(pDX, m_nODFilter, 1, 1000);
	DDX_Check(pDX, IDC_NOISE_CK, m_ckNoise);
	DDX_Text(pDX, IDC_FILT_NOISE_ED, m_nNoise);
	DDV_MinMaxInt(pDX, m_nNoise, 0, 10);
	DDX_Text(pDX, IDC_FILT_HIQ_ED, m_nHiQFilter);
	DDV_MinMaxInt(pDX, m_nHiQFilter, 5, 2000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilter, CDialog)
	//{{AFX_MSG_MAP(CFilter)
	ON_BN_CLICKED(ID_AMA_SONO_SEL, OnAmaSonoSel)
	ON_BN_CLICKED(IDC_NOISE_CK, OnNoiseCk)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(ID_SAVE_CONFIG, OnSaveConfig)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilter message handlers

void CFilter::OnCancel() 
	{
	// TODO: Add extra cleanup here
	UpdateConfigRec();	// copy data to config rec structure
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CFilter::OnOK() 
	{
	// TODO: Add extra validation here
	UpdateConfigRec();	// copy data to config rec structure
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

void CFilter::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::PostNcDestroy();
	// Same code for all windows
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}

BOOL CFilter::OnInitDialog() 
	{
	CString s;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// copied initially from tholds.cpp

	m_sbOD.SetScrollRange(1,1000,TRUE);
	m_sbOD.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbID.SetScrollRange(1,1000,TRUE);
	m_sbID.EnableScrollBar(ESB_ENABLE_BOTH);
	
	m_sbNoise.SetScrollRange(1,10,TRUE);
	m_sbNoise.EnableScrollBar(ESB_ENABLE_BOTH);
	
	m_sbHiQ.SetScrollRange(5,2000,TRUE);
	m_sbHiQ.EnableScrollBar(ESB_ENABLE_BOTH);
	
	
	LoadConfigRec();	
	UpdateScrollInfo();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CFilter::OnAmaSonoSel() 
	{
#if 0
	// TODO: Add your control notification handler code here
	CString s;

	UpdateConfigRec();	// copy data to config rec structure
	m_nAmaSonoSel ^= 1;	
	m_nAmaSonoSel &= 1;
	ConfigRec.AmaSonoSelFilter = m_nAmaSonoSel;
	LoadConfigRec();
	UpdateScrollInfo();
	
#endif
	}

void CFilter::OnNoiseCk() 
	{
	// TODO: Add your control notification handler code here
	m_ckNoise ^= 1;
	m_ckNoise &= 1;
	UpdateConfigRec();	// copy data to config rec structure
	
	}

void CFilter::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	// TODO: Add your message handler code here and/or call default
	// copied from acal.cpp

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
		nDelta = -20;
		break;

	case SB_PAGERIGHT:
		nDelta = 20;
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


	switch(nCtlId)
		{
		// Abs controls
	case IDC_FILT_OD_SB:
		m_nODFilter = nResult;
		break;

	case IDC_FILT_ID_SB:
		m_nIDFilter = nResult;
		break;
	
	case IDC_FILT_NOISE_SB:
		m_nNoise = nResult;
		break;

	case IDC_FILT_HIQ_SB:
		m_nHiQFilter = nResult;	// Amalog
		break;

	default:
		break;
		}


	pScrollBar->SetScrollPos(nResult, TRUE);


	UpdateData(FALSE);	// copy date to screen
	UpdateConfigRec();	// copy data to config rec structure
	UpdateScrollInfo();

	// Use one location to send filter message if scroll bar not noise limiter

#if 0
	if (nCtlId != IDC_FILT_NOISE_SB)
		{	// filter setting
		if (!m_nAmaSonoSel)
			{	// Amalog
			SendMsg(SET_AMA_FILTERS);
			}
		else
			{	// Sono
			SendMsg(SET_SONO_FILTERS);
			}
		}
#endif
//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	
	}

void CFilter::KillMe()
	{
	// Public access to OnCancel
	OnCancel();
	}

void CFilter::OnSaveConfig() 
	{
	// TODO: Add your control notification handler code here
	// CODE Copied from tholds
	// Call public fils saveas operation from main asi dlg

//	ConfigRec.MapTholdSel = m_nMapThold;
	if (pCTscanDlg) pCTscanDlg->SaveConfigRec();
	
	}

BOOL CFilter::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Asidlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CFilter::UpdateConfigRec()
	{
#if 0
//	int ib0, ib1, ic0, ic1;

	if (!m_nAmaSonoSel)
		{	// amalog
		ConfigRec.AmaRec.Band[HP].Filter = m_nODFilter;
		ConfigRec.AmaRec.Band[LP].Filter = m_nIDFilter;
		ConfigRec.AmaRec.NoiseLevel = (BYTE) m_nNoise;
		ConfigRec.AmaRec.NoiseEnabled = (BYTE) m_ckNoise;
		ConfigRec.AmaRec.HiQ = m_nHiQFilter;
		}	// amalog

	else
		{	// sono
		ConfigRec.SonoRec.Band[HP].Filter = m_nODFilter;
		ConfigRec.SonoRec.Band[LP].Filter = m_nIDFilter;
		ConfigRec.SonoRec.NoiseLevel = (BYTE) m_nNoise;
		ConfigRec.SonoRec.NoiseEnabled = (BYTE) m_ckNoise;
		ConfigRec.SonoRec.HiQ = m_nHiQFilter;
		}	// sono
#endif

	}



void CFilter::UpdateScrollInfo()
	{
	
	m_sbOD.SetScrollPos(m_nODFilter);
	SetDlgItemInt(IDC_FILT_OD_ED,m_nODFilter,FALSE);
	m_sbID.SetScrollPos(m_nIDFilter);
	SetDlgItemInt(IDC_FILT_ID_ED,m_nIDFilter,FALSE);
	m_sbNoise.SetScrollPos(m_nNoise);
	SetDlgItemInt(IDC_FILT_NOISE_ED,m_nNoise,FALSE);
	m_sbHiQ.SetScrollPos(m_nHiQFilter);
	SetDlgItemInt(IDC_FILT_HIQ_ED,m_nHiQFilter,FALSE);

	}

void CFilter::LoadConfigRec()
	{	// copied from acal.cpp
#if 0
	CString s;

	if (!m_nAmaSonoSel)
		{	// amalog
		m_nODFilter = ConfigRec.AmaRec.Band[HP].Filter;
		m_nIDFilter = ConfigRec.AmaRec.Band[LP].Filter;
		m_nNoise = (int) ConfigRec.AmaRec.NoiseLevel;
		m_ckNoise = (int) ConfigRec.AmaRec.NoiseEnabled;
		m_nHiQFilter = ConfigRec.AmaRec.HiQ;
		s = _T("AMALOG");
		}	// amalog

	else
		{	// sono
		m_nODFilter = ConfigRec.SonoRec.Band[HP].Filter;
		m_nIDFilter = ConfigRec.SonoRec.Band[LP].Filter;
		m_nNoise = (int) ConfigRec.SonoRec.NoiseLevel;
		m_ckNoise = (int) ConfigRec.SonoRec.NoiseEnabled;
		m_nHiQFilter = ConfigRec.SonoRec.HiQ;
		s = _T("SONOSCOPE");
		}	// sono
	GetDlgItem(ID_AMA_SONO_SEL)->SetWindowText(s);
	CheckDlgButton(IDC_NOISE_CK, m_ckNoise);

#endif
	}
