// Unicoded tblCalibrationDetails.cpp

// tblCalibrationDetails.cpp : implementation file
//
 
#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblCalibrationDetails.h"
#include "Extern.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif
 
/////////////////////////////////////////////////////////////////////////////
// tblCalibrationDetails
 
IMPLEMENT_DYNAMIC(CtblCalibrationDetails, CRecordset) 
 
CtblCalibrationDetails::CtblCalibrationDetails(CDatabase* pdb) 
	: CRecordset(pdb) 
{ 
	 //{{AFX_FIELD_INIT(tblCalibrationDetails)
	m_CalibrationDetailID = _T(""); 
	m_CalibrationName = _T(""); 
	m_EquipmentID = _T(""); 
	m_WorkOrderID = _T(""); 
	m_nFields = 6; 
	 //}}AFX_FIELD_INIT
	m_nDefaultType = dynaset; 
} 
 
 
CString CtblCalibrationDetails::GetDefaultConnect() 
{ 
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
} 
 
CString CtblCalibrationDetails::GetDefaultSQL() 
{ 
		return _T("[dbo].[tblCalibrationDetails]"); 
} 
 
void CtblCalibrationDetails::DoFieldExchange(CFieldExchange* pFX) 
{ 
	 //{{AFX_FIELD_MAP(CtblCalibrationDetails)
	pFX->SetFieldType(CFieldExchange::outputColumn); 
	{
		RFX_Binary(pFX, _T("[CalibrationData]"), m_CalibrationData, MAX_BLOB_SIZE );  //sizeof(CALIBRATION_DETAIL) );
		RFX_Text(pFX, _T("[CalibrationDetailID]"), m_CalibrationDetailID); 
		RFX_Text(pFX, _T("[CalibrationName]"), m_CalibrationName); 
		RFX_Date(pFX, _T("[CalibrationTimeStamp]"), m_CalibrationTimeStamp); 
		RFX_Text(pFX, _T("[EquipmentID]"), m_EquipmentID); 
		RFX_Text(pFX, _T("[WorkOrderID]"), m_WorkOrderID); 
	}
	 //}}AFX_FIELD_MAP
} 
 
/////////////////////////////////////////////////////////////////////////////
// tblCalibrationDetails diagnostics
 
#ifdef _DEBUG
void CtblCalibrationDetails::AssertValid() const 
{ 
	CRecordset::AssertValid(); 
} 
 
void CtblCalibrationDetails::Dump(CDumpContext& dc) const 
{ 
	CRecordset::Dump(dc); 
} 
#endif //_DEBUG
