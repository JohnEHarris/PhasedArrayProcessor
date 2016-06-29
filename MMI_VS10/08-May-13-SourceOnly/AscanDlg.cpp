// AscanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "AscanDlg.h"
#include "TscanDlg.h"
#include "math.h"
#include "stdlib.h"
#include "afxmt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ASCAN_TIMER 1000
#define GATES_LEVEL_TIMER 1011

extern CPtrList g_pTcpListAscan;
extern CCriticalSection g_CriticalSectionAscan;
extern int gChannel;   // active channel
extern int gGate;      // active gate
extern int gSysInitFlag;
extern CONFIG_REC ConfigRec;
extern SITE_SPECIFIC_DEFAULTS SiteDefault;

CSCAN_REVOLUTION g_RawFlawCscan[2];
int  g_nRawFlawBuffer = 0;
RAW_WALL_HEAD       g_RawFlawHead;
CSCAN_REVOLUTION    g_RawFlaw[NUM_MAX_REVOL];

/////////////////////////////////////////////////////////////////////////////
// CAscanDlg dialog


CAscanDlg::CAscanDlg(CWnd* pParent /*=NULL*/)
	: CResizingDialog(CAscanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAscanDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	SetControlInfo(IDC_BTN_ASCAN, RESIZE_BOTH);
	SetControlInfo(IDC_STATIC_DELAY, ANCHORE_LEFT | ANCHORE_BOTTOM);
	SetControlInfo(IDC_STATIC_HRIGHT, ANCHORE_RIGHT | ANCHORE_BOTTOM);
	SetControlInfo(IDC_EXPAND, ANCHORE_BOTTOM);
	SetControlInfo(IDC_FROZEN, ANCHORE_BOTTOM);
	SetControlInfo(IDC_STATIC_ASCANUNIT, ANCHORE_BOTTOM);
	SetControlInfo(IDC_EXPAND_BORDER, ANCHORE_BOTTOM);
	SetControlInfo(IDC_STATIC_MODE, ANCHORE_BOTTOM);
	SetControlInfo(IDC_STATIC_TRIG, ANCHORE_BOTTOM);
	SetControlInfo(IDC_STATIC_STEP, ANCHORE_BOTTOM);
	SetControlInfo(IDC_RADIO_TCGMODE1, ANCHORE_BOTTOM);
	SetControlInfo(IDC_RADIO_TCGMODE2, ANCHORE_BOTTOM);
	SetControlInfo(IDC_RADIO_TCGMODE3, ANCHORE_BOTTOM);
	SetControlInfo(IDC_RADIO_TRIGOFF, ANCHORE_BOTTOM);
	SetControlInfo(IDC_RADIO_TRIGIP, ANCHORE_BOTTOM);
	SetControlInfo(IDC_RADIO_TRIGIF, ANCHORE_BOTTOM);
	SetControlInfo(IDC_RADIO_STEP200, ANCHORE_BOTTOM);
	SetControlInfo(IDC_RADIO_STEP400, ANCHORE_BOTTOM);
	SetControlInfo(IDC_RADIO_STEP600, ANCHORE_BOTTOM);
	SetControlInfo(IDC_RADIO_STEP800, ANCHORE_BOTTOM);
	SetControlInfo(IDC_BTN_RESET, ANCHORE_BOTTOM);

	m_pDrawAscan = new CDrawTraces(&m_btnDrawAscan);
	m_pDrawAscan->m_iTraceLength = 500;
	m_pDrawAscan->m_sMinYValue = 0;
	m_pDrawAscan->m_sMaxYValue = 100;
	//m_pDrawGates->DrawXYInit();

	m_bDrawAscanOnce = false;

	for (int i=0; i<500; i++)
		m_nAscan[i] = 0;

	m_nAscanBaseLine  = 0;

	m_sDefFileDir = _T("C:\\PhasedArray\\Ascan");

	m_bkColor = RGB(0, 0, 0);
	m_traceColor = RGB(255, 255, 255);

	for (int j=0; j<4; j++)
		m_AscanGates.bGateSelect[j] = 0;

	m_bDrawTcgCurve = FALSE;

	 m_nNormalHeight = 0;
	 m_nExpandedHeight = 0;
	 m_bExpanded = TRUE;

	 m_bRememberSize = FALSE;

	 m_nTcgMode = 0;
	 m_bShowTcgCurve = FALSE;
	 m_bShowBscan = FALSE;
	 m_bGrayscale = FALSE;
	 m_bLinearScale = TRUE;

	 m_nLiveFrozenPeak = 0;

	 m_nRefreshRate = ConfigRec.receiver.ascan_refreshrate;
	 m_bBcastAscan = ConfigRec.receiver.ascan_broadcast;

	 m_bOldPipePresent = FALSE;
	 m_nFlawRevCnt = 1;
}


void CAscanDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAscanDlg)
	DDX_Control(pDX, IDC_EXPAND_BORDER, m_Devide);
	DDX_Control(pDX, IDC_BTN_ASCAN, m_btnDrawAscan);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAscanDlg, CResizingDialog)
	//{{AFX_MSG_MAP(CAscanDlg)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_EXPAND, OnExpand)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RADIO_STEP200, OnRadioStep200)
	ON_BN_CLICKED(IDC_RADIO_STEP400, OnRadioStep400)
	ON_BN_CLICKED(IDC_RADIO_STEP600, OnRadioStep600)
	ON_BN_CLICKED(IDC_RADIO_STEP800, OnRadioStep800)
	ON_BN_CLICKED(IDC_RADIO_TCGMODE1, OnRadioTcgmode1)
	ON_BN_CLICKED(IDC_RADIO_TCGMODE2, OnRadioTcgmode2)
	ON_BN_CLICKED(IDC_RADIO_TCGMODE3, OnRadioTcgmode3)
	ON_BN_CLICKED(IDC_RADIO_TRIGIF, OnRadioTrigIf)
	ON_BN_CLICKED(IDC_RADIO_TRIGIP, OnRadioTrigIp)
	ON_BN_CLICKED(IDC_RADIO_TRIGOFF, OnRadioTrigOff)
	ON_BN_CLICKED(IDC_BTN_RESET, OnBtnReset)
	ON_BN_CLICKED(IDC_FROZEN, OnFrozen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAscanDlg message handlers

void CAscanDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CResizingDialog::PostNcDestroy();

	delete this;
}

void CAscanDlg::OnCancel() 
{
	return;
	// TODO: Add extra cleanup here
	m_pTcpThreadRxList->SendSlaveMsg(ASCAN_MODE, 0,0,0,0,0,0);
	delete m_pDrawAscan;

	SaveWindowPosition(_T("A-Scan"));
	((CTscanDlg*) GetParent())->m_pAscanDlg = NULL;

	DestroyWindow();
}

void CAscanDlg::OnOK() 
{
	return;
	// TODO: Add extra validation here
	m_pTcpThreadRxList->SendSlaveMsg(ASCAN_MODE, 0,0,0,0,0,0);
	delete m_pDrawAscan;

	SaveWindowPosition(_T("A-Scan"));
	((CTscanDlg*) GetParent())->m_pAscanDlg = NULL;

	DestroyWindow();
}

void CAscanDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	m_btnDrawAscan.UpdateWindow();
	//m_pDrawAscan->DrawFrame();
	m_bDrawAscanOnce = false;
	DrawAscan();
	m_bDrawAscanOnce = true;
	
	// Do not call CResizingDialog::OnPaint() for painting messages
}

#if 0
void CAscanDlg::DrawAscan()
{
#if 1
	int i;
	int x, y;
	COLORREF curveClr = RGB(255, 255, 255);

	/* draw last A-Scan again to erase it */
	if (m_bDrawAscanOnce)
	{
		for (i=0; i<500; i++)
		{
			if (i == 0)
			{
				m_pDrawAscan->m_sLastXValue = (short) m_LastAscan[i].x;
				m_pDrawAscan->m_sLastY1Value = (short) m_LastAscan[i].y;
			}
			else
				m_pDrawAscan->DrawXYPointXORPen(m_LastAscan[i].x, m_LastAscan[i].y, curveClr);
		}
	}
	else
		m_bDrawAscanOnce = true;

	/* draw current A-Scan*/
	for (i=0; i<500; i++)
	{
		x = i;

		y = m_nAscan[i];

		if (i == 0)
		{
			m_pDrawAscan->m_sLastXValue = x;
			m_pDrawAscan->m_sLastY1Value = y;
		}
		else
			m_pDrawAscan->DrawXYPointXORPen(x, y, curveClr);

		m_LastAscan[i].x = x;
		m_LastAscan[i].y = y;
	}
#endif
}
#endif

#if 0
void CAscanDlg::DrawAscan()
{
	int i;
	int x, y;
	COLORREF curveClr = RGB(255, 255, 255);
	CPoint ptAscan[512];

	/* draw last A-Scan again to erase it */
	if (m_bDrawAscanOnce)
	{
		//m_pDrawAscan->DrawPolylineXORPen(m_LastAscan, 500, RGB(0,0,0), FALSE);
	}
	else
		m_bDrawAscanOnce = true;

	/* draw current A-Scan*/
	for (i=0; i<500; i++)
	{
		ptAscan[i].x = x = i;

		y = m_nAscan[i];
		if (y < 0) y = 0;
		if (y > 100) y = 100;

		ptAscan[i].y = y;
	}

	m_pDrawAscan->DrawPolylineXORPen(ptAscan, 500, curveClr, TRUE);

	for (i=0; i<500; i++)
		m_LastAscan[i] = ptAscan[i];
}
#endif

#if 1
void CAscanDlg::DrawAscan()
{
	int i;
	int x, y;
	COLORREF curveClr = RGB(255, 255, 255);
	CPoint ptAscan[1024];
	BOOL bRF = ConfigRec.receiver.det_option[gChannel];
	int nCount;

	nCount = m_pDrawAscan->m_iTraceLength;

	/* draw current A-Scan*/
	for (i=0; i<nCount; i++)
	{
		ptAscan[i].x = x = i;

		if (bRF)
		{
			if (ConfigRec.gates.polarity[gChannel][gGate])  //minus polarity
				y =  m_nAscanBaseLine - m_nAscan[i];  //invert
			else
				y = m_nAscan[i] + m_nAscanBaseLine;
		}
		else
			y = m_nAscan[i] + m_nAscanBaseLine;
		if (y < 0) y = 0;
		if (y > 100) y = 100;

		ptAscan[i].y = y;
	}

	CPoint TcgGainTable[256];
	for (i=0; i<256; i++)
	{
		TcgGainTable[i].x = m_TcgGainTable[i].x;
		TcgGainTable[i].y = m_TcgGainTable[i].y;
	}
	m_pDrawAscan->DrawAscanBscan(ptAscan, nCount, m_bkColor, m_traceColor, TRUE, 
									m_nAscanBaseLine, &m_AscanGates, TcgGainTable, 256, 
									m_bDrawTcgCurve, m_bShowBscan, m_bGrayscale, m_bLinearScale,
									m_bClearBscan, m_nLiveFrozenPeak);
	m_bClearBscan = FALSE;
}
#endif

void CAscanDlg::SaveWindowPosition(LPCTSTR lpszProfileName)
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
    pApp->WriteProfileInt(lpszProfileName, _T("Width"), rect.Width());
	if (m_bExpanded)
		pApp->WriteProfileInt(lpszProfileName, _T("Height"), rect.Height());
	else
		pApp->WriteProfileInt(lpszProfileName, _T("Height"), rect.Height()+m_nHeightDiff);

    pApp->WriteProfileInt(lpszProfileName, _T("Trace Color"), m_nTraceColor);
	pApp->WriteProfileInt(lpszProfileName, _T("Background Color"), m_nBkColor);
}

void CAscanDlg::RestoreWindowPosition(LPCTSTR lpszProfileName)
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
	int cx, cy, width, height;

    cx = pApp->GetProfileInt(lpszProfileName, _T("Left"), x);
    cy = pApp->GetProfileInt(lpszProfileName, _T("Top"), y);
    width = pApp->GetProfileInt(lpszProfileName, _T("Width"), rect.Width());
    height = pApp->GetProfileInt(lpszProfileName, _T("Height"), rect.Height());

	MoveWindow(cx, cy, width, height);

	m_nTraceColor = pApp->GetProfileInt(lpszProfileName, _T("Trace Color"), 0);
	switch(m_nTraceColor)
	{
	case 0:  //white
		m_traceColor = RGB(255, 255, 255);
		break;

	case 1:  //black
		m_traceColor = RGB(0, 0, 0);
		break;

	case 2:  //red
		m_traceColor = RGB(255, 0, 0);
		break;

	case 3:  //green
		m_traceColor = RGB(0, 255, 0);
		break;

	case 4:  //blue
		m_traceColor = RGB(0, 0, 255);
		break;

	default:
		m_traceColor = RGB(255, 255, 255);
		break;
	}

	m_nBkColor = pApp->GetProfileInt(lpszProfileName, _T("Background Color"), 0);
	switch(m_nBkColor)
	{
	case 0:  //black
		m_bkColor = RGB(0, 0, 0);
		break;

	case 1:  //white
		m_bkColor = RGB(255, 255, 255);
		break;

	default:
		m_bkColor = RGB(0, 0, 0);
		break;
	}
}

BOOL CAscanDlg::OnInitDialog() 
{
	CResizingDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	RestoreWindowPosition(_T("A-Scan"));

	SetTimer(ASCAN_TIMER,50,NULL);

	if ( ConfigRec.receiver.det_option[gChannel] == 1 )  //RF
		m_pDrawAscan->m_iTraceLength = 1000;
	else
		m_pDrawAscan->m_iTraceLength = 500;

	UpdateDlg();

	m_pDrawAscan->DrawFrame();

	ContractDialog();

	CString s;
	if (m_nLiveFrozenPeak == 0) //show live signal
		s.Format(_T("Live"));
	if (m_nLiveFrozenPeak == 1) //show frozen signal
		s.Format(_T("Frozen"));
	if (m_nLiveFrozenPeak == 2) //show peak signal
		s.Format(_T("Peak"));
	GetDlgItem(IDC_FROZEN)->SetWindowText(s);

	SetWindowPos (&wndTopMost, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

	SetDefID(-1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAscanDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == GATES_LEVEL_TIMER)
	{
		KillTimer(GATES_LEVEL_TIMER);
		SendMsg(GATES_LEVEL);
		CResizingDialog::OnTimer(nIDEvent);
		return;
	}

	I_MSG_ASCAN *pImsgAscan;
	I_MSG_RAW_FLAW *pRawFlaw;
	int i;
	int nAscan[1024], iAscan;
	BOOL bDraw = FALSE;
	BOOL bRF = ConfigRec.receiver.det_option[gChannel];

	for (i=0; i<1000; i++)
	{
		if (m_nLiveFrozenPeak == 0) //show live signal
			nAscan[i] = -128;
		else if (m_nLiveFrozenPeak == 2) //show peak signal
			nAscan[i] = m_nAscan[i];
	}

	CPtrList pTcpListAscan;
	I_MSG_RUN *pImsgRun;
    g_CriticalSectionAscan.Lock();
    while (!g_pTcpListAscan.IsEmpty() )
    {
		pImsgRun = (I_MSG_RUN *) g_pTcpListAscan.RemoveHead();
		pTcpListAscan.AddTail((void *) pImsgRun);
	}
	g_CriticalSectionAscan.Unlock();


    while (!pTcpListAscan.IsEmpty() )
    {
		bDraw = TRUE;
        pImsgAscan = (I_MSG_ASCAN *) pTcpListAscan.RemoveHead();

		if (pImsgAscan->MstrHdr.MsgId == ASCAN_MODE)
		{
			for (i=0; i<1000; i++)
			{
				if (bRF)
				{
					nAscan[i] = (int) ((char) pImsgAscan->Ascan[i]);
				}
				else //Full Wave
				{
					iAscan = abs((int) ((char) pImsgAscan->Ascan[i]));
					if (iAscan > nAscan[i])
						nAscan[i] = iAscan;
				}
			}
		}
		else
		{
			pRawFlaw = (I_MSG_RAW_FLAW *) pImsgAscan;

			i = pRawFlaw->MstrHdr.nSlave;
			if (i < 10)
			{
				if (i == 0)
				{
					if ( (m_bOldPipePresent == FALSE) && (pRawFlaw->MstrHdr.IdataStat.nDup & 0x0002) )
					{
						m_nFlawRevCnt = 1;
						g_RawFlawHead.nJointNum = (DWORD) pRawFlaw->MstrHdr.IdataStat.nRcv;
					}

					if ( (m_bOldPipePresent == TRUE) && (pRawFlaw->MstrHdr.IdataStat.nDup & 0x0002) && (m_nFlawRevCnt < NUM_MAX_REVOL) )
						m_nFlawRevCnt++;

					g_RawFlawHead.nNumRev = m_nFlawRevCnt - 1;

					if ( (m_bOldPipePresent == TRUE) && ((pRawFlaw->MstrHdr.IdataStat.nDup & 0x0002) == 0) )
					{
						//save flaw data to file
						if (SiteDefault.nRecordWallData)
							SaveFlawDataToFile();
					}

					if ( ( ( g_RawFlawCscan[g_nRawFlawBuffer].nMotionBus & 0x0003) == 0x0003 ) && (m_nFlawRevCnt <= NUM_MAX_REVOL) && (m_nFlawRevCnt > 1) )
					{
						memcpy( (void *) &g_RawFlaw[m_nFlawRevCnt-2], (void *) &g_RawFlawCscan[g_nRawFlawBuffer], sizeof (CSCAN_REVOLUTION) );
					}

					g_nRawFlawBuffer = (g_nRawFlawBuffer + 1) % 2;
					g_RawFlawCscan[g_nRawFlawBuffer].nMotionBus = (WORD) pRawFlaw->MstrHdr.IdataStat.nDup;

					if (pRawFlaw->MstrHdr.IdataStat.nDup & 0x0002)
						m_bOldPipePresent = TRUE;
					else
						m_bOldPipePresent = FALSE;
				}

				BYTE *pByte = (BYTE *) g_RawFlawCscan[g_nRawFlawBuffer].Amp;
				memcpy( (void *) &pByte[1152 * i], (void *) pRawFlaw->Buf, 1152);

			}
		}

		delete pImsgAscan;
	}

	if ( bDraw  && (m_nLiveFrozenPeak !=1) )
	{
		for (i=0; i<1000; i++)
			m_nAscan[i] = nAscan[i];

		GetAscanGates();
		DrawTcgCurve();
		m_btnDrawAscan.UpdateWindow();
		DrawAscan();
	}

	CResizingDialog::OnTimer(nIDEvent);
}

void CAscanDlg::GetAscanGates()
{
	char Gate1Trig = ConfigRec.gates.trg_option[gChannel][0];
	char Gate2Trig = ConfigRec.gates.trg_option[gChannel][1];
	char Gate3Trig = ConfigRec.gates.trg_option[gChannel][2];
	char Gate4Trig = ConfigRec.gates.trg_option[gChannel][3];
	float  ascan_delay = (float) ConfigRec.receiver.ascan_delay[gChannel] / 50.f;  //in microseconds
	float  ascan_range = (float) ConfigRec.receiver.ascan_range[gChannel];         //in microseconds
	float  iTraceLength = (float) m_pDrawAscan->m_iTraceLength;
	int  nStart, nStop, nInterfaceTrig=0;
	BOOL bInterfaceTrig = FALSE;
	BOOL bRF;

	if ( ConfigRec.OscopeRec.T2Button[0] == 1)  //show all gates
		m_AscanGates.nWhichGate = 0;
	else
		m_AscanGates.nWhichGate = gGate + 1;  //show active gate

	if ( (Gate1Trig==0) && (Gate2Trig==0) && (Gate3Trig==0) && (Gate4Trig==0) )
		m_AscanGates.nWhichGate = -1;

	if ( (m_AscanGates.nWhichGate > 0) && (ConfigRec.gates.trg_option[gChannel][gGate] == 0) )
		m_AscanGates.nWhichGate = -1;

	for (int i=0; i<4; i++)
	{
		if ((i==0) || (ConfigRec.gates.trg_option[gChannel][i] != 2) )
		{
			m_AscanGates.GateStart[i] = (short) (iTraceLength * ((float) ConfigRec.gates.delay[gChannel][i] / 10.f + 5.0f - ascan_delay) / ascan_range);
			m_AscanGates.GateStop[i] = m_AscanGates.GateStart[i] + (short) (iTraceLength * (float) ConfigRec.gates.range[gChannel][i] / (10.f * ascan_range));
		}
		else  //interface trigger
		{
			if (bInterfaceTrig)
			{
				m_AscanGates.GateStart[i] = nInterfaceTrig + (short) (iTraceLength * ((float) ConfigRec.gates.delay[gChannel][i] / 10.f) / ascan_range);
				m_AscanGates.GateStop[i] = m_AscanGates.GateStart[i] + (short) (iTraceLength * (float) ConfigRec.gates.range[gChannel][i] / (10.f * ascan_range));
			}
			else
			{
				m_AscanGates.GateStart[i] = 0;
				m_AscanGates.GateStop[i] = 0;
			}
		}

		m_AscanGates.GateLevel[i] = ConfigRec.gates.level[gChannel][i] + m_nAscanBaseLine;

		if (i == 0)
		{
			nStart = m_AscanGates.GateStart[i];
			nStop =  m_AscanGates.GateStop[i];
			if (nStop <= 0)
				bInterfaceTrig = FALSE;  //if Gate 1 out of screen (A-scan), we have no data to compare
			else
			{
				if (nStart < 0)
					nStart = 0;
				if (nStop >= iTraceLength)
					nStop = (int) iTraceLength - 1;
				int nAscan;
				for (int j=nStart; j<=nStop; j++)
				{
					bRF = !ConfigRec.gates.det_option[gChannel][0];
					//if (bRF && ConfigRec.gates.polarity[gChannel][0])  //minus polarity
						//nAscan = 0 - m_nAscan[j];
					//else
						nAscan = m_nAscan[j];
					if (nAscan > ConfigRec.gates.level[gChannel][i])
					{
						bInterfaceTrig = TRUE;
						nInterfaceTrig = j;
						break;
					}
				}
			}
		}

		if (ConfigRec.gates.trg_option[gChannel][i] == 0)  //disabled
		{
			m_AscanGates.GateStart[i] = 0;
			m_AscanGates.GateStop[i] = 0;
		}
		else if (ConfigRec.OscopeRec.T2Indx[0] == 2)  //Threshold detect
		{
			nStart = m_AscanGates.GateStart[i];
			nStop =  m_AscanGates.GateStop[i];
			m_AscanGates.GateStart[i] = 0;
			m_AscanGates.GateStop[i] = 0;
			if (nStop > 0)
			{
				if (nStart < 0)
					nStart = 0;
				if (nStop >= iTraceLength)
					nStop = (int) iTraceLength - 1;
				int nAscan;
				for (int j=nStart; j<=nStop; j++)
				{
					bRF = !ConfigRec.gates.det_option[gChannel][i];
					if (bRF && ConfigRec.gates.polarity[gChannel][i])  //minus polarity
						nAscan = 0 - m_nAscan[j];
					else
						nAscan = m_nAscan[j];
					if (nAscan > ConfigRec.gates.level[gChannel][i])
					{
						m_AscanGates.GateStart[i] = j;
						m_AscanGates.GateStop[i] = j + (short) (iTraceLength * (float) ConfigRec.gates.blank[gChannel][i] / (10.f * ascan_range));
						break;
					}
				}
			}
		}
		else if ( (i==3) && (ConfigRec.OscopeRec.T2Indx[0] == 4) )  //TOF Gate, Gate 4 only
		{
			nStart = m_AscanGates.GateStart[i];
			nStop =  m_AscanGates.GateStop[i];
			m_AscanGates.GateStart[i] = 0;
			m_AscanGates.GateStop[i] = 0;
			if (ConfigRec.timeoff.trigger[gChannel][i] == 1)  //TOF start on initial pulse
			{
				m_AscanGates.GateStart[i] = (short) (iTraceLength * (5.0f - ascan_delay) / ascan_range);
				m_AscanGates.GateStop[i] = (short) iTraceLength;
			}
			if (ConfigRec.timeoff.trigger[gChannel][i] == 2)  //TOF start on interface detect
			{
				if (bInterfaceTrig)
				{
					m_AscanGates.GateStart[i] = nInterfaceTrig;
					m_AscanGates.GateStop[i] = (short) iTraceLength;
				}
				else
				{
					m_AscanGates.GateStart[i] = 0;
					m_AscanGates.GateStop[i] = 0;
				}
			}
			if ( (nStop > 0) && (ConfigRec.timeoff.trigger[gChannel][i] > 0) )
			{
				if (nStart < 0)
					nStart = 0;
				if (nStop >= iTraceLength)
					nStop = (int) iTraceLength - 1;
				int nAscan;
				BOOL bSearchStart = TRUE;
				for (int j=nStart; j<=nStop; j++)
				{
					bRF = !ConfigRec.gates.det_option[gChannel][i];
					if (bRF && ConfigRec.gates.polarity[gChannel][i])  //minus polarity
						nAscan = 0 - m_nAscan[j];
					else
						nAscan = m_nAscan[j];
					if (nAscan > ConfigRec.gates.level[gChannel][i]) //look for the threshold trigger points
					{
						if (bSearchStart)
						{
							bSearchStart = FALSE;
							if (ConfigRec.timeoff.trigger[gChannel][i] > 2)
							{
								m_AscanGates.GateStart[i] = j;
								m_AscanGates.GateStop[i] = j + (short) (iTraceLength * (float) ConfigRec.gates.blank[gChannel][i] / (10.f * ascan_range));
							}
							if (ConfigRec.timeoff.trigger[gChannel][i] == 1)  //TOF start on initial pulse
							{
								m_AscanGates.GateStop[i] = j;
								break;
							}
							if (ConfigRec.timeoff.trigger[gChannel][i] == 2)  //TOF start on interface detect
							{
								if (bInterfaceTrig)
								{
									m_AscanGates.GateStop[i] = j;
								}
								break;
							}

							if (m_AscanGates.GateStop[i] > nStop)
							{
								m_AscanGates.GateStop[i] = (short) iTraceLength;
								break;
							}
							else
							{
								j = m_AscanGates.GateStop[i];
								m_AscanGates.GateStop[i] = (short) iTraceLength;
							}
						}
						else
						{
							m_AscanGates.GateStop[i] = j;
							break;
						}

					}
				}
			}
		}
	}

	m_bInterfaceTrig = bInterfaceTrig;
	m_nInterfaceTrig = nInterfaceTrig;
}

void CAscanDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int i;
	float fx1, fy1;

	for (i=0; i<4; i++)
		m_AscanGates.bGateSelect[i] = 0;

	ClientToScreen(&point);
	m_ptDelayChangeStart = point;
	
	CRect rect, rectBase, rectGate[4];
	m_btnDrawAscan.GetWindowRect(&rect);

	if (m_bShowBscan)
		rect.DeflateRect(0, 0, 0, rect.Height()/2);

	int x1, y1, x2, y2;
	x1 = rect.left;
	y1 = rect.top + (100-(m_nAscanBaseLine+3))*rect.Height()/100;
	x2 = rect.left+10*rect.Width()/500 + 1;
	y2 = rect.top + (100-(m_nAscanBaseLine-3))*rect.Height()/100;
	rectBase.SetRect(x1, y1, x2, y2);

	for (i=0; i<4; i++)
	{
		x1 = rect.left + m_AscanGates.GateStart[i] * rect.Width() / m_pDrawAscan->m_iTraceLength;
		y1 = rect.bottom - m_AscanGates.GateLevel[i] * rect.Height() / 100 - 15;
		x2 = rect.left + m_AscanGates.GateStop[i] * rect.Width() / m_pDrawAscan->m_iTraceLength + 5;
		y2 = rect.bottom - m_AscanGates.GateLevel[i] * rect.Height() / 100 + 15;
		rectGate[i].SetRect(x1, y1, x2, y2);
	}

	if (rectBase.PtInRect(point) )  //cursor inside the baseline triangle
	{
		m_bMoveAscanBaseLine = TRUE;
		m_bMoveAscanDelay = FALSE;
		m_bMoveGate = FALSE;
		m_bChangeGateRange = FALSE;
		m_bMoveTcgCurve = FALSE;
		m_bMoveRcvrGain = FALSE;
	}
	else
	{
		m_bMoveAscanBaseLine = FALSE;
		m_bMoveAscanDelay = TRUE;
		m_bMoveGate = FALSE;
		m_bChangeGateRange = FALSE;
		m_bMoveTcgCurve = FALSE;
		m_bMoveRcvrGain = FALSE;

		m_nAscanDelayStart = ConfigRec.receiver.ascan_delay[gChannel];

		for (i=0; i<4; i++)
		{
			if ( rectGate[i].PtInRect(point) && ((ConfigRec.OscopeRec.T2Button[0] == 1) || (gGate==i)) )  //cursor inside gate
			{
				gGate=i;
				SendMsg(GATE_SELECT);
				((CTscanDlg*) GetParent())->UpdateChnlSelected();

				if ( (rectGate[i].right - point.x) > 10 )  //move gate with mouse movement
				{
					m_bMoveAscanDelay = FALSE;
					m_bChangeGateRange = FALSE;
					m_bMoveGate = TRUE;
					
					m_AscanGates.bGateSelect[i] = SELECT_GATE_DELAY;
					m_nGateDelayStart = ConfigRec.gates.delay[gChannel][gGate];
					m_nGateLevelStart = ConfigRec.gates.level[gChannel][gGate];
				}
				else  //change gate range with mouse movement
				{
					m_bMoveAscanDelay = FALSE;
					m_bChangeGateRange = TRUE;
					m_bMoveGate = FALSE;
					
					m_AscanGates.bGateSelect[i] = SELECT_GATE_RANGE;
					m_nGateRangeStart = ConfigRec.gates.range[gChannel][gGate];
				}

				SendMsg(SET_ASCAN_REGISTERS);
				break;
			}
		}

		if (m_bDrawTcgCurve && !m_bMoveGate && !m_bChangeGateRange )  //now check if need move TCG curve
		{
			for (i=1; i<253; i++)  //cursor pointing to TCG curve?
			{
				x1 = rect.left + m_TcgGainTable[i].x * rect.Width() / m_pDrawAscan->m_iTraceLength;
				y1 = rect.bottom - m_TcgGainTable[i].y * rect.Height() / 100;
				fx1 = (float) (x1-point.x);
				fy1 = (float) (y1-point.y);
				//if ( sqrt( (x1-point.x)*(x1-point.x) + (y1-point.y)*(y1-point.y) ) < 20.0f )
				if ( sqrt( fx1*fx1 + fy1*fy1 ) < 20.0f )
				{
					m_bMoveAscanDelay = FALSE;
					if (m_pTcpThreadRxList->GetTcgTrigSel(gChannel) > 0)
					{
						m_bMoveTcgCurve = TRUE;
						m_AscanGates.bGateSelect[0] = SELECT_TCG_CURVE;
						if (m_nTcgMode < 2)  //linear or curve mode
						{
							if (i < 50) m_nCurveAdjust = 0;
							else if (i > 200) m_nCurveAdjust = 1;
							else if ( (i>100) && (i<150) ) m_nCurveAdjust = 0;
							else if ( (i>50) && (i<100) ) m_nCurveAdjust = 3;
							else m_nCurveAdjust = 1;
						}
						else  //gate mode
						{
							m_nCurveAdjust = 1000;
							for (int j=0; j<4; j++)  //4 gates
							{
								if ( (point.x > rectGate[j].left) && (point.x < rectGate[j].right) )
								{
									if (m_pTcpThreadRxList->GetTcgTrigSel(gChannel) == m_pTcpThreadRxList->GetGateTrigMode(gChannel, j))  //both by IP or both by IF
									{
										m_nCurveAdjust = j;
										//break;  //don't break meaning only change the last gate if two or more gates overlap
									}
								}
							}
						}
					}
					else
					{
						m_bMoveRcvrGain = TRUE;
						m_AscanGates.bGateSelect[0] = SELECT_RCVR_GAIN;
					}
					break;
				}
			}

			if (!m_bMoveTcgCurve && !m_bMoveRcvrGain)  //cursor pointing to receiver gain?
			{
				x1 = rect.left + m_TcgGainTable[0].x * rect.Width() / m_pDrawAscan->m_iTraceLength;
				y1 = rect.bottom  - m_TcgGainTable[0].y * rect.Height() / 100;
				x2 = rect.left + m_TcgGainTable[255].x * rect.Width() / m_pDrawAscan->m_iTraceLength;
				y2 = rect.bottom - m_TcgGainTable[255].y * rect.Height() / 100;

				if ( (point.x < x1) && (abs(point.y - y1) < 7) )
				{
					m_bMoveAscanDelay = FALSE;
					m_bMoveRcvrGain = TRUE;
					m_AscanGates.bGateSelect[0] = SELECT_RCVR_GAIN;
				}
				else if ( (point.x > x2) && (abs(point.y - y2) < 7) )
				{
					m_bMoveAscanDelay = FALSE;
					m_bMoveRcvrGain = TRUE;
					m_AscanGates.bGateSelect[0] = SELECT_RCVR_GAIN;
				}
			}
		}
	}

	CResizingDialog::OnLButtonDown(nFlags, point);
}

void CAscanDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	float DelayChange;
	int  ascan_delay = m_nAscanDelayStart;  //(int) ConfigRec.receiver.ascan_delay[gChannel];
	float  ascan_range = (float) ConfigRec.receiver.ascan_range[gChannel];
		
	ClientToScreen(&point);
	CRect rect;
	m_btnDrawAscan.GetWindowRect(&rect);

	if (m_bShowBscan)
		rect.DeflateRect(0, 0, 0, rect.Height()/2);

	if (nFlags & MK_LBUTTON)
	{
		if (m_bMoveAscanBaseLine && rect.PtInRect(point))
		{
			m_nAscanBaseLine = (rect.bottom - point.y)*100/rect.Height();
			ConfigRec.receiver.ascan_baseline[gChannel] = m_nAscanBaseLine;
		}
		else if (m_bMoveAscanDelay && rect.PtInRect(point))
		{
			DelayChange = (float) ( ascan_range * (m_ptDelayChangeStart.x - point.x) / (float) rect.Width() );
			//m_ptDelayChangeStart.x = point.x;

			ascan_delay += (int) (DelayChange * 50);
			if (ascan_delay < 251)
				ascan_delay = 251;
			if (ascan_delay > 25000)
				ascan_delay = 25000;
			ConfigRec.receiver.ascan_delay[gChannel] = ascan_delay;

			UpdateDlg();

			SendMsg(SET_ASCAN_REGISTERS);
		}
		else if (m_bMoveGate && rect.PtInRect(point))
		{
			//Gate Delay
			DelayChange = (float) ( ascan_range * (point.x - m_ptDelayChangeStart.x) / (float) rect.Width() );

			short GateDelay = m_nGateDelayStart + (int) (DelayChange * 10);
			if (GateDelay < DELAYMIN)
				GateDelay = DELAYMIN;
			if (GateDelay > DELAYMAX)
				GateDelay = DELAYMAX;

			ConfigRec.gates.delay[gChannel][gGate] = GateDelay;

			SendMsg(GATES_DELAY);			// Set gate delay

			//Gate Level
			DelayChange = (float) ( 100 * (m_ptDelayChangeStart.y - point.y) / (float) rect.Height() );

			GateDelay = m_nGateLevelStart + (int) (DelayChange);
			if (GateDelay < LEVEL1MIN)
				GateDelay = LEVEL1MIN;
			if (GateDelay > LEVEL1MAX)
				GateDelay = LEVEL1MAX;

			ConfigRec.gates.level[gChannel][gGate] = GateDelay;

			SetTimer(GATES_LEVEL_TIMER, 200, NULL);
			//SendMsg(GATES_LEVEL);			// Set gate delay
		}
		else if (m_bChangeGateRange && rect.PtInRect(point))
		{
			//Gate Range
			DelayChange = (float) ( ascan_range * (point.x - m_ptDelayChangeStart.x) / (float) rect.Width() );

			short GateDelay = m_nGateRangeStart + (int) (DelayChange * 10);
			if (GateDelay < RANGEMIN)
				GateDelay = RANGEMIN;
			if (GateDelay > RANGEMAX)
				GateDelay = RANGEMAX;

			ConfigRec.gates.range[gChannel][gGate] = GateDelay;

			SendMsg(GATES_RANGE);			// Set gate range
		}
		else if (m_bMoveTcgCurve && rect.PtInRect(point))
		{
			//Tcg gain change
			if ((m_nTcgMode != 2) && (m_nCurveAdjust >= 0) && (m_nCurveAdjust <= 4))
			{
				float GainChange = (float) ( 100.f * (point.y - m_ptDelayChangeStart.y) / (float) rect.Height() );
				m_ptDelayChangeStart.y = point.y;

				m_pTcpThreadRxList->SetTcgGainTable(m_nCurveAdjust,gChannel, gGate, GainChange, m_nTcgMode);
			}
			else if (m_nTcgMode == 2)
			{
				if ( (m_nCurveAdjust >=0) && (m_nCurveAdjust <4) )
				{
					float fGain;
					float GainChange = (float) ( 100.f * (point.y - m_ptDelayChangeStart.y) / (float) rect.Height() );
					m_ptDelayChangeStart.y = point.y;
					GetGainOfGate(gChannel, m_nCurveAdjust);

					m_fGain -= GainChange;
					fGain = m_fGain - m_pTcpThreadRxList->m_TruscopGainOffset/m_pTcpThreadRxList->m_TruscopGainScaling;
					if (fGain < 0) fGain = 0.0f;
					if (fGain > 40.0) fGain = 40.0f;

					m_pTcpThreadRxList->SetTcgGainTable(0,gChannel, m_nCurveAdjust, fGain, m_nTcgMode);
				}
			}
		}
		else if (m_bMoveRcvrGain && rect.PtInRect(point))
		{
			float GainChange = (float) ( 100.f * ( m_ptDelayChangeStart.y - point.y) / (float) rect.Height() );
			m_ptDelayChangeStart.y = point.y;
			float fGain = m_pTcpThreadRxList->GetRcvrGain(gChannel) + GainChange;
			if (fGain > 40.0f) fGain = 40.0f;
			if (fGain < 0.0f) fGain = 0.0f;
			m_pTcpThreadRxList->SetRcvrGain(gChannel, fGain);
		}
	}
	
	CResizingDialog::OnMouseMove(nFlags, point);
}

BOOL CAscanDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bMoveGate)
	{
		//Gate Delay
		float DelayChange = 0.1f*((float) zDelta/WHEEL_DELTA);

		short GateDelay = ConfigRec.gates.delay[gChannel][gGate] + (int) (DelayChange * 10);
		if (GateDelay < DELAYMIN)
			GateDelay = DELAYMIN;
		if (GateDelay > DELAYMAX)
			GateDelay = DELAYMAX;

		ConfigRec.gates.delay[gChannel][gGate] = GateDelay;

		SendMsg(GATES_DELAY);			// Set gate delay
	}
	else if (m_bChangeGateRange)
	{
		//Gate Range
		float DelayChange = 0.1f*((float) zDelta/WHEEL_DELTA);

		short GateDelay = ConfigRec.gates.range[gChannel][gGate] + (int) (DelayChange * 10);
		if (GateDelay < RANGEMIN)
			GateDelay = RANGEMIN;
		if (GateDelay > RANGEMAX)
			GateDelay = RANGEMAX;

		ConfigRec.gates.range[gChannel][gGate] = GateDelay;

		SendMsg(GATES_RANGE);			// Set gate range
	}
	else if (m_bMoveTcgCurve)
	{
		float GainChange;

		if (m_pTcpThreadRxList->GetTcgTrigSel(gChannel) != 0)
		{
			if ((m_nTcgMode != 2) && (m_nCurveAdjust >= 0) && (m_nCurveAdjust <= 4))
			{
				GainChange = -0.5f*(zDelta/WHEEL_DELTA);

				m_pTcpThreadRxList->SetTcgGainTable(m_nCurveAdjust,gChannel, gGate, GainChange, m_nTcgMode);
			}
			
			if (m_nTcgMode == 2)
			{
				if ( (m_nCurveAdjust >=0) && (m_nCurveAdjust <4) )
				{
					float fGain;
					GainChange = -0.5f*(zDelta/WHEEL_DELTA);
					GetGainOfGate(gChannel, m_nCurveAdjust);

					m_fGain -= GainChange;
					fGain = m_fGain - m_pTcpThreadRxList->m_TruscopGainOffset/m_pTcpThreadRxList->m_TruscopGainScaling;
					if (fGain < 0) fGain = 0.0f;
					if (fGain > 40.0) fGain = 40.0f;

					m_pTcpThreadRxList->SetTcgGainTable(0,gChannel, m_nCurveAdjust, fGain, m_nTcgMode);
				}
			}
		}
	}
	else if (m_bMoveRcvrGain)
	{
		float GainChange = 0.2f*((float) zDelta/WHEEL_DELTA);
		float fGain = m_pTcpThreadRxList->GetRcvrGain(gChannel) + GainChange;
		if (fGain > 40.0f) fGain = 40.0f;
		if (fGain < 0.0f) fGain = 0.0f;
		m_pTcpThreadRxList->SetRcvrGain(gChannel, fGain);
	}
	else  //move ascan baseline
	{
		m_nAscanBaseLine += (zDelta/100);
		if (m_nAscanBaseLine < 0) m_nAscanBaseLine  = 0;
		if (m_nAscanBaseLine > 100) m_nAscanBaseLine  = 100;
		
		ConfigRec.receiver.ascan_baseline[gChannel] = m_nAscanBaseLine;
	}

	if (m_bMoveGate || m_bChangeGateRange || m_bMoveTcgCurve || m_bMoveRcvrGain)
		((CTscanDlg*) GetParent())->UpdateDlgs();

	return CResizingDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CAscanDlg::UpdateDlg()
{
	int  ascan_delay = (int) ConfigRec.receiver.ascan_delay[gChannel] - 250;
	int  ascan_range = (int) ConfigRec.receiver.ascan_range[gChannel];
	BOOL bRF = ConfigRec.receiver.det_option[gChannel];

	switch(ascan_range)
	{
	case 5:  //5 us
		if (bRF)
			m_pDrawAscan->m_iTraceLength = 500;
		else
			m_pDrawAscan->m_iTraceLength = 500;//63;
		break;

	case 10:  //10 us
		if (bRF)
			m_pDrawAscan->m_iTraceLength = 1000;
		else
			m_pDrawAscan->m_iTraceLength = 1000;//125;
		break;
	case 20:  //20 us
		if (bRF)
			m_pDrawAscan->m_iTraceLength = 1000;
		else
			m_pDrawAscan->m_iTraceLength = 1000;//250;
		break;
	case 30:  //30 us
		if (bRF)
			m_pDrawAscan->m_iTraceLength = 1000;
		else
			m_pDrawAscan->m_iTraceLength = 1000;//375;
		break;
	case 40:  //40 us
	case 50:  //50 us
	case 100:  //100 us
	case 150:  //150 us
	default:
		if (bRF)
			m_pDrawAscan->m_iTraceLength = 1000;
		else
			m_pDrawAscan->m_iTraceLength = 1000;//500;
		break;
	}

	m_nAscanBaseLine = ConfigRec.receiver.ascan_baseline[gChannel];

	ascan_range *= 50;
	ascan_range += ascan_delay;

	CString s;
	float fact;
	if (ConfigRec.bMotionTime & 0x80)  //distance
	{
		if (ConfigRec.bEnglishMetric == ENGLISH)
		{
			if (ConfigRec.UtRec.Shoe[gChannel/10].Ch[gChannel%10].Type == IS_WALL)
				fact = 0.23f;
			else
				fact = 0.13f;
			SetDlgItemText(IDC_STATIC_ASCANUNIT, _T("inch"));
		}
		else  //metric
		{
			if (ConfigRec.UtRec.Shoe[gChannel/10].Ch[gChannel%10].Type == IS_WALL)
				fact = 5.9f;
			else
				fact = 3.2f;
			SetDlgItemText(IDC_STATIC_ASCANUNIT, _T("mm"));
		}
	}
	else  //time
	{
		fact = 1.0f;
		SetDlgItemText(IDC_STATIC_ASCANUNIT, _T("us"));
	}
	if (ascan_delay < 0)
		s.Format(_T("%4.1f"), ascan_delay * 0.02f * fact);
	else if (ascan_delay < 500)
		s.Format(_T("%3.1f"), ascan_delay * 0.02f * fact);
	else if (ascan_delay < 5000)
		s.Format(_T("%4.1f"), ascan_delay * 0.02f * fact);
	else
		s.Format(_T("%5.1f"), ascan_delay * 0.02f * fact);
	SetDlgItemText(IDC_STATIC_DELAY, s);

	if (ascan_range < 500)
		s.Format(_T("%3.1f"), ascan_range * 0.02f * fact);
	else if (ascan_range < 5000)
		s.Format(_T("%4.1f"), ascan_range * 0.02f * fact);
	else
		s.Format(_T("%5.1f"), ascan_range * 0.02f * fact);
	SetDlgItemText(IDC_STATIC_HRIGHT, s);

	//check tcg mode radio button
	if (m_nTcgMode == 0)
		CheckRadioButton(IDC_RADIO_TCGMODE1,IDC_RADIO_TCGMODE3,IDC_RADIO_TCGMODE1);
	if (m_nTcgMode == 1)
		CheckRadioButton(IDC_RADIO_TCGMODE1,IDC_RADIO_TCGMODE3,IDC_RADIO_TCGMODE2);
	if (m_nTcgMode == 2)
		CheckRadioButton(IDC_RADIO_TCGMODE1,IDC_RADIO_TCGMODE3,IDC_RADIO_TCGMODE3);

	// check trigger radio button
	int nTrigSel = m_pTcpThreadRxList->GetTcgTrigSel(gChannel);
	CheckRadioButton(IDC_RADIO_TRIGOFF, IDC_RADIO_TRIGIF, IDC_RADIO_TRIGOFF+nTrigSel);

	// check step radio button
	m_nTcgStep = m_pTcpThreadRxList->GetTcgUpdateRate(gChannel);
	CheckRadioButton(IDC_RADIO_STEP200, IDC_RADIO_STEP800, IDC_RADIO_STEP200+m_nTcgStep);

	m_pTcpThreadRxList->ComputeTcgGainTable(gChannel);
}

void CAscanDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	CMenu Menu;
	CPoint pt;
	CRect rect;
	BOOL bRF = ConfigRec.receiver.det_option[gChannel];

	m_nRefreshRate = ConfigRec.receiver.ascan_refreshrate;

	// Preserve the system menu properties
	pt = point;
	GetClientRect(&rect);
	ScreenToClient(&pt);

	// Was the mouse clicked on the client area or system area?
	if ( rect.PtInRect (pt) )
	{	// client area
		Menu.LoadMenu(IDR_MENU_ASCAN);

		CMenu *pContextMenu = Menu.GetSubMenu(0);
		if (bRF)
		{
			pContextMenu->CheckMenuItem(ID_MENU_RF, MF_CHECKED);
			pContextMenu->CheckMenuItem(ID_MENU_FW, MF_UNCHECKED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE5, MF_ENABLED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE10, MF_ENABLED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE50, MF_GRAYED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE100, MF_GRAYED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE200, MF_GRAYED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE500, MF_GRAYED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE1000, MF_GRAYED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE2000, MF_GRAYED);
		}
		else
		{
			pContextMenu->CheckMenuItem(ID_MENU_RF, MF_UNCHECKED);
			pContextMenu->CheckMenuItem(ID_MENU_FW, MF_CHECKED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE5, MF_GRAYED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE10, MF_GRAYED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE50, MF_ENABLED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE100, MF_ENABLED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE200, MF_ENABLED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE500, MF_ENABLED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE1000, MF_ENABLED);
			pContextMenu->EnableMenuItem(ID_MENU_RANGE2000, MF_ENABLED);
		}
		if (m_bDrawTcgCurve)
		{
			pContextMenu->CheckMenuItem(ID_MENU_SHOWTCG, MF_CHECKED);
			pContextMenu->CheckMenuItem(ID_MENU_HIDETCG, MF_UNCHECKED);
		}
		else
		{
			pContextMenu->CheckMenuItem(ID_MENU_SHOWTCG, MF_UNCHECKED);
			pContextMenu->CheckMenuItem(ID_MENU_HIDETCG, MF_CHECKED);
		}
		if (m_bShowBscan)
		{
			pContextMenu->CheckMenuItem(ID_MENU_SHOWBSCAN, MF_CHECKED);
			pContextMenu->CheckMenuItem(ID_MENU_HIDEBSCAN, MF_UNCHECKED);
		}
		else
		{
			pContextMenu->CheckMenuItem(ID_MENU_SHOWBSCAN, MF_UNCHECKED);
			pContextMenu->CheckMenuItem(ID_MENU_HIDEBSCAN, MF_CHECKED);
		}
		if (m_bGrayscale)
		{
			pContextMenu->CheckMenuItem(ID_MENU_GRAYSCALE, MF_CHECKED);
			pContextMenu->CheckMenuItem(ID_MENU_COLOR, MF_UNCHECKED);
		}
		else
		{
			pContextMenu->CheckMenuItem(ID_MENU_GRAYSCALE, MF_UNCHECKED);
			pContextMenu->CheckMenuItem(ID_MENU_COLOR, MF_CHECKED);
		}
		if (m_bLinearScale)
		{
			pContextMenu->CheckMenuItem(ID_MENU_LINEAR, MF_CHECKED);
			pContextMenu->CheckMenuItem(ID_MENU_LOG, MF_UNCHECKED);
		}
		else
		{
			pContextMenu->CheckMenuItem(ID_MENU_LINEAR, MF_UNCHECKED);
			pContextMenu->CheckMenuItem(ID_MENU_LOG, MF_CHECKED);
		}

		if (m_bBcastAscan)
		{
			pContextMenu->CheckMenuItem(ID_MENU_BROADCAST, MF_CHECKED);
		}
		else
		{
			pContextMenu->CheckMenuItem(ID_MENU_BROADCAST, MF_UNCHECKED);
		}

		for (int i=0; i<3; i++)
		{
			pContextMenu->CheckMenuItem(ID_MENU_REFRESH10+i, MF_UNCHECKED);
		}
		pContextMenu->CheckMenuItem(ID_MENU_REFRESH60, MF_UNCHECKED);
		pContextMenu->CheckMenuItem(ID_MENU_REFRESH10+m_nRefreshRate/10-1, MF_CHECKED);

		int nCmd = pContextMenu->TrackPopupMenu( TPM_LEFTALIGN |
						TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
						point.x, point.y, this, NULL);

		switch (nCmd)
		{
		case ID_MENU_RANGE5:
			ConfigRec.receiver.ascan_range[gChannel] = 5;
			break;

		case ID_MENU_RANGE10:
			ConfigRec.receiver.ascan_range[gChannel] = 10;
			break;

		case ID_MENU_RANGE20:
			ConfigRec.receiver.ascan_range[gChannel] = 20;
			break;

		case ID_MENU_RANGE30:
			ConfigRec.receiver.ascan_range[gChannel] = 30;
			break;

		case ID_MENU_RANGE40:
			ConfigRec.receiver.ascan_range[gChannel] = 40;
			break;

		case ID_MENU_RANGE50:
			ConfigRec.receiver.ascan_range[gChannel] = 50;
			break;

		case ID_MENU_RANGE100:
			ConfigRec.receiver.ascan_range[gChannel] = 100;
			break;

		case ID_MENU_RANGE200:
			ConfigRec.receiver.ascan_range[gChannel] = 200;
			break;

		case ID_MENU_RANGE500:
			ConfigRec.receiver.ascan_range[gChannel] = 500;
			break;

		case ID_MENU_RANGE1000:
			ConfigRec.receiver.ascan_range[gChannel] = 1000;
			break;

		case ID_MENU_RANGE2000:
			ConfigRec.receiver.ascan_range[gChannel] = 2000;
			break;

		case ID_MENU_RF:
			ConfigRec.receiver.det_option[gChannel] = 1;
			//if (ConfigRec.receiver.ascan_range[gChannel] > 40)
				//ConfigRec.receiver.ascan_range[gChannel] = 40;
			break;

		case ID_MENU_FW:
			ConfigRec.receiver.det_option[gChannel] = 0;
			if (ConfigRec.receiver.ascan_range[gChannel] < 20)
				ConfigRec.receiver.ascan_range[gChannel] = 20;
			break;

		case ID_MENU_PRINT:
			PrintAscan();
			break;

		case ID_MENU_SAVEASCAN:
			SaveAscan();
			break;

		case ID_MENU_COPYASCAN:
			CopyAscan();
			break;

		case ID_MENU_BKBLACK:
			m_bkColor = RGB(0, 0, 0);
			m_nBkColor = 0;
			break;

		case ID_MENU_BKWHITE:
			m_bkColor = RGB(255, 255, 255);
			m_nBkColor = 1;
			break;

		case ID_MENU_TRACEWHITE:
			m_traceColor = RGB(255, 255, 255);
			m_nTraceColor = 0;
			break;

		case ID_MENU_TRACEBLACK:
			m_traceColor = RGB(0, 0, 0);
			m_nTraceColor = 1;
			break;

		case ID_MENU_TRACERED:
			m_traceColor = RGB(255, 0, 0);
			m_nTraceColor = 2;
			break;

		case ID_MENU_TRACEGREEN:
			m_traceColor = RGB(0, 255, 0);
			m_nTraceColor = 3;
			break;

		case ID_MENU_TRACEBLUE:
			m_traceColor = RGB(0, 0, 255);
			m_nTraceColor = 4;
			break;

		case ID_MENU_SHOWTCG:
			m_bShowTcgCurve = TRUE;
			break;

		case ID_MENU_HIDETCG:
			m_bShowTcgCurve = FALSE;
			break;

		case ID_MENU_GATE1OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[0] != 0 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[0] = 0;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[0] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[0] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[0];

			}
			m_pTcpThreadRxList->SendTcgFunction(gChannel);
			break;

		case ID_MENU_GATE2OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[1] != 0 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[1] = 0;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[1] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[1] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[1];

			}
			m_pTcpThreadRxList->SendTcgFunction(gChannel);
			break;

		case ID_MENU_GATE3OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[2] != 0 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[2] = 0;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[2] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[2] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[2];

			}
			m_pTcpThreadRxList->SendTcgFunction(gChannel);
			break;

		case ID_MENU_GATE4OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[3] != 0 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[3] = 0;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[3] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[3] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[3];

			}
			m_pTcpThreadRxList->SendTcgFunction(gChannel);
			break;

		case ID_MENU_UNDOGATE1OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[0] == 1 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[0] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[0] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[0];

			}
			m_pTcpThreadRxList->SendTcgFunction(gChannel);
			break;

		case ID_MENU_UNDOGATE2OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[1] == 1 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[1] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[1] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[1];

			}
			m_pTcpThreadRxList->SendTcgFunction(gChannel);
			break;

		case ID_MENU_UNDOGATE3OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[2] == 1 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[2] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[2] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[2];

			}
			m_pTcpThreadRxList->SendTcgFunction(gChannel);
			break;

		case ID_MENU_UNDOGATE4OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOff[3] == 1 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateOn[3] = 1;
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].GateGain[3] 
					= (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].UndoGateOffGain[3];

			}
			m_pTcpThreadRxList->SendTcgFunction(gChannel);
			break;

		case ID_MENU_CURVE2OFF:
			if ( (m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].Curve2On != 0 )
			{
				(m_pTcpThreadRxList->m_pConfigRec)->receiver.TcgRec[gChannel].Curve2On = 0;
			}
			m_pTcpThreadRxList->SendTcgFunction(gChannel);
			break;

		case ID_MENU_SHOWBSCAN:
			m_bShowBscan = TRUE;
			m_bClearBscan = TRUE;
			break;

		case ID_MENU_HIDEBSCAN:
			m_bShowBscan = FALSE;
			break;

		case ID_MENU_GRAYSCALE:
			m_bGrayscale = TRUE;
			break;

		case ID_MENU_COLOR:
			m_bGrayscale = FALSE;
			break;

		case ID_MENU_LINEAR:
			m_bLinearScale = TRUE;
			break;

		case ID_MENU_LOG:
			m_bLinearScale = FALSE;
			break;

		case ID_MENU_REFRESH10:
			m_nRefreshRate = ConfigRec.receiver.ascan_refreshrate = 10;
			SendMsg(ASCAN_REFRESH_RATE);
			SetTimer(ASCAN_TIMER,100,NULL);
			return;
			break;

		case ID_MENU_REFRESH20:
			m_nRefreshRate = ConfigRec.receiver.ascan_refreshrate = 20;
			SendMsg(ASCAN_REFRESH_RATE);
			SetTimer(ASCAN_TIMER,50,NULL);
			return;
			break;

		case ID_MENU_REFRESH30:
			m_nRefreshRate = ConfigRec.receiver.ascan_refreshrate = 30;
			SendMsg(ASCAN_REFRESH_RATE);
			SetTimer(ASCAN_TIMER,30,NULL);
			return;
			break;

		case ID_MENU_REFRESH60:
			m_nRefreshRate = ConfigRec.receiver.ascan_refreshrate = 60;
			SendMsg(ASCAN_REFRESH_RATE);
			SetTimer(ASCAN_TIMER,15,NULL);
			return;
			break;

		case ID_MENU_BROADCAST:
			m_bBcastAscan = ConfigRec.receiver.ascan_broadcast = !m_bBcastAscan;
			SendMsg(ASCAN_BROADCAST);
			return;
			break;

		default:
			return;
			break;
		}

		UpdateDlg();
		SendMsg(SET_ASCAN_PEAK_SEL_REG);
		SendMsg(SET_ASCAN_REGISTERS);
	}
	else
		CWnd::OnContextMenu(pWnd,point);	// system area
}

BOOL CAscanDlg::SendMsg(int MsgNum)
{	// wrapper so we don't have to reference msg sender
		// in Tscandlg window.
	return 	((CTscanDlg*) GetParent())->SendMsg(MsgNum);
}


void CAscanDlg::PrintAscan() 
{
	// TODO: Add your control notification handler code here
	CDC dc,*pDC;
    CRect DrawRect(0,0,0,0);
    CRect CalRect(0,0,1000,200);  // where to draw the graph
    CRect MapRect(0,0,1000,400);  
    CTime   theTime = CTime::GetCurrentTime();
    CFont oFont,*pFont;
    LOGFONT lfFont;
    CPoint  returnLocation;
    CSize size;
    CPen  pen(PS_SOLID, 0, RGB(0,0,0));  // BLACK
    int nHeight,RowHeight_h;
    int temp;
    int channel = gChannel;
    int Xoffset = 100;
    int Yoffset = 20;
	TEXTMETRIC tm;					// struct which hold characteristics of screen
    BOOL NewPage = FALSE;
	int cxChar, cyChar;				// char size in pixels
	CString ss;

	CPrintDialog dlgp(FALSE);
	dlgp.GetDefaults();		//dlg.GetPrinterDC();
	dc.Attach(dlgp.GetPrinterDC());
	pDC = &dc;

	if ( *pDC == NULL )
	{
		AfxMessageBox(_T("No Default Printer Available, Please Setup a Default Printer"));	
			return;  // no printer or print cancelled
	}

    DOCINFO di;   // setup DOCINFO structure
    ::ZeroMemory(&di, sizeof(DOCINFO));
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = _T("Flaw Map");  // need date & wo info here ????
	int pageWidth = dc.GetDeviceCaps(HORZRES);
    int pageHeight = dc.GetDeviceCaps(VERTRES);
	int color = dc.GetDeviceCaps(NUMCOLORS);

	nHeight = -((pDC->GetDeviceCaps(LOGPIXELSY))/72);

    pFont = pDC->GetCurrentFont();
    pFont->GetLogFont( &lfFont );
    RowHeight_h = (int)(1.1 * abs(lfFont.lfHeight));
    CPoint print_origin(Xoffset,Yoffset);
    BOOL bContinue = TRUE;
	pDC->GetTextMetrics (&tm);	// put after setting viewport/window ext
	//xChar = tm.tmAveCharWidth;
	cyChar = 3*(tm.tmHeight + tm.tmExternalLeading)/4;
//	yChar = tm.tmHeight;
	cxChar = 3*tm.tmAveCharWidth/2;
	print_origin.x = pageWidth/20;
	print_origin.y = pageHeight/20 ;

    if ( pDC->StartDoc(&di) > 0 )
	{ 
	    pDC->StartPage();

	    pDC->SetMapMode(MM_TEXT );
	    pDC->SetTextAlign(TA_LEFT);

	    ss.Format(_T("TRUSCAN A-Scan Channel %d"), gChannel+1);

	    temp = CViewcfg::PrintHeading(pDC, print_origin, pageWidth, nHeight, 
						ss, "");

        print_origin.y = temp;

        CalRect.SetRect(0,0, pageWidth*9/10, (int)(-pageHeight/3) );
        CalRect.NormalizeRect();

        pDC->SetMapMode(MM_ANISOTROPIC);
        pDC->SetWindowExt(m_pDrawAscan->m_iTraceLength,-100);   // define size of window in logical units

		int wallorg	= (int)(pageHeight*.71);
		pDC->SetViewportOrg( print_origin.x,wallorg);
		pDC->SetViewportExt( CalRect.Width(), CalRect.Height());

		//draw scale lines
		int i, y, xV, yH;
		float ixDiv, iyDiv;

		ixDiv = (float) m_pDrawAscan->m_iTraceLength / 10.f;
		iyDiv = 10.f;
		COLORREF clr = RGB(0, 60, 0);
		CPen scalePen(PS_DOT, 0, clr);
		pDC->SelectObject(&scalePen);
		for(i=1; i<10; i++)
		{
			xV = (int) (ixDiv * i + 0.5f);
			yH = (int) (iyDiv * i + 0.5f);
			pDC->MoveTo(xV, 0);
			pDC->LineTo(xV,100);
			pDC->MoveTo(0, yH);
			pDC->LineTo(m_pDrawAscan->m_iTraceLength,yH);
		}

		//draw baseline
		CPen basePen(PS_SOLID, 0, RGB(120, 120, 120));
		pDC->SelectObject (&basePen);
		pDC->MoveTo(0, m_nAscanBaseLine);
		pDC->LineTo(m_pDrawAscan->m_iTraceLength, m_nAscanBaseLine);

		pDC->SelectObject(&pen);

		//draw bounding rect
		pDC->MoveTo(0, 0);
		pDC->LineTo(m_pDrawAscan->m_iTraceLength, 0);
		pDC->LineTo(m_pDrawAscan->m_iTraceLength, 100);
		pDC->LineTo(0, 100);
		pDC->LineTo(0, 0);

		BOOL bRF = ConfigRec.receiver.det_option[gChannel];
		CPoint point[1024];
		for (i=0; i<m_pDrawAscan->m_iTraceLength; i++)
		{
			if (bRF)
			{
				if (ConfigRec.gates.polarity[gChannel][gGate])  //minus polarity
					y =  m_nAscanBaseLine - m_nAscan[i];  //invert
				else
					y = m_nAscan[i] + m_nAscanBaseLine;
			}
			else
				y = m_nAscan[i] + m_nAscanBaseLine;
			if (y < 0) y = 0;
			if (y > 100) y = 100;
			point[i].x = i;
			point[i].y = y;
		}
		pDC->Polyline(point, m_pDrawAscan->m_iTraceLength);

		CFont xFont;
		xFont.CreateFont(-4,0,0,0,FW_NORMAL,0,0,0,
			DEFAULT_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,
			DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE,  _T("Arial"));
		CFont *pOldFont = pDC->SelectObject(&xFont);

		int  ascan_delay = (int) ConfigRec.receiver.ascan_delay[gChannel] - 250;
		int  ascan_range = (int) ConfigRec.receiver.ascan_range[gChannel];

		ascan_range *= 50;
		ascan_range += ascan_delay;

		if (ascan_delay < 0)
			ss.Format(_T("%4.1f us"), ascan_delay * 0.02f);
		else if (ascan_delay < 500)
			ss.Format(_T("%3.1f us"), ascan_delay * 0.02f);
		else if (ascan_delay < 5000)
			ss.Format(_T("%4.1f us"), ascan_delay * 0.02f);
		else
			ss.Format(_T("%5.1f us"), ascan_delay * 0.02f);

		pDC->TextOut(-2,-2, ss);

		if (ascan_range < 500)
			ss.Format(_T("%3.1f us"), ascan_range * 0.02f);
		else if (ascan_range < 5000)
			ss.Format(_T("%4.1f us"), ascan_range * 0.02f);
		else
			ss.Format(_T("%5.1f us"), ascan_range * 0.02f);

		pDC->TextOut( (int) (m_pDrawAscan->m_iTraceLength*0.95f), -2, ss);

		pDC->SelectObject(pOldFont);
		xFont.DeleteObject();

        pDC->EndPage();
	}

	pDC->EndDoc();
	dc.Detach();
}	

void CAscanDlg::SaveAscan() 
{
    TCHAR szFilter[] = _T("A-Scan Data Files(*.csv) | *.csv||");
    CFileDialog dlg (FALSE, _T("csv"), _T("*.csv"),OFN_OVERWRITEPROMPT, 
                 szFilter);
	CString szTitle = _T ("Save A-Scan to File");
	dlg.m_ofn.lpstrTitle = szTitle;
    if (m_sDefFileDir.GetLength() > 0) 
		dlg.m_ofn.lpstrInitialDir = m_sDefFileDir;
	else
		dlg.m_ofn.lpstrInitialDir = _T("C:\\PhasedArray\\Ascan");

    if ( dlg.DoModal() == IDOK)
    {   
		// file open dialog returned OK
        CString s, t, sFileName;
		TCHAR cDriveLetter;
        s = dlg.GetPathName();
		sFileName = dlg.GetFileName();
		cDriveLetter = s.GetAt(0);
		t.Format(_T("%c:"),cDriveLetter);
		if ( GetDriveType(t) != DRIVE_FIXED )
		{
			AfxMessageBox(_T("This is NOT a hard disk. Please select a hard disk"));
			return;
		}

		m_sDefFileDir = s;
		m_sDefFileDir.TrimRight(sFileName);

        try
        {
            CFile file (s, CFile::modeCreate | CFile::modeReadWrite);

			file.SeekToBegin();

			s.Format(_T("Truscan A-Scan Data File\r\n"));
            file.Write( s, s.GetLength() );
			s.Format(_T("Time(us), Amplitude\r\n"));
            file.Write( s, s.GetLength() );

			int iTraceLength = m_pDrawAscan->m_iTraceLength;
			float  ascan_delay = ((float) ConfigRec.receiver.ascan_delay[gChannel] - 250.f) / 50.f;
			float  ascan_range = (float) ConfigRec.receiver.ascan_range[gChannel];
			float time;
			for (int i=0; i<iTraceLength; i++)
			{
				time = ascan_delay + i * ascan_range / (float) iTraceLength;
				s.Format(_T("%8.2f, %d\r\n"), time, m_nAscan[i]);
				file.Write( s, s.GetLength() );
			}
        }
        catch (CFileException* e)
        {
            e->ReportError();
            e->Delete();
            return;
        }
	}
}

void CAscanDlg::CopyAscan()
{
	if (::OpenClipboard (m_hWnd)) 
	{
		// Make a copy of the bitmap.
		BITMAP bm;
		CBitmap bitmap;
		m_pDrawAscan->m_bitmap.GetObject (sizeof (bm), &bm);
		CRect Rect;
		m_btnDrawAscan.GetClientRect(&Rect);
		bm.bmWidth = Rect.Width();
		bm.bmHeight = Rect.Height();
		bitmap.CreateBitmapIndirect (&bm);

		CDC dcMemSrc, dcMemDest;
		dcMemSrc.CreateCompatibleDC (NULL);
		CBitmap* pOldBitmapSrc = dcMemSrc.SelectObject (&(m_pDrawAscan->m_bitmap));

		dcMemDest.CreateCompatibleDC (NULL);
		CBitmap* pOldBitmapDest = dcMemDest.SelectObject (&bitmap);

		dcMemDest.StretchBlt (0, 0, bm.bmWidth, bm.bmHeight, &dcMemSrc, 0, 0/*bm.bmHeight-1*/, bm.bmWidth, bm.bmHeight, SRCCOPY);
		dcMemDest.MoveTo(0, 0);
		dcMemDest.LineTo(Rect.Width()-1, 0);
		dcMemDest.LineTo(Rect.Width()-1, Rect.Height()-1);
		dcMemDest.LineTo(0, Rect.Height()-1);
		dcMemDest.LineTo(0, 0);
		HBITMAP hBitmap = (HBITMAP) bitmap.Detach ();

		dcMemDest.SelectObject (pOldBitmapDest);
		dcMemSrc.SelectObject (pOldBitmapSrc);

		// Place the copy on the clipboard.
		::EmptyClipboard ();
		::SetClipboardData (CF_BITMAP, hBitmap);
		::CloseClipboard ();
	}
	else
		AfxMessageBox(_T("Copy to clipboard failed!"));

}

void CAscanDlg::SetRfFullWaveMode()
{
	ConfigRec.receiver.det_option[gChannel] = !ConfigRec.gates.det_option[gChannel][gGate];

	if ( ConfigRec.receiver.det_option[gChannel] == 0 )  //FW
	{
		if (ConfigRec.receiver.ascan_range[gChannel] < 20)
			ConfigRec.receiver.ascan_range[gChannel] = 20;
	}

	UpdateDlg();

	SendMsg(SET_ASCAN_PEAK_SEL_REG);
	SendMsg(SET_ASCAN_REGISTERS);
	//AfxMessageBox("Set Ascan Registers.");
}

void CAscanDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bMoveGate || m_bChangeGateRange || m_bMoveTcgCurve || m_bMoveRcvrGain)
		((CTscanDlg*) GetParent())->UpdateDlgs();

	CResizingDialog::OnLButtonUp(nFlags, point);
}

void CAscanDlg::DrawTcgCurve()
{
	int i;
	int x, y;
	COLORREF curveClr = RGB(255, 255, 0);
	float  ascan_delay = (float) (ConfigRec.receiver.ascan_delay[gChannel] - 250) / 50.f;  //in microseconds
	float  ascan_range = (float) ConfigRec.receiver.ascan_range[gChannel];         //in microseconds
	float  iTraceLength = (float) m_pDrawAscan->m_iTraceLength;

	if (!m_bShowTcgCurve)
	{
		m_bDrawTcgCurve = FALSE;
		return;
	}

	m_bDrawTcgCurve = TRUE;

	/* draw current TCG curve */
	for (i=0; i<256; i++)
	{
		if (m_pTcpThreadRxList->GetTcgTrigSel(gChannel) < 2)  //TCG triggered by Initial Pulse or off
			x = (int) ( iTraceLength * ((i*TCG_STEP_INCREMENT*(m_pTcpThreadRxList->GetTcgUpdateRate(gChannel)+1)) + 0.0f - ascan_delay) / ascan_range );
		else  //by Interface triggering
			x = m_nInterfaceTrig + (int) ( iTraceLength * ((i*TCG_STEP_INCREMENT*(m_pTcpThreadRxList->GetTcgUpdateRate(gChannel)+1))) / ascan_range );

		if ( (m_pTcpThreadRxList->GetTcgTrigSel(gChannel) == 1) || ((m_pTcpThreadRxList->GetTcgTrigSel(gChannel) == 2) && m_bInterfaceTrig) )
		{
			if (gChannel % 2 == 0)
				y = (int) ((m_pTcpThreadRxList->m_bufGainTable[i]&0x0000FFFF)/10);
			else
				y = (int) (((m_pTcpThreadRxList->m_bufGainTable[i]&0xFFFF0000)>>16)/10);
		}
		else
			y = (int) m_pTcpThreadRxList->GetRcvrGain(gChannel);

		if ( (i==0) || (i>252) )
			y = (int) m_pTcpThreadRxList->GetRcvrGain(gChannel);

		m_TcgGainTable[i].x = x;
		m_TcgGainTable[i].y = y;
	}
}

void CAscanDlg::OnExpand()
{
	 CRect rcDlg, rcMarker;
	 GetWindowRect(&rcDlg);
	 //m_nExpandedHeight = rcDlg.Height();

	 m_Devide.GetWindowRect(&rcMarker);

	 m_nNormalHeight = (rcMarker.top - rcDlg.top);
	 m_nExpandedHeight = m_nNormalHeight + m_nHeightDiff;

	 //GetWindowRect(&rcDlg);

	 if (m_bExpanded)
	 {
		  rcDlg.SetRect(rcDlg.left, rcDlg.top,
		   rcDlg.left + rcDlg.Width(),
		   rcDlg.top + m_nNormalHeight);
		  m_bShowTcgCurve = FALSE;
	 }
	 else
	 {
		  rcDlg.SetRect( rcDlg.left, rcDlg.top,
		   rcDlg.left + rcDlg.Width(),
		   rcDlg.top + m_nExpandedHeight);
		  m_bShowTcgCurve = TRUE;
	 }

	 CResizingDialog::m_bResizeDlg = FALSE;
	 MoveWindow(rcDlg, TRUE);
	 CResizingDialog::m_bResizeDlg = TRUE;

	 m_bExpanded = !m_bExpanded;

	 EnableExpandedControls(m_bExpanded);

	 CString str;
	 if (m_bExpanded)
		 str.Format(_T("<<"));
	 else
		 str.Format(_T(">>"));
	 SetDlgItemText(IDC_EXPAND, str);
}

void CAscanDlg::EnableExpandedControls(BOOL bEnabled)
{
	 HWND hWndChild = ::GetDlgItem(m_hWnd, IDC_EXPAND_BORDER);

	 while (hWndChild != NULL)
	 {
	  ::EnableWindow(hWndChild, bEnabled);
	  hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT);
	 }
}

void CAscanDlg::ContractDialog()
{
	 CRect rcDlg, rcMarker;
	 GetWindowRect(&rcDlg);
	 m_nExpandedHeight = rcDlg.Height();

	 m_Devide.GetWindowRect(&rcMarker);

	 m_nNormalHeight = (rcMarker.top - rcDlg.top);

	 m_nHeightDiff = m_nExpandedHeight - m_nNormalHeight;

	 rcDlg.SetRect( rcDlg.left, rcDlg.top,
	  rcDlg.left + rcDlg.Width(),
	  rcDlg.top + m_nNormalHeight);

	 CResizingDialog::m_bResizeDlg = FALSE;
	 MoveWindow(rcDlg, TRUE);
	 CResizingDialog::m_bResizeDlg = TRUE;

	 m_bExpanded = FALSE;

	 EnableExpandedControls(m_bExpanded);
}


void CAscanDlg::OnSize(UINT nType, int cx, int cy) 
{
	CResizingDialog::OnSize(nType, cx, cy);
	
	m_bClearBscan = TRUE;
	// TODO: Add your message handler code here
}

void CAscanDlg::OnRadioStep200() 
{
	// TODO: Add your control notification handler code here
	int nStep = GetCheckedRadioButton(IDC_RADIO_STEP200, IDC_RADIO_STEP800) - IDC_RADIO_STEP200;
	if ( nStep != 0) return;

	m_pTcpThreadRxList->SetTcgUpdateRate(gChannel, 0);
	m_nTcgStep = 0;

	m_pTcpThreadRxList->ComputeTcgGainTable(gChannel);
}

void CAscanDlg::OnRadioStep400() 
{
	// TODO: Add your control notification handler code here
	int nStep = GetCheckedRadioButton(IDC_RADIO_STEP200, IDC_RADIO_STEP800) - IDC_RADIO_STEP200;
	if ( nStep != 1) return;

	m_pTcpThreadRxList->SetTcgUpdateRate(gChannel, 1);
	m_nTcgStep = 1;

	m_pTcpThreadRxList->ComputeTcgGainTable(gChannel);
}

void CAscanDlg::OnRadioStep600() 
{
	// TODO: Add your control notification handler code here
	int nStep = GetCheckedRadioButton(IDC_RADIO_STEP200, IDC_RADIO_STEP800) - IDC_RADIO_STEP200;
	if ( nStep != 2) return;

	m_pTcpThreadRxList->SetTcgUpdateRate(gChannel, 2);
	m_nTcgStep = 2;

	m_pTcpThreadRxList->ComputeTcgGainTable(gChannel);
}

void CAscanDlg::OnRadioStep800() 
{
	// TODO: Add your control notification handler code here
	int nStep = GetCheckedRadioButton(IDC_RADIO_STEP200, IDC_RADIO_STEP800) - IDC_RADIO_STEP200;
	if ( nStep != 3) return;

	m_pTcpThreadRxList->SetTcgUpdateRate(gChannel, 3);
	m_nTcgStep = 3;

	m_pTcpThreadRxList->ComputeTcgGainTable(gChannel);
}

void CAscanDlg::OnRadioTcgmode1() 
{
	// TODO: Add your control notification handler code here
	m_nTcgMode = 0;
}

void CAscanDlg::OnRadioTcgmode2() 
{
	// TODO: Add your control notification handler code here
	m_nTcgMode = 1;
}

void CAscanDlg::OnRadioTcgmode3() 
{
	// TODO: Add your control notification handler code here
	m_nTcgMode = 2;
}

void CAscanDlg::OnRadioTrigIf() 
{
	// TODO: Add your control notification handler code here
	int nTrig = GetCheckedRadioButton(IDC_RADIO_TRIGOFF, IDC_RADIO_TRIGIF) - IDC_RADIO_TRIGOFF;
	if (nTrig != 2) return;

	unsigned short nOldTrig = m_pTcpThreadRxList->GetTcgTrigSel(gChannel);

	m_pTcpThreadRxList->SetTcgTrigSel(gChannel, 2);

	if (nOldTrig == 0)
	{
		m_pTcpThreadRxList->SendTcgFunction(gChannel);
	}
}

void CAscanDlg::OnRadioTrigIp() 
{
	// TODO: Add your control notification handler code here
	int nTrig = GetCheckedRadioButton(IDC_RADIO_TRIGOFF, IDC_RADIO_TRIGIF) - IDC_RADIO_TRIGOFF;
	if (nTrig != 1) return;

	unsigned short nOldTrig = m_pTcpThreadRxList->GetTcgTrigSel(gChannel);

	m_pTcpThreadRxList->SetTcgTrigSel(gChannel, 1);

	if (nOldTrig == 0)
	{
		m_pTcpThreadRxList->SendTcgFunction(gChannel);
	}
}

void CAscanDlg::OnRadioTrigOff() 
{
	// TODO: Add your control notification handler code here
	int nTrig = GetCheckedRadioButton(IDC_RADIO_TRIGOFF, IDC_RADIO_TRIGIF) - IDC_RADIO_TRIGOFF;
	if (nTrig != 0) return;

	unsigned short nOldTrig = m_pTcpThreadRxList->GetTcgTrigSel(gChannel);

	m_pTcpThreadRxList->SetTcgTrigSel(gChannel, 0);
	
	if (nOldTrig > 0)
	{
		SendMsg(RECEIVER_GAIN);
	}
}

void CAscanDlg::GetGainOfGate(short nChannel, short nGate)
{
	int iStart = (int) ( (m_pTcpThreadRxList->GetGateDelay(nChannel,nGate)+0.5f*m_pTcpThreadRxList->GetGateRange(nChannel,nGate))/
		((m_pTcpThreadRxList->GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
	if (iStart < 0) iStart = 0;
	if (iStart > 255) iStart = 255;
	int GainCounts;

	if ( (gChannel%2) == 0 )
		GainCounts = m_pTcpThreadRxList->m_bufGainTable[iStart] & 0x0000FFFF;
	else
		GainCounts = (m_pTcpThreadRxList->m_bufGainTable[iStart] & 0xFFFF0000) >> 16;

	m_fGain = m_pTcpThreadRxList->m_pConfigRec->receiver.TcgRec[nChannel].GateGain[nGate];
	m_fGain += m_pTcpThreadRxList->m_TruscopGainOffset/m_pTcpThreadRxList->m_TruscopGainScaling;
}

void CAscanDlg::OnBtnReset() 
{
	// TODO: Add your control notification handler code here
	m_pTcpThreadRxList->ReSetTcgGainTable(gChannel, TRUE);
	((CTscanDlg*) GetParent())->UpdateDlgs();
}

void CAscanDlg::OnFrozen() 
{
	// TODO: Add your control notification handler code here
	BOOL bRF = ConfigRec.receiver.det_option[gChannel];

	m_nLiveFrozenPeak++;

	if (bRF)
		m_nLiveFrozenPeak %= 2;
	else
		m_nLiveFrozenPeak %= 3;

	CString s;
	if (m_nLiveFrozenPeak == 0) //show live signal
		s.Format(_T("Live"));
	if (m_nLiveFrozenPeak == 1) //show frozen signal
		s.Format(_T("Frozen"));
	if (m_nLiveFrozenPeak == 2) //show peak signal
	{
		s.Format(_T("Peak"));
		for (int i=0; i<1000; i++)
			m_nAscan[i] = -128;
	}

	GetDlgItem(IDC_FROZEN)->SetWindowText(s);
}

void CAscanDlg::KillMe()
{
	m_pTcpThreadRxList->SendSlaveMsg(ASCAN_MODE, 0,0,0,0,0,0);
	delete m_pDrawAscan;

	KillTimer(ASCAN_TIMER);

	SaveWindowPosition(_T("A-Scan"));
	((CTscanDlg*) GetParent())->m_pAscanDlg = NULL;

	DestroyWindow();
}

void CAscanDlg::UpdateRefreshRate()
{
	m_nRefreshRate = ConfigRec.receiver.ascan_refreshrate;

	switch (m_nRefreshRate)
	{
	case 10:
		SetTimer(ASCAN_TIMER,100,NULL);
		break;

	case 20:
		SetTimer(ASCAN_TIMER,50,NULL);
		break;

	case 30:
		SetTimer(ASCAN_TIMER,30,NULL);
		break;

	case 60:
		SetTimer(ASCAN_TIMER,15,NULL);
		break;

	default:
		SetTimer(ASCAN_TIMER,15,NULL);
		break;
	}
}


void CAscanDlg::SaveFlawDataToFile()
{
	char sFileName[200], sFileTemp[200], ch[20];
	CString s;

	g_RawFlawHead.JobRec = ConfigRec.JobRec;
	g_RawFlawHead.fWall = ConfigRec.JobRec.Wall;
	g_RawFlawHead.fOD = ConfigRec.JobRec.OD;

	s = ConfigRec.JobRec.WO;
	CstringToChar(s,ch);
	if ( s.GetLength() )
		sprintf ((char *) sFileName, "C:/PhasedArray/FlawData/%s_%d", ch /*ConfigRec.JobRec.WO*/, g_RawFlawHead.nJointNum);
	else
		sprintf (sFileName, "host:/PhasedArray/FlawData/Unknown_%d", g_RawFlawHead.nJointNum);

	/* if the file name already exists, change it */
	int i = 2;
	sprintf (sFileTemp, "%s.flaw", sFileName);
	FILE *fDX;
	while (1)
	{
		fDX = fopen (sFileTemp, "r");
		if (fDX != NULL)  /* exist */
		{
			sprintf (sFileTemp, "%s_%d.flaw", sFileName, i);
			i++;
			fclose (fDX);
		}
		else
		{
			break;
		}
	}

	try
	{
		CString szFileTemp = sFileTemp;
		CFile file (szFileTemp, CFile::modeCreate | CFile::modeReadWrite);

		file.Write( (void *) &g_RawFlawHead, sizeof(RAW_WALL_HEAD) );
		file.Write( (void *) g_RawFlaw, g_RawFlawHead.nNumRev*sizeof(CSCAN_REVOLUTION) );
	}
    catch (CFileException* e)
    {
        e->ReportError();
        e->Delete();
    }
}
