#if !defined(AFX_TBLSENSORLOCATIONS_H__1187D852_A83C_46F1_BD7C_454E10D6B220__INCLUDED_)
#define AFX_TBLSENSORLOCATIONS_H__1187D852_A83C_46F1_BD7C_454E10D6B220__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tblSensorLocations.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CtblSensorLocations recordset

class CtblSensorLocations : public CRecordset
{
public:
	CtblSensorLocations(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblSensorLocations)

// Field/Param Data
	//{{AFX_FIELD(CtblSensorLocations, CRecordset)
	CString	m_SensorLocationID;
	CString	m_EquipmentID;
	BYTE	m_EquipmentType;
	BYTE	m_ShoesPerEquipment;
	BYTE	m_ShoeNumber;
	int		m_ShoeAngle;
	CString	m_ShoeXLocation;
	BYTE	m_SensorsPerShoe;
	BYTE	m_SensorNumber;
	long	m_SensorType;
	CString	m_SensorXLocation;
	int		m_SensorAngle;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CtblSensorLocations)
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

#endif // !defined(AFX_TBLSENSORLOCATIONS_H__1187D852_A83C_46F1_BD7C_454E10D6B220__INCLUDED_)
