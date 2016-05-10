// GainDlg.cpp : implementation file
//
/*
02-29-00	Copied largely from acal.cpp.  Provides for proportional
		gain control only.  Channels are not individually controlled
		either.  For absolute or channel gain control, the user must
		go to the calibrate screen.


*/
#include "stdafx.h"
#include "Truscan.h"
// include "GainDlg.h"	.. jeh 02/29/00
#include "TscanDlg.h"


#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// Od is on top or first in displays in this dialog.  therefore,
// od = 0, id = 1.  
#define	ACAL_OD			0
#define	ACAL_ID			1

/////////////////////////////////////////////////////////////////////////////
// CGainDlg dialog


CGainDlg::CGainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGainDlg)
	m_nIdGain = 0;
	m_nOdGain = 0;
	m_nS1Gain = 0;
	m_nS2Gain = 0;
	m_nS1Band = -1;
	m_nS2Band = -1;
	m_pUndo = new CONFIG_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
	//}}AFX_DATA_INIT
}


void CGainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGainDlg)
	DDX_Control(pDX, IDC_S2_SB, m_sbS2Gain);
	DDX_Control(pDX, IDC_S1_SB, m_sbS1Gain);
	DDX_Control(pDX, IDC_OD_SB, m_sbOdBandGain);
	DDX_Control(pDX, IDC_ID_SB, m_sbIdBandGain);
	DDX_Text(pDX, IDC_EDIT_ID, m_nIdGain);
	DDV_MinMaxInt(pDX, m_nIdGain, 1, 255);
	DDX_Text(pDX, IDC_EDIT_OD, m_nOdGain);
	DDV_MinMaxInt(pDX, m_nOdGain, 1, 255);
	DDX_Text(pDX, IDC_EDIT_S1, m_nS1Gain);
	DDV_MinMaxInt(pDX, m_nS1Gain, 1, 255);
	DDX_Text(pDX, IDC_EDIT_S2, m_nS2Gain);
	DDV_MinMaxInt(pDX, m_nS2Gain, 1, 255);
	DDX_Radio(pDX, IDC_S1_OD, m_nS1Band);
	DDX_Radio(pDX, IDC_S2_OD, m_nS2Band);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGainDlg, CDialog)
	//{{AFX_MSG_MAP(CGainDlg)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_S1_ID, OnS1Id)
	ON_BN_CLICKED(IDC_S1_OD, OnS1Od)
	ON_BN_CLICKED(IDC_S2_ID, OnS2Id)
	ON_BN_CLICKED(IDC_S2_OD, OnS2Od)
	//ON_BN_CLICKED(ID_ACAL_UNDO, OnAcalUndo)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGainDlg message handlers

void CGainDlg::OnCancel() 
	{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CGainDlg::OnOK() 
	{
	// TODO: Add extra validation here
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

void CGainDlg::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Same code for all windows
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}

void CGainDlg::KillMe() 
	{
	// Public access to OnCancel
	OnCancel();
	}

BOOL CGainDlg::OnInitDialog() 
	{
	CString s;
	
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	LoadConfigRec();

	m_nS1Band = m_nS2Band = ACAL_OD;


	// Set scroll bar properties

	m_sbS1Gain.SetScrollRange(1,255,TRUE);
	m_sbS1Gain.SetScrollPos(m_nS1Gain = GetShoeGain(m_nS1Band,0,1) );
	m_sbS1Gain.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbS2Gain.SetScrollRange(1,255,TRUE);
	m_sbS2Gain.SetScrollPos(m_nS2Gain = GetShoeGain(m_nS2Band,1,1) );
	m_sbS2Gain.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbIdBandGain.SetScrollRange(1,255,TRUE);
	m_sbIdBandGain.SetScrollPos(m_nIdGain = GetBandGain(ACAL_ID,1) );
	m_sbIdBandGain.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbOdBandGain.SetScrollRange(1,255,TRUE);
	m_sbOdBandGain.SetScrollPos(m_nOdGain = GetBandGain(ACAL_OD,1) );
	m_sbOdBandGain.EnableScrollBar(ESB_ENABLE_BOTH);

	UpdateScrollInfo();

	switch (m_nAmaSonoSel)
		{
	case 0:
		s = _T("Amalog");
#if 0
		for ( sh = 0; sh < 2; sh++)
			{
			wMask = 1;
			if ( sh == 0) nID = IDC_CK1_CH01;
			else nID = IDC_CK2_CH01;

			for ( i = 0; i < 16; i++)
				{
				pButton = (CButton *)GetDlgItem(nID + i);
				j = wMask & ConfigRec.AmaRec.ChnlOn[sh] ;
				pButton->SetCheck(j);
				wMask = wMask << 1;
				}
			}
#endif

		break;
	case 1:
		s = _T("SonoScope");
#if 0
		for ( sh = 0; sh < 2; sh++)
			{
			if ( sh == 0)
				{
				nID = IDC_CK1_CH01;
				wMask = 1;
				}
			else 
				{
				nID = IDC_CK2_CH01;
				wMask = 0;	// No 2nd shoe for sono
				}

			for ( i = 0; i < 16; i++)
				{
				pButton = (CButton *)GetDlgItem(nID + i);
				j = wMask & ConfigRec.SonoRec.ChnlOn[sh] ;
				pButton->SetCheck(j);
				wMask = wMask << 1;
				}
			}
#endif
		break;
	default:
		s = _T(" ? ");
		break;
		}
	
	s += _T("  Gains");
	SetWindowText(s);
	UpdateData(FALSE);	// Copy variables to screen
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}



void CGainDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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


	switch(nCtlId)
		{
		// Abs controls
	case IDC_S1_SB:
		if (!SetShoeGain(m_nS1Band,0, 1, nResult) ) nResult = nStart;
		m_nS1Gain = nResult;
		// Send gain msg
		break;
	
	
	case IDC_S2_SB:
		if ( !m_nAmaSonoSel)
			{// Amalog.. proportional
			if (!SetShoeGain(m_nS2Band,1, 1, nResult) )
				nResult = nStart;
			}
		else  nResult = nStart;// sono
		m_nS2Gain = nStart;
		break;
	
	case IDC_OD_SB:
		// always proportional gain
		if (!SetBandGain(ACAL_OD, 1, nResult) ) nResult = nStart;
		m_nOdGain = nResult;
		m_nS1Band = m_nS2Band = ACAL_OD;
		break;
	
	case IDC_ID_SB:
		if (!SetBandGain(ACAL_ID, 1, nResult) )  nResult = nStart;
		m_nIdGain = nResult;
		m_nS1Band = m_nS2Band = ACAL_ID;
		break;
	
	default:
		break;
		}

		
	pScrollBar->SetScrollPos(nResult, TRUE);


	UpdateData(FALSE);	// copy date to screen
	UpdateConfigRec();	// copy data to config rec structure
	UpdateScrollInfo();

//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
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

Copied from acal.cpp

*/
/***************************************************************/

int CGainDlg::GetBandGain(int band, int mode)
	{
	// 0 = lp, 1 = hp
	// 0 = abs, 1 = prop
	// return gain value  0-255

	return 0;

	}

BOOL CGainDlg::SetBandGain(int band, int mode, int gain)
	{
#if 0
	// 0 = lp, 1 = hp
	// 0 = abs, 1 = prop
	// 0 if fails to set gain, !0 if gain is set

	if (gSetBandGain(m_nAmaSonoSel, band, mode, gain) )
		{
		if (!m_nAmaSonoSel ) SendMsg(SET_ALL_AGAINS);
		else SendMsg(SET_ALL_SGAINS);
		return TRUE;
		}
	else return FALSE;
#endif
	return TRUE;
	}

int CGainDlg::GetShoeGain(int band, int shoe, int mode)
	{
#if 0
	// 0 = sh1, 1 = sh2
	// 0 = abs, 1 = prop
	// return gain value  0-255

	return gGetShoeGain(m_nAmaSonoSel, band, shoe, mode);
#endif
	return 0;
	}

BOOL CGainDlg::SetShoeGain(int band, int shoe, int mode, int gain)
	{
#if 0
	// 0 = sh1, 1 = sh2
	// 0 = abs, 1 = prop
	// 0 if fails to set gain, !0 if gain is set
	shoe &= 1;

	if ( gSetShoeGain(m_nAmaSonoSel, band, shoe, mode, gain) )
		{
		if (!m_nAmaSonoSel )
			{
			ConfigRec.AmaRec.CurrentShoe = shoe;
			ConfigRec.AmaRec.CurrentShoeBand[shoe] = band;
			SendMsg(SET_ALL_AGAINS);
			}
		else 
			{
			shoe = 0;
			ConfigRec.SonoRec.CurrentShoe = shoe;
			ConfigRec.SonoRec.CurrentShoeBand[shoe] = band;
			SendMsg(SET_ALL_SGAINS);
			}
		return TRUE;
		}
	else return FALSE;
#endif
	return TRUE;


	}


BOOL CGainDlg::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Asidlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}



void CGainDlg::LoadConfigRec()
	{
#if 0
//	m_nAmaSonoSel = ConfigRec.AmaSonoSel & 1;  come from menu invocation

	if (!m_nAmaSonoSel)
		{	// amalog
		m_nS1Band = ConfigRec.AmaRec.CurrentShoeBand[0] & 1;
		m_nS2Band = ConfigRec.AmaRec.CurrentShoeBand[1] & 1;


		m_nIdGain = GetBandGain(ACAL_ID, 1);
		m_nOdGain = GetBandGain(ACAL_OD, 1);


		}	// amalog

	else
		{	// sono
		m_nS1Band = ConfigRec.SonoRec.CurrentShoeBand[0] & 1;
		m_nS2Band = ConfigRec.SonoRec.CurrentShoeBand[1] & 1;


		m_nIdGain = GetBandGain(ACAL_ID, 1);
		m_nOdGain = GetBandGain(ACAL_OD, 1);

		}	// sono
#endif
	}

void CGainDlg::UpdateConfigRec()
	{
#if 0
	if (!m_nAmaSonoSel)
		{	// amalog
		
		ConfigRec.AmaRec.CurrentShoeBand[0] = m_nS1Band;
		ConfigRec.AmaRec.CurrentShoeBand[1] = m_nS2Band;


		}	// amalog

	else
		{	// sono
		
		ConfigRec.SonoRec.CurrentShoeBand[0] = m_nS1Band;
		ConfigRec.SonoRec.CurrentShoeBand[1] = m_nS2Band;

		}	// sono
#endif

	}


void CGainDlg::UpdateScrollInfo()
	{

	m_nOdGain = GetBandGain(ACAL_OD, 1);
	m_sbOdBandGain.SetScrollPos(m_nOdGain);
	SetDlgItemInt(IDC_EDIT_OD, m_nOdGain, FALSE);

	m_nIdGain = GetBandGain(ACAL_ID, 1);
	m_sbIdBandGain.SetScrollPos(m_nIdGain);
	SetDlgItemInt(IDC_EDIT_ID, m_nIdGain, FALSE);

	m_nS1Gain = GetShoeGain(m_nS1Band,0,1);
	m_sbS1Gain.SetScrollPos(m_nS1Gain);
	SetDlgItemInt(IDC_EDIT_S1, m_nS1Gain, FALSE);


	if ( !m_nAmaSonoSel)
		{// Amalog
		m_nS2Gain = GetShoeGain(m_nS2Band,1,1);
		}
	m_sbS2Gain.SetScrollPos(m_nS2Gain);
	SetDlgItemInt(IDC_EDIT_S2, m_nS2Gain, FALSE);

	}

void CGainDlg::OnS1Id() 
	{
	// TODO: Add your control notification handler code here
	m_nS1Band = ACAL_ID;
	m_nS1Gain = GetShoeGain(m_nS1Band, 0, 1);
	UpdateScrollInfo();
	}

void CGainDlg::OnS1Od() 
	{
	// TODO: Add your control notification handler code here
	m_nS1Band = ACAL_OD;
	m_nS1Gain = GetShoeGain(m_nS1Band, 0, 1);
	UpdateScrollInfo();
	}

void CGainDlg::OnS2Id() 
	{
	// TODO: Add your control notification handler code here
	m_nS2Band = ACAL_ID;
	m_nS2Gain = GetShoeGain(m_nS2Band, 1, 1);
	UpdateScrollInfo();
	}

void CGainDlg::OnS2Od() 
	{
	// TODO: Add your control notification handler code here
	m_nS2Band = ACAL_OD;
	m_nS2Gain = GetShoeGain(m_nS2Band, 1, 1);
	UpdateScrollInfo();
	
	}

void CGainDlg::OnAcalUndo() 
	{
#if 0
	// TODO: Add your control notification handler code here
	if (MessageBox(	_T("UnDo all changes since last SAVE operation?"),
			_T("Restore Configuration"),MB_YESNO)== IDYES)
		{
		memcpy ( (void *) &ConfigRec, (void *) m_pUndo, sizeof(CONFIG_REC));
		UpdateScrollInfo();
		if (!m_nAmaSonoSel ) SendMsg(SET_ALL_AGAINS);
		else SendMsg(SET_ALL_SGAINS);
		}
#endif	
	}

void CGainDlg::OnSave() 
	{
	// TODO: Add your control notification handler code here
	// Save config rec to hard disk
	pCTscanDlg->FileSaveAs();
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
	
	}

CGainDlg::~CGainDlg()
	{
	delete m_pUndo;
	}
