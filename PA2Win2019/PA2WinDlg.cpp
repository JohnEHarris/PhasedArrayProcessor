
// PA2WinDlg.cpp : implementation file
// Author:		jeh -- spring 2017
// Purpose:	Windows MFC based dialog to run both PAP and	PAG code
//			Customization for either PAP or PAG is done with configuration file
//			called Hardware.ini
//

#include "stdafx.h"
#include "PA2Win.h"
#include "PA2WinDlg.h"
#include "afxdialogex.h"
#include "VersionsPag.h"		// DEFINITIONS FOR VERSION NUMBERS
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <stdio.h>
#include <share.h>
#include "..\Include\Global.h"
#include "..\Include\PA2Struct.h"
#include "TuboIni.h"
#include "ServerConnectionManagement.h"
#include <synchapi.h>	// get/set system clock interval in ms
#include <timeapi.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef I_AM_PAG
/*
I AM THE GUI
*/
#endif

#ifdef I_AM_PAP
/*
I AM THE PHASED ARRAY PROCESSOR
*/
#endif

class CClientConnectionManagement *pCCM[MAX_CLIENTS];	// global, static ptrs to class instances defined outside of the class definition.
class CServerConnectionManagement *pSCM[MAX_SERVERS];	// global, static ptrs to class instances define outside of the class definition.
												//  -- not created with 'new'
// C code callable from any class
//pointer objects deleted, but not nulled
// Since we are using poiter to pointer, setting pointer to 0 only sets argument pointer
// eg, pCritSec and not the pointer that pCritSec points to 
int KillLinkedList( CRITICAL_SECTION *pCritSec, CPtrList *pList )
	{
	int i;
	void *pv;
	if( (pCritSec) && (pList))
		{
		EnterCriticalSection( (LPCRITICAL_SECTION) pCritSec );
		while (i= pList->GetCount())
			{
			pv = pList->RemoveHead();
			delete pv;
			}
		delete pList;
		delete pCritSec;
		//pList = 0;
		//pCritSec = 0;
		return 1;
		}
	else
		{
		if (pCritSec == 0)	return 0;
		if (pList == 0)		return 0;
		}
	return 1;
	}




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// c routine to convert CString to char array
// make sure the ptr to the char array is big enough to hold the converted string
// converted string is null terminated.

// To fix the Unicode wreck, convert CStrings to char's for 
// operation such as sscanf and printf
// Lose significance when really using 16 bit character - but
// not when reading values from registry or other numeric ascii
// conversions.  Make sure the character array is big enough to 
// take the bytes.

void CstringToTChar(CString &s, _TCHAR *p, int nSizeOfArray)
	{
	int i;
	int nLen = s.GetLength();
	_TCHAR c;
	memset(p, 0, nSizeOfArray);
	if (nLen >= nSizeOfArray)	nLen = nSizeOfArray - 1;
	for (i = 0; i < nLen; i++)
		{
		p[i] = (_TCHAR)s.GetAt(i);
		c = (_TCHAR)s.GetAt(i);
		}
	}

void CstringToChar(CString &s, char *p, int nSizeOfArray)
	{
	int i;
	int nLen = s.GetLength();
	char c;
	memset(p, 0, nSizeOfArray);
	if (nLen >= nSizeOfArray)	nLen = nSizeOfArray - 1;
	for (i = 0; i < nLen; i++)
		{
		p[i] = (char)s.GetAt(i);
		c = (char)s.GetAt(i);
		}
	}

void CstringToChar(CString s, char *pChar)
	{
	char c;
	int i = 0;
	while (c = (char)s.GetAt(i))	{ pChar[i++] = c; }
	pChar[i] = 0;
	}

void CstringToTChar(CString s, TCHAR *pChar)
	{
	TCHAR c;
	int i = 0;
	while (c = (TCHAR)s.GetAt(i))	{ pChar[i++] = c; }
	pChar[i] = 0;
	}

char *GetTimeStringPtr(void)
	{
	static char buffer[128];
	time_t now;
	struct tm * timeinfo;
	time ( &now );
	timeinfo = localtime ( &now );
	strftime (buffer,80,"%Y_%m_%d %H:%M:%S",timeinfo);
	return &buffer[0];
	}


// global/universal time string generator
CString GetTimeString(void)
	{
	CString s;
	s = GetTimeStringPtr();
	return s;
	}


// CPA2WinDlg dialog

// Also get  'ini' file name based on the role: either a Phased Array GUI 
// or a Phased Array Processor -- PAP
//
void CPA2WinDlg::MakeDebugFiles(void)
	{
	CString t = AfxGetAppName();	// shows AGS3
	CString AppName = t+ _T(".exe");
	CString ch;	// trim characters
	int i;
		
	CString s,Fake;	// fake is a debug file with fake data and fake data msg to PAG
	CString DeBug;	// another debug file to catch printf statements since vs2015 does not output to monitor screen
	CString Commands;	// catch commands from PAG to PAP
	CString ReadBack;	// intercept read back message and write to file

	t = GetCommandLine();	// shows ""D:\PhasedArrayGenerator\PA_Master_VS2010\Debug\PhasedArrayMasterVS2010.exe" -i -d"
	i = t.Find(_T(".exe"));
	if ( i > 0)
		t.Delete(i+4,32);	// leaves ""D:\PhasedArrayGenerator\PA_Master_VS2010\Debug\PhasedArrayMasterVS2010.exe"
	ch = _T('"');		// double quote
	ch += _T(" ");		// space
	t.TrimLeft(ch);		// leading double quote shows "D:\PhasedArrayGenerator\PA_Master_VS2010\Debug\PhasedArrayMasterVS2010.exe"
	t.TrimRight(ch);	// trailing double quote shows "D:\PhasedArrayGenerator\PA_Master_VS2010\Debug\PhasedArrayMasterVS2010.exe"


	i = t.Find(AppName);		// we know the name of the program
	t.Delete(i,32);				// t has path to 'exe' w/o the exe file name
	Fake = t;
	Fake = t + _T("FakeData.txt");
	DeBug = t + _T("Debug.log");
	Commands = t + _T("Commands.log");
	ReadBack = t + _T("ReadBack.log");
	// shows "D:\PhasedArrayGenerator\PA_Master_VS2010\Debug\"
	// The name of the App becomes an implicit part of the key
	// We only write Tuboscope, but clever windows makes a subregistry entry of AdpMMI
	// SetRegistryKey Causes application settings to be stored in the registry instead of .INI files.
	// SetRegistryKey(_T("Tuboscope"));	// gen HKEY_CUR_USR\Software\Tuboscope\AdpMMI  key

#ifdef I_AM_PAG
	// The GUI also know as PAG
	t += _T("HardwareConfig_GUI.ini"); // shows "D:\PhasedArrayGenerator\PA_Master_VS2010\.\Debug\HardwareConfig.ini"
	// t = _T("D:\\PhasedArrayGenerator\\PA_Master_VS2010\\Debug\\HardwareConfig.ini");
	// m_pszProfileName=_tcsdup(t); // File in the exe directory whether \Debug or \Release
#else
	// Was called Service App. Now known as PAP
	t += _T("HardwareConfig_PAP.ini");
	TRACE(_T("Using ini file: %s\n"), t);
#endif

	// To use an INI file instead, set m_pszProfileName to the ini file path
	// First free the string allocated by MFC at CWinApp startup.
	// The string is allocated before InitInstance is called.
	// free((void*)m_pszProfileName);
	//Change the name to the .INI file.
	//The CWinApp destructor will free the memory.
	if ( gDlg.pTuboIni == NULL )
		gDlg.pTuboIni = new CTuboIni(t);	// 12
	//m_pTuboIni = new CTuboIni(t);
	//i = sizeof (CTuboIni);

	m_nFakeDataExists = m_nDebugLogExists = m_mCommandLogExists = m_nReadBackExists = 0;

	CFileException fileException;
  
	if ( !m_FakeData.Open( Fake, CFile::modeCreate |   
			CFile::modeReadWrite | CFile::shareDenyNone , &fileException ) )
		{
		TRACE( _T("Can't open file %s, error = %u\n"),
		Fake, fileException.m_cause );
		}
	else m_nFakeDataExists = 1;

	//m_DebugLog
	if ( !m_DebugLog.Open( DeBug, CFile::modeCreate |   
			CFile::modeReadWrite | CFile::shareDenyNone , &fileException ) )
		{
		TRACE( _T("Can't open file %s, error = %u\n"),
		DeBug, fileException.m_cause );
		}
	else m_nDebugLogExists = 1;

	// m_CommandLog
	if (!m_CommandLog.Open(Commands, CFile::modeCreate |
		CFile::modeReadWrite | CFile::shareDenyNone, &fileException))
		{
		TRACE(_T("Can't open file %s, error = %u\n"),
			Commands, fileException.m_cause);
		}
	else m_mCommandLogExists = 1;

	// ReadBack of commands written to NIOS addresses
	if (!m_ReadBackLog.Open(ReadBack, CFile::modeCreate |
		CFile::modeReadWrite | CFile::shareDenyNone, &fileException))
		{
		TRACE(_T("Can't open file %s, error = %u\n"),
			ReadBack, fileException.m_cause);
		}
	else m_nReadBackExists = 1;


	}


CPA2WinDlg::CPA2WinDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PA2WIN_DIALOG, pParent)
	{
	int i,j;
	nLoc = 20;
	m_ptheApp = (CPA2WinApp *) AfxGetApp();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	gDlg.pUIDlg = this;
	nShutDown = 0;
	m_nMsgSeqCnt = 0;

	g_hTimerTick = ::CreateEvent(0, TRUE, FALSE, 0);
	//memset((void*)stSCM, 0, sizeof(stSCM)*MAX_SERVERS);
	//memset((void*)stCCM, 0, sizeof(stCCM)*MAX_CLIENTS);

	for ( i = 0; i < MAX_SERVERS; i++)
		{		
		pSCM[i] = NULL;
		for (j = 0; j < MAX_CLIENTS; j++)
			{
			stSCM[i].bActualClientConnection[j] = 0xff;	// a static structure
			}
		}

	for (i = 0; i < MAX_CLIENTS; i++)
		{		
		pCCM[i] = NULL;	
		}

	// DEFAULT VALUES - CAN BE OVERWRIDDEN WITH COMMAND FROM PAG
	gMaxChnlsPerMainBang	= MAX_CHNLS_PER_MAIN_BANG;
	gMaxSeqCount = MAX_SEQ_COUNT;
	pCSSaveDebug = new CRITICAL_SECTION();
	InitializeCriticalSectionAndSpinCount(pCSSaveDebug,4);
	pCSSaveCommands = new CRITICAL_SECTION();
	InitializeCriticalSectionAndSpinCount(pCSSaveCommands, 4);
	pCSSaveReadBack = new CRITICAL_SECTION();
	InitializeCriticalSectionAndSpinCount(pCSSaveReadBack, 4);

	MakeDebugFiles();

/***********************************************************************

This big comment was a # if 0 but made Visual Studio miss the definition of GetServerConnectionManagementInfo()

MAX_SERVERS				// how many servers supported by this application program
MAX_CLIENTS_PER_SERVER	// how many client connection on each server

PAM server that Instruments connects to:
192.168.10.10 : 7502
During development,Instruments at 192.168.10.201+ connect to PAG at 192.168.10.10 on port 7502
IP Port is the port the server listens on. 
The Client Base IP is the IP of the 1st element in the 
ST_SERVERS_CLIENT_CONNECTION *pClientConnection[MAX_CLIENTS_PER_SERVER];
Here, ClientBaseIp[16] = 192.168.10.201

************************************************************************************/

	// get configuration info from ini file
	GetServerConnectionManagementInfo();
	SaveServerConnectionManagementInfo();

	GetClientConnectionManagementInfo();
	SaveClientConnectionManagementInfo();
#if 0
	// apparently only for multimedia apps
	TIMECAPS tc;	// tc.wPeriodMin; tc.wPeriodMax in milliseconds
	j = sizeof(tc);
	i = timeGetDevCaps(&tc, j);
	if (i == MMSYSERR_NOERROR)
		{
		UINT min = tc.wPeriodMin;
		UINT max = tc.wPeriodMax;
		}
#endif
	}

CPA2WinDlg::~CPA2WinDlg()
	{
	int i, j;
	i = j = 0;
	// In general, whatever was built in OnInitDialog should be destroyed here
	nShutDown = 1;	// Stop TestThread worker loop
	// lower thread priority to allow signaled thread chance to exit
	AfxGetThread()->SetThreadPriority( THREAD_PRIORITY_BELOW_NORMAL );

	// KIll the test thread
	if (m_pTestThread)
		{
		::SetEvent(g_hTimerTick);
		Sleep(250);
		Sleep(250);
		}

	DestroyCCM();
	DestroySCM();
		
	CloseFakeData();
	CloseDebugLog();
	CloseCommandLog();
	CloseReadBackLog();

	Sleep( 100 );
	if (pCSSaveDebug)
		delete pCSSaveDebug;
	pCSSaveDebug = 0;

	if (pCSSaveCommands)
		delete pCSSaveCommands;
	pCSSaveCommands = 0;

	if (pCSSaveReadBack)
		delete pCSSaveReadBack;
	pCSSaveReadBack = 0;

	if (gDlg.pNcNx)
		{
		delete gDlg.pNcNx;
		gDlg.pNcNx = 0;
		}
	if (gDlg.pTuboIni)
		{
		delete gDlg.pTuboIni;
		gDlg.pTuboIni = 0;
		}


	TRACE( _T( "CPA2WinDlg destructor has run\n" ) );

	}

void CPA2WinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_LIST1, m_lbOutput );
	}

BEGIN_MESSAGE_MAP(CPA2WinDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED( IDOK, &CPA2WinDlg::OnBnClickedOk )
	ON_BN_CLICKED( IDCANCEL, &CPA2WinDlg::OnBnClickedCancel )
	ON_COMMAND( ID_FILE_EXIT, &CPA2WinDlg::OnFileExit )
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_COMMAND( ID_CONFIGURE_NCNX, &CPA2WinDlg::OnConfigureNcNx )
	ON_BN_CLICKED( IDC_BN_ERASE_DBG, &CPA2WinDlg::OnBnClickedBnEraseDbg )
	ON_BN_CLICKED( IDC_BN_SHUTDOWN, &CPA2WinDlg::OnBnClickedBnShutdown )
	ON_LBN_SELCHANGE(IDC_LIST1, &CPA2WinDlg::OnLbnSelchangeList1)
END_MESSAGE_MAP()


// CPA2WinDlg message handlers

// Read pap number from a usb memory stick. File name is 4.Wall  for example
// Implies the usb stick will identify 1 of 8 different PAP's
// Look for usb stick starting on G drive assuming the PAP code/Windows machine
// has only a C hard drive. File content is a single number from 0 to 7
// Default pap number is held in file on C drive
// Clive/Robert chage file format to be EITHER n.Soco or n.wall where n is the machine number.
// This make searching more tedious. The PAP will only search directories (D, E, F an finally C)
// In each directory I will search for n.wall where n ranges from 1-8
// For my purposes, I will subtract 1 for a range of 0-7  -- NOT


#ifdef I_AM_PAP
// There can only be 1 wall assignment per PAP for this implementation
// If the wall assignment changes due to insertion of a memory stick with
// a different number, the old number file must be deleted
// This function specifies the file name to retain
// all other files in range of 1-8 with format n.wall will be deleted
void CPA2WinDlg::DeleteOldPapNumbers(int nNewPap)
	{
	char DeleteTarget[256] = "C:\\LocalAppExes\\MyID\\1.wall";
	char DeleteName[256];
	CString sPath, s;
	int i, j;
	char nC;
	CFile DelFile;	// file name to delet if it exists
	CFileException fileException;

	s = DeleteTarget;
	j = s.Find(_T(".wall"));
	j--;	// move back to machine number
	DeleteTarget[j] = 0;
	// scan for 1-8 file name to delete
	for (i = 1; i < 9; i++)
		{
		if ( i != nNewPap) // erase previous wall machine assignment files
			{
			sPath = DeleteTarget;	//C:\\LocalAppExes\\MyID\\  .
			nC = '0' + i;	// make ascii number 1-8
			sPath += nC;
			sPath += _T(".wall");		//C:\\LocalAppExes\\MyID\\nC.wall
			if (DelFile.Open(sPath, CFile::modeRead | CFile::shareDenyNone, &fileException))
				{
				DelFile.Close();
				CstringToChar(sPath, DeleteName);
				DeleteFileA(DeleteName);
				}
			}
		}
	}

// Always attempt to read from inserted memory stick
// As a fall back, look on C drive
// Make sure only 1 wall system assignment is in C drive
// Assume memory stick can take drive numbers D: thru G:. Start at G and search backward
//
void CPA2WinDlg::ReadPAPnumber(void)
	{
	//m_PapNumberFile
	CString sPath, s;
	char PathName[16] = "G:\\1.wall";		//"G:\\1.wall"; -- number '1' in 4th position
	char LocalApp[128] = "C:\\LocalAppExes\\MyID";
	char FileName[16];
	char AltFileName[16] = "\\1.wall";
	//char FileText[1024];	//info to be written into permanent LocalAppExes file in ascii format
	// Victor wants to write to the file, so we won't

	CFileException fileException;
	int i, j;
	gbAssignedPAPNumber = 128;	// INVALID PAP number

	for (i = 5; i > 0; i--)
		{	// drive letter loop 
		memcpy(FileName, PathName, 16);		//G:\1.wall  --the '1' is in  index 3
		for (j = 0; j < 5; j++)	
			{ // look for 1-8 for instrument number
			sPath = FileName;	// convert char string to MFC string
			if (!m_PapNumberFile.Open(sPath, CFile::modeRead | CFile::shareDenyNone, &fileException))
				{	// did not open the file
				//				TRACE(_T("Can't open file %s, error = %u\n"),
				//					sPath, fileException.m_cause);
				FileName[3]++;	// increment the number in the file name.. starts with 1
				}

			else
				{	// found it on memory stick
				s.Format(_T("Found file %s\n"), sPath);
				TRACE(s);
				gbAssignedPAPNumber = FileName[3] - '0';	// map char 1-8 to binary  1-8 
				m_PapNumberFile.Close();
				// create the same file name in C:\\ for default when memory stick is missing
				if (FileName[0] == 'C')
					{
					s.Format(_T("Memory stick missing, using default PAP number %d asssignment from C: drive\n"), gbAssignedPAPNumber);
					TRACE(s);
					return;
					}

				// Found file on Memory stick, copy to LocalAppExes:
				if (FileName[0] != 'C')
					{
					sPath = LocalApp;
					sPath += (char *)&FileName[2];
					if (!m_AltPapNumberFile.Open(sPath, CFile::modeReadWrite | CFile::shareDenyNone, &fileException))
						{
						TRACE(_T("Can't open file %s, error = %u\n"),
								sPath, fileException.m_cause);
						}
					else
						{
						m_AltPapNumberFile.SeekToEnd();
						s.Format(_T("\n\nCopied Wall unit number %d file to LocalAppExes folder\n"), gbAssignedPAPNumber);
						TRACE(s);
						pMainDlg->SaveDebugLog(s);
						}
#if 0
					// don't write to the file in C, Victor wants to wrtie network info there
					try
						{
						CstringToChar(s, FileText);
						m_AltPapNumberFile.Write(FileText, (int)strlen(FileText));
						m_AltPapNumberFile.Close();
						}
					catch (CFileException* e)
						{
						e->ReportError();
						e->Delete();
						}
#endif
					// Delete all other competing file assignments
					DeleteOldPapNumbers(gbAssignedPAPNumber); // keeps this one, deletes all others
					return;
					}
				}	// found it

			}	// look for 1-8 for instrument number

		PathName[0]--;	// setp thru drive letters. G, F, E, D, C
		}	// (i = 5; i > 0; i--) loop thru drive letters

	if (i == 0)
		{
		TRACE(_T("Failed to find file name and thus PAP number\n"));	// debug shows C:\\9.wall
		if (FileName[0] == 'C')
			{
			// attempt to use stored file name in  LocalAppExes\MyID
			GetPAPFromCDrive();
			if (gbAssignedPAPNumber < 8)
				{
				s.Format(_T("PAP number = %d from C drive\n"), gbAssignedPAPNumber);
				TRACE(s);
				pMainDlg->SaveDebugLog(s);
				return;		// succeeded in finding wall machine number
				}
			}


		gbAssignedPAPNumber = 8;	//INVALID
		s.Format(_T("Invalid PAP number = %d\n"), gbAssignedPAPNumber);
		TRACE(s);
		pMainDlg->SaveDebugLog(s);
		return;
		}
	return;
	}
#else

void CPA2WinDlg::ReadPAPnumber(void)
	{
	gbAssignedPAPNumber = 0;
	}
#endif

#define SIMULATOR
// Find the PAP number in folder C:\LocalAppExes\MyID
void CPA2WinDlg::GetPAPFromCDrive(void)
	{
	CString sPath, s;
#ifdef SIMULATOR
	char PathName[128] = "d:\\1.wall";
#else
	char PathName[128] =  "C:\\LocalAppExes\\MyID\\1.wall";
#endif
	int i, j;	// character position for numerical value of wall file name
	CString t;
	CFileException fileException;
	t = PathName;
	i = t.Find(_T(".wall"));	// character location just past integer identifying wall number
	i--;	// point to location of the 1 character in 1.wall

	for (j = 0; j < 8; j++)
		{
		sPath = PathName;
		if (!m_PapNumberFile.Open(sPath, CFile::modeRead | CFile::shareDenyNone, &fileException))
			{	// did not open the file
			//				TRACE(_T("Can't open file %s, error = %u\n"),
			//					sPath, fileException.m_cause);
			PathName[i]++;	// increment the number in the file name.. starts with 1
			}

		else
			{	// found it on C drive
			s.Format(_T("Found file %s\n"), sPath);
			TRACE(s);
			gbAssignedPAPNumber = PathName[i] - '0';	// map char 1-8 to binary  1-8 
			m_PapNumberFile.Close();
			// create the same file name in C:\\ for default when memory stick is missing
			return;
			}
		}	//for (j = 0; j < 8; j++)
	gbAssignedPAPNumber = 8;	//INVALID
	return;
	}


// Display a matrix of n rows of servers with m columns of connected clients.
void CPA2WinDlg::ShowConnectedClients(void)
	{
	int i;
	i = 0;
	}

void CPA2WinDlg::AddConnectedClient(int nServer, int nClientNum, CString& sIP4)
	{
	int i;
	i = 0;
	}


BOOL CPA2WinDlg::OnInitDialog()
	{
	CString sDlgName, s;
	CDialogEx::OnInitDialog();
	int i;
	s.Format( PA2_VERSION );
	i = sizeof(ASCAN_DATA);
	i = sizeof(IDATA_FROM_HW);
#ifdef I_AM_PAG
	sDlgName = _T( "PA2Win -- Phase Array GUI Version -- PAG " );
	#else
	sDlgName = _T( "PA2Win -- Phase Array Processor Version -- PAP " );
#endif
	bAppIsClosing = 0;	// just started
	sDlgName += s;
	SetWindowText(sDlgName);
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	i = sizeof(gPksPerSec);
	memset((void *)&gPksPerSec, 0, sizeof(gPksPerSec));
	gnFifoCnt = 0;

	m_sHwVerAdc = m_sSwVerAdc = m_sHwVerPulser = m_sSwVerPulser = _T("");
	// TODO: Add extra initialization here
	m_lbOutput.ResetContent();
	StartTimer();
	// Used to attempt to connect to servers

// Test thread creation and thread message posting
	m_pTestThread = (CTestThread *) AfxBeginThread(
										RUNTIME_CLASS(CTestThread),
										THREAD_PRIORITY_NORMAL,
										0,	// stack size
										0,	// create flag, run on start//CREATE_SUSPENDED,	// runstate
										NULL);	// security ptr


	// Test posting a message to newly created thread
	if (m_pTestThread)
		i = m_pTestThread->PostThreadMessage(WM_USER_THREAD_HELLO_WORLD,1,5L);

	Sleep(50);
	//PAP will get its PAP number (0-7) by reading a memory stick -- not a very reliable way
	//PAP will get its IP address via DHCP. Hence, PAP number will not be tied to IP address
	// At present (2018-08-13) both servers use the same NIC for PAP
//#ifdef I_AM_PAP
	ReadPAPnumber();
//#endif
	GetAllIP4AddrForThisMachine();	
	InitializeClientConnectionManagement();	// moved from after thread list creation
	InitializeServerConnectionManagement();

	StructSizes();
	s.Format(_T("Version =   %d_%02d_%03d\n\n"), VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
	TRACE(s);
	DlgDebugOut(s);

	memset((void*)guPktAttempts, 0, sizeof(guPktAttempts));
	i = sizeof(guPktAttempts);
	gwPap_Prf = 10;		// just a guess until PAG sets prf. Will steal the value out of msg to Pulser
	m_uMsgSeqCntChange = 0;		// wait til adc running and then store version numbers as strings
#if 0

	// Set priority to lower maybe good?
	DWORD dwError, dwThreadPri;
	// curious to know process priority and thread priority
	HANDLE hProcess = GetCurrentProcess();			// = -1
	DWORD dwClassPriority = GetPriorityClass(hProcess);		// = normal = 0x20
	HANDLE hThread = GetCurrentThread();		// = -2
	int nPriority = GetThreadPriority(hThread); // = 0
#if 1
	if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL))
		{
		dwError = GetLastError();
		if (ERROR_THREAD_MODE_ALREADY_BACKGROUND == dwError)
			_tprintf(TEXT("Already in background mode\n"));
		else _tprintf(TEXT("Failed to enter background mode (%d)\n"), dwError);
		//goto Cleanup;
		}
#endif

	// Display thread priority

	dwThreadPri = GetThreadPriority(GetCurrentThread());	// -1 = THREAD_PRIORITY_BELOW_NORMAL

	_tprintf(TEXT("Current thread priority is 0x%x\n"), dwThreadPri);

#endif
	return TRUE;  // return TRUE  unless you set the focus to a control
	}

void CPA2WinDlg::GetServerConnectionManagementInfo(void)
	{
	if (gDlg.pTuboIni == NULL)	return;
	int i;
	CString szPort, szIp, szI, sSrvSection;
	sSrvSection = _T("ServerConnectionManagement");

	
	gnMaxServers = gDlg.pTuboIni->GetProfileInt(sSrvSection, _T("! MaxServers"), MAX_SERVERS);
	//gnMaxServers = m_ptheApp->GetProfileIntA(_T("ServerConnectionManagement"),_T("[-]MaxServers"), MAX_SERVERS);
	gnMaxClientsPerServer = gDlg.pTuboIni->GetProfileInt(sSrvSection,_T("# MaxClientsPerServer"), MAX_CLIENTS_PER_SERVER);
	szI.Format(_T("gnMaxClientsPerServer = %d\n"), gnMaxClientsPerServer );
	TRACE(szI);
	for ( i = 0; i < gnMaxServers; i++)
		{
		szI.Format(_T("%d-Server_IP_Addr"), i);
		szIp = gDlg.pTuboIni->GetProfileString(sSrvSection, szI, _T("192.168.10.10"));
		szI += _T("  ") + szIp + _T("\n");
		//TRACE(szI);
		CstringToChar(szIp,gServerArray[i].Ip);
		// Server Name
		szI.Format(_T("%d-Server_Description"), i);

		stSCM[i].sServerDescription = gDlg.pTuboIni->GetProfileString(sSrvSection, szI,_T("Unknown"));
		// The server's port that listens for clients to connect
		szI.Format(_T("%d-Server_IP_Port"), i);
		gServerArray[i].uPort =  gDlg.pTuboIni->GetProfileInt(sSrvSection,szI, 7502);

		szI.Format(_T("%d-Client_Packet_Size"), i);
		gServerArray[i].nPacketSize = gDlg.pTuboIni->GetProfileInt(sSrvSection,szI,INSTRUMENT_PACKET_SIZE);

		szI.Format(_T("%d-Client_Base_IP"), i);
#ifdef I_AM_PAP
		szIp = gDlg.pTuboIni->GetProfileString(sSrvSection,szI, _T("192.168.10.200"));
#else
		szIp = gDlg.pTuboIni->GetProfileString(sSrvSection, szI, _T("192.168.11.40"));
#endif
		CstringToChar(szIp,gServerArray[i].ClientBaseIp);
		}
	}


// Save the hardware configuration information for the SCM to the registry
// When using the CTuboIni class, write the section header by itself before writing any key values
// Only for previous MIT version of TuboIni
//
// character order space ! # $ & * + - before numbers.
void CPA2WinDlg::SaveServerConnectionManagementInfo(void)
	{
	if (gDlg.pTuboIni == NULL)	return;
	int i;
	CString szPort, szIp, szI, sSrvSection;
	sSrvSection = _T("ServerConnectionManagement");
	//if (m_pTuboIni->m_pDictionary == NULL)	return;

	// Write the section header -- utilize empty-string Key and empty-string value
	// gDlg.pTuboIni->WriteProfileString(_T("ServerConnectionManagement"),_T(""), _T(""));

	gDlg.pTuboIni->WriteProfileInt(sSrvSection, _T("! MaxServers"), gnMaxServers);
	gDlg.pTuboIni->WriteProfileInt(sSrvSection,_T("# MaxClientsPerServer"), gnMaxClientsPerServer);
	for ( i = 0; i < gnMaxServers; i++)
		{
		switch (i)
			{
		case 0:
			szI.Format(_T("%d-Server_Description"), i);
#ifdef I_AM_PAG
			szIp = _T("PAG Server for PAP's 1-N");
#else
			szIp = _T("PAP Server for Instruments 1-N");
#endif
			gDlg.pTuboIni->WriteProfileString(sSrvSection,szI, szIp);
			break;
		case 1:
			szI.Format(_T("%d-Server_Description"), i);
#ifdef I_AM_PAG
			szIp = _T("PAG Server for All Wall 1-N");
#else
			szIp = _T("PAP Server All Walls for Instruments 1-N");
#endif
			gDlg.pTuboIni->WriteProfileString(sSrvSection, szI, szIp);
			break;
		default:
			break;
			}
		szI.Format(_T("%d-Server_IP_Addr"), i);
		szIp = gServerArray[i].Ip;

		gDlg.pTuboIni->WriteProfileString(sSrvSection,szI, szIp);
		szI.Format(_T("%d-Server_IP_Port"), i);
		gDlg.pTuboIni->WriteProfileInt(sSrvSection,szI, gServerArray[i].uPort);
		szI.Format(_T("%d-Client_Packet_Size"), i);
		gDlg.pTuboIni->WriteProfileInt(sSrvSection,szI,gServerArray[i].nPacketSize);
		
		szI.Format(_T("%d-Client_Base_IP"), i);
		szIp = gServerArray[i].ClientBaseIp;
		gDlg.pTuboIni->WriteProfileString(sSrvSection,szI, szIp);
		}
	}


// Read the registry/file to determine the hardware configuration of the CCM
// PAM clients connect to the PAG server which is listening at port 7501
// Client 0 should be the connection to the PAG server.
// 2016-05-17 The PAM now called the Receiver is a client to only the PT
// 2nd client if connected to PAG_AW server for  testing.
// PAG and PAG_AW will be separate machines in the general case
void CPA2WinDlg::GetClientConnectionManagementInfo()
	{
	int i;
	CString szPort, szIp, szI, sClientSection;
		
	if (gDlg.pTuboIni == NULL)	return;

	sClientSection = _T("ClientConnectionManagement");

	gnMaxClients = gDlg.pTuboIni->GetProfileInt(sClientSection,_T("! MaxClients"), MAX_CLIENTS);
	for ( i = 0; i < gnMaxClients; i++)
		{
		szI.Format(_T("%d-Client_Name"), i);	// url of the client machine
		stSocketNames[i].sClientName =  gDlg.pTuboIni->GetProfileString(sClientSection,szI, _T("localhost"));
		szI.Format(_T("%d-Client_IP4"), i);	// dotted IP 192.168.10.10 etc
		stSocketNames[i].sClientIP4 =  gDlg.pTuboIni->GetProfileString(sClientSection,szI, _T(""));

		szI.Format(_T("%d-Server_Name"), i);	// url of the client machine
		stSocketNames[i].sServerName =  gDlg.pTuboIni->GetProfileString(sClientSection,szI, _T(""));
		szI.Format(_T("%d-Server_IP4"), i);	// dotted IP 192.168.10.10 etc
		stSocketNames[i].sServerIP4 =  gDlg.pTuboIni->GetProfileString(sClientSection,szI, _T("192.168.11.20"));

		szI.Format(_T("%d-Server_Listen_Port"), i);
		stSocketNames[i].nPort = gDlg.pTuboIni->GetProfileInt(sClientSection,szI, 7501);

		szI.Format(_T("%d-Server_Cmd_Packet_Size"), i);
		stSocketNames[i].nPacketSize = gDlg.pTuboIni->GetProfileInt( sClientSection, szI, 1460 );	// sizeof( MMI_CMD ));

		szI.Format(_T("%d-Win_Version"), i);
		stSocketNames[i].nWinVersion = gDlg.pTuboIni->GetProfileInt(sClientSection,szI, 7);

		};
	}
void CPA2WinDlg::SaveClientConnectionManagementInfo()
	{
	int i;
	CString szPort, szIp, szI, szComment;
	m_ptheApp->WriteProfileInt(_T("ClientConnectionManagement"),_T("[-]MaxClients"), gnMaxClients);
	for ( i = 0; i < gnMaxClients; i++)
		{
		szI.Format(_T("[%d]Comment"), i);
		switch (i)
			{
#ifdef I_AM_PAG
		case 0:	// PAG to SysCp
			szComment = _T("PAG as a client of SysCp");
			m_ptheApp->WriteProfileStringW (_T("ClientConnectionManagement"),szI, szComment);
			break;
		case 1:	// PAG to GDP
			szComment = _T("PAG as a client of GDP");
			m_ptheApp->WriteProfileStringW (_T("ClientConnectionManagement"),szI, szComment);
			break;
		case 2:	// PAG to DataBase
			szComment = _T("PAG as a client of DataBase");
			m_ptheApp->WriteProfileStringW (_T("ClientConnectionManagement"),szI, szComment);
			break;
		default:
			szComment = _T("PAG as a client of ???");
			m_ptheApp->WriteProfileStringW (_T("ClientConnectionManagement"),szI, szComment);
			break;
#else
			// PAP assignemnts
		case 0:
			szComment = _T("PAP as a client of PAG");
			m_ptheApp->WriteProfileStringW(_T("ClientConnectionManagement"), szI, szComment);
			break;
		case 1:
			szComment = _T("PAP_AW as a client of PAG_AW");
			m_ptheApp->WriteProfileStringW(_T("ClientConnectionManagement"), szI, szComment);
			break;
		default:
			szComment = _T("PAP as a client of PAG");
			m_ptheApp->WriteProfileStringW (_T("ClientConnectionManagement"),szI, szComment);
			break;
#endif
			}

		szI.Format(_T("[%d]ClientName"), i);	// url of the client machine
		m_ptheApp->WriteProfileStringW (_T("ClientConnectionManagement"),szI, stSocketNames[i].sClientName);
		szI.Format(_T("[%d]ClientIP4"), i);	// dotted IP 192.168.10.10 etc
		m_ptheApp->WriteProfileStringW(_T("ClientConnectionManagement"),szI, stSocketNames[i].sClientIP4);

		szI.Format(_T("[%d]ServerName"), i);	// url of the server machine the client wants to connect to
		m_ptheApp->WriteProfileStringW(_T("ClientConnectionManagement"),szI, stSocketNames[i].sServerName);
		szI.Format(_T("[%d]ServerIP4"), i);	// dotted IP 192.168.10.10 etc
		m_ptheApp->WriteProfileStringW(_T("ClientConnectionManagement"),szI, stSocketNames[i].sServerIP4);

		szI.Format(_T("[%d]ListenPort"), i);	// Server listens for client on ListenPort
		m_ptheApp->WriteProfileInt(_T("ClientConnectionManagement"),szI, stSocketNames[i].nPort);

		szI.Format(_T("[%d]Packet Size"), i);
		m_ptheApp->WriteProfileInt(_T("ClientConnectionManagement"),szI, stSocketNames[i].nPacketSize);

		szI.Format(_T("[%d]Win Version"), i);
		m_ptheApp->WriteProfileInt(_T("ClientConnectionManagement"),szI, stSocketNames[i].nWinVersion);

		}
	}


// Assumes the first client connection from the PAP is to the Receiver or PAG
// hence stSocketNames[0]
// convert characters 192.168.10.200 to uint
void CPA2WinDlg::SetMy_PAP_Number(CString &Ip4, UINT uPort)
	{
	char txt[32];
	CstringToChar(stSocketNames[0].sClientIP4, txt);
	UINT uBaseIp = ntohl(inet_addr(txt));
	CstringToChar(Ip4, txt);
	UINT uMyIp = ntohl(inet_addr(txt));
	m_nPapNumber = uMyIp - uBaseIp;
	m_uPapPort = uPort;
	}



void CPA2WinDlg::StartTimer()
	{	// helper function to start timer with one call
	if (m_uStatTimer)	return;	// already running

	// 70 ticks per second
	m_uStatTimer = (UINT)SetTimer(IDT_TIMER, 1000, NULL);
	if (!m_uStatTimer) MessageBox(_T("Failed to start timer"));
	m_nTimerCount = 0;
	}

void CPA2WinDlg::StopTimer()
	{	// helper function to stop timer with one call
	if (m_uStatTimer)
		{
		KillTimer( m_uStatTimer );
		m_uStatTimer = 0;
		}
	}

void CPA2WinDlg::GetAllIP4AddrForThisMachine()
	{
	USES_CONVERSION;
	CString s;

	TCHAR sFQDN [MAX_FQDN_LENGTH + 1];		// fully qualified domain name string
	DWORD dwFQDN = MAX_FQDN_LENGTH + 1;		// buffer for sFQDN

	// because we are running a system using DHCP, DNS,
	// and dynamic IP addresses, we have no prior knowledge
	// about any given host's IP address so we have to
	// fish for it

	// find out where we are
	CString sComputerName = _T("");		// FQDN of this host
	CString theIP = _T("");				// the stringized ip

    ::GetComputerNameEx(ComputerNameDnsFullyQualified,
		                (LPTSTR) sFQDN, 
		                &dwFQDN);

	sComputerName = W2A(sFQDN);


	// make sure we're searching for something real
    if (sComputerName.IsEmpty())
		return;		// theIP;

//	struct sockaddr_in theHost;	// structure used in the conversion
	struct in_addr addr;		// jeh from help system
	HOSTENT * hostent;			// structure returned by gethostbyname
	int i;
	for ( i = 0; i < 20; i++) sThisMachineIP4Addr[i].Empty();
	i = 0;

    // do the lookup
	hostent = gethostbyname(W2A(sComputerName));
	// Note  The gethostbyname function has been deprecated by the introduction of the getaddrinfo function. 
	// Developers creating Windows Sockets 2 applications are urged to use the getaddrinfo function instead of gethostbyname. 
	// getaddrinfo() is preferred way to get this info... but realy complex. Use help to find getaddrinfo
	// Developers are encouraged to use the GetAddrInfoW Unicode function rather than the getaddrinfo ANSI function.

	// make sure we found something
	if (hostent != NULL)
		{
		// jeh code to show all hosts.. from help system for hostent structure
		if (hostent->h_addrtype == AF_INET)
			{
			while (hostent->h_addr_list[i] != 0)
				{
				uThisMachineIP4Addr[i] = addr.s_addr = *(u_long *) hostent->h_addr_list[i];
				s = inet_ntoa(addr);
				TRACE1("Host IP4 addr = %s\n",s);
				sThisMachineIP4Addr[i++] = s;
				}
			// After all 'real' IP4 added, add loopback into table
			uThisMachineIP4Addr[i]		= 0x0100007f;
			sThisMachineIP4Addr[i++]	= _T("127.0.0.1");
			}
		}
	}

#ifdef I_AM_PAG
void CPA2WinDlg::InitializeClientConnectionManagement(void)
	{
	int i, j;
	j = 0;
	// Instantiate all CCM instances for as many client connections as are going to be supported
	// Convention is for case 0 to be SysCp connection since all connect to SysCp
	// Convention is for case 1 to be connection to the GDP
	// Convention is for case 2 to be connection to the database
	// Convention is for case 3 to be connection to the PLC -- no handled with clives class
	// 	All these connections are tcp/ip clients and the other end is a tcp/ip server.
	CString sClientIP,  sServerIp, sServerName, s;	// sServerName use the url for this server
	UINT uServerPort = 0;

	for ( i = 0; i < MAX_CLIENTS; i++)
		{
		if (pCCM[i])	continue;	// instance already exists
		switch (i)
			{
		case 0:
		// If here I am the GUI. In test mode have nothing to connect to.
	
			if (0 == FindServerSideIP( i ))
				{
				// Could not find the server
				TRACE( "Could not find a server for SysCp - case 0\n" );
				break;
				}
			if (0 == FindClientSideIP( i ))
				{	// find client side connection for 1st connection... probably the same IP for all 
					// client side connections unless more than one NIC
				TRACE( "Could not find a client IP for SysCp - case 0\n" );
				break;
				}
#if 0		
			// Make a specific child class of CCM to handle the SysCp
						pCCM_SysCp = (CCCM_SysCp *)new CCCM_SysCp( i, PHASE_ARRAY_WALL_COMMAND_PROCESSOR );	// ptr to the class, not the connection structure
						if (NULL == pCCM_SysCp)
							{
							TRACE1( "pCCM_SysCp[%d] is NULL\n", i );
							break;
							}
						if (pCCM_SysCp->m_pstCCM == NULL)
							{
							TRACE1( "pCCM_SysCp[%d]->m_pstCCM is NULL\n", i );
							break;
							}
						//pCCM[i] = pCCM_SysCp; causes problem on shut down

						pCCM_SysCp->UniqueProc();	//just for debugging
						pCCM_SysCp->SetSocketName( _T( "CCM_SysCp" ) );
						pCCM_SysCp->SetWinVersion( theApp.m_iWinVer );

						if (pCCM_SysCp->m_pstCCM->sClientIP4.GetLength() > 6)	// copy found IP for client into identity struct
							pCCM_SysCp->SetClientIp( pCCM_SysCp->m_pstCCM->sClientIP4 );
						if (pCCM_SysCp->m_pstCCM->sServerIP4.GetLength() > 6)	// copy found IP for server into identity struct
							pCCM_SysCp->SetServerIp( pCCM_SysCp->m_pstCCM->sServerIP4 );

						// Set thread priorities for send and receive threads
						pCCM_SysCp->m_pstCCM->nReceivePriority		= THREAD_PRIORITY_ABOVE_NORMAL;
						pCCM_SysCp->m_pstCCM->nSendPriority			= THREAD_PRIORITY_BELOW_NORMAL;
						pCCM_SysCp->m_pstCCM->nCmdProcessPriority	= THREAD_PRIORITY_NORMAL;	// not used by PAG
						pCCM_SysCp->CreateReceiveThread();		Sleep( 50 );
						pCCM_SysCp->CreateSendThread();			Sleep( 50 );
						pCCM_SysCp->InitReceiveThread();		Sleep( 50 );
						// causes CClientCommunicationThread::InitTcpThread(WPARAM w, LPARAM lParam) to run
						pCCM_SysCp->InitSendThread();			Sleep( 50 );
						//pCCM_PAG->CreateCmdProcessThread();		Sleep(50);	// only in PAM
#endif
				break;	// case 0


		case 1:		// assume connectiong to GDP
			if (!FindServerSideIP( i ))
				{
				TRACE( "Could not find server IP for GDP.. we are toast\n" );
				break;
				}
			sServerIp = stCCM[i].sServerIP4;

			if (0 == FindClientSideIP( i ))
				{	// find client side connection for 1st connection... probably the same IP for all 
					// client side connections unless more than one NIC
				TRACE( "Could not find a client IP for SysCp - case 0\n" );
				break;
				}

			// FindClientSideIP(i);	// assume syscp found the ip for this client for all other servers. If not
			// craft CODE in FindClientSideIP to find another ip address to link with the database.
			if (stCCM[0].sClientIP4.GetLength() > 6)
						sClientIP = stCCM[0].sClientIP4;	// use case 0 for syscp
					else
						{	// try something else. If that fails, abort since we can't hook up with the data base
						}

#if 0
			// Make a specific child class of CCM to handle the GDP
						pCCM_GDP = (CCCM_GDP *) new CCCM_GDP( i, PHASE_ARRAY_WALL_COMMAND_PROCESSOR );	// ptr to the class, not the connection structure
						if (NULL == pCCM_GDP)
							{
							TRACE1( "pCCM_GDP[%d] is NULL\n", i );
							break;
							}
						if (pCCM_GDP->m_pstCCM == NULL)
							{
							TRACE1( "pCCM_GDP[%d]->m_pstCCM is NULL\n", i );
							break;
							}
#endif

			break;	// case 1


		default:
			pCCM[i] = NULL;
			break;

			}
		}	// for ( i = 0; i < MAX_CLIENTS; i++)


	TRACE1("\nPAG CLIENT CONNECTION MANAGEMENT has completed for MAX_CLIENTS = %d \n", MAX_CLIENTS);
	}

#else
// Phased Array Processor
// PAP has a client connection for NcNx Idata
// and a 2nd client connection for All Wall data -- same NIC, different port

void CPA2WinDlg::InitializeClientConnectionManagement(void)
	{
	int i, j;
	j = 0;

	// 	All these connections are tcp/ip clients and the other end is a tcp/ip server.
	CString sClientIP,  sServerIp, sServerName, s;	// sServerName use the url for this server
	UINT uServerPort = 0;

	for ( i = 0; i < gnMaxClients; i++)
		{
		if (pCCM[i])	continue;	// instance already exists
		switch (i)
			{
		case 0:
			// NcNx data client of NcNx Server
			// I_AM_PAP. I connect to the PAG NcNx server and another All_wall server
			sClientIP = GetClientIP( i );
			sServerIp = GetServerIP( i );
			sServerName = GetServerName( i );
			uServerPort = GetServerPort( i ) & 0xffff;	// port on the PAG server that we will try to connect to
			// Make a specific child class of CCM to handle the Phased Array GUI - PAG
			pCCM_PAG = (CCCM_PAG *) new CCCM_PAG( i );
			pCCM[i] = (CClientConnectionManagement *)pCCM_PAG;
			pCCM[i]->m_nMyConnection = i;
			j = sizeof( CCCM_PAG );
			if (NULL == pCCM_PAG)
				{
				TRACE1( "pCCM_PAG[%d] is NULL\n", i );
				break;
				}
			if (pCCM_PAG->m_pstCCM == NULL)
				{
				TRACE1( "pCCM_PAG[%d]->m_pstCCM is NULL\n", i );
				break;
				}
			//pCCM[i] = pCCM_SysCp; causes problem on shut down

			pCCM_PAG->UniqueProc();	// JUST FOR DEBUG
			pCCM_PAG->SetSocketName( _T( "CCM_PAG" ) );
			//pCCM_PAG->SetWinVersion(theApp.m_iWinVer);
			pCCM_PAG->SetClientIp( sClientIP );
			pCCM_PAG->SetServerIp( sServerIp );
			pCCM_PAG->SetServerName( sServerName );	// url of server, e.g. srvhouapp67
			pCCM_PAG->SetServerPort( uServerPort );


			pCCM_PAG->m_pstCCM->nReceivePriority	= THREAD_PRIORITY_ABOVE_NORMAL;
			pCCM_PAG->m_pstCCM->nSendPriority		= THREAD_PRIORITY_BELOW_NORMAL;
			pCCM_PAG->m_pstCCM->nCmdProcessPriority	= THREAD_PRIORITY_BELOW_NORMAL;
			pCCM_PAG->CreateReceiveThread();		Sleep( 50 );
			pCCM_PAG->CreateSendThread();			Sleep( 50 );
			pCCM_PAG->InitReceiveThread();			Sleep( 50 );
			// causes CClientCommunicationThread::InitTcpThread(WPARAM w, LPARAM lParam) to run
			pCCM_PAG->InitSendThread();				Sleep( 50 );
			pCCM_PAG->CreateCmdProcessThread();		Sleep( 50 );
			while (pCCM_PAG->m_pstCCM->pCmdProcessThread == NULL)
				{
				Sleep(50);
				}
			break;

			// 2018-05-23 Now have an All-Wall channel with different port and or IP Address
			// For testing with PAG it is same IP address and different port. For general case
			// will be separate IP and Port from PAG
		case 1:
			sClientIP = GetClientIP(i);
			sServerIp = GetServerIP(i);
			sServerName = GetServerName(i);
			uServerPort = GetServerPort(i) & 0xffff;	// port on the PAG server that we will try to connect to
														// Make a specific child class of CCM to handle the Phased Array GUI - PAG
			pCCM_PAG_AW = (CCCM_PAG *) new CCCM_PAG(i);
			pCCM[1] = pCCM_PAG_AW;
			j = sizeof(CCCM_PAG);
			if (NULL == pCCM_PAG_AW)
				{
				TRACE1("pCCM_PAG_AW[%d] is NULL\n", i);
				break;
				}
			if (pCCM_PAG_AW->m_pstCCM == NULL)
				{
				TRACE1("pCCM_PAG_AW[%d]->m_pstCCM is NULL\n", i);
				break;
				}
			//pCCM[i] = pCCM_SysCp; causes problem on shut down

			pCCM_PAG_AW->UniqueProc();	// JUST FOR DEBUG
			pCCM_PAG_AW->SetSocketName(_T("CCM_PAG_AW"));
			//pCCM_PAG->SetWinVersion(theApp.m_iWinVer);
			pCCM_PAG_AW->SetClientIp(sClientIP);
			pCCM_PAG_AW->SetServerIp(sServerIp);
			pCCM_PAG_AW->SetServerName(sServerName);	// url of server, e.g. srvhouapp67
			pCCM_PAG_AW->SetServerPort(uServerPort);


			pCCM_PAG_AW->m_pstCCM->nReceivePriority = THREAD_PRIORITY_ABOVE_NORMAL;
			pCCM_PAG_AW->m_pstCCM->nSendPriority = THREAD_PRIORITY_BELOW_NORMAL;
			pCCM_PAG_AW->m_pstCCM->nCmdProcessPriority = THREAD_PRIORITY_BELOW_NORMAL;
			pCCM_PAG_AW->CreateReceiveThread();		Sleep(50);
			pCCM_PAG_AW->CreateSendThread();			Sleep(50);
			pCCM_PAG_AW->InitReceiveThread();			Sleep(50);
			// causes CClientCommunicationThread::InitTcpThread(WPARAM w, LPARAM lParam) to run
			pCCM_PAG_AW->InitSendThread();				Sleep(50);
			pCCM_PAG_AW->CreateCmdProcessThread();		Sleep(50);
			break;

		default:
			pCCM[i] = NULL;

				break;
			}

		}	// for ( i


	TRACE1("\nPAP CLIENT CONNECTION MANAGEMENT has completed for MAX_CLIENTS = %d \n", MAX_CLIENTS);
	}
#endif

// Initialize the Server subsystems of the Phased Array GUI.
// This windows program receives inspection data from the Phased Array Master(s) of which there
// may be several. In one mode of operation, it sends data to a wall bar display program which is
// a client program on another port.
void CPA2WinDlg::InitializeServerConnectionManagement(void)
	{
	int i, j;
	CString s;
	UINT uPort;
	int nError;

	for ( i = 0; i < gnMaxServers; i++)
		{

		if (pSCM[i])	continue;	// instance already exists

		switch (i)
			{
			// There may be multiple Phased Array Processor (PAP) computers connected to PAG
			// #define ePAP_Server			0 - IN ServerConnectionManagement.h
		case ePAP_Server:
			// a server for PAP NcNx data
			pSCM[i] = new CServerConnectionManagement(i);
			j = sizeof(pSCM[i]);
			j = sizeof(CServerConnectionManagement);
			if (pSCM[i])
				{
				s = gServerArray[i].Ip;			// a global static table of ip addresses
				pSCM[i]->SetServerIP4(s);		// if PAP _T("192.168.10.10")); if PAG _T("192.168.11.20")
				uPort = gServerArray[i].uPort;
				pSCM[i]->SetServerPort(uPort);	// 7501);
				pSCM[i]->SetServerType(ePhaseArrayMaster);
				s = gServerArray[i].ClientBaseIp;
				pSCM[i]->SetClientBaseIp(s);
				// m_pstSCM->nListenThreadPriority = THREAD_PRIORITY_NORMAL; in SCM constructor
				// start the listen thread which will create a listener socket
				// the listener socket's OnAccept() function will create the connection thread, dialog and socket
				// the connection socket's OnReceive will populate the input data linked list and post messages
				// to the main dlg/application to process the data.
				pSCM[i]->m_pstSCM->nServerIsListening = 0;

				nError = pSCM[i]->StartListenerThread(i);
				if (nError)
					{
					s.Format(_T("Failed to start listener Thread[%d], ERROR = %d\n"), i, nError);
					TRACE(s);
					delete pSCM[i];
					pSCM[i] = NULL;
					break;
					}

				// wait a while until listening
				for (j = 0; j < 100; j++)
					{
					Sleep(40);
					if (pSCM[i]->m_pstSCM->nServerIsListening)	break;
					}
				if (j < 100)	break;
				else
					{
					s.Format(_T("Listener Thread[%d] timed out, ERROR = %d\n"), i, nError);
					TRACE(s);
					delete pSCM[i];
					pSCM[i] = NULL;
					break;
					}
				}
			break;

#ifdef I_AM_PAG
		case ePAP_AllWall_server:	// PAG function, gets All Wall Data
									// For PAP, becomes command server for Pulser Board.. numerical value is 1
			pSCM[i] = new CServerConnectionManagement(i);
			j = sizeof(pSCM[i]);
			j = sizeof(CServerConnectionManagement);
			if (pSCM[i])
				{
				s = gServerArray[i].Ip;			// a global static table of ip addresses
				pSCM[i]->SetServerIP4(s);		// _T("192.168.10.20"));
				uPort = gServerArray[i].uPort;
				pSCM[i]->SetServerPort(uPort);	// 7520);
				pSCM[i]->SetServerType(ePAP_AllWall_server);
				s = gServerArray[i].ClientBaseIp;
				pSCM[i]->SetClientBaseIp(s);
				// m_pstSCM->nListenThreadPriority = THREAD_PRIORITY_NORMAL; in SCM constructor
				// start the listen thread which will create a listener socket
				// the listener socket's OnAccept() function will create the connection thread, dialog and socket
				// the connection socket's OnReceive will populate the input data linked list and post messages
				// to the main dlg/application to process the data.
				nError = pSCM[i]->StartListenerThread(i);
				if (nError)
					{
					s.Format(_T("Failed to start listener Thread[%d], ERROR = %d\n"), i, nError);
					TRACE(s);
					delete pSCM[i];
					pSCM[i] = NULL;
					}
				}
			break;

#else
			// PAP server for Pulser board
		case ePAP_Pulser_server:		// For PAP, becomes command server for Pulser Board
			Sleep(500);		// hope ADC connects before pulser
			pSCM[i] = new CServerConnectionManagement(i);
			j = sizeof(pSCM[i]);
			j = sizeof(CServerConnectionManagement);
			if (pSCM[i])
				{
				s = gServerArray[i].Ip;			// a global static table of ip addresses
				pSCM[i]->SetServerIP4(s);		// _T("192.168.10.10"));
				uPort = gServerArray[i].uPort;
				pSCM[i]->SetServerPort(uPort);	// 7602);
				pSCM[i]->SetServerType(ePhaseArrayMaster);
				s = gServerArray[i].ClientBaseIp;
				pSCM[i]->SetClientBaseIp(s);
				// m_pstSCM->nListenThreadPriority = THREAD_PRIORITY_NORMAL; in SCM constructor
				// start the listen thread which will create a listener socket
				// the listener socket's OnAccept() function will create the connection thread, dialog and socket
				// the connection socket's OnReceive will populate the input data linked list and post messages
				// to the main dlg/application to process the data.
				nError = pSCM[i]->StartListenerThread(i);
				if (nError)
					{
					s.Format(_T("Failed to start listener Thread[%d], ERROR = %d\n"), i, nError);
					TRACE(s);
					delete pSCM[i];
					pSCM[i] = NULL;
					}
				}
			break;


#endif
		default:
			pSCM[i] = NULL;
			break;
			}
		}
	TRACE1("\nSERVER CONNECTION MANAGEMENT has completed for MAX_SERVERS = %d \n", MAX_SERVERS);
	}


#ifdef I_AM_PAP
void CServerRcvListThread::MakeFakeDataHead(IDATA_FROM_HW *pData)
//void CServerRcvListThread::MakeFakeDataHead(SRawDataPacket *pData)
	{
	//pData->wMsgID	= eRawInsp;	// raw data=10
	//pData->wByteCount = 1064;
//	pData->bDin		= FORWARD | PIPE_PRESENT;
	//pData->wMsgSeqCnt++;
	pData->wLocation = nLoc++;
	if (nLoc > 500) 
		nLoc = 20;
	pData->wAngle	= nLoc % 12;
	pData->wPeriod	= 1465;	// 300 ms = 200 rpm

	}
#endif

// return 0 if unable to resolve IP address of client side
int CPA2WinDlg::FindClientSideIP(int nWhichConnection)
{
	// Since we assume the main dlg knows what servers it will connect with
	// read thru the static stSocketNames and init the larger stCCM structure.
	// if we have input non-null strings for client and server end address, attempt to 
	// use them to create the connections to the servers.

	// for testing to begin with (6-26-12) we will derive the first nic address from 
	// looking up the machine name and get the 2nd (server side ip) from its url.
	// Since we don't have a real syscp, this will fall back to our local Syscp 127.0.0.1

	CString ClientIP = _T("");
	CString nl = _T( "\n" );
	UINT ClientPort = 0;
	CString stmp,s;
	int nReturn = 0;	// assume failure

	 	// get the IP of the test client
    TRACE(_T("Resolving client IP....\n"));
	stmp = GetIPv4();

	// if test client IP was resolved,
	// returned string will not be null
    if ( (stmp.IsEmpty()) || (nWhichConnection >= MAX_CLIENTS) )
		{
		//sockerr = SOCKET_ERROR;
		TRACE(_T("Could not resolve SysCP client IP.\n"));
		return nReturn;
		}
	else 
		{
		s = _T( "Resolved SysCP client IP - " ) + stmp + nl;
		TRACE(s);
		stCCM[nWhichConnection].sClientIP4 = stmp;	// ccm class does not exist yet
			
		nReturn = 1;
		return nReturn;
		}
}
// return 0 if unable to resolve IP address of server side
// Convention is for case 0 to be SysCp connection since all connect to SysCp
// Convention is for case 1 to be connection to the GDP
// Convention is for case 2 to be connetion to the database
// Convention is for case 3 to be connection to the PLC
//
// stCCM[] should have been filled in by registry reads occurring before this function was called.
//
int CPA2WinDlg::FindServerSideIP(int nWhichConnection)
	{
	// Since we assume the main dlg knows what servers it will connect with
	// read thru the static stSocketNames and init the larger stCCM structure.
	// if we have input non-null strings for client and server end address, attempt to 
	// use them to create the connections to the servers.

	// for testing to begin with (6-26-12) we will derive the first nic address from 
	// looking up the machine name and get the 2nd (server side ip) from its url.
	// Since we don't have a real syscp, this will fall back to our local Syscp 127.0.0.1

	CString ServerIP = _T("");
	CString sClientIP,  sServerIp, sServerName, s;	// sServerName use the url for this server

	CString stmp;
	int nReturn = 0;	// assume failure

	if ( nWhichConnection >= MAX_CLIENTS )	return nReturn;

	stmp.Empty();
	sServerName	= stCCM[nWhichConnection].sServerName;
	sServerIp	= stCCM[nWhichConnection].sServerIP4;

	switch (nWhichConnection)
		{

	case 0:		// SysCp by convention

		TRACE(_T("Resolving SysCP server IP....\n"));
		if (!sServerName.IsEmpty())
			{
			stmp = GetIPv4(sServerName);			// sometimes (MC_SCP_HOSTNAME);
			}
		if (stmp.IsEmpty())
			{
			stmp = sServerIp;
			}

		// stmp = GetIPv4(MC_SCP_HOSTNAME);

		// if first attempt failed
		// try LOCAL_HOST which should
		// always return 127.0.0.1 .. add localhost to file C:\Windows\System32\drivers\etc\hosts
		// # localhost name resolution is handled within DNS itself.
		// #	127.0.0.1       localhost
		// #	::1             localhost

		if (stmp.IsEmpty())
			{
			stmp = GetIPv4(LOCAL_HOST);
			}

	// if server IP was resolved,
	// returned string will not be NULL
    if (stmp.IsEmpty())
		{	TRACE(_T("Could not resolve SysCP server IP."));		return nReturn;		} 
	else 
		{    
		TRACE(_T("Resolved SysCP server IP - \n") + stmp);
		stCCM[nWhichConnection].sServerIP4 = stmp;
		return (nReturn = 1);
		}

	case 1:		// CASE 1 is now GDP

		TRACE(_T("Resolving GDP server IP....\n"));
 		if (!sServerName.IsEmpty())
			{
			stmp = GetIPv4(sServerName);			
			}
		if (stmp.IsEmpty())
			{
			stmp = sServerIp;
			}		
		//stmp = GetIPv4(MC_DBS_HOSTNAME);
		if (stmp.IsEmpty())
			{	TRACE(_T("Could not resolve GDP server IP.\n"));		return nReturn;		} 
		else 
			{    
			TRACE(_T("Resolved GDP server IP - ") + stmp);
			stCCM[nWhichConnection].sServerIP4 = stmp;
			return (nReturn = 1);
			}

	case 2:
		// database by convention MC_DBS_HOSTNAME 

	default:
		stmp.Format(_T("Failed to Resolve server IP for connection %d"),nWhichConnection);
		TRACE(stmp);
		stmp += _T("\n");
		TRACE(stmp);
		break;
		}	// end switch ()
	return nReturn;
	}

CString CPA2WinDlg::GetIPv4(void)
{
	// use this version of GetIPv4 when
	// you want to find out what the
	// IP is for the machine THIS
	// application is running on

	USES_CONVERSION;
	CString s;

	TCHAR sFQDN [MAX_FQDN_LENGTH + 1];		// fully qualified domain name string
	DWORD dwFQDN = MAX_FQDN_LENGTH + 1;		// buffer for sFQDN

	// because we are running a system using DHCP, DNS,
	// and dynamic IP addresses, we have no prior knowledge
	// about any given host's IP address so we have to
	// fish for it

	// find out where we are
	CString sComputerName = _T("");		// FQDN of this host
	CString theIP = _T("");				// the stringized ip

    ::GetComputerNameEx(ComputerNameDnsFullyQualified,
		                (LPTSTR) sFQDN, 
		                &dwFQDN);

	sComputerName = W2A(sFQDN);


	// make sure we're searching for something real
    if (sComputerName.IsEmpty())
		return theIP;

    struct sockaddr_in theHost;	// structure used in the conversion
	struct in_addr addr;		// jeh from help system
	HOSTENT * hostent;			// structure returned by gethostbyname
	int i = 0;

    // do the lookup
	hostent = gethostbyname(W2A(sComputerName));
	// Note  The gethostbyname function has been deprecated by the introduction of the getaddrinfo function. 
	// Developers creating Windows Sockets 2 applications are urged to use the getaddrinfo function instead of gethostbyname. 
	// getaddrinfo() is preferred way to get this info... but realy complex. Use help to find getaddrinfo
	// Developers are encouraged to use the GetAddrInfoW Unicode function rather than the getaddrinfo ANSI function.

	// make sure we found something
	if (hostent != NULL)
	{
	// jeh code to show all hosts.. from help system for hostent structure
	if (hostent->h_addrtype == AF_INET)
		{
		while (hostent->h_addr_list[i] != 0)
			{
			addr.s_addr = *(u_long *) hostent->h_addr_list[i++];
			s = inet_ntoa(addr);
			TRACE1("Host IP4 addr = %s\n",s);
			}
		}

	   // save the important bits
	   // note, if the host is a multi-homed
	   // then hostent->h_address_list WILL CONTAIN
	   // MORE THAN ONE IP. this method currently
	   // assumes that the host is NOT multi-homed
       memcpy(&theHost.sin_addr, 
		      hostent->h_addr_list[0], 
			  hostent->h_length);

	   // convert to IP address
	   theIP = inet_ntoa((IN_ADDR) theHost.sin_addr);
	} else {
	   theIP.Empty();
	}

	// return something to the caller
	// either a null string or the
	// actual IP
	return theIP;
}

CString CPA2WinDlg::GetIPv4(CString sComputerName)
{
	// use this version of GetIPv4 when
	// you want to determine the IP
	// of a machine OTHER than the
	// one this application is running
	// on.  example GetIPv4(_T("mc-dbs"))
	// will return the IP of the computer
	// named "mc-dbs"
	USES_CONVERSION;

	// because we are running a system using DHCP, DNS,
	// and dynamic IP addresses, we have no prior knowledge
	// about any given host's IP address so we have to
	// fish for it

	CString theIP = _T("");				// the stringized ip
    CString stmp = _T("");				// temporary string holder
	stmp = W2A(sComputerName);

	// make sure we're searching for something real
    if (stmp.IsEmpty())
		return theIP;		// theIP is an empty string here

    struct sockaddr_in theHost;	// structure used in the conversion
	HOSTENT * hostent;			// structure returned by gethostbyname

    // do the lookup
	hostent = gethostbyname(W2A(stmp));

	// make sure we found something
	if (hostent != NULL)
	{
	   // save the important bits
	   // note, if the host is a multi-homed
	   // then hostent->h_address_list WILL CONTAIN
	   // MORE THAN ONE IP. this method currently
	   // assumes that the host is NOT multi-homed
       memcpy(&theHost.sin_addr, 
		      hostent->h_addr_list[0], 
			  hostent->h_length);

	   // convert to IP address
	   theIP = inet_ntoa((IN_ADDR) theHost.sin_addr);
	} else {
	   theIP.Empty();
	} //if

	// return something to the caller,
	// either a null string or the
	// actual IP
	return theIP;
}





void CPA2WinDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPA2WinDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPA2WinDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPA2WinDlg::OnBnClickedOk()
	{
	// TODO: Add your control notification handler code here
	StopTimer();
	OnBnClickedBnShutdown();
	CDialogEx::OnOK();
	}

void CPA2WinDlg::OnBnClickedCancel()
	{
	// TODO: Add your control notification handler code here
	StopTimer();
	OnBnClickedBnShutdown();
	CDialogEx::OnCancel();
	}

void CPA2WinDlg::OnBnClickedBnShutdown()
	{
	// TODO: Add your control notification handler code here
	// Kill all threads but don't exit the program
	// Used to see if we can delete all objects created with new when the program runs.
	int i = 0;
	ST_SERVER_CONNECTION_MANAGEMENT *pstSCM = 0;
	CString s;

	bAppIsClosing = nShutDown = 1;
	AfxGetThread()->SetThreadPriority( THREAD_PRIORITY_BELOW_NORMAL );

	// KIll the test thread
	if (m_pTestThread)
		{
		::SetEvent(g_hTimerTick);
		Sleep(250);
		Sleep(250);
		}

	DestroyCCM();
	DestroySCM();
		
	CloseFakeData();
	CloseDebugLog();
	m_nFakeDataExists = m_nDebugLogExists = 0;
	Sleep( 100 );
	if (pCSSaveDebug)
		delete pCSSaveDebug;
	pCSSaveDebug = 0;

	}

void CPA2WinDlg::OnFileExit()
	{
	// TODO: Add your command handler code here
	CDialogEx::OnOK();
	}


// When it is time to create the thread that services the client's data received into the RcvPacketList
// Call back to the top level of the application and choose the correct thread type base on knowing 
// the function of the server itself.
// This call returns the thread ptr of the thread created.
// The thread created will have custom processing of the RcvPacketList based on which server/client pair
// is being processed. For Phased Array Master, the primary client on the other end is the PA Instrument.
// It sends inspection data packets which are processed in a manner dictated by the Message ID of the packet.
//
CServerRcvListThread* CPA2WinDlg::CreateServerReceiverThread(int nServerNumber, int nPriority)

	{
	CString s;
	CServerRcvListThread *pThread = NULL;

	if (nServerNumber >= MAX_SERVERS)
		{
		s.Format(_T("CPA2WinDlg::CreateReceiverThread invalid server index = %d\n"), nServerNumber);
		TRACE(s);
		return pThread;	// NULL is invalid for a thread ptr
		}
	switch (nServerNumber)
		{
		// AfxBeginThread(RUNTIME_CLASS (), priority, stack_size, suspended-or-run, security)
	case 0:
	case 1:	//special case for all wall - using two different servers for essentially the same input data
			// server[0] get NcNx data, server[1] gets all wall data -- the same data stream from the adc
		// Case 0 is instrument data serviced by a custom thread class named CServerRcvListThread.
		// Other types of service offered to other types of clients would likely have a different
		// custom thread to process the data which came from the client.
		pThread = 	
				(CServerRcvListThread *) AfxBeginThread(
					RUNTIME_CLASS (CServerRcvListThread),
					nPriority,	0, CREATE_SUSPENDED, NULL);		// nromally THREAD_PRIORITY_NORMAL
		break;
	default:

		break;
		}
	return pThread;
	}

#define DEBUGIT

/********  Debug File *********/
void CPA2WinDlg::SaveDebugLog(CString& s)
	{
#ifdef DEBUGIT
	char ch[4000];
	if (0 == m_nDebugLogExists)
		{
		TRACE(_T("Debug log file not available\n"));
		return;
		}

	if ( m_DebugLog.m_hFile > 0)
		{
	EnterCriticalSection(pCSSaveDebug);
	try
		{
		CstringToChar(s, ch, 4000);
		m_DebugLog.Write(ch,(int)strlen(ch));	// I want to see ASCII in the file
		//m_DebugLog.Flush(); -- not needed. Kills Nc Nx processing time
		}
	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}
	LeaveCriticalSection(pCSSaveDebug);
		}
#endif
	}

void CPA2WinDlg::CloseDebugLog(void)
	{
	if (0 == m_nDebugLogExists)
		{
		TRACE(_T("Debug log file not available\n"));
		return;
		}
	try
		{
		m_DebugLog.Close();
		}
	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}
	m_nDebugLogExists = 0;
	}

/********  Command File *********/

void CPA2WinDlg::SaveCommandLog(CString& s)
	{
#if 1
	char ch[4000];
	if (0 == m_mCommandLogExists)
		{
		TRACE(_T("Command log file not available\n"));
		return;
		}

	if (m_CommandLog.m_hFile > 0)
		{
		EnterCriticalSection(pCSSaveCommands);
		try
			{
			CstringToChar(s, ch, 4000);
			m_CommandLog.Write(ch, (int)strlen(ch));	// I want to see ASCII in the file
			//m_DebugLog.Flush(); -- not needed. Kills Nc Nx processing time
			}
		catch (CFileException* e)
			{
			e->ReportError();
			e->Delete();
			}
		LeaveCriticalSection(pCSSaveCommands);
		}
#endif
	}

void CPA2WinDlg::CloseCommandLog(void)
	{
	if (0 == m_mCommandLogExists)
		{
		TRACE(_T("Command log file not available\n"));
		return;
		}
	try
		{
		m_CommandLog.Close();
		}
	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}
	m_mCommandLogExists = 0;
	}


/********  Fake Data File *********/

void CPA2WinDlg::SaveFakeData(CString& s)
	{
#ifdef DEBUGIT
	char ch[4000];
	if (0 == m_nFakeDataExists)
		{
		TRACE(_T("Fake data file not available\n"));
		return;
		}
	try
		{
		CstringToChar(s, ch, 4000);
		m_FakeData.Write(ch,(int)strlen(ch));	// I want to see ASCII in the file
		//m_FakeData.Flush();
		}
	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}
#endif

	}

void CPA2WinDlg::CloseFakeData(void)
	{
	if (0 == m_nFakeDataExists)
		{
		TRACE(_T("Fake data file not available\n"));
		return;
		}
	try
		{
		m_FakeData.Close();
		}
	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}
	m_nFakeDataExists = 0;
	}

/********  ReadBack File *********/
void CPA2WinDlg::SaveReadBackLog(CString& s)
	{
#ifdef DEBUGIT
	char ch[16000];
	if (0 == m_nReadBackExists)
		{
		TRACE(_T("Cmd Read Back log file not available\n"));
		return;
		}

	if (m_ReadBackLog.m_hFile > 0)
		{
		EnterCriticalSection(pCSSaveReadBack);
		try
			{
			CstringToChar(s, ch, 8000);
			m_ReadBackLog.Write(ch, (int)strlen(ch));	// I want to see ASCII in the file
			//m_DebugLog.Flush(); -- not needed. Kills Nc Nx processing time
			}
		catch (CFileException* e)
			{
			e->ReportError();
			e->Delete();
			}
		LeaveCriticalSection(pCSSaveReadBack);
		}
#endif
	}

void CPA2WinDlg::CloseReadBackLog(void)
	{
	if (0 == m_nReadBackExists)
		{
		TRACE(_T("Read Back log file not available\n"));
		return;
		}
	try
		{
		m_ReadBackLog.Close();
		}
	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}
	m_nReadBackExists = 0;
	}



// This timer only runs the PA2WinDlg screen.
// Timed connection attempts to PAG if this is PAP are done by pings from the TestThread
// TestThread calls 
void CPA2WinDlg::OnTimer( UINT_PTR nIDEvent )
	{
	// TODO: Add your message handler code here and/or call default
	WORD wVerH, wVerS;	// hardware/software version temps
	m_nTimerCount++;
	time(&m_tTimeNow);
	UpdateTimeDate( &m_tTimeNow );

	// update ascan for scope output on NcNx screen if it is open
	if (gDlg.pNcNx)
		{
		gDlg.pNcNx->IncrementAscanCnt();
		gDlg.pNcNx->ShowLastCmdSeq();
		gDlg.pNcNx->ShowIdataSource();
		gDlg.pNcNx->ShowSmallCmds();
		gDlg.pNcNx->ShowLargeCmds();
		gDlg.pNcNx->ShowPulserCmds();
		gDlg.pNcNx->ShowSmallQ();
		gDlg.pNcNx->ShowLargeQ();
		gDlg.pNcNx->ShowPulserQ();
		wVerH = wVerS = 0;	// kill warning in PAG
		}

#ifdef I_AM_PAP
	if (gDlg.pNcNx)	return;		// don't show when NcNx dialog on screen
	// update last Idata packet to list box.
	// All this to save processing time in displaying data on screen
	// Do it once so no extra time for conversions of numbers to text
	//m_sHwVerAdc = m_sSwVerAdc = m_sHwVerPulser = m_sSwVerPulse
//	if ((gLastAscanPap.wFPGA_VersionA != 0) && (m_sHwVerAdc.GetLength() == 0))
		{
		wVerH = gLastAscanPap.wFPGA_VersionA;
		wVerS = gLastAscanPap.wNIOS_VersionA;
		m_sHwVerAdc.Format(_T("[%01d.%01d.%03d"), MAJVER(wVerH), MINVER(wVerH), BLDVER(wVerH));
		m_sSwVerAdc.Format(_T("    %01d.%01d.%03d"), MAJVER(wVerS), MINVER(wVerS), BLDVER(wVerS));
		//return;
		}
	
//	if ((gwFPGA_VersionP != 0) && (m_sHwVerPulser.GetLength() == 0))
		{
		wVerH = gwFPGA_VersionP;
		wVerS = gwNIOS_VersionP;
		m_sHwVerPulser.Format(_T("		          [%01d.%01d.%03d "), MAJVER(wVerH), MINVER(wVerH), BLDVER(wVerH));
		m_sSwVerPulser.Format(_T("   %01d.%01d.%03d"), MAJVER(wVerS), MINVER(wVerS), BLDVER(wVerS));
		//return;
		}
	// grab hw/sw version numbers from packet data
	ShowIdata();

#endif
	CDialogEx::OnTimer( nIDEvent );
	}

bool CPA2WinDlg::UpdateTimeDate(time_t *tNow)
	{
	struct tm *today;
	CString t;
	//int i;	//,j;
	//WORD wMask;

//	time(tNow);

	/* Convert to time structure  */
	today = localtime( tNow );
	if (today ==0) return TRUE;
		{
		t.Format(_T("%02d:%02d:%02d"), today->tm_hour, today->tm_min, today->tm_sec);
		SetDlgItemText(IDC_STAT_TIME, t);
		t.Format(_T("%02d/%02d/%02d"), today->tm_mon+1, today->tm_mday, (today->tm_year % 100) );
		SetDlgItemText(IDC_STAT_DATE, t);
		}

	// when the text is drawn, onctlcolor intercepts before the draw and picks the color
    return FALSE;
	}

#ifdef I_AM_PAG

// Deconstruct/destroy all created for Client Connection Management system
void CPA2WinDlg::DestroyCCM(void)
	{
	int nError, j;
	CString s;
	nError = j = 0;
	}
#endif


#ifdef I_AM_PAP

// Deconstruct/destroy all created for Client Connection Management system
void CPA2WinDlg::DestroyCCM( void )
	{
	int i, nError, j, iClient;
	CString s;
	nError = j = 0;
	
	for (iClient = 0; iClient < gnMaxClients; iClient++)
		{
		// every client connetion to an external server will likely have different characteristics
		// Thus each connection will involve different shut down operations
		// Assume the first connection is to the test GUI aka PAG

		switch (iClient)
			{
		case 0:
			// PAP does connect to the GUI - PAG.
			// It is a special child case of the general CCM
		if (pCCM_PAG)
			{
			// close down the connection to the	PAG BY 
			// closing the connected socket, deleteing critical sections and lists
			// and deleting any other supporting threads.
			// delete the client socket, then the cmd process thread, then the send thread,
			// then the receive thead
			// maybe have receive thread kill socket since it owns it.
			if (pCCM_PAG->m_pstCCM)
				{
				if (pCCM_PAG->m_pstCCM->pSocket)
					{	// socket exists
					pCCM_PAG->KillSocket();
					Sleep(10);
					}	
				}	// socket exists

			// wait for a while for the socket to be destroyed
			i = 0;
			if (pCCM_PAG->m_pstCCM->pSocket)
				{
				while ((i < 100) && (pCCM_PAG->m_pstCCM->pSocket->m_hSocket != 0xffffffff))
					{
					Sleep(10);		i++;
					}
				}
			if ( i >= 100) TRACE("CCM - Failed to kill Client socket");
			Sleep( 20 );
			pCCM_PAG->KillReceiveThread();
			Sleep( 20 );
			i = 0;
			while (( i < 50 ) && ( pCCM_PAG->m_pstCCM->pReceiveThread != 0) )
				{	Sleep (10);		i++;	}
			if ( i >= 50) TRACE("CCM - Failed to kill Receive Thread");
			
			pCCM_PAG->KillSendThread();
			Sleep( 20 );
			i = 0;
			while (( i < 50 ) && ( pCCM_PAG->m_pstCCM->pSendThread != 0) )
				{	Sleep (10);		i++;	}
			if ( i >= 50) TRACE("CCM - Failed to kill Send Thread");

			pCCM_PAG->KillCmdProcessThread();
			Sleep( 20 );
			i = 0;
				while (( i < 50 ) && ( pCCM_PAG->m_pstCCM->pCmdProcessThread != 0) )
					{	Sleep (10);		i++;	}
				if ( i >= 50) TRACE("CCM - Failed to kill CmdProcess Thread");
			
			i = pCCM_PAG->m_nMyConnection;
			delete pCCM_PAG;	
			pCCM_PAG = NULL;
			pCCM[iClient] = NULL;	// MAYBE NOT A GOOD IDEA TO HAVE TWO POINTERS TO THE SAME CLASS INSTANCE
			break;
			}	// if (pCCM_PAG)
			// delete send thread and critical sections, delete receive thread and critical sections
			break;

		case 1:
		

			// PAP does connect to the GUI - PAG.
			// It is a special child case of the general CCM
			if (pCCM_PAG_AW)
				{
				// close down the connection to the	PAG BY 
				// closing the connected socket, deleteing critical sections and lists
				// and deleting any other supporting threads.
				// delete the client socket, then the cmd process thread, then the send thread,
				// then the receive thead
				// maybe have receive thread kill socket since it owns it.
				if (pCCM_PAG_AW->m_pstCCM)
					{
					if (pCCM_PAG_AW->m_pstCCM->pSocket)
						{	// socket exists
						pCCM_PAG_AW->KillSocket();
						}
					}	// socket exists

						// wait for a while for the socket to be destroyed
				i = 0;
				while ((i < 100) && (pCCM_PAG_AW->m_pstCCM->pSocket != 0))
					{
					Sleep(10);		i++;
					}
				if (i >= 100) TRACE("CCM - Failed to kill Client socket");
				Sleep(20);
				pCCM_PAG_AW->KillReceiveThread();
				Sleep(20);
				i = 0;
				while ((i < 50) && (pCCM_PAG_AW->m_pstCCM->pReceiveThread != 0))
					{
					Sleep(10);		i++;
					}
				if (i >= 50) TRACE("CCM - Failed to kill Receive Thread");

				pCCM_PAG_AW->KillSendThread();
				Sleep(20);
				i = 0;
				while ((i < 50) && (pCCM_PAG_AW->m_pstCCM->pSendThread != 0))
					{
					Sleep(10);		i++;
					}
				if (i >= 50) TRACE("CCM - Failed to kill Send Thread");

				pCCM_PAG_AW->KillCmdProcessThread();
				Sleep(20);
				i = 0;
				while ((i < 50) && (pCCM_PAG_AW->m_pstCCM->pCmdProcessThread != 0))
					{
					Sleep(10);		i++;
					}
				if (i >= 50) TRACE("CCM - Failed to kill CmdProcess Thread");

				i = pCCM_PAG_AW->m_nMyConnection;
				delete pCCM_PAG_AW;
				pCCM_PAG_AW = NULL;
				pCCM[iClient] = NULL;	// MAYBE NOT A GOOD IDEA TO HAVE TWO POINTERS TO THE SAME CLASS INSTANCE
				break;
				}	// if (pCCM_PAG_AW)
					// delete send thread and critical sections, delete receive thread and critical sections


		default:
			//for ( i = 0; i < MAX_CLIENTS; i++)
				{
				if (pCCM[iClient])	delete pCCM[iClient];	// in 2016 there is only pCCM_PAG
				pCCM[iClient] = NULL;
				//Sleep(10);
				}
			i = 14;
			s = _T("Here we are trying to close CCM stuff\n");
			TRACE(s);
			break;

			}	 // switch (iClient)

		}	// for (iClient = 0; iClient < gnMaxClients; iClient++)

	}	// DestroyCCM( void )
#endif


// Deconstruct/destroy all created for Server Connection Management system
// called from the destructor of CPA2WinDlg
void CPA2WinDlg::DestroySCM( void )
	{
	int i;		// , j, k;
	ST_SERVER_CONNECTION_MANAGEMENT *pstSCM;
	CString s;
	// lower thread priority to allow signaled thread chance to exit
	AfxGetThread()->SetThreadPriority( THREAD_PRIORITY_BELOW_NORMAL );

	for (i = 0; i < gnMaxServers; i++)
		{
		if ( pSCM[i])
			{
			pstSCM = pSCM[i]->m_pstSCM;
			pSCM[i]->ServerShutDown( i );
			delete pSCM[i];
			}
		pSCM[i] = 0;
		}	// for (i = 0; i < gnMaxServers; i++)
	}



//Only open this dialog if on the PAG machine
//
void CPA2WinDlg::OnConfigureNcNx()
	{
#ifdef I_AM_PAG
	// TODO: Add your command handler code here
	if (gDlg.pNcNx == NULL)
		{
		gDlg.pNcNx = new CNcNx;
		if (gDlg.pNcNx)
			{
			gDlg.pNcNx->Create(IDD_NCNX_PA);
			}
		}
	else gDlg.pNcNx->SetFocus();
#else
	MessageBox( _T( "NcNx Dialog only available from Phased Array GUI" ), _T( "eError" ), MB_OK );
#endif
	}

void CPA2WinDlg::SaveMyWindowPosition()
	{
    WINDOWPLACEMENT wp;

    // Save closing location of window
    GetWindowPlacement(&wp);
    SaveWindowLastPosition(_T("PA2Win"), &wp);
	}


void CPA2WinDlg::GetMyWindowPosition()
	{
    WINDOWPLACEMENT wp;
    RECT rect;
    int dx, dy;     // width and height of original window
    
    GetWindowPlacement(&wp);
    dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
    dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
    GetWindowLastPosition(_T("PA2Win"), &rect);

    if ( ( (rect.right - rect.left) >= dx ) &&
         ( (rect.bottom - rect.top) >= dy ))
    {
        wp.rcNormalPosition = rect;
        //SetWindowPlacement(&wp);
    }

	if ( (rect.left < 2048) && (rect.left >= 0) && (rect.top < 500) && (rect.top >= 0))
		SetWindowPos (NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	else
		SetWindowPos (NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

void CPA2WinDlg::SaveWindowLastPosition(LPCTSTR lpszEntry, WINDOWPLACEMENT *wp)
	{
	// Input is name of dialog to save and last position of window
	CString s;	// string showing top, bottom, left & right
	RECT *rect;

	rect = (RECT *) &wp->rcNormalPosition;
	s.Format(_T("%4d,%4d,%4d,%4d"), rect->top, rect->bottom,
				rect->left, rect->right);
	m_ptheApp->WriteProfileString(( CURRENT_VERSION),
							lpszEntry, s);
	}

// Globally available method to get last position of window from registry
void CPA2WinDlg::GetWindowLastPosition(LPCTSTR lpszEntry, RECT *rect)
	{
	CString s;
	char t[60];

	s = m_ptheApp->GetProfileString(( CURRENT_VERSION),
							lpszEntry,_T(""));
	CstringToChar(s,t);
	sscanf(t, "%d,%d,%d,%d", &rect->top, &rect->bottom, 
			&rect->left, &rect->right);

	}

// gather size of the structures used in this program
void CPA2WinDlg::StructSizes( void )
	{
	int i;
	CString s;
	i = sizeof(CCmdFifo);	// 16544
	s.Format( _T( "sizeof(CCmdFifo) %d" ), i );
	DlgDebugOut( s );
	i = sizeof(PULSER_DATA);			// 64
	i = sizeof(CRITICAL_SECTION);		// 24
	i = sizeof(CPtrList);					// 28
	i = sizeof(CClientConnectionManagement);//16
	i = sizeof(CClientCommunicationThread);//168
	i = sizeof(CClientSocket);				//36
	i = sizeof(CCmdProcessThread);			//76
	i = sizeof(CCCM_PAG);					//32
	i = sizeof(CHwTimer);	// 496
	i = sizeof( CIniFile );	// 12
	i = sizeof( CInspState ); // 12
	i = sizeof(CNcNx);	// 2800
	i = sizeof(CServerConnectionManagement);	// 12
	i = sizeof(CServerListenThread);	// 80
	i = sizeof(CServerRcvListThread);	// 220
	i = sizeof(CServerSocket);	// 4276
	i = sizeof(CServerSocketOwnerThread);	// 108
	i = sizeof(CvChannel);	// 160
	i = sizeof(CTestThread); // 72
	i = sizeof(CTuboIni); // 12
	i = sizeof( ST_SERVERS_CLIENT_CONNECTION ); // 640
	i = sizeof( ST_SERVER_CONNECTION_MANAGEMENT ); // 160
	i = sizeof( ST_CLIENT_CONNECTION_MANAGEMENT ); // 172
	i = sizeof( CPA2WinApp );	// 204
	i = sizeof( CPA2WinDlg );	// 720
	i = sizeof( Nc_FIFO );	// 24 but 3 copies
	i = sizeof( Nx_FIFO );	// 52
	i = sizeof( PAP_INST_CHNL_NCNX );	// 1056
	i = sizeof( CIniSectionA );	// 44
	i = sizeof( CIniKeyA );	// 60
	i = sizeof( CIniSectionW );	// 44
	i = sizeof( CIniKeyW );	// 60
	i = sizeof( CShellManager );	// 12
	i = sizeof( IDATA_PAP );	// 1088
//	i = sizeof( InputRawDataPacket );	// 944 ->272 replaced by IDATA_FROM_HW
//	i = sizeof( stRawSeqPacket );	// 130 ->34 
	i = sizeof( IDATA_FROM_HW );	// 1088
	i = sizeof(IDATA_FROM_HW_HDR);
	i = sizeof( ASCAN_DATA_HDR );	// 64
	i = sizeof(SEQ_DATA);	// 32
	i = sizeof(ASCAN_DATA);	// 1088
	i = sizeof(ST_CHNL_GAIN_DELAY_CMD);	// 32
	i = sizeof(READBACK_DATA);	//1088
	i = sizeof(ST_TCG_BEAM_GAIN_READBACK_DATA);	// 3122
	i = sizeof(gLastBeamGainReadBack.Seq[0]);	// 1040
	i = sizeof(CMD204H_READBACK);	// 1058
	i = sizeof(CMD205H_READBACK); // 792
	i = sizeof(ST_TCG_SEQ_GAIN_READBACK_DATA); // 774
	i = sizeof(ST_SEQ_TCG_GAIN); //1056

	}

// Display string s on main dlg list box
void CPA2WinDlg::DlgDebugOut( CString s )
	{
	m_lbOutput.AddString( s );
	}


int CPA2WinDlg::GetAdcCmdQ(void)		// return number of commands queued for ADC
	{
	// ADC command q serviced by SRV[0]
	int i = 0;
	if (stSCM[0].pClientConnection[0]->pSendPktList)
		{
		i = stSCM[0].pClientConnection[0]->pSendPktList->GetCount();
		}
	return i;
	}


int CPA2WinDlg::GetPulserCmdQ(void)
	{
	// pulser command q services by SRV[1]
	int i = 0;
	if (stSCM[1].pClientConnection[0])
		{
		if (stSCM[1].pClientConnection[0]->pSendPktList)
			{
			i = stSCM[1].pClientConnection[0]->pSendPktList->GetCount();
			}
		}
	return i;
	}


// Show Idata on Pa2win dlg when running as PAP
// CALLED FROM 1 SECOND TIMER
void CPA2WinDlg::ShowIdata(void)
	{
#ifdef I_AM_PAP
	CString s,t;
	int i,j,mn, mx;
	int hd, pp, ie;
	UINT uGood, uLost;
	if (gLastIdataPap.wMsgID == eNcNxInspID)
		{
#ifdef SHOW_CH0_ALL_SEQ
			s.Format(_T("Idata-to-PT WMin=%d, G1=%d, G2=%d, Raw[0][0] = %4d,  Raw[1][0] = %4d, Raw[2][0] = %4d, MsgSeqCnt= %d, ZeroCnt = %3d, Not 0 = %d"),
				gLastIdataPap.PeakChnl[0].wTofMin,
				gLastIdataPap.PeakChnl[0].bId2, gLastIdataPap.PeakChnl[0].bOd3,
				gwMin0, gwMin1_0, gwMin2_0, m_nMsgSeqCnt, gwZeroCnt, gwNot0);

			m_lbOutput.AddString(s);
			//gwMin0 = 0xffff;
			gwMax0 = gwZeroCnt = gwNot0 = 0;
			m_nMsgSeqCnt = gwMsgSeqCnt;
#else


		// Check the command queues for the ADC and Pulser. If any commands are waiting
		// exit this routine
		if (i = GetAdcCmdQ()) // apparently only adc has much in queue
			{
			s.Format(_T(" *** BUSY SENDING COMMANDS. ADC cmd Q = %04d *** Small Rcvd = %04d, Large Rcvd =%04d  ***"), 
				i, gLastAscanPap.wSmallCmds, gLastAscanPap.wLargeCmds);
			m_lbOutput.AddString(s);
			return;
			}
		if (i = GetPulserCmdQ())
			{
			s.Format(_T(" *** BUSY SENDING COMMANDS. Pulser cmd Q = %d ***"), i);
			m_lbOutput.AddString(s);
			return;
			}


		// Generate packet rate here but output at end of list box
		for (i = 0; i < 2; i++)
			{
			if (gPksPerSec[i].nTrigger)
				{
				uLost = uGood = 0;
				if (pCCM[i])
					{
					if (pCCM[i]->m_pstCCM)
						{
						if (i == 1)
							uLost = 0;
						uLost = pCCM[i]->m_pstCCM->uLostSentPackets;
						uGood = pCCM[i]->m_pstCCM->uPacketsSent;
#if 0
						fAvgAttempts = 0.0;	// this really did reveal much info
						// better to show number of attempts
						// compute avg number of xmit to get packet sent
						for (j = 0; j < 10; j++)
							{
							fAvgAttempts += float(guPktAttempts[i][j]);
							}
						if (uGood) 
							fAvgAttempts = (fAvgAttempts + (float) (uLost*6))/ (float)uGood;
#endif
						}
					}
				gPksPerSec[i].nTrigger = 0;
				m_sPktRate[i].Format(
				_T("[%08d]Server[%d]Socket[%d]::ShowIdata - [SeqCnt=%05d] Packets/sec = %6.1f  Good = %06d Lost =%04d  Attempts[ %d,%d,%d,%d,%d,%d]\n"),
					gPksPerSec[i].uPktsSent, i, gPksPerSec[i].nClientIndx,
					gPksPerSec[i].wMsgSeqCnt, gPksPerSec[i].fPksPerSec, uGood, uLost, 
					guPktAttempts[i][0], guPktAttempts[i][1], guPktAttempts[i][2], guPktAttempts[i][3], guPktAttempts[i][4], guPktAttempts[i][5] );
				TRACE(m_sPktRate[i]);
				pMainDlg->SaveDebugLog(m_sPktRate[i]);
				}
			}


		// Show the current flaw gate values for all channels going to PT. erase first so no scrolling
		m_lbOutput.ResetContent();
		t = _T("");
		for (i = 0; i < 8; i++)	// label chnl on top line
			{
			s.Format(_T("Ch%d                   "), i);
			t += s;
			}

		m_lbOutput.AddString(t);
			
		for (j = 0; j < 3; j++)
			{
			t = _T("");
			for (i = 0; i < 8; i++)
				{
				mn = gLastIdataPap.PeakChnl[j * 8 + i].wTofMin;
				if (mn > 999) mn = 999;
				mx = gLastIdataPap.PeakChnl[j * 8 + i].wTofMax;
				s.Format(_T("Min=%04d  Max=%04d    "), 
					mn, mx );
				t += s;
				}
			m_lbOutput.AddString(t);
			}
#endif
		// find the first sequence in the data
		for (j = 0; j < gLastAllWall.bSeqModulo; j++)
			{
			if (((j + gLastAllWall.bStartSeqNumber) % gLastAllWall.bSeqModulo) == 0)
				{
				break;	// j is the seq index for hw seq 0
				}
			}
			
		s = t = _T( "" );	// blank line
		m_lbOutput.AddString(s);
		s = _T("All Wall 1st 8 beams");
		m_lbOutput.AddString(s);
		for (i = 0; i < 8; i++)
			{
			mn = gLastAllWall.Seq[j].vChnl[i].wTof;
			if (mn > 999) mn = 999;

			s.Format(_T("AllWall[0][%d]=%04d    "), i,mn);
			t += s;
			}
		m_lbOutput.AddString(t);
		s = _T("");
		m_lbOutput.AddString(s);

		hd = pp = ie = 0;
		if (gLastIdataPap.bDin & PP_SAM) pp = 1;
		if (gLastIdataPap.bDin & IE_SAM) ie = 1;
		if (gLastIdataPap.bDin & HD_SAM) hd = 1;
		// Hardware input status
		//       0123456 89012345 78901234 678901  456789012
		s = _T( "Digital PP  IE  HD   X-Loc Angle  Period   RotateCnt     MsgSeq  Glitch  Cmd/Cnt    1stWord  Seq  Chnl  Gate  (NIOS)Small Large  Pulser  (PAP)Small Large  Pulser  Status" );
		t = s;
		//m_lbOutput.AddString(t);	// show top line
		//s.Format(_T("    MsgCnt = %d, GlitchCnt = %d  CmdId  1stWord"),
		//	gwMsgSeqCnt, gLastIdataPap.bNiosGlitchCnt, 
		//	gLastIdataPap.wLastCmdId, gLastIdataPap.w1stWordCmd);
		//t += s;
		m_lbOutput.AddString(t);
		s.Format( _T( "0x%04x  %d   %d   %d    %05d %04d   %06d   %05d" ),
			gLastIdataPap.bDin, pp, ie, hd, gLastIdataPap.wLocation, gLastIdataPap.wAngle,
			gLastIdataPap.wPeriod, gLastIdataPap.wRotationCnt );
		t = s;
		s.Format(_T("         %05d   %03d     %03d/%05d  %05d    %02d   %02d    %02d"),
			gwMsgSeqCnt, gLastIdataPap.bNiosGlitchCnt, 
			//gLastCmd.wMsgID, gLastCmd.wMsgSeqCnt, gLastIdataPap.w1stWordCmd,
			gwLastCmdId, gwLastCmdSeqCnt, gw1stWordCmd,
			(gLastIdataPap.bCmdSeq % 10), (gLastIdataPap.bCmdChnl % 10), gLastIdataPap.bCmdGate&3);
		t += s;
		// count of commands received by ADC & Pulser board, then wStatus value
		s.Format(_T("          %05d %05d  %05d"), gLastAscanPap.wSmallCmds, gLastAscanPap.wLargeCmds, gLastAscanPap.wPulserCmds);
		t += s;
		// Show number of cmds received by PAP 
		s.Format(_T("        %05d %05d  %05d   %04x"), gwPapSmallCmds, gwPapLargeCmds, gwPapPulserCmds, gwStatus);
		t += s;
		m_lbOutput.AddString(t);

		s = _T(" ");
		m_lbOutput.AddString(s);
		// Check the command queues for the ADC and Pulser. If any commands are waiting
		// exit this routine
		s = _T(" *** BUSY SENDING COMMANDS ***");
		if (GetAdcCmdQ())
			{
			m_lbOutput.AddString(s);
			return;
			}
		if (GetPulserCmdQ()) 
			{
			m_lbOutput.AddString(s);
			return;
			}
		// on next 2 lines, display the packet/second for Nx data and all wall data
		// skip one line

		for (i = 0; i < 2; i++)
			{
			switch (i)
				{
			case 0:
				s = _T("NcNx Wall:  ");		s += m_sPktRate[i];		break;
			case 1:
				s = _T("All  Wall:  ");		s += m_sPktRate[i];		break;
				}
			m_lbOutput.AddString(s);
			}

		// Next line show ADC versions & temp then Pulser versions & temp
#if 1
		t = _T("");
		m_lbOutput.AddString(t);
		s = _T("ADC: [  VerHW     VerSW     TempCPU  TempBd]  PULSER: [  VerHW     VerSW     TempCPU       PRF ]");
		m_lbOutput.AddString(s);
		s.Format(_T("     %s %s   %03d C    %03d C ]"), m_sHwVerAdc, m_sSwVerAdc,
					gLastAscanPap.bFPGATempA, gLastAscanPap.bBoardTempA);
		t += s;
#if 1
		s.Format(_T("%s %s   %03d C       %5d ]"), m_sHwVerPulser, m_sSwVerPulser,
			gLastAscanPap.wCPU_TempP, gwPap_Prf);	//gwPap_Prf swiped from message to pulser. Not fed back from pulser
		t += s;
#endif
		m_lbOutput.AddString(t);
#endif

		}	// if (gLastIdataPap.wMsgID == eNcNxInspID)
#endif
	}

void CPA2WinDlg::OnBnClickedBnEraseDbg()
	{
	// TODO: Add your control notification handler code here
	m_lbOutput.ResetContent();
	}


void CPA2WinDlg::DebugToNcNxDlg( CString s )
	{
	if (gDlg.pNcNx)
		gDlg.pNcNx->DebugOut(s);
	}

#ifdef I_AM_PAG
// Code to send messages from windows test dialog to the boards via the PAP.
// Adapted from PhasedArrayMMI CTscanDlg code. In PhasdArrayMMI, the server to the
// inspection machinery was called PAM - Phased Array Master. Here it is called
// PhasedArrayProcessor - PAP
// nClientNumber is the target PAP that will receive the message. Internally the 
// message specifies which board will receive the message. The PAP uses that information
// to route the message to the correct board.
//
// 2018-08-10 Now can have IP addresses which do not dictate the client connection
// This will result because of the use of DHCP to set PAP IP addresses.
// In this send procedure map the intended client number into the actual number in the 
// stSCM[0] is the NcNx server for all PAP's, stSCM[1] is the server for All wall inspection data
// the client number in this call is the logical nClientNumber..0 would be the first Pap, 1 would be the 2nd etc
// Because the connection of  the client to the server no longer determines the client number, there must
// be a mapping between physical client number and the connection client number
// Use an array gbActualClientConnection[MAX_CLIENTS_PER_SERVER] to map the logical client number
// to the physical connection number generated when a client connects.
// 2018-08-13 now use stSCM[ePAP_Server].bActualClientConnection[nClientNumber] to map client  number to actual
// connection

// Assumes the server for PAP is stSCM[0] which for now is true
BOOL CPA2WinDlg::SendMsgToPAP(int nClientNumber, int nMsgID, void *pMsg)	// the client is the PAP
	{
	CString s;
	int nLen;		// how long will the returned message be?
	int rc;
	GenericPacketHeader *pHeader;
	ST_SMALL_CMD *pSmallCmd = (ST_SMALL_CMD *)pMsg;
	ST_SERVER_CONNECTION_MANAGEMENT &pSCC = stSCM[0];

	pHeader = (GenericPacketHeader*)pMsg;
	nLen = pHeader->wByteCount;

	// map the logical connection number to the physical connection number
	// must know connection pap .. ePAP_Server = 0

	BYTE bPhyCon = stSCM[ePAP_Server].bActualClientConnection[nClientNumber];


	if ( (bPhyCon < 0) || (bPhyCon >= MAX_CLIENTS))
		{
		s.Format(_T("CPA2WinDlg::SendMsgToPAP FAIL, bad client = %d\n"), bPhyCon);
		TRACE(s);
		if (gDlg.pNcNx)
			gDlg.pNcNx->DebugOut(s);		
		delete pMsg;	// clean up the mess
		return FALSE;
		}

	nClientNumber = bPhyCon;	// set connection number to the physical number


		// assuming server # 0 = ePAP_Server is the PAP server
	if (NULL == stSCM[ePAP_Server].pClientConnection[nClientNumber])
		{
		s.Format(_T("CPA2WinDlg::SendMsgToPAP No Valid ClientConnection for client = %d\n"),nClientNumber );
		TRACE(s);
		if (gDlg.pNcNx)
			gDlg.pNcNx->DebugOut(s);
		delete pMsg;	// clean up the mess
		return FALSE;
		}

	if (NULL == stSCM[ePAP_Server].pClientConnection[nClientNumber]->pSocket)
		{
		s.Format(_T("CPA2WinDlg::SendMsgToPAP pSocket is NULL for client = %d\n"), nClientNumber);
		TRACE(s);
		if (gDlg.pNcNx)
			gDlg.pNcNx->DebugOut(s);
		delete pMsg;	// clean up the mess
		return FALSE;
		}

	if (0 == stSCM[ePAP_Server].pClientConnection[nClientNumber]->bConnected)
		{
		s.Format(_T("CPA2WinDlg::SendMsgToPAP Client = %d is not connected\n"),nClientNumber );
		TRACE(s);
		if (gDlg.pNcNx)
			gDlg.pNcNx->DebugOut(s);

#if 1

		delete pMsg;	// clean up the mess
		return FALSE;
#endif
		}

	// allow for more than one type of message. As of June 2016 only NcNx msg
	if (nMsgID > 0)	
		{

		// int CServerConnectionManagement::SendPacketToPAP
		// posts a thread message to a ServerSocketOwnerThread to empty the linked list and send all packets
		// Message activates CServerSocketOwnerThread::TransmitPackets(WPARAM w, LPARAM lParam)

		s = _T("rc = stSCM[ePAP_Server].pSCM->SendMsgToPAP\n");
//		if (gDlg.pNcNx)
//			gDlg.pNcNx->DebugOut(s);

		// auto delete
		rc = stSCM[ePAP_Server].pSCM->SendPacketToPAP(nClientNumber, (BYTE *)pMsg, nLen, 1);
		if (rc < 0)
			{
			s.Format(_T("PA2WinDlg::SendMsgToPAP Failed to send pkt, err num  = %d\n"), rc);
			TRACE(s);
			if (gDlg.pNcNx)
				gDlg.pNcNx->DebugOut(s);

			delete pMsg;	// clean up the mess
			return FALSE;
			}

		// Instrument can only take 5 commands at a time.
		// Check feedback info in Idata to see how deep Instruments fifo is
		// temporary fix??
		//Sleep(10); // WAS 50 put delay into PAP instead of PAG
		return TRUE;
		}

	else
		{

		s.Format(_T("PA2WinDlg::SendMsgToPAP FAIL, Unknown know msg ID = %d\n"), nMsgID);
		TRACE(s);
		if (gDlg.pNcNx)
			gDlg.pNcNx->DebugOut(s);

		delete pMsg;	// clean up the mess
		return FALSE;
		}

	return FALSE;
	}

#endif


	void CPA2WinDlg::OnLbnSelchangeList1()
	{
		// TODO: Add your control notification handler code here
	}
