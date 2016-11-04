// Unicoded tblCalibrationTemplates.cpp

// tblCalibrationTemplates.cpp : implementation file
//
 
#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblCalibrationTemplates.h"
#include "Extern.h"
 
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CtblCalibrationTemplates
 
IMPLEMENT_DYNAMIC(CtblCalibrationTemplates, CRecordset) 
 
CtblCalibrationTemplates::CtblCalibrationTemplates(CDatabase* pdb) 
	: CRecordset(pdb) 
{ 
	 //{{AFX_FIELD_INIT(CtblCalibrationTemplates)
	m_CalibrationTemplateID = _T(""); 
	m_EquipmentID = _T(""); 
	m_CalWorkOrder = _T(""); 
	m_PipeDiameter = _T(""); 
	m_PipeNominalWall = _T(""); 
	m_CalTemplateName = _T(""); 
	m_nFields = 8; 
	 //}}AFX_FIELD_INIT
	m_nDefaultType = dynaset; 
} 
 
 
CString CtblCalibrationTemplates::GetDefaultConnect() 
{ 
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
} 
 
CString CtblCalibrationTemplates::GetDefaultSQL() 
{ 
		return _T("[dbo].[tblCalibrationTemplates]"); 
} 
 
void CtblCalibrationTemplates::DoFieldExchange(CFieldExchange* pFX) 
{ 
	 //{{AFX_FIELD_MAP(CtblCalibrationTemplates)
	pFX->SetFieldType(CFieldExchange::outputColumn); 
	{
		RFX_Text(pFX, _T("[CalibrationTemplateID]"), m_CalibrationTemplateID); 
		RFX_Text(pFX, _T("[EquipmentID]"), m_EquipmentID); 
		RFX_Text(pFX, _T("[CalWorkOrder]"), m_CalWorkOrder); 
		RFX_Text(pFX, _T("[PipeDiameter]"), m_PipeDiameter); 
		RFX_Text(pFX, _T("[PipeNominalWall]"), m_PipeNominalWall); 
		RFX_Date(pFX, _T("[CalTemplateTimeStamp]"), m_CalTemplateTimeStamp); 
		RFX_Binary(pFX, _T("[CalTemplateData]"), m_CalTemplateData, MAX_BLOB_SIZE); 
		RFX_Text(pFX, _T("[CalTemplateName]"), m_CalTemplateName); 
	}
	 //}}AFX_FIELD_MAP
} 
 
/////////////////////////////////////////////////////////////////////////////
// CtblCalibrationTemplates diagnostics
 
#ifdef _DEBUG
void CtblCalibrationTemplates::AssertValid() const 
{ 
	CRecordset::AssertValid(); 
} 
 
void CtblCalibrationTemplates::Dump(CDumpContext& dc) const 
{ 
	CRecordset::Dump(dc); 
} 
#endif //_DEBUG
