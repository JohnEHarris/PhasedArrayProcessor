// Unicoded tblJointDetails.cpp

// tblJointDetails.cpp : implementation file
//
 
#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblJointDetails.h"
#include "Extern.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CtblJointDetails
 
IMPLEMENT_DYNAMIC(CtblJointDetails, CRecordset) 
 
CtblJointDetails::CtblJointDetails(CDatabase* pdb) 
	: CRecordset(pdb) 
{ 
	 //{{AFX_FIELD_INIT(CtblJointDetails)
	m_JointTimeStamp = 0;
	m_CustomerJointIdentifier = _T("");
	m_CustomerJointSuffix = _T("");
	m_FinalDisposition = 0;
	m_JointDetailID = _T(""); 
	m_JointLength = _T(""); 
	m_JointSequenceNumber = 0; 
	m_JointSuffix = _T(""); 
	m_JointType = _T(""); 
	m_MotionPulseLength = _T(""); 
	m_ProveUpClassification = 0; 
	m_WallDropoutCount = 0; 
	m_WallDropoutDuration = 0.0; 
	m_WorkOrderID = _T(""); 
	m_nFields = 14; 
	 //}}AFX_FIELD_INIT
	m_nDefaultType = dynaset; 
} 
 
 
CString CtblJointDetails::GetDefaultConnect() 
{ 
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
} 
 
CString CtblJointDetails::GetDefaultSQL() 
{ 
		return _T("[dbo].[tblJointDetails]"); 
} 
 
void CtblJointDetails::DoFieldExchange(CFieldExchange* pFX) 
{ 
	 //{{AFX_FIELD_MAP(CtblJointDetails)
	pFX->SetFieldType(CFieldExchange::outputColumn); 
	{
		RFX_Text(pFX, _T("[CustomerJointIdentifier]"), m_CustomerJointIdentifier); 
		RFX_Long(pFX, _T("[FinalDisposition]"), m_FinalDisposition); 
		RFX_Text(pFX, _T("[JointDetailID]"), m_JointDetailID); 
		RFX_Text(pFX, _T("[CustomerJointSuffix]"), m_CustomerJointSuffix); 
		RFX_Text(pFX, _T("[JointLength]"), m_JointLength); 
		RFX_Long(pFX, _T("[JointSequenceNumber]"), m_JointSequenceNumber); 
		RFX_Text(pFX, _T("[JointSuffix]"), m_JointSuffix); 
		RFX_Date(pFX, _T("[JointTimeStamp]"), m_JointTimeStamp); 
		RFX_Text(pFX, _T("[JointType]"), m_JointType); 
		RFX_Text(pFX, _T("[MotionPulseLength]"), m_MotionPulseLength); 
		RFX_Long(pFX, _T("[ProveUpClassification]"), m_ProveUpClassification); 
		RFX_Long(pFX, _T("[WallDropoutCount]"), m_WallDropoutCount); 
		RFX_Single(pFX, _T("[WallDropoutDuration]"), m_WallDropoutDuration); 
		RFX_Text(pFX, _T("[WorkOrderID]"), m_WorkOrderID); 
	}
	 //}}AFX_FIELD_MAP
} 
 
/////////////////////////////////////////////////////////////////////////////
// CtblJointDetails diagnostics
 
#ifdef _DEBUG
void CtblJointDetails::AssertValid() const 
{ 
	CRecordset::AssertValid(); 
} 
 
void CtblJointDetails::Dump(CDumpContext& dc) const 
{ 
	CRecordset::Dump(dc); 
} 
#endif //_DEBUG
