// TcgSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
#include "TcgSetupDlg.h"
#include "TscanDlg.h"

#include "..\include\cfg100.h"
#include "..\include\udp_msg.h"			//;  Instdata.h included

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int gChannel;   // active channel
extern int gGate;      // active gate
extern CPtrList plistUtData;  /* linked list holding the IData */
extern CONFIG_REC ConfigRec;

/////////////////////////////////////////////////////////////////////////////
// CTcgSetupDlg dialog


CTcgSetupDlg::CTcgSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTcgSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTcgSetupDlg)
	m_fGain = 0.0f;
	//}}AFX_DATA_INIT

	m_pDrawGates = new CDrawTraces(&m_btnDrawGates);
	m_pDrawGates->m_iTraceLength = 500;
	m_pDrawGates->m_sMinYValue = 0;
	m_pDrawGates->m_sMaxYValue = 100;
	//m_pDrawGates->DrawXYInit();

	m_nTcgMode = 0;
	m_nGateSel = 0;
	m_nCurveAdjust = -1;

	//m_iLastGateStart = 0;
	//m_iLastGateStop = 0;
	//m_iLastLevel = 0;
	//m_iLastAmp = 0;
	//m_iLastTof = 0;

	m_bDrawGateOnce = false;
	m_bDrawTcgCurveOnce = false;

	m_pUndo = new CONFIG_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
}


void CTcgSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTcgSetupDlg)
	DDX_Control(pDX, IDC_STATIC_TIMEHIGH, m_staticTimeHigh);
	DDX_Control(pDX, IDC_SCROLLBAR_GAIN, m_sbGain);
	DDX_Control(pDX, IDC_BTN_DRAWGATES, m_btnDrawGates);
	DDX_Text(pDX, IDC_EDIT_GAINDB, m_fGain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTcgSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CTcgSetupDlg)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_RADIO_TCGMODE1, OnRadioTcgmode1)
	ON_BN_CLICKED(IDC_RADIO_TCGMODE2, OnRadioTcgmode2)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_RESET, OnBtnReset)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_RADIO_TCGMODE3, OnRadioTcgmode3)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_RADIO_STEP200, OnRadioStep200)
	ON_BN_CLICKED(IDC_RADIO_STEP400, OnRadioStep400)
	ON_BN_CLICKED(IDC_RADIO_STEP600, OnRadioStep600)
	ON_BN_CLICKED(IDC_RADIO_STEP800, OnRadioStep800)
	ON_BN_CLICKED(IDC_RADIO_TRIGIF, OnRadioTrigIF)
	ON_BN_CLICKED(IDC_RADIO_TRIGIP, OnRadioTrigIP)
	ON_BN_CLICKED(IDC_RADIO_TRIGOFF, OnRadioTrigOff)
	ON_BN_CLICKED(IDC_BTN_UNDO, OnBtnUndo)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTcgSetupDlg message handlers

void CTcgSetupDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();

	KillTimer(100);

	delete m_pDrawGates;
	delete m_pUndo;

	SaveWindowPosition(_T("Tcg Setup"));
	((CTscanDlg*) GetParent())->m_pTcgSetupDlg = NULL;

	DestroyWindow();
}

void CTcgSetupDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();

	delete m_pDrawGates;
	delete m_pUndo;

	SaveWindowPosition(_T("Tcg Setup"));
	((CTscanDlg*) GetParent())->m_pTcgSetupDlg = NULL;

	DestroyWindow();
}

void CTcgSetupDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	//m_pTcpThreadRxList->StartTcpAcqDataThread();

	CDialog::PostNcDestroy();

	delete this;
}

BOOL CTcgSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
#if 0
	m_comboTcgTrig.SetCurSel(m_pTcpThreadRxList->GetTcgTrigSel(gChannel));
	m_comboTcgStep.SetCurSel(m_pTcpThreadRxList->GetTcgUpdateRate(gChannel));
	m_nGateSel = gGate;

	// set tcg curve length consistent with tcg step
	m_nTcgStep = m_pTcpThreadRxList->GetTcgUpdateRate(gChannel);
	m_pDrawGates->m_iTraceLength = 500 * (m_nTcgStep + 1);
	CString strTimeHigh;
	strTimeHigh.Format(_T("%d us"), (int) (50*(m_nTcgStep + 1)));
	m_staticTimeHigh.SetWindowText(strTimeHigh);

	CheckRadioButton(IDC_RADIO_TCGMODE1,IDC_RADIO_TCGMODE2,IDC_RADIO_TCGMODE1);
	CheckRadioButton(IDC_RADIO_Gate1,IDC_RADIO_Gate4,IDC_RADIO_Gate1+m_nGateSel);


	m_sbGain.SetScrollRange(GAINMIN,GAINMAX,TRUE);
	GetGainOfGate();
#endif

	m_sbGain.SetScrollRange(GAINMIN,GAINMAX,TRUE);

	m_nTcgMode = 0;
	CheckRadioButton(IDC_RADIO_TCGMODE1,IDC_RADIO_TCGMODE2,IDC_RADIO_TCGMODE1);

	UpdateDlg();

	SetTimer(100,500,NULL);

	RestoreWindowPosition(_T("Tcg Setup"));

	SetDefID(-1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CTcgSetupDlg::OnRadioTcgmode1() 
{
	// TODO: Add your control notification handler code here
	m_nTcgMode = 0;

	m_sbGain.EnableWindow(FALSE);

	int iChannel, iGate;

	iChannel = gChannel;
	iGate = m_nGateSel;
	//m_pTcpThreadRxList->SetTcgGainTable(0,iChannel, iGate, m_fGain, m_nTcgMode);
}

void CTcgSetupDlg::OnRadioTcgmode2() 
{
	// TODO: Add your control notification handler code here
	m_nTcgMode = 1;

	m_sbGain.EnableWindow(FALSE);

	int iChannel, iGate;

	iChannel = gChannel;
	iGate = m_nGateSel;
	//m_pTcpThreadRxList->SetTcgGainTable(0,iChannel, iGate, m_fGain, m_nTcgMode);
}

void CTcgSetupDlg::OnRadioTcgmode3() 
{
	// TODO: Add your control notification handler code here
	m_nTcgMode = 2;

	m_sbGain.EnableWindow(TRUE);

	int iChannel, iGate;

	iChannel = gChannel;
	iGate = m_nGateSel;
	//m_pTcpThreadRxList->SetTcgGainTable(0,iChannel, iGate, m_fGain, m_nTcgMode);
}


void CTcgSetupDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	m_btnDrawGates.UpdateWindow();
	m_pDrawGates->DrawFrame();
	m_bDrawGateOnce = false;
	m_bDrawTcgCurveOnce = false;
	DrawGate2(1);
	DrawGate2(2);
	DrawGate2(3);
	DrawGate2(4);
	DrawTcgCurve();
	m_bDrawGateOnce = true;

	// Do not call CDialog::OnPaint() for painting messages
}

void CTcgSetupDlg::DrawGate(int nGateNumber)
{
	short sTrace[2];
	m_pDrawGates->m_psTrace1 = &sTrace[0];

	int iTof, iAmp, iLevel;
	int iChannel, iGate;
	int i;

	iChannel = gChannel;
	iGate = nGateNumber - 1;
		m_pTcpThreadRxList->ReadAmplitudeTOF();
		iAmp = (int) m_pTcpThreadRxList->m_Amplitude[iChannel][iGate];
		if (iAmp <= 2) iAmp = 2;
		iTof = (int) (m_pTcpThreadRxList->m_TOF[iChannel][iGate] * 10);
		float Delay = m_pTcpThreadRxList->GetGateDelay(iChannel, iGate);
		float Range = m_pTcpThreadRxList->GetGateRange(iChannel, iGate);
		iLevel = (int) m_pTcpThreadRxList->GetGateLevel(iChannel, iGate);
		//iAmp = 25;
		//Delay = 10;
		//Range = 5;
		m_pDrawGates->m_sLastXValue = (int) (Delay*10);
		m_pDrawGates->m_sLastY1Value = iLevel;
		int x = (int) (Range*10);
		//m_pDrawGates->DrawGate(iGateStart, iGateStop, iLevel, iAmp, iTof, nGateNumber);
		for (i=0; i<x; i++)
		{
			m_pDrawGates->DrawNext(iLevel,nGateNumber);
		}
		//m_pDrawGates->m_sLastXValue = (int) (Delay*10+5);
		m_pDrawGates->m_sLastXValue = iTof - 1;
		for (i=0; i<3; i++)
		{
			m_pDrawGates->m_sLastY1Value = 0;
			m_pDrawGates->DrawNext(iAmp,nGateNumber);
		}

	m_pDrawGates->m_psTrace1 = NULL;
}

void CTcgSetupDlg::DrawGate2(int nGateNumber)
{
	int iTof, iAmp, iLevel;
	int iChannel, iGate;

	iChannel = gChannel;
	iGate = nGateNumber - 1;
	//m_pTcpThreadRxList->ReadAmplitudeTOF();
	iAmp = (int) m_pTcpThreadRxList->m_Amplitude[iChannel][iGate];
	if (iAmp <= 2) iAmp = 2;
	iTof = (int) (m_pTcpThreadRxList->m_TOF[iChannel][iGate] * 10);
	int nGateTrig = m_pTcpThreadRxList->GetGateTrigMode(iChannel, iGate);
	float Gate1Delay = m_pTcpThreadRxList->GetGateDelay(iChannel, 0);
	float Gate1Range = m_pTcpThreadRxList->GetGateRange(iChannel, 0);
	float Delay = m_pTcpThreadRxList->GetGateDelay(iChannel, iGate);
	if (nGateTrig == 2)
	{
		Delay += Gate1Delay + Gate1Range/2.0f;
	}
	float Range = m_pTcpThreadRxList->GetGateRange(iChannel, iGate);
	iLevel = (int) m_pTcpThreadRxList->GetGateLevel(iChannel, iGate);
	int iGateStart = (int) (Delay*10);
	int nTimeHigh = 500*(m_nTcgStep+1);
	if (iGateStart > nTimeHigh) iGateStart = nTimeHigh;
	int iGateStop = (int) ((Delay+Range)*10);
	if (iGateStop > nTimeHigh) iGateStop = nTimeHigh;
	iTof = (int) ((iGateStart+iGateStop)/2);
	if (m_bDrawGateOnce)
		m_pDrawGates->DrawGate(m_iLastGateStart[iGate], m_iLastGateStop[iGate], 
		                  m_iLastLevel[iGate], m_iLastAmp[iGate], m_iLastTof[iGate], nGateNumber);
	m_pDrawGates->DrawGate(iGateStart, iGateStop, iLevel, iAmp, iTof, nGateNumber);
	m_iLastGateStart[iGate] = iGateStart;
	m_iLastGateStop[iGate] = iGateStop;
	m_iLastLevel[iGate] = iLevel;
	m_iLastAmp[iGate] = iAmp;
	m_iLastTof[iGate] = iTof;
}

void CTcgSetupDlg::DrawTcgCurve()
{
	int i;
	int x, y;
	COLORREF curveClr = RGB(255, 0, 0);

	/* draw last TCG curve again to erase it */
	if (m_bDrawTcgCurveOnce)
	{
		for (i=0; i<256; i++)
		{
			if (i == 0)
			{
				m_pDrawGates->m_sLastXValue = (short) m_LastTcgGainTable[i].x;
				m_pDrawGates->m_sLastY1Value = (short) m_LastTcgGainTable[i].y;
			}
			else
				m_pDrawGates->DrawXYPointXORPen(m_LastTcgGainTable[i].x, m_LastTcgGainTable[i].y, curveClr);
		}
	}
	else
		m_bDrawTcgCurveOnce = true;

	/* draw current TCG curve */
	for (i=0; i<256; i++)
	{
		x = (int) (i*0.2f*(m_pTcpThreadRxList->GetTcgUpdateRate(gChannel)+1)/0.1f);

		if (gChannel % 2 == 0)
			y = (int) ((m_pTcpThreadRxList->m_bufGainTable[i]&0x0000FFFF)/10);
		else
			y = (int) (((m_pTcpThreadRxList->m_bufGainTable[i]&0xFFFF0000)>>16)/10);

		if (i == 0)
		{
			m_pDrawGates->m_sLastXValue = x;
			m_pDrawGates->m_sLastY1Value = y;
		}
		else
			m_pDrawGates->DrawXYPointXORPen(x, y, curveClr);

		m_LastTcgGainTable[i].x = x;
		m_LastTcgGainTable[i].y = y;
	}
}

void CTcgSetupDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	// CODE IS DIRECTION OF SCROLL MOTION
	//KillTimer(100);

	if (m_nTcgMode != 2)
		return;

	if (m_pTcpThreadRxList->GetTcgTrigSel(gChannel) == 0)
		return;

	int nDelta;		// where the bar started and amount of change
	int nMax, nMin;
	int nStart;

	nMin=GAINMIN;
	nMax=GAINMAX;

	nStart = pScrollBar->GetScrollPos();

	switch ( nSBCode)
		{	// switch on type of motion
	case SB_LINELEFT:
		nDelta = -1;
		break;

	case SB_LINERIGHT:
		nDelta = 1;
		break;

	case SB_PAGELEFT:
		nDelta = -10;	//-(nMax - nMin)/10;
		break;

	case SB_PAGERIGHT:
		nDelta = 10;	//(nMax - nMin)/10;
		break;

	case SB_THUMBTRACK:
		nDelta = (int)nPos - nStart;
		break;


	default:
		nDelta = 0;		//need a value 8-27-2001
		return;
		break;

		}	// switch on type of motion

	nStart += nDelta;
	if ( nStart > nMax) nStart = nMax;
	if ( nStart < nMin) nStart = nMin;


	pScrollBar->SetScrollPos(nStart, TRUE);

	m_fGain = (float) nStart / ((float) m_pTcpThreadRxList->m_TruscopGainScaling);
	UpdateData(FALSE);

	int iChannel, iGate;
	float fGain;

	fGain = m_fGain - m_pTcpThreadRxList->m_TruscopGainOffset/m_pTcpThreadRxList->m_TruscopGainScaling;

	iChannel = gChannel;
	iGate = m_nGateSel;
	m_pTcpThreadRxList->SetTcgGainTable(0,iChannel, iGate, fGain, m_nTcgMode);

	//Invalidate(false);
	m_btnDrawGates.UpdateWindow();
	DrawGate2(1);
	DrawGate2(2);
	DrawGate2(3);
	DrawGate2(4);
	
	DrawTcgCurve();
	//SetTimer(100, 100, NULL);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTcgSetupDlg::OnBtnReset() 
{
	// TODO: Add your control notification handler code here

	m_pTcpThreadRxList->ReSetTcgGainTable(gChannel, TRUE);

	Invalidate(false);

	//DrawTcgCurve();
}

void CTcgSetupDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	//InvalidateRect(NULL,false);
	m_btnDrawGates.UpdateWindow();

	//m_pTcpThreadRxList->ReadAmplitudeTOF();

	DrawGate2(1);
	DrawGate2(2);
	DrawGate2(3);
	DrawGate2(4);

	//GetGainOfGate();

	CDialog::OnTimer(nIDEvent);
}


BOOL CTcgSetupDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	//return true;
	return CDialog::OnEraseBkgnd(pDC);
}

void CTcgSetupDlg::GetGainOfGate()
{
	m_nGateSel = gGate;

	int iStart = (int) ( (m_pTcpThreadRxList->GetGateDelay(gChannel,m_nGateSel)+0.5f*m_pTcpThreadRxList->GetGateRange(gChannel,m_nGateSel))/
		((m_pTcpThreadRxList->GetTcgUpdateRate(gChannel)+1)*0.2));
	if (iStart < 0) iStart = 0;
	if (iStart > 255) iStart = 255;
	int GainCounts;

	if ( (gChannel%2) == 0 )
		GainCounts = m_pTcpThreadRxList->m_bufGainTable[iStart] & 0x0000FFFF;
	else
		GainCounts = (m_pTcpThreadRxList->m_bufGainTable[iStart] & 0xFFFF0000) >> 16;
	//m_fGain = (float) (GainCounts - m_pTcpThreadRxList->m_GainOffset)/((float) m_pTcpThreadRxList->m_GainScaling);
	m_fGain = m_pTcpThreadRxList->m_pConfigRec->receiver.TcgRec[gChannel].GateGain[gGate];
	GainCounts = (int) (m_fGain*m_pTcpThreadRxList->m_TruscopGainScaling + m_pTcpThreadRxList->m_TruscopGainOffset);
	m_fGain += m_pTcpThreadRxList->m_TruscopGainOffset/m_pTcpThreadRxList->m_TruscopGainScaling;
	m_sbGain.SetScrollPos(GainCounts,TRUE);

	UpdateData(false);
}


void CTcgSetupDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ClientToScreen(&point);
	m_ptGainChangeStart = point;
	CRect rect;
	m_btnDrawGates.GetWindowRect(&rect);
	if (rect.PtInRect(point))
	{
		if (point.x < (rect.Width()/5 + rect.left))
			m_nCurveAdjust = 0;
		else if (point.x > (rect.right - rect.Width()/5))
			m_nCurveAdjust = 1;
		else if ((point.x > (rect.Width()*2/5 + rect.left)) && (point.x < (rect.Width()*3/5 + rect.left)))
			m_nCurveAdjust = 2;
		else if ((point.x > (rect.Width()/5 + rect.left)) && (point.x < (rect.Width()*2/5 + rect.left)))
			m_nCurveAdjust = 3;
		else
			m_nCurveAdjust = 4;
	}

		//AfxMessageBox(_T("Left Button down"));
	 
	CDialog::OnLButtonDown(nFlags, point);
}


void CTcgSetupDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	float GainChange;
	int iChannel, iGate;

	if (m_pTcpThreadRxList->GetTcgTrigSel(gChannel) != 0)
	{
		if ((nFlags & MK_LBUTTON) && (m_nTcgMode != 2) && (m_nCurveAdjust >= 0) && (m_nCurveAdjust <= 4))
		{
			ClientToScreen(&point);
			CRect rect;
			m_btnDrawGates.GetWindowRect(&rect);
			GainChange = (float) ( 0.1f*(point.y - m_ptGainChangeStart.y) );
			if ( m_nTcgMode == 0 )
				m_ptGainChangeStart.y = point.y;

			iChannel = gChannel;
			iGate = m_nGateSel;
			m_pTcpThreadRxList->SetTcgGainTable(m_nCurveAdjust,iChannel, iGate, GainChange, m_nTcgMode);
			DrawTcgCurve();
		}

		if ((nFlags & MK_LBUTTON) && m_nTcgMode == 2)
		{
			float fGain;
			ClientToScreen(&point);
			GainChange = (float) ( 0.1f*(point.y - m_ptGainChangeStart.y) );
			m_ptGainChangeStart.y = point.y;

			GetGainOfGate();

			m_fGain -= GainChange;
			fGain = m_fGain - m_pTcpThreadRxList->m_TruscopGainOffset/m_pTcpThreadRxList->m_TruscopGainScaling;
			if (fGain < 0) fGain = 0.0f;
			if (fGain > 79.9) fGain = 79.9f;

			iChannel = gChannel;
			iGate = m_nGateSel;
			m_pTcpThreadRxList->SetTcgGainTable(0,iChannel, iGate, fGain, m_nTcgMode);
			DrawTcgCurve();
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}


void CTcgSetupDlg::UpdateDlg()
{
	m_pTcpThreadRxList->ComputeTcgGainTable(gChannel);
	m_pTcpThreadRxList->ComputeGateTcgGain(gChannel, gGate);

	m_nGateSel = gGate;

	// set tcg curve length consistent with tcg step
	m_nTcgStep = m_pTcpThreadRxList->GetTcgUpdateRate(gChannel);
	m_pDrawGates->m_iTraceLength = 500 * (m_nTcgStep + 1);
	CString strTimeHigh;
	strTimeHigh.Format(_T("%d us"), (int) (50*(m_nTcgStep + 1)));
	m_staticTimeHigh.SetWindowText(strTimeHigh);

	// check trigger radio button
	int nTrigSel = m_pTcpThreadRxList->GetTcgTrigSel(gChannel);
	CheckRadioButton(IDC_RADIO_TRIGOFF, IDC_RADIO_TRIGIF, IDC_RADIO_TRIGOFF+nTrigSel);

	// check step radio button
	CheckRadioButton(IDC_RADIO_STEP200, IDC_RADIO_STEP800, IDC_RADIO_STEP200+m_nTcgStep);

	CheckRadioButton(IDC_RADIO_Gate1,IDC_RADIO_Gate4,IDC_RADIO_Gate1+m_nGateSel);

	GetGainOfGate();

	Invalidate(false);
}


void CTcgSetupDlg::SaveWindowPosition(LPCTSTR lpszProfileName)
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

void CTcgSetupDlg::RestoreWindowPosition(LPCTSTR lpszProfileName)
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

BOOL CTcgSetupDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	float GainChange;
	int iChannel, iGate;

	if (m_pTcpThreadRxList->GetTcgTrigSel(gChannel) != 0)
	{
		m_nCurveAdjust = -1;

		//AfxMessageBox("OnMouseWheel");

		//ClientToScreen(&pt);
		CRect rect;
		m_btnDrawGates.GetWindowRect(&rect);
		if (rect.PtInRect(pt))
		{
			if (pt.x < (rect.Width()/5 + rect.left))
				m_nCurveAdjust = 0;
			else if (pt.x > (rect.right - rect.Width()/5))
				m_nCurveAdjust = 1;
			else if ((pt.x > (rect.Width()*2/5 + rect.left)) && (pt.x < (rect.Width()*3/5 + rect.left)))
				m_nCurveAdjust = 2;
			else if ((pt.x > (rect.Width()/5 + rect.left)) && (pt.x < (rect.Width()*2/5 + rect.left)))
				m_nCurveAdjust = 3;
			else
				m_nCurveAdjust = 4;
		}

		if ((m_nTcgMode != 2) && (m_nCurveAdjust >= 0) && (m_nCurveAdjust <= 4))
		{
			GainChange = -0.5f*(zDelta/WHEEL_DELTA);

			iChannel = gChannel;
			iGate = m_nGateSel;
			m_pTcpThreadRxList->SetTcgGainTable(m_nCurveAdjust,iChannel, iGate, GainChange, m_nTcgMode);
			DrawTcgCurve();
		}
		
		if (m_nTcgMode == 2)
		{
			float fGain;
			GainChange = -0.5f*(zDelta/WHEEL_DELTA);
			GetGainOfGate();

			m_fGain -= GainChange;
			fGain = m_fGain - m_pTcpThreadRxList->m_TruscopGainOffset/m_pTcpThreadRxList->m_TruscopGainScaling;
			if (fGain < 0) fGain = 0.0f;
			if (fGain > 79.9) fGain = 79.9f;

			iChannel = gChannel;
			iGate = m_nGateSel;
			m_pTcpThreadRxList->SetTcgGainTable(0,iChannel, iGate, fGain, m_nTcgMode);
			DrawTcgCurve();
		}
	}
	
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CTcgSetupDlg::KillMe()
{
	OnOK();
}

void CTcgSetupDlg::OnRadioStep200() 
{
	// TODO: Add your control notification handler code here
	int nStep = GetCheckedRadioButton(IDC_RADIO_STEP200, IDC_RADIO_STEP800) - IDC_RADIO_STEP200;
	if ( nStep != 0) return;

	m_pTcpThreadRxList->SetTcgUpdateRate(gChannel, 0);
	
	// set tcg curve length consistent with tcg step
	m_nTcgStep = m_pTcpThreadRxList->GetTcgUpdateRate(gChannel);
	m_pDrawGates->m_iTraceLength = 500 * (m_nTcgStep + 1);

	CString strTimeHigh;
	strTimeHigh.Format(_T("%d us"), (int) (50*(m_nTcgStep + 1)));
	m_staticTimeHigh.SetWindowText(strTimeHigh);

	//OnBtnReset();
	UpdateDlg();
}

void CTcgSetupDlg::OnRadioStep400() 
{
	// TODO: Add your control notification handler code here
	int nStep = GetCheckedRadioButton(IDC_RADIO_STEP200, IDC_RADIO_STEP800) - IDC_RADIO_STEP200;
	if ( nStep != 1) return;

	m_pTcpThreadRxList->SetTcgUpdateRate(gChannel, 1);
	
	// set tcg curve length consistent with tcg step
	m_nTcgStep = m_pTcpThreadRxList->GetTcgUpdateRate(gChannel);
	m_pDrawGates->m_iTraceLength = 500 * (m_nTcgStep + 1);

	CString strTimeHigh;
	strTimeHigh.Format(_T("%d us"), (int) (50*(m_nTcgStep + 1)));
	m_staticTimeHigh.SetWindowText(strTimeHigh);

	//OnBtnReset();
	UpdateDlg();
}

void CTcgSetupDlg::OnRadioStep600() 
{
	// TODO: Add your control notification handler code here
	int nStep = GetCheckedRadioButton(IDC_RADIO_STEP200, IDC_RADIO_STEP800) - IDC_RADIO_STEP200;
	if ( nStep != 2) return;

	m_pTcpThreadRxList->SetTcgUpdateRate(gChannel, 2);
	
	// set tcg curve length consistent with tcg step
	m_nTcgStep = m_pTcpThreadRxList->GetTcgUpdateRate(gChannel);
	m_pDrawGates->m_iTraceLength = 500 * (m_nTcgStep + 1);

	CString strTimeHigh;
	strTimeHigh.Format(_T("%d us"), (int) (50*(m_nTcgStep + 1)));
	m_staticTimeHigh.SetWindowText(strTimeHigh);

	//OnBtnReset();
	UpdateDlg();
}

void CTcgSetupDlg::OnRadioStep800() 
{
	// TODO: Add your control notification handler code here
	int nStep = GetCheckedRadioButton(IDC_RADIO_STEP200, IDC_RADIO_STEP800) - IDC_RADIO_STEP200;
	if ( nStep != 3) return;

	m_pTcpThreadRxList->SetTcgUpdateRate(gChannel, 3);
	
	// set tcg curve length consistent with tcg step
	m_nTcgStep = m_pTcpThreadRxList->GetTcgUpdateRate(gChannel);
	m_pDrawGates->m_iTraceLength = 500 * (m_nTcgStep + 1);

	CString strTimeHigh;
	strTimeHigh.Format(_T("%d us"), (int) (50*(m_nTcgStep + 1)));
	m_staticTimeHigh.SetWindowText(strTimeHigh);

	//OnBtnReset();
	UpdateDlg();
}

void CTcgSetupDlg::OnRadioTrigIF() 
{
	// TODO: Add your control notification handler code here
	int nTrig = GetCheckedRadioButton(IDC_RADIO_TRIGOFF, IDC_RADIO_TRIGIF) - IDC_RADIO_TRIGOFF;
	if (nTrig != 2) return;

	unsigned short nOldTrig = m_pTcpThreadRxList->GetTcgTrigSel(gChannel);

	m_pTcpThreadRxList->SetTcgTrigSel(gChannel, 2);

	if (nOldTrig == 0)
	{
		m_pTcpThreadRxList->SendTcgFunction(gChannel);
		UpdateDlg();
	}
}

void CTcgSetupDlg::OnRadioTrigIP() 
{
	// TODO: Add your control notification handler code here
	int nTrig = GetCheckedRadioButton(IDC_RADIO_TRIGOFF, IDC_RADIO_TRIGIF) - IDC_RADIO_TRIGOFF;
	if (nTrig != 1) return;

	unsigned short nOldTrig = m_pTcpThreadRxList->GetTcgTrigSel(gChannel);

	m_pTcpThreadRxList->SetTcgTrigSel(gChannel, 1);

	if (nOldTrig == 0)
	{
		m_pTcpThreadRxList->SendTcgFunction(gChannel);
		UpdateDlg();
	}
}

void CTcgSetupDlg::OnRadioTrigOff() 
{
	// TODO: Add your control notification handler code here
	int nTrig = GetCheckedRadioButton(IDC_RADIO_TRIGOFF, IDC_RADIO_TRIGIF) - IDC_RADIO_TRIGOFF;
	if (nTrig != 0) return;

	m_pTcpThreadRxList->SetTcgTrigSel(gChannel, 0);	
	
}

void CTcgSetupDlg::OnBtnUndo() 
{
	// TODO: Add your control notification handler code here
	unsigned short nOldTrig;
	unsigned short nOldStep;
	unsigned short nNewTrig;
	unsigned short nNewStep;
	int result;

	if (MessageBox(	_T("Undo all TCG changes since last SAVE operation?"),
			_T("Restore Configuration"),MB_YESNO)== IDYES)
	{
		//memcpy ( (void *) &ConfigRec, (void *) m_pUndo, sizeof(CONFIG_REC));
		for (int i=0; i<MAX_CHANNEL; i++)
		{
			nOldTrig = m_pTcpThreadRxList->GetTcgTrigSel(i);
			nOldStep = m_pTcpThreadRxList->GetTcgUpdateRate(i);

			nNewTrig = m_pUndo->receiver.tcg_trigger[i];
			nNewStep = m_pUndo->receiver.tcg_step[i];

			(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[i] = m_pUndo->receiver.TcgRec[i];
			(m_pTcpThreadRxList->m_pConfigRec)->receiver.tcg_trigger[i] = m_pUndo->receiver.tcg_trigger[i];
			(m_pTcpThreadRxList->m_pConfigRec)->receiver.tcg_step[i] = m_pUndo->receiver.tcg_step[i];

			if ( (nOldTrig != nNewTrig) || (nOldStep != nNewStep) )
				m_pTcpThreadRxList->WriteTcgStepTrigSel(i);

			result = memcmp ( (void *) &( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[i] ),
							  (void *) &( m_pUndo->receiver.TcgRec[i] ), sizeof(TCG_REC) );
			if ( (m_pTcpThreadRxList->GetTcgTrigSel(i) != 0) && (result != 0) )
				m_pTcpThreadRxList->SendTcgFunction(i);
		}
	}

	UpdateDlg();
}

void CTcgSetupDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	CMenu Menu;
	CPoint pt;
	CRect rect;

	// Preserve the system menu properties
	pt = point;
	GetClientRect(&rect);
	ScreenToClient(&pt);

	// Was the mouse clicked on the client area or system area?
	if ( rect.PtInRect (pt) )
	{	// client area
		Menu.LoadMenu(IDR_MENU_TCGGATEOFF);

		CMenu *pContextMenu = Menu.GetSubMenu(0);
		int nCmd = pContextMenu->TrackPopupMenu( TPM_LEFTALIGN |
						TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
						point.x, point.y, this, NULL);

		switch (nCmd)
		{
		case ID_MENU_GATE1OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[0] != 0 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[0] = 0;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[0] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[0] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[0];

			}
			break;

		case ID_MENU_GATE2OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[1] != 0 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[1] = 0;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[1] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[1] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[1];

			}
			break;

		case ID_MENU_GATE3OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[2] != 0 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[2] = 0;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[2] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[2] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[2];

			}
			break;

		case ID_MENU_GATE4OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[3] != 0 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[3] = 0;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[3] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[3] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[3];

			}
			break;

		case ID_MENU_UNDOGATE1OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[0] == 1 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[0] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[0] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[0];

			}
			break;

		case ID_MENU_UNDOGATE2OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[1] == 1 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[1] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[1] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[1];

			}
			break;

		case ID_MENU_UNDOGATE3OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[2] == 1 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[2] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[2] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[2];

			}
			break;

		case ID_MENU_UNDOGATE4OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[3] == 1 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[3] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[3] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[3];

			}
			break;

		case ID_MENU_CURVE2OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].Curve2On != 0 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].Curve2On = 0;
			}
			break;

		default:
			return;
			break;
		}

		UpdateDlg();
		m_pTcpThreadRxList->SendTcgFunction(gChannel);
	}
	else
		CWnd::OnContextMenu(pWnd,point);	// system area
}
