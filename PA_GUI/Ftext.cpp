// Ftext.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"
//#include "Ftext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CString szFlaw;
	UINT m_uIpxTimer;

/////////////////////////////////////////////////////////////////////////////
// Ftext dialog
extern BOOL FtextON ;
extern CTscanDlg *pCTscanDlg;


Ftext::Ftext(CWnd* pParent /*=NULL*/, int nID /* 0 */)
	: CDialog(Ftext::IDD, pParent)
{
	//{{AFX_DATA_INIT(Ftext)
	//}}AFX_DATA_INIT
//		m_LBtext.ResetContent();

	m_nID = nID;
}


void Ftext::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Ftext)
	DDX_Control(pDX, IDC_Ltext, m_LBtext);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Ftext, CDialog)
	//{{AFX_MSG_MAP(Ftext)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Ftext message handlers

void Ftext::OnOK() 
{
	// TODO: Add extra validation here
	SavePosition();

	if (m_uIpxTimer)
		{
		KillTimer(m_uIpxTimer);
		m_uIpxTimer = 0;
		}
	//m_pDlg = NULL;
	FtextON = FALSE;	
	CDialog::OnOK();

	//((CInspect *) GetParent())->m_pFlawReport = NULL;
	if (m_nID == 0)
	{
		if (pCTscanDlg->m_pInspectDlg1)
			pCTscanDlg->m_pInspectDlg1->m_pFlawReport = NULL;
	}
	else
	{
		if (pCTscanDlg->m_pInspectDlg2)
			pCTscanDlg->m_pInspectDlg2->m_pFlawReport = NULL;
	}

	DestroyWindow();
}

void Ftext::KillMe()
{
	OnOK();
}

BOOL Ftext::OnInitDialog() 
{
	CDialog::OnInitDialog();

	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	GetWindowPlacement(&wp);
	dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	if (m_nID == 0)
		pCTscanDlg->GetWindowLastPosition(_T("FLAW_REPORT_DLG"), &rect);
	else
		pCTscanDlg->GetWindowLastPosition(_T("FLAW_REPORT_DLG 2"), &rect);
	if ( ( (rect.right - rect.left) >= dx ) &&
			( (rect.bottom - rect.top) >= dy ))
	{
		wp.rcNormalPosition = rect;
		SetWindowPlacement(&wp);
	}

	// TODO: Add extra initialization here
	m_LBtext.ResetContent();
	m_uIpxTimer = 0;
	m_uIpxTimer = SetTimer(1001, 1000, NULL);
//  m_LBtext.AddString(szFlaw);
//	m_pDlg->ShowWindow(SW_SHOW);

	SetWindowPos (&wndTopMost, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

	if (m_nID ==0)
		SetWindowText(_T("Flaw Report 1"));
	else
		SetWindowText(_T("Flaw Report 2"));


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Ftext::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
//  m_LBtext.AddString(szFlaw);
//	m_pDlg->ShowWindow(SW_SHOW);
	CDialog::OnTimer(nIDEvent);
}

void Ftext::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	//m_pDlg = NULL;
	delete this;
}

void Ftext::SavePosition()
{
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
	if (m_nID ==0)
		pCTscanDlg->SaveWindowLastPosition(_T("FLAW_REPORT_DLG"), &wp);
	else
		pCTscanDlg->SaveWindowLastPosition(_T("FLAW_REPORT_DLG 2"), &wp);
}