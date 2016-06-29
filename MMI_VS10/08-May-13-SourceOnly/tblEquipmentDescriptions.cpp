// Unicoded tblEquipmentDescriptions.cpp

// tblEquipmentDescriptions.cpp : implementation file
//
 
#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblEquipmentDescriptions.h"
#include "extern.h"
 
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif
 
 
/////////////////////////////////////////////////////////////////////////////
// CtblEquipmentDescriptions
 
IMPLEMENT_DYNAMIC(CtblEquipmentDescriptions, CRecordset) 
 
CtblEquipmentDescriptions::CtblEquipmentDescriptions(CDatabase* pdb) 
	: CRecordset(pdb) 
{ 
	 //{{AFX_FIELD_INIT(tbldescrip)
	m_EquipmentID = _T(""); 
	m_EquipmentNumber = _T(""); 
	m_EquipmentFamilyName = _T(""); 
	m_EquipmentDescription = _T(""); 
	m_EquipmentLocation = _T(""); 
	m_EquipmentInspectionLine = _T(""); 
	m_EquipmentPosition = 0; 
	m_EquipmentNumberInt = 0; 
	m_MotionPulseLength = _T(""); 
	m_nFields = 9; 
	 //}}AFX_FIELD_INIT
	m_nDefaultType = dynaset; 
} 
 
 
CString CtblEquipmentDescriptions::GetDefaultConnect() 
{ 
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
} 
 
CString CtblEquipmentDescriptions::GetDefaultSQL() 
{ 
		return _T("[dbo].[tblEquipmentDescriptions]"); 
} 
 
void CtblEquipmentDescriptions::DoFieldExchange(CFieldExchange* pFX) 
{ 
	 //{{AFX_FIELD_MAP(tbldescrip)
	pFX->SetFieldType(CFieldExchange::outputColumn); 
	{
		RFX_Text(pFX, _T("[EquipmentID]"), m_EquipmentID); 
		RFX_Text(pFX, _T("EquipmentNumber"), m_EquipmentNumber); 
		RFX_Text(pFX, _T("EquipmentFamilyName"), m_EquipmentFamilyName); 
		RFX_Text(pFX, _T("[EquipmentDescription]"), m_EquipmentDescription); 
		RFX_Text(pFX, _T("[EquipmentLocation]"), m_EquipmentLocation); 
		RFX_Text(pFX, _T("[EquipmentInspectionLine]"), m_EquipmentInspectionLine); 
		RFX_Long(pFX, _T("[EquipmentPosition]"), m_EquipmentPosition); 
		RFX_Long(pFX, _T("[EquipmentNumberInt]"), m_EquipmentNumberInt); 
		RFX_Text(pFX, _T("[MotionPulseLength]"), m_MotionPulseLength); 
	}
	 //}}AFX_FIELD_MAP
} 
 
/////////////////////////////////////////////////////////////////////////////
// tbldescrip diagnostics
 
#ifdef _DEBUG
void CtblEquipmentDescriptions::AssertValid() const 
{ 
	CRecordset::AssertValid(); 
} 
 
void CtblEquipmentDescriptions::Dump(CDumpContext& dc) const 
{ 
	CRecordset::Dump(dc); 
} 
#endif //_DEBUG
