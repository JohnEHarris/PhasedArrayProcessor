// Nc.cpp : implementation file
//


#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

#include "Extern.h"
// #include "Nc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNc dialog


CNc::CNc(CWnd* pParent /*=NULL*/)
	: CDialog(CNc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNc)
	m_nLid = 2;
	m_nLod = 2;
	m_nQ1id = 2;
	m_nQ1od = 2;
	m_nQ2id = 2;
	m_nQ2od = 2;
	m_nQ3id = 2;
	m_nQ3od = 2;
	m_nTid = 2;
	m_nTod = 2;
	m_nTol = 0;
	m_nWallNx = 5;
	m_nLamid = 1;
	m_nLamod = 1;
	//}}AFX_DATA_INIT
}


void CNc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNc)
	DDX_Control(pDX, IDC_SB_NC_LAMOD, m_sbLamod);
	DDX_Control(pDX, IDC_SB_NC_LAMID, m_sbLamid);
	DDX_Control(pDX, IDC_SB_NC_Q2ID, m_sbQ2id);
	DDX_Control(pDX, IDC_SB_WALL_NX, m_sbWallNx);
	DDX_Control(pDX, IDC_SB_NC_TOL, m_sbTol);
	DDX_Control(pDX, IDC_SB_NC_TOD, m_sbTod);
	DDX_Control(pDX, IDC_SB_NC_TID, m_sbTid);
	DDX_Control(pDX, IDC_SB_NC_Q3OD, m_sbQ3od);
	DDX_Control(pDX, IDC_SB_NC_Q3ID, m_sbQ3id);
	DDX_Control(pDX, IDC_SB_NC_Q2OD, m_sbQ2od);
	DDX_Control(pDX, IDC_SB_NC_Q1OD, m_sbQ1od);
	DDX_Control(pDX, IDC_SB_NC_Q1ID, m_sbQ1id);
	DDX_Control(pDX, IDC_SB_NC_LOD, m_sbLod);
	DDX_Control(pDX, IDC_SB_NC_LID, m_sbLid);
	DDX_Text(pDX, IDC_ED_NC_LID, m_nLid);
	DDV_MinMaxInt(pDX, m_nLid, 1, 2);
	DDX_Text(pDX, IDC_ED_NC_LOD, m_nLod);
	DDV_MinMaxInt(pDX, m_nLod, 1, 2);
	DDX_Text(pDX, IDC_ED_NC_Q1ID, m_nQ1id);
	DDV_MinMaxInt(pDX, m_nQ1id, 1, 2);
	DDX_Text(pDX, IDC_ED_NC_Q1OD, m_nQ1od);
	DDV_MinMaxInt(pDX, m_nQ1od, 1, 2);
	DDX_Text(pDX, IDC_ED_NC_Q2ID, m_nQ2id);
	DDV_MinMaxInt(pDX, m_nQ2id, 1, 2);
	DDX_Text(pDX, IDC_ED_NC_Q2OD, m_nQ2od);
	DDV_MinMaxInt(pDX, m_nQ2od, 1, 2);
	DDX_Text(pDX, IDC_ED_NC_Q3ID, m_nQ3id);
	DDV_MinMaxInt(pDX, m_nQ3id, 1, 2);
	DDX_Text(pDX, IDC_ED_NC_Q3OD, m_nQ3od);
	DDV_MinMaxInt(pDX, m_nQ3od, 1, 2);
	DDX_Text(pDX, IDC_ED_NC_TID, m_nTid);
	DDV_MinMaxInt(pDX, m_nTid, 1, 2);
	DDX_Text(pDX, IDC_ED_NC_TOD, m_nTod);
	DDV_MinMaxInt(pDX, m_nTod, 1, 2);
	DDX_Text(pDX, IDC_ED_NC_TOL, m_nTol);
	DDV_MinMaxInt(pDX, m_nTol, 0, 4);
	DDX_Text(pDX, IDC_ED_WALL_NX, m_nWallNx);
	DDV_MinMaxInt(pDX, m_nWallNx, 1, 10);
	DDX_Text(pDX, IDC_ED_NC_LAMID, m_nLamid);
	DDV_MinMaxInt(pDX, m_nLamid, 1, 2);
	DDX_Text(pDX, IDC_ED_NC_LAMOD, m_nLamod);
	DDV_MinMaxInt(pDX, m_nLamod, 1, 2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNc, CDialog)
	//{{AFX_MSG_MAP(CNc)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(ID_SAVE_CONFIG, OnSaveConfig)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNc message handlers

void CNc::PostNcDestroy() 
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

BOOL CNc::OnInitDialog() 
	{
	CScrollBar *pSb[12] = { &m_sbLid, &m_sbLod, &m_sbTid, &m_sbTod, &m_sbQ1id, &m_sbQ1od,
							&m_sbQ2id, &m_sbQ2od, &m_sbQ3id, &m_sbQ3od, &m_sbLamid, &m_sbLamod };

	int *pEn[12] = { &m_nLid, &m_nLod, &m_nTid, &m_nTod,&m_nQ1id, &m_nQ1od,
						&m_nQ2id, &m_nQ2od,&m_nQ3id, &m_nQ3od, &m_nLamid, &m_nLamod};
	int i;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	GetConfigRec();
	for ( i = 0; i < 12; i++)
		{	// init nc scroll bars 
		pSb[i]->SetScrollRange(1,2,TRUE);
		pSb[i]->EnableScrollBar(ESB_ENABLE_BOTH);
		pSb[i]->SetScrollPos(*pEn[i],TRUE);
		}

	m_sbTol.SetScrollRange(0,3,TRUE);
	m_sbTol.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbTol.SetScrollPos(m_nTol, TRUE);

	m_sbWallNx.SetScrollRange(1,10,TRUE);
	m_sbWallNx.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbWallNx.SetScrollPos(m_nWallNx, TRUE);
	
	UpdateData(FALSE);	// copy data to screen

	SetDefID(-1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CNc::OnCancel() 
	{
	// TODO: Add extra cleanup here
	UpdateConfigRec();

	SendMsg(SET_NC_NX);	// Sets all nc's and Nx 
	if (  pCTscanDlg->m_pInspectDlg1 )
		pCTscanDlg->m_pInspectDlg1->OnUpdateData();
	if (  pCTscanDlg->m_pInspectDlg2 )
		pCTscanDlg->m_pInspectDlg2->OnUpdateData();

	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CNc::OnOK() 
	{
	// TODO: Add extra validation here
	UpdateConfigRec();

	SendMsg(SET_NC_NX);	// Sets all nc's and Nx 
	if (  pCTscanDlg->m_pInspectDlg1 )
		pCTscanDlg->m_pInspectDlg1->OnUpdateData();
	if (  pCTscanDlg->m_pInspectDlg2 )
		pCTscanDlg->m_pInspectDlg2->OnUpdateData();

	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

void CNc::KillMe()
	{
	// Public access to OnCancel
	OnCancel();
	}


void CNc::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	// TODO: Add your message handler code here and/or call default
	int nStart, nDelta;		// where the bar started and amount of change
	int nResult;			// normally start + delta
	int nMax, nMin;
	int nCtlId;



	nStart = pScrollBar->GetScrollPos();
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
		nDelta = -5;
		break;

	case SB_PAGERIGHT:
		nDelta = 5;
		break;

	case SB_THUMBTRACK:
		nDelta = (int)nPos - nStart;
		break;


	default:
		return;
		break;

		}	// switch on type of motion

	nResult = nStart;
	nResult += nDelta;
	if ( nResult > nMax) nResult = nMax;
	if ( nResult < nMin) nResult = nMin;


	// Now determine which scroll bar was used
	nCtlId = pScrollBar->GetDlgCtrlID();



	switch(nCtlId)
		{

	case IDC_SB_NC_LID:		m_nLid = nResult;		break;
	case IDC_SB_NC_LOD:		m_nLod = nResult;		break;
	case IDC_SB_NC_TID:		m_nTid = nResult;		break;
	case IDC_SB_NC_TOD:		m_nTod = nResult;		break;
	case IDC_SB_NC_Q1ID:	m_nQ1id = nResult;		break;
	case IDC_SB_NC_Q1OD:	m_nQ1od = nResult;		break;
	case IDC_SB_NC_Q2ID:	m_nQ2id = nResult;		break;
	case IDC_SB_NC_Q2OD:	m_nQ2od = nResult;		break;
	case IDC_SB_NC_Q3ID:	m_nQ3id = nResult;		break;
	case IDC_SB_NC_Q3OD:	m_nQ3od = nResult;		break;
	case IDC_SB_NC_LAMID:	m_nLamid = nResult;		break;
	case IDC_SB_NC_LAMOD:	m_nLamod = nResult;		break;
	case IDC_SB_WALL_NX:	m_nWallNx = nResult;	break;
	case IDC_SB_NC_TOL:		m_nTol = nResult;		break;
	
	
	default:
		break;
		}

		
	pScrollBar->SetScrollPos(nResult, TRUE);

	UpdateConfigRec();

	UpdateData(FALSE);	// copy data to screen
	SendMsg(SET_NC_NX);	// Sets all nc's and Nx 

	}

BOOL CNc::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Tscan dlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CNc::GetConfigRec()
	{
	m_nLid = ConfigRec.UtRec.NcLong[0];
	m_nLod = ConfigRec.UtRec.NcLong[1];
	m_nTid = ConfigRec.UtRec.NcTran[0];
	m_nTod = ConfigRec.UtRec.NcTran[1];
	m_nQ1id = ConfigRec.UtRec.NcOblq1[0];
	m_nQ1od = ConfigRec.UtRec.NcOblq1[1];
	m_nQ2id = ConfigRec.UtRec.NcOblq2[0];
	m_nQ2od = ConfigRec.UtRec.NcOblq2[1];
	m_nQ3id = ConfigRec.UtRec.NcOblq3[0];
	m_nQ3od = ConfigRec.UtRec.NcOblq3[1];
	m_nLamid = ConfigRec.UtRec.NcLamin[0];
	m_nLamod = ConfigRec.UtRec.NcLamin[1];
	m_nWallNx = ConfigRec.UtRec.NxWall[0];
	m_nTol = ConfigRec.UtRec.Tol;

	}

void CNc::UpdateConfigRec()
	{
	ConfigRec.UtRec.NcLong[0] = m_nLid;
	ConfigRec.UtRec.NcLong[1] = m_nLod;
	ConfigRec.UtRec.NcTran[0] = m_nTid;
	ConfigRec.UtRec.NcTran[1] = m_nTod;
	ConfigRec.UtRec.NcOblq1[0] = m_nQ1id;
	ConfigRec.UtRec.NcOblq1[1] = m_nQ1od;
	ConfigRec.UtRec.NcOblq2[0] = m_nQ2id;
	ConfigRec.UtRec.NcOblq2[1] = m_nQ2od;
	ConfigRec.UtRec.NcOblq3[0] = m_nQ3id;
	ConfigRec.UtRec.NcOblq3[1] = m_nQ3od;
	ConfigRec.UtRec.NcLamin[0] = m_nLamid;
	ConfigRec.UtRec.NcLamin[1] = m_nLamod;
	ConfigRec.UtRec.NxWall[0] = m_nWallNx;
	ConfigRec.UtRec.Tol = m_nTol;

	}

void CNc::OnSaveConfig() 
	{
	// TODO: Add your control notification handler code here
	UpdateConfigRec();
	if (pCTscanDlg) pCTscanDlg->SaveConfigRec();
	}
