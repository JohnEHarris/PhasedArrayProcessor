// Unicoded tblSignalTypes.cpp

// tblSignalTypes.cpp : implementation file
//
 
#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblSignalTypes.h"
#include "extern.h"

 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CtblSignalTypes
 
IMPLEMENT_DYNAMIC(CtblSignalTypes, CRecordset) 
 
CtblSignalTypes::CtblSignalTypes(CDatabase* pdb) 
	: CRecordset(pdb) 
{ 
	 //{{AFX_FIELD_INIT(CtblSignalTypes)
	m_SignalType = 0; 
	m_SignalTypeID = _T(""); 
	m_SignalTypeName = _T(""); 
	m_nFields = 3; 
	 //}}AFX_FIELD_INIT
	m_nDefaultType = dynaset; 
} 
 
 
CString CtblSignalTypes::GetDefaultConnect() 
{ 
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
}

CString CtblSignalTypes::GetDefaultSQL() 
{ 
		return _T("[dbo].[tblSignalTypes]"); 
} 
 
void CtblSignalTypes::DoFieldExchange(CFieldExchange* pFX) 
{ 
	 //{{AFX_FIELD_MAP(CtblSignalTypes)
	pFX->SetFieldType(CFieldExchange::outputColumn); 
	{
		CString _stmp_[10];
		RFX_Long(pFX, _T("[SignalType]"), m_SignalType); 
		RFX_Text(pFX, _T("[SignalTypeID]"), m_SignalTypeID); 
		RFX_Text(pFX, _T("[SignalTypeName]"), m_SignalTypeName); 
	}
	 //}}AFX_FIELD_MAP
} 
 
/////////////////////////////////////////////////////////////////////////////
// CtblSignalTypes diagnostics
 
#ifdef _DEBUG
void CtblSignalTypes::AssertValid() const 
{ 
	CRecordset::AssertValid(); 
} 
 
void CtblSignalTypes::Dump(CDumpContext& dc) const 
{ 
	CRecordset::Dump(dc); 
} 
#endif //_DEBUG
