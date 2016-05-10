// FReport.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "FReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CString szReport;

/////////////////////////////////////////////////////////////////////////////
// CFReport dialog


CFReport::CFReport(CWnd* pParent /*=NULL*/)
	: CDialog(CFReport::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFReport)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFReport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFReport)
	DDX_Control(pDX, IDC_LFreport, m_sText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFReport, CDialog)
	//{{AFX_MSG_MAP(CFReport)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFReport message handlers

BOOL CFReport::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_sText.ResetContent();
	m_sText.AddString(szReport);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFReport::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnTimer(nIDEvent);
}
