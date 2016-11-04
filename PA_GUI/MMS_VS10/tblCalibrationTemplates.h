#if !defined(AFX_TBLCALIBRATIONTEMPLATES_H__08A559F9_44CB_4A37_BBC1_ADAEE9026490__INCLUDED_)
#define AFX_TBLCALIBRATIONTEMPLATES_H__08A559F9_44CB_4A37_BBC1_ADAEE9026490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tblCalibrationTemplates.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CtblCalibrationTemplates recordset

class CtblCalibrationTemplates : public CRecordset
{
public:
	CtblCalibrationTemplates(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblCalibrationTemplates)

// Field/Param Data
	//{{AFX_FIELD(CtblCalibrationTemplates, CRecordset)
	CString	m_CalibrationTemplateID;
	CString	m_EquipmentID;
	CString	m_CalWorkOrder;
	CString	m_PipeDiameter;
	CString	m_PipeNominalWall;
	CTime	m_CalTemplateTimeStamp;
	CByteArray	m_CalTemplateData;
	CString	m_CalTemplateName;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CtblCalibrationTemplates)
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

#endif // !defined(AFX_TBLCALIBRATIONTEMPLATES_H__08A559F9_44CB_4A37_BBC1_ADAEE9026490__INCLUDED_)
