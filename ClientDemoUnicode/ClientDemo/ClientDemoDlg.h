// CommDemoDlg.h : header file
//

#pragma once
#include "ClientSocket.h"
#include "afxcmn.h"
#include "afxwin.h"

// CCommDemoDlg dialog
class CCommDemoDlg : public CDialog
{
// Construction
public:
	CCommDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CLIENTDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	CListCtrl m_listReceive;
	int m_listReceiveDeleteCount;
	void AddReceive(CString s);
	afx_msg void OnBnClickedButtonConnect();
	CString m_sIPAddr;
	unsigned int m_iSeq;
	CString m_sUniString;
	CClientSocket * m_pSocket;
	int m_iMode;
	void SetConnectButton(int state);
	CButton m_cbConnect;
	afx_msg void OnBnClickedButtonSend1();
	afx_msg void OnBnClickedButtonSend();	// send message 2
	afx_msg void OnBnClickedButtonSend3();
	CString m_sMsg2WO;
	CString m_sMsg2Lot;
	CString m_sMsg3Lot;
	int m_iPipeNum;
	int m_iClass;
	float m_fLongID;
	float m_fLongOD;
	float m_fJudge;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	bool m_bWatchdogReceived;
	unsigned int m_iCounter;
	BOOL m_bSendWatchdog;
	afx_msg void OnBnClickedRadioUnit1();
	afx_msg void OnBnClickedRadioUnit2();
	unsigned int m_iUnitID;
	BOOL m_bUnit1;
	afx_msg void OnBnClickedCheckWatchdog();
	UINT m_iInspSpeed;
	float m_fLineSpeed;
	int m_iRpm;
	float m_fAmaMag;
	float m_fSonoMag1;
	float m_fSonoMag2;
	float m_fSonoMag3;
	float m_fSonoMag4;
	float m_fQuesTholdID;
	float m_fQuesTholdOD;
	float m_fRejTholdID;
	float m_fRejTholdOD;
};
