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
#if 1
IMPLEMENT_DYNAMIC(CIP_Connect, CDialogEx)

CIP_Connect::CIP_Connect(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IP_CONNECT, pParent)
	{
// COPIED FROM NcNx dialog constructor
	//m_DlgLocationKey = _T("NC_NX_PA2");
	//m_DlgLocationSection = _T("Dialog Locations");	// Section is always this string for all dlgs in ini file
	//m_nPopulated = 0;
	int i = 0;
	}

CIP_Connect::~CIP_Connect()
	{
	gDlg.pIpConnect = 0;
	}

void CIP_Connect::DoDataExchange(CDataExchange* pDX)
	{
	CDialogEx::DoDataExchange(pDX);
	}


BEGIN_MESSAGE_MAP(CIP_Connect, CDialogEx)
END_MESSAGE_MAP()

#endif
// CIP_Connect message handlers







BOOL CIP_Connect::OnInitDialog()
	{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
	}


void CIP_Connect::OnOK()
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnOK();
	}


void CIP_Connect::OnCancel()
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnCancel();
	}

