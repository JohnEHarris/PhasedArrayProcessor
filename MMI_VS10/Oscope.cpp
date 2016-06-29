// Oscope.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
// #include "Oscope.h"

#include "TscanDlg.h"

#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COscope dialog


COscope::COscope(CWnd* pParent /*=NULL*/)
	: CDialog(COscope::IDD, pParent)
{
	//{{AFX_DATA_INIT(COscope)
	m_nMuxSel1 = -1;
	m_nMuxSel2 = -1;
	//}}AFX_DATA_INIT
}


void COscope::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COscope)
	DDX_Control(pDX, IDC_TRACE2, m_cbTrace2);
	DDX_Control(pDX, IDC_TRACE1, m_cbTrace1);
	DDX_Radio(pDX, IDC_SCOPE_STD1, m_nMuxSel1);
	DDX_Radio(pDX, IDC_SCOPE_STD2, m_nMuxSel2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COscope, CDialog)
	//{{AFX_MSG_MAP(COscope)
	ON_BN_CLICKED(IDC_SCOPE_ABS1, OnScopeAbs1)
	ON_BN_CLICKED(IDC_SCOPE_ABS2, OnScopeAbs2)
	ON_BN_CLICKED(IDC_SCOPE_DIF1, OnScopeDif1)
	ON_BN_CLICKED(IDC_SCOPE_DIF2, OnScopeDif2)
	ON_BN_CLICKED(IDC_SCOPE_STD1, OnScopeStd1)
	ON_BN_CLICKED(IDC_SCOPE_STD2, OnScopeStd2)
	ON_CBN_SELCHANGE(IDC_TRACE1, OnSelchangeTrace1)
	ON_CBN_SELCHANGE(IDC_TRACE2, OnSelchangeTrace2)
	ON_BN_CLICKED(IDC_CHNL_SAVE, OnChnlSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COscope message handlers

void COscope::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void COscope::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void COscope::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
}

BOOL COscope::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// Get previously save scope selections
	m_nMuxSel1 = ConfigRec.OscopeRec.T1Button % 3;
	m_nIndx1 = ConfigRec.OscopeRec.T1Indx;

	m_nMuxSel2 = ConfigRec.OscopeRec.T2Button % 3;
	m_nIndx2 = ConfigRec.OscopeRec.T2Indx;

	switch (m_nMuxSel1)
		{
	case 0:
	default:
		pMux1 = (SCOPE_MUX_PROTO *) &StdScope1;
		break;
	case 1:
		pMux1 = (SCOPE_MUX_PROTO *) &AbsScope1;
		break;
	case 2:
		pMux1 = (SCOPE_MUX_PROTO *) &DifScope1;
		break;
		}

	InitComboBox(0);

	
	switch (m_nMuxSel2)
		{
	case 0:
	default:
		pMux2 = (SCOPE_MUX_PROTO *) &StdScope2;
		break;
	case 1:
		pMux2 = (SCOPE_MUX_PROTO *) &AbsScope2;
		break;
	case 2:
		pMux2 = (SCOPE_MUX_PROTO *) &DifScope2;
		break;
		}

	InitComboBox(1);

	UpdateData(FALSE);	// Copy variables to screen
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void COscope::OnScopeAbs1() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	pMux1 = (SCOPE_MUX_PROTO *) &AbsScope1;
	ConfigRec.OscopeRec.T1Button = m_nMuxSel1;
	InitComboBox(0);
	}

void COscope::OnScopeAbs2() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	pMux2 = (SCOPE_MUX_PROTO *) &AbsScope2;
	ConfigRec.OscopeRec.T2Button = m_nMuxSel2;
	InitComboBox(1);
	}

void COscope::OnScopeDif1() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	pMux1 = (SCOPE_MUX_PROTO *) &DifScope1;
	ConfigRec.OscopeRec.T1Button = m_nMuxSel1;
	InitComboBox(0);
	}

void COscope::OnScopeDif2() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	pMux2 = (SCOPE_MUX_PROTO *) &DifScope2;
	ConfigRec.OscopeRec.T2Button = m_nMuxSel2;
	InitComboBox(1);
	}

void COscope::OnScopeStd1() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	pMux1 = (SCOPE_MUX_PROTO *) &StdScope1;
	ConfigRec.OscopeRec.T1Button = m_nMuxSel1;
	InitComboBox(0);
	}

void COscope::OnScopeStd2() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen TO variables
	pMux2 = (SCOPE_MUX_PROTO *) &StdScope2;
	ConfigRec.OscopeRec.T2Button = m_nMuxSel2;
	InitComboBox(1);
	}

void COscope::InitComboBox(int Box)
	{
	// Empty the combo box of previous contents and load with new 
	// list of selections..

	int i;
	CString s;

	switch (Box)
		{
	case 0:
		// scope 1 combo box
		// delete previous contents
		m_cbTrace1.ResetContent();
		if (!pMux1) break;

		// select new list based on radio button selection
		for ( i = 0; i < pMux1->Entries; i++)
			{
			s = pMux1->se[i].Txt;
			m_cbTrace1.AddString(s);
			}
		if ( m_nIndx1 < pMux1->Entries) m_cbTrace1.SetCurSel(m_nIndx1);
		else	m_cbTrace1.SetCurSel(0);
		break;

	case 1:
		// scope 2 combo box
		m_cbTrace2.ResetContent();
		if (!pMux2) break;

		// select new list based on radio button selection
		for ( i = 0; i < pMux2->Entries; i++)
			{
			s = pMux2->se[i].Txt;
			m_cbTrace2.AddString(s);
			}
		if ( m_nIndx2 < pMux2->Entries) m_cbTrace2.SetCurSel(m_nIndx2);
		else	m_cbTrace2.SetCurSel(0);
		break;
	default:
		break;
		}

	}

void COscope::OnSelchangeTrace1() 
	{
	// TODO: Add your control notification handler code here
	m_nIndx1 = m_cbTrace1.GetCurSel();
	ConfigRec.OscopeRec.T1Indx = m_nIndx1;
	ConfigRec.OscopeRec.T1MuxNo = pMux1->se[m_nIndx1].MuxNo;
	ConfigRec.OscopeRec.T1MuxChnl = pMux1->se[m_nIndx1].MuxChnl;
	// Send Message
	pCTscanDlg->SendMsg(SET_SCOPE_SELECT);
	}

void COscope::OnSelchangeTrace2() 
	{
	// TODO: Add your control notification handler code here
	m_nIndx2 = m_cbTrace2.GetCurSel();
	ConfigRec.OscopeRec.T2Indx = m_nIndx2;
	ConfigRec.OscopeRec.T2MuxNo = pMux2->se[m_nIndx2].MuxNo;
	ConfigRec.OscopeRec.T2MuxChnl = pMux2->se[m_nIndx2].MuxChnl;
	// Send Message
	pCTscanDlg->SendMsg(SET_SCOPE_SELECT);
	}

void COscope::OnChnlSave() 
	{
	// TODO: Add your control notification handler code here
	// Save config rec to hard disk
	pCTscanDlg->FileSave();
	}
