// asi.cpp : Defines the class behaviors for the application.
// JEH 01-11-00
// file/projecty renamed from asi to Truscan/Tscan 5/30/00
// Truscan.cpp
//

#include "stdafx.h"
#include "Truscan.h"

// Manually added by jeh ahead of asicatdlg.h
//#include "Asi_Ipx.h"
//#include "Asi_Msg.h"
#include "ErrMsg.h"
#include "TscanDlg.h"
#include "ChangePsWd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



extern CTscanDlg* pCTscanDlg;
/////////////////////////////////////////////////////////////////////////////
// CTscanApp

BEGIN_MESSAGE_MAP(CTscanApp, CWinApp)
	//{{AFX_MSG_MAP(CTscanApp)
	ON_COMMAND(ID_SETUP_CHANGEPW, OnSetupChangePW)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTscanApp construction

CTscanApp::CTscanApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTscanApp object

CTscanApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTscanApp initialization

// Limit to only one copy running at a time
#include "LimitSingleInstance.h"

CLimitSingleInstance g_OneInst(_T("TruScope2App"));
BOOL CTscanApp::OnIdle(LONG lCount)
{
#if 0
	// In this example, as in most applications, you should let the
   // base class CWinApp::OnIdle complete its processing before you
   // attempt any additional idle loop processing.
   if (CTscanApp::OnIdle(lCount))
      return TRUE;   

   // The base class CWinApp::OnIdle reserves the lCount values 0 
   // and 1 for the framework's own idle processing.   If you wish to
   // share idle processing time at a peer level with the framework,
   // then replace the above if-statement with a straight call to
   // CWinApp::OnIdle; and then add a case statement for lCount value
   // 0 and/or 1. Study the base class implementation first to 
   // understand how your idle loop tasks will compete with the 
   // framework's idle loop processing.

   switch (lCount)
   {
/*      case 2:
         Task1();
         return TRUE; // next time give Task2 a chance
*/
		case 100:
//         Task2();
         return TRUE; // 
		default:         return FALSE; // cycle through the idle loop tasks again
   }
#endif
   while (lCount>0) lCount--;
   return FALSE;
}


BOOL CTscanApp::InitInstance()
	{

#if 0
	_DEBUG
	// Debugging info to fix switch from debug project to release project
	afxMemDF |= checkAlwaysMemDF;
#endif


	if (g_OneInst.IsAnotherInstanceRunning() ) 
		{
		return FALSE;	//abort
		}
	// Copy from AGS3 project to locate registry info in *.ini file
	CFile f;
	BYTE buf[10000];
	CFileException e;
	TCHAR szError[1024];
	//TCHAR* pszFileName = _T("D:\\PhasedArrayGenerator\\MMI_VS10\\MCConfig.ini");
	//TCHAR* pszFileName = _T("../MCConfig.ini");
	TCHAR* pszFileName = _T("MCConfig.ini");
	if(!f.Open(pszFileName, CFile::modeReadWrite, &e))
		{
		TRACE(_T("File could not be opened %d\n"), e.m_cause);
		e.GetErrorMessage(szError, 1024);
		}
	else
		{
		f.Read(buf, 10000);
		Sleep(10);
		f.Close();
		}


 

#ifdef _I_AM_PAG
		// hook in the database fuctionality provided by MillConsoleDatabaseInterface DLL
#if 0
		typedef	BOOL (*DBConnected)();
		m_hMCDBI = LoadLibrary(_T("MillConsoleDatabaseInterface.dll"));
		m_hDBAvailable = (m_hMCDBI)? ((DBConnected)GetProcAddress(m_hMCDBI, "isDBConnected"))() : FALSE;
#endif
#endif

	if (!AfxSocketInit())
		{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
		}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	//Enable3dControls();			// Call this when using MFC in a shared DLL.. remove per vs2010 compiler
#else
	//Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


	SetRegistryKey(_T("Tuboscope"));	// gen HKEY_CUR_USR\Software\Tuboscope\Asi  key
//	WriteProfileString(_T("Version 1.0"), _T("JunkKey"), _T("This is junk"));

	CTscanDlg dlg;
	m_pMainWnd = &dlg;
	pCTscanDlg = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
	}
char szHelpDirName[80];				// directory location for help files
char szHelpFileName[80] = ".\\help\\Truscope.hlp";
char szCustomerName[128];
char szHomePath[255];	// ultimately has 'home' directory path


#if 0
void GetTuboHelp (HWND hWnd, LPCSTR HelpFile, UINT help_indx, DWORD dwData)
	{
	char  far *p;
	p = (char far *)HelpFile;		// kill compiler warn
/*	if ( strlen(szHelpDirName) > 0)
		{
		if ( (p = strchr(szHelpDirName,':')) != NULL)
			{
			c = *(p-1);
			system(toupper(c)-'A');	// change drives in DOS
            }
		chdir(szHelpDirName);
        }
*/	// We don't care what file name is passed, we are using Truscope.hlp
	WinHelp(hWnd, szHelpFileName, help_indx, dwData);
	}
#endif

void CTscanApp::OnSetupChangePW() 
{
	// TODO: Add your command handler code here
	CChangePsWd dlg;

	dlg.DoModal();
}
