// OdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

#include "Extern.h"
//#include "OdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COdDlg dialog


COdDlg::COdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COdDlg)
	m_szNewOd = _T("");
	//}}AFX_DATA_INIT
}


void COdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COdDlg)
	DDX_Text(pDX, IDC_NEW_OD, m_szNewOd);
	DDV_MaxChars(pDX, m_szNewOd, 8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COdDlg, CDialog)
	//{{AFX_MSG_MAP(COdDlg)
	ON_EN_CHANGE(IDC_NEW_OD, OnChangeNewOd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COdDlg message handlers

void COdDlg::OnChangeNewOd() 
	{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

//	GetDlgItemText(IDC_NEW_OD,m_szNewOd);
	UpdateData(TRUE);	//	Copy screen to variable
	pCTscanDlg->m_szNewOd = m_szNewOd;
	}

BOOL COdDlg::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// Display existing number.  If return is ok, change 
	// global od to value entered
	pCTscanDlg->m_szNewOd = m_szNewOd = pCTscanDlg->m_szStat_Od;
	UpdateData(FALSE);	// Copy variables to screen
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}
