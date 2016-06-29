// tblJointConfigLinkTable_MULTI.h : Implementation of the CtblJointConfigLinkTable_MULTI class

// CtblJointConfigLinkTable_MULTI implementation

// code generated on Thursday, May 18, 2006, 10:56 AM

#include "stdafx.h"
#include "TruScan.h"
#include "tscandlg.h"
#include "tblJointConfigLinkTable_MULTI.h"
IMPLEMENT_DYNAMIC(CtblJointConfigLinkTable_MULTI, CRecordset)

CtblJointConfigLinkTable_MULTI::CtblJointConfigLinkTable_MULTI(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_JointConfigLinkTable_JointDetailID = _T("");
	m_JointConfigLinkTable_CalibrationDetailID = _T("");
	m_JointConfigLinkTable_EquipmentNumberInt = 0;

	m_tblJointDetailsTable_JointSequenceNumber = 0;
	m_tblJointDetails_JointSuffix = _T("");
	m_tblJointDetails_WorkOrderID = _T("");
	m_tblJointDetails_TimeStamp = _T("");
	m_nFields = 7;
	m_nDefaultType = dynaset;
}
// #error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CtblJointConfigLinkTable_MULTI::GetDefaultConnect()
{
	    CString stmp;
		stmp.LoadString(IDA_ODBCDSNMILLCONSOLEINSPECT_64000);
		return stmp; //_T("ODBC;DSN=MillConsoleInspectionDatabase");
}

CString CtblJointConfigLinkTable_MULTI::GetDefaultSQL()
{
	return _T("[dbo].[tblJointConfigLinkTable],[dbo].[tblJointDetails]");
}

void CtblJointConfigLinkTable_MULTI::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[dbo].[tblJointConfigLinkTable].[JointDetailID]"), m_JointConfigLinkTable_JointDetailID);
	RFX_Text(pFX, _T("[dbo].[tblJointConfigLinkTable].[CalibrationDetailID]"), m_JointConfigLinkTable_CalibrationDetailID);
	RFX_Long(pFX, _T("[dbo].[tblJointConfigLinkTable].[EquipmentNumberInt]"), m_JointConfigLinkTable_EquipmentNumberInt);

//	RFX_Text(pFX, _T("[tblJointConfigLinkTable].[JointDetailID]"), m_JointConfigLinkTable_JointDetailID);
//	RFX_Text(pFX, _T("[tblJointConfigLinkTable].[CalibrationDetailID]"), m_JointConfigLinkTable_CalibrationDetailID);
//	RFX_Long(pFX, _T("[tblJointConfigLinkTable].[EquipmentNumberInt]"), m_JointConfigLinkTable_EquipmentNumberInt);

	RFX_Long(pFX, _T("[dbo].[tblJointDetails].[JointSequenceNumber]"), m_tblJointDetailsTable_JointSequenceNumber);
	RFX_Text(pFX, _T("[dbo].[tblJointDetails].[JointSuffix]"), m_tblJointDetails_JointSuffix);
	RFX_Text(pFX, _T("[dbo].[tblJointDetails].[WorkOrderID]"), m_tblJointDetails_WorkOrderID);
	RFX_Text(pFX, _T("[dbo].[tblJointDetails].[JointTimeStamp]"), m_tblJointDetails_TimeStamp);
	// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type

}
/////////////////////////////////////////////////////////////////////////////
// CtblJointConfigLinkTable diagnostics

#ifdef _DEBUG
void CtblJointConfigLinkTable_MULTI::AssertValid() const
{
	CRecordset::AssertValid();
}

void CtblJointConfigLinkTable_MULTI::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



