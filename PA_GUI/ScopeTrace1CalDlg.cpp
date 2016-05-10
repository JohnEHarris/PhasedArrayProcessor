// ScopeTrace1Cal.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "TscanDlg.h"
#include "Extern.h"

#include "ScopeTrace1CalDlg.h"
#include "ScopeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CTscanDlg* pCTscanDlg;
//static CTscanDlg* m_pCTscanDlg;

extern CPtrList g_pTcpListUtData;  /* linked list holding the IData */
extern CCriticalSection g_CriticalSection;  /* Critical section for mutual exclusion access of g_pTcpListUtData */

/////////////////////////////////////////////////////////////////////////////
// CScopeTrace1CalDlg dialog


CScopeTrace1CalDlg::CScopeTrace1CalDlg(CWnd* pParent /*=NULL*/, CTcpThreadRxList* pThreadRxList /*=NULL*/)
	: CDialog(CScopeTrace1CalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScopeTrace1CalDlg)
	m_nEditValue = 0;
	//}}AFX_DATA_INIT

	m_bModified = false;
	m_pTcpThreadRxList = pThreadRxList;
	m_nInstrument = gChannel/MAX_CHANNEL_PER_INSTRUMENT;

	//m_pCTscanDlg = pCTscanDlg;

	m_bDo90Percent = TRUE;
}


void CScopeTrace1CalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScopeTrace1CalDlg)
	DDX_Control(pDX, IDC_BTN_VERIFY, m_btnVerify);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Control(pDX, IDC_COMBO_INSTRUMENT, m_cbInstrument);
	DDX_Control(pDX, IDCANCEL, m_btnExit);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_editValue);
	DDX_Control(pDX, IDC_EDIT_LEBEL2, m_editLabel2);
	DDX_Control(pDX, IDC_EDIT_LABEL1, m_editLabel1);
	DDX_Control(pDX, IDOK, m_btnNext);
	DDX_Control(pDX, IDBACK, m_btnBack);
	DDX_Control(pDX, IDC_EDIT_TRACE1CAL, m_editTrace1Cal);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_nEditValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScopeTrace1CalDlg, CDialog)
	//{{AFX_MSG_MAP(CScopeTrace1CalDlg)
	ON_BN_CLICKED(IDBACK, OnBack)
	ON_WM_LBUTTONDOWN()
	ON_EN_CHANGE(IDC_EDIT_VALUE, OnChangeEditValue)
	ON_CBN_SELCHANGE(IDC_COMBO_INSTRUMENT, OnSelchangeComboInstrument)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_BTN_VERIFY, OnBtnVerify)
	ON_BN_CLICKED(IDC_RADIO_DO90, OnRadioDo90)
	ON_BN_CLICKED(IDC_RADIO_DO10, OnRadioDo10)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_SEEKBUTTONVALUECHANGED, OnSeekButtonValueChanged )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScopeTrace1CalDlg message handlers
BOOL CScopeTrace1CalDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SaveOriginalConfig();

	m_cbInstrument.SetCurSel(m_nInstrument);

	Trace1CalStep1();

	CRect rect;
	m_editValue.GetWindowRect(&rect);
	ScreenToClient(&rect);
	int y1 = rect.bottom + 10;
	int y2 = rect.bottom + 30;
	int x1 = rect.left - 50;
	int x2 = x1;

	x1 = x2 + 10;
	x2 = x1 + 30;
	m_btnFastBackward.Create(_T("<<"), WS_CHILD | WS_VISIBLE, CRect(x1,y1,x2,y2), this, IDC_FASTBACKWARD_BUTTON );
	x1 = x2 + 10;
	x2 = x1 + 30;
	m_btnBackward.Create( _T("<"), WS_CHILD | WS_VISIBLE, CRect(x1,y1,x2,y2), this, IDC_BACKWARD_BUTTON );
	x1 = x2 + 10;
	x2 = x1 + 30;
	m_btnForward.Create( _T(">"), WS_CHILD | WS_VISIBLE, CRect(x1,y1,x2,y2), this, IDC_FORWARD_BUTTON );
	x1 = x2 + 10;
	x2 = x1 + 30;
	m_btnFastForward.Create( _T(">>"), WS_CHILD | WS_VISIBLE, CRect(x1,y1,x2,y2), this, IDC_FASTFORWARD_BUTTON );

	SetSeekButton();

	m_pTcpThreadRxList->SendSlaveMsg(NOP_MODE, 0,0,0,0,0,0);
	m_pTcpThreadRxList->SendSlaveMsg(ADC_MODE, m_nInstrument,0,0,0,0,0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScopeTrace1CalDlg::OnOK() 
{
	// TODO: Add extra validation here
	switch (m_nStep)
	{
	case 1:
		Trace1CalStep2();
		break;
	case 2:
		Trace1CalStep3();
		break;
	case 3:
		Trace1CalStep4();
		break;
	case 4:
		Trace1CalStep5();
		break;
	case 5:
		if (m_bModified)
		{
			int nID = AfxMessageBox(_T("Save the scope 1 alignment changes?"),
									   MB_YESNOCANCEL);
			switch (nID)
			{
			case IDYES:
				SaveCurrentCalChanges();
				break;
			case IDNO:
				break;
			case IDCANCEL:
				return;
				break;
			}
		}
		if (pCTscanDlg->m_pScopeTrace1CalDlg != NULL)
			pCTscanDlg->m_pScopeTrace1CalDlg = NULL;
		if ( memcmp( (void *) m_pTcpThreadRxList->m_pConfigRec, (void *) &m_OriginalConfigRec, sizeof (CONFIG_REC) ) != 0)
			RestoreOriginalConfig();

		KillTimer(100);
		::Sleep(100);
		m_pTcpThreadRxList->ClearListUtData();

		DestroyWindow();
		break;
	default:
		break;
	}

	//CDialog::OnOK();
}

void CScopeTrace1CalDlg::OnCancel() 
{
	if (m_nStep != 1 && m_bModified)
	{
		int nID = AfxMessageBox(_T("Save current scope alignment changes?"),
								   MB_YESNOCANCEL);
		switch (nID)
		{
		case IDOK:
			SaveCurrentCalChanges();
			break;
		case IDNO:
			break;
		case IDCANCEL:
			return;
			break;
		}
	}

	if (pCTscanDlg->m_pScopeTrace1CalDlg != NULL)
		pCTscanDlg->m_pScopeTrace1CalDlg = NULL;

	if ( memcmp( (void *) m_pTcpThreadRxList->m_pConfigRec, (void *) &m_OriginalConfigRec, sizeof (CONFIG_REC) ) != 0)
		RestoreOriginalConfig();

	// TODO: Add extra cleanup here

	KillTimer(100);
	::Sleep(100);
	m_pTcpThreadRxList->ClearListUtData();

	DestroyWindow();
	//CDialog::OnCancel();
}

void CScopeTrace1CalDlg::OnBack() 
{
	// TODO: Add your control notification handler code here
	switch (m_nStep)
	{
	case 1:
		break;
	case 2:
		Trace1CalStep1();
		break;
	case 3:
		Trace1CalStep2();
		break;
	case 4:
		Trace1CalStep3();
		break;
	case 5:
		Trace1CalStep4();
		break;
	default:
		break;
	}
}

void CScopeTrace1CalDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();

	delete this;
}

void CScopeTrace1CalDlg::Trace1CalStep1()
{
	m_btnBack.ShowWindow(SW_HIDE);
	m_btnNext.ShowWindow(SW_SHOW);
	m_editValue.ShowWindow(SW_HIDE);
	m_editLabel1.ShowWindow(SW_HIDE);
	m_editLabel2.ShowWindow(SW_HIDE);
	m_cbInstrument.EnableWindow(TRUE);
	m_btnStart.ShowWindow(SW_HIDE);
	m_btnVerify.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO_DO90)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO_DO10)->ShowWindow(SW_HIDE);

	m_nStep = 1;

	CString str, s;
	str.Format(_T("Step 1:\r\n\r\n"));
	s.Format(_T("This step sets the Trace I and II reference levels for all following adjustments - it\r\nshould be checked periodically.\r\n\r\n"));
	str += s;
	s.Format(_T("Notes:\t1) Trace II must be aligned before aligning Trace I.\r\n"));
	str += s;
	s.Format(_T("\t2) Receiver Offset must be nulled before aligning Trace I.\r\n\r\n"));
	str += s;
	s.Format(_T("Note that Trace I A_Scan Zero and Fs scaling is the same for Rf and Fw modes.\r\n\r\n"));
	str += s;
	s.Format(_T("Set Oscilloscope Trace I and II (scope channels 1 & 2)\tVertical     = 0.5 V/Div\r\n"));
	str += s;
	s.Format(_T("Set Oscilloscope Sweep Time\t\t\t\tHorizontal = 2.0 uS/Div\r\n\r\n"));
	str += s;
	s.Format(_T("Set Oscilloscope Trace I and II = \"On\"\r\n"));
	str += s;
	s.Format(_T("Set Oscilloscope Trace I and II Input Selects = \"Gnd\".\r\n\r\n"));
	str += s;
	s.Format(_T("Adjust Trace I and II sweep to exactly 3 divisions BELOW the center grid line.\r\n\r\n"));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, or \"Exit\" to quit."));
	str += s;
	m_editTrace1Cal.SetMargins(10,10);
	m_editTrace1Cal.SetWindowText(str);
}

void CScopeTrace1CalDlg::Trace1CalStep2()
{
	CWaitCursor waitCursor;

	m_nStep = 2;

	m_btnBack.ShowWindow(SW_SHOW);
	m_btnNext.ShowWindow(SW_SHOW);
	m_editValue.ShowWindow(SW_SHOW);
	m_editLabel1.ShowWindow(SW_SHOW);
	m_editLabel2.ShowWindow(SW_SHOW);
	m_editLabel1.SetWindowText(_T("mdac"));
	m_editLabel2.SetWindowText(_T("Scope Null"));
	m_cbInstrument.EnableWindow(FALSE);
	m_btnStart.ShowWindow(SW_HIDE);
	m_btnVerify.ShowWindow(SW_HIDE);

	ActionStep(m_nStep);
	::Sleep(50);

	WORD nChannel = m_pTcpThreadRxList->m_nActiveChannel;
	UpdateData(false);

	m_pTcpThreadRxList->SetScopeTrace1Mdac(m_nInstrument, 0x0011);  //force noise reject always on

	::Sleep(1500);

	m_pTcpThreadRxList->ClearListUtData();

	WORD Dummy;
	//for (int i=0; i<5; i++)
	//{
		m_pTcpThreadRxList->SendSlaveMsg(ADC_MODE, m_nInstrument,0,0,0,0,0);

		::Sleep(50);

		if (!GetAdcMsg(&Dummy, &m_nAdcNull))
			AfxMessageBox(_T("Failed reading AD converter!"));
		SetDlgItemInt(IDC_EDIT_ADC, m_nAdcNull, TRUE);
	//}

	CString str, s;
	str.Format(_T("Step 2:\r\n\r\n"));
	s.Format(_T("This step aligns Trace I \"A_Scan Baseline\" level on the Adi board.  The computer has now set:\r\n"));
	str += s;

	s.Format(_T("\tA. Active Channel\t\t\t= Lowest channel number in instrument\r\n"));
	str += s;
	s.Format(_T("\tB. Active Gate\t\t\t= I\r\n"));
	str += s;
	s.Format(_T("\tC. For all channels:\r\n"));
	str += s;
	s.Format(_T("\t\t1. Pulser\t\t\t= Off\r\n"));
	str += s;
	s.Format(_T("\t\t2. Receiver Detect Mode\t= Rf\r\n"));
	str += s;
	s.Format(_T("\t\t3. Receiver Polarity\t\t= Plus\r\n"));
	str += s;
	s.Format(_T("\t\t4. Receiver Gain\t\t= -20 dB\r\n"));
	str += s;
	s.Format(_T("\t\t5. Receiver Filter\t\t= 2.25 MHz\r\n"));
	str += s;
	s.Format(_T("\t\t6. Receiver Sequence Enable\t= On\r\n"));
	str += s;
	s.Format(_T("\tD. Noise Reject\t\t\t= \"Reject All\"\r\n"));
	str += s;
	s.Format(_T("\tE. Gates I\t\t\tTrigger\t= Ip\r\n"));
	str += s;
	s.Format(_T("\t\t\t\tRange\t= 4 uS\r\n"));
	str += s;
	s.Format(_T("\t\t\t\tDelay\t= 4 uS\r\n"));
	str += s;
	s.Format(_T("\tF. Gates II to IV\t\tTrigger\t= Off\r\n\r\n"));
	str += s;

	s.Format(_T("Set Oscilloscope Trace I Input Select\t=  \"Dc\".\r\n"));
	str += s;
	s.Format(_T("Set Oscilloscope Trace II\t\t=  \"Off\".\r\n\r\n"));
	str += s;
	s.Format(_T("Using the arrow keys, re-adjust Trace I sweep to exactly 3 divisions BELOW the center "));
	str += s;
	s.Format(_T("grid line.\r\n\r\n"));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace1Cal.SetMargins(10,10);
	m_editTrace1Cal.SetWindowText(str);

	SetSeekButton();

	//SetTimer(100, 110, NULL);
}


void CScopeTrace1CalDlg::Trace1CalStep3()
{
	CWaitCursor waitCursor;

	m_editLabel1.SetWindowText(_T(""));
	m_editLabel2.SetWindowText(_T(""));
	m_btnStart.ShowWindow(SW_SHOW);
	m_btnVerify.ShowWindow(SW_SHOW);
	m_btnStart.SetWindowText(_T("Start"));
	m_btnVerify.SetWindowText(_T("Verify"));

	m_nStep = 3;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);

	KillTimer(100);
	::Sleep(200);
	m_pTcpThreadRxList->ClearListUtData();

	ActionStep(m_nStep);
	m_pTcpThreadRxList->SetScopeTrace1Mdac(m_nInstrument, 0xF101);  //force noise reject off

	CString str, s;
	str.Format(_T("Step 3:\r\n\r\n"));
	s.Format(_T("This step aligns Scope Trace I \"A-Scan Zero\" (all channels in the instrument) on the Adi Board. The computer has now set:\r\n\r\n"));
	str += s;
	s.Format(_T("\tNoise Reject = \"No Reject\"\r\n\r\n"));
	str += s;

	s.Format(_T("You should now see a straight line on the scope at some level.  The computer will step through all channels in the instrument and measure the Zero voltage offset level for each.\r\n\r\n"));
	str += s;
	s.Format(_T("Click \"Start\" to initiate the automatic measurement process.  You should see the A_Scan level step up or down to stop at exactly 3 divisions BELOW the center grid line for each channel.  The display indicates which channel is being scanned.\r\n\r\n"));
	str += s;
	s.Format(_T("When \"Channel Zero Complete\" is displayed, click \"Do Again\" to re-scan the channels, \"Next\" to continue, \"Back\" to return to the previous step, \"Verify\" to visually verify the scan result, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace1Cal.SetMargins(10,10);
	m_editTrace1Cal.SetWindowText(str);

	SetSeekButton();
}

void CScopeTrace1CalDlg::Trace1CalStep4()
{
	CWaitCursor waitCursor;

	SetTimer(100, 110, NULL);

	m_editLabel1.SetWindowText(_T("mdac"));
	m_editLabel2.SetWindowText(_T("V_Gain"));
	m_btnStart.ShowWindow(SW_SHOW);
	m_btnVerify.ShowWindow(SW_SHOW);
	m_btnNext.SetWindowText(_T("Next"));
	m_btnStart.SetWindowText(_T("Do Another Channel"));
	m_btnVerify.SetWindowText(_T("Copy"));
	m_editValue.ShowWindow(SW_SHOW);
	m_btnExit.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_RADIO_DO90)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO_DO10)->ShowWindow(SW_HIDE);

	m_nStep = 4;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);

	ActionStep(m_nStep);
	m_pTcpThreadRxList->SetScopeTrace1Mdac(m_nInstrument, 0xF101);  //force noise reject off

	CString str, s;
	str.Format(_T("Step 4:\r\n\r\n"));
	s.Format(_T("This step aligns \"A_Scan Channel Full Scale\" on the Adi board.  One or more channels, in any order, may be aligned to \"Full Scale\" by selecting the \"Active Channel\" number.  The computer has now set:\r\n"));
	str += s;

	s.Format(_T("\tA. Active Channel\t\t\t= Lowest channel number in instrument - user\r\n"));
	str += s;
	s.Format(_T("\t\t\t\t\t may change to any channel\r\n"));
	str += s;
	s.Format(_T("\tB. Active Gate\t\t\t= I\r\n"));
	str += s;
	s.Format(_T("\tC. For all channels:\r\n"));
	str += s;
	s.Format(_T("\t\t1. Pulser\t\t\t= On\r\n"));
	str += s;
	s.Format(_T("\t\t2. Receiver Detect Mode\t= Fw\r\n"));
	str += s;
	s.Format(_T("\t\t3. Receiver Gain\t\t= -20 dB\r\n"));
	str += s;
	s.Format(_T("\t\t4. Receiver Filter\t\t= 2.25 MHz\r\n"));
	str += s;
	s.Format(_T("\t\t5. Receiver Sequence Enable\t= On\r\n"));
	str += s;
	s.Format(_T("\tD. Noise Reject\t\t\t= \"Reject Off\"\r\n"));
	str += s;
	s.Format(_T("\tE. Gates I\t\t\tTrigger\t= Ip\r\n"));
	str += s;
	s.Format(_T("\t\t\t\tRange\t= 4 uS\r\n"));
	str += s;
	s.Format(_T("\t\t\t\tDelay\t= 4 uS\r\n"));
	str += s;
	s.Format(_T("\tF. Gates II to IV\t\tTrigger\t= Off\r\n\r\n"));
	str += s;

	s.Format(_T("Set Oscilloscope Trace I Input Select\t=  \"Dc\".\r\n"));
	str += s;
	s.Format(_T("Set Oscilloscope Trace II\t\t=  \"Dc\" (Verify baseline position)\r\n\r\n"));
	str += s;

	s.Format(_T("Connect the 2.25 MHz liquid filled test probe block cable to the receiver channel to be set.  Click on \"Gate Delay\" and adjust Gate I Delay to highlight; then repeat for \"Gate Range\" to place one echo, the first back wall, inside Gate I.  Click on and adjust \"Receiver Gain\" for \"Catch\"=200 (100%% of Full Scale).\r\n\r\n"));
	str += s;

	s.Format(_T("Re-click on Scope Calibration drop-down box to highlight and using the arrow keys, adjust the amplitude of the Gate I echo on scope Trace I to exactly 5.0 divisions above the Trace I A_Scan baseline (2 divisions ABOVE the center grid line).\r\n\r\n"));
	str += s;
	s.Format(_T("The test probe block cable must be moved to each successive receiver channel to be aligned.\r\n\r\n"));
	str += s;
	s.Format(_T("When Channel is done, click \"Do Another Channel\" to do next channel, \"Copy\" to copy to other channels in the instrument, \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace1Cal.SetMargins(10,10);
	m_editTrace1Cal.SetWindowText(str);

	SetSeekButton();
}

/*
void CScopeTrace1CalDlg::Trace1CalStep5()
{
	CWaitCursor waitCursor;

	m_editLabel1.SetWindowText(_T("mdac"));
	m_editLabel2.SetWindowText(_T("V_Nr"));
	m_editValue.ShowWindow(SW_SHOW);
	m_btnNext.SetWindowText(_T("Next"));
	GetDlgItem(IDC_RADIO_DO90)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_RADIO_DO10)->ShowWindow(SW_SHOW);

	m_nStep = 5;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);

	ActionStep(m_nStep);
	m_pTcpThreadRxList->SetScopeTrace1Mdac(m_nInstrument, 0xF101);  //force noise reject off

	CString str, s;
	str.Format(_T("Step 5:\r\n\r\n");
	s.Format(_T("This step aligns \"A_Scan Baseline Noise Reject\" on the Adi Board.  The computer has now set:\r\n");
	str += s;
	s.Format(_T("\tA. Active Channel\t\t\t= Lowest channel number in instrument\r\n");
	str += s;
	s.Format(_T("\tB. Noise Reject\t\t\t= \"Reject Off\"\r\n");
	str += s;
	s.Format(_T("\tC. Gate I Threshold\t\t\t= 10%%\r\n");
	str += s;

	s.Format(_T("Connect the 2.25 MHz liquid filled test probe block cable to \"Active Channel\".  You should now see the first back-wall echo (scope Trace I) within Gate I (Trace II).\r\n\r\n");
	str += s;
	s.Format(_T("Adjust Receiver Gain for \"Catch\"=180 (90%% of Full Scale).  Verify that Trace I Gate I echo signal is exactly 4.5 divisions above the baseline.\r\n\r\n");
	str += s;
	s.Format(_T("Click \"Do 90 %%\"; the echo signal will disappear.  Using the down arrow key, click until the echo signal just flickers.\r\n\r\n");
	str += s;
	s.Format(_T("Adjust Receiver Gain for \"Catch\"=20 (10%% of Full Scale).  Verify that Trace I Gate I echo signal is exactly 1 division above the baseline.\r\n\r\n");
	str += s;
	s.Format(_T("Click \"Do 10 %%\"; the echo signal will disappear.  Using the down arrow key, click until the echo signal just flickers.\r\n\r\n\r\n");
	str += s;

	s.Format(_T("When through, click \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n");
	str += s;
	m_editTrace1Cal.SetMargins(10,10);
	m_editTrace1Cal.SetWindowText(str);

	SetSeekButton();
}
*/

void CScopeTrace1CalDlg::Trace1CalStep5()
{
	m_btnNext.SetWindowText(_T("Finish"));
	m_btnExit.ShowWindow(SW_HIDE);
	m_btnStart.ShowWindow(SW_HIDE);
	m_btnVerify.ShowWindow(SW_HIDE);
	m_editValue.ShowWindow(SW_HIDE);
	m_editLabel1.ShowWindow(SW_HIDE);
	m_editLabel2.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO_DO90)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO_DO10)->ShowWindow(SW_HIDE);

	KillTimer(100);
	::Sleep(200);
	m_pTcpThreadRxList->ClearListUtData();

	m_nStep = 5;

	CString str, s;
	str.Format(_T("Step 5:\r\n\r\n"));
	s.Format(_T("Alignment is now complete.\r\n\r\n\r\n"));
	str += s;
	s.Format(_T("When through, click \"Finish\" to save results , exit the procedure and restore original instrument setup, \"Back\" to return to the previous step.\r\n"));
	str += s;
	m_editTrace1Cal.SetMargins(10,10);
	m_editTrace1Cal.SetWindowText(str);
}

void CScopeTrace1CalDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

#if 1
LONG CScopeTrace1CalDlg::OnSeekButtonValueChanged( UINT id, LONG value )
{
#if 1
	if (m_nStep == 1)
		return 0L;

	m_bModified = true;

	int nMin, nMax, nStart,increment;

	nStart = (int) value;
	m_ValueStep[m_nStep] = nStart;

	nMin=0;
	nMax=0xFFF;
	//nStart = ConfigRec[gBoard].gates.delay[gChannel][gGate];
	increment = 10;

	m_btnForward.SetParameters( GetSafeHwnd(), nStart, CSeekButton::UP_KEY, 1, nMin, nMax);
	m_btnBackward.SetParameters( GetSafeHwnd(), nStart, CSeekButton::DOWN_KEY, 1, nMin, nMax);
	m_btnFastForward.SetParameters( GetSafeHwnd(), nStart, CSeekButton::UP_KEY, increment, nMin, nMax);
	m_btnFastBackward.SetParameters( GetSafeHwnd(), nStart, CSeekButton::DOWN_KEY, increment, nMin, nMax);

	m_nEditValue = nStart;

	UpdateData(FALSE);
	UpdateWindow();

	WORD nChannel = m_pTcpThreadRxList->m_nActiveChannel;

	m_pTcpThreadRxList->SetScopeTrace1CalData(m_nInstrument, nChannel, m_nStep, m_nEditValue, m_bDo90Percent);
#endif
	return 0L;
}
#endif


void CScopeTrace1CalDlg::SaveCurrentCalChanges()
{
	CONFIG_REC *m_pConfigRec = m_pTcpThreadRxList->m_pConfigRec;

	m_OriginalConfigRec.OscopeRec.V_Scope_Null[m_nInstrument] = m_pConfigRec->OscopeRec.V_Scope_Null[m_nInstrument];
	m_OriginalConfigRec.OscopeRec.V_Nr[m_nInstrument][0] = m_pConfigRec->OscopeRec.V_Nr[m_nInstrument][0];
	m_OriginalConfigRec.OscopeRec.V_Nr[m_nInstrument][1] = m_pConfigRec->OscopeRec.V_Nr[m_nInstrument][1];

	int nChnlBase = m_nInstrument * MAX_CHANNEL_PER_INSTRUMENT;
	for (int i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
	{
		m_OriginalConfigRec.OscopeRec.V_Gain[i+nChnlBase] = m_pConfigRec->OscopeRec.V_Gain[i+nChnlBase];
		m_OriginalConfigRec.OscopeRec.V_Ch_Null[i+nChnlBase] = m_pConfigRec->OscopeRec.V_Ch_Null[i+nChnlBase];
	}
}

void CScopeTrace1CalDlg::SaveOriginalConfig()
{
	m_OriginalConfigRec = m_pTcpThreadRxList->GetConfigRec();

	m_ValueStep[2] = m_OriginalConfigRec.OscopeRec.V_Scope_Null[m_nInstrument];
	m_ValueStep[3] = m_OriginalConfigRec.OscopeRec.V_Ch_Null[m_pTcpThreadRxList->m_nActiveChannel];
	m_ValueStep[4] = m_OriginalConfigRec.OscopeRec.V_Gain[m_pTcpThreadRxList->m_nActiveChannel];
	m_ValueStep[5] = m_OriginalConfigRec.OscopeRec.V_Nr[m_nInstrument][1];
}

#pragma optimize( "g", off )    // add this to remove internal compiler error when building release version
bool CScopeTrace1CalDlg::RestoreOriginalConfig()
{
	m_pTcpThreadRxList->InitializeConfigRec(&m_OriginalConfigRec, TRUE);

	pCTscanDlg->SystemInit();

	pCTscanDlg->UpdateDlgs();

	return true;
}
#pragma optimize( "", on )

void CScopeTrace1CalDlg::ActionStep(int nStep)
{
	int i, j;
	int nChannel = m_nInstrument*MAX_CHANNEL_PER_INSTRUMENT;

	m_pTcpThreadRxList->m_pConfigRec->UtRec.Shoe[m_nInstrument].sequence_length = 1;
	m_pTcpThreadRxList->SetSequenceLength(nChannel);

	// set up all channels in the instrument
	for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
	{
		//pCTscanDlg->ChangeActiveChannelTo(i+nChannel);
		gChannel = i+nChannel;
		m_pTcpThreadRxList->SendSlaveMsg(CHANNEL_SELECT, i+nChannel, 0, 0, 0, 0, 0);

		m_pTcpThreadRxList->SetTcgTrigSel(i+nChannel, 0);
		m_pTcpThreadRxList->SetRcvrGain(i+nChannel, 0);

		// Set receiver filter to 2.25MHz
		m_pTcpThreadRxList->SetRcvrFilter(gChannel,
										  1);
		// turn off gate 2, 3, and 4
		for (j=0; j<4; j++)
		{
			//pCTscanDlg->ChangeActiveGateTo(j);
			gGate = j;
			m_pTcpThreadRxList->SendSlaveMsg(GATE_SELECT, j, 0, 0, 0, 0, 0);
			if (j == 0)
			{
				m_pTcpThreadRxList->SetGateTrigMode(i+nChannel, j, 1);
				m_pTcpThreadRxList->SetGatePolarity(i+nChannel, j, 0);

				if (nStep < 4)
					m_pTcpThreadRxList->SetGateDetectMode(i+nChannel, 0, 0);  //RF
				else
					m_pTcpThreadRxList->SetGateDetectMode(i+nChannel, 0, 1);  //Fw

				// Set gate delay to 4 uS
				m_pTcpThreadRxList->SetGateDelay(i+nChannel, j, 4.0);

				// Set gate range to 4 uS
				m_pTcpThreadRxList->SetGateRange(i+nChannel, j, 4.0);
			}
			else
				m_pTcpThreadRxList->SetGateTrigMode(i+nChannel,j,0);
		}
	}

	pCTscanDlg->ChangeActiveChannelTo(nChannel);
	pCTscanDlg->ChangeActiveGateTo(0);
	
	switch(nStep)
	{
	case 2:
		SetSysPulserOnOff(FALSE);  // turn pulser off
		SetSysRcvrOnOff(TRUE);     // turn receiver on
		break;
	case 3:
		SetSysPulserOnOff(FALSE);  // turn pulser off
		SetSysRcvrOnOff(TRUE);     // turn receiver on
		break;
	case 4:
		SetSysPulserOnOff(TRUE);   // turn pulser on
		break;
	case 5:
		m_pTcpThreadRxList->SetGateLevel(nChannel, 0, 10);
		break;
	case 6:
		break;
	default:
		return;
		break;
	}

	// Set scope trace 2 to display active gate only
	m_pTcpThreadRxList->SetScopeTrace2DisplaySel(nChannel, 0);

	pCTscanDlg->UpdateDlgs();

	m_nEditValue = m_ValueStep[m_nStep] = (int) m_pTcpThreadRxList->GetScopeTrace1CalData(m_nInstrument, nChannel, m_nStep, m_bDo90Percent);
	m_pTcpThreadRxList->SetScopeTrace1CalData(m_nInstrument, nChannel, m_nStep, (WORD) m_nEditValue, m_bDo90Percent);
}

void CScopeTrace1CalDlg::OnChangeEditValue() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if (m_editValue.GetWindowTextLength() == 0)
		return;
	//CString str;
	//m_editValue.GetWindowText(str);
	//if (str.Find("\r") == -1)
		//return;
	//AfxMessageBox(str);
	UpdateData(TRUE);

	if ( (m_nEditValue < 0) || (m_nEditValue > 4095) )
	{
		AfxMessageBox(_T("Please enter a number between 0 and 4095"));
		m_nEditValue = m_ValueStep[m_nStep];
		UpdateData(FALSE);
		return;
	}

	m_ValueStep[m_nStep] = m_nEditValue;
	if (m_pTcpThreadRxList->m_nNumberOfBoards > 1)
		m_pTcpThreadRxList->SetScopeTrace1CalData(m_nInstrument, 0, m_nStep, m_nEditValue, m_bDo90Percent);
	m_bModified = true;

	SetSeekButton();
}

void CScopeTrace1CalDlg::SetSeekButton()
{
	int nMin, nMax, nStart, increment;
	HWND hWnd = GetSafeHwnd();

	increment = 10;
		
	nMin=0;
	nMax=0xFFF;
	nStart = m_ValueStep[m_nStep];

	switch (m_nStep)
		{
	case 1:
		hWnd = NULL;
		break;
	case 12:   
		hWnd = NULL;
		break;
	default:
		break;
		}

	m_btnForward.SetParameters( hWnd, nStart, 0, 1, nMin, nMax);
	m_btnBackward.SetParameters( hWnd, nStart, CSeekButton::DOWN_KEY, 1, nMin, nMax);
	m_btnFastForward.SetParameters( hWnd, nStart, 0, increment, nMin, nMax);
	m_btnFastBackward.SetParameters( hWnd, nStart, CSeekButton::DOWN_KEY, increment, nMin, nMax);
}

void CScopeTrace1CalDlg::OnSelchangeComboInstrument() 
{
	// TODO: Add your control notification handler code here
	m_nInstrument = (WORD) m_cbInstrument.GetCurSel();

	UpdateDlg();
}

void CScopeTrace1CalDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	// copy inspection data from g_pListUtData to plistUtdata
	m_pTcpThreadRxList->SendSlaveMsg(ADC_MODE, m_nInstrument,0,0,0,0,0);

	I_MSG_RUN *pImsgRun;
	g_CriticalSection.Lock();
	while (!g_pTcpListUtData.IsEmpty() )
	{
		pImsgRun = (I_MSG_RUN *) g_pTcpListUtData.RemoveHead();
		plistUtData.AddTail((void *) pImsgRun);
	}
	g_CriticalSection.Unlock();
	
	I_MSG_RUN	*pImsg;
	I_MSG_ADC	*pImsgAdc;

	while (plistUtData.GetCount() > 1)
	{
		pImsg = (I_MSG_RUN *) plistUtData.RemoveHead();
		delete pImsg;
	}

	while (!plistUtData.IsEmpty() )
	{
		pImsg = (I_MSG_RUN *) plistUtData.RemoveHead();
		pImsgAdc = (I_MSG_ADC *) pImsg;
		if (pImsg->MstrHdr.MsgId  == ADC_MODE)
			SetDlgItemInt(IDC_EDIT_GATE1AMP, (UINT) pImsgAdc->nGate1Amp, FALSE);
		delete pImsg;
	}

	CDialog::OnTimer(nIDEvent);
}


void CScopeTrace1CalDlg::SetSysPulserOnOff(BOOL bOn)
{
	CONFIG_REC *pConfigRec = m_pTcpThreadRxList->m_pConfigRec;

	for (int nShoe=0; nShoe<MAX_SHOES; nShoe++)
	{
		for (int nChannel=0; nChannel<MAX_CHANNEL_PER_INSTRUMENT; nChannel++)
		{
			if (bOn)
				pConfigRec->UtRec.Shoe[nShoe].Ch[nChannel].Fire_Sequence = 1;
			else
				pConfigRec->UtRec.Shoe[nShoe].Ch[nChannel].Fire_Sequence = 0;

			m_pTcpThreadRxList->EnableFireSequence(nShoe*10+nChannel, TRUE);
		}
	}
}


void CScopeTrace1CalDlg::SetSysRcvrOnOff(BOOL bOn)
{
	CONFIG_REC *pConfigRec = m_pTcpThreadRxList->m_pConfigRec;

	for (int nShoe=0; nShoe<MAX_SHOES; nShoe++)
	{
		for (int nChannel=0; nChannel<MAX_CHANNEL_PER_INSTRUMENT; nChannel++)
		{
			if (bOn)
				pConfigRec->UtRec.Shoe[nShoe].Ch[nChannel].Rcvr_Sequence = 1;
			else
				pConfigRec->UtRec.Shoe[nShoe].Ch[nChannel].Rcvr_Sequence = 0;

			m_pTcpThreadRxList->EnableFireSequence(nShoe*10+nChannel, TRUE);
		}
	}
}


void CScopeTrace1CalDlg::SetNoiseReject(BOOL bReject)
{

}


BOOL CScopeTrace1CalDlg::GetAdcMsg(WORD *pGate1Amp, short *pAdcValue)
{
	int nTryCnt = 0;
	BOOL bTryAgain = TRUE;
	I_MSG_RUN *pImsgRun;
	I_MSG_ADC *pAdcMsg;

	while ( bTryAgain && (nTryCnt<100) )
	{
		g_CriticalSection.Lock();
		while (!g_pTcpListUtData.IsEmpty() )
		{
			pImsgRun = (I_MSG_RUN *) g_pTcpListUtData.RemoveHead();
			pAdcMsg = (I_MSG_ADC *) pImsgRun;
			if (pImsgRun->MstrHdr.MsgId == ADC_MODE)
			{
				*pGate1Amp = pAdcMsg->nGate1Amp;
				*pAdcValue = pAdcMsg->nADC;
				bTryAgain = FALSE;
			}
			delete pImsgRun;
		}
		g_CriticalSection.Unlock();

		if (bTryAgain)
			::Sleep(50);

		nTryCnt++;
	}

	if (bTryAgain)
		return FALSE;
	else
		return TRUE;
}

void CScopeTrace1CalDlg::OnBtnStart() 
{
	// TODO: Add your control notification handler code here
	int i;
	short nChnlBase = m_nInstrument * MAX_CHANNEL_PER_INSTRUMENT;
	short nChannel = m_pTcpThreadRxList->m_nActiveChannel;
	short nADClow;
	short nADChigh;
	WORD dummy;
	WORD nMdacLow = 0x0500;
	WORD nMdacHigh = 0x0A00;
	float slope, offset;
	WORD nChnlNull;
	CWaitCursor waitCursor;

	switch (m_nStep)
	{
	case 3:
		m_btnStart.EnableWindow(FALSE);	

		for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
		{
			pCTscanDlg->ChangeActiveChannelTo(i+nChnlBase);
			pCTscanDlg->UpdateWindow();
			m_pTcpThreadRxList->SetScopeTrace1CalData(m_nInstrument, i+nChnlBase, 3, nMdacLow, m_bDo90Percent);
			::Sleep(50);
			m_pTcpThreadRxList->SendSlaveMsg(ADC_MODE, m_nInstrument,0,0,0,0,0);
			if (!GetAdcMsg(&dummy, &nADClow))
				AfxMessageBox(_T("Failed reading AD converter!"));
			SetDlgItemInt(IDC_EDIT_ADC, nADClow, TRUE);
			m_pTcpThreadRxList->SetScopeTrace1CalData(m_nInstrument, i+nChnlBase, 3, nMdacHigh, m_bDo90Percent);
			::Sleep(50);
			m_pTcpThreadRxList->SendSlaveMsg(ADC_MODE, m_nInstrument,0,0,0,0,0);
			if (!GetAdcMsg(&dummy, &nADChigh))
				AfxMessageBox(_T("Failed reading AD converter!"));
			SetDlgItemInt(IDC_EDIT_ADC, nADChigh, TRUE);

			slope = ((float) (nADChigh-nADClow)) / ((float) (nMdacHigh-nMdacLow));
			offset = (float) nADChigh - slope*((float) nMdacHigh);

			if (slope != 0.0f)
				nChnlNull = (WORD) (((float) m_nAdcNull - offset)/slope);
			else
				nChnlNull = 0x800;

			m_pTcpThreadRxList->SetScopeTrace1CalData(m_nInstrument, i+nChnlBase, 3, nChnlNull, m_bDo90Percent);
			SetDlgItemInt(IDC_EDIT_VALUE, nChnlNull, FALSE);
			::Sleep(50);

			::Sleep(500);
		}

		MessageBox(_T("Channel Zero Complete."), _T("Truscan"), MB_OK);
		m_btnStart.EnableWindow(TRUE);
		m_btnStart.SetWindowText(_T("Do Again"));
		break;

	case 4:
		nChannel = nChnlBase + (nChannel+1)%MAX_CHANNEL_PER_INSTRUMENT;
		pCTscanDlg->ChangeActiveChannelTo(nChannel);
		break;

	default:
		break;
	}

}

void CScopeTrace1CalDlg::OnBtnVerify() 
{
	// TODO: Add your control notification handler code here
	int i, ic;
	short nChnlBase = m_nInstrument * MAX_CHANNEL_PER_INSTRUMENT;
	short nChannel = m_pTcpThreadRxList->m_nActiveChannel;

	switch (m_nStep)
	{
	case 3:
		for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
		{
			pCTscanDlg->ChangeActiveChannelTo(i+nChnlBase);
			pCTscanDlg->UpdateWindow();
			::Sleep(1000);
		}

		MessageBox(_T("Verification Complete."), _T("Truscan"), MB_OK);
		break;

	case 4:
		if ( MessageBox(_T("Do you really want to do this?"), _T("Copy V_Gain"),MB_YESNO)== IDNO )
			return;

		for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
		{
			ic = nChnlBase + i;
			if (ic != nChannel)
			{
				m_pTcpThreadRxList->m_pConfigRec->OscopeRec.V_Gain[ic] 
					= m_pTcpThreadRxList->m_pConfigRec->OscopeRec.V_Gain[nChannel];
			}
		}
		break;

	default:
		break;
	}
}

void CScopeTrace1CalDlg::OnRadioDo90() 
{
	// TODO: Add your control notification handler code here
	m_bDo90Percent = TRUE;

	m_nEditValue = m_ValueStep[m_nStep] = 0x0001;
	m_pTcpThreadRxList->SetScopeTrace1CalData(m_nInstrument, 0, m_nStep, (WORD) m_nEditValue, m_bDo90Percent);

	UpdateData(FALSE);
	SetSeekButton();
}

void CScopeTrace1CalDlg::OnRadioDo10() 
{
	// TODO: Add your control notification handler code here
	m_bDo90Percent = FALSE;

	m_nEditValue = m_ValueStep[m_nStep] = 0x0001;
	m_pTcpThreadRxList->SetScopeTrace1CalData(m_nInstrument, 0, m_nStep, (WORD) m_nEditValue, m_bDo90Percent);

	UpdateData(FALSE);
	SetSeekButton();
}


void CScopeTrace1CalDlg::UpdateDlg()
{
	int nChnlBase = m_nInstrument * MAX_CHANNEL_PER_INSTRUMENT;
	int nMaxChnl = nChnlBase + MAX_CHANNEL_PER_INSTRUMENT;

	if ( (gChannel < nChnlBase) || (gChannel >= nMaxChnl) )
	{
		AfxMessageBox (_T("The active channel you selected is not in the current instrument you are calibrating.  The active channel will be set to the lowest channel number in the current instrument."));
		pCTscanDlg->ChangeActiveChannelTo(nChnlBase);
	}

	m_nEditValue = m_ValueStep[m_nStep] = (int) m_pTcpThreadRxList->GetScopeTrace1CalData(m_nInstrument, 
																	m_pTcpThreadRxList->m_nActiveChannel,
																	m_nStep, 
																	m_bDo90Percent);
	SetSeekButton();
	UpdateData(FALSE);
}