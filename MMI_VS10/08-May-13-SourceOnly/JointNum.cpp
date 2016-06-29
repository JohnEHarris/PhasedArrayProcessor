// JointNum.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
// include "JointNum.h"
#include "TscanDlg.h"

#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJointNum dialog


CJointNum::CJointNum(CWnd* pParent /*=NULL*/)
	: CDialog(CJointNum::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJointNum)
	m_uNewJntNum = 0;
	//}}AFX_DATA_INIT
}


void CJointNum::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJointNum)
	DDX_Text(pDX, IDC_NEW_JNT_NUM, m_uNewJntNum);
	DDV_MinMaxUInt(pDX, m_uNewJntNum, 1, 4000000000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJointNum, CDialog)
	//{{AFX_MSG_MAP(CJointNum)
	ON_EN_CHANGE(IDC_NEW_JNT_NUM, OnChangeNewJntNum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJointNum message handlers

BOOL CJointNum::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// Display existing number.  If return is ok, change 
	// global joint number to value entered
	pCTscanDlg->m_uNewJntNum = m_uNewJntNum = pCTscanDlg->m_uJntNum;
	UpdateData(FALSE);	// Copy variables to screen
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CJointNum::OnChangeNewJntNum() 
	{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_uNewJntNum = GetDlgItemInt(IDC_NEW_JNT_NUM); // local variable
	pCTscanDlg->m_uNewJntNum = m_uNewJntNum;
	
	}
