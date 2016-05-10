// tblJointConfigLinkTable.h : Implementation of the CtblJointConfigLinkTable class

// CtblJointConfigLinkTable implementation

// code generated on Monday, March 06, 2006, 9:33 AM

#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblJointConfigLinkTable.h"


IMPLEMENT_DYNAMIC(CtblJointConfigLinkTable, CRecordset)

CtblJointConfigLinkTable::CtblJointConfigLinkTable(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_JointDetailID = _T("");
	m_CalibrationDetailID = _T("");
	m_EquipmentNumberInt = 0;
	m_nFields = 3;
	m_nDefaultType = dynaset;
}
// #error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CtblJointConfigLinkTable::GetDefaultConnect()
{
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
}

CString CtblJointConfigLinkTable::GetDefaultSQL()
{
	return _T("[dbo].[tblJointConfigLinkTable]");
}

void CtblJointConfigLinkTable::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[JointDetailID]"), m_JointDetailID);
	RFX_Text(pFX, _T("[CalibrationDetailID]"), m_CalibrationDetailID);
	RFX_Long(pFX, _T("[EquipmentNumberInt]"), m_EquipmentNumberInt);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type

}
/////////////////////////////////////////////////////////////////////////////
// CtblJointConfigLinkTable diagnostics

#ifdef _DEBUG
void CtblJointConfigLinkTable::AssertValid() const
{
	CRecordset::AssertValid();
}

void CtblJointConfigLinkTable::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


