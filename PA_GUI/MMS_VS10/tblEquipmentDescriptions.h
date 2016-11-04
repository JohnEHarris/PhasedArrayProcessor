#if !defined(AFX_TBLEQUIPMENTDESCRIPTIONS_H__4C4C49FE_8A43_424B_BEC5_80E22D266FB7__INCLUDED_)
#define AFX_TBLEQUIPMENTDESCRIPTIONS_H__4C4C49FE_8A43_424B_BEC5_80E22D266FB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tblEquipmentDescriptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CtblEquipmentDescriptions recordset

class CtblEquipmentDescriptions : public CRecordset
{
public:
	CtblEquipmentDescriptions(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblEquipmentDescriptions)

// Field/Param Data
	//{{AFX_FIELD(CtblEquipmentDescriptions, CRecordset)
	CString	m_EquipmentDescription;
	CString	m_EquipmentFamilyName;
	CString	m_EquipmentID;
	CString	m_EquipmentInspectionLine;
	CString	m_EquipmentLocation;
	CString	m_EquipmentNumber;
	long	m_EquipmentNumberInt;
	long	m_EquipmentPosition;
	CString	m_MotionPulseLength;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CtblEquipmentDescriptions)
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

#endif // !defined(AFX_TBLEQUIPMENTDESCRIPTIONS_H__4C4C49FE_8A43_424B_BEC5_80E22D266FB7__INCLUDED_)
