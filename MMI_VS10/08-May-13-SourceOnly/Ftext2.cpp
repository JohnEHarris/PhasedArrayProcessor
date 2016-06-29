// Ftext2.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"
//#include "Ftext2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CString szFlaw;
	UINT m_uIpxTimer2;

/////////////////////////////////////////////////////////////////////////////
// Ftext2 dialog
extern BOOL FtextON2 ;
extern CTscanDlg *pCTscanDlg;


Ftext2::Ftext2(CWnd* pParent /*=NULL*/, int nID /* 1 */)
	: CDialog(Ftext2::IDD, pParent)
{
	//{{AFX_DATA_INIT(Ftext2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nID = nID;
}


void Ftext2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Ftext2)
	DDX_Control(pDX, IDC_Ltext, m_LBtext);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Ftext2, CDialog)
	//{{AFX_MSG_MAP(Ftext2)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Ftext2 message handlers

BOOL Ftext2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	GetWindowPlacement(&wp);
	dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		pCTscanDlg->GetWindowLastPosition(_T("FLAW_REPORT_DLG 2"), &rect);
	if ( ( (rect.right - rect.left) >= dx ) &&
			( (rect.bottom - rect.top) >= dy ))
	{
		wp.rcNormalPosition = rect;
		SetWindowPlacement(&wp);
	}

	// TODO: Add extra initialization here
	m_LBtext.ResetContent();
	m_uIpxTimer2 = 0;
	m_uIpxTimer2 = SetTimer(1001, 1000, NULL);
//	m_LBtext.AddString(szFlaw);
//	m_pDlg->ShowWindow(SW_SHOW);

	SetWindowPos (&wndTopMost, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

		SetWindowText(_T("Flaw Report 2"));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Ftext2::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnTimer(nIDEvent);
}

void Ftext2::OnOK() 
{
	// TODO: Add extra validation here
	SavePosition();

	if (m_uIpxTimer2)
		{
		KillTimer(m_uIpxTimer2);
		m_uIpxTimer2 = 0;
		}
	//m_pDlg = NULL;
	FtextON2 = FALSE;	
	CDialog::OnOK();

	DestroyWindow();
}

void Ftext2::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
		CDialog::OnDestroy();
	//m_pDlg = NULL;
	delete this;

}
void Ftext2::SavePosition()
{
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
		pCTscanDlg->SaveWindowLastPosition(_T("FLAW_REPORT_DLG 2"), &wp);
}
void Ftext2::KillMe()
{
	OnOK();
}
