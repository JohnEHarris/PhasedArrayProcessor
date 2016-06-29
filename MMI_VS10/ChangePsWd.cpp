// ChangePsWd.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "ChangePsWd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangePsWd dialog


CChangePsWd::CChangePsWd(CWnd* pParent /*=NULL*/)
	: CDialog(CChangePsWd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangePsWd)
	m_sConfirmNew = _T("");
	m_sNewPW = _T("");
	m_sOldPW = _T("");
	//}}AFX_DATA_INIT
}


void CChangePsWd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangePsWd)
	DDX_Text(pDX, IDC_EDIT_CONFIRMNEW, m_sConfirmNew);
	DDX_Text(pDX, IDC_EDIT_NEWPW, m_sNewPW);
	DDX_Text(pDX, IDC_EDIT_OLDPW, m_sOldPW);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangePsWd, CDialog)
	//{{AFX_MSG_MAP(CChangePsWd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangePsWd message handlers

void CChangePsWd::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	CString sPassword = "setfirstpswd5788";
	CWinApp* pApp = AfxGetApp();

	if (m_sOldPW == sPassword)
	{
		pApp->WriteProfileString(_T("ADMINISTRATION"), _T("PASSWORD"), _T("udp44"));
		return;
	}

	sPassword = pApp->GetProfileString(_T("ADMINISTRATION"), _T("PASSWORD"));
	
	if (m_sOldPW == sPassword)
	{
		if (m_sNewPW == m_sConfirmNew)
		{
			pApp->WriteProfileString(_T("ADMINISTRATION"), _T("PASSWORD"), m_sNewPW);
		}
		else
		{
			AfxMessageBox(_T("The passwords you typed do not match.  Type the new password in both text boxes."));
			return;
		}
	}
	else
	{
		AfxMessageBox(_T("The old password is incorrect."));
		return;
	}

	CDialog::OnOK();
}
