// MC_SysCPTestClientDlg.h : header file
//
#if 0


#if !defined(AFX_MC_SysCPTESTCLIENTDLG_H__62E0A55A_629F_4667_AA6C_171C326DDD3B__INCLUDED_)
#define AFX_MC_SysCPTESTCLIENTDLG_H__62E0A55A_629F_4667_AA6C_171C326DDD3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4100)


#include "..\Common\Constants\MC_SysCP_Constants.h"
#include "ConnectionSocket.h"
#include "FakeLogMessage.h"

#define MAIN_DLG_NAME	CMC_SysCPTestClientDlg
// moved from MC_SysCPTestClientDlg.cpp to here 12-Jul-12 jeh

#define DESCRIPTION _T("Stand Alone")
#define	LOCATION	_T("Solvang, CA")
#define LINE		_T("Randy's Lab")

/////////////////////////////////////////////////////////////////////////////
// CMC_SysCPTestClientDlg dialog

class CMC_SysCPTestClientDlg : public CDialog
{
// Construction
public:
	CString m_sCurrentPipeNumberID;
	void AddDebugMessage(CString sMsg);
	int m_DebugMessageCount;
	USHORT m_ClientThreadID;
	// these structures exist on the stack for this instance of the class
	ALARM_DETAIL m_AlarmDetail;
	CLIENT_IDENTITY_DETAIL m_ClientIdentity;
	PIPE_NUMBER_DETAIL m_PipeNumberDetail;
	STATUS_DETAIL m_StatusDetail;

	CConnectionSocket * m_sockClient;
	CMC_SysCPTestClientDlg(CWnd* pParent = NULL);	// standard constructor


	// jeh
	// return 0 if fail to determine the IP address, non zero means success
	int FindClientSideIP(int nWhichConnection);	// nWhichConnection is count of order in which connections are attempted
	int FindServerSideIP(int nWhichConnection);
	LRESULT GetReceivedPackets(WPARAM wWhichList, LPARAM lParam);	


// Dialog Data
	//{{AFX_DATA(CMC_SysCPTestClientDlg)
	enum { IDD = IDD_MC_SYSCPTESTCLIENT_DIALOG };
	CListBox	m_list_CommandParameters;
	CButton	m_check_ClearOnDisconnect;
	CComboBox	m_SelectedFamilyName;
	CListCtrl	m_DebugMessages;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMC_SysCPTestClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
private:
	CString GetIPv4(CString sComputerName);
	CString GetIPv4();

// Implementation
protected:
	void MakeClearAlarmDetail();
	void MakeAllAlarmDetail();
	void MakeStatusDetail(USHORT status);
	void MakePipeNumberDetail(USHORT status);
	void LoadFamilyNameList();
	int RandomBetween(int min, int max);
	void MakeRandomAlarmDetail();

	void MakeEquipmentDetail(CString EquipmentNumber,
							 CString FamilyName,
							 CString EquipmentDescription,
							 CString EquipmentLocation,
							 CString EquipmentInspectionLine);

	void MakeVersionDetail();
	void MakeConnectionDetail(CString ServerIP, UINT ServerPort, CString ClientIP, UINT ClientPort);
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMC_SysCPTestClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonDisconnect();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSelchangeComboRole();
	afx_msg void OnButtonFakeRandomAlarms();
	afx_msg void OnButtonClear();
	afx_msg void OnButtonFakeLogmessage();
	afx_msg void OnButtonInspectEnable();
	afx_msg void OnButtonInspectDisable();
	afx_msg void OnButtonFakeAllAlarms();
	afx_msg void OnButtonFakeClearAlarms();
	afx_msg void GetReceivedData();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	UINT m_iSysCPPort;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MC_SysCPTESTCLIENTDLG_H__62E0A55A_629F_4667_AA6C_171C326DDD3B__INCLUDED_)

#endif
