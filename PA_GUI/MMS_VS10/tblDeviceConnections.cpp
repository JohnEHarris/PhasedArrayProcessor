// Unicoded tblDeviceConnections.cpp

// tblDeviceConnections.cpp : implementation file
//
 
#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblDeviceConnections.h"
#include "EXTERN.H"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CtblDeviceConnections
 
IMPLEMENT_DYNAMIC(CtblDeviceConnections, CRecordset) 
 
CtblDeviceConnections::CtblDeviceConnections(CDatabase* pdb) 
	: CRecordset(pdb) 
{ 
	 //{{AFX_FIELD_INIT(CtblDeviceConnections)
	m_DeviceConnectionID = _T(""); 
	m_DeviceSource = _T(""); 
	m_DeviceDestination = _T(""); 
	m_DeviceType = _T(""); 
	m_nFields = 4; 
	 //}}AFX_FIELD_INIT
	m_nDefaultType = dynaset; 
} 
 
 
CString CtblDeviceConnections::GetDefaultConnect() 
{ 
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
} 
 
CString CtblDeviceConnections::GetDefaultSQL() 
{ 
		return _T("[dbo].[tblDeviceConnections]"); 
} 
 
void CtblDeviceConnections::DoFieldExchange(CFieldExchange* pFX) 
{ 
	 //{{AFX_FIELD_MAP(CtblDeviceConnections)
	pFX->SetFieldType(CFieldExchange::outputColumn); 
	{
		RFX_Text(pFX, _T("[DeviceConnectionID]"), m_DeviceConnectionID); 
		RFX_Text(pFX, _T("[DeviceSource]"), m_DeviceSource); 
		RFX_Text(pFX, _T("[DeviceDestination]"), m_DeviceDestination); 
		RFX_Text(pFX, _T("[DeviceType]"), m_DeviceType); 
	}
	 //}}AFX_FIELD_MAP
} 
 
/////////////////////////////////////////////////////////////////////////////
// CtblDeviceConnections diagnostics
 
#ifdef _DEBUG
void CtblDeviceConnections::AssertValid() const 
{ 
	CRecordset::AssertValid(); 
} 
 
void CtblDeviceConnections::Dump(CDumpContext& dc) const 
{ 
	CRecordset::Dump(dc); 
} 
#endif //_DEBUG
