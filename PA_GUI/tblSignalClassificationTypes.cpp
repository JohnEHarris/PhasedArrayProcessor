// Unicoded tblSignalClassificationTypes.cpp

// tblSignalClassificationTypes.cpp : implementation file
//
 
#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblSignalClassificationTypes.h"
#include "extern.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CtblSignalClassificationTypes
 
IMPLEMENT_DYNAMIC(CtblSignalClassificationTypes, CRecordset) 
 
CtblSignalClassificationTypes::CtblSignalClassificationTypes(CDatabase* pdb) 
	: CRecordset(pdb) 
{ 
	 //{{AFX_FIELD_INIT(CtblSignalClassificationTypes)
	m_SignalClassificationTypeID = _T(""); 
	m_SignalClassificationType = 0; 
	m_SignalClassificationTypeName = _T(""); 
	m_nFields = 3; 
	 //}}AFX_FIELD_INIT
	m_nDefaultType = dynaset; 
} 
 
 
CString CtblSignalClassificationTypes::GetDefaultConnect() 
{ 
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
} 
 
CString CtblSignalClassificationTypes::GetDefaultSQL() 
{ 
		return _T("[dbo].[tblSignalClassificationTypes]"); 
} 
 
void CtblSignalClassificationTypes::DoFieldExchange(CFieldExchange* pFX) 
{ 
	 //{{AFX_FIELD_MAP(CtblSignalClassificationTypes)
	pFX->SetFieldType(CFieldExchange::outputColumn); 
	{
		RFX_Text(pFX, _T("[SignalClassificationTypeID]"), m_SignalClassificationTypeID); 
		RFX_Long(pFX, _T("[SignalClassificationType]"), m_SignalClassificationType); 
		RFX_Text(pFX, _T("[SignalClassificationTypeName]"), m_SignalClassificationTypeName); 
	}
	 //}}AFX_FIELD_MAP
} 
 
/////////////////////////////////////////////////////////////////////////////
// CtblSignalClassificationTypes diagnostics
 
#ifdef _DEBUG
void CtblSignalClassificationTypes::AssertValid() const 
{ 
	CRecordset::AssertValid(); 
} 
 
void CtblSignalClassificationTypes::Dump(CDumpContext& dc) const 
{ 
	CRecordset::Dump(dc); 
} 
#endif //_DEBUG
