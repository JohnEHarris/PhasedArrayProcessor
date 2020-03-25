// CIP_Connect.cpp : implementation file
//
// author: John Harris
// date:   3/19/2020
// purpose: show IP connections to instruments and PAP. Also show Wall instrument number
//

#include "stdafx.h"
//#include "pch.h"
#include "PA2Win.h"
#include "PA2WinDlg.h"
#include "afxdialogex.h"
#include <stdio.h>
#include <string.h>
#include "..\Include\Global.h"
#include "CIP_Connect.h"


// CIP_Connect dialog

IMPLEMENT_DYNAMIC(CIP_Connect, CDialogEx)

CIP_Connect::CIP_Connect(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IP_CONNECT, pParent)
	{
// COPIED FROM NcNx dialog constructor
//	m_DlgLocationKey = _T("NC_NX_PA2");
//	m_DlgLocationSection = _T("DIALOG LOCATIONS");	// Section is always this string for all dlgs in ini file
	//m_nPopulated = 0;
	m_DlgLocationKey = _T("IP_CONNECT");
	m_DlgLocationSection = _T("DIALOG LOCATIONS");	// Section is always this string for all dlgs
	m_uStatTimer = 0;
	}

CIP_Connect::~CIP_Connect()
	{
#if 0
	// crash app if close with X in top right corner
	if (gDlg.pIpConnect)
		{
		delete gDlg.pIpConnect;
		gDlg.pIpConnect = 0;
		}
#endif
	}

void CIP_Connect::DoDataExchange(CDataExchange* pDX)
	{
	CDialogEx::DoDataExchange(pDX);
	}


BEGIN_MESSAGE_MAP(CIP_Connect, CDialogEx)
END_MESSAGE_MAP()


//
// Use the ini file to reposition window in same location it was when the window closed.
void CIP_Connect::PositionWindow()
	{
	if (gDlg.pTuboIni == NULL)	return;

	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	GetWindowPlacement(&wp);
	dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	// m_DlgLocationKey set in constructor -- NOT IMPLEMENTED FOR INI FILE 3/19/20
	gDlg.pTuboIni->GetWindowLastPosition(m_DlgLocationSection, m_DlgLocationKey, &rect);

	if (((rect.right - rect.left) >= dx) &&
		((rect.bottom - rect.top) >= dy))
		{
		wp.rcNormalPosition = rect;
		SetWindowPlacement(&wp);
		}
	}

void CIP_Connect::RemoteTimer()
	{
	CString s;
	char t[10];
	m_nTimerCount++;
	SetDlgItemText(IDC_ED_UUI2_IP2, gsUUI_PAP_NxIP);  // server for PAP Nx
	SetDlgItemText(IDC_ED_UUI2_IP3, gsUUI_PAP_AllWall_IP);
	
	_itoa(gbAssignedPAPNumber,t,10);
	s = t;
	s += _T(" - defined by: ");
	s += gsWallAssignPath;
	SetDlgItemText(IDC_ED_WALL_DEV_NUMBER, s);

	// file path
	s = gsIniFilePath;
	SetDlgItemText(IDC_ED_FILE_PATH, s);

	//PAP Server for Wall
	s = gsPAP2Wall_IP;
	SetDlgItemText(IDC_ED_PAPSRV_GATES, s);

	//PAP Server for pULSER
	s = gsPAP2Pulser_IP;
	SetDlgItemText(IDC_ED_PAPSRV_PLSR, s);

	// Nx client of UUI
	s = gsPAP_Nx2UUI_IP; 
	SetDlgItemText(IDC_PAP_NX_CLIENT, s);

	s = gsPAP_AW2UUI_IP;
	SetDlgItemText(IDC_ED_AW_CLIENT, s);
	
	// Gate board client of PAP
	s = gsWall_IP;
	SetDlgItemText(IDC_ED_GATEBRD_Nx, s);
	
	// PUlser board client OF PAP
	s = gsPulser_IP;
	SetDlgItemText(IDC_ED_PULSER, s);
	}

void CIP_Connect::UpdateTime(CString t)
	{
	SetDlgItemText(IDC_ED_TIME, t);
	}

void CIP_Connect::OnTimer(UINT_PTR nIDEvent)
	{
	CDialogEx::OnTimer( nIDEvent );
	m_nTimerCount++;
	}
// CIP_Connect message handlers


BOOL CIP_Connect::OnInitDialog()
	{
	CDialogEx::OnInitDialog();
	PositionWindow();
	// Start 1 second timer
	// StartTimer();
	// gbAssignedPAPNumber display ..if 8 put message unknown wall instrument number

	// TODO: Add extra initialization here -- get the ip connection info and wall instrument number

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
	}


void CIP_Connect::OnOK()
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnOK(); 
	//StopTimer();
	if (gDlg.pIpConnect)
		{
		delete gDlg.pIpConnect;
		gDlg.pIpConnect = 0;
		}
	}


void CIP_Connect::OnCancel()
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnCancel();  // same as clicking 'X' in top right corner of dialog
	if (gDlg.pIpConnect)
		{
		delete gDlg.pIpConnect;
		gDlg.pIpConnect = 0;
		}
	}


#if 0
// timer operations come from main dialog now 3/23/20
void CIP_Connect::StartTimer()
	{	// helper function to start timer with one call
	if (m_uStatTimer)	return;	// already running

	// 70 ticks per second
	m_uStatTimer = (UINT)SetTimer(IDT_TIMER2, 1000, NULL);
	if (!m_uStatTimer) MessageBox(_T("Failed to start timer"));
	m_nTimerCount = 0;
	}

void CIP_Connect::StopTimer()
	{	// helper function to stop timer with one call
	if (m_uStatTimer)
		{
		KillTimer(m_uStatTimer);
		m_uStatTimer = 0;
		}
	}
#endif