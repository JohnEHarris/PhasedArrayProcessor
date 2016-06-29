// ChnlOn.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"
// include "ChnlOn.h"	02/27/00


#include "Extern.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChnlOn dialog


CChnlOn::CChnlOn(CWnd* pParent /*=NULL*/)
	: CDialog(CChnlOn::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChnlOn)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CChnlOn::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChnlOn)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CChnlOn, CDialog)
	//{{AFX_MSG_MAP(CChnlOn)
	ON_BN_CLICKED(IDC_BT1_ALLOFF, OnBt1Alloff)
	ON_BN_CLICKED(IDC_BT1_ALLON, OnBt1Allon)
	ON_BN_CLICKED(IDC_BT2_ALLOFF, OnBt2Alloff)
	ON_BN_CLICKED(IDC_BT2_ALLON, OnBt2Allon)
	ON_BN_CLICKED(IDC_CHNL_SAVE, OnChnlSave)
	//}}AFX_MSG_MAP

	// manually added by jeh 02-02-00
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CK1_CH01, IDC_CK1_CH16, DoS1CkBox)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CK2_CH01, IDC_CK2_CH16, DoS2CkBox)

	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChnlOn message handlers

void CChnlOn::OnOK() 
	{
	// TODO: Add extra validation here
	CDialog::OnOK();
	CDialog::DestroyWindow();
	if (CAcal::m_pDlg)
		{
		::PostMessage(CAcal::m_pDlg->m_hWnd, 
						WM_USER_UPDATE_ACAL_SCROLLBARS,0,0);
		}
	}

void CChnlOn::OnCancel() 
	{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	if (CAcal::m_pDlg)
		{
		::PostMessage(CAcal::m_pDlg->m_hWnd, 
						WM_USER_UPDATE_ACAL_SCROLLBARS,0,0);
		}
	}

void CChnlOn::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::PostNcDestroy();
	// Same code for all windows
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}

BOOL CChnlOn::OnInitDialog() 
	{
#if 0
	CString s;
	CButton *pButton;
	int i, sh, nID;
	int j;
	WORD wMask;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	switch (m_nAmaSonoSel)
		{
	case 0:
		s = _T("Amalog");
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
		SendMsg(SET_ACHNL_ON);

		break;
	case 1:
		s = _T("Sonoscope");
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
				wMask = 1;	// No 2nd shoe for sono but make rim work anyway
				}

			for ( i = 0; i < 16; i++)
				{
				pButton = (CButton *)GetDlgItem(nID + i);
				j = wMask & ConfigRec.SonoRec.ChnlOn[sh] ;
				pButton->SetCheck(j);
				wMask = wMask << 1;
				}
			}
		SendMsg(SET_SCHNL_ON);
		break;

	default:
		s = _T("Unknown Machine");
		break;
		}
	
	s += _T("  Chnl ON/OFF Control");
	SetWindowText(s);
#endif	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CChnlOn::KillMe()
	{
	// Public access to OnCancel
	OnCancel();
	}

BOOL CChnlOn::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Asidlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CChnlOn::DoS1CkBox(UINT nID)
	{	// jeh 02/27/00 one command for all shoe 1 chk boxes
#if 0
	int i,ick;
	WORD wMask = 1;
	CString s;
	CButton *pButton;

	i = nID - IDC_CK1_CH01;
	ASSERT( i >= 0 && i <= 16);
	pButton = (CButton *)GetDlgItem(nID);
	ick =  pButton->GetCheck();
	wMask = wMask << i;	// set appropriate bit in mask
	if ( ick)
		{	// button is now checked
		if ( !m_nAmaSonoSel) ConfigRec.AmaRec.ChnlOn[0] |= wMask;	// set bit for chnl i
		else ConfigRec.SonoRec.ChnlOn[0] |= wMask;
		}
	else
		{	// just cleared or turned off chnl
		if ( !m_nAmaSonoSel) ConfigRec.AmaRec.ChnlOn[0] &= ~wMask;	// clear bit for chnl i
		else ConfigRec.SonoRec.ChnlOn[0] &= ~wMask;
		}

	if ( !m_nAmaSonoSel) SendMsg(SET_ACHNL_ON);
	else SendMsg(SET_SCHNL_ON);


#if 0
	s.Format(_T("Chk box = %02d", i+1);
	MessageBox(s);
	m_nChnlSel = i;	// new chnl selected
	m_nS1ChGain = GetChnlGain(m_nS1Band, 0, m_nChnlSel);// new chnl gain
	UpdateScrollInfo();	// show on scroll bars and edit boxes
	s.Format(_T("CH=%02d", i+1);
	GetDlgItem(IDC_STATIC_S1_CH)->SetWindowText(s);
#endif
#endif
	}

void CChnlOn::DoS2CkBox(UINT nID)
	{
#if 0
	int i,ick;
	WORD wMask = 1;
	CString s;
	CButton *pButton;

	i = nID - IDC_CK2_CH01;
	ASSERT( i >= 0 && i <= 16);
	pButton = (CButton *)GetDlgItem(nID);
	ick =  pButton->GetCheck();
	wMask = wMask << i;	// set appropriate bit in mask
	if ( ick)
		{	// button is now checked
		if ( !m_nAmaSonoSel) ConfigRec.AmaRec.ChnlOn[1] |= wMask;	// set bit for chnl i
		else ConfigRec.SonoRec.ChnlOn[1] |= wMask;
		}
	else
		{	// just cleared or turned off chnl
		if ( !m_nAmaSonoSel) ConfigRec.AmaRec.ChnlOn[1] &= ~wMask;	// clear bit for chnl i
		else ConfigRec.SonoRec.ChnlOn[1] &= ~wMask;
		}

	if ( !m_nAmaSonoSel) SendMsg(SET_ACHNL_ON);
	else SendMsg(SET_SCHNL_ON);
#endif
	}


#if 0
// sono needs both shoes on 5/3/2000
void CChnlOn::DoS2CkBox(UINT nID)
	{	// jeh 02/27/00 one command for all shoe 2 chk boxes
	int i,ick;
	WORD wMask = 1;
//	CString s;
	CButton *pButton;


	i = nID - IDC_CK2_CH01;
	ASSERT( i >= 0 && i <= 16);
	pButton = (CButton *)GetDlgItem(nID);
	if ( m_nAmaSonoSel)
		{// Sonoscope
		pButton->SetCheck(0);	// no 2nd shoe on this sonoscope
		}
	else 
		{
		ick =  pButton->GetCheck();
		wMask = wMask << i;	// set appropriate bit in mask
		if ( ick)
			{	// button is now checked
			ConfigRec.AmaRec.ChnlOn[1] |= wMask;	// set bit for chnl i
			}
		else
			{	// just cleared or turned off chnl
			ConfigRec.AmaRec.ChnlOn[1] &= ~wMask;	// clear bit for chnl i
			}

		SendMsg(SET_ACHNL_ON);
		}
	}

#endif


void CChnlOn::DoAllChnls(int shoe, int value)
	{	// Do the same operation to all channels in a shoe
		// value = 1 -> turn on else turn off
#if 0
	WORD wMask;
	int i, nID;
	CButton *pButton;

	shoe &= 1;
	value &= 1;

	if (value) wMask = 0xffff;	// all 1's
	else wMask = 0;

#if 0
	if (( m_nAmaSonoSel) & (shoe) )
		{
		wMask = 0;	// 2nd shoe sono
		value = 0;
		}
#endif

	if ( !m_nAmaSonoSel)
		{
		ConfigRec.AmaRec.ChnlOn[shoe] = wMask;
		SendMsg(SET_ACHNL_ON);
		}
	else 
		{
		ConfigRec.SonoRec.ChnlOn[shoe] = wMask;
		SendMsg(SET_SCHNL_ON);
		}

	if ( shoe) nID = IDC_CK2_CH01;	// 2nd shoe
	else nID = IDC_CK1_CH01;

	for ( i = 0; i < 16; i++)
		{
		pButton = (CButton *)GetDlgItem(nID + i);
		pButton->SetCheck(value);
		}
#endif
	}

void CChnlOn::OnBt1Alloff() 
	{
	// TODO: Add your control notification handler code here
	DoAllChnls(0,0);	// shoe 1, off
	}

void CChnlOn::OnBt1Allon() 
	{
	// TODO: Add your control notification handler code here
	DoAllChnls(0,1);	// shoe 1, on
	}

void CChnlOn::OnBt2Alloff() 
	{
	// TODO: Add your control notification handler code here
	DoAllChnls(1,0);	// shoe 2, off
	}

void CChnlOn::OnBt2Allon() 
	{
	// TODO: Add your control notification handler code here
	DoAllChnls(1,1);	// shoe 2, on
	}

void CChnlOn::OnChnlSave() 
	{
	// TODO: Add your control notification handler code here
	// Save config rec to hard disk
	pCTscanDlg->FileSaveAs();
	}
