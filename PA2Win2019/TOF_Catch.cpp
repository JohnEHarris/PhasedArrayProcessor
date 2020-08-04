// TOF_Catch.cpp : implementation file
//

//#include "pch.h"

#include "stdafx.h"
//#include "pch.h"
#include "PA2Win.h"
#include "PA2WinDlg.h"
#include "afxdialogex.h"
#include <stdio.h>
#include <string.h>
#include "..\Include\Global.h"

#include "TOF_Catch.h"

// TOF_Catch dialog

IMPLEMENT_DYNAMIC(TOF_Catch, CDialogEx)

TOF_Catch::TOF_Catch(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_TOF_RECORD, pParent)
	{

	}

TOF_Catch::~TOF_Catch()
	{
	}

void TOF_Catch::DoDataExchange(CDataExchange* pDX)
	{
	CDialogEx::DoDataExchange(pDX);
	}


BEGIN_MESSAGE_MAP(TOF_Catch, CDialogEx)
	ON_COMMAND(ID_DEBUG_TOFSHOW, &TOF_Catch::OnDebugTofshow)
	ON_BN_CLICKED(IDC_TOF_LOG, &TOF_Catch::OnBnClickedTofLog)
END_MESSAGE_MAP()


// TOF_Catch message handlers


void TOF_Catch::OnCancel( )
	{
	// TODO: Add your specialized code here and/or call the base class
	CDialogEx::OnCancel( );
	if (gDlg.pTofCatch)
		{
		delete gDlg.pTofCatch;
		gDlg.pTofCatch = 0;
		}
	}


BOOL TOF_Catch::OnInitDialog( )
	{
	CDialogEx::OnInitDialog( );
	CButton *pButton;
	pButton = (CButton *) GetDlgItem(IDC_TOF_LOG);
	pButton->SetCheck(gbTofRecord);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
	}


void TOF_Catch::OnOK( )
	{
	// TODO: Add your specialized code here and/or call the base class
	CDialogEx::OnOK( );
	if (gDlg.pTofCatch)
		{
		delete gDlg.pTofCatch;
		gDlg.pTofCatch = 0;
		}
	}


void TOF_Catch::OnDebugTofshow( )
	{
	// TODO: Add your command handler code here
	}


void TOF_Catch::OnBnClickedTofLog( )
	{
	// User clicked a button. Which button? If the check box, change the state of IDC_TOF_LOG
	CButton *pButton;
	pButton = (CButton *) GetDlgItem(IDC_TOF_LOG);
	gbTofRecord = pButton->GetCheck();

	}
