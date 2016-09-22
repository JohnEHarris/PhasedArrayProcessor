// ServiceApp.cpp: implementation of the CServiceApp class.
//
//////////////////////////////////////////////////////////////////////
// For debugging, run this service as manual start.
// Start the program/debugging session before powering up the instrument
// Run the MMI last of all
//2016-05-24 CNTService constructor runs as a result of being the base class of the service app
// CServiceApp::CServiceApp() : CNTService

/**********************
First step in converting from Yanming's c array based structure to my class structure is to
replace the server connection from the master to the instruments. This means replace 
ServerSocketInit() and ServerSocketThread() with code from ServerConnectionManagement and its
managed classes.

**********************/

#include "stdafx.h"
#include "MainModuleGlobal.h"
#include "ServiceApp.h"
#include "winsock2.h"
#include "math.h"
#include "time.h"

#include <windows.h>
#include <process.h>
#include <stddef.h>
#include <stdlib.h>
#include <conio.h>
#include <ws2spi.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <stdio.h>
#include <share.h>

#include "InstMsgProcess.h"
#include "HwTimer.h"
#include "InspState.h"
#include "ClientConnectionManagement.h"	// 21-Jan-13 jeh
#include "CCM_PAG.h"					// 22-Jan-13 jeh
#include "ClientSocket.h"


#define CURRENT_VERSION		"Version 1.1"
#define BUILD_VERSION			3

#define VERSION_MAJOR           1
#define VERSION_MINOR           1
#define VERSION_BUILD           BUILD_VERSION


#if 0

1.0.003			06-Sep-16	CvChannel* pvChannel now two dimensional. [0][j] - fix 1st index later
1.1.002			15-Jul-16	Eliminate Yanming code not being used
1.1.001			May 2016	New Phased Array 2
1.0.01			2016-06-14 Nc Nx working with fake data input, good output to PAG - from header file.
1.0.001			12-Mar-2013	Add Timer Tick to PAM to restart PAG connection attempt if not connected. PAM and PAG code the same almost
							Steers commands with MMI_CMD variables PAM_Number and Inst_Number_In_PAM


#endif


#undef YANMING_CODE
// HYBRID_CODE used my instrument server (ServerConnectionManagement), legacy connection to mmi

CServiceApp theApp;		// the persistent instance of the application
UINT uAppTimerTick;		// approximately 50 ms

UINT uVchannelConstructor[4][40];	// count when constructor called. 4 inst, 40 chnl

UINT CheckKey( void *dummy );

void ShutDownSystem();

/*  Begin Globals */

BOOL repeat = TRUE;     /* Global repeat flag and video variable */
// global flags to regulate how often or 'if' a thread is created/run

CInspState InspState;		// one instance of a state keeping class.. not a pointer!
C_MSG_ALL_THOLD  g_AllTholds;
C_MSG_NC_NX g_NcNx;
float  g_fMotionPulseLen = 0.506329f;

/*  End Globals */


/*************** BEGIN FUNCTION PROTOTYPES ******************/
					
//void SetGetInspectMode_M (int nSetGet/* 0=SET, 1=GET */, int *nMode, int *nMotionTime);
void Inspection_Process_Control();

//void SetGetSiteDefaults (int nSetGet /* 0=SET, 1=Get */, SITE_SPECIFIC_DEFAULTS *pSiteDef);
int GetMaxXOffset(void);
int GetMinXOffset(void);
int GetMaxXSpan(void);



int FindWhichSlave(int nChannel);
int FindSlaveChannel(int nChannel);
int FindDisplayChannel(int nArray, int nArrayCh);
void ComputeTranFocusDelay(float thickness, float zf_value, float water_path, float incident_angle, WORD *td);
void CstringToTChar(CString &s, _TCHAR *p, int nSizeOfArray);
void CstringToChar(CString &s, char *p, int nSizeOfArray);

/*************** END FUNCTION PROTOTYPES ******************/



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


/*************** END FUNCTION PROTOTYPES ******************/					
//CServiceApp *pTheApp;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CServiceApp::CServiceApp()
	: CNTService(TEXT("PhasedArray_Master"), TEXT("Phased Array Master"))
	, m_hStop(0)
	{
//	WSADATA wsaData;
//	int rv, 
	int i;
	//pTheApp = this;

	AfxSocketInit();
	// unnecessary. Program nulls all that can be nulled on start
#if 0
	for ( i = 0; i < MAX_SERVERS; i++)
		{
		pSCM[i] = NULL;
		//memset((void *) &stSCM[i], 0, sizeof(ST_SERVER_CONNECTION_MANAGEMENT));
		}
#endif


	m_nShutDownCount = 0;
	pCSSaveDebug =new CRITICAL_SECTION();
	InitializeCriticalSectionAndSpinCount(pCSSaveDebug,4);

	for ( i = 0; i < MAX_CLIENTS_PER_SERVER; i++)
		{
		pAppInstAccess[i] = new CRITICAL_SECTION();
		InitializeCriticalSectionAndSpinCount(pAppInstAccess[i],4);
		}


	g_NcNx.Long[0] = 1;
	g_NcNx.Long[1] = 1;
	g_NcNx.Tran[0] = 1;
	g_NcNx.Tran[1] = 1;
	g_NcNx.Oblq1[0] = 1;
	g_NcNx.Oblq1[1] = 1;
	g_NcNx.Oblq2[0] = 1;
	g_NcNx.Oblq2[1] = 1;
	g_NcNx.Oblq3[0] = 1;
	g_NcNx.Oblq3[1] = 1;
	g_NcNx.Lamin[0] = 1;
	g_NcNx.Lamin[1] = 1;
	g_NcNx.Wall[0] = 1;

#if 0
	// debug running average tested for 1,2,4,8 nx values and works
	// hwtimer ticks at 3.58 MHz
	// for n== 1, 20 loops = 10 ticks  less than 3 uSec
	// for n== 2, 20 loops = 10 ticks
	// for n== 4, 20 loops = 10 ticks
	// for n== 8, 20 loops = 12 ticks  12/3.58 = 3.4 uSec on Dual Pentium 1.8 Ghz system
	// for n== 8, 20 loops = 17 ticks  with avg and search for min wall range 4. 17/3.6 = 4.7 uSec  19-apr-2012
	// for n== 8, 20 loops = 18 ticks  with avg and search for min wall range 8. 18/3.6 = 5 uSec  19-apr-2012
	// 
#endif

	}

CServiceApp::~CServiceApp()
	{
	int i = 1;	// FOR DEBUG, lock in loop before stop event
	//Stop();

	if (m_nFakeDataExists)
		m_FakeData.Close();

	if (m_nDebugLogExists)
		m_DebugLog.Close();

	if (pCSSaveDebug)	// critical section access to debug log file
		delete pCSSaveDebug;
	
	for ( i = 0; i < MAX_CLIENTS_PER_SERVER; i++)
		{
		delete pAppInstAccess[i];
		}	
	
	ShutDown(); // first place when closing dos window


	if( m_hStop )
		::SetEvent(m_hStop);
	
	Sleep(6000);	// long enough to break out of Run infinite loop.. leave in for Yanming code

	ReportStatus(SERVICE_STOP_PENDING, 11000);

}


// Next 4 registry operations copied from TscanDlg.cpp
/***********
GetServerConnectionManagementInfo()
SaveServerConnectionManagementInfo()
GetClientConnectionManagementInfo()
SaveClientConnectionManagementInfo()

This big comment was a # if 0 but made Visual Studio miss the definition of GetServerConnectionManagementInfo()
			
typedef struct
	{
	char Ip[16];	// dotted address eg., "192.168.10.10"
	UINT uPort;		// port to listen on
	int nPacketSize;	// Expected packet size from client...ie. how many bytes to receive in a packet
	char ClientBaseIp[16];
	}	SRV_SOCKET_INFO;	// Element of a server listener socket

MAX_SERVERS				// how many servers supported by this application program
MAX_CLIENTS_PER_SERVER	// how many client connection on each server

PAM server that Instruments connects to:
192.168.10.10 : 7502
During development,Instruments at 192.168.10.201+ connect to PAG at 192.168.10.10 on port 7502
IP Port is the port the server listens on. 
The Client Base IP is the IP of the 1st element in the 
ST_SERVERS_CLIENT_CONNECTION *pClientConnection[MAX_CLIENTS_PER_SERVER];
Here, ClientBaseIp[16] = 192.168.10.201
*****/

// Read the registry to determine the hardware configuration of the SCM
void CServiceApp::GetServerConnectionManagementInfo(void)
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

		// The server's port that listens for clients to connect
		szI.Format(_T("%d-Server_IP_Port"), i);
		gServerArray[i].uPort =  gDlg.pTuboIni->GetProfileInt(sSrvSection,szI, 7502);

		szI.Format(_T("%d-Client_Packet_Size"), i);
		gServerArray[i].nPacketSize = gDlg.pTuboIni->GetProfileInt(sSrvSection,szI,INSTRUMENT_PACKET_SIZE);

		szI.Format(_T("%d-Client_Base_IP"), i);
		szIp = gDlg.pTuboIni->GetProfileString(sSrvSection,szI, _T("192.168.10.201"));
		CstringToChar(szIp,gServerArray[i].ClientBaseIp);
		}
	}

// Save the hardware configuration information for the SCM to the registry
// When using the CTuboIni class, write the section header by itself before writing any key values
// Only for previous MIT version of TuboIni
//
// character order space ! # $ & * + - before numbers.
void CServiceApp::SaveServerConnectionManagementInfo(void)
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
			szIp = _T("PAM Server for Instruments 1-N");
			gDlg.pTuboIni->WriteProfileString(sSrvSection,szI, szIp);
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

	/***************** CLIENT CONNECTION MANAGEMENT ***************/

#if 0
typedef struct
	{
	CString sClientName;			// symbolic name of the client network address,  eg., MC_ACP_HOSTNAME = "mc-acp"
	CString sClientIP4;				// IP4 dotted address of client, normally this computers NIC address, eg., 192.168.10.10
	CString sServerName;			// symbolic name of the server network address we want to connect to with this connection, eg., mc-scp 
	CString sServerIP4;				// IP4 dotted address of server eg., 192.168.10.50
	short nPort;					// added in PAM - port the server is listenin on.
	int nPacketSize;				// added in PAM
	int nWinVersion;

	}	ST_SOCKET_NAMES;
MAX_CLIENTS
#endif

// Read the registry to determine the hardware configuration of the CCM
// PAM clients connect to the PAG server which is listening at port 7501
// Client 0 should be the connection to the PAG server.
// 2016-05-17 The PAM now called the Receiver is a client to only the PT
void CServiceApp::GetClientConnectionManagementInfo(void)
	{
	if (gDlg.pTuboIni == NULL)	return;

	int i;
	CString szPort, szIp, szI, sClientSection;
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
		stSocketNames[i].sServerIP4 =  gDlg.pTuboIni->GetProfileString(sClientSection,szI, _T("192.168.10.20"));

		szI.Format(_T("%d-Server_Listen_Port"), i);
		stSocketNames[i].nPort = gDlg.pTuboIni->GetProfileInt(sClientSection,szI, 7501);

		szI.Format(_T("%d-Server_Cmd_Packet_Size"), i);
		stSocketNames[i].nPacketSize = gDlg.pTuboIni->GetProfileInt(sClientSection,szI, sizeof(MMI_CMD));

		szI.Format(_T("%d-Win_Version"), i);
		stSocketNames[i].nWinVersion = gDlg.pTuboIni->GetProfileInt(sClientSection,szI, 7);

		}	
	}

// Save the hardware configuration information for the CCM to the registry
void CServiceApp::SaveClientConnectionManagementInfo(void)
	{
	int i;
	if (gDlg.pTuboIni == NULL)	return;

	CString szPort, szIp, szI, sClientSection;
	sClientSection = _T("ClientConnectionManagement");	
	//if (m_pTuboIni->m_pDictionary == NULL)	return;

	// Write the section header
	//gDlg.pTuboIni->WriteProfileString(sClientSection,_T(""), _T(""));

	// force max clients key to top of section
	gDlg.pTuboIni->WriteProfileInt(sClientSection,_T("! MaxClients"), gnMaxClients);
	for ( i = 0; i < gnMaxClients; i++)
		{

		szI.Format(_T("%d-Client_Name"), i);	// url of the client machine
		gDlg.pTuboIni->WriteProfileString(sClientSection,szI, stSocketNames[i].sClientName);
		szI.Format(_T("%d-Client_IP4"), i);	// dotted IP 192.168.10.10 etc
		gDlg.pTuboIni->WriteProfileString(sClientSection,szI, stSocketNames[i].sClientIP4);

		szI.Format(_T("%d-Server_Name"), i);	// url of the client machine
		gDlg.pTuboIni->WriteProfileString(sClientSection,szI, stSocketNames[i].sServerName);
		szI.Format(_T("%d-Server_IP4"), i);	// dotted IP 192.168.10.10 etc
		gDlg.pTuboIni->WriteProfileString(sClientSection,szI, stSocketNames[i].sServerIP4);

		szI.Format(_T("%d-Server_Listen_Port"), i);
		gDlg.pTuboIni->WriteProfileInt(sClientSection,szI, stSocketNames[i].nPort);

		szI.Format(_T("%d-Server_Cmd_Packet_Size"), i);
		gDlg.pTuboIni->WriteProfileInt(sClientSection,szI, stSocketNames[i].nPacketSize);

		szI.Format(_T("%d-Win_Version"), i);
		gDlg.pTuboIni->WriteProfileInt(sClientSection,szI, stSocketNames[i].nWinVersion);

		}
	}

// Assumes the first client connection from the PAP is to the Receiver or PAG
// hence stSocketNames[0]
// convert characters 192.168.10.200 to uint
void CServiceApp::SetMy_PAM_Number(CString &Ip4, UINT uPort)
	{
	char txt[32];
	CstringToChar(stSocketNames[0].sClientIP4, txt);
	UINT uBaseIp = ntohl(inet_addr(txt));
	CstringToChar(Ip4, txt);
	UINT uMyIp = ntohl(inet_addr(txt));
	m_nPamNumber = uMyIp - uBaseIp;
	m_uPamPort = uPort;
	}

/******************************************************************/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL CServiceApp :: InitInstance() 
	{
	// Since the service has no user interface, we will store information about hardware setting
	// for TCP/IP in an INI file. This info copied from AGS3 project startup application
	// The ini file will be store in the folder which has the executable code

	int i;
	TRACE1("_WIN32_WINNT  is 0x%0x\n", _WIN32_WINNT);
	memset (&uVchannelConstructor,0, sizeof(uVchannelConstructor));
	CString s,Fake;	// fake is a debug file with fake data and fake data msg to PAG
	CString DeBug;	// another debug file to catch printf statements since vs2015 does not output to monitor screen

	// ths didn't work with 2010 compiler. Might have with vs2005
	CString t = AfxGetAppName();	// shows AGS3
	CString AppName = t+ _T(".exe");
	CString ch;	// trim characters
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
	// shows "D:\PhasedArrayGenerator\PA_Master_VS2010\Debug\"
	// The name of the App becomes an implicit part of the key
	// We only write Tuboscope, but clever windows makes a subregistry entry of AdpMMI
	// SetRegistryKey Causes application settings to be stored in the registry instead of .INI files.
	// SetRegistryKey(_T("Tuboscope"));	// gen HKEY_CUR_USR\Software\Tuboscope\AdpMMI  key

	// To use an INI file instead, set m_pszProfileName to the ini file path
	// First free the string allocated by MFC at CWinApp startup.
	// The string is allocated before InitInstance is called.
	// free((void*)m_pszProfileName);
	//Change the name to the .INI file.
	//The CWinApp destructor will free the memory.
	t += _T("HardwareConfig.ini"); // shows "D:\PhasedArrayGenerator\PA_Master_VS2010\.\Debug\HardwareConfig.ini"
	// t = _T("D:\\PhasedArrayGenerator\\PA_Master_VS2010\\Debug\\HardwareConfig.ini");
	// m_pszProfileName=_tcsdup(t); // File in the exe directory

	//SetRegistryKey(_T("Tuboscope"));	// gen HKEY_CUR_USR\Software\Tuboscope\  key


	SetRegistryKey(_T("Tuboscope"));	// gen HKEY_CUR_USR\Software\Tuboscope\  key
	// Use a custom Tubo INI file reader/writer to configure the PAM
	if ( gDlg.pTuboIni == NULL )
		gDlg.pTuboIni = new CTuboIni(t);	// 12
	//m_pTuboIni = new CTuboIni(t);
	//i = sizeof (CTuboIni);


	// Open a debugger file for fake data
	//TCHAR* pszFileName = Fake.GetString();	//_T("c:\\test\\myfile.dat");
	m_nFakeDataExists = m_nDebugLogExists = 0;
	//CFile myFile;
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

	RegisterService(__argc, __argv);
	//printf("Starting the Service/n"); // causes an exception 


	return FALSE;
	}

// Output fake data to a file
void CServiceApp::SaveFakeData(CString& s)
	{
#ifdef _DEBUG
	char ch[4000];
	CstringToChar(s,ch,4000);
	if (0 == m_nFakeDataExists)
		{
		TRACE(_T("Fake data file not available\n"));
		return;
		}
	try
		{
		m_FakeData.Write(ch,strlen(ch));	// I want to see ASCII in the file
		//m_FakeData.Flush();
		}
	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}
#endif

	}

void CServiceApp::CloseFakeData(void)
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

void CServiceApp::SaveDebugLog(CString& s)
	{
#ifdef _DEBUG
	char ch[4000];
	CstringToChar(s,ch,4000);
	if (0 == m_nDebugLogExists)
		{
		TRACE(_T("Debug log file not available\n"));
		return;
		}
	EnterCriticalSection(pCSSaveDebug);
	try
		{
		m_DebugLog.Write(ch,strlen(ch));	// I want to see ASCII in the file
		//m_DebugLog.Flush(); -- not needed. Kills Nc Nx processing time
		}
	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}
	LeaveCriticalSection(pCSSaveDebug);
#endif
	}
	
void CServiceApp::CloseDebugLog(void)
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
	}



int CServiceApp::ExitInstance()
	{
	int i;
	i = 1;
	CloseFakeData();
	CloseDebugLog();
#if 0
	if (m_pMySampleMem)
		delete m_pMySampleMem;
  
	DoCleanup();
#endif
	return CWinApp::ExitInstance();
	}

// If the service could exit normally, this routine would be called. Normally does not happen when
// debugging as the cmd line window is close and the whole program aborts.
// use a local variable in Run() to be set by the debugger to force a "graceful" shutdown
void CServiceApp::ShutDown(void)
	{
	int i, j,k;
	int nError;
	//void *pV;
	CString s;
	ST_SERVER_CONNECTION_MANAGEMENT *pstSCM;
	k = 0;

	if (m_nShutDownCount)
		return;	// only one time thru shutdown

	// global variable
	m_nShutDownCount++;	// set to 0 in constructor

	// need to kill the server listen thread and socket.. done below in server shut down routine

#if 1
	for ( j = 0; j < MAX_SERVERS; j++)
		{	// loop MAX_SERVERS
		pstSCM = &stSCM[j];
		// let the listener thread kill the listener socket if it still exists
		if (NULL == pstSCM) goto SERVERS_CLIENT_LOOP;
		//
		nError = pstSCM->pSCM->ServerShutDown(j);
		if (nError !=0)
			{
			s.Format(_T("Failed to shutdown Server%d Error = %d\n"), j, nError);
			}

		while (k)
			{
			Sleep(20);	// let debugger follow to ServerListenThread			
			}
#if 0
		if (pstSCM->pCSDebugIn)
			{
			EnterCriticalSection(pstSCM->pCSDebugIn );
			while (	pstSCM->pInDebugMessageList->GetCount() > 0)
				{
				pV = (void *) pstSCM->pInDebugMessageList->RemoveHead();
				delete pV;
				}
			LeaveCriticalSection(pstSCM->pCSDebugIn );
			delete pstSCM->pInDebugMessageList;
			}

			

		if (pstSCM->pCSDebugOut)
			{
			EnterCriticalSection(pstSCM->pCSDebugOut );
			while (	pstSCM->pOutDebugMessageList->GetCount() > 0)
				{
				pV = (void *) pstSCM->pOutDebugMessageList->RemoveHead();
				delete pV;
				}
			LeaveCriticalSection(pstSCM->pCSDebugOut );
			delete pstSCM->pOutDebugMessageList;
			}

#endif

SERVERS_CLIENT_LOOP:
		;

#if 0
		for ( i = 0; i < MAX_CLIENTS_PER_SERVER; i++)
			{
			if ( NULL == stSCM[j].pClientConnection[i]) continue;
			for ( k = 0; k < MAX_CHNLS_PER_MAIN_BANG; k++)
				{
				if (stSCM[j].pClientConnection[i]->pvChannel[k] )
					{
					delete stSCM[j].pClientConnection[i]->pvChannel[k];
					stSCM[j].pClientConnection[i]->pvChannel[k] = 0;
					}
				}

			// delete linked lists and critical sections and Async socket
			delete stSCM[j].pClientConnection[i];
			
			}
#endif
		}		// loop MAX_SERVERS
#endif

	if (m_pTestThread)
		{
		::SetEvent(m_pTestThread->m_hTimerTick);
		Sleep(5);
		i = m_pTestThread->PostThreadMessage(WM_QUIT,0,0L); // fails, returns 0
		Sleep(200);
		if (i)
			j = i;
		else 
		j = i*2;
		//Sleep(20);
		}

	Sleep(10);
	m_pTestThread = 0;

#if 1
	for ( i = 0; i < MAX_SERVERS; i++)
		{
		if (pSCM[i])
			{
			pSCM[i]->ServerShutDown(i);
			//Sleep(200);
			delete pSCM[i];
			}
		pSCM[i] = NULL;
		//Sleep(10);
		}

	//Sleep(300);
	// This code taken from void CTscanDlg::OnCancel() - the PAG


	//if (pCCM_SysCp)
#endif

	if (pCCM_PAG)
		{
		// delete the client socket, then the cmd process thread, then the send thread, then the receive thead
		if (pCCM_PAG->m_pstCCM)
			{
			if (pCCM_PAG->m_pstCCM->pSocket)
				{	// socket exists
				if (pCCM_PAG->m_pstCCM->pSocket->ShutDown(2))
					{
					s = _T("Shutdown of client socket was successful\n");
					TRACE(s);					
					nError = GetLastError();
					}
				else
					{
					nError = GetLastError();	// WSAENOTCONN                      10057L
					s .Format(_T("Shutdown of client socket[%d] failed\n"), nError);
					TRACE(s);
					}
				}	// socket exists

			pCCM_PAG->KillReceiveThread();
			i = 0;
			while (( i < 50 ) && ( pCCM_PAG->m_pstCCM->pReceiveThread != 0) )
				{	Sleep (10);		i++;	}
			if ( i >= 50) TRACE("CCM - Failed to kill Receive Thread");
			
			pCCM_PAG->KillSendThread();
			i = 0;
			while (( i < 50 ) && ( pCCM_PAG->m_pstCCM->pSendThread != 0) )
				{	Sleep (10);		i++;	}
			if ( i >= 50) TRACE("CCM - Failed to kill Send Thread");

			pCCM_PAG->KillCmdProcessThread();
			i = 0;
			while (( i < 50 ) && ( pCCM_PAG->m_pstCCM->pCmdProcessThread != 0) )
				{	Sleep (10);		i++;	}
			if ( i >= 50) TRACE("CCM - Failed to kill CmdProcess Thread");

			}
		// delete send thread and critical sections, delete receive thread and critical sections
		delete pCCM_PAG;	
		pCCM_PAG = NULL;		
		}

	for ( i = 0; i < MAX_CLIENTS; i++)
		{
		if (pCCM[i])	delete pCCM[i];	// in 2016 there is only pCCM_PAG
		pCCM[i] = NULL;
		//Sleep(10);
		}
	i = 14;
	s = _T("Here we are trying to close CCM stuff");
	TRACE(s);

#if 1
	ReportStatus(SERVICE_STOP_PENDING, 11000);
	if( m_hStop )
		::SetEvent(m_hStop);
	m_hStop = 0;
#endif

	}



/********************************* RUN ******************************/
/********************************* RUN ******************************/
/********************************* RUN ******************************/


void CServiceApp :: Run( DWORD, LPTSTR *) 
	{	// args not used in this small example
		// report to the SCM that we're about to start
	int i,j,k;
	CString s;
	int nDebugShutdown = 0;		// manual way to shut down when running the debugger
	// To shutdown in debugger, set nDebugShutdown = 1 inside the Run() infinite loop

	ReportStatus(SERVICE_START_PENDING);
	
	m_hStop = ::CreateEvent(0, TRUE, FALSE, 0);

	// You might do some more initialization here.
	// Parameter processing for instance ...
	
	// report SERVICE_RUNNING immediately before you enter the main-loop
	// DON'T FORGET THIS!
	ReportStatus(SERVICE_RUNNING);


	m_ptheApp = (CServiceApp *) AfxGetApp();

#if 1
	// If no registry entry exists, the user will get the development defaults.
	// Probably won't work in a real system.
	// access to our custom ini files	
	// gDlg.pTuboIni = new CTuboIni(_T("D:\\PhasedArrayGenerator\\PA_Master_VS2010\\Debug\\HardwareConfig.ini"));
	if (gDlg.pTuboIni)
		{
		//if (m_pTuboIni->m_pDictionary)
			{
			GetServerConnectionManagementInfo();
			GetClientConnectionManagementInfo();
			//Sleep(500);

#define CREATE_NEW_INI_FILE
#ifdef CREATE_NEW_INI_FILE
			// do we need a new ini file because of architecture changes?
			// generate a default file by including these opeartions. Otherwise
			// change an existing ini file with notepad and let it configure the PAM
			// when the 2 Get functions above execute.
			//
			SaveServerConnectionManagementInfo();	
			SaveClientConnectionManagementInfo();

#endif
			}
		delete gDlg.pTuboIni;	// this will rewrite the ini file
		}
#endif


#if 0
//ifdef _DEBUG
printf("Size of PACKET_STATS is %d\r\n", sizeof(PACKET_STATS));
printf("Size of UDP_CMD_HDR is %d\r\n", sizeof(UDP_CMD_HDR));
printf("Size of SLAVE_HDR is %d\r\n", sizeof(SLAVE_HDR));
printf("Size of UDP_CMD is %d\r\n", sizeof(UDP_CMD));
printf("Size of UDP_SLAVE_DATA is %d\r\n", sizeof(UDP_SLAVE_DATA));
printf("Size of INST_DATA is %d\r\n", sizeof(INST_DATA));
printf("Size of IPXHEADER is %d\r\n", sizeof(IPXHEADER));
printf("Size of PACKET_DATA is %d\r\n", sizeof(PACKET_DATA));
printf("Size of INST_CMD_DATA is %d\r\n", sizeof(INST_CMD_DATA));
printf("Size of MMI_CMD is %d\r\n", sizeof(MMI_CMD));
printf("Size of UT_SHOE is %d\r\n", sizeof(UT_SHOE));
printf("Size of INSP_HDR is %d\r\n", sizeof(INSP_HDR));
printf("Size of UT_INSP is %d\r\n", sizeof(UT_INSP));
printf("Size of PEAK_DATA is %d\r\n", sizeof(PEAK_DATA));
printf("Size of I_MSG_RUN is %d\r\n", sizeof(I_MSG_RUN));
printf("Size of I_MSG_CAL is %d\r\n", sizeof(I_MSG_CAL));
printf("Size of I_MSG_PKT is %d\r\n", sizeof(I_MSG_PKT));
printf("Size of I_MSG_NET is %d\r\n", sizeof(I_MSG_NET));
printf("Size of SITE_SPECIFIC_DEFAULTS is %d\r\n", sizeof(SITE_SPECIFIC_DEFAULTS));
printf("Size of MAX_CLIENTS is %d\r\n", MAX_CLIENTS);
printf("Size of ST_CLIENT_CONNECTION_MANAGEMENT is %d\r\n", sizeof(ST_CLIENT_CONNECTION_MANAGEMENT));
printf("Size of CAsyncSocket is %d\r\n", sizeof(CAsyncSocket));
printf("Size of MAX_CLIENTS_PER_SERVER is %d\r\n", MAX_CLIENTS_PER_SERVER);
printf("Size of MAX_SERVERS is %d\r\n", MAX_SERVERS);
printf("Size of ST_SERVERS_CLIENT_CONNECTION is %d\r\n", sizeof(ST_SERVERS_CLIENT_CONNECTION));
printf("Size of ST_SERVER_CONNECTION_MANAGEMENT is %d\r\n", sizeof(ST_SERVER_CONNECTION_MANAGEMENT));
printf("Size of SRawDataPacket is %d\r\n", sizeof(SRawDataPacket));	// 1040



#if 0
Debugging Phased Array Master.
	Summer 2012
Size of INSP_HDR is 72
Size of I_MSG_RUN is 1264
Size of I_MSG_CAL is 1104
Size of I_MSG_PKT is 408
Size of I_MSG_NET is 1248
Size of SITE_SPECIFIC_DEFAULTS is 108


31-Jan-13
Debugging Phased Array Master.
Size of PACKET_STATS is 20
Size of UDP_CMD_HDR is 72
Size of SLAVE_HDR is 116
Size of UDP_CMD is 1316
Size of UDP_SLAVE_DATA is 212
Size of INST_DATA is 1282
Size of IPXHEADER is 30
Size of PACKET_DATA is 1328
Size of INST_CMD_DATA is 78
Size of MMI_CMD is 1272
Size of UT_SHOE is 80
Size of INSP_HDR is 68   vs 72
Size of UT_INSP is 1120
Size of PEAK_DATA is 96
Size of I_MSG_RUN is 1260   vs 1264
Size of I_MSG_CAL is 1100   vs 1104
Size of I_MSG_PKT is 400   vs 408
Size of I_MSG_NET is 1239   vs 1248
Size of SITE_SPECIFIC_DEFAULTS is 108
Size of MAX_CLIENTS is 4
Size of ST_CLIENT_CONNECTION_MANAGEMENT is 145
Size of CAsyncSocket is 8
Size of MAX_CLIENTS_PER_SERVER is 8
Size of MAX_SERVERS is 1
Size of ST_SERVERS_CLIENT_CONNECTION is 102
Size of ST_SERVER_CONNECTION_MANAGEMENT is 140
Size of SRawDataPacket is 1040


#endif


#endif

	uAppTimerTick = 0;	// crude tick to be used by CCM and SCM for keep alive messages

// Test thread creation and thread message posting
	m_pTestThread = (CTestThread *) AfxBeginThread(
										RUNTIME_CLASS(CTestThread),
										THREAD_PRIORITY_NORMAL,
										0,	// stack size
										0,	// create flag, run on start//CREATE_SUSPENDED,	// runstate
										NULL);	// security ptr

	Sleep(50);
	// Test posting a message to newly created thread
	if (m_pTestThread)
		i = m_pTestThread->PostThreadMessage(WM_USER_THREAD_HELLO_WORLD,1,5L);


	GetAllIP4AddrForThisMachine();
	InitializeServerConnectionManagement();	// crashes if ServerConnect called after ClientConnect
	Sleep(500);
	InitializeClientConnectionManagement();
	Sleep(200);
	// For debugging, write ini file to see changes
	if (gDlg.pTuboIni)
		gDlg.pTuboIni->SaveIniFile();

	goto WHILE_TARGET;	// for hybrid model with my SCM but YG's client connection, comment out the goto


WHILE_TARGET:
	// jeh code for Run()
	I_MSG_RUN sendBuf;
	I_MSG_NET *pNetBuf;
	pNetBuf = (I_MSG_NET *) &sendBuf;
	pNetBuf->MstrHdr.MsgId = NET_MODE;
	pNetBuf->bConnected[0] = 1;

	// testing only
	//int j;

	// for hybrid code of my server and Yangming's client connections, jump to the old while() loop
	// goto YG_RUN;
	// IF WE bypass the loop below, must not store Instrument data in linked lists.
	// change ServerSocket::OnReceive()


#ifndef YANMING_CODE
// JEH code for infinite while loop in Run()
//	while (1)
	while( ::WaitForSingleObject(m_hStop, 10) != WAIT_OBJECT_0 )
		{	// the jeh do nothing main loop
		Sleep(50);
		uAppTimerTick++;
		if (nDebugShutdown)	break;	// use quick watch to change value to 1 when stepping with debugger
//		void *pv;
		// attempt to make a connection attempted when the server was busy/non responsive

#if 1
		k = 1;	// debug  prevent accessing pClientConnection until configured
		while (k)
			{
			Sleep(1000);
			i = 0;
			for ( j = 0; j < MAX_CLIENTS_PER_SERVER; j++)
				{
				if (stSCM[i].pClientConnection[j] > 0)
					{
					if ( (stSCM[i].pClientConnection[j]->pSocket)	) // && (stSCM[i].nComThreadExited[J] == 0))
						{
						// Tell ReceiverList thread to flush the received data
						// WPARAM = j
						CWinThread * pThread = stSCM[i].pClientConnection[j]->pServerRcvListThread;
						//if (pThread)
						//	pThread->PostThreadMessage(WM_USER_FLUSH_LINKED_LISTS, j, 0);
						}
					}
				}
			}
#if 0

		for ( i = 0; i < MAX_SERVERS; i++)
			{
			//if (k == 0)	continue;
			if (stSCM[i].pSCM)
				{
				// for each server in the system, empyt the linked list created by the instruments
				// Server 0 by convention is the server receiving data packets from the instruments
				// This crashes the program when linked lists are being deleted when instruments go away
				// Remove critical sections and instead send thread messages to ServerSocketOwners to flush their data
				for ( j = 0; j < MAX_CLIENTS_PER_SERVER; j++)
					{
					k = (int) stSCM[i].pClientConnection[j];	// race condition of completing connection in debug
					if ( k == 0)
						{ 
						// no client connection
						//ReleaseInstrumentListAccess(j);
						continue;
						}
					if (stSCM[i].pSCM->m_pstSCM[i].nComThreadExited[j] == 1)
						{ 
						// no client connection
						// ReleaseInstrumentListAccess(j);
						continue;
						}

					if(k > 0)	// break for debug
						{	// empty the linked lists
						k = (int) stSCM[i].pClientConnection[j]->pSocket;	//debug
						if ( (stSCM[i].pClientConnection[j]->pSocket)	) // && (stSCM[i].nComThreadExited[J] == 0))
							{
							// Tell ReceiverList thread to flush the received data
							CWinThread * pThread = stSCM[i].pClientConnection[j]->pServerRcvListThread;
							if (pThread)
								pThread->PostThreadMessage(WM_USER_FLUSH_LINKED_LISTS, j, 0);
							Sleep(10);
#if 0
							stSCM[i].pClientConnection[j]->pSocket->LockRcvPktList();
							while (stSCM[i].pClientConnection[j]->pRcvPktList->GetCount())
								{
								pv = stSCM[i].pClientConnection[j]->pRcvPktList->RemoveHead();
								// normally would send data to PAG here???
								delete pv;
								}
							stSCM[i].pClientConnection[j]->pSocket->UnLockRcvPktList();
							// data to be sent
							stSCM[i].pClientConnection[j]->pSocket->LockSendPktList();
							while (stSCM[i].pClientConnection[j]->pSendPktList->GetCount())
								{
								pv = stSCM[i].pClientConnection[j]->pSendPktList->RemoveHead();
								// Normally would send commands to Instrument here
								delete pv;
								}
							stSCM[i].pClientConnection[j]->pSocket->UnLockSendPktList();
#endif
							}

						}	// empty the linked lists
					//ReleaseInstrumentListAccess(j);
					Sleep(50);

					}	// j loop
				}
			}
#endif
#endif
		}		// the jeh do nothing main loop

	// ASSUMINMG WE GOT HERE WITH THE DEBUGGER BY FORCING A CALL TO Shutdown()
	ShutDown();
		
	WSACleanup();  //  Free resources allocated by WSAStartup()	

	Sleep(500);
	ReportStatus(SERVICE_STOPPED);
#if 0
	return;

	// Now clean up the application
	TRACE(_T("Exit the inifinte loop in RUN\n"));
	if( m_hStop )
		{
		::CloseHandle(m_hStop);
		m_hStop = 0;
		}

	// This code taken from void CTscanDlg::OnCancel() - the PAG


	for ( i = 0; i < MAX_CLIENTS; i++)
		{
		if (pCCM[i])	delete pCCM[i];
		pCCM[i] = NULL;
		Sleep(10);
		}
	i = 14;
	s = _T("Here we are");
	//if (pCCM_SysCp)
	if (pCCM_PAG)
		{		delete pCCM_PAG;	pCCM_PAG = NULL;		}

	for ( i = 0; i < MAX_SERVERS; i++)
		{
		if (pSCM[i])
			{
			pSCM[i]->ServerShutDown(i);
			Sleep(200);
			delete pSCM[i];
			}
		pSCM[i] = NULL;
		Sleep(10);
		}

	Sleep(10);



	// WHILE vestiges of YG code may still be in project
	//goto YG_END;

#endif
#endif


//YG_END:
#if 0
	if( m_hStop )
		::CloseHandle(m_hStop);




//service_exit:


	WSACleanup();  //  Free resources allocated by WSAStartup()	



	ReportStatus(SERVICE_STOPPED);
#endif
	}	// void CServiceApp :: Run( DWORD, LPTSTR *)


void CServiceApp :: Stop() 
	{
	// report to the SCM that we're about to stop
	// Note that the service might Sleep(), so we have to tell
	// the SCM
	//	"The next operation may take me up to 11 seconds. Please be patient."
	//ReportStatus(SERVICE_STOP_PENDING, 11000);
	if( m_hStop )
		::SetEvent(m_hStop);
	Sleep(10);
	m_hStop = 0;
	CServiceApp::Shutdown();
	
	}
/**************************************** Nov 2012 ************************************/
/****** Add components from the GUI for running the server portion of the master  ******/


// c routine to convert CString to char array
// make sure the ptr to the char array is big enough to hold the converted string
// converted string is null terminated.
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

// global/universal time string generator
CString GetTimeString(void)
	{
	CString s = GetTimeStringPtr();
	return s;
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

// A ServicApp class memeber to initialize the server portion of the master code.
// The Master process/computer which acts as a server to its connected instruments 
// is assumed to always have the same IP address and port listening number no matter
// how many Master computers there may be. Thus the instrument does not have to have
// special knowledge about the Master or in some way be configured for a specific
// master instrument. This means (1) that multiple master will have to be implemented
// as individual machines. (2) Each master will connect to its instruments thru a switch
// and (3) all the masters will connect to the User interface (PAG - Phased Array GUI) thru
// another switch. Thus for multiple masters, each master will have to have a NIC which 
// faces the instruments and another NIC which faces the PAG.

// Initialize the Server subsystems of the Phased Array Master.
// This windows program receives inspection data from the Instruments of which there
// may be several. 
//
/*
Consider some hypothetical cases.
1 Pc with 1 Nic servicing 16 instruments. Assume we want a "server" class instance for every 8 instruments.
This would mean we would have 2 instances of a server in one machine using one NIC. This would require
a different listening port for each server. Clearly, all the TCP/IP traffic would have to go thru one NIC.
Thus there is probably be no advantage to doing this. We could just as well put all 16 instrument into one "server"instance.

Consider 1 PC with 2 NIC's. The NIC's would have to have different IP addresses. This would require the instrument
to know what the IP address of their NIC was. This introduces configuration complexities into the deployment of 
instruments and would likely lead to problems in the field when personnel begin to swap boards/systems when trouble
shooting a problem.

Consider 2 PC's each with 1 NIC. Both PC's can present the same IP address and port number to their instrument as long
as the 16 instruments are in 2 groups, each group connected thru a switch to its PC and NIC. The processing load
and network traffic are cut in half by having duplicate hardware. There is no configuration required of the 
instrument since the IP address and port are the same for both groups of instruments. This last scenario means that
on a given machine, there is only one server involved. Hence MAX_SERVERS = 1.

*/
// Adapted from Pharse Array GUI TruscanDlg.cpp code.
//
void CServiceApp::InitializeServerConnectionManagement(void)
	{
	int i, j;
	CString s;
	UINT uPort;
	int nError;
	i = _MSC_VER;	// 1900

	if (gnMaxServers > MAX_SERVERS)
		{
		TRACE1("gnMaxServers = %d greater than MAX_SERVERS\n", gnMaxServers);
		gnMaxServers = MAX_SERVERS;
		}
	for ( i = 0; i < gnMaxServers; i++)
		{

		if (pSCM[i])	continue;	// instance already exists

		switch (i)
			{
		case 0:		// There are multiple client instrument looking for the PAM server. This is the only server
					// for instruments in this application.
			pSCM[i] = new CServerConnectionManagement(i);
			j = sizeof(CServerConnectionManagement);
			if (pSCM[i])
				{
				s = gServerArray[i].Ip;			// a global static table of ip addresses define by an ini file
				pSCM[i]->SetServerIP4(s);		// _T("192.168.10.10"));
				uPort = gServerArray[i].uPort;
				pSCM[i]->SetServerPort(uPort);	// 7502);
				pSCM[i]->SetServerType(eInstrument);
				pSCM[i]->SetClientBaseIp(gServerArray[i].ClientBaseIp);
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
					//delete pSCM[i];
					pSCM[i] = NULL;
					}
				}
			break;
		default:
			pSCM[i] = NULL;
			break;
			}
		}
		s.Format(_T("\nSERVER CONNECTION MANAGEMENT has completed for MAX_SERVERS = %d \n"), MAX_SERVERS);
		TRACE(s);
	}

// undo the initialization
int CServiceApp :: KillServerConnectionManagement(int nServer)
	{
	int nError, nResult;
	int i;
	CString s;

	nResult = 0;
	if (gnMaxServers > MAX_SERVERS)
		{
		TRACE1("gnMaxServers = %d greater than MAX_SERVERS\n", gnMaxServers);
		gnMaxServers = MAX_SERVERS;
		}
	for ( i = 0; i < gnMaxServers; i++)
		{

		if (pSCM[i])
			{	// kill this server' listen thread
			nError = pSCM[i]->StopListenerThread(i);
			if (nError)
				{
				s.Format(_T("Failed to stop listener Thread[%d], ERROR = %d\n"), i, nError);
				TRACE(s);
				//delete pSCM[i];	// delete anyway
				pSCM[i] = NULL;
				nResult |= nError;
				}
			}
		}
	return nResult;
	}


// Call this routine once during OnInitDialog and possibly avoid calling GetIPv4(void)
// It finds all the host, ie, network interfaces attached to this machine
// including wireless and virtual if a virtual machine is installed.
// From this list we can determine what servers can be reached by which NIC's
// assuming we are not using routers.
// -- copied from TScanDlg.cpp
void CServiceApp::GetAllIP4AddrForThisMachine(void)
	{
	USES_CONVERSION;
	CString s;
	char t[1024];

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

	sComputerName = T2A(sFQDN);


	// make sure we're searching for something real
    if (sComputerName.IsEmpty())
		return;		// theIP;

    //struct sockaddr_in theHost;	// structure used in the conversion
	struct in_addr addr;		// jeh from help system
	HOSTENT * hostent;			// structure returned by gethostbyname
	int i;
	for ( i = 0; i < 20; i++) sThisMachineIP4Addr[i].Empty();
	i = 0;

    // do the lookup
	hostent = gethostbyname(CT2A(sComputerName));
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
#ifdef _DEBUG
				CstringToChar(s,t);
				printf("Host IP4 addr = %s\n",t);
#else
				t[0] = 0;
#endif
				}
			uThisMachineIP4Addr[i]		= 0x0100007f;
			sThisMachineIP4Addr[i++]	= _T("127.0.0.1");
			}
		}
	}

// Start the system which connects client TCP/IP routines in this application to servers.
// There is one instance of the ClientConnectionManagement class for each client connection
// The primary connection for the PAM is to the Phased Array GUI (PAG) which is a server
// to each Phased Array Master machine in the system
//
// Adapted from PAG which is called TruscanDlg.cpp
// Redefine the IP finding routines here to look like the scheme used by the
// ServerConnectionManagement system
//
void CServiceApp::InitializeClientConnectionManagement(void)
	{
	int i, j;
#if 1
	// Instantiate all CCM instances for as many client connections as are going to be supported
	// Convention is for case 0 to be Phased Array GUI - PAG
	// Convention is for case 1 to be the PLC
	// Assume IP addresses will be assigned via ini file or array. Set without dhcp searches
	// 	All these connections are tcp/ip clients and the other end is a tcp/ip server.
	CString sClientIP,  sServerIp, sServerName, s;	// sServerName use the url for this server
	UINT uServerPort;

	for ( i = 0; i < MAX_CLIENTS; i++)
		{
		if (pCCM[i])	continue;	// instance already exists

		switch (i)
			{
		case 0:		// assume connecting to PAG

#if 0
			if ( 0 == FindClientSideIP(i) )
				{	// find client side connection for 1st connection... probably the same IP for all 
					// client side connections unless more than one NIC
				break;
				}
			FindServerSideIP(i);
#endif
			sClientIP = GetClientIP(i);
			sServerIp = GetServerIP(i);
			sServerName = GetServerName(i);
			uServerPort = GetServerPort(i) & 0xffff;	// port on the PAG server that we will try to connect to
			// Make a specific child class of CCM to handle the Phased Array GUI - PAG
			pCCM_PAG = (CCCM_PAG *) new CCCM_PAG(i);
			j = sizeof(CCCM_PAG);
			if (NULL == pCCM_PAG)
				{
				TRACE1("pCCM_PAG[%d] is NULL\n", i);
				break;
				}
			if (pCCM_PAG->m_pstCCM == NULL) 
				{
				TRACE1("pCCM_PAG[%d]->m_pstCCM is NULL\n", i);
				break;
				}
			//pCCM[i] = pCCM_SysCp; causes problem on shut down

			pCCM_PAG->UniqueProc();	// JUST FOR DEBUG
			pCCM_PAG->SetSocketName(_T("CCM_PAG"));
			//pCCM_PAG->SetWinVersion(theApp.m_iWinVer);
			pCCM_PAG->SetClientIp(sClientIP);
			pCCM_PAG->SetServerIp(sServerIp);
			pCCM_PAG->SetServerName(sServerName);	// url of server, e.g. srvhouapp67
			pCCM_PAG->SetServerPort(uServerPort);
			
						
			pCCM_PAG->m_pstCCM->nReceivePriority	= THREAD_PRIORITY_ABOVE_NORMAL;
			pCCM_PAG->m_pstCCM->nSendPriority		= THREAD_PRIORITY_BELOW_NORMAL;
			pCCM_PAG->m_pstCCM->nCmdProcessPriority	= THREAD_PRIORITY_BELOW_NORMAL;
			pCCM_PAG->CreateReceiveThread();		Sleep(50);
			pCCM_PAG->CreateSendThread();			Sleep(50);
			pCCM_PAG->InitReceiveThread();			Sleep(50); 
			// causes CClientCommunicationThread::InitTcpThread(WPARAM w, LPARAM lParam) to run
			pCCM_PAG->InitSendThread();				Sleep(50);
			pCCM_PAG->CreateCmdProcessThread();		Sleep(50);

#if 0

			if (pCCM_SysCp->m_pstCCM->sClientIP4.GetLength() > 6)	// copy found IP for client into identity struct
				pCCM_SysCp->SetClientIp(pCCM_SysCp->m_pstCCM->sClientIP4);
			if (pCCM_SysCp->m_pstCCM->sServerIP4.GetLength() > 6)	// copy found IP for server into identity struct
				pCCM_SysCp->SetServerIp(pCCM_SysCp->m_pstCCM->sServerIP4);

			// Set thread priorities for send and receive threads
			pCCM_SysCp->m_pstCCM->nReceivePriority	= THREAD_PRIORITY_ABOVE_NORMAL;
			pCCM_SysCp->m_pstCCM->nSendPriority		= THREAD_PRIORITY_BELOW_NORMAL;
			pCCM_SysCp->CreateReceiveThread();		Sleep(50);
			pCCM_SysCp->CreateSendThread();			Sleep(50);
			pCCM_SysCp->InitReceiveThread();		Sleep(50);
			pCCM_SysCp->InitSendThread();			Sleep(50);
			break;	// case 0


		case 1:		// assume connectiong to THE PLC
			// FindClientSideIP(i);	// assume syscp found the ip for this client for all other servers. If not
			// craft CODE in FindClientSideIP to find another ip address to link with the database.
			if (stCCM[0].sClientIP4.GetLength() > 6)
				sClientIP = stCCM[0].sClientIP4;	// use case 0 for syscp
			else
				{	// try something else. If that fails, abort since we can't hook up with the data base
				}

			if (!FindServerSideIP(i))
				{
				TRACE("Could not find server IP for Database.. we are toast\n");
				break;
				}
			sServerIp = stCCM[i].sServerIP4;

#endif

			break;	// case 1

		default:
			pCCM[i] = NULL;
			break;

			}
		}	// for ( i

#endif	
	s.Format(_T("\nCLIENT CONNECTION MANAGEMENT has completed for MAX_CLIENTS = %d \n"), MAX_CLIENTS);
	TRACE(s);

	}

// When it is time to create the thread that services the client's data received into the RcvPacketList
// Call back to the top level of the application and choose the correct thread type based on knowing 
// the function of the server itself.
// This call returns the thread ptr of the thread created.
// The thread created will have custom processing of the RcvPacketList based on which server/client pair
// is being processed. For Phased Array Master, the primary client on the other end is the PA Instrument.
// It sends inspection data packets which are processed in a manner dictated by the Message ID of the packet.
//
//ifdef SERVER_RCVLIST_THREADBASE		
// DEFINE in project defines under C/C++ | Preprocessor
CServerRcvListThreadBase* CServiceApp::CreateServerReceiverThread(int nServerNumber, int nPriority)
	{
	CString s;
	CServerRcvListThreadBase *pThread = NULL;

	if (nServerNumber >= MAX_SERVERS)
		{
		s.Format(_T("CServiceApp::CreateReceiverThread invalid server index = %d\n"), nServerNumber);
		TRACE(s);
		return pThread;	// NULL is invalid for a thread ptr
		}
	switch (nServerNumber)
		{
		// AfxBeginThread(RUNTIME_CLASS (), priority, stack_size, suspended-or-run, security)
	case 0:
		// Case 0 is instrument data serviced by a custom thread class named CServerRcvListThread.
		// Other types of service offered to other types of clients would likely have a different
		// custom thread to process the data which came from the client.
		pThread = 
			
				(CServerRcvListThreadBase *) AfxBeginThread(
					RUNTIME_CLASS (CServerRcvListThread),
					nPriority,	0, CREATE_SUSPENDED, NULL);		// normally normal priority
		break;
	default:

		break;
		}
	return pThread;
	}

// Data comes to the PAM (Phased Array Master) from instruments. The data is processed into new messages types
// and forwarded to the PAG (Phased Array GUI). INPUT data to PAM comes thru a sockets associated with the 
// Server Connection Management class. In particular, this data is associated with SCM instance 0.
// PAM's connection to the PAG is via a Client Connection Management socket, specifically CCM[0] instance
// CCM[0] instance has a child class of CCM, namely CCM_PAG
// NOTE!!! PamSendToPag DOES NOT DELETE THE MEMORY pointed to by pBuf
// In the FakeData generation whicH calls PamSendToPag pBuf is deleted after the return to the fake data generator
void CServiceApp::PamSendToPag(void *pBuf, int nLen)
	{
	CString s;
	int i = ePAM_Client_Of_PAG_Server;	// normally 0
	if (nLen < 1) 
		{
		TRACE(_T("ERROR - CServiceApp::PamSendToPag(nLen < 1)\n"));
		return;
		}
	ASSERT(i<=MAX_CLIENTS);
//	CClientConnectionManagement *pccm = pCCM[i];
	CClientConnectionManagement *pccm = pCCM_PAG;
	if ( NULL == pccm)
		{
		TRACE(_T("CServiceApp::PamSendToPag pccm is NULL\n"));
		return;
		}
	CClientSocket *pSocket = pccm->GetSocketPtr();
	ASSERT(pSocket);
	ASSERT(pBuf);
#if 0
	stSEND_PACKET *pNew = (stSEND_PACKET *) pBuf;
	if (pNew->nLength != nLen)
		{
		s.Format(_T("CServiceApp::PamSendToPag nLen != nLength\n"), nLen, pNew->nLength);
		TRACE(s);
		}
	i = pSocket->Send(&pNew->Msg, pNew->nLength, 0);
#endif
	i = pSocket->Send(pBuf, nLen, 0);
	if ( i != nLen)
		{
		s.Format(_T("CServiceApp::PamSendToPag requested to send %d bytes, sent %d\n"), nLen, i);
		TRACE(s);
		}

	}

// Input the client number for the instrument making the request
// Client numbers are derived from the base IP address of the client range
// and as of 2016-09-16 only range from 0-7
// The client will still have to access its own linked list thru its own local critical sections
//
int CServiceApp::GetInstrumentListAccess(int nInstNumber)
	{
	if (( nInstNumber < 0) || (nInstNumber >= MAX_CLIENTS_PER_SERVER))
		{
		TRACE1("Invalid GetInstrumentListAccess number = %d\n", nInstNumber);
		return 0;
		}
	EnterCriticalSection(pAppInstAccess[nInstNumber]);
	return 1;
	}

void CServiceApp::ReleaseInstrumentListAccess(int nInstNumber)
	{
	if (( nInstNumber < 0) || (nInstNumber >= MAX_CLIENTS_PER_SERVER))
		{
		TRACE1("Invalid ReleaseInstrumentListAccess number = %d\n", nInstNumber);
		return;
		}
	LeaveCriticalSection(pAppInstAccess[nInstNumber]);
	}


/**************************************** Nov 20120 ************************************/
/**************************************** Nov 20120 ************************************/

/* CheckKey - Thread to wait for a keystroke, then clear repeat flag. */
UINT CheckKey( void *dummy )
{

	while ( _getch() != 'c')
	{
		;
	}
    repeat = 0;    /* _endthread implied */
	_endthread();

	return 0;
}



/********** Control inspection process *************************************
*  Respond to pipe in and out
*/
void Inspection_Process_Control()
{
#if 0
	{
	case 0:		/* pipe is not present */
		switch (g_nMotionBus & 0x00000001)     /* look at the new pipe present status */
		{
		case 0:		/* pipe is not present */
			break;

		case 1:		/* pipe is present. */
			PipeInProcess();
			break;
		}
		break;

	case 1:		/* pipe is present */
		switch (g_nMotionBus & 0x00000001)     /* look at the new status of the two stations */
		{
		case 0:		/* pipe is present */
			PipeOutProcess();
			break;

		case 1:		/* pipe is present */
			break;
		}
		break;

	}

#endif
}



/**********************************************Build Image Buf***************************************************/


/*******************************************************************
* Use static variable to hold current inspection mode:
*
*  RUN_MODE, CAL_MODE, PKT_MODE, NOP_MODE
*/


int FindWhichSlave(int nChannel)
{	return 0;		}//nSlave;


int FindSlaveChannel(int nChannel)
{	return 0;	}//nSlaveCh;


int FindDisplayChannel(int nArray, int nArrayCh)
{	return 0;	}//nDispCh;

