// CompV.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
// include only the parent dialog header asidlg.h for all child dialogs
#include "TscanDlg.h"
#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompV dialog


CCompV::CCompV(CWnd* pParent /*=NULL*/)
	: CDialog(CCompV::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCompV)
	m_fCompV = 0.0f;
	//}}AFX_DATA_INIT
}


void CCompV::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompV)
	DDX_Text(pDX, IDC_EN_COMP, m_fCompV);
	DDV_MinMaxFloat(pDX, m_fCompV, 0.f, 20.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompV, CDialog)
	//{{AFX_MSG_MAP(CCompV)
	ON_EN_CHANGE(IDC_EN_COMP, OnChangeEnComp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompV message handlers

void CCompV::OnChangeEnComp() 
	{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	}

BOOL CCompV::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (ConfigRec.bEnglishMetric & 1) //metric
		m_fCompV = ConfigRec.UtRec.CompVelocity * 25.4f;
	else  //English
		m_fCompV = ConfigRec.UtRec.CompVelocity;

	UpdateData(FALSE);	// copy data to screen
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CCompV::OnOK() 
	{
	// TODO: Add extra validation here

	UpdateData(TRUE);	/* copy screen to data */
	if (ConfigRec.bEnglishMetric & 1) //metric
		ConfigRec.UtRec.CompVelocity = m_fCompV / 25.4f;	// feed value into global variable
	else
		ConfigRec.UtRec.CompVelocity = m_fCompV;

	if ((ConfigRec.UtRec.CompVelocity < 0.05f) || (ConfigRec.UtRec.CompVelocity > 20.0f) )
		{
		MessageBox( _T("Valid velocity is 0.05 to 20"), 
					_T("Invalid Velocity Input"),	MB_OK);
		return;
		}

	CDialog::OnOK();
	}
