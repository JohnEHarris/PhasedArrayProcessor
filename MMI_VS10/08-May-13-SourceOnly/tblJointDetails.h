#if !defined(AFX_TBLJOINTDETAILS_H__DC0FA55E_16FF_4462_A2DD_A43E61ACCDCB__INCLUDED_)
#define AFX_TBLJOINTDETAILS_H__DC0FA55E_16FF_4462_A2DD_A43E61ACCDCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tblJointDetails.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CtblJointDetails recordset

class CtblJointDetails : public CRecordset
{
public:
	CtblJointDetails(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblJointDetails)

// Field/Param Data
	//{{AFX_FIELD(CtblJointDetails, CRecordset)
	CString	m_CustomerJointIdentifier;
	CString m_CustomerJointSuffix;
	long	m_FinalDisposition;
	CString	m_JointDetailID;
	CString	m_JointLength;
	long	m_JointSequenceNumber;
	CString	m_JointSuffix;
	CTime	m_JointTimeStamp;
	CString	m_JointType;
	CString	m_MotionPulseLength;
	long	m_ProveUpClassification;
	long	m_WallDropoutCount;
	float	m_WallDropoutDuration;
	CString	m_WorkOrderID;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CtblJointDetails)
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

#endif // !defined(AFX_TBLJOINTDETAILS_H__DC0FA55E_16FF_4462_A2DD_A43E61ACCDCB__INCLUDED_)
