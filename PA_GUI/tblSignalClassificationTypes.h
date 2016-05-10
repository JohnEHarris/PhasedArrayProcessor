#if !defined(AFX_TBLSIGNALCLASSIFICATIONTYPES_H__A0C3A3AE_99AD_4936_9A43_44F1FC526BE4__INCLUDED_)
#define AFX_TBLSIGNALCLASSIFICATIONTYPES_H__A0C3A3AE_99AD_4936_9A43_44F1FC526BE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tblSignalClassificationTypes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CtblSignalClassificationTypes recordset

class CtblSignalClassificationTypes : public CRecordset
{
public:
	CtblSignalClassificationTypes(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblSignalClassificationTypes)

// Field/Param Data
	//{{AFX_FIELD(CtblSignalClassificationTypes, CRecordset)
	CString	m_SignalClassificationTypeID;
	long	m_SignalClassificationType;
	CString	m_SignalClassificationTypeName;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CtblSignalClassificationTypes)
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

#endif // !defined(AFX_TBLSIGNALCLASSIFICATIONTYPES_H__A0C3A3AE_99AD_4936_9A43_44F1FC526BE4__INCLUDED_)
