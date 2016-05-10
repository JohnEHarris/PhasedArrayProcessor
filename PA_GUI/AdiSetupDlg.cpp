// AdiSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "AdiSetupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdiSetupDlg dialog


CAdiSetupDlg::CAdiSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdiSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdiSetupDlg)
	//}}AFX_DATA_INIT

	for (int i=0; i<32; i++)
	{
		m_nBtnChecked[i] = 0;
	}
}


void CAdiSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdiSetupDlg)
	DDX_Control(pDX, IDC_CHECK_BIT31, m_btnBit31);
	DDX_Control(pDX, IDC_CHECK_BIT30, m_btnBit30);
	DDX_Control(pDX, IDC_CHECK_BIT29, m_btnBit29);
	DDX_Control(pDX, IDC_CHECK_BIT28, m_btnBit28);
	DDX_Control(pDX, IDC_CHECK_BIT27, m_btnBit27);
	DDX_Control(pDX, IDC_CHECK_BIT26, m_btnBit26);
	DDX_Control(pDX, IDC_CHECK_BIT25, m_btnBit25);
	DDX_Control(pDX, IDC_CHECK_BIT24, m_btnBit24);
	DDX_Control(pDX, IDC_CHECK_BIT23, m_btnBit23);
	DDX_Control(pDX, IDC_CHECK_BIT22, m_btnBit22);
	DDX_Control(pDX, IDC_CHECK_BIT21, m_btnBit21);
	DDX_Control(pDX, IDC_CHECK_BIT20, m_btnBit20);
	DDX_Control(pDX, IDC_CHECK_BIT19, m_btnBit19);
	DDX_Control(pDX, IDC_CHECK_BIT18, m_btnBit18);
	DDX_Control(pDX, IDC_CHECK_BIT17, m_btnBit17);
	DDX_Control(pDX, IDC_CHECK_BIT16, m_btnBit16);
	DDX_Control(pDX, IDC_CHECK_BIT15, m_btnBit15);
	DDX_Control(pDX, IDC_CHECK_BIT14, m_btnBit14);
	DDX_Control(pDX, IDC_CHECK_BIT13, m_btnBit13);
	DDX_Control(pDX, IDC_CHECK_BIT12, m_btnBit12);
	DDX_Control(pDX, IDC_CHECK_BIT11, m_btnBit11);
	DDX_Control(pDX, IDC_CHECK_BIT10, m_btnBit10);
	DDX_Control(pDX, IDC_CHECK_BIT09, m_btnBit9);
	DDX_Control(pDX, IDC_CHECK_BIT08, m_btnBit8);
	DDX_Control(pDX, IDC_CHECK_BIT07, m_btnBit7);
	DDX_Control(pDX, IDC_CHECK_BIT06, m_btnBit6);
	DDX_Control(pDX, IDC_CHECK_BIT05, m_btnBit5);
	DDX_Control(pDX, IDC_CHECK_BIT04, m_btnBit4);
	DDX_Control(pDX, IDC_CHECK_BIT03, m_btnBit3);
	DDX_Control(pDX, IDC_CHECK_BIT02, m_btnBit2);
	DDX_Control(pDX, IDC_CHECK_BIT01, m_btnBit1);
	DDX_Control(pDX, IDC_CHECK_BIT0, m_btnBit0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdiSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CAdiSetupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdiSetupDlg message handlers

void CAdiSetupDlg::OnOK() 
{
	// TODO: Add extra validation here
	GetCheckedBox();

	DWORD dwInputInv = 0x00000000;

	for (int i=0; i<32; i++)
	{
		dwInputInv |= ( (DWORD) m_nBtnChecked[i] << i );
	}

	m_pTcpThreadRxList->SetAdiInputInvert(dwInputInv);

	CDialog::OnOK();
}

void CAdiSetupDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CAdiSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetCheckedBox();

	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdiSetupDlg::SetCheckedBox()
{
	DWORD nInputInv = m_pTcpThreadRxList->GetAdiInputInvert();
	for (int i=0; i<32; i++)
	{
		m_nBtnChecked[i] = (BYTE) ( (nInputInv >> i) & 0x00000001 );
	}

	m_btnBit0.SetCheck(m_nBtnChecked[0]);
	m_btnBit1.SetCheck(m_nBtnChecked[1]);
	m_btnBit2.SetCheck(m_nBtnChecked[2]);
	m_btnBit3.SetCheck(m_nBtnChecked[3]);
	m_btnBit4.SetCheck(m_nBtnChecked[4]);
	m_btnBit5.SetCheck(m_nBtnChecked[5]);
	m_btnBit6.SetCheck(m_nBtnChecked[6]);
	m_btnBit7.SetCheck(m_nBtnChecked[7]);
	m_btnBit8.SetCheck(m_nBtnChecked[8]);
	m_btnBit9.SetCheck(m_nBtnChecked[9]);

	m_btnBit10.SetCheck(m_nBtnChecked[10]);
	m_btnBit11.SetCheck(m_nBtnChecked[11]);
	m_btnBit12.SetCheck(m_nBtnChecked[12]);
	m_btnBit13.SetCheck(m_nBtnChecked[13]);
	m_btnBit14.SetCheck(m_nBtnChecked[14]);
	m_btnBit15.SetCheck(m_nBtnChecked[15]);
	m_btnBit16.SetCheck(m_nBtnChecked[16]);
	m_btnBit17.SetCheck(m_nBtnChecked[17]);
	m_btnBit18.SetCheck(m_nBtnChecked[18]);
	m_btnBit19.SetCheck(m_nBtnChecked[19]);

	m_btnBit20.SetCheck(m_nBtnChecked[20]);
	m_btnBit21.SetCheck(m_nBtnChecked[21]);
	m_btnBit22.SetCheck(m_nBtnChecked[22]);
	m_btnBit23.SetCheck(m_nBtnChecked[23]);
	m_btnBit24.SetCheck(m_nBtnChecked[24]);
	m_btnBit25.SetCheck(m_nBtnChecked[25]);
	m_btnBit26.SetCheck(m_nBtnChecked[26]);
	m_btnBit27.SetCheck(m_nBtnChecked[27]);
	m_btnBit28.SetCheck(m_nBtnChecked[28]);
	m_btnBit29.SetCheck(m_nBtnChecked[29]);

	m_btnBit30.SetCheck(m_nBtnChecked[30]);
	m_btnBit31.SetCheck(m_nBtnChecked[31]);
}

void CAdiSetupDlg::GetCheckedBox()
{
	m_nBtnChecked[0] = m_btnBit0.GetCheck();
	m_nBtnChecked[1] = m_btnBit1.GetCheck();
	m_nBtnChecked[2] = m_btnBit2.GetCheck();
	m_nBtnChecked[3] = m_btnBit3.GetCheck();
	m_nBtnChecked[4] = m_btnBit4.GetCheck();
	m_nBtnChecked[5] = m_btnBit5.GetCheck();
	m_nBtnChecked[6] = m_btnBit6.GetCheck();
	m_nBtnChecked[7] = m_btnBit7.GetCheck();
	m_nBtnChecked[8] = m_btnBit8.GetCheck();
	m_nBtnChecked[9] = m_btnBit9.GetCheck();

	m_nBtnChecked[10] = m_btnBit10.GetCheck();
	m_nBtnChecked[11] = m_btnBit11.GetCheck();
	m_nBtnChecked[12] = m_btnBit12.GetCheck();
	m_nBtnChecked[13] = m_btnBit13.GetCheck();
	m_nBtnChecked[14] = m_btnBit14.GetCheck();
	m_nBtnChecked[15] = m_btnBit15.GetCheck();
	m_nBtnChecked[16] = m_btnBit16.GetCheck();
	m_nBtnChecked[17] = m_btnBit17.GetCheck();
	m_nBtnChecked[18] = m_btnBit18.GetCheck();
	m_nBtnChecked[19] = m_btnBit19.GetCheck();

	m_nBtnChecked[20] = m_btnBit20.GetCheck();
	m_nBtnChecked[21] = m_btnBit21.GetCheck();
	m_nBtnChecked[22] = m_btnBit22.GetCheck();
	m_nBtnChecked[23] = m_btnBit23.GetCheck();
	m_nBtnChecked[24] = m_btnBit24.GetCheck();
	m_nBtnChecked[25] = m_btnBit25.GetCheck();
	m_nBtnChecked[26] = m_btnBit26.GetCheck();
	m_nBtnChecked[27] = m_btnBit27.GetCheck();
	m_nBtnChecked[28] = m_btnBit28.GetCheck();
	m_nBtnChecked[29] = m_btnBit29.GetCheck();

	m_nBtnChecked[30] = m_btnBit30.GetCheck();
	m_nBtnChecked[31] = m_btnBit31.GetCheck();
}