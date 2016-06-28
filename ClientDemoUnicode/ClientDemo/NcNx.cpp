// NcNx.cpp : implementation file
//

#include "stdafx.h"
#include "ClientDemo.h"
#include "ClientDemodlg.h"
//#include "NcNx.h"
#include "afxdialogex.h"


// CNcNx dialog

IMPLEMENT_DYNAMIC(CNcNx, CDialogEx)

CNcNx::CNcNx(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNcNx::IDD, pParent)
	{

	}

CNcNx::~CNcNx()
	{
	gDlg.pNcNx = NULL;
	}

void CNcNx::DoDataExchange(CDataExchange* pDX)
{
CDialogEx::DoDataExchange(pDX);
DDX_Control(pDX, IDC_SPCHNL_TYPES, m_spChTypes);
DDX_Control(pDX, IDC_SPCHNL_REPEAT, m_spChRepeat);
DDX_Control(pDX, IDC_CB_PAM, m_cbPam);
DDX_Control(pDX, IDC_CB_INST, m_cbInst);
DDX_Control(pDX, IDC_CB_NC_ID, m_cbNcID);
DDX_Control(pDX, IDC_CB_THLD_ID, m_cbThldID);
DDX_Control(pDX, IDC_CB_M_ID, m_cbMID);
DDX_Control(pDX, IDC_CB_NC_OD, m_cbNcOD);
DDX_Control(pDX, IDC_CB_THLD_OD, m_cbThldOD);
DDX_Control(pDX, IDC_CB_M_OD, m_cbMOD);
DDX_Control(pDX, IDC_CB_NX, m_cbNx);
DDX_Control(pDX, IDC_CB_MAX_WALL, m_cbMaxWall);
DDX_Control(pDX, IDC_CB_MIN_WALL, m_cbMinWall);
DDX_Control(pDX, IDC_CB_DROP_CNT, m_cbDropCnt);
DDX_Control(pDX, IDC_LB_NC_NX, m_lbOutput);
	}


BEGIN_MESSAGE_MAP(CNcNx, CDialogEx)
	ON_BN_CLICKED(IDC_BN_SEND, &CNcNx::OnBnClickedBnSend)
END_MESSAGE_MAP()


// CNcNx message handlers


BOOL CNcNx::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	}


void CNcNx::OnOK()
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnOK();
	this->DestroyWindow();
	}


void CNcNx::OnCancel()
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnCancel();
	}


// READ the list box contents for NcNx values, format into a message and send to PAM
void CNcNx::OnBnClickedBnSend()
	{
	// TODO: Add your control notification handler code here
	}
