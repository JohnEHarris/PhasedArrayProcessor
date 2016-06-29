// Shear.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
// include only the parent dialog header asidlg.h for all child dialogs
#include "TscanDlg.h"
#include "Extern.h"
// #include "Shear.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShear dialog


CShear::CShear(CWnd* pParent /*=NULL*/)
	: CDialog(CShear::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShear)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CShear::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShear)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShear, CDialog)
	//{{AFX_MSG_MAP(CShear)
	ON_EN_CHANGE(IDC_EN_SHEAR, OnChangeEnShear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShear message handlers

BOOL CShear::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CShear::OnChangeEnShear() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
