// Unicoded tblWorkOrders.cpp

// tblWorkOrders.cpp : implementation file
//
 
#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblWorkOrders.h"
//#include "extern.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CtblWorkOrders
 
IMPLEMENT_DYNAMIC(CtblWorkOrders, CRecordset) 
 
CtblWorkOrders::CtblWorkOrders(CDatabase* pdb) 
	: CRecordset(pdb) 
{ 
	 //{{AFX_FIELD_INIT(CtblWorkOrders)
	m_WorkOrder = _T(""); 
	m_WorkOrderDate = 0; 
	m_WorkOrderHeat = _T(""); 
	m_WorkOrderID = _T("");
	m_WorkOrderLot = _T("");
	m_nFields = 5; 
	 //}}AFX_FIELD_INIT
	m_nDefaultType = dynaset; 
} 
 
 
CString CtblWorkOrders::GetDefaultConnect() 
{ 
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
} 
 
CString CtblWorkOrders::GetDefaultSQL() 
{ 
		return _T("[dbo].[tblWorkOrders]"); 
} 
 
void CtblWorkOrders::DoFieldExchange(CFieldExchange* pFX) 
{ 
	 //{{AFX_FIELD_MAP(CtblWorkOrders)
	pFX->SetFieldType(CFieldExchange::outputColumn); 
	{
		RFX_Text(pFX, _T("[WorkOrder]"), m_WorkOrder); 
		RFX_Date(pFX, _T("[TimeTag]"), m_WorkOrderDate); 
		RFX_Text(pFX, _T("[Heat]"), m_WorkOrderHeat); 
		RFX_Text(pFX, _T("[WorkOrderID]"), m_WorkOrderID); 
		RFX_Text(pFX, _T("[Lot]"), m_WorkOrderLot); 
	}
	 //}}AFX_FIELD_MAP
} 
 
/////////////////////////////////////////////////////////////////////////////
// CtblWorkOrders diagnostics
 
#ifdef _DEBUG
void CtblWorkOrders::AssertValid() const 
{ 
	CRecordset::AssertValid(); 
} 
 
void CtblWorkOrders::Dump(CDumpContext& dc) const 
{ 
	CRecordset::Dump(dc); 
} 
#endif //_DEBUG
