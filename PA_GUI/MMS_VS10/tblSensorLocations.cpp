// Unicoded tblSensorLocations.cpp

// tblSensorLocations.cpp : implementation file
//
 
#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblSensorLocations.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CtblSensorLocations
 
IMPLEMENT_DYNAMIC(CtblSensorLocations, CRecordset) 
 
CtblSensorLocations::CtblSensorLocations(CDatabase* pdb) 
	: CRecordset(pdb) 
{ 
	 //{{AFX_FIELD_INIT(CtblSensorLocations)
	m_SensorLocationID = _T(""); 
	m_EquipmentID = _T(""); 
	m_EquipmentType = 0; 
	m_ShoesPerEquipment = 0; 
	m_ShoeNumber = 0; 
	m_ShoeAngle = 0; 
	m_ShoeXLocation = _T(""); 
	m_SensorsPerShoe = 0; 
	m_SensorNumber = 0; 
	m_SensorType = 0; 
	m_SensorXLocation = _T(""); 
	m_SensorAngle = 0; 
	m_nFields = 12; 
	 //}}AFX_FIELD_INIT
	m_nDefaultType = dynaset; 
} 
 
 
CString CtblSensorLocations::GetDefaultConnect() 
{ 
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
} 
 
CString CtblSensorLocations::GetDefaultSQL() 
{ 
		return _T("[dbo].[tblSensorLocations]"); 
} 
 
void CtblSensorLocations::DoFieldExchange(CFieldExchange* pFX) 
{ 
	 //{{AFX_FIELD_MAP(CtblSensorLocations)
	pFX->SetFieldType(CFieldExchange::outputColumn); 
	{
		RFX_Text(pFX, _T("[SensorLocationID]"), m_SensorLocationID); 
		RFX_Text(pFX, _T("[EquipmentID]"), m_EquipmentID); 
		RFX_Byte(pFX, _T("[EquipmentType]"), m_EquipmentType); 
		RFX_Byte(pFX, _T("[ShoesPerEquipment]"), m_ShoesPerEquipment); 
		RFX_Byte(pFX, _T("[ShoeNumber]"), m_ShoeNumber); 
		RFX_Int(pFX, _T("[ShoeAngle]"), m_ShoeAngle); 
		RFX_Text(pFX, _T("[ShoeXLocation]"), m_ShoeXLocation); 
		RFX_Byte(pFX, _T("[SensorsPerShoe]"), m_SensorsPerShoe); 
		RFX_Byte(pFX, _T("[SensorNumber]"), m_SensorNumber); 
		RFX_Long(pFX, _T("[SensorType]"), m_SensorType); 
		RFX_Text(pFX, _T("[SensorXLocation]"), m_SensorXLocation); 
		RFX_Int(pFX, _T("[SensorAngle]"), m_SensorAngle); 
	}
	 //}}AFX_FIELD_MAP
} 
 
/////////////////////////////////////////////////////////////////////////////
// CtblSensorLocations diagnostics
 
#ifdef _DEBUG
void CtblSensorLocations::AssertValid() const 
{ 
	CRecordset::AssertValid(); 
} 
 
void CtblSensorLocations::Dump(CDumpContext& dc) const 
{ 
	CRecordset::Dump(dc); 
} 
#endif //_DEBUG
