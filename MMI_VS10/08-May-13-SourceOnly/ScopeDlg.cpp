// ScopeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "ScopeDlg.h"
#include "..\include\cfg100.h"
#include "TscanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int gChannel, gGate;

/////////////////////////////////////////////////////////////////////////////
// CScopeDlg dialog


CScopeDlg::CScopeDlg(CWnd* pParent /*=NULL*/, CTcpThreadRxList* pThreadRxList /*NULL*/)
	: CDialog(CScopeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScopeDlg)
	//}}AFX_DATA_INIT

	m_pThreadRxList = pThreadRxList;

	m_bScope1RFon = false;
	m_bScope2AllGatesOn = true;
	m_bScope2Gate1On = true;
	m_bScope2Gate2On = true;
	m_bScope2Gate3On = true;
	m_bScope2Gate4On = true;
	m_nCBscope1Sel = 0;
	m_nCBscope2Sel = 0;
	m_nCBscope3Sel = 0;
	m_bEndAreaToolEnabled = true;
	//WriteToEndAreaToolEnabledRegister();
	m_bScope2Visible = false;

	//m_pScopeTrace2CalDlg = NULL;

	m_nActiveChnl = 0;
	m_nActiveGate = 0;
}


void CScopeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScopeDlg)
	DDX_Control(pDX, IDC_CB_SCOPE3, m_CBscope3);
	DDX_Control(pDX, IDC_BTN_ALLGATES, m_BTNscope2AllGates);
	DDX_Control(pDX, IDC_CB_SCOPE2, m_CBscope2);
	DDX_Control(pDX, IDC_CB_SCOPE1, m_CBscope1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScopeDlg, CDialog)
	//{{AFX_MSG_MAP(CScopeDlg)
	ON_BN_CLICKED(IDC_BTN_ALLGATES, OnBtnAllgates)
	ON_CBN_SELCHANGE(IDC_CB_SCOPE1, OnSelchangeCbScope1)
	ON_CBN_SELCHANGE(IDC_CB_SCOPE2, OnSelchangeCbScope2)
	ON_CBN_SETFOCUS(IDC_CB_SCOPE2, OnSetfocusCbScope2)
	ON_CBN_SELCHANGE(IDC_CB_SCOPE3, OnSelchangeCbScope3)
	ON_CBN_SETFOCUS(IDC_CB_SCOPE3, OnSetfocusCbScope3)
	ON_BN_CLICKED(IDC_BTN_FASTBACK, OnBtnFastback)
	ON_BN_CLICKED(IDC_BTN_FASTFORWARD, OnBtnFastforward)
	ON_BN_CLICKED(IDC_BTN_SLOWBACK, OnBtnSlowback)
	ON_BN_CLICKED(IDC_BTN_SLOWFORWARD, OnBtnSlowforward)
	ON_BN_CLICKED(IDC_BTN_ACTIVEGATE, OnBtnActivegate)
	ON_CBN_SETFOCUS(IDC_CB_SCOPE1, OnSetfocusCbScope1)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_WM_MOVING()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScopeDlg message handlers
BOOL CScopeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// Set Scope 1 "RF" button to on or off, "Normal" button to off or on
	//if (m_bScope1RFon) {
	if (m_pThreadRxList == NULL)
	{
		if (m_bScope1RFon) {
			m_bScope1RFon = true;
		}
		else {
			m_bScope1RFon = false;
		}

		// Set Scope 2 "All Gates" button to on or off, "Active Gate" button to off or on.
		// If "All Gates" button is on, turn on the gates that were chosen before. 
		//if (m_bScope2AllGatesOn) {
		if (m_bScope2AllGatesOn) {
			m_bScope2AllGatesOn = true;
			m_BTNscope2AllGates.SetWindowText(_T("All"));
			m_bScope2Gate1On = true;
			m_bScope2Gate2On = true;
			m_bScope2Gate3On = true;
			m_bScope2Gate4On = true;
		}
		else 
		{
			m_bScope2AllGatesOn = false;
			m_BTNscope2AllGates.SetWindowText(_T("Active"));
			m_bScope2Gate1On = false;
			m_bScope2Gate2On = false;
			m_bScope2Gate3On = false;
			m_bScope2Gate4On = false;
		}

		// Add items to combo box scope1
		//m_CBscope1.ResetContent();
		//for (i=0; i<4; i++) {
		//	m_CBscope1.AddString(scope1_combo_items[i]);
		//}
		m_nCBscope1Sel = 0;
		m_CBscope1.SetCurSel(m_nCBscope1Sel);

		// Add items to combo box scope2
		//m_CBscope2.ResetContent();
		//for (i=0; i<7; i++) {
		//	m_CBscope2.AddString(scope2_combo_items[i]);
		//}
		m_nCBscope2Sel = 0;
		m_CBscope2.SetCurSel(m_nCBscope2Sel);

		m_nCBscope3Sel = 0;
		m_CBscope3.SetCurSel(m_nCBscope3Sel);

		GetDlgItem(IDC_BTN_ACTIVEGATE)->SetWindowText(_T("I"));
		SetDlgItemInt(IDC_EDIT_ACTIVECHNL, 1, FALSE);
	}
	else
	{
		UpdateChnlGate();
	}
	
	SetDefID(-1);

	//RestoreWindowPosition(_T("Scope Control"));
	//SetWindowPosition();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*
void CScopeDlg::OnBtnActivegate() 
{
	// TODO: Add your control notification handler code here
	int nChannel = m_pThreadRxList->m_nActiveChannel;
	int nGate = m_pThreadRxList->m_nActiveGate;

	if (m_pThreadRxList->GetScopeTrace2DisplaySel(nChannel) != 0) {
		m_pThreadRxList->SetScopeTrace2DisplaySel(nChannel, 0);
		m_bScope2AllGatesOn = false;
		m_BTNscope2Gate1.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE1_OFF));
		m_BTNscope2Gate2.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE2_OFF));
		m_BTNscope2Gate3.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE3_OFF));
		m_BTNscope2Gate4.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE4_OFF));
		m_BTNscope2AllGates.SetBitmap(m_hbmpScope2AllGatesOff);
		m_BTNscope2ActiveGate.SetBitmap(m_hbmpScope2ActiveGateOn);
	}
	else {
		m_pThreadRxList->SetScopeTrace2DisplaySel(nChannel, 1);
		m_bScope2AllGatesOn = true;
		m_BTNscope2AllGates.SetBitmap(m_hbmpScope2AllGatesOn);
		m_BTNscope2ActiveGate.SetBitmap(m_hbmpScope2ActiveGateOff);
		if (m_bScope2Gate1On) {
			m_BTNscope2Gate1.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE1_ON));
		}
		if (m_bScope2Gate2On) {
			m_BTNscope2Gate2.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE2_ON));
		}
		if (m_bScope2Gate3On) {
			m_BTNscope2Gate3.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE3_ON));
		}
		if (m_bScope2Gate4On) {
			m_BTNscope2Gate4.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE4_ON));
		}
	}

	if (m_nCBscope2Sel == 0)
	{
		if (m_bScope2AllGatesOn)
			m_pThreadRxList->SetScopeTrace2DiagnosSel(m_pThreadRxList->m_nActiveChannel, 0xE);
		else
			m_pThreadRxList->SetScopeTrace2DiagnosSel(m_pThreadRxList->m_nActiveChannel, 0xF);
	}
}
*/

void CScopeDlg::OnBtnAllgates() 
{
	// TODO: Add your control notification handler code here
	int nChannel = m_pThreadRxList->m_nActiveChannel;
	int nGate = m_pThreadRxList->m_nActiveGate;

	if (m_pThreadRxList->GetScopeTrace2DisplaySel(nChannel) != 0)
	{
		m_pThreadRxList->SetScopeTrace2DisplaySel(nChannel, 0);
		m_bScope2AllGatesOn = false;
		m_BTNscope2AllGates.SetWindowText(_T("Active"));
		m_bScope2Gate1On = false;
		m_bScope2Gate2On = false;
		m_bScope2Gate3On = false;
		m_bScope2Gate4On = false;
	}
	else if (m_nCBscope2Sel == 0)
	{
		m_pThreadRxList->SetScopeTrace2DisplaySel(nChannel, 1);
		m_bScope2AllGatesOn = true;
		m_BTNscope2AllGates.SetWindowText(_T("All"));
		m_bScope2Gate1On = true;
		m_bScope2Gate2On = true;
		m_bScope2Gate3On = true;
		m_bScope2Gate4On = true;
	}

	if (m_nCBscope2Sel == 0)
	{
		if (m_bScope2AllGatesOn)
			m_pThreadRxList->SetScopeTrace2DiagnosSel(m_pThreadRxList->m_nActiveChannel, 0xE);
		else
			m_pThreadRxList->SetScopeTrace2DiagnosSel(m_pThreadRxList->m_nActiveChannel, 0xF);
	}

}

/*
void CScopeDlg::OnBtnGate1() 
{
	// TODO: Add your control notification handler code here
	if (m_bScope2AllGatesOn) {
		if (m_bScope2Gate1On) {
			m_pThreadRxList->SetScopeTrace2GateSel(m_pThreadRxList->m_nActiveChannel,
													0,
													0 );
			m_bScope2Gate1On = false;
			m_BTNscope2Gate1.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE1_OFF));
		}
		else {
			m_pThreadRxList->SetScopeTrace2GateSel(m_pThreadRxList->m_nActiveChannel,
													0,
													1 );
			m_bScope2Gate1On = true;
			m_BTNscope2Gate1.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE1_ON));
		}
	}
}

void CScopeDlg::OnBtnGate2() 
{
	// TODO: Add your control notification handler code here
	if (m_bScope2AllGatesOn) {
		if (m_bScope2Gate2On) {
			m_pThreadRxList->SetScopeTrace2GateSel(m_pThreadRxList->m_nActiveChannel,
													1,
													0 );
			m_bScope2Gate2On = false;
			m_BTNscope2Gate2.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE2_OFF));
		}
		else {
			m_pThreadRxList->SetScopeTrace2GateSel(m_pThreadRxList->m_nActiveChannel,
													1,
													1 );
			m_bScope2Gate2On = true;
			m_BTNscope2Gate2.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE2_ON));
		}
	}
}

void CScopeDlg::OnBtnGate3() 
{
	// TODO: Add your control notification handler code here
	if (m_bScope2AllGatesOn) {
		if (m_bScope2Gate3On) {
			m_pThreadRxList->SetScopeTrace2GateSel(m_pThreadRxList->m_nActiveChannel,
													2,
													0 );
			m_bScope2Gate3On = false;
			m_BTNscope2Gate3.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE3_OFF));
		}
		else {
			m_pThreadRxList->SetScopeTrace2GateSel(m_pThreadRxList->m_nActiveChannel,
													2,
													1 );
			m_bScope2Gate3On = true;
			m_BTNscope2Gate3.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE3_ON));
		}
	}
}

void CScopeDlg::OnBtnGate4() 
{
	// TODO: Add your control notification handler code here
	if (m_bScope2AllGatesOn) {
		if (m_bScope2Gate4On) {
			m_pThreadRxList->SetScopeTrace2GateSel(m_pThreadRxList->m_nActiveChannel,
													3,
													0 );
			m_bScope2Gate4On = false;
			m_BTNscope2Gate4.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE4_OFF));
		}
		else {
			m_pThreadRxList->SetScopeTrace2GateSel(m_pThreadRxList->m_nActiveChannel,
													3,
													1 );
			m_bScope2Gate4On = true;
			m_BTNscope2Gate4.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE2GATE4_ON));
		}
	}
}

void CScopeDlg::OnBtnNormal() 
{
	// TODO: Add your control notification handler code here
	if (m_bScope1RFon) {
		m_pThreadRxList->SetScopeTrace1DisplaySel(m_pThreadRxList->m_nActiveChannel,
																			 0 );
		m_bScope1RFon = false;
		m_BTNscope1RF.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE1RF_OFF));
		m_BTNscope1Normal.SetBitmap(m_hbmpScope1NormalOn);
	}
	else {
		m_pThreadRxList->SetScopeTrace1DisplaySel(m_pThreadRxList->m_nActiveChannel,
																			 1 );
		m_bScope1RFon = true;
		m_BTNscope1RF.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE1RF_ON));
		m_BTNscope1Normal.SetBitmap(m_hbmpScope1NormalOff);
	}
}

void CScopeDlg::OnBtnRf() 
{
	// TODO: Add your control notification handler code here
	if (m_bScope1RFon) {
		m_pThreadRxList->SetScopeTrace1DisplaySel(m_pThreadRxList->m_nActiveChannel,
																			 0 );
		m_bScope1RFon = false;
		m_BTNscope1RF.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE1RF_OFF));
		m_BTNscope1Normal.SetBitmap(m_hbmpScope1NormalOn);
	}
	else {
		m_pThreadRxList->SetScopeTrace1DisplaySel(m_pThreadRxList->m_nActiveChannel,
																			 1 );
		m_bScope1RFon = true;
		m_BTNscope1RF.SetIcon (AfxGetApp ()->LoadIcon (IDI_SCOPE1RF_ON));
		m_BTNscope1Normal.SetBitmap(m_hbmpScope1NormalOff);
	}
}
*/

void CScopeDlg::OnSelchangeCbScope1() 
{
	// TODO: Add your control notification handler code here
	m_nCBscope1Sel = m_CBscope1.GetCurSel();

	if (m_nCBscope1Sel != 1)
	{
		m_pThreadRxList->SetScopeTrace1DisplaySel(m_pThreadRxList->m_nActiveChannel,
																			 0 );
		m_bScope1RFon = false;

		int nValue;
		if (m_nCBscope1Sel == 0)
			nValue = 0;
		else if (m_nCBscope1Sel == 2)
			nValue = 2;
		else if (m_nCBscope1Sel == 3)
			nValue = 3;
		else if (m_nCBscope1Sel == 4)
			nValue = 1;

		m_pThreadRxList->SetScopeTrace1TestSel(m_pThreadRxList->m_nActiveChannel,
											   nValue );
	}
	else  //Rf Test
	{
		m_pThreadRxList->SetScopeTrace1TestSel(m_pThreadRxList->m_nActiveChannel,
											   0 );
		m_pThreadRxList->SetScopeTrace1DisplaySel(m_pThreadRxList->m_nActiveChannel,
																			 1 );
		m_bScope1RFon = true;
	}
}

void CScopeDlg::OnSelchangeCbScope2() 
{
	// TODO: Add your control notification handler code here
	m_nCBscope2Sel = m_CBscope2.GetCurSel();
	if (m_nCBscope2Sel < 8)
	{
		m_pThreadRxList->SetScopeTrace2TestSel(m_pThreadRxList->m_nActiveChannel,
											   m_nCBscope2Sel );
		if (m_nCBscope2Sel != 0)
		{
			if (m_bScope2AllGatesOn)
				OnBtnAllgates();
			m_pThreadRxList->SetScopeTrace2DiagnosSel(m_pThreadRxList->m_nActiveChannel, 0xF);
		}

		if (m_nCBscope2Sel == 0)
		{
			if (m_bScope2AllGatesOn)
				m_pThreadRxList->SetScopeTrace2DiagnosSel(m_pThreadRxList->m_nActiveChannel, 0xE);
			else
				m_pThreadRxList->SetScopeTrace2DiagnosSel(m_pThreadRxList->m_nActiveChannel, 0xF);
		}
	}
	else
	{
		int n = m_nCBscope2Sel - 8;
		m_pThreadRxList->SetScopeTrace2DiagnosSel(m_pThreadRxList->m_nActiveChannel, n);
	}	
}


void CScopeDlg::UpdateChnlGate()
{
	int nChannel = m_pThreadRxList->m_nActiveChannel;
	int nGate = m_pThreadRxList->m_nActiveGate;
	nGate %= MAX_GATES;

	m_nActiveGate = nGate;
	CString s;
	s.Format(_T("%1d"), m_nActiveGate+1);
	GetDlgItem(IDC_BTN_ACTIVEGATE)->SetWindowText(s);

	SetDlgItemInt(IDC_EDIT_ACTIVECHNL, nChannel+1, FALSE);
	m_nActiveChnl = nChannel%MEM_MAX_CHANNEL;

		// Set Scope 1 "RF" button to on or off, "Normal" button to off or on
	//if (m_bScope1RFon) {
	if (m_pThreadRxList->GetScopeTrace1DisplaySel(nChannel) == 1)
	{
		m_bScope1RFon = true;
	}
	else
	{
		m_bScope1RFon = false;
	}

	// Set Scope 2 "All Gates" button to on or off, "Active Gate" button to off or on.
	// If "All Gates" button is on, turn on the gates that were chosen before. 
	//if (m_bScope2AllGatesOn) {
	if (m_pThreadRxList->GetScopeTrace2DisplaySel(nChannel) != 0)
	{
		m_bScope2AllGatesOn = true;
		m_pThreadRxList->SetScopeTrace2DisplaySel(nChannel, 1);
		m_BTNscope2AllGates.SetWindowText(_T("All"));
		m_bScope2Gate1On = true;
		m_pThreadRxList->SetScopeTrace2GateSel(nChannel, 0, 1);
		m_bScope2Gate2On = true;
		m_pThreadRxList->SetScopeTrace2GateSel(nChannel, 1, 1);
		m_bScope2Gate3On = true;
		m_pThreadRxList->SetScopeTrace2GateSel(nChannel, 2, 1);
		m_bScope2Gate4On = true;
		m_pThreadRxList->SetScopeTrace2GateSel(nChannel, 3, 1);
	}
	else {
		m_bScope2AllGatesOn = false;
		m_pThreadRxList->SetScopeTrace2DisplaySel(nChannel, 0);
		m_BTNscope2AllGates.SetWindowText(_T("Active"));
	}

	// Add items to combo box scope1
	//m_CBscope1.ResetContent();
	//for (i=0; i<4; i++) {
	//	m_CBscope1.AddString(scope1_combo_items[i]);
	//}
	if (!m_bScope1RFon)
	{
		m_pThreadRxList->SetScopeTrace1DisplaySel(m_pThreadRxList->m_nActiveChannel,
																		 0 );

		short nValue = m_pThreadRxList->GetScopeTrace1TestSel(nChannel);

		if (nValue == 0)
			m_nCBscope1Sel = 0;
		else if (nValue == 2)
			m_nCBscope1Sel = 2;
		else if (nValue == 3)
			m_nCBscope1Sel = 3;
		else if (nValue == 1)
			m_nCBscope1Sel = 4;

		m_pThreadRxList->SetScopeTrace1TestSel(m_pThreadRxList->m_nActiveChannel,
											   nValue );
	}
	else  //Rf Test
	{
		m_pThreadRxList->SetScopeTrace1DisplaySel(m_pThreadRxList->m_nActiveChannel,
																			 1 );
		m_nCBscope1Sel = 1;
	}
	m_CBscope1.SetCurSel(m_nCBscope1Sel);

	// Add items to combo box scope2
	//m_CBscope2.ResetContent();
	//for (i=0; i<7; i++) {
	//	m_CBscope2.AddString(scope2_combo_items[i]);
	//}
	if (m_nCBscope2Sel<8)
	{
		m_nCBscope2Sel = m_pThreadRxList->GetScopeTrace2TestSel(nChannel);
		m_CBscope2.SetCurSel(m_nCBscope2Sel);
	}
	OnSelchangeCbScope2();

	if (m_pThreadRxList->m_nNumberOfBoards > 1)
	{
		m_nCBscope3Sel = m_pThreadRxList->GetScopeTrigger(nChannel);

		m_CBscope3.SetCurSel(m_nCBscope3Sel);
		OnSelchangeCbScope3();
	}
}

void CScopeDlg::OnSetfocusCbScope2() 
{
	// TODO: Add your control notification handler code here
	CRect rectEdit;
	m_CBscope2.GetWindowRect(rectEdit);

	CMenu menu;
	menu.LoadMenu (IDR_MENU_SCOPE2);
	CMenu* pMenu = menu.GetSubMenu (0);
	int nCmd = pMenu->TrackPopupMenu (TPM_LEFTALIGN |
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
		rectEdit.left, rectEdit.bottom,
		this);

	SetFocus();

	switch (nCmd) {
	case ID_MENU_SCOPE2GATE:
		m_CBscope2.SetCurSel(0);
		break;
	case ID_MENU_SCOPE2IFDET:
		m_CBscope2.SetCurSel(1);
		break;
	case ID_MENU_SCOPE2THDET:
		m_CBscope2.SetCurSel(2);
		break;
	case ID_MENU_SCOPE2PKDET:
		m_CBscope2.SetCurSel(3);
		break;
	case ID_MENU_SCOPE2TFGATE:
		m_CBscope2.SetCurSel(4);
		break;
	case ID_MENU_SCOPE2ALGATE:
		m_CBscope2.SetCurSel(5);
		break;
	case ID_MENU_SCOPE2ALWALL:
		m_CBscope2.SetCurSel(6);
		break;
	case ID_MENU_SCOPE2TCGGATE:
		m_CBscope2.SetCurSel(7);
		break;
	case ID_MENU_SCOPE2MBS:
		m_CBscope2.SetCurSel(8);
		break;
	case ID_MENU_SCOPE2EAS:
		m_CBscope2.SetCurSel(9);
		break;
	case ID_MENU_SCOPE2FOS:
		m_CBscope2.SetCurSel(10);
		break;
	case ID_MENU_SCOPE2FOF:
		m_CBscope2.SetCurSel(11);
		break;
	case ID_MENU_SCOPE2TICKS:
		m_CBscope2.SetCurSel(12);
		break;
	case ID_MENU_SCOPE2TOP:
		m_CBscope2.SetCurSel(13);
		break;
	case ID_MENU_SCOPE2MOTION1:
		m_CBscope2.SetCurSel(14);
		break;
	case ID_MENU_SCOPE2MOTION2:
		m_CBscope2.SetCurSel(15);
		break;
	case ID_MENU_SCOPE2SLAVEAA:
		m_CBscope2.SetCurSel(16);
		break;
	case ID_MENU_SCOPE2SLAVEBB:
		m_CBscope2.SetCurSel(17);
		break;
	case ID_MENU_SCOPE2CALIBRATE:
		ScopeTrace2Calibrate();
		break;
	default:
		break;
	}

	OnSelchangeCbScope2();
}

void CScopeDlg::ScopeTrace1Calibrate()
{
	CTscanDlg* pTscanDlg = (CTscanDlg*) GetParent();

	if (pTscanDlg->m_pScopeTrace1CalDlg != NULL)
		pTscanDlg->m_pScopeTrace1CalDlg->SetFocus();
	else
	{
		pTscanDlg->m_pScopeTrace1CalDlg = new CScopeTrace1CalDlg(pTscanDlg, m_pThreadRxList);
		pTscanDlg->m_pScopeTrace1CalDlg->Create(IDD_SCOPETRACE1CAL, pTscanDlg);

		int cy = ::GetSystemMetrics(SM_CYSCREEN); 
		int cx = ::GetSystemMetrics(SM_CXSCREEN); 
		CRect rectDlg;
		pTscanDlg->m_pScopeTrace1CalDlg->GetWindowRect(&rectDlg);
		int y = cy/2 - rectDlg.Height()/2;     
		int x = cx/2 - rectDlg.Width()/2;
		pTscanDlg->m_pScopeTrace1CalDlg->MoveWindow(x, y+50, rectDlg.Width(), rectDlg.Height(), true);
		pTscanDlg->m_pScopeTrace1CalDlg->ShowWindow(SW_SHOW);
	}
}

void CScopeDlg::ScopeTrace2Calibrate()
{
	CTscanDlg* pTscanDlg = (CTscanDlg*) GetParent();

	if (pTscanDlg->m_pScopeTrace2CalDlg != NULL)
		pTscanDlg->m_pScopeTrace2CalDlg->SetFocus();
	else
	{
		pTscanDlg->m_pScopeTrace2CalDlg = new CScopeTrace2CalDlg(pTscanDlg, m_pThreadRxList);
		pTscanDlg->m_pScopeTrace2CalDlg->Create(IDD_SCOPETRACE2CAL1, pTscanDlg);

		int cy = ::GetSystemMetrics(SM_CYSCREEN); 
		int cx = ::GetSystemMetrics(SM_CXSCREEN); 
		CRect rectDlg;
		pTscanDlg->m_pScopeTrace2CalDlg->GetWindowRect(&rectDlg);
		int y = cy/2 - rectDlg.Height()/2;     
		int x = cx/2 - rectDlg.Width()/2;
		pTscanDlg->m_pScopeTrace2CalDlg->MoveWindow(x, y, rectDlg.Width(), rectDlg.Height(), true);
		pTscanDlg->m_pScopeTrace2CalDlg->ShowWindow(SW_SHOW);
	}
}

void CScopeDlg::OnSelchangeCbScope3() 
{
	// TODO: Add your control notification handler code here
	m_nCBscope3Sel = m_CBscope3.GetCurSel();
	if (m_pThreadRxList->m_nNumberOfBoards > 1)
	{
		int n = m_nCBscope3Sel;
		m_pThreadRxList->SetScopeTrigger(m_pThreadRxList->m_nActiveChannel, n);
	}	
}


void CScopeDlg::OnSetfocusCbScope3() 
{
	// TODO: Add your control notification handler code here
	CRect rectEdit;
	m_CBscope3.GetWindowRect(rectEdit);

	CMenu menu;
	menu.LoadMenu (IDR_MENU_SCOPE3);
	CMenu* pMenu = menu.GetSubMenu (0);
	int nCmd = pMenu->TrackPopupMenu (TPM_LEFTALIGN |
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
		rectEdit.left, rectEdit.bottom,
		this);

	SetFocus();

	switch (nCmd) {
	case ID_MENU_SCOPE3IP:
		m_CBscope3.SetCurSel(0);
		break;
	case ID_MENU_SCOPE3EAS:
		m_CBscope3.SetCurSel(1);
		break;
	case ID_MENU_SCOPE3FOS:
		m_CBscope3.SetCurSel(2);
		break;
	case ID_MENU_SCOPE3FOF:
		m_CBscope3.SetCurSel(3);
		break;
	case ID_MENU_SCOPE3TICKS:
		m_CBscope3.SetCurSel(4);
		break;
	case ID_MENU_SCOPE3TOP:
		m_CBscope3.SetCurSel(5);
		break;
	case ID_MENU_SCOPE3MOTION1:
		m_CBscope3.SetCurSel(6);
		break;
	case ID_MENU_SCOPE3MOTION2:
		m_CBscope3.SetCurSel(7);
		break;
	case ID_MENU_SCOPE3SLAVEA:
		m_CBscope3.SetCurSel(8);
		break;
	case ID_MENU_SCOPE3SLAVEB:
		m_CBscope3.SetCurSel(9);
		break;

	}

	OnSelchangeCbScope3();
}

void CScopeDlg::KillMe()
{
	OnCancel();
}

void CScopeDlg::OnOK() // un-DO button
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();

	//SaveWindowPosition(_T("Scope Control"));
	//((CTscanDlg*) GetParent())->m_pScopeDlg = NULL;
	//DestroyWindow();

	OnCancel();
}

void CScopeDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CTscanDlg* pTscanDlg = (CTscanDlg*) GetParent();

	//CDialog::OnCancel();
	if (pTscanDlg->m_pScopeTrace2CalDlg != NULL)
		return;

	SaveWindowPosition(_T("Scope Control"));
	((CTscanDlg*) GetParent())->m_pScopeDlg = NULL;

	DestroyWindow();
}

void CScopeDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();

	delete this;
}

void CScopeDlg::SaveWindowPosition(LPCTSTR lpszProfileName)
{
    // place your SaveState or GlobalSaveState call in
    // CMainFrame's OnClose() or DestroyWindow(), not in OnDestroy()
    ASSERT_VALID(this);
    ASSERT(GetSafeHwnd());

    CWinApp* pApp = AfxGetApp();

	CRect rect;
	GetWindowRect(&rect);

    pApp->WriteProfileInt(lpszProfileName, _T("Left"), rect.left);
    pApp->WriteProfileInt(lpszProfileName, _T("Top"), rect.top);
}

void CScopeDlg::RestoreWindowPosition(LPCTSTR lpszProfileName)
{
    // place your SaveState or GlobalSaveState call in
    // CMainFrame's OnClose() or DestroyWindow(), not in OnDestroy()
    ASSERT_VALID(this);
    ASSERT(GetSafeHwnd());

    CWinApp* pApp = AfxGetApp();

	CRect rect;
	GetWindowRect(&rect);
	int x = rect.left;
	int y = rect.top;
	int cx, cy;

    cx = pApp->GetProfileInt(lpszProfileName, _T("Left"), x);
    cy = pApp->GetProfileInt(lpszProfileName, _T("Top"), y);

	MoveWindow(cx, cy, rect.Width(), rect.Height());
}


void CScopeDlg::SetWindowPosition(int top)
{
	CRect rectParent;
	GetParent()->GetClientRect(&rectParent);

	CRect rect;
	GetWindowRect(&rect);

	int left = rectParent.right - rect.Width();

	MoveWindow(left, top, rect.Width(), rect.Height());
}


void CScopeDlg::OnBtnFastback() 
{
	// TODO: Add your control notification handler code here
	m_nActiveChnl -= 10;
	if (m_nActiveChnl < 0)
		m_nActiveChnl += MEM_MAX_CHANNEL;
	m_nActiveChnl %= MEM_MAX_CHANNEL;

	SetDlgItemInt(IDC_EDIT_ACTIVECHNL, m_nActiveChnl+1, FALSE);

	ChangeActiveChnl(m_nActiveChnl);
}

void CScopeDlg::OnBtnFastforward() 
{
	// TODO: Add your control notification handler code here
	m_nActiveChnl += 10;
	m_nActiveChnl %= MEM_MAX_CHANNEL;

	SetDlgItemInt(IDC_EDIT_ACTIVECHNL, m_nActiveChnl+1, FALSE);

	ChangeActiveChnl(m_nActiveChnl);
}

void CScopeDlg::OnBtnSlowback() 
{
	// TODO: Add your control notification handler code here
	m_nActiveChnl -= 1;
	if (m_nActiveChnl < 0)
		m_nActiveChnl += MEM_MAX_CHANNEL;
	m_nActiveChnl %= MEM_MAX_CHANNEL;

	SetDlgItemInt(IDC_EDIT_ACTIVECHNL, m_nActiveChnl+1, FALSE);

	ChangeActiveChnl(m_nActiveChnl);
}

void CScopeDlg::OnBtnSlowforward() 
{
	// TODO: Add your control notification handler code here
	m_nActiveChnl += 1;
	m_nActiveChnl %= MEM_MAX_CHANNEL;

	SetDlgItemInt(IDC_EDIT_ACTIVECHNL, m_nActiveChnl+1, FALSE);

	ChangeActiveChnl(m_nActiveChnl);
}

void CScopeDlg::OnBtnActivegate() 
{
	// TODO: Add your control notification handler code here
	m_nActiveGate++;
	m_nActiveGate %= MAX_GATES;

	CString s;
	s.Format(_T("%1d"), m_nActiveGate+1);
	GetDlgItem(IDC_BTN_ACTIVEGATE)->SetWindowText(s);

	gGate = m_nActiveGate;
	((CTscanDlg *) GetParent())->SendMsg(GATE_SELECT);
	((CTscanDlg *) GetParent())->UpdateChnlSelected();

	if (((CTscanDlg *) GetParent())->m_pAscanDlg)
		(((CTscanDlg *) GetParent())->m_pAscanDlg)->SetRfFullWaveMode();
}

void CScopeDlg::OnSetfocusCbScope1() 
{
	// TODO: Add your control notification handler code here
	CRect rectEdit;
	m_CBscope1.GetWindowRect(rectEdit);

	CMenu menu;
	menu.LoadMenu (IDR_MENU_SCOPE1);
	CMenu* pMenu = menu.GetSubMenu (0);
	int nCmd = pMenu->TrackPopupMenu (TPM_LEFTALIGN |
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
		rectEdit.left, rectEdit.bottom,
		this);

	SetFocus();

	switch (nCmd) {
	case ID_MENU_SCOPE1ASCAN:
		m_CBscope1.SetCurSel(0);
		break;
	case ID_MENU_SCOPE1RFTEST:
		m_CBscope1.SetCurSel(1);
		break;
	case ID_MENU_SCOPE1TCGCURVE:
		m_CBscope1.SetCurSel(2);
		break;
	case ID_MENU_SCOPE1ANALOG:
		m_CBscope1.SetCurSel(3);
		break;
	case ID_MENU_SCOPE1AMPLITUDE:
		m_CBscope1.SetCurSel(4);
		break;
	case ID_MENU_SCOPE1CALIBRATE:
		ScopeTrace1Calibrate();
		break;
	default:
		break;
	}

	OnSelchangeCbScope1();
}

void CScopeDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ClientToScreen(&point);
#if 0
	if (nFlags & MK_LBUTTON)
	{
		CRect rect;
		GetWindowRect(&rect);

		//rect.left += point.x - m_ptMouseStart.x;
		//rect.top += point.y - m_ptMouseStart.y;
		rect.OffsetRect(point.x - m_ptMouseStart.x, point.y - m_ptMouseStart.y);

		MoveWindow(&rect, TRUE);

		m_ptMouseStart = point;
	}
#endif
	CDialog::OnMouseMove(nFlags, point);
}

void CScopeDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ClientToScreen(&point);

	m_ptMouseStart = point;

	CDialog::OnLButtonDown(nFlags, point);
}

void CScopeDlg::ChangeActiveChnl(short nChannel)
{
	gChannel = nChannel;
	((CTscanDlg *) GetParent())->SendMsg(CHANNEL_SELECT);
	((CTscanDlg *) GetParent())->ChangeActiveChannel();
}

