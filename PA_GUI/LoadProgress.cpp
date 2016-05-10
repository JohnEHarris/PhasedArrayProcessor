// LoadProgress.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "LoadProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoadProgress dialog


CLoadProgress::CLoadProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadProgress)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLoadProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadProgress)
	DDX_Control(pDX, IDC_PROGRESS1, m_pcLoadProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadProgress, CDialog)
	//{{AFX_MSG_MAP(CLoadProgress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadProgress message handlers

void CLoadProgress::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();

	DestroyWindow();
}

void CLoadProgress::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();

	DestroyWindow();
}

void CLoadProgress::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();

	delete this;
}

void CLoadProgress::KillMe()
{
	OnCancel();
}
