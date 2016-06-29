// WcJob.cpp : implementation file
//	03/01/00 imported from well check project by jeh

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

//#include "WcJob.h"


#include <string.h>
#include "extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Here is our static variable 'instantiation' point
//CWcJob*  CWcJob::m_pDlg;	// jeh




// Global c function

void QualifyJobRecord(void)
	{
	int n = sizeof(TCHAR);

	// Make sure string loaded from config file are null terminated
	ConfigRec.JobRec.BusUnit[sizeof(ConfigRec.JobRec.BusUnit)/n		-1] = 0;
	ConfigRec.JobRec.Comment[sizeof(ConfigRec.JobRec.Comment)/n		-1] = 0;
	ConfigRec.JobRec.Customer[sizeof(ConfigRec.JobRec.Customer)/n	-1] = 0;
	ConfigRec.JobRec.Date[sizeof(ConfigRec.JobRec.Date)/n			-1] = 0;
	ConfigRec.JobRec.Location[sizeof(ConfigRec.JobRec.Location)/n	-1] = 0;
	ConfigRec.JobRec.Operator[sizeof(ConfigRec.JobRec.Operator)/n	-1] = 0;
	ConfigRec.JobRec.WellFoot[sizeof(ConfigRec.JobRec.WellFoot)/n	-1] = 0;
	ConfigRec.JobRec.WellName[sizeof(ConfigRec.JobRec.WellName)/n	-1] = 0;
	ConfigRec.JobRec.WO[sizeof(ConfigRec.JobRec.WO)/n				-1] = 0;
	ConfigRec.JobRec.Oblq1Name[sizeof(ConfigRec.JobRec.Oblq1Name)/n	-1] = 0;
	ConfigRec.JobRec.Oblq2Name[sizeof(ConfigRec.JobRec.Oblq2Name)/n	-1] = 0;
	ConfigRec.JobRec.Oblq3Name[sizeof(ConfigRec.JobRec.Oblq3Name)/n	-1] = 0;
	ConfigRec.JobRec.RbNum[sizeof(ConfigRec.JobRec.RbNum)/n			-1] = 0;
	}



////////////////////////////////////////////////////////////////////////////
// CWcJob dialog


CWcJob::CWcJob(CWnd* pParent /*=NULL*/)
	: CDialog(CWcJob::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWcJob)
	m_szLocation = _T("");
	m_szWO = _T("");
	m_szComment = _T("");
	m_szDate = _T("");
	m_szBusiness = _T("");
	m_szOperator = _T("");
	m_szWellFoot = _T("");
	m_szCustomer = _T("");
	m_nWallOption = -1;
	m_szHeat = _T("");
	m_szLotNum = _T("");
	m_szRbNum = _T("");
	m_szOblq1Name = _T("");
	m_szOblq2Name = _T("");
	m_szOblq3Name = _T("");
	m_bWallDeviation = FALSE;
	m_fWall1Offset = 0.0f;
	m_fWall2Offset = 0.0f;
	m_fWall3Offset = 0.0f;
	m_fWall4Offset = 0.0f;
	//}}AFX_DATA_INIT
}


void CWcJob::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWcJob)
	DDX_Text(pDX, IDC_JOB_LOCATION, m_szLocation);
	DDV_MaxChars(pDX, m_szLocation, 38);
	DDX_Text(pDX, IDC_JOB_WO, m_szWO);
	DDV_MaxChars(pDX, m_szWO, 9);
	DDX_Text(pDX, IDC_JOB_COMMENT, m_szComment);
	DDV_MaxChars(pDX, m_szComment, 79);
	DDX_Text(pDX, IDC_JOB_DATE, m_szDate);
	DDV_MaxChars(pDX, m_szDate, 9);
	DDX_Text(pDX, IDC_JOB_BUSINESS_UNIT, m_szBusiness);
	DDV_MaxChars(pDX, m_szBusiness, 30);
	DDX_Text(pDX, IDC_JOB_OPERATOR, m_szOperator);
	DDV_MaxChars(pDX, m_szOperator, 9);
	DDX_Text(pDX, IDC_JOB_WELL_FT, m_szWellFoot);
	DDV_MaxChars(pDX, m_szWellFoot, 10);
	DDX_Text(pDX, IDC_JOB_CUSTOMER, m_szCustomer);
	DDV_MaxChars(pDX, m_szCustomer, 31);
	DDX_Radio(pDX, IDC_RB_WALL_NONE, m_nWallOption);
	DDX_Text(pDX, IDC_JOB_HEAT, m_szHeat);
	DDV_MaxChars(pDX, m_szHeat, 32);
	DDX_Text(pDX, IDC_JOB_LOTNUM, m_szLotNum);
	DDV_MaxChars(pDX, m_szLotNum, 9);
	DDX_Text(pDX, IDC_JOB_RBNUM, m_szRbNum);
	DDV_MaxChars(pDX, m_szRbNum, 9);
	DDX_Text(pDX, IDC_JOB_Q1, m_szOblq1Name);
	DDV_MaxChars(pDX, m_szOblq1Name, 4);
	DDX_Text(pDX, IDC_JOB_Q2, m_szOblq2Name);
	DDV_MaxChars(pDX, m_szOblq2Name, 4);
	DDX_Text(pDX, IDC_JOB_Q3, m_szOblq3Name);
	DDV_MaxChars(pDX, m_szOblq3Name, 4);
	DDX_Check(pDX, IDC_CK_WALLDEVIATION, m_bWallDeviation);
	DDX_Text(pDX, IDC_EDIT_WALL1OFFSET, m_fWall1Offset);
	DDV_MinMaxFloat(pDX, m_fWall1Offset, 0.f, 60.f);
	DDX_Text(pDX, IDC_EDIT_WALL2OFFSET, m_fWall2Offset);
	DDV_MinMaxFloat(pDX, m_fWall2Offset, 0.f, 60.f);
	DDX_Text(pDX, IDC_EDIT_WALL3OFFSET, m_fWall3Offset);
	DDV_MinMaxFloat(pDX, m_fWall3Offset, 0.f, 60.f);
	DDX_Text(pDX, IDC_EDIT_WALL4OFFSET, m_fWall4Offset);
	DDV_MinMaxFloat(pDX, m_fWall4Offset, 0.f, 60.f);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CWcJob, CDialog)
	//{{AFX_MSG_MAP(CWcJob)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_CK_WALLDEVIATION, OnCkWallDeviation)
	//}}AFX_MSG_MAP


	// manually added to hadle wall radio buttons
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RB_WALL_NONE,
	IDC_RB_WALL_OPT4, DoWallOptionRadioButton)		

	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWcJob message handlers

void CWcJob::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Same code for all windows
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}


void CWcJob::OnOK() 
	{
	// TODO: Add extra validation here

	// Update info in config record
	UpdateData(TRUE);	// Move from screen into class variables

#if 1
	CstringToTChar(m_szBusiness,ConfigRec.JobRec.BusUnit);
	CstringToTChar( m_szComment,ConfigRec.JobRec.Comment);	
	CstringToTChar( m_szCustomer,ConfigRec.JobRec.Customer);	
	CstringToTChar( m_szDate,ConfigRec.JobRec.Date);		
	CstringToTChar( m_szLocation,ConfigRec.JobRec.Location);	
	CstringToTChar( m_szOperator,ConfigRec.JobRec.Operator);	
	CstringToTChar( m_szWellFoot,ConfigRec.JobRec.WellFoot);	
//	CstringToTChar( m_szWellName,ConfigRec.JobRec.WellName);	
	CstringToTChar( m_szHeat,ConfigRec.JobRec.Heat);		
	CstringToTChar( m_szWO,ConfigRec.JobRec.WO);			
	CstringToTChar( m_szLotNum,ConfigRec.JobRec.LotNum);		
	CstringToTChar( m_szRbNum,ConfigRec.JobRec.RbNum);		
	CstringToTChar( m_szOblq1Name,ConfigRec.JobRec.Oblq1Name);	
	CstringToTChar( m_szOblq2Name,ConfigRec.JobRec.Oblq2Name);	
	CstringToTChar( m_szOblq3Name,ConfigRec.JobRec.Oblq3Name);	

#endif

	ConfigRec.JobRec.WallStatFlag = m_nWallOption & 3;	// 4 options

	ConfigRec.JobRec.fChnlOffset[1] = m_fWall2Offset;
	ConfigRec.JobRec.fChnlOffset[2] = m_fWall3Offset;
	ConfigRec.JobRec.fChnlOffset[3] = m_fWall4Offset;

	// Update title bar on TscanDlg dialog  (parent dialog)
	//::PostMessage(pCTscanDlg->m_hWnd,
					//WM_USER_UPDATE_TITLEBAR,0,0);
	pCTscanDlg->UpdateTitleBar();

	pCTscanDlg->SetOblqName();
	if (pCTscanDlg->m_pInspectDlg1)
		pCTscanDlg->m_pInspectDlg1->SetOblqName();
	if (pCTscanDlg->m_pInspectDlg2)
		pCTscanDlg->m_pInspectDlg2->SetOblqName();
	
	if (pCTscanDlg != NULL) pCTscanDlg->SendMsg(STORE_JOBREC_MSG);

	CDialog::OnOK();	// must be called first
	CDialog::DestroyWindow();
	}

BOOL CWcJob::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	QualifyJobRecord();
	// Copy config file char strings to CStrings
	m_szBusiness = ConfigRec.JobRec.BusUnit;
	m_szComment = ConfigRec.JobRec.Comment;
	m_szCustomer = ConfigRec.JobRec.Customer;
	m_szDate = ConfigRec.JobRec.Date;
	m_szLocation = ConfigRec.JobRec.Location;
	m_szOperator = ConfigRec.JobRec.Operator;
	m_szWellFoot = ConfigRec.JobRec.WellFoot;
//	m_szWellName = ConfigRec.JobRec.WellName;
	m_szHeat = ConfigRec.JobRec.Heat;
	m_szWO = ConfigRec.JobRec.WO;
	m_szLotNum = ConfigRec.JobRec.LotNum;
	m_szRbNum = ConfigRec.JobRec.RbNum;
	m_nWallOption = ConfigRec.JobRec.WallStatFlag & 3;	// 4 options
	m_bWallDeviation = ConfigRec.JobRec.ShowWallDiff;
	m_szOblq1Name = ConfigRec.JobRec.Oblq1Name;
	m_szOblq2Name = ConfigRec.JobRec.Oblq2Name;
	m_szOblq3Name = ConfigRec.JobRec.Oblq3Name;
//	m_nWallOption = ConfigRec.JobRec.WallStatFlag = 0;

	m_fWall1Offset = ConfigRec.JobRec.fChnlOffset[0];
	m_fWall2Offset = ConfigRec.JobRec.fChnlOffset[1];
	m_fWall3Offset = ConfigRec.JobRec.fChnlOffset[2];
	m_fWall4Offset = ConfigRec.JobRec.fChnlOffset[3];
	
	// Move from class variables onto screen
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}


void CWcJob::OnSave() 
	{
	// TODO: Add your control notification handler code here
	// Update info in config record
	UpdateData(TRUE);	// Move from screen into class variables

#if 0
	strcpy(ConfigRec.JobRec.BusUnit, (LPCTSTR) m_szBusiness);
	strcpy(ConfigRec.JobRec.Comment, (LPCTSTR) m_szComment);
	strcpy(ConfigRec.JobRec.Customer, (LPCTSTR) m_szCustomer);
	strcpy(ConfigRec.JobRec.Date, (LPCTSTR) m_szDate);
	strcpy(ConfigRec.JobRec.Location, (LPCTSTR) m_szLocation);
	strcpy(ConfigRec.JobRec.Operator, (LPCTSTR) m_szOperator);
	strcpy(ConfigRec.JobRec.WellFoot, (LPCTSTR) m_szWellFoot);
//	strcpy(ConfigRec.JobRec.WellName, (LPCTSTR) m_szWellName);
	strcpy(ConfigRec.JobRec.WO, (LPCTSTR) m_szWO);
	strcpy(ConfigRec.JobRec.LotNum, (LPCTSTR) m_szLotNum);
	strcpy(ConfigRec.JobRec.RbNum, (LPCTSTR) m_szRbNum);
	strcpy(ConfigRec.JobRec.Oblq1Name, (LPCTSTR) m_szOblq1Name);
	strcpy(ConfigRec.JobRec.Oblq2Name, (LPCTSTR) m_szOblq2Name);
	strcpy(ConfigRec.JobRec.Oblq3Name, (LPCTSTR) m_szOblq3Name);
#endif

	CstringToTChar(m_szBusiness,ConfigRec.JobRec.BusUnit);
	CstringToTChar( m_szComment,ConfigRec.JobRec.Comment);	
	CstringToTChar( m_szCustomer,ConfigRec.JobRec.Customer);	
	CstringToTChar( m_szDate,ConfigRec.JobRec.Date);		
	CstringToTChar( m_szLocation,ConfigRec.JobRec.Location);	
	CstringToTChar( m_szOperator,ConfigRec.JobRec.Operator);	
	CstringToTChar( m_szWellFoot,ConfigRec.JobRec.WellFoot);	
//	CstringToTChar( m_szWellName,ConfigRec.JobRec.WellName);	
	CstringToTChar( m_szHeat,ConfigRec.JobRec.Heat);		
	CstringToTChar( m_szWO,ConfigRec.JobRec.WO);			
	CstringToTChar( m_szLotNum,ConfigRec.JobRec.LotNum);		
	CstringToTChar( m_szRbNum,ConfigRec.JobRec.RbNum);		
	CstringToTChar( m_szOblq1Name,ConfigRec.JobRec.Oblq1Name);	
	CstringToTChar( m_szOblq2Name,ConfigRec.JobRec.Oblq2Name);	
	CstringToTChar( m_szOblq3Name,ConfigRec.JobRec.Oblq3Name);



	ConfigRec.JobRec.WallStatFlag = m_nWallOption & 3;	// 4 options
	// Save config rec to hard disk
	pCTscanDlg->FileSaveAs();
//	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
//	ShowDlgTitle();
	}

void CWcJob::OnCancel() 
	{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CWcJob::DoWallOptionRadioButton(UINT nID)
	{
	// TODO: Add your control notification handler code here
	// 11/29/00 Give user options for wall processing including NONE

	UpdateData(TRUE);	// Move from screen into class variables
	
	}

void CWcJob::OnCkWallDeviation() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ConfigRec.JobRec.ShowWallDiff = m_bWallDeviation;
}
