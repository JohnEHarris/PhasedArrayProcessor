// Example.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
#include "Example.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Example dialog


Example::Example(CWnd* pParent /*=NULL*/)
	: CDialog(Example::IDD, pParent)
{
	//{{AFX_DATA_INIT(Example)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void Example::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Example)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Example, CDialog)
	//{{AFX_MSG_MAP(Example)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Example message handlers

BOOL Example::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// use same name for timer in all modules
	m_uIpxTimer = SetTimer(IDT_IPX_TIMER, 100, NULL);
	if (!m_uIpxTimer) MessageBox("Failed to start IPX timer");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Example::OnCancel() 
{
	// TODO: Add extra cleanup here
	// Same code for all windows
	// Need this to get to PostNcDestroy which nulls ptr to the routine
	if ( m_uIpxTimer ) KillTimer(m_uIpxTimer);
	
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	
}

void Example::OnOK() 
{
	// TODO: Add extra validation here
	// Same code for all windows
	if ( m_uIpxTimer ) KillTimer(m_uIpxTimer);
	
	CDialog::OnOK();
	CDialog::DestroyWindow();
	
}

void Example::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Same code for all windows
	// Let the top level dialog AsiDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
}

void Example::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnTimer(nIDEvent);
}

void Example::KillMe() 
	{
	// Public access to OnCancel
	// Add to every modeless dialog
	OnCancel();
	}
