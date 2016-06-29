// FlawDlg.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "FlawDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlawDlg dialog
extern CString szReport;


CFlawDlg::CFlawDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlawDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFlawDlg)
	//}}AFX_DATA_INIT
}


void CFlawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlawDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFlawDlg, CDialog)
	//{{AFX_MSG_MAP(CFlawDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlawDlg message handlers

BOOL CFlawDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_sFlaw.r	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
