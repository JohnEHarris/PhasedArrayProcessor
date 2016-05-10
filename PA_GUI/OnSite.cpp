// OnSite.cpp : implementation file
//

#include "stdafx.h"
//#include "Asi.h"
//#include "OnSite.h"
#include "Truscan.h"
#include "TscanDlg.h"
#include "math.h"

#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char  *ArrayScanType[16] = {"Compression Linear Scan", "Shear Linear Scan" ,"Left+Right+Wall 8 Beams", "Left+Right+Wall 16 Beams", 
"Left+Right 8 Beams", "Left+Right 16 Beams", "Straight 8 Beams (12345678)", "Straight 8 Beams (56781234)", 
"Straight 24 Beams (800)", "Straight 24 Beams (080)", "Left+Right+Wall 8 Beams Focusing", "Long+Oblq1 Left+Oblq1 Right 8 Beams (12345678)", 
"Long+Oblq1 Left+Oblq1 Right 8 Beams (56781234)", "Straight 8 Beams (12345678)", "Straight 8 Beams (56781234)", "Wall 25 Beams 90 Degree Probe"};

/////////////////////////////////////////////////////////////////////////////
// COnSite dialog


COnSite::COnSite(CWnd* pParent /*=NULL*/)
	: CDialog(COnSite::IDD, pParent)
{
	//{{AFX_DATA_INIT(COnSite)
	m_nWoffset = -1;
	m_nClkOffset = 0;
	m_nXoffset = 0;
	m_nXoffset2 = 0;
	m_nWallDropTime = 0;
	m_bRecordWall = FALSE;
	m_fLineSpeed = 0.0f;
	m_fRotateSpeed = 0.0f;
	m_b20ChnlPerHead = FALSE;
	m_nMaxWallWindowSize = 10;
	m_sTranAngle = _T("");
	m_sTranShearAngle = _T("");
	m_fFocusPointZf = 0.25f;
	m_fWaterPath = 1.0f;
	m_nEncoderDivider = 0;
	m_fMotionPulseLen = 0.5f;
	//}}AFX_DATA_INIT

	m_nArrayNumber = 0;
}


void COnSite::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnSite)
	DDX_Control(pDX, IDC_COMBO_BASEGAIN, m_cbBaseGain);
	DDX_Control(pDX, IDC_SB_TRANANGLE, m_sbTranAngle);
	DDX_Control(pDX, IDC_COMBO_ARRAYNUMBER, m_cbArrayNumber);
	DDX_Control(pDX, IDC_COMBO_ARRAYSCANTYPE, m_cbArrayScanType);
	DDX_Control(pDX, IDC_SB_WALLWINDOW, m_sbWallWindow);
	DDX_Control(pDX, IDC_SB_WALL_DROP, m_sbWallDrop);
	DDX_Control(pDX, IDC_SB_X_OFFSET2, m_sbX2);
	DDX_Control(pDX, IDC_SB_X_OFFSET, m_sbX);
	DDX_Control(pDX, IDC_SB_CLK_OFFSET, m_sbClk);
	DDX_Radio(pDX, IDC_RB_PLUS, m_nWoffset);
	DDX_Text(pDX, IDC_EN_CLK_OFFSET, m_nClkOffset);
	DDV_MinMaxInt(pDX, m_nClkOffset, -6, 6);
	DDX_Text(pDX, IDC_EN_X_OFFSET, m_nXoffset);
	DDV_MinMaxInt(pDX, m_nXoffset, -120, 120);
	DDX_Text(pDX, IDC_EN_X_OFFSET2, m_nXoffset2);
	DDV_MinMaxInt(pDX, m_nXoffset2, -120, 120);
	DDX_Text(pDX, IDC_EN_WALLDROPTIME, m_nWallDropTime);
	DDX_Check(pDX, IDC_CHECK_RECORDWALL, m_bRecordWall);
	DDX_Text(pDX, IDC_EDIT_FPM, m_fLineSpeed);
	DDV_MinMaxFloat(pDX, m_fLineSpeed, 0.5f, 600.f);
	DDX_Text(pDX, IDC_EDIT_RPM, m_fRotateSpeed);
	DDV_MinMaxFloat(pDX, m_fRotateSpeed, 10.f, 600.f);
	DDX_Check(pDX, IDC_CHECK_20HEAD, m_b20ChnlPerHead);
	DDX_Text(pDX, IDC_EN_WALLWINDOW, m_nMaxWallWindowSize);
	DDX_Text(pDX, IDC_EN_TRANANGLE, m_sTranAngle);
	DDX_Text(pDX, IDC_EN_TRANSHEARANGLE, m_sTranShearAngle);
	DDX_Text(pDX, IDC_EN_FOCALPOINT, m_fFocusPointZf);
	DDV_MinMaxFloat(pDX, m_fFocusPointZf, 0.f, 1000.f);
	DDX_Text(pDX, IDC_EN_WATERPATH, m_fWaterPath);
	DDV_MinMaxFloat(pDX, m_fWaterPath, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT_ENCODERDIVIDER, m_nEncoderDivider);
	DDV_MinMaxInt(pDX, m_nEncoderDivider, 0, 255);
	DDX_Text(pDX, IDC_EN_MOTIONPULSELEN, m_fMotionPulseLen);
	DDV_MinMaxFloat(pDX, m_fMotionPulseLen, 0.f, 1000.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COnSite, CDialog)
	//{{AFX_MSG_MAP(COnSite)
	ON_BN_CLICKED(IDC_RB_MINUS, OnRbMinus)
	ON_BN_CLICKED(IDC_RB_PLUS, OnRbPlus)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_RECORDWALL, OnCheckRecordwall)
	ON_BN_CLICKED(IDC_CHECK_20HEAD, OnCheck20ChnlPerHead)
	ON_CBN_SELCHANGE(IDC_COMBO_ARRAYSCANTYPE, OnSelchangeComboArrayScanType)
	ON_CBN_SELCHANGE(IDC_COMBO_ARRAYNUMBER, OnSelchangeComboArrayNumber)
	ON_CBN_SELCHANGE(IDC_COMBO_BASEGAIN, OnSelchangeComboBasegain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COnSite message handlers

BOOL COnSite::OnInitDialog() 
	{

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// Get existing values from registry
	pCTscanDlg->GetRegistrySiteDefaults();

	m_nClkOffset = SiteDefault.nDefaultClockOffset;
	m_nXoffset = SiteDefault.nDefaultXOffset;
	m_nXoffset2 = SiteDefault.nDefaultXOffset2;
	m_nWallDropTime = SiteDefault.nWallDropTime;
	m_nMaxWallWindowSize = SiteDefault.nMaxWallWindowSize;
	m_sTranAngle.Format(_T("%4.1f"), SiteDefault.nTranAngle / 10.0f);
	m_sTranShearAngle.Format(_T("%4.1f"), asin(sin(SiteDefault.nTranAngle / 10.0f * 3.14159f / 180.0f) * 3.2f / 1.483f) *180.0f / 3.14159f );
	m_nEncoderDivider = SiteDefault.nEncoderDivider;
	if (SiteDefault.nRecordWallData)
		m_bRecordWall = TRUE;
	else
		m_bRecordWall = FALSE;
	if (SiteDefault.nDefaultWOffset == 45)	m_nWoffset = 0;
	else m_nWoffset = 1;

	if (SiteDefault.n20ChnlPerHead)
		m_b20ChnlPerHead = TRUE;
	else
		m_b20ChnlPerHead = FALSE;

	m_fLineSpeed = ((float) SiteDefault.nDefaultLineSpeed)/100.0f;
	m_fRotateSpeed = ((float) SiteDefault.nDefaultRotateSpeed)/100.0f;

	m_sbClk.SetScrollRange(-6,6,TRUE);
	m_sbClk.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbClk.SetScrollPos(m_nClkOffset, TRUE);

	m_sbX.SetScrollRange(-120,120,TRUE);
	m_sbX.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbX.SetScrollPos(m_nXoffset, TRUE);

	m_sbX2.SetScrollRange(-120,120,TRUE);
	m_sbX2.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbX2.SetScrollPos(m_nXoffset2, TRUE);

	m_sbWallDrop.SetScrollRange(1,500,TRUE);
	m_sbWallDrop.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbWallDrop.SetScrollPos(m_nWallDropTime, TRUE);

	m_sbWallWindow.SetScrollRange(1,50,TRUE);
	m_sbWallWindow.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbWallWindow.SetScrollPos(m_nMaxWallWindowSize, TRUE);

	m_sbTranAngle.SetScrollRange(100,300,TRUE);
	m_sbTranAngle.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbTranAngle.SetScrollPos(SiteDefault.nTranAngle, TRUE);

#if ( (SYSTEM_ID == RAW_WALL_SYS) || (SYSTEM_SUB_ID == BESSEMER_SYS) )
	GetDlgItem(IDC_STATIC_STATION1)->SetWindowText(_T("Away"));
	GetDlgItem(IDC_STATIC_STATION2)->SetWindowText(_T("Toward"));
#else
	GetDlgItem(IDC_STATIC_STATION1)->SetWindowText(_T("Station 1"));
	GetDlgItem(IDC_STATIC_STATION2)->SetWindowText(_T("Station 2"));
#endif

	m_cbArrayScanType.SetCurSel(SiteDefault.nPhasedArrayScanType[m_nArrayNumber]);

	m_cbArrayNumber.SetCurSel(m_nArrayNumber);

	m_cbBaseGain.SetCurSel(SiteDefault.nAd9272PgaGain);

	m_fFocusPointZf = SiteDefault.fFocusPointZf;
	m_fWaterPath = SiteDefault.fWaterPath;

	m_fMotionPulseLen = SiteDefault.fMotionPulseLen;

	ListArrayScanType();

	UpdateData(FALSE);	// copy data to screen
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void COnSite::OnRbMinus() 
	{
	// Minus shoe angle from one to another corresponds to CLOCKWISE rotation
	UpdateData(TRUE);	/* copy screen to data */
	SiteDefault.nDefaultWOffset = -45;
	SiteDefault.bRotClkCountsUp = 1;	// up counter, cw rotation
	}

void COnSite::OnRbPlus() 
	{
	// Plus shoe angle from one to another corresponds to COUNTER CLOCKWISE rotation
	UpdateData(TRUE);	/* copy screen to data */	
	SiteDefault.nDefaultWOffset = 45;
	SiteDefault.bRotClkCountsUp = 0;	// down counter, ccw rotation
	}

void COnSite::OnCancel() 
	{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
	}

void COnSite::OnOK() 
	{
	// TODO: Add extra validation here
	// Store current values in the registry of the MMI
	UpdateData(TRUE);
	SiteDefault.nDefaultLineSpeed = (WORD) (m_fLineSpeed*100.0f);
	SiteDefault.nDefaultRotateSpeed = (WORD) (m_fRotateSpeed*100.0f);
	SiteDefault.fWaterPath = m_fWaterPath;
	SiteDefault.fFocusPointZf = m_fFocusPointZf;
	SiteDefault.nEncoderDivider = m_nEncoderDivider;
	SiteDefault.fMotionPulseLen = m_fMotionPulseLen;

	pCTscanDlg->SetRegistrySiteDefaults();
	
	pCTscanDlg->SendMsg(SITE_SPECIFIC_MSG);

	CDialog::OnOK();
	}





void COnSite::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
	case SB_PAGELEFT:
		nDelta = -1;
		break;

	case SB_LINERIGHT:
	case SB_PAGERIGHT:
		nDelta = 1;
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

	case IDC_SB_CLK_OFFSET:
		m_nClkOffset = SiteDefault.nDefaultClockOffset = nResult;
		break;

	case IDC_SB_X_OFFSET:
		m_nXoffset = SiteDefault.nDefaultXOffset = nResult;
		break;

	case IDC_SB_X_OFFSET2:
		m_nXoffset2 = SiteDefault.nDefaultXOffset2 = nResult;
		break;

	case IDC_SB_WALL_DROP:
		m_nWallDropTime = SiteDefault.nWallDropTime = nResult;
		break;

	case IDC_SB_WALLWINDOW:
		m_nMaxWallWindowSize = SiteDefault.nMaxWallWindowSize = nResult;
		break;

	case IDC_SB_TRANANGLE:
		SiteDefault.nTranAngle = nResult;
		m_sTranAngle.Format(_T("%4.1f"), nResult / 10.0f);
		m_sTranShearAngle.Format(_T("%4.1f"), asin(sin(SiteDefault.nTranAngle / 10.0f * 3.14159f / 180.0f) * 3.2f / 1.483f) *180.0f / 3.14159f );
		break;

	default:
		break;
		}
	
	UpdateData(FALSE);	// copy data to screen
	pScrollBar->SetScrollPos(nResult, TRUE);
	
	}

void COnSite::OnCheckRecordwall() 
{
	// TODO: Add your control notification handler code here
	m_bRecordWall ^= 1;

	if (m_bRecordWall)
		SiteDefault.nRecordWallData = 1;
	else
		SiteDefault.nRecordWallData = 0;
}

void COnSite::OnCheck20ChnlPerHead() 
{
	// TODO: Add your control notification handler code here
	m_b20ChnlPerHead ^= 1;

	if (m_b20ChnlPerHead)
		SiteDefault.n20ChnlPerHead = 1;
	else
		SiteDefault.n20ChnlPerHead = 0;
}

void COnSite::OnSelchangeComboArrayScanType() 
{
	// TODO: Add your control notification handler code here
	SiteDefault.nPhasedArrayScanType[m_nArrayNumber] = m_cbArrayScanType.GetCurSel();

	ListArrayScanType();
}

void COnSite::OnSelchangeComboArrayNumber() 
{
	// TODO: Add your control notification handler code here
	m_nArrayNumber = m_cbArrayNumber.GetCurSel();

	m_cbArrayScanType.SetCurSel( SiteDefault.nPhasedArrayScanType[m_nArrayNumber] );
}

void COnSite::ListArrayScanType()
{
	int nScanType;
	CString str;

	nScanType = SiteDefault.nPhasedArrayScanType[0];
	str.Format(_T("Array 01:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY1, str);

	nScanType = SiteDefault.nPhasedArrayScanType[1];
	str.Format(_T("Array 02:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY2, str);

	nScanType = SiteDefault.nPhasedArrayScanType[2];
	str.Format(_T("Array 03:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY3, str);

	nScanType = SiteDefault.nPhasedArrayScanType[3];
	str.Format(_T("Array 04:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY4, str);

	nScanType = SiteDefault.nPhasedArrayScanType[4];
	str.Format(_T("Array 05:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY5, str);

	nScanType = SiteDefault.nPhasedArrayScanType[5];
	str.Format(_T("Array 06:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY6, str);

	nScanType = SiteDefault.nPhasedArrayScanType[6];
	str.Format(_T("Array 07:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY7, str);

	nScanType = SiteDefault.nPhasedArrayScanType[7];
	str.Format(_T("Array 08:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY8, str);

	nScanType = SiteDefault.nPhasedArrayScanType[8];
	str.Format(_T("Array 09:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY9, str);

	nScanType = SiteDefault.nPhasedArrayScanType[9];
	str.Format(_T("Array 10:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY10, str);

	nScanType = SiteDefault.nPhasedArrayScanType[10];
	str.Format(_T("Array 11:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY11, str);

	nScanType = SiteDefault.nPhasedArrayScanType[11];
	str.Format(_T("Array 12:   %s"), ArrayScanType[nScanType]);
	SetDlgItemText(IDC_STATIC_ARRAY12, str);


}

void COnSite::OnSelchangeComboBasegain() 
{
	// TODO: Add your control notification handler code here
	SiteDefault.nAd9272PgaGain = m_cbBaseGain.GetCurSel();
}
