// Disk.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisk dialog
typedef struct {
	unsigned long	free;	// free disk space in megabytes
	unsigned long	used;
	unsigned long	total;
	} SPACE;

SPACE	Drive[24];	// drives c-z

char LastDrive = 'Z';
char drive_char;
TCHAR tbuf[20];
CString str;
//HWND     hDlgDLG_FREEDISK;
DWORD LastErr;
int GetDiskSpace ( LPCTSTR drive )
	{
	// Pass a drive letter.  If drive exists, return 1, else returns 0
	unsigned long FreeBytes, UsedBytes, TotalBytes;
//	unsigned long bps;	// bytes per sector
	unsigned long bps, spc ;  // sector per cluster 
	int rtnValue;
//	LPCTSTR lpDirectoryName=_T("c:");
//	lpDirectoryName=drive_char -'A';
	rtnValue=GetDiskFreeSpace(drive,&bps, &spc, &FreeBytes,&TotalBytes);
//	rtnValue=GetDiskFreeSpaceEx(drive, (PULARGE_INTEGER )&FreeBytes,(PULARGE_INTEGER )&TotalBytes,NULL);
	//bpc = (unsigned long) dtable.df_bsec * (unsigned long) dtable.df_sclus;
	if (rtnValue)
	{
		FreeBytes *= bps * spc;
		TotalBytes *= bps * spc;
		UsedBytes = TotalBytes - FreeBytes;
		Drive[drive_char - 'C'].free =  (FreeBytes/1048576L);	//  in megabytes
		Drive[drive_char - 'C'].used =  (UsedBytes/1048576L);	//  in megabytes
		Drive[drive_char - 'C'].total = (TotalBytes/1048576L);	//  in megabytes
	}
	else LastErr=GetLastError();
		//Borland version function
	return rtnValue;
	}

CDisk::CDisk(CWnd* pParent /*=NULL*/)
	: CDialog(CDisk::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDisk)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDisk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisk)
	DDX_Control(pDX, IDC_LIST1, m_Sdisk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisk, CDialog)
	//{{AFX_MSG_MAP(CDisk)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisk message handlers

void CDisk::OnOK() 
{
	// TODO: Add extra validation here

//		ReleaseDC (hDlg, hDC);
//		SendMsg(WM_SETREDRAW);	//now redraw
	
	CDialog::OnOK();
}

BOOL CDisk::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int i;
	
	// TODO: Add extra initialization here
	CString st;
		if (GetPrivateProfileString(_T("Truscope"),_T("LASTDRIVE"),_T(""),tbuf,
								4, _T("TRUSCOPE.INI")) != 0)
			{
			LastDrive = toupper(tbuf[0]);
			if ( ( LastDrive > 'Z') || ( LastDrive < 'C') ) LastDrive = 'Z';
			}

//_getdrive();
		for (char i = 0; i <= LastDrive - 'C'; i++)
		{
			drive_char = toupper(i+'C');

//			if( _chdrive( drive_char ) )
			{
				st.Format(_T("%.1s:\\"),&drive_char);
         
				if (!GetDiskSpace((LPCTSTR) st)) 
				{	// found last actual disk drive
				LastDrive = i+'C' - 1;
				break;
				}
			}
			// Drive structure now has space info for this drive 
			// Display info
		}
		m_Sdisk.ResetContent( );

		for ( i = 0; i <= LastDrive - 'C'; i++)
			{
			str.Format(_T("   %c                % 6u           % 6u           % 6u"),
				(char)(i+'C'), Drive[i].free, Drive[i].used, Drive[i].total);
			m_Sdisk.AddString(str);
			}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
