#if !defined(AFX_TBLDEVICECONNECTIONS_H__BF2D08F1_2036_4D18_9DD4_33733235D63A__INCLUDED_)
#define AFX_TBLDEVICECONNECTIONS_H__BF2D08F1_2036_4D18_9DD4_33733235D63A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tblDeviceConnections.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CtblDeviceConnections recordset

class CtblDeviceConnections : public CRecordset
{
public:
	CtblDeviceConnections(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblDeviceConnections)

// Field/Param Data
	//{{AFX_FIELD(CtblDeviceConnections, CRecordset)
	CString	m_DeviceConnectionID;
	CString	m_DeviceSource;
	CString	m_DeviceDestination;
	CString	m_DeviceType;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CtblDeviceConnections)
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

#endif // !defined(AFX_TBLDEVICECONNECTIONS_H__BF2D08F1_2036_4D18_9DD4_33733235D63A__INCLUDED_)
