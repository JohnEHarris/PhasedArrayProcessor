// EndAreaToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "EndAreaToolDlg.h"
#include "TscanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int gChannel;

/////////////////////////////////////////////////////////////////////////////
// CEndAreaToolDlg dialog


CEndAreaToolDlg::CEndAreaToolDlg(CWnd* pParent /*=NULL*/, CTcpThreadRxList *pTcpThreadRxList /*=NULL*/)
	: CDialog(CEndAreaToolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEndAreaToolDlg)
	m_fEndDelay = 0.0f;
	//}}AFX_DATA_INIT

	m_pTcpThreadRxList = pTcpThreadRxList;

	memset((void *) m_nEndToolEnabled, 0, 100);
}


void CEndAreaToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEndAreaToolDlg)
	DDX_Control(pDX, IDC_BTN_ENDENABLE, m_btnEndTool);
	DDX_Text(pDX, IDC_EDIT_ENDDELAY, m_fEndDelay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEndAreaToolDlg, CDialog)
	//{{AFX_MSG_MAP(CEndAreaToolDlg)
	ON_BN_CLICKED(IDC_BTN_ENDENABLE, OnBtnEndAreaToolEnable)
	ON_BN_CLICKED(IDC_BTN_ENDOFPIPE, OnBtnEndofPipe)
	ON_EN_CHANGE(IDC_EDIT_ENDDELAY, OnChangeEditEnddelay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEndAreaToolDlg message handlers
BOOL CEndAreaToolDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_nEndToolEnabled[gChannel])
		m_btnEndTool.SetWindowText (_T("End Area Tool Enabled"));
	else
		m_btnEndTool.SetWindowText (_T("End Area Tool Disabled"));

	return TRUE;
}

void CEndAreaToolDlg::OnBtnEndAreaToolEnable() 
{
	// TODO: Add your control notification handler code here
	if (m_nEndToolEnabled[gChannel])
	{
		m_nEndToolEnabled[gChannel] = 0;
		m_btnEndTool.SetWindowText (_T("End Area Tool Disabled"));
	}
	else
	{
		m_nEndToolEnabled[gChannel] = 1;
		m_btnEndTool.SetWindowText (_T("End Area Tool Enabled"));
	}

	m_pTcpThreadRxList->SendSlaveMsg (0x33, (WORD) m_nEndToolEnabled[gChannel], 0,0,0,0,0);
}

void CEndAreaToolDlg::OnBtnEndofPipe() 
{
	// TODO: Add your control notification handler code here
	m_pTcpThreadRxList->SendSlaveMsg (0x34, 0, 0,0,0,0,0);
}

void CEndAreaToolDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();

	((CTscanDlg*) GetParent())->m_pEndAreaDlg = NULL;
	DestroyWindow();

}

void CEndAreaToolDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();

	delete this;
}

void CEndAreaToolDlg::UpdateDlg()
{
	if (m_nEndToolEnabled[gChannel])
		m_btnEndTool.SetWindowText (_T("End Area Tool Enabled"));
	else
		m_btnEndTool.SetWindowText (_T("End Area Tool Disabled"));
}

void CEndAreaToolDlg::OnChangeEditEnddelay() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData (TRUE);

	WORD nEndDelay;

	nEndDelay = (WORD) (m_fEndDelay * 47.0f + 6.0f);

	m_pTcpThreadRxList->SendSlaveMsg (0x37, nEndDelay, 0,0,0,0,0);
}
