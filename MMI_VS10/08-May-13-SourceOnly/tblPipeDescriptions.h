#if !defined(AFX_TBLPIPEDESCRIPTIONS_H__EFDC8F86_4F8D_4B88_AF92_0CCF41FD1166__INCLUDED_)
#define AFX_TBLPIPEDESCRIPTIONS_H__EFDC8F86_4F8D_4B88_AF92_0CCF41FD1166__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tblPipeDescriptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CtblPipeDescriptions recordset

class CtblPipeDescriptions : public CRecordset
{
public:
	CtblPipeDescriptions(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblPipeDescriptions)

// Field/Param Data
	//{{AFX_FIELD(CtblPipeDescriptions, CRecordset)
	CString	m_PipeDescriptionID;
	CString	m_PipeDiameter;
	CString	m_PipeGrade;
	CString	m_PipeNominalWall;
	long	m_PipeRange;
	CString	m_PipeWeightPerUnit;
	CString	m_WorkOrderID;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CtblPipeDescriptions)
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

#endif // !defined(AFX_TBLPIPEDESCRIPTIONS_H__EFDC8F86_4F8D_4B88_AF92_0CCF41FD1166__INCLUDED_)
