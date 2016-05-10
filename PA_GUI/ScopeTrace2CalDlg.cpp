// ScopeTrace2Cal.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "TscanDlg.h"

#include "ScopeTrace2CalDlg.h"
#include "ScopeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CTscanDlg* pCTscanDlg;
//static CTscanDlg* m_pCTscanDlg;

/////////////////////////////////////////////////////////////////////////////
// CScopeTrace2CalDlg dialog


CScopeTrace2CalDlg::CScopeTrace2CalDlg(CWnd* pParent /*=NULL*/, CTcpThreadRxList* pThreadRxList /*=NULL*/)
	: CDialog(CScopeTrace2CalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScopeTrace2CalDlg)
	m_nEditValue = 0;
	//}}AFX_DATA_INIT

	m_bModified = false;
	m_pTcpThreadRxList = pThreadRxList;
	m_nInstrument = gChannel/MAX_CHANNEL_PER_INSTRUMENT;

	//m_pCTscanDlg = pCTscanDlg;
}


void CScopeTrace2CalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScopeTrace2CalDlg)
	DDX_Control(pDX, IDC_COMBO_INSTRUMENT, m_cbInstrument);
	DDX_Control(pDX, IDCANCEL, m_btnExit);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_editValue);
	DDX_Control(pDX, IDC_EDIT_LEBEL2, m_editLabel2);
	DDX_Control(pDX, IDC_EDIT_LABEL1, m_editLabel1);
	DDX_Control(pDX, IDOK, m_btnNext);
	DDX_Control(pDX, IDBACK, m_btnBack);
	DDX_Control(pDX, IDC_EDIT_TRACE2CAL, m_editTrace2Cal);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_nEditValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScopeTrace2CalDlg, CDialog)
	//{{AFX_MSG_MAP(CScopeTrace2CalDlg)
	ON_BN_CLICKED(IDBACK, OnBack)
	ON_WM_LBUTTONDOWN()
	ON_EN_CHANGE(IDC_EDIT_VALUE, OnChangeEditValue)
	ON_CBN_SELCHANGE(IDC_COMBO_INSTRUMENT, OnSelchangeComboInstrument)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_SEEKBUTTONVALUECHANGED, OnSeekButtonValueChanged )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScopeTrace2CalDlg message handlers
BOOL CScopeTrace2CalDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SaveOriginalConfig();

	m_cbInstrument.SetCurSel(m_nInstrument);

	Trace2CalStep1();

	CRect rect;
	m_editValue.GetWindowRect(&rect);
	ScreenToClient(&rect);
	int y1 = rect.bottom + 10;
	int y2 = rect.bottom + 30;
	int x1 = rect.left - 50;
	int x2 = x1;

	x1 = x2 + 10;
	x2 = x1 + 30;
	m_btnFastBackward.Create( _T("<<"), WS_CHILD | WS_VISIBLE, CRect(x1,y1,x2,y2), this, IDC_FASTBACKWARD_BUTTON );
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
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScopeTrace2CalDlg::OnOK() 
{
	// TODO: Add extra validation here
	switch (m_nStep)
	{
	case 1:
		Trace2CalStep2();
		break;
	case 2:
		Trace2CalStep3();
		break;
	case 3:
		Trace2CalStep4();
		break;
	case 4:
		Trace2CalStep5();
		break;
	case 5:
		Trace2CalStep6();
		break;
	case 6:
		Trace2CalStep7();
		break;
	case 7:
		Trace2CalStep8();
		break;
	case 8:
		Trace2CalStep9();
		break;
	case 9:
		Trace2CalStep10();
		break;
	case 10:
		Trace2CalStep11();
		break;
	case 11:
		if (m_bModified)
		{
			int nID = AfxMessageBox(_T("Save current scope alignment changes?"),
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
		if ( memcmp( (void *) m_pTcpThreadRxList->m_pConfigRec, (void *) &m_OriginalConfigRec, sizeof (CONFIG_REC) ) != 0)
			RestoreOriginalConfig();
		if (pCTscanDlg->m_pScopeTrace2CalDlg != NULL)
			pCTscanDlg->m_pScopeTrace2CalDlg = NULL;
		DestroyWindow();
		break;
	}

	//CDialog::OnOK();
}

void CScopeTrace2CalDlg::OnCancel() 
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

	if ( memcmp( (void *) m_pTcpThreadRxList->m_pConfigRec, (void *) &m_OriginalConfigRec, sizeof (CONFIG_REC) ) != 0)
		RestoreOriginalConfig();

	// TODO: Add extra cleanup here
	//((CScopeDlg*) GetParent())->m_pScopeTrace2CalDlg = NULL;
	if (pCTscanDlg->m_pScopeTrace2CalDlg != NULL)
		pCTscanDlg->m_pScopeTrace2CalDlg = NULL;

	DestroyWindow();
	//CDialog::OnCancel();
}

void CScopeTrace2CalDlg::OnBack() 
{
	// TODO: Add your control notification handler code here
	switch (m_nStep)
	{
	case 1:
		break;
	case 2:
		Trace2CalStep1();
		break;
	case 3:
		Trace2CalStep2();
		break;
	case 4:
		Trace2CalStep3();
		break;
	case 5:
		Trace2CalStep4();
		break;
	case 6:
		Trace2CalStep5();
		break;
	case 7:
		Trace2CalStep6();
		break;
	case 8:

		Trace2CalStep7();
		break;
	case 9:
		Trace2CalStep8();
		break;
	case 10:
		Trace2CalStep9();
		break;
	case 11:
		Trace2CalStep10();
		break;
	case 12:
		Trace2CalStep11();
		break;
	}
}

void CScopeTrace2CalDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();

	delete this;
}

void CScopeTrace2CalDlg::Trace2CalStep1()
{
	m_btnBack.ShowWindow(SW_HIDE);
	m_btnNext.ShowWindow(SW_SHOW);
	m_editValue.ShowWindow(SW_HIDE);
	m_editLabel1.ShowWindow(SW_HIDE);
	m_editLabel2.ShowWindow(SW_HIDE);
	m_cbInstrument.EnableWindow(TRUE);

	m_nStep = 1;

	CString str, s;
	str.Format(_T("Step 1:\r\n\r\n"));
	s.Format(_T("This step sets the scope Trace II reference for all following adjustments - it\r\nshould be checked periodically.\r\n\r\n"));
	str += s;
	s.Format(_T("Note that Trace II Gate Zero and Fs scaling is the same for Rf and Fw modes.\r\n\r\n"));
	str += s;
	s.Format(_T("Set Oscilloscope Trace II (scope channel 2)\tVertical     = 0.5 V/Div\r\n"));
	str += s;
	s.Format(_T("Set Oscilloscope Sweep Time\t\t\tHorizontal = 5.0 uS/Div\r\n\r\n"));
	str += s;
	s.Format(_T("Set Oscilloscope Trace I  = \"Off\"\r\n"));
	str += s;
	s.Format(_T("Set Oscilloscope Trace II = \"On\".\r\n"));
	str += s;
	s.Format(_T("Set Oscilloscope Trace II Input Select = \"Gnd\".\r\n\r\n"));
	str += s;
	s.Format(_T("Adjust Trace II sweep to exactly 3 divisions BELOW the center grid line.\r\n\r\n"));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, or \"Exit\" to quit."));
	str += s;
	m_editTrace2Cal.SetMargins(10,10);
	m_editTrace2Cal.SetWindowText(str);
}

void CScopeTrace2CalDlg::Trace2CalStep2()
{
	m_btnBack.ShowWindow(SW_SHOW);
	m_btnNext.ShowWindow(SW_SHOW);
	m_editValue.ShowWindow(SW_SHOW);
	m_editLabel1.ShowWindow(SW_SHOW);
	m_editLabel2.ShowWindow(SW_SHOW);
	m_editLabel1.SetWindowText(_T("shunt mdac"));
	m_editLabel2.SetWindowText(_T("Rf&Fw Mode Shunt"));
	m_cbInstrument.EnableWindow(FALSE);

	m_nStep = 2;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);

	if (m_pTcpThreadRxList->m_nNumberOfBoards > 1)
		m_pTcpThreadRxList->SetScopeTrace2CalData(m_nStep, m_nEditValue);

	CString str, s;
	str.Format(_T("Step 2:\r\n\r\n"));
	s.Format(_T("This step aligns Trace II \"Gate Baseline\" on the Adi board.\r\n\r\n"));
	str += s;
	s.Format(_T("Set Trace II input Select =  \"Dc\".\r\n\r\n"));
	str += s;
	s.Format(_T("Using the arrow keys, re-adjust Trace II sweep to exactly 3 divisions BELOW the center "));
	str += s;
	s.Format(_T("grid line.\r\n\r\n"));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace2Cal.SetMargins(10,10);
	m_editTrace2Cal.SetWindowText(str);

	SetSeekButton();
}

void CScopeTrace2CalDlg::Trace2CalStep3()
{
	m_editLabel1.SetWindowText(_T("Gate I mdac"));
	m_editLabel2.SetWindowText(_T("Gate I Zero"));

	m_nStep = 3;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);


	ActionStep(m_nStep);

	CString str, s;
	str.Format(_T("Step 3:\r\n\r\n"));
	s.Format(_T("This step aligns \"Gate I Zero\" on the Adi Board. The computer has now set:\r\n"));
	str += s;
	s.Format(_T("\tA. Active Channel\t\t\t\t= Lowest channel number in system\r\n"));
	str += s;
	s.Format(_T("\tB. Active Gate\t\t\t\t= I\r\n"));
	str += s;
	s.Format(_T("\tC. For all channels:\r\n"));
	str += s;
	s.Format(_T("\t\t1. Pulser\t\t\t\t= Off\r\n"));
	str += s;
	s.Format(_T("\t\t2. Receiver Detect Mode\t\t= Fw\r\n"));
	str += s;
	s.Format(_T("\t\t3. Receiver Gain\t\t\t= -20 dB\r\n"));
	str += s;
	s.Format(_T("\t\t4. Receiver Sequence Enable\t\t= On\r\n"));
	str += s;
	s.Format(_T("\tD. Gates I through IV\tTrigger\t\t= Ip\r\n"));
	str += s;
	s.Format(_T("\t\t\t\tRange\t\t= 4 uS\r\n"));
	str += s;
	s.Format(_T("\t\t\t\tDelay\t\t= 4 uS\r\n"));
	str += s;

	s.Format(_T("You should now see Gate I on the scope at some level.  One Gate at a time is set to trig on Ip. Only one channel is used, Active Gate is shown.\r\n\r\n"));
	str += s;
	s.Format(_T("Using the arrow keys, adjust the TOP of the gate to exactly 3 divisions BELOW the center grid line (same as the Shunt level - "));
	str += s;
	s.Format(_T("you should now see a straight line).\r\n\r\n\r\n"));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace2Cal.SetMargins(10,10);
	m_editTrace2Cal.SetWindowText(str);

	SetSeekButton();
}

void CScopeTrace2CalDlg::Trace2CalStep4()
{
	m_editLabel1.SetWindowText(_T("Gate I mdac"));
	m_editLabel2.SetWindowText(_T("Gate I Full Scale"));

	m_nStep = 4;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);

	ActionStep(m_nStep);

	CString str, s;
	str.Format(_T("Step 4:\r\n\r\n"));
	s.Format(_T("This step aligns \"Gate I Full Scale\" on the Adi Board. \r\n\r\n"));
	str += s;
	s.Format(_T("Using the arrow keys, adjust the TOP of the gate to exactly 2 divisions ABOVE the center grid line ("));
	str += s;
	s.Format(_T("you should now see Gate I spanning from 3 divisions below the center grid line to 2 divisions above the center grid line).\r\n\r\n\r\n"));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace2Cal.SetMargins(10,10);
	m_editTrace2Cal.SetWindowText(str);

	SetSeekButton();
}

void CScopeTrace2CalDlg::Trace2CalStep5()
{
	m_editLabel1.SetWindowText(_T("Gate II mdac"));
	m_editLabel2.SetWindowText(_T("Gate II Zero"));

	m_nStep = 5;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);

	ActionStep(m_nStep);

	CString str, s;
	str.Format(_T("Step 5:\r\n\r\n"));
	s.Format(_T("This step aligns \"Gate II Zero\" on the Adi Board.\r\n"));
	str += s;
	s.Format(_T("You should now see Gate II on the scope at some level.  The next 2 steps repeat the previous 2 steps to align Gate II.\r\n\r\n"));
	str += s;
	s.Format(_T("Using the arrow keys, adjust the TOP of the gate to exactly 3 divisions BELOW the center grid line."));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace2Cal.SetMargins(10,10);
	m_editTrace2Cal.SetWindowText(str);

	SetSeekButton();
}

void CScopeTrace2CalDlg::Trace2CalStep6()
{
	m_editLabel1.SetWindowText(_T("Gate II mdac"));
	m_editLabel2.SetWindowText(_T("Gate II Full Scale"));

	m_nStep = 6;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);

	ActionStep(m_nStep);

	CString str, s;
	str.Format(_T("Step 6:\r\n\r\n"));
	s.Format(_T("This step aligns \"Gate II Full Scale\" on the Adi Board. \r\n\r\n"));
	str += s;
	s.Format(_T("Using the arrow keys, adjust the TOP of the gate to exactly 2 divisions ABOVE the center grid line ("));
	str += s;
	s.Format(_T("you should now see Gate II spanning from 3 divisions below the center grid line to 2 divisions above the center grid line).\r\n\r\n\r\n"));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace2Cal.SetMargins(10,10);
	m_editTrace2Cal.SetWindowText(str);

	SetSeekButton();
}

void CScopeTrace2CalDlg::Trace2CalStep7()
{
	m_editLabel1.SetWindowText(_T("Gate III mdac"));
	m_editLabel2.SetWindowText(_T("Gate III Zero"));

	m_nStep = 7;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);

	ActionStep(m_nStep);

	CString str, s;
	str.Format(_T("Step 7:\r\n\r\n"));
	s.Format(_T("This step aligns \"Gate III Zero\" on the Adi Board.\r\n"));
	str += s;
	s.Format(_T("You should now see Gate III on the scope at some level.  The next 2 steps repeat the previous 2 steps to align Gate III.\r\n\r\n"));
	str += s;
	s.Format(_T("Using the arrow keys, adjust the TOP of the gate to exactly 3 divisions BELOW the center grid line."));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace2Cal.SetMargins(10,10);
	m_editTrace2Cal.SetWindowText(str);

	SetSeekButton();
}

void CScopeTrace2CalDlg::Trace2CalStep8()
{
	m_editLabel1.SetWindowText(_T("Gate III mdac"));
	m_editLabel2.SetWindowText(_T("Gate III Full Scale"));

	m_nStep = 8;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);

	ActionStep(m_nStep);

	CString str, s;
	str.Format(_T("Step 8:\r\n\r\n"));
	s.Format(_T("This step aligns \"Gate III Full Scale\" on the Adi Board. \r\n\r\n"));
	str += s;
	s.Format(_T("Using the arrow keys, adjust the TOP of the gate to exactly 2 divisions ABOVE the center grid line ("));
	str += s;
	s.Format(_T("you should now see Gate III spanning from 3 divisions below the center grid line to 2 divisions above the center grid line).\r\n\r\n\r\n"));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace2Cal.SetMargins(10,10);
	m_editTrace2Cal.SetWindowText(str);

	SetSeekButton();
}

void CScopeTrace2CalDlg::Trace2CalStep9()
{
	m_editLabel1.SetWindowText(_T("Gate IV mdac"));
	m_editLabel2.SetWindowText(_T("Gate IV Zero"));

	m_nStep = 9;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);

	ActionStep(m_nStep);

	CString str, s;
	str.Format(_T("Step 9:\r\n\r\n"));
	s.Format(_T("This step aligns \"Gate IV Zero\" on the Adi Board.\r\n"));
	str += s;
	s.Format(_T("You should now see Gate IV on the scope at some level.  The next 2 steps repeat the previous 2 steps to align Gate IV.\r\n\r\n"));
	str += s;
	s.Format(_T("Using the arrow keys, adjust the TOP of the gate to exactly 3 divisions BELOW the center grid line."));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace2Cal.SetMargins(10,10);
	m_editTrace2Cal.SetWindowText(str);

	SetSeekButton();
}

void CScopeTrace2CalDlg::Trace2CalStep10()
{
	m_btnNext.SetWindowText(_T("Next"));
	m_btnExit.ShowWindow(SW_SHOW);
	m_editValue.ShowWindow(SW_SHOW);
	m_editLabel1.ShowWindow(SW_SHOW);
	m_editLabel2.ShowWindow(SW_SHOW);
	m_editLabel1.SetWindowText(_T("Gate IV mdac"));
	m_editLabel2.SetWindowText(_T("Gate IV Full Scale"));

	m_nStep = 10;
	m_nEditValue = m_ValueStep[m_nStep];
	UpdateData(false);

	ActionStep(m_nStep);

	CString str, s;
	str.Format(_T("Step 10:\r\n\r\n"));
	s.Format(_T("This step aligns \"Gate IV Full Scale\" on the Adi Board. \r\n\r\n"));
	str += s;
	s.Format(_T("Using the arrow keys, adjust the TOP of the gate to exactly 2 divisions ABOVE the center grid line ("));
	str += s;
	s.Format(_T("you should now see Gate IV spanning from 3 divisions below the center grid line to 2 divisions above the center grid line).\r\n\r\n\r\n"));
	str += s;
	s.Format(_T("When through, click \"Next\" to continue, \"Back\" to return to the previous step, or \"Exit\" to quit.\r\n"));
	str += s;
	m_editTrace2Cal.SetMargins(10,10);
	m_editTrace2Cal.SetWindowText(str);

	SetSeekButton();
}

void CScopeTrace2CalDlg::Trace2CalStep11()
{
	m_btnNext.SetWindowText(_T("Finish"));
	m_btnExit.ShowWindow(SW_HIDE);
	m_editValue.ShowWindow(SW_HIDE);
	m_editLabel1.ShowWindow(SW_HIDE);
	m_editLabel2.ShowWindow(SW_HIDE);

	m_nStep = 11;

	CString str, s;
	str.Format(_T("Step 11:\r\n\r\n"));
	s.Format(_T("Alignment is now complete.\r\n\r\n\r\n"));
	str += s;
	s.Format(_T("When through, click \"Finish\" to save results , exit the procedure and restore original instrument setup, \"Back\" to return to the previous step.\r\n"));
	str += s;
	m_editTrace2Cal.SetMargins(10,10);
	m_editTrace2Cal.SetWindowText(str);
}

void CScopeTrace2CalDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

#if 1
LONG CScopeTrace2CalDlg::OnSeekButtonValueChanged( UINT id, LONG value )
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

	if (m_pTcpThreadRxList->m_nNumberOfBoards > 1)
		m_pTcpThreadRxList->SetScopeTrace2CalData(m_nStep, m_nEditValue);
#endif
	return 0L;
}
#endif


void CScopeTrace2CalDlg::SaveCurrentCalChanges()
{
	m_OriginalConfigRec.OscopeRec.fw_shunt[m_nInstrument] = m_ValueStep[2];
	for (int i=0; i<4; i++)
	{
		m_OriginalConfigRec.OscopeRec.gate_zero[m_nInstrument][i] = m_ValueStep[3+i*2];
		m_OriginalConfigRec.OscopeRec.gate_FullScale[m_nInstrument][i] = m_ValueStep[4+i*2];
	}
}

void CScopeTrace2CalDlg::SaveOriginalConfig()
{
	m_OriginalConfigRec = m_pTcpThreadRxList->GetConfigRec();

	m_ValueStep[2] = m_OriginalConfigRec.OscopeRec.fw_shunt[m_nInstrument];
	for (int i=0; i<4; i++)
	{
		m_ValueStep[3+i*2] = m_OriginalConfigRec.OscopeRec.gate_zero[m_nInstrument][i];
		m_ValueStep[4+i*2] = m_OriginalConfigRec.OscopeRec.gate_FullScale[m_nInstrument][i];
	}
}

#pragma optimize( "g", off )    // add this to remove internal compiler error when building release version
bool CScopeTrace2CalDlg::RestoreOriginalConfig()
{
	m_pTcpThreadRxList->InitializeConfigRec(&m_OriginalConfigRec, TRUE);

	pCTscanDlg->SystemInit();

	pCTscanDlg->UpdateDlgs();

	return true;
}
#pragma optimize( "", on )

void CScopeTrace2CalDlg::ActionStep(int nStep)
{
	int i, j;
	int nChannel = m_nInstrument*MAX_CHANNEL_PER_INSTRUMENT;

	if (m_nStep == 3)
	{
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

				m_pTcpThreadRxList->SetGateTrigMode(i+nChannel, j, 1);
				m_pTcpThreadRxList->SetGatePolarity(i+nChannel, j, 0);

				m_pTcpThreadRxList->SetGateDetectMode(i+nChannel, j, 1);  //Fw

				// Set gate delay to 4 uS
				m_pTcpThreadRxList->SetGateDelay(i+nChannel, j, 4.0);

				// Set gate range to 4 uS
				m_pTcpThreadRxList->SetGateRange(i+nChannel, j, 4.0);
			}
		}
	}

	pCTscanDlg->ChangeActiveChannelTo(nChannel);
	pCTscanDlg->ChangeActiveGateTo(0);

	switch(nStep)
	{
	case 3:
		SetSysPulserOnOff(FALSE);  // turn pulser off
		SetSysRcvrOnOff(TRUE);     // turn receiver on
	case 4:
		pCTscanDlg->ChangeActiveGateTo(0);
		break;
	case 5:
	case 6:
		pCTscanDlg->ChangeActiveGateTo(1);
		break;
	case 7:
	case 8:
		pCTscanDlg->ChangeActiveGateTo(2);
		break;
	case 9:
	case 10:
		pCTscanDlg->ChangeActiveGateTo(3);
		break;
	default:
		return;
		break;
	}
	
	// Set scope trace 2 to display active gate only
	m_pTcpThreadRxList->SetScopeTrace2DisplaySel(nChannel, 0);

	pCTscanDlg->UpdateDlgs();

	m_pTcpThreadRxList->SetScopeTrace2CalData(2, m_ValueStep[2]);
	m_pTcpThreadRxList->SetScopeTrace2CalData(m_nStep, m_nEditValue);

}

void CScopeTrace2CalDlg::OnChangeEditValue() 
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
		m_pTcpThreadRxList->SetScopeTrace2CalData(m_nStep, m_nEditValue);
	m_bModified = true;

	SetSeekButton();
}

void CScopeTrace2CalDlg::SetSeekButton()
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
	case 11:   
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


void CScopeTrace2CalDlg::SetSysPulserOnOff(BOOL bOn)
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


void CScopeTrace2CalDlg::SetSysRcvrOnOff(BOOL bOn)
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


void CScopeTrace2CalDlg::OnSelchangeComboInstrument() 
{
	// TODO: Add your control notification handler code here
	m_nInstrument = (WORD) m_cbInstrument.GetCurSel();

	UpdateDlg();
}


void CScopeTrace2CalDlg::UpdateDlg()
{
	int nChnlBase = m_nInstrument * MAX_CHANNEL_PER_INSTRUMENT;
	int nMaxChnl = nChnlBase + MAX_CHANNEL_PER_INSTRUMENT;

	if ( (gChannel < nChnlBase) || (gChannel >= nMaxChnl) )
	{
		AfxMessageBox (_T("The active channel you selected is not in the current instrument you are calibrating.  The active channel will be set to the lowest channel number in the current instrument."));
		pCTscanDlg->ChangeActiveChannelTo(nChnlBase);
	}
}
