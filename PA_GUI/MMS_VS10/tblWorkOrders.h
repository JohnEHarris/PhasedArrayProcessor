#if !defined(AFX_TBLWORKORDERS_H__7DDA51F9_69CE_4486_9282_D60751E7DD74__INCLUDED_)
#define AFX_TBLWORKORDERS_H__7DDA51F9_69CE_4486_9282_D60751E7DD74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tblWorkOrders.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CtblWorkOrders recordset

class CtblWorkOrders : public CRecordset
{
public:
	CtblWorkOrders(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblWorkOrders)

// Field/Param Data
	//{{AFX_FIELD(CtblWorkOrders, CRecordset)
	CString	m_WorkOrder;
	CTime	m_WorkOrderDate;
	CString	m_WorkOrderHeat;
	CString	m_WorkOrderID;
	CString	m_WorkOrderLot;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CtblWorkOrders)
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

#endif // !defined(AFX_TBLWORKORDERS_H__7DDA51F9_69CE_4486_9282_D60751E7DD74__INCLUDED_)
