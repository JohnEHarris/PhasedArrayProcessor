// Tholds.cpp : implementation file
//
/*
01/24/00	Take thold values from config record read in
03/06/00	Mods to allow thold dialog to run either as modal or
			modeless.  Original version is modeless.  Interface
			to replay of pipe data is modal.
07/05/00	Change from asi code to ut code.  Tholds can be same
			for top (od) and bottom (id) traces.  All tholds ref
			elements of configrec.

*/


#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

//include "Tholds.h"	included in asidlg.h

#include "Extern.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTholds dialog


CTholds::CTholds(CWnd* pParent /*=NULL*/)
	: CDialog(CTholds::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTholds)
	m_nWallMax = 1;
	m_nWallMin = -1;
	m_nLid = 1;
	m_nLod = 1;
	m_nQ1id = 1;
	m_nQ1od = 1;
	m_nQ2id = 1;
	m_nQ2od = 1;
	m_nQ3id = 1;
	m_nQ3od = 1;
	m_nTid = 1;
	m_nTod = 1;
	m_nLamid = 1;
	m_nLamod = 1;
	//}}AFX_DATA_INIT
}


void CTholds::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTholds)
	DDX_Control(pDX, IDC_SB_LAMOD, m_sbLamod);
	DDX_Control(pDX, IDC_SB_LAMID, m_sbLamid);
	DDX_Control(pDX, IDC_SB_Q3OD, m_sbQ3od);
	DDX_Control(pDX, IDC_SB_Q3ID, m_sbQ3id);
	DDX_Control(pDX, IDC_SB_Q2OD, m_sbQ2od);
	DDX_Control(pDX, IDC_SB_Q2ID, m_sbQ2id);
	DDX_Control(pDX, IDC_SB_Q1OD, m_sbQ1od);
	DDX_Control(pDX, IDC_SB_Q1ID, m_sbQ1id);
	DDX_Control(pDX, IDC_SB_WALL_MIN, m_sbWallMin);
	DDX_Control(pDX, IDC_SB_WALL_MAX, m_sbWallMax);
	DDX_Control(pDX, IDC_SB_TOD, m_sbTod);
	DDX_Control(pDX, IDC_SB_TID, m_sbTid);
	DDX_Control(pDX, IDC_SB_LOD, m_sbLod);
	DDX_Control(pDX, IDC_SB_LID, m_sbLid);
	DDX_Text(pDX, IDC_ED_WALL_MAX, m_nWallMax);
	DDV_MinMaxInt(pDX, m_nWallMax, 1, 25);
	DDX_Text(pDX, IDC_ED_WALL_MIN, m_nWallMin);
	DDV_MinMaxInt(pDX, m_nWallMin, -25, -1);
	DDX_Text(pDX, IDC_ED_LID, m_nLid);
	DDV_MinMaxInt(pDX, m_nLid, 1, 100);
	DDX_Text(pDX, IDC_ED_LOD, m_nLod);
	DDV_MinMaxInt(pDX, m_nLod, 1, 100);
	DDX_Text(pDX, IDC_ED_Q1ID, m_nQ1id);
	DDV_MinMaxInt(pDX, m_nQ1id, 1, 100);
	DDX_Text(pDX, IDC_ED_Q1OD, m_nQ1od);
	DDV_MinMaxInt(pDX, m_nQ1od, 1, 100);
	DDX_Text(pDX, IDC_ED_Q2ID, m_nQ2id);
	DDV_MinMaxInt(pDX, m_nQ2id, 1, 100);
	DDX_Text(pDX, IDC_ED_Q2OD, m_nQ2od);
	DDV_MinMaxInt(pDX, m_nQ2od, 1, 100);
	DDX_Text(pDX, IDC_ED_Q3ID, m_nQ3id);
	DDV_MinMaxInt(pDX, m_nQ3id, 1, 100);
	DDX_Text(pDX, IDC_ED_Q3OD, m_nQ3od);
	DDV_MinMaxInt(pDX, m_nQ3od, 1, 100);
	DDX_Text(pDX, IDC_ED_TID, m_nTid);
	DDV_MinMaxInt(pDX, m_nTid, 1, 100);
	DDX_Text(pDX, IDC_ED_TOD, m_nTod);
	DDV_MinMaxInt(pDX, m_nTod, 1, 100);
	DDX_Text(pDX, IDC_ED_LAMID, m_nLamid);
	DDV_MinMaxInt(pDX, m_nLamid, 1, 100);
	DDX_Text(pDX, IDC_ED_LAMOD, m_nLamod);
	DDV_MinMaxInt(pDX, m_nLamod, 1, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTholds, CDialog)
	//{{AFX_MSG_MAP(CTholds)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(ID_SAVE_CONFIG, OnSaveConfig)
	ON_BN_CLICKED(IDOK, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTholds message handlers

BOOL CTholds::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// 1-21-00
	CString s;

	m_nWallMin = ConfigRec.UtRec.TholdWall[0];	// thin is negative %
	m_nWallMax = ConfigRec.UtRec.TholdWall[1];

	m_nLid = ConfigRec.UtRec.TholdLong[0][0];
	m_nLod = ConfigRec.UtRec.TholdLong[0][1];

	m_nTid = ConfigRec.UtRec.TholdLong[1][0];
	m_nTod = ConfigRec.UtRec.TholdLong[1][1];

	m_nQ1id = ConfigRec.UtRec.TholdLong[2][0];
	m_nQ1od = ConfigRec.UtRec.TholdLong[2][1];

	m_nQ2id = ConfigRec.UtRec.TholdLong[3][0];
	m_nQ2od = ConfigRec.UtRec.TholdLong[3][1];

	m_nQ3id = ConfigRec.UtRec.TholdLong[4][0];
	m_nQ3od = ConfigRec.UtRec.TholdLong[4][1];

	m_nLamid = ConfigRec.UtRec.TholdLong[5][0];
	m_nLamod = ConfigRec.UtRec.TholdLong[5][1];

	m_sbWallMin.SetScrollRange(-MAX_DELTA_WALL,-1,TRUE);
	m_sbWallMin.SetScrollPos(m_nWallMin);
	m_sbWallMin.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbWallMax.SetScrollRange(1,MAX_DELTA_WALL,TRUE);
	m_sbWallMax.SetScrollPos(m_nWallMax);
	m_sbWallMax.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbLod.SetScrollRange(1,100,TRUE);
	m_sbLod.SetScrollPos(m_nLod);
	m_sbLod.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbLid.SetScrollRange(1,100,TRUE);
	m_sbLid.SetScrollPos(m_nLid);
	m_sbLid.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbTod.SetScrollRange(1,100,TRUE);
	m_sbTod.SetScrollPos(m_nTod);
	m_sbTod.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbTid.SetScrollRange(1,100,TRUE);
	m_sbTid.SetScrollPos(m_nTid);
	m_sbTid.EnableScrollBar(ESB_ENABLE_BOTH);

	// oblq1
	m_sbQ1od.SetScrollRange(1,100,TRUE);
	m_sbQ1od.SetScrollPos(m_nQ1od);
	m_sbQ1od.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbQ1id.SetScrollRange(1,100,TRUE);
	m_sbQ1id.SetScrollPos(m_nQ1id);
	m_sbQ1id.EnableScrollBar(ESB_ENABLE_BOTH);

	// oblq2
#if (SYSTEM_ID == RAW_WALL_SYS)
	m_sbQ2od.SetScrollRange(1,20,TRUE);
#else
	m_sbQ2od.SetScrollRange(1,100,TRUE);
#endif
	m_sbQ2od.SetScrollPos(m_nQ2od);
	m_sbQ2od.EnableScrollBar(ESB_ENABLE_BOTH);

#if (SYSTEM_ID == RAW_WALL_SYS)
	m_sbQ2id.SetScrollRange(1,20,TRUE);
#else
	m_sbQ2id.SetScrollRange(1,100,TRUE);
#endif
	m_sbQ2id.SetScrollPos(m_nQ2id);
	m_sbQ2id.EnableScrollBar(ESB_ENABLE_BOTH);

	// oblq3
	m_sbQ3od.SetScrollRange(1,100,TRUE);
	m_sbQ3od.SetScrollPos(m_nQ3od);
	m_sbQ3od.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbQ3id.SetScrollRange(1,100,TRUE);
	m_sbQ3id.SetScrollPos(m_nQ3id);
	m_sbQ3id.EnableScrollBar(ESB_ENABLE_BOTH);

	// laminar
	m_sbLamod.SetScrollRange(1,100,TRUE);
	m_sbLamod.SetScrollPos(m_nLamod);
	m_sbLamod.EnableScrollBar(ESB_ENABLE_BOTH);

	m_sbLamid.SetScrollRange(1,100,TRUE);
	m_sbLamid.SetScrollPos(m_nLamid);
	m_sbLamid.EnableScrollBar(ESB_ENABLE_BOTH);

	UpdateData(FALSE);	// Copy variables to screen

	SetDefID(-1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CTholds::OnCancel() 
	{
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CTholds::OnOK() 
	{
	// Send UT thold msg to update inspection screen
	int i;

	ConfigRec.UtRec.TholdWallThds[0] = (short) ((float)(100 + ConfigRec.UtRec.TholdWall[0]) *
													ConfigRec.JobRec.Wall * 10.0f + 0.5f);
	ConfigRec.UtRec.TholdWallThds[1] = (short) ((float)(100 + ConfigRec.UtRec.TholdWall[1]) *
													ConfigRec.JobRec.Wall * 10.0f + 0.5f);
	// wall in thousandths from % depth X wall in inches

	if (  pCTscanDlg->m_pInspectDlg1 && CTholds::m_pDlg)
		{	// modeless window
		for ( i = 0; i < 7; i++) DrawThold(i);
		SendMsg(SET_ALL_THOLDS);		// Set all thresholds
//		pCTscanDlg->m_pInspectDlg1->m_nDelayedErase = 3;
		}

//	CDialog::OnOK();
//	CDialog::DestroyWindow();
	}

void CTholds::KillMe() 
	{
	// Public access to OnCancel
	OnCancel();
	}

void CTholds::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Same code for all windows
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	// Tholds can be modal or modeless dialog
	// if modeless destroy this
	if (m_pDlg)
		{
		m_pDlg = NULL;
		delete this;
		}
	}

void CTholds::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	// TODO: Add your message handler code here and/or call default

	// CODE IS DIRECTION OF SCROLL MOTION
	int *nStart, nDelta;		// where the bar started and amount of change
	int nMax, nMin;
	int nCtlId;
	int dummy = 5;
	CString s;

//	float fStart;			// floating scroll bar start point



	nCtlId = pScrollBar->GetDlgCtrlID();

	switch (nCtlId)
		{
	case IDC_SB_WALL_MIN:
		nStart = &m_nWallMin;
		break;

	case IDC_SB_WALL_MAX:
		nStart = &m_nWallMax;
		break;

	case IDC_SB_LID:
		nStart = &m_nLid;
		break;

	case IDC_SB_LOD:
		nStart = &m_nLod;
		break;

	case IDC_SB_TID:
		nStart = &m_nTid;
		break;

	case IDC_SB_TOD:
		nStart = &m_nTod;
		break;

		// oblq1
	case IDC_SB_Q1ID:
		nStart = &m_nQ1id;
		break;

	case IDC_SB_Q1OD:
		nStart = &m_nQ1od;
		break;

		// oblq2
	case IDC_SB_Q2ID:
		nStart = &m_nQ2id;
		break;

	case IDC_SB_Q2OD:
		nStart = &m_nQ2od;
		break;

		// oblq3
	case IDC_SB_Q3ID:
		nStart = &m_nQ3id;
		break;

	case IDC_SB_Q3OD:
		nStart = &m_nQ3od;
		break;

		// laminar
	case IDC_SB_LAMID:
		nStart = &m_nLamid;
		break;

	case IDC_SB_LAMOD:
		nStart = &m_nLamod;
		break;

	default:
		nStart = &dummy;
		break;

		}

	*nStart = pScrollBar->GetScrollPos();
	pScrollBar->GetScrollRange(&nMin, &nMax);

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
		nDelta = (int)nPos - *nStart;
		break;


	default:
		return;
		break;

		}	// switch on type of motion

	*nStart += nDelta;
	if ( *nStart > nMax) *nStart = nMax;
	if ( *nStart < nMin) *nStart = nMin;


	pScrollBar->SetScrollPos(*nStart, TRUE);

	UpdateData(FALSE);

	switch (nCtlId)
		{
	case IDC_SB_WALL_MIN:
	case IDC_SB_WALL_MAX:
		ConfigRec.UtRec.TholdWall[0] = (char) (m_nWallMin & 0xff);
		ConfigRec.UtRec.TholdWall[1] = m_nWallMax;
		DrawThold(0);	// wall
		break;

	case IDC_SB_LID:
	case IDC_SB_LOD:
		ConfigRec.UtRec.TholdLong[0][0] = m_nLid;
		ConfigRec.UtRec.TholdLong[0][1] = m_nLod;
		DrawThold(1);	//long
		break;

	case IDC_SB_TID:
	case IDC_SB_TOD:
		ConfigRec.UtRec.TholdLong[1][0] = m_nTid;
		ConfigRec.UtRec.TholdLong[1][1] = m_nTod;
		DrawThold(2);
		break;

		// oblq1
	case IDC_SB_Q1ID:
	case IDC_SB_Q1OD:
		ConfigRec.UtRec.TholdLong[2][0] = m_nQ1id;
		ConfigRec.UtRec.TholdLong[2][1] = m_nQ1od;
		DrawThold(3);	
		break;

		// oblq2
	case IDC_SB_Q2ID:
	case IDC_SB_Q2OD:
		ConfigRec.UtRec.TholdLong[3][0] = m_nQ2id;
		ConfigRec.UtRec.TholdLong[3][1] = m_nQ2od;
		DrawThold(4);	
		break;

		// oblq3
	case IDC_SB_Q3ID:
	case IDC_SB_Q3OD:
		ConfigRec.UtRec.TholdLong[4][0] = m_nQ3id;
		ConfigRec.UtRec.TholdLong[4][1] = m_nQ3od;
		DrawThold(5);	
		break;

		// laminar
	case IDC_SB_LAMID:
	case IDC_SB_LAMOD:
		ConfigRec.UtRec.TholdLong[5][0] = m_nLamid;
		ConfigRec.UtRec.TholdLong[5][1] = m_nLamod;
		DrawThold(6);	
		break;

	default:
		break;
		}
		
//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	
	}

void CTholds::OnQuestionable() 
	{
	// TODO: Add your control notification handler code here
	int i;

	UpdateData(TRUE);	// Copy screen TO variables 
//	ConfigRec.MapTholdSel = m_nMapThold;
	for ( i = 0; i < 7; i++) DrawThold(i);	
	}

void CTholds::OnReject() 
	{
	// TODO: Add your control notification handler code here
	int i;

	UpdateData(TRUE);	// Copy screen TO variables 
//	ConfigRec.MapTholdSel = m_nMapThold;
	for ( i = 0; i < 7; i++) DrawThold(i);	
	}

void CTholds::OnSaveConfig() 
	{
	// TODO: Add your control notification handler code here
	// Call public fils saveas operation from main asi dlg

//	ConfigRec.MapTholdSel = m_nMapThold;
	if (pCTscanDlg) pCTscanDlg->SaveConfigRec();
	}

BOOL CTholds::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Asidlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}


void CTholds::DrawThold(int Thold)
{
	if (pCTscanDlg->m_pInspectDlg1)
		pCTscanDlg->m_pInspectDlg1->DrawThold(Thold);

	if (pCTscanDlg->m_pInspectDlg2)
		pCTscanDlg->m_pInspectDlg2->DrawThold(Thold);
}


void CTholds::OnApply() 
{
	// TODO: Add your control notification handler code here
	int i;

	ConfigRec.UtRec.TholdWallThds[0] = (short) ((float)(100 + ConfigRec.UtRec.TholdWall[0]) *
													ConfigRec.JobRec.Wall * 10.0f + 0.5f);
	ConfigRec.UtRec.TholdWallThds[1] = (short) ((float)(100 + ConfigRec.UtRec.TholdWall[1]) *
													ConfigRec.JobRec.Wall * 10.0f + 0.5f);
	// wall in thousandths from % depth X wall in inches

	if (  pCTscanDlg->m_pInspectDlg1 )
		{	// modeless window
		for ( i = 0; i < 7; i++) DrawThold(i);
		SendMsg(SET_ALL_THOLDS);		// Set all thresholds
//		pCTscanDlg->m_pInspectDlg1->m_nDelayedErase = 3;
		}
	CDialog::OnOK();	
}
