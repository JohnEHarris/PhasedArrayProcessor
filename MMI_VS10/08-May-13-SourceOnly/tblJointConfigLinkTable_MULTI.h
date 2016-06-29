// tblJointConfigLinkTable_MULTI.h : Declaration of the CtblJointConfigLinkTable_MULTI

#pragma once

// code generated on Thursday, May 18, 2006, 10:56 AM

class CtblJointConfigLinkTable_MULTI : public CRecordset
{
public:
	CtblJointConfigLinkTable_MULTI(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblJointConfigLinkTable_MULTI)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_JointConfigLinkTable_JointDetailID;
	CString	m_JointConfigLinkTable_CalibrationDetailID;
	long	m_JointConfigLinkTable_EquipmentNumberInt;
	long	m_tblJointDetailsTable_JointSequenceNumber;
	CString	m_tblJointDetails_JointSuffix;
	CString m_tblJointDetails_WorkOrderID;
	CString m_tblJointDetails_TimeStamp;

// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultConnect();	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};


