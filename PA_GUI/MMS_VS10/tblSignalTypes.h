#if !defined(AFX_TBLSIGNALTYPES_H__57BE4997_909B_4A46_97DB_0DB97E794D0D__INCLUDED_)
#define AFX_TBLSIGNALTYPES_H__57BE4997_909B_4A46_97DB_0DB97E794D0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tblSignalTypes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CtblSignalTypes recordset

class CtblSignalTypes : public CRecordset
{
public:
	CtblSignalTypes(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblSignalTypes)

// Field/Param Data
	//{{AFX_FIELD(CtblSignalTypes, CRecordset)
	long	m_SignalType;
	CString	m_SignalTypeID;
	CString	m_SignalTypeName;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CtblSignalTypes)
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

#endif // !defined(AFX_TBLSIGNALTYPES_H__57BE4997_909B_4A46_97DB_0DB97E794D0D__INCLUDED_)
