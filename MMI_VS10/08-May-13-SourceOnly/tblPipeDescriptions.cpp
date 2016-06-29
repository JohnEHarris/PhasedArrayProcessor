// Unicoded tblPipeDescriptions.cpp

// tblPipeDescriptions.cpp : implementation file
//
 
#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblPipeDescriptions.h"
#include "Extern.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CtblPipeDescriptions
 
IMPLEMENT_DYNAMIC(CtblPipeDescriptions, CRecordset) 
 
CtblPipeDescriptions::CtblPipeDescriptions(CDatabase* pdb) 
	: CRecordset(pdb) 
{ 
	 //{{AFX_FIELD_INIT(CtblPipeDescriptions)
	m_PipeDescriptionID = _T(""); 
	m_PipeDiameter = _T(""); 
	m_PipeGrade = _T(""); 
	m_PipeNominalWall = _T(""); 
	m_PipeRange = 0; 
	m_PipeWeightPerUnit = _T(""); 
	m_WorkOrderID = _T(""); 
	m_nFields = 7; 
	 //}}AFX_FIELD_INIT
	m_nDefaultType = dynaset; 
} 
 
 
CString CtblPipeDescriptions::GetDefaultConnect() 
{ 
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
} 
 
CString CtblPipeDescriptions::GetDefaultSQL() 
{ 
		return _T("[dbo].[tblPipeDescriptions]"); 
} 
 
void CtblPipeDescriptions::DoFieldExchange(CFieldExchange* pFX) 
{ 
	 //{{AFX_FIELD_MAP(CtblPipeDescriptions)
	pFX->SetFieldType(CFieldExchange::outputColumn); 
	{
		RFX_Text(pFX, _T("[PipeDescriptionID]"), m_PipeDescriptionID); 
		RFX_Text(pFX, _T("[PipeDiameter]"), m_PipeDiameter); 
		RFX_Text(pFX, _T("[PipeGrade]"), m_PipeGrade); 
		RFX_Text(pFX, _T( "[PipeNominalWall]"), m_PipeNominalWall); 
		RFX_Long(pFX, _T("[PipeRange]"), m_PipeRange); 
		RFX_Text(pFX, _T("[PipeWeightPerUnit]"), m_PipeWeightPerUnit); 
		RFX_Text(pFX, _T("[WorkOrderID]"), m_WorkOrderID); 
	}
	 //}}AFX_FIELD_MAP
} 
 
/////////////////////////////////////////////////////////////////////////////
// CtblPipeDescriptions diagnostics
 
#ifdef _DEBUG
void CtblPipeDescriptions::AssertValid() const 
{ 
	CRecordset::AssertValid(); 
} 
 
void CtblPipeDescriptions::Dump(CDumpContext& dc) const 
{ 
	CRecordset::Dump(dc); 
} 
#endif //_DEBUG
