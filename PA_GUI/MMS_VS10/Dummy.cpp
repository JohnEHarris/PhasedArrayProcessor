// Dummy.cpp : implementation file
//

#include "stdafx.h"
#include "TruScan.h"
#include "Dummy.h"
#include "afxdialogex.h"


// CDummy dialog

IMPLEMENT_DYNAMIC(CDummy, CDialog)

CDummy::CDummy(CWnd* pParent /*=NULL*/)
	: CDialog(CDummy::IDD, pParent)
{

}

CDummy::~CDummy()
{
}

void CDummy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDummy, CDialog)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CDummy message handlers


LRESULT CDummy::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CDialog::OnNcHitTest(point);
}
