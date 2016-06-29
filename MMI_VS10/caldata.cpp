// caldata.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "caldata.h"
#include "Truscan.h"
#include "TscanDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CTscanDlg* pCTscanDlg;
/////////////////////////////////////////////////////////////////////////////
// caldata dialog


caldata::caldata(CWnd* pParent /*=NULL*/)
	: CDialog(caldata::IDD, pParent)
{
	//{{AFX_DATA_INIT(caldata)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void caldata::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(caldata)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(caldata, CDialog)
	//{{AFX_MSG_MAP(caldata)
	ON_BN_CLICKED(IDOK, OnPrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// caldata message handlers

BOOL caldata::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString s;
	for (int i = 0; i < MEM_MAX_CHANNEL; i++)
	{
		s.Format(_T("   %3d          %3d         %3d"),i+1,
			CAcal::m_pDlg->m_pBarII[i][0],
			CAcal::m_pDlg->m_pBarII[i][1]);
		GetDlgItem(IDC_E1 + i)->SetWindowText(s);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void caldata::OnPrint() 
{
	// TODO: Add your control notification handler code here
	pCTscanDlg->OnFilePrintscreen();
	
}
