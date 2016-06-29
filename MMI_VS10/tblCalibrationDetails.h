#if !defined(AFX_TBLCALIBRATIONDETAILS_H__404DC508_8638_40FB_8D7C_26F5220D2E6E__INCLUDED_)
#define AFX_TBLCALIBRATIONDETAILS_H__404DC508_8638_40FB_8D7C_26F5220D2E6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tblCalibrationDetails.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// tblCalibrationDetails recordset

class CtblCalibrationDetails : public CRecordset
{
public:
	CtblCalibrationDetails(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblCalibrationDetails)

// Field/Param Data
	//{{AFX_FIELD(tblCalibrationDetails, CRecordset)
	CByteArray	m_CalibrationData;
	CString	m_CalibrationDetailID;
	CString	m_CalibrationName;
	CTime	m_CalibrationTimeStamp;
	CString	m_EquipmentID;
	CString	m_WorkOrderID;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(tblCalibrationDetails)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TBLCALIBRATIONDETAILS_H__404DC508_8638_40FB_8D7C_26F5220D2E6E__INCLUDED_)
