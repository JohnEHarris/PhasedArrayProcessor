// ServiceApp.cpp: implementation of the CServiceApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServiceApp.h"
#include "winsock2.h"
#include "math.h"

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
#include "InstMsgProcess.h"
#include "RunningAverage.h"
#include "HwTimer.h"

CServiceApp theApp;

UINT CheckKey( void *dummy );
UINT ClientSocketInit(void *dummy);
int readn( int fd, char *bp, int len);
UINT tRcvrProcessMmiMsg(void *dummy);
UINT ProcessMmiMsg(void *dummy);
UINT tcpServerWorkTask(void *pSlave);
UINT tInstMsgProcess (void *pCInstMsgProcess);	// swap this for tcpServerWorkTask above
UINT tcpServerWorkTask_WD(void *pSlave);
BOOL ServerSocketInit();
UINT ServerSocketThread(void *dummy);
BOOL ServerSocketInit_WallDisplay();
UINT ServerSocketThread_WallDisplay(void *dummy);
UINT tSendRawFlawToMMI(void *dummy);
UINT tWriteWallDataToFile (void *dummy);
UINT tSendWallDisplayMsg(void *dummy);


void ShutDownSystem();

BOOL repeat = TRUE;     /* Global repeat flag and video variable */
int  g_nServerSocket=-1;
int  g_nServerSocketWD=-1;
int  g_nMmiSocket=-1;
int  g_nRunClientSocketInitThread = 1;
int  g_nRunRcvrProcessMmiMsgThread = 1;
int  g_nRunProcessMmiMsgThread = 1;
int  g_nRunSendRawFlawToMMIThread = 1;
int  g_nRunWriteWallDataToFileThread= 1;
int  g_nRunSendWallDisplayMsgThread = 1;

// Encapsulate the processing of data from inspection instruments in this class
CInstMsgProcess* g_pInstMsgProcess[32];

CWinThread* g_pThreadClientSocketInit;
CWinThread* g_pThreadRcvrProcessMmiMsg;
CWinThread* g_pThreadProcessMmiMsg;
CWinThread* g_pThreadServerSocket;
CWinThread* g_pThreadServerSocket_WD;
CWinThread* g_pThreadSlave[MAX_SHOES];	// a server task for every client instrument employing tcpServerWorkTask
CWinThread* g_pThreadSendRawFlawToMMI;
CWinThread* g_pThreadWriteWallDataToFile;
CWinThread* g_pThreadWallDisplay;
CWinThread* g_pThreadSendWallDisplayMsg;

int  g_nSlave[NUM_OF_SLAVES];
int  g_nWD=0;
int  g_hPipeMmiMsg[2];
int  g_hPipeWallDisplay[2];
enum PIPES { READ, WRITE }; /* Constants 0 and 1 for READ and WRITE */

/*  Begin Globals */

int    g_SocketSlave[NUM_OF_SLAVES];    /* TCP sockets for communicating with slaves */
int    g_socketWallDisplay = -1;    /* TCP socket for communicating with wall display program */
int    g_nRcvrIdataCnt;
int    g_nAuxClkIntCnt = 0;
int    g_nAuxClkIntCnt2 = 0;
float  g_fTimePerTurn = 0.5f;       /* unit: second */
float  g_fTimePerInch = 0.25f;      /* unit: second */
int    g_nLostIdataCnt = 0;
BYTE   g_bConnected[NUM_OF_SLAVES+1];   /* network connection status.  0: not connected, 1: connected.  Master in 0, Slave1 in 1, etc. */
BYTE   g_nBoardRevision[NUM_OF_SLAVES+1][MAX_BOARD_PER_INSTRUMENT];
char   g_SlaveCodeRev[40];
BYTE   g_AdiStatus;
int    g_nNoMmiReplyCnt = 0;
int    g_nXloc_S2 = 0;              /* X location of Station 2 */
int    g_nTick = 0;                 /* Angular location in ticks (180 ticks for one full rotation */
WORD   g_nPeriod;		    /* period of rotation in 0.1 ms */
short  g_nVelocityDt;               /* delta t to travel 4 inches in 1 ms clocks */
WORD   g_nNextWindow = 0;           /* next available inspect window for display */
WORD   g_nStation1Window = 0;       /* the inspect window to which the station 1 Idata is sent */
WORD   g_nStation2Window = 0;       /* the inspect window to which the station 2 Idata is sent */
DWORD  g_nStation1JointNum = 0;
DWORD  g_nStation2JointNum = 0;
DWORD  g_nOldMotionBus = 0;
BOOL   g_bStartOfRevS1 = FALSE;
BOOL   g_bStartOfRevS2 = FALSE;
C_MSG_ALL_THOLD  g_AllTholds;
C_MSG_NC_NX g_NcNx;
WALL_COEF  g_WallCoef;
BOOL   g_bRunCalJoint = FALSE;
DWORD  g_nNextRealJointNum = 1;
DWORD  g_nNextCalJointNum = 5001;
int    g_nMaxXSpan;                 /* distance between the leftmost transducer and the rightmost transducer */
BYTE   g_nPlcOfWho = 0;             /* 0=Master, 1=Instrument 1, etc. */
DWORD  g_dwPlc[4];
int    g_nXscale = 900;
int    gChannel = 0;
int    gGate = 0;
BYTE   g_bBcastAscan = 0;
BOOL   g_bAnyShoeDown = FALSE;

BOOL   g_b20ChnlPerHead = FALSE;
WORD   g_nRecordWallData=0;
BOOL   g_bShowWallDiff = FALSE;

BOOL   g_bSendRawFlawToMMI = FALSE;

WORD   g_nPulserPRF = 1000;
WORD   g_nMaxWallWindowSize = 10;
float  g_WallDropTime = 0.1f;       /* initialized to 0.1 seconds */
int    g_NumberOfScans = 3;
int	   g_ArrayScanNum[NUM_OF_SLAVES];
int    g_SequenceLength[NUM_OF_SLAVES];
int    g_nPhasedArrayScanType[NUM_OF_SLAVES];
int    g_nInspectMode = CAL_MODE;

int  g_nPipeStatus = PIPE_NOT_PRESENT;     /* temporary */
DWORD g_nJointNum = 1;
int  g_nXloc = 0;             /* temporary */
int  g_nOldXloc = 0;
WORD   g_nMotionBus = 0;
float  g_fMotionPulseLen = 0.506329f;
int    g_nShowWallBars = 1;

CSCAN_REVOLUTION g_RawFlaw[2];
int    g_nRawFlawBuffer = 0;  /* the buffer to write the raw flaw to */

RAW_WALL_HEAD       g_RawWallHead;
WALL_REVOLUTION     g_RawWall[NUM_MAX_REVOL];
RAW_WALL_HEAD       g_RawWallHeadCopy;
WALL_REVOLUTION     g_RawWallCopy[NUM_MAX_REVOL];
int  g_nWallRevCnt;
int  g_nXlocRevStart;         /* starting X location of the revolution */
int  g_nWallAscanCnt[NUM_WALL_CHANNEL*2];
void InitRawWallBuf(void);
JOB_REC  g_JobRec;
BOOL g_bWriteWallDataFile=FALSE;

I_MSG_CAL  CalBuf;
I_MSG_RUN ImageBuf[IMAGE_BUF_DIM];

int nBufin, nBufout;			/* index from 0-IMAGE_BUF_DIM - 1 */
int nBufcnt;				/* image buffer management */
int nPreviousX =0;
int nFlush;
int nMaxX;
/*  End Globals */


/*************** BEGIN FUNCTION PROTOTYPES ******************/
					
void FlushImageBufArray(void);
BOOL SendSlaveMsg(int nWhichSlave, MMI_CMD *pSendBuf);
void PipeInProcess (void);
void PipeOutProcess (void);
void SetGetInspectMode_M (int nSetGet/* 0=SET, 1=GET */, int *nMode, int *nMotionTime);
void Inspection_Process_Control();

void SetGetChannelCfg (int nSetGet /* 0=SET, 1=Get */, CHANNEL_CONFIG2 *pChnlCfg, int nSlave);
void SetGetSiteDefaults (int nSetGet /* 0=SET, 1=Get */, SITE_SPECIFIC_DEFAULTS *pSiteDef);
void InitImageBufArray(void);
int GetMaxXOffset(void);
int GetMinXOffset(void);
int GetMaxXSpan(void);

void NiosRawData_to_PeakData(SRawDataPacket *pInspData, PEAK_DATA *pPeakData, int nInspectMode);
void NC_Process (BYTE *RcvrSequence, BYTE *AmpID, BYTE *AmpOD, BYTE *FlawID, BYTE *FlawOD, CHANNEL_INFO *ChannelInfo, int nSlave, int nTick, int nSeq);
void NX_Process (BYTE *RcvrSequence, WORD *InputWallReading, WORD *WallMin, WORD *WallMax, float ascan_delta_t, CHANNEL_INFO *ChannelInfo, BOOL bFirstAscan, int nSlave, int nTick, int nXloc, int nSeq);

BOOL PipeIsPresent() {return g_nPipeStatus;};
BOOL SendMmiMsg(int socketFD, I_MSG_RUN *pSendBuf);
BOOL BoxIsEnabled(int nBox) { return TRUE;};

void BuildCalMsg(UDP_SLAVE_DATA *pSlvData, int nSlave);
int	BuildImageMap(UDP_SLAVE_DATA *pSlvData, int si, BOOL *bStartOfRev);
I_MSG_RUN* GetNextImageBuf(void);
BOOL GetNextRealTimeImageBuf(I_MSG_RUN** pIBuf);
void ComputeEcc(I_MSG_RUN *pRun, C_MSG_ALL_THOLD *pThold);
void SendPipeLocation(int nStation, SITE_SPECIFIC_DEFAULTS *pSiteDefault);
void SendRawFlawToMMI();
int FindWhichSlave(int nChannel);
int FindSlaveChannel(int nChannel);
int FindDisplayChannel(int nArray, int nArrayCh);
void ComputeTranFocusDelay(float thickness, float zf_value, float water_path, float incident_angle, WORD *td);

/*************** END FUNCTION PROTOTYPES ******************/					




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServiceApp::CServiceApp()
	: CNTService(TEXT("PhasedArray_Master"), TEXT("Phased Array Master"))
	, m_hStop(0)
{
	int i;

	for (i=0; i<NUM_OF_SLAVES; i++)
	{
		g_SocketSlave[i] = -1;   /* initialized to an invalid socket. */
		g_nSlave[i] = i;
		g_nWD = 0;
		g_bConnected[i+1] = 0;
	}

	for ( i = 0; i < 32; i++)	g_pInstMsgProcess[i] = NULL;
//	g_pInstMsgProcess[0] = new CInstMsgProcess(0);	// DEBUGGING
	g_NcNx.Long[0] = 1;
	g_NcNx.Long[0] = 1;
	g_NcNx.Tran[0] = 1;
	g_NcNx.Tran[0] = 1;
	g_NcNx.Oblq1[0] = 1;
	g_NcNx.Oblq1[0] = 1;
	g_NcNx.Oblq2[0] = 1;
	g_NcNx.Oblq2[0] = 1;
	g_NcNx.Oblq3[0] = 1;
	g_NcNx.Oblq3[0] = 1;
	g_NcNx.Lamin[0] = 1;
	g_NcNx.Lamin[0] = 1;
	g_NcNx.Wall[0] = 1;

#if 0
	// debug running average tested for 1,2,4,8 nx values and works
	// hwtimer ticks at 3.58 MHz
	// for n== 1, 20 loops = 10 ticks  less than 3 uSec
	// for n== 2, 20 loops = 10 ticks
	// for n== 4, 20 loops = 10 ticks
	// for n== 8, 20 loops = 12 ticks  12/3.58 = 3.4 uSec on Dual Pentium 1.8 Ghz system
#endif
#if 0
	CHwTimer *pTimer;
	pTimer = new CHwTimer();
	int nDeltaT[20];
	WORD wResult[20];
	WORD dbgWall = 100;
	CRunningAverage *pRunAvg = new CRunningAverage(8);
	pRunAvg->SetDropOutThreshold(30);	// allow us to see the wall filtered value roll off
	pRunAvg->SetLowWallLimit(0);		// ignore the lower limit for this test
	pTimer->HwStartTime();
	for ( i = 0; i < 20; i++)
		{
		wResult[i] = pRunAvg->Input(dbgWall);
		if (i > 10) dbgWall = 1;
		}
	pTimer->HwStopTime();

	pTimer->GetDeltaTimeArray(nDeltaT);
	pRunAvg->DefaultLowWallLimit();
#endif

	for (i=0; i<MAX_SHOES; i++)
	{
		g_WallCoef.fWallSlope[i] = 1.0f;
		g_WallCoef.WallOffset[i] = 0;
	}

	for (i=0; i<NUM_OF_SLAVES; i++)
	{
		g_ArrayScanNum[i] = 3;
		g_SequenceLength[i] = 24;
		g_nPhasedArrayScanType[i] = 2;
	}

}

CServiceApp::~CServiceApp()
{
int i;
for ( i = 0; i < 32; i++)
	{
	if ( g_pInstMsgProcess[i])	delete g_pInstMsgProcess[i];
	g_pInstMsgProcess[i] = NULL;
	}
}


BOOL CServiceApp :: InitInstance() {
	RegisterService(__argc, __argv);
	return FALSE;
}


void CServiceApp :: Run( DWORD, LPTSTR *) 
{		// args not used in this small example
	// report to the SCM that we're about to start
	ReportStatus(SERVICE_START_PENDING);
	
	m_hStop = ::CreateEvent(0, TRUE, FALSE, 0);

	// You might do some more initialization here.
	// Parameter processing for instance ...
	
	// report SERVICE_RUNNING immediately before you enter the main-loop
	// DON'T FORGET THIS!
	ReportStatus(SERVICE_RUNNING);

#if 0
/*
	FILE *ipfile;
	CString sip;
	ipfile = fopen("ip_address.txt","r");
	if (ipfile ==  NULL)
		goto service_exit;

	//read the ip address and close the file
	fscanf(ipfile, "%s", sip);
	//if ( (sip != "192.168.8.10") && (sip != "192.168.8.200") )
		//goto service_exit;

	fclose (ipfile);
*/
#endif

#if 0
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
#if 0
Debugging Phased Array Master.
Size of PACKET_STATS is 20
Size of UDP_CMD_HDR is 72
Size of SLAVE_HDR is 116
Size of UDP_CMD is 916
Size of UDP_SLAVE_DATA is 212
Size of INST_DATA is 1282
Size of IPXHEADER is 30
Size of PACKET_DATA is 1328
Size of INST_CMD_DATA is 78
Size of MMI_CMD is 872
Size of UT_SHOE is 80
Size of INSP_HDR is 72
Size of UT_INSP is 1120
Size of PEAK_DATA is 96
Size of I_MSG_RUN is 1264
Size of I_MSG_CAL is 1104
Size of I_MSG_PKT is 408
Size of I_MSG_NET is 1248
Size of SITE_SPECIFIC_DEFAULTS is 108

#endif


#endif

	if ( _pipe( g_hPipeMmiMsg, sizeof(MMI_CMD) * 1000, O_BINARY ) == -1 )
	{
		//printf("Failed creating pipe.\n");
		goto service_exit;
	}

	if ( _pipe( g_hPipeWallDisplay, 1000 * 1000, O_BINARY ) == -1 )
	{
		//printf("Failed creating pipe.\n");
		goto service_exit;
	}

	InitRawWallBuf();

	WSADATA wsaData;
	int rv, i;

	rv = WSAStartup(0x0202, &wsaData);   //  Must be 1st fcn call
	if (rv)
	{ 
		//ErrMsgSockStartup(rv);
		//AfxMessageBox(_T("This program will not run without Winsock 2"));
		//printf("Can't start Winsock 2.\n");
		goto service_exit;
	}

    /* Launch CheckKey thread to check for terminating keystroke. */
    //_beginthread( CheckKey, 0, NULL );
	// This program is a server to the inspection machine clients

repeat_serverinit:

	if ( ServerSocketInit() )
		g_pThreadServerSocket = AfxBeginThread(ServerSocketThread, NULL, THREAD_PRIORITY_NORMAL);
	// if we can create a server socket, we will start a server thread to accept connections from 
	// the instrument pulser-receiver clients
	else
	{
		//printf("Failed ServerSocketInit.\n");
		//goto service_exit;
		TRACE(_T("ServerSocketInit() failed\n"));
		_tprintf(TEXT("ServerSocketInit() failed\n"));

		::Sleep(500);
		goto repeat_serverinit;
	}

// This program is a server to the program to display wall readings as a bar chart (WallDisplay.exe)
repeat_serverinit_WD:

	if ( ServerSocketInit_WallDisplay() )
		g_pThreadServerSocket_WD = AfxBeginThread(ServerSocketThread_WallDisplay, NULL, THREAD_PRIORITY_NORMAL);
	else
	{
		//printf("Failed ServerSocketInit.\n");
		//goto service_exit;
		::Sleep(500);
		goto repeat_serverinit_WD;
	}

	g_nRunClientSocketInitThread = 1;
	g_pThreadClientSocketInit = AfxBeginThread(ClientSocketInit, NULL, THREAD_PRIORITY_NORMAL);

	g_nRunRcvrProcessMmiMsgThread = 1;
	g_pThreadRcvrProcessMmiMsg = AfxBeginThread(tRcvrProcessMmiMsg, NULL, THREAD_PRIORITY_NORMAL);

	g_nRunProcessMmiMsgThread = 1;
	g_pThreadProcessMmiMsg = AfxBeginThread(ProcessMmiMsg, NULL, THREAD_PRIORITY_NORMAL);

	g_nRunSendRawFlawToMMIThread = 1;
	g_pThreadSendRawFlawToMMI = AfxBeginThread(tSendRawFlawToMMI, NULL, THREAD_PRIORITY_NORMAL);

	g_nRunWriteWallDataToFileThread = 1;
	g_pThreadWriteWallDataToFile = AfxBeginThread(tWriteWallDataToFile, NULL, THREAD_PRIORITY_NORMAL);

	g_nRunSendWallDisplayMsgThread = 1;
	g_pThreadSendWallDisplayMsg = AfxBeginThread(tSendWallDisplayMsg, NULL, THREAD_PRIORITY_NORMAL);

	int rc;
	I_MSG_RUN sendBuf;
	I_MSG_NET *pNetBuf;
	pNetBuf = (I_MSG_NET *) &sendBuf;
	pNetBuf->MstrHdr.MsgId = NET_MODE;
	pNetBuf->bConnected[0] = 1;

	// enter main-loop
	// If the Stop() method sets the event, then we will break out of
	// this loop.
	while( ::WaitForSingleObject(m_hStop, 10) != WAIT_OBJECT_0 ) 
	{
		// popup a small message box every 10 seconds
		//::MessageBox(0, TEXT("Hi, here is your very first MFC based NT-service"), TEXT("MFC SampleService"), MB_OK);
		if (g_nMmiSocket >= 0)
		{
			for (i=1; i<NUM_OF_SLAVES+1; i++)
			{
				pNetBuf->bConnected[i] = g_bConnected[i];
			}

			pNetBuf->InspHdr.NextJointNum = g_nJointNum;

			rc = send( g_nMmiSocket, (char *) &sendBuf, sizeof(I_MSG_RUN), 0 );
			if ( rc <= 0 )
			{
				//AfxMessageBox(_T("send call failed.\nMessage ID = "+str));
				//return FALSE;
			}

			for (i=0; i<NUM_OF_SLAVES; i++)
			{
				g_bConnected[i+1] = 0;
			}
		}

		::Sleep( 3000 );
		//AfxMessageBox(sip);
	}	// while waiting for stop

	/**********  SHUT DOWN SEQUENCE ************/


	if( m_hStop )
		::CloseHandle(m_hStop);

	if (g_nRunClientSocketInitThread > 0)
	{
		g_nRunClientSocketInitThread = 0;
		::WaitForSingleObject(g_pThreadClientSocketInit->m_hThread, INFINITE);
		//AfxMessageBox("Exit ClientSocketInit Thread successfully.");
	}

	if (g_nRunRcvrProcessMmiMsgThread > 0)
	{
		g_nRunRcvrProcessMmiMsgThread = 0;
		closesocket(g_nMmiSocket);
		g_nMmiSocket = -1;
		::WaitForSingleObject(g_pThreadRcvrProcessMmiMsg->m_hThread, 10000);
		//AfxMessageBox("Exit tRcvrProcessMmiMsg Thread successfully.");
	}

	if (g_nRunProcessMmiMsgThread > 0)
	{
		g_nRunProcessMmiMsgThread = 0;
		_write ( g_hPipeMmiMsg[WRITE], (void *) &sendBuf, sizeof(MMI_CMD));
		::Sleep(4);
		//close( g_hPipeMmiMsg[READ] );
		::WaitForSingleObject(g_pThreadProcessMmiMsg->m_hThread, 10000);
		//AfxMessageBox("Exit tRcvrProcessMmiMsg Thread successfully.");
	}

	BYTE dummy[1000];
	if (g_nRunSendWallDisplayMsgThread > 0)
	{
		g_nRunSendWallDisplayMsgThread = 0;
		_write ( g_hPipeWallDisplay[WRITE], (void *) dummy, 1000);
		::Sleep(4);
		::WaitForSingleObject(g_pThreadSendWallDisplayMsg->m_hThread, 10000);
	}

	closesocket(g_nServerSocket);
	g_nServerSocket = -1;
	::WaitForSingleObject(g_pThreadServerSocket->m_hThread, INFINITE);

	closesocket(g_nServerSocketWD);
	g_nServerSocketWD = -1;
	::WaitForSingleObject(g_pThreadServerSocket_WD->m_hThread, INFINITE);

	for (i=0; i<NUM_OF_SLAVES; i++)
	{
		if (g_SocketSlave[i] >= 0)
		{
			closesocket(g_SocketSlave[i]);
			g_SocketSlave[i] = -1;
			::WaitForSingleObject(g_pThreadSlave[i]->m_hThread, INFINITE);
		}
	}

	closesocket(g_socketWallDisplay);
	g_socketWallDisplay = -1;
	::WaitForSingleObject(g_pThreadWallDisplay->m_hThread, INFINITE);

	if (g_nRunSendRawFlawToMMIThread > 0)
	{
		g_nRunSendRawFlawToMMIThread = 0;
		::WaitForSingleObject(g_pThreadSendRawFlawToMMI->m_hThread, INFINITE);
	}

	if (g_nRunWriteWallDataToFileThread > 0)
	{
		g_nRunWriteWallDataToFileThread = 0;
		::WaitForSingleObject(g_pThreadWriteWallDataToFile->m_hThread, INFINITE);
	}



service_exit:

	//_getch();

	WSACleanup();  //  Free resources allocated by WSAStartup()	

    _close( g_hPipeMmiMsg[READ] );
    _close( g_hPipeMmiMsg[WRITE] );

    _close( g_hPipeWallDisplay[READ] );
    _close( g_hPipeWallDisplay[WRITE] );

	ReportStatus(SERVICE_STOPPED);

}


void CServiceApp :: Stop() {
	// report to the SCM that we're about to stop
	// Note that the service might Sleep(), so we have to tell
	// the SCM
	//	"The next operation may take me up to 11 seconds. Please be patient."
	ReportStatus(SERVICE_STOP_PENDING, 11000);

	if( m_hStop )
		::SetEvent(m_hStop);
	
}


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

// Try to connect to the TruWall MMI control program
UINT ClientSocketInit(void *dummy)
{
	struct sockaddr_in serverAddr;
	int sockAddrSize;
	int socketFD;
	unsigned short nPortNumber = 7501;

	/* Set up the server address */
	sockAddrSize = sizeof (struct sockaddr_in);
	memset ((char *) &serverAddr, 0, sockAddrSize);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( nPortNumber );  /* the port number must be the same as that in the concentrator code */
	serverAddr.sin_addr.s_addr = inet_addr("192.168.10.10");

	/* create client's socket */
	if ((socketFD = socket( AF_INET, SOCK_STREAM, 0 )) == ERROR )
	{
		//AfxMessageBox("ClientSocketInit - socket call failed.");
		return 0;
	}

	/* connect to MMI server */
	while (connect (socketFD, (struct sockaddr *) &serverAddr, sockAddrSize) != 0)
	{
		//AfxMessageBox("Master - Connect call failed.  Try again.");
		closesocket (socketFD);
		g_nMmiSocket = -1;

		if (g_nRunClientSocketInitThread == 0)
			return 0;

		if ((socketFD = socket( AF_INET, SOCK_STREAM, 0 )) == ERROR )
		{
			//AfxMessageBox("Phased Array Master - socket call failed.");
			return 0;
		}
		::Sleep(5000);
		/* return FALSE; */
	}

	g_nMmiSocket = socketFD;
	g_nRunClientSocketInitThread = 0;  //signify the thread has exited.

   	return 1;
}

// create a server socket so that instruments can connect to this command line program (PhasedArray_Master.exe)
// via tcp/ip. Address is usually 192.168.10.10 port 7502
BOOL ServerSocketInit()
{
	struct sockaddr_in local;
	int rc, nSocket;
	int optval;
	static int nCnt = 0;

	local.sin_family = AF_INET;
	local.sin_port = htons( 7502 );
//	local.sin_addr.s_addr = htonl( INADDR_ANY );
	local.sin_addr.s_addr = inet_addr("192.168.10.10");
	nSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if ( nSocket < 0 )
	{
		//AfxMessageBox(_T("Socket call failed."));
		//printf("socket call failed.\n");
		return FALSE;
	}
	optval = TRUE;	// turn the option on, if false, turn it off
	rc = setsockopt( nSocket, SOL_SOCKET, SO_REUSEADDR,	(char*)&optval, sizeof(BOOL) );
	if ( 0 != rc)
		{
		rc = WSAGetLastError();
		closesocket(nSocket);
		return FALSE;
		}


	rc = bind( nSocket, ( struct sockaddr * )&local, sizeof( local ) );
	// associates a local address with a socket
	// If no error occurs, bind returns zero
	if ( 0 != rc )
	{	
	rc = WSAGetLastError();	//0x2740 = 10048
#if 0

		WSAEADDRINUSE 
		10048  
		Address already in use. 
		Typically, only one usage of each socket address (protocol/IP address/port) is permitted. 
		This error occurs if an application attempts to bind a socket to an IP address/port that 
		has already been used for an existing socket, or a socket that was not closed properly, 
		or one that is still in the process of closing. For server applications that need to bind 
		multiple sockets to the same port number, consider using setsockopt (SO_REUSEADDR). 
		Client applications usually need not call bind at all�connect chooses an unused port automatically. 
		When bind is called with a wildcard address (involving ADDR_ANY), a WSAEADDRINUSE error 
		could be delayed until the specific address is committed. This could happen with a call 
		to another function later, including connect, listen, WSAConnect, or WSAJoinLeaf.
 
		if (nCnt < 5)
		{
			nCnt++;
			AfxMessageBox(_T("Bind call failed.\nNetwork cable might be unplugged.\nNetwork switch might not be powered on."));
			printf("Bind call failed.\n");
		}
#endif		
		closesocket(nSocket);
		return FALSE;
	}
	rc = listen( nSocket, 5 );
	// places a socket in a state where it is listening for an incoming connection
	if ( rc )
	{
		//AfxMessageBox(_T("Listen call failed."));
		//printf("listen call failed.\n");
		closesocket(nSocket);
		return FALSE;
	}

	//StartServerSocketThread();
	g_nServerSocket = nSocket;

	return TRUE;
}

// This thread only starts if the ServerSocketInit succeeded in creating a listening socket
// creates a connection for as many clients as are listening at the time it is called
// but not more than MAX_SHOES. These are instrument clients
//
UINT ServerSocketThread(void *dummy)
{
	int nClientSocket, nTmpSocket;
	//DWORD optval = 1;
    struct sockaddr_in  clientAddr;    /* client's socket address */ 
    int                 sockAddrSize;  /* size of socket address structure */ 
	int                 nWhichSlave;
	CWinThread *pThread;

	sockAddrSize = sizeof (struct sockaddr_in);

	while(1)
	{
		nClientSocket = accept( g_nServerSocket, ( struct sockaddr * ) &clientAddr, &sockAddrSize );
		// the accept function returns as soon as it receives a CONNECT ACK message

		if ( nClientSocket < 0 )
		{
			AfxMessageBox(_T("accept call failed"));
			return 0;
		}
		else
		{
			nWhichSlave = ntohl (clientAddr.sin_addr.s_addr) - 0xC0A80AC9;   /* 0xC0A808C9 = 192.168.10.201 */
			if (nWhichSlave >= 0 && nWhichSlave < NUM_OF_SLAVES)
			{
			// CHANGE from global arrays to handle connections to instance of InstMsgProcess class
#if 0
				if (g_SocketSlave[nWhichSlave] >= 0)
				{	// it had a previous connection associated with it
					closesocket(g_SocketSlave[nWhichSlave]);
					::Sleep(200);
				}
#endif
				if (g_pInstMsgProcess[nWhichSlave])	// class instance already servicing this connection
					{
					nTmpSocket = g_pInstMsgProcess[nWhichSlave]->GetSocket();
					//if (nTmpSocket >= 0)	closesocket(nTmpSocket);	// see destructor below
					delete g_pInstMsgProcess[nWhichSlave];		// closes open socket
					g_pInstMsgProcess[nWhichSlave] = NULL;
					::Sleep(200);
					}
#if 0
				g_SocketSlave[nWhichSlave] = nClientSocket;   /* for Slave 1, nWhichSlave = 0. */

				// Start a tcpip connection and thread for every client instrument
				// When the instrument sends data, this thread processes the data and ships it to 
				// other threads as needed for further processing
				g_pThreadSlave[nWhichSlave] = AfxBeginThread(tcpServerWorkTask, &g_nSlave[nWhichSlave], THREAD_PRIORITY_NORMAL);
#endif
			// create a new instance of the class and use the socket which has just been made
				g_pInstMsgProcess[nWhichSlave] = new CInstMsgProcess(nWhichSlave);
				if (g_pInstMsgProcess[nWhichSlave])
					{
					g_pInstMsgProcess[nWhichSlave]->SetSocket(nClientSocket);
					g_SocketSlave[nWhichSlave] = nClientSocket;	// debug feature
					g_pInstMsgProcess[nWhichSlave]->m_bConnected = 1;
					pThread = AfxBeginThread(tInstMsgProcess, g_pInstMsgProcess[nWhichSlave], THREAD_PRIORITY_NORMAL);
					g_pThreadSlave[nWhichSlave] = pThread;
					}
				else
					{	// fatal error
					AfxMessageBox("Fatal Error - failed to create new CInstMsgProcess");
					return 0;
					}

				// legacy code
				/*setsockopt(
					g_SocketSlave[nWhichSlave],
					SOL_SOCKET,
					TCP_NODELAY,
					(const char FAR *) &optval,
					sizeof(optval)
					);*/
			}
		}
		//AfxMessageBox(_T("Master connected to MMI successfully."));
		//AfxBeginThread(ServerSocketThread, nSocket, THREAD_PRIORITY_NORMAL);
	}	// while (1)
	
	return 0;
}


// Waits for a connection attempt by the Wall Bar Display client (WallDisplay.exe)
//
BOOL ServerSocketInit_WallDisplay()
{
	struct sockaddr_in local;
	int rc, nSocket;
	int optval;
	//static int nCnt = 0;

	local.sin_family = AF_INET;
	local.sin_port = htons( 7515 );
//	local.sin_addr.s_addr = htonl( INADDR_ANY );
	local.sin_addr.s_addr = inet_addr("192.168.10.10");
	nSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if ( nSocket < 0 )
	{
		//AfxMessageBox(_T("Socket call failed."));
		//printf("socket call failed.\n");
		return FALSE;
	}
	optval = TRUE;	// turn the option on, if false, turn it off
	rc = setsockopt( nSocket, SOL_SOCKET, SO_REUSEADDR,	(char*)&optval, sizeof(BOOL) );
	if ( 0 != rc)
		{
		rc = WSAGetLastError();
		closesocket(nSocket);
		return FALSE;
		}

	rc = bind( nSocket, ( struct sockaddr * )&local, sizeof( local ) );
	if ( rc < 0 )
	{	/*
		if (nCnt < 5)
		{
			nCnt++;
			//AfxMessageBox(_T("Bind call failed.\nNetwork cable might be unplugged.\nNetwork switch might not be powered on."));
			//printf("Bind call failed.\n");
		}
		*/
		closesocket(nSocket);
		return FALSE;
	}
	rc = listen( nSocket, 5 );
	if ( rc )
	{
		//AfxMessageBox(_T("Listen call failed."));
		//printf("listen call failed.\n");
		closesocket(nSocket);
		return FALSE;
	}

	//StartServerSocketThread();
	g_nServerSocketWD = nSocket;

	return TRUE;
}


UINT ServerSocketThread_WallDisplay(void *dummy)
{
	int nClientSocket;
	//DWORD optval = 1;
    struct sockaddr_in  clientAddr;    /* client's socket address */ 
    int                 sockAddrSize;  /* size of socket address structure */ 
//	int                 nWhichSlave;

	sockAddrSize = sizeof (struct sockaddr_in);

	while(1)
	{
		nClientSocket = accept( g_nServerSocketWD, ( struct sockaddr * ) &clientAddr, &sockAddrSize );

		if ( nClientSocket < 0 )
		{
			//AfxMessageBox(_T("accept call failed"));
			return 0;
		}
		else
		{
			if (g_socketWallDisplay >= 0)
			{
				closesocket(g_socketWallDisplay);
				::Sleep(200);
			}

			g_socketWallDisplay = nClientSocket;


			g_pThreadWallDisplay = AfxBeginThread(tcpServerWorkTask_WD, &g_nWD, THREAD_PRIORITY_NORMAL);

				/*setsockopt(
					g_SocketSlave[nWhichSlave],
					SOL_SOCKET,
					TCP_NODELAY,
					(const char FAR *) &optval,
					sizeof(optval)
					);*/
		}
		//AfxMessageBox(_T("Master connected to MMI successfully."));
		//AfxBeginThread(ServerSocketThread, nSocket, THREAD_PRIORITY_NORMAL);
	}
	
	return 0;
}



/***************************************************
   readn - read exactly n bytes from Tcp socket fd
****************************************************/
int readn( int fd, char *bp, int len)
{
	int cnt;
	int rc;

	cnt = len;
	while ( cnt > 0 )
	{
		// recv will block if no data is available and the socket is synchronous
		// if the socket is async,	WSAEWOULDBLOCK is returned if no data is available
		rc = recv( fd, bp, cnt, 0 );
		if ( rc < 0 )				/* read error? */
		{
			if ( WSAGetLastError() == WSAEINTR )	/* interrupted? */
				continue;			/* restart the read */
			return -1;				/* return error */
		}
		if ( rc == 0 )				/* EOF? */
			return len - cnt;		/* return short count */
		bp += rc;
		cnt -= rc;
	}
	return len;
}


#if 1
/* Thread receiving MMI messages */
UINT tRcvrProcessMmiMsg(void *dummy)
{
	MMI_CMD rcvrBuf;
	int rc;

	while(1)
	{
		if (g_nRunRcvrProcessMmiMsgThread == 0)
		{
			closesocket (g_nMmiSocket);
			g_nMmiSocket = -1;
			return 0;
		}

	    if (g_nMmiSocket >= 0)
	    {
			if (readn (g_nMmiSocket, (char *) &rcvrBuf, sizeof(MMI_CMD)) <= 0)
			{
				closesocket (g_nMmiSocket);
				g_nMmiSocket = -1;
				if (g_nRunRcvrProcessMmiMsgThread == 0)
					return 0;
				g_nRunClientSocketInitThread = 1;
				g_pThreadClientSocketInit = AfxBeginThread(ClientSocketInit, NULL, NULL);
			}
			else
			{
				//ProcessMmiMsg(&rcvrBuf);
				rc = _write ( g_hPipeMmiMsg[WRITE], (void *) &rcvrBuf, sizeof(MMI_CMD));
				//if (rc <0)
					//printf("write pipe failed.");
			}
	    }
	    else
	    {
			::Sleep(2000);
	    }
	}

 	closesocket (g_nMmiSocket);
	g_nMmiSocket = -1;
  	
	return 1;
}
#endif


/***************************************************
   readn - read exactly n bytes from pipe
****************************************************/
int readnpipe( int fd, char *bp, int len)
{
	int cnt;
	int rc;

	cnt = len;
	while ( cnt > 0 )
	{
		rc = _read( fd, bp, cnt );
		if ( rc < 0 )				/* read error? */
		{
			return -1;				/* return error */
		}
		if ( rc == 0 )				/* EOF? */
			return len - cnt;		/* return short count */
		bp += rc;
		cnt -= rc;
	}
	return len;
}



#if 1
/****************************************************************************
* Helper:  process MMI message
*
*/
UINT ProcessMmiMsg(void *dummy)
{
	MMI_CMD readBuf;
	MMI_CMD *pMmiCmd = (MMI_CMD *) &readBuf;

	WORD *pWArg;
	DWORD *pDWArg;
	float *pFArg;
	WORD MsgId;
	int i, rc;  /* generic looper */
	static int nChannel = 0;
	static int nGate = 0;
	static int old_gChannel = 0;
	int  nWhichSlave;
	SHOE_CONFIG  *pShCfg;
	C_MSG_ALL_THOLD *pThold;
	WALL_COEF *pWallCoef;
	SITE_SPECIFIC_DEFAULTS *pSiteDef;
	CHANNEL_CONFIG2 ChannelCfg;
	static int  nInspectMode = NOP_MODE;
	int nMotionTime;
	MMI_CMD  tempCmd;
	WORD nEnableAscan = 0;
	JOB_REC *pJobRec;
	C_MSG_NC_NX *pNcNx = (C_MSG_NC_NX *) pMmiCmd->CmdBuf;

	pJobRec = (JOB_REC *) pMmiCmd->CmdBuf;

	pWArg = (WORD *) pMmiCmd->CmdBuf;
	pDWArg = (DWORD *) pMmiCmd->CmdBuf;
	pFArg = (float *) pMmiCmd->CmdBuf;
	pShCfg = (SHOE_CONFIG *) pMmiCmd->CmdBuf;
	pThold = (C_MSG_ALL_THOLD *) pMmiCmd->CmdBuf;

	while (1)
	{
		if (g_nRunProcessMmiMsgThread == 0)
		{
			return 0;
		}

		rc = readnpipe(g_hPipeMmiMsg[READ], (char *) &readBuf, sizeof(MMI_CMD));

		if (rc  == sizeof(MMI_CMD))
		{
		
			MsgId = pMmiCmd->MsgId;

			//nWhichSlave = nChannel / g_NumberOfScans;
			nWhichSlave = FindWhichSlave(nChannel);

			switch(MsgId)
			{
			case CHANNEL_SELECT:
				nChannel = pWArg[0];
				gChannel = nChannel;// % MAX_CHANNEL_PER_INSTRUMENT;
				//nWhichSlave = nChannel / g_NumberOfScans;
				nWhichSlave = FindWhichSlave(nChannel);
				//pWArg[0] = nChannel % g_NumberOfScans;
				pWArg[0] = FindSlaveChannel(nChannel);
				SetGetInspectMode_M (1 /* GET */, &nInspectMode, &nMotionTime);
				if (nInspectMode != PKT_MODE)
					SendSlaveMsg (nWhichSlave, pMmiCmd);
				else
				{
					/* if active channel is changed from one slave to another */
					if ( (nChannel/g_NumberOfScans) != (old_gChannel/g_NumberOfScans) )
					{
						nWhichSlave = old_gChannel / g_NumberOfScans;
						tempCmd.MsgId = NOP_MODE;
						SendSlaveMsg (nWhichSlave, &tempCmd);
						nWhichSlave = nChannel / g_NumberOfScans;
						tempCmd.MsgId = PKT_MODE;
						SendSlaveMsg (nWhichSlave, &tempCmd);
					}
					else
						SendSlaveMsg (nWhichSlave, pMmiCmd);
				}
				old_gChannel = nChannel;
				break;

			case SET_ASCAN_READ_SEQ:
				nWhichSlave = FindWhichSlave(pWArg[0]);
				SendSlaveMsg (nWhichSlave, pMmiCmd);
				//printf("set ascan read seq.\n");
				break;

			case SET_ASCAN_READ_BEAM:
				SendSlaveMsg (pWArg[0], pMmiCmd);
				break;

			case GATE_SELECT:
				nGate = pWArg[0] % MAX_GATES;
				gGate = nGate;
				SendSlaveMsg (nWhichSlave, pMmiCmd);
				break;

			case PULSER_ONOFF:
				nWhichSlave = pWArg[0];
				SendSlaveMsg (nWhichSlave, pMmiCmd);
				break;

			case PULSER_PRF:
				g_nPulserPRF = pWArg[1];
				if (g_nPulserPRF < 1) g_nPulserPRF = 1000;
				for (i=0; i<NUM_OF_SLAVES; i++)
				{
					SendSlaveMsg (i, pMmiCmd);
				}
				break;

			case SCOPE_TRACE1_MDAC:
				SendSlaveMsg (pWArg[0], pMmiCmd);
				break;

			case RUN_MODE:  /* inspection run mode */
				nInspectMode = RUN_MODE;
				nMotionTime = (int) pWArg[0];
				g_nXloc = 0;
				g_nXloc_S2 = 0;
				g_nTick = 0;
				g_nOldMotionBus = 0;
				SetGetInspectMode_M (0 /* SET */, &nInspectMode, &nMotionTime);
				g_nAuxClkIntCnt = -10;
				for (i=0; i<NUM_OF_SLAVES; i++)
				{
					SendSlaveMsg (i, pMmiCmd);
				}
				break;

			case CAL_MODE:  /* calibration mode */
				nInspectMode = CAL_MODE;
				SetGetInspectMode_M (0 /* SET */, &nInspectMode, &nMotionTime);
				for (i=0; i<NUM_OF_SLAVES; i++)
				{
					SendSlaveMsg (i, pMmiCmd);
				}
				break;

			case PKT_MODE:  /* calibration mode */
				nInspectMode = PKT_MODE;
				SetGetInspectMode_M (0 /* SET */, &nInspectMode, &nMotionTime);
				SendSlaveMsg (nWhichSlave, pMmiCmd);
				break;

			case PLC_MODE:  /*  */
				nInspectMode = PLC_MODE;
				SetGetInspectMode_M (0 /* SET */, &nInspectMode, &nMotionTime);

				g_nPlcOfWho = (BYTE) pWArg[0];

 	   			if (pWArg[0] > 0)
 	   			{
 	   				nWhichSlave = pWArg[0] - 1;
 	   				SendSlaveMsg (nWhichSlave, pMmiCmd);
 	   			}
				break;

			case ADC_MODE:
  	  			nWhichSlave = pWArg[0];
  	  			SendSlaveMsg (nWhichSlave, pMmiCmd);
  	  			break;

			case NOP_MODE:  /* inspection stop mode */
		#if 1
				InitImageBufArray ();
		#endif
  	  			nInspectMode = NOP_MODE;
   	 			SetGetInspectMode_M (0 /* SET */, &nInspectMode, &nMotionTime);
    			g_nXloc = 0;
    			g_nXloc_S2 = 0;
    			g_nTick = 0;
				g_nNextWindow = 0;
 	   			for (i=0; i<NUM_OF_SLAVES; i++)
  	  			{
   	 				SendSlaveMsg (i, pMmiCmd);
    			}
    			break;

			case ASCAN_MODE:
				//printf("MasterTCPIP -- ASCAN_MODE.\n");
 	   			nEnableAscan = pWArg[0];
  	  			for (i=0; i<NUM_OF_SLAVES; i++)
   	 			{
    					if ( (i == nWhichSlave) && (nEnableAscan == 1) )
    						pWArg[0] = 1;
    					else
    						pWArg[0] = 0;
	    				SendSlaveMsg (i, pMmiCmd);
 	   			}
  	  			break;

			case CHANNEL_CONFIG_MSG:
    			nWhichSlave = pShCfg->nSlave;
    			for ( i = 0; i < MAX_CHANNEL_PER_INSTRUMENT; i++)
    			{
	    			ChannelCfg.Ch[nWhichSlave][i].Type = pShCfg->Ch[i].Type;
 	   				ChannelCfg.Ch[nWhichSlave][i].cXOffset = pShCfg->Ch[i].cXOffset;
  	  				ChannelCfg.Ch[nWhichSlave][i].cWOffset = pShCfg->Ch[i].cWOffset;
   	 			}
    			//ChannelCfg.cClockOffset = pChnlCfg->cClockOffset;
    			SetGetChannelCfg (0 /* SET */, &ChannelCfg, nWhichSlave);
    			g_nMaxXSpan = GetMaxXSpan();
				InitImageBufArray ();
  	  			SendSlaveMsg (nWhichSlave, pMmiCmd);		
   	 			break;

			case SET_ALL_THOLDS:
    			for (i=0; i<NUM_OF_SLAVES; i++)
    			{
    				SendSlaveMsg (i, pMmiCmd);
				}
 	   			memcpy ( (void *) &g_AllTholds, (void *) pThold, sizeof(C_MSG_ALL_THOLD) );
  	  			break;

			case SITE_SPECIFIC_MSG:
				pSiteDef = (SITE_SPECIFIC_DEFAULTS *) pMmiCmd->CmdBuf;
 	   			SetGetSiteDefaults(0 /*SET*/, pSiteDef);
  	  			g_nRecordWallData = pSiteDef->nRecordWallData;

   	 			if (pSiteDef->nDefaultLineSpeed > 0)
    					g_fTimePerInch = 1.0f / ( ((float) pSiteDef->nDefaultLineSpeed)*12.0f/6000.0f );
    			if (pSiteDef->nDefaultRotateSpeed > 0)
    				g_fTimePerTurn = 1.0f / ( ((float)pSiteDef->nDefaultRotateSpeed)/6000.0f );

    			if (pSiteDef->n20ChnlPerHead == 0)
    				g_b20ChnlPerHead = FALSE;
    			else
    				g_b20ChnlPerHead = TRUE;

				if ( pSiteDef->fMotionPulseLen > 0.0f)
					g_fMotionPulseLen = pSiteDef->fMotionPulseLen;


    			for (i=0; i<NUM_OF_SLAVES; i++)
    			{
    				SendSlaveMsg (i, pMmiCmd);
				}

 	   			g_nMaxWallWindowSize = pSiteDef->nMaxWallWindowSize;
  	  			if ( (g_nMaxWallWindowSize > WALL_BUF_SIZE) || (g_nMaxWallWindowSize < 1) )
   	 				g_nMaxWallWindowSize = 10;

    			pMmiCmd->MsgId = WALL_DROP_TIME;
    			pWArg[0] = pSiteDef->nWallDropTime;
				for (i=0; i<NUM_OF_SLAVES; i++)
				{
		   			SendSlaveMsg (i, pMmiCmd);
	  			}
   	 			g_WallDropTime = pWArg[0] / 1000.f;

				g_NumberOfScans = 0;
				// a slave = an instrument processor. It has a tcpip link associated
				// a slave can only produce one type of inspection scan
				for (i=0; i<NUM_OF_SLAVES; i++)
				{
					switch(pSiteDef->nPhasedArrayScanType[i])
					{
					case LINEAR_SCAN_0_DEGREE:
					case LINEAR_SCAN_37_DEGREE:
						g_NumberOfScans += 1;
						g_ArrayScanNum[i] = 1;
						g_SequenceLength[i] = 49;
						break;

					default:
					case THREE_SCAN_LRW_8_BEAM:
					case THREE_SCAN_LRW_8_BEAM_FOCUS:
					case THREE_SCAN_LO1LO1R_8_BEAM_12345678:
					case THREE_SCAN_LO1LO1R_8_BEAM_56781234:
						g_NumberOfScans += 3;
						g_ArrayScanNum[i] = 3;
						g_SequenceLength[i] = 24;
						break;

					case THREE_SCAN_LRW_16_BEAM:
						g_NumberOfScans += 3;
						g_ArrayScanNum[i] = 3;
						g_SequenceLength[i] = 48;
						break;

					case TWO_SCAN_LR_8_BEAM:
						g_NumberOfScans += 2;
						g_ArrayScanNum[i] = 2;
						g_SequenceLength[i] = 16;
						break;

					case TWO_SCAN_LR_16_BEAM:
						g_NumberOfScans += 2;
						g_ArrayScanNum[i] = 2;
						g_SequenceLength[i] = 32;
						break;

					case LONG_8_BEAM_12345678:
					case LONG_8_BEAM_56781234:
						g_NumberOfScans += 1;
						g_ArrayScanNum[i] = 1;
						g_SequenceLength[i] = 8;
						break;

					case LONG_24_BEAM_800:
					case LONG_24_BEAM_080:
					case LONG_24_BEAM_12345678:
					case LONG_24_BEAM_56781234:
						g_NumberOfScans += 1;
						g_ArrayScanNum[i] = 1;
						g_SequenceLength[i] = 24;
						break;

					case WALL_25_BEAM_90_DEGREE_PROBE:
						g_NumberOfScans += 1;
						g_ArrayScanNum[i] = 1;
						g_SequenceLength[i] = 25;
						break;
					}

					g_nPhasedArrayScanType[i] = pSiteDef->nPhasedArrayScanType[i];
				}
					
				
    			break;

			case SET_NC_NX:
    			memcpy ( (void *) &g_NcNx, (void *) pNcNx, sizeof (C_MSG_NC_NX) );
    			for (i=0; i<NUM_OF_SLAVES; i++)
    			{
    				SendSlaveMsg (i, pMmiCmd);
				}
 	   			break;

			case SET_WALL_COEFS:
    			pWallCoef = (WALL_COEF *) pMmiCmd->CmdBuf;
    			for (i=0; i<MAX_SHOES; i++)
    			{
	    			pWallCoef->fWallSlope[0] = pWallCoef->fWallSlope[i];
 	   				pWallCoef->WallOffset[0] = pWallCoef->WallOffset[i];

  	  				SendSlaveMsg (i, pMmiCmd);
   	 			}
    			memcpy( (void *) &g_WallCoef, (void *) pWallCoef, sizeof (WALL_COEF) );
    			break;

			case 0x69:   /* sequence length */
			case FIRING_SEQ:
    			nWhichSlave = pWArg[0]/10;
				pWArg[0] %= 10;
 				SendSlaveMsg (nWhichSlave, pMmiCmd);
  				break;

			case JOINT_NUMBER_MSG:
   				g_nJointNum = pDWArg[0];
    			for (i=0; i<NUM_OF_SLAVES; i++)
    			{
	    			SendSlaveMsg (i, pMmiCmd);
				}
				break;

			case RECEIVER_GAIN:
			case TCG_FN:
				nWhichSlave = FindWhichSlave(pMmiCmd->ChnlNum);
				SendSlaveMsg (nWhichSlave, pMmiCmd);
   	 			break;

			case RECEIVER_FCNT:   /* receiver offset */
    			nWhichSlave = FindWhichSlave(pWArg[1]);
    			SendSlaveMsg (nWhichSlave, pMmiCmd);
    			break;

			case ENET_STATS_MODE:
    			g_nNoMmiReplyCnt = 0;
    			for (i=0; i<NUM_OF_SLAVES; i++)
				{
 	   			SendSlaveMsg (i, pMmiCmd);
  	  			}
   	 			break;

			case 0x63:   /* TCG_TRIGGER, TCG_STEP */
    			nWhichSlave = FindWhichSlave(pWArg[1]);
 	   			SendSlaveMsg (nWhichSlave, pMmiCmd);
  	  			break;

			case RUN_CAL_JOINT:
				if (pWArg[0])
 	   				g_bRunCalJoint = TRUE;
  	  			else
   	 				g_bRunCalJoint = FALSE;
    			if (g_bRunCalJoint)
    			{
    				g_nNextRealJointNum = g_nJointNum;
    				g_nJointNum = g_nNextCalJointNum;
				}
 	   			else
  	  			{
   	 				g_nNextCalJointNum = g_nJointNum;
    				g_nJointNum = g_nNextRealJointNum;
    			}
    			break;

			case LAST_JOINT_NUMBER:
				g_nNextRealJointNum = pDWArg[0]+1;
 	   			g_nNextCalJointNum = pDWArg[1]+1;
  	  			if (g_bRunCalJoint)
   	 			{
    				g_nJointNum = g_nNextCalJointNum;
    			}
    			else
				{
		   			g_nJointNum = g_nNextRealJointNum;
  	  			}
   	 			break;

			case STORE_JOBREC_MSG:
				g_bShowWallDiff = pJobRec->ShowWallDiff;
				memcpy( (void *) &g_JobRec, (void *) pJobRec, sizeof (JOB_REC) );

			case WALL_DROP_TIME:
				for (i=0; i<NUM_OF_SLAVES; i++)
 	   			{
  	  				SendSlaveMsg (i, pMmiCmd);
   	 			}
    			break;

			case LOAD_CONFIG_FILE:
 	   			for (i=0; i<NUM_OF_SLAVES; i++)
  	  			{
   	 				SendSlaveMsg (i, pMmiCmd);
    			}
    			if (g_AdiStatus != 3)
    				g_AdiStatus = 1;
				break;

			case ASCAN_REFRESH_RATE:
 				for (i=0; i<NUM_OF_SLAVES; i++)
  				{
   	 				SendSlaveMsg (i, pMmiCmd);
    			}
    			break;

			case ASCAN_BROADCAST:
				g_bBcastAscan = (BYTE) pWArg[0];
 	   			break;

			case SET_X_SCALE:
				g_nXscale = pWArg[0];
 	   			if (g_nXscale < 600) g_nXscale = 900;
  	  			if (g_nXscale > 900) g_nXscale = 900;
   	 			break;

			case SET_PIPE_PRESENT:
 				for (i=0; i<NUM_OF_SLAVES; i++)
  				{
   	 				SendSlaveMsg (i, pMmiCmd);
    			}
    			break;

			case TURN_OFF_MASTER:
				//ShutDownSystem();
				g_nShowWallBars = pWArg[0];
				break;

			default:
    			SendSlaveMsg (nWhichSlave, pMmiCmd);
    			break;
			}
		}
		else if (rc >= 0)
		{
			;
			//printf("Read End of Pipe.");
		}
		else
		{
			//printf("Read Pipe Error.");
			return 0;
		}

		//::Sleep(2);
	}
}


/****************************************************************************
* Helper:  send messages to Nios slave over TCP socket
*
*/
BOOL SendSlaveMsg(int nWhichSlave, MMI_CMD *pSendBuf)
{
	WORD *pWArg;
	int rc, data, i;
	BYTE buf[CmdPacketLength], value;
	SCmdPacket *pCmd = (SCmdPacket *) buf;
	static BYTE GateTrigger[MAX_CHANNEL][4];
	static BYTE GateDetectMode[MAX_CHANNEL][4];
	static BYTE GatePolarity[MAX_CHANNEL][4];
	static BYTE GateTofTrigger[MAX_CHANNEL][4];
	static BYTE GateTofStop[MAX_CHANNEL][4];
	static BYTE TcgTrigger[NUM_OF_SLAVES][4];
	static BYTE TcgStep[NUM_OF_SLAVES];
	static TCG_REC_MSG TcgRec[NUM_OF_SLAVES][4];
	SITE_SPECIFIC_DEFAULTS *pSiteDef;
	int    nSlaveCh=0;
	static int nOldSlave;
	WORD td[16];  //focusing time delays


	if ( (pSendBuf->MsgId == SET_ASCAN_READ_SEQ) || (pSendBuf->MsgId == SET_ASCAN_READ_BEAM) )
	{
		pCmd->DataHead.bMsgID = SET_ASCAN_SEQ_REG_CMD_ID;

		if (nWhichSlave != nOldSlave)
		{
			for (i=0; i<NUM_OF_SLAVES; i++)
			{
				pCmd->wData[3] = 0;   //turn off Ascan send
				if (g_SocketSlave[i] >= 0)
					rc = send( g_SocketSlave[i], (char *) pCmd, CmdPacketLength, 0 );
			}
		}

		nOldSlave = nWhichSlave;
	}

	pWArg = (WORD *) pSendBuf->CmdBuf;

	if (g_SocketSlave[nWhichSlave] >= 0)
	{
		switch (pSendBuf->MsgId)
		{
		case SET_ASCAN_READ_SEQ:
			pCmd->DataHead.bMsgID = SET_ASCAN_SEQ_REG_CMD_ID;
			pCmd->wData[2] = FindSlaveChannel(pWArg[0]);
			pCmd->wData[3] = 1;  //turn on ascan send
			rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			//printf("set ascan read seq.\n");
		
			break;

		case SET_ASCAN_READ_BEAM:
			pCmd->DataHead.bMsgID = SET_ASCAN_SEQ_ONE_BEAM_CMD_ID;
			pCmd->wData[0] = pWArg[1];
			rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			break;
			
		case RECEIVER_GAIN:
			if (pSendBuf->ChnlNum < g_NumberOfScans)
			{
				nSlaveCh = FindSlaveChannel(pSendBuf->ChnlNum);
				pCmd->DataHead.bMsgID = SET_GAIN_DAC_CMD_ID;
				pCmd->wData[0] = (WORD) ( (pWArg[0] % 420) * 3 / 10.0f + 0.5f );
				pCmd->wData[1] = nSlaveCh;
				if (TcgTrigger[nWhichSlave][nSlaveCh] == 0)
					rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			}
			break;

		case TCG_FN:
			if (pSendBuf->ChnlNum < g_NumberOfScans)
			{
				nSlaveCh = FindSlaveChannel(pSendBuf->ChnlNum);
				pCmd->DataHead.bMsgID = SET_TCG_FN_CMD_ID;
				pCmd->DataHead.bSpare1 = FindSlaveChannel(pSendBuf->ChnlNum);
				memcpy ( (void *) pCmd->wData, (void *) pSendBuf->CmdBuf, sizeof(TCG_REC_MSG) );
				memcpy ( (void *) &TcgRec[nWhichSlave][nSlaveCh], (void *) pSendBuf->CmdBuf, sizeof(TCG_REC_MSG) );
				if (TcgTrigger[nWhichSlave][nSlaveCh] > 0)
					rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			}
			break;

		case TCG_TRIGGER:  //TCG_Trigger, TCG_STEP
			if (pWArg[1] < g_NumberOfScans)
			{
				nSlaveCh = FindSlaveChannel(pWArg[1]);
				TcgTrigger[nWhichSlave][nSlaveCh] = pWArg[0] & 0x000F;
				TcgStep[nWhichSlave] = (pWArg[0] & 0x00F0) >> 4;
				pCmd->DataHead.bMsgID = SET_TCG_STEP_CMD_ID;
				
				pCmd->wData[0] = TcgStep[nWhichSlave];  //step: 0, 1, 2, or 3
				pCmd->wData[1] = nSlaveCh;  //sequence number to set

				for (i=0; i<4; i++)
					pCmd->wData[2+i] = TcgTrigger[nWhichSlave][i];
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			}
			break;

		case RECEIVER_RF_VIDEO:
			if (gChannel < g_NumberOfScans)
			{
				pCmd->DataHead.bMsgID = SET_GATE_DATA_MODE_CMD_ID;
				GateDetectMode[gChannel][gGate] = (BYTE) pWArg[0];
				value = 0;
				for (i=0; i<4; i++)
				{
					if (GateDetectMode[gChannel][i] == 1)  //Full Wave mode
						value = value | (1 << (i*2+1));
					else if (GateDetectMode[gChannel][i] == 0)  //RF mode
					{
						if (GatePolarity[gChannel][i] == 1)  //negative
							value = value  | (1 << (i*2));
					}
				}
				pCmd->wData[0] = value;
				pCmd->wData[2] = FindSlaveChannel(gChannel);
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
				//::Sleep(2);
				//Set Ascan to RF (non peak hold) or Full Wave (peak hold) mode
				//pCmd->DataHead.bMsgID = SET_ASCAN_PEAK_SEL_REG_CMD_ID;
				//pCmd->wData[0] = (BYTE) (pWArg[0] & 0x1);
				//rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			}
			break;

		case SET_ASCAN_PEAK_SEL_REG:
			if (gChannel < g_NumberOfScans)
			{
				pCmd->DataHead.bMsgID = SET_ASCAN_PEAK_SEL_REG_CMD_ID;
				pCmd->wData[0] = pWArg[0] & 0x1;
				//if (pCmd->wData[0] == 1) pCmd->wData[0]=2;
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
				//printf("SET_ASCAN_PEAK_SEL_REG pWArg = %d, sel =%d\n", pWArg[0], pCmd->wData[0]);
			}
			break;

		case RECEIVER_POLT:
			if (gChannel < g_NumberOfScans)
			{
				pCmd->DataHead.bMsgID = SET_GATE_DATA_MODE_CMD_ID;
				GatePolarity[gChannel][gGate] = (BYTE) pWArg[0];
				value = 0;
				for (i=0; i<4; i++)
				{
					if (GateDetectMode[gChannel][i] == 1)  //Full Wave mode
						value = value | (1 << (i*2+1));
					else if (GateDetectMode[gChannel][i] == 0)  //RF mode
					{
						if (GatePolarity[gChannel][i] == 1)  //negative
							value = value  | (1 << (i*2));
					}
				}
				pCmd->wData[0] = value;
				pCmd->wData[2] = FindSlaveChannel(gChannel);
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			}
			break;

		case GATES_DELAY:
			if (gChannel < g_NumberOfScans)
			{
				nSlaveCh = FindSlaveChannel(gChannel);
				pCmd->DataHead.bMsgID = SET_GATE_DELAY_CMD_ID;
				pCmd->wData[0] = gGate;
				pCmd->wData[1] = 100* (pWArg[0] - 6) / 50;
				pCmd->wData[2] = nSlaveCh;
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
				
				if ( (TcgTrigger[nWhichSlave][nSlaveCh] > 0) && (TcgRec[nWhichSlave][nSlaveCh].GateOn[gGate] > 0) )
				{
					//::Sleep(2);
					pCmd->DataHead.bMsgID = SET_TCG_FN_CMD_ID;
					pCmd->DataHead.bSpare1 = nSlaveCh;
					memcpy ( (void *) pCmd->wData, (void *) &TcgRec[nWhichSlave][nSlaveCh], sizeof(TCG_REC_MSG) );
					rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
				}
			}
			break;

		case GATES_RANGE:
			if (gChannel < g_NumberOfScans)
			{
				nSlaveCh = FindSlaveChannel(gChannel);
				pCmd->DataHead.bMsgID = SET_GATE_RANGE_CMD_ID;
				pCmd->wData[0] = gGate;
				pCmd->wData[1] = 100* (pWArg[0] + 2) / 50;
				pCmd->wData[2] = nSlaveCh;
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );

				if ( (TcgTrigger[nWhichSlave][nSlaveCh] > 0) && (TcgRec[nWhichSlave][nSlaveCh].GateOn[gGate] > 0) )
				{
					//::Sleep(2);
					pCmd->DataHead.bMsgID = SET_TCG_FN_CMD_ID;
					pCmd->DataHead.bSpare1 = nSlaveCh;
					memcpy ( (void *) pCmd->wData, (void *) &TcgRec[nWhichSlave][nSlaveCh], sizeof(TCG_REC_MSG) );
					rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
				}
			}
			break;

		case GATES_BLANK:
			if (gChannel < g_NumberOfScans)
			{
				pCmd->DataHead.bMsgID = SET_GATE_BLANK_CMD_ID;
				pCmd->wData[0] = gGate;
				pCmd->wData[1] = 100* (pWArg[0] + 2) / 50;
				pCmd->wData[2] = FindSlaveChannel(gChannel);
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			}
			break;

		case GATES_TRIGGER:
			if (gChannel < g_NumberOfScans)
			{
				pCmd->DataHead.bMsgID = SET_GATE_CONTROL_CMD_ID;
				GateTrigger[gChannel][gGate] = (BYTE) pWArg[0];
				value = 0;
				for (i=0; i<4; i++)
				{
					if (GateTrigger[gChannel][i] > 1)
						value = value | (1 << (i+4)) | (1 << i);  //interface triggering
					else if (GateTrigger[gChannel][i] > 0)
						value = value  | (1 << i);     //initial pulse triggering
				}
				pCmd->wData[0] = value;
				pCmd->wData[2] = FindSlaveChannel(gChannel);
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			}
			break;

		case GATES_LEVEL:
			if (gChannel < g_NumberOfScans)
			{
				pCmd->DataHead.bMsgID = SET_GATE_THRESHOLD_CMD_ID;
				pCmd->wData[0] = gGate;
				pCmd->wData[1] = pWArg[0];
				pCmd->wData[2] = FindSlaveChannel(gChannel);
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			}
			break;

		case TOF_TRIGGER:
			if (gChannel < g_NumberOfScans)
			{
				pCmd->DataHead.bMsgID = SET_GATE_TOF_MODE_CMD_ID;
				GateTofTrigger[gChannel][gGate] = (BYTE) pWArg[0];
				value = 0;
				for (i=1; i<4; i++)
				{
					if (GateTofTrigger[gChannel][i] > 1)
						value = value | (1 << (i+2));
					if (GateTofStop[gChannel][i] > 0)
						value = value  | (1 << (i-1));
				}
				pCmd->wData[0] = value;
				pCmd->wData[2] = FindSlaveChannel(gChannel);
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			}
			break;

		case TOF_STOPON:
			if (gChannel < g_NumberOfScans)
			{
				pCmd->DataHead.bMsgID = SET_GATE_TOF_MODE_CMD_ID;
				GateTofStop[gChannel][gGate] = (BYTE) pWArg[0];
				value = 0;
				for (i=1; i<4; i++)
				{
					if (GateTofTrigger[gChannel][i] > 1)
						value = value | (1 << (i+2));
					if (GateTofStop[gChannel][i] > 0)
						value = value  | (1 << (i-1));
				}
				pCmd->wData[0] = value;
				pCmd->wData[2] = FindSlaveChannel(gChannel);
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			}
			break;

		case PULSER_PRF:
			pCmd->DataHead.bMsgID = SET_PULSE_RATE_CMD_ID;
			if (pWArg[1] <= 0) pWArg[1] = 100;
			pCmd->wData[0] = (WORD) ((DWORD) (80000000 / pWArg[1])  & 0x0000FFFF);
			pCmd->wData[1] = (WORD) (((DWORD) (80000000 / pWArg[1])  & 0x003F0000) >> 16);
			pCmd->wData[2] = pWArg[1];
			rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
			break;

		case SET_ASCAN_REGISTERS:
			if (gChannel < g_NumberOfScans)
			{
				pCmd->DataHead.bMsgID = SET_ASCAN_RANGE_CMD_ID;
				data = (int) pWArg[1] / 10 - 1;
				if (data < 0) data = 0;
				pCmd->wData[0] = data;
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
				//::Sleep(2);
				pCmd->DataHead.bMsgID = SET_ASCAN_DELAY_CMD_ID;
				data = (pWArg[0] - 250) * 2 + 25;  //add 25 to align gate with signal
				if (data > 50000) data = 50000;
				pCmd->wData[0] = data;
				rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );
				//printf("Set ascan registers");
			}
			break;

		case SITE_SPECIFIC_MSG:
			pSiteDef = (SITE_SPECIFIC_DEFAULTS *) pSendBuf->CmdBuf;
			pCmd->DataHead.bMsgID = SET_SCAN_TYPE_CMD_ID;
			pCmd->wData[0] = pSiteDef->nPhasedArrayScanType[nWhichSlave];
			pCmd->wData[1] = (WORD)(1000.0f * sin((double) pSiteDef->nTranAngle / 10.0f * 3.14159f / 180.0f) * 0.6f / 1.483f);
			pCmd->wData[2] = pSiteDef->nEncoderDivider;
			pCmd->wData[3] = 35;   //oblique 1 time delay step
			pCmd->wData[4] = (unsigned short) (g_AllTholds.fOD * 1000.0f + 0.5f);   //OD
			pCmd->wData[5] = (unsigned short) (pSiteDef->fWaterPath * 1000.0f + 0.5f);   //water path
			pCmd->wData[6] = (unsigned short) (0.6f * 1000.0f + 0.5f);   //array pitch
			if ( pSiteDef->nPhasedArrayScanType[nWhichSlave] == THREE_SCAN_LRW_8_BEAM_FOCUS)
			{
				ComputeTranFocusDelay(g_AllTholds.fWall, pSiteDef->fFocusPointZf, pSiteDef->fWaterPath, pSiteDef->nTranAngle / 10.0f, td);
				for (i=0; i<16; i++)
					pCmd->wData[2+i] = td[i];
			}
			rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );

			//set ad9272 PGA gain
			pCmd->DataHead.bMsgID = CHANGE_9272_GAIN;
			pCmd->wData[0] = pSiteDef->nAd9272PgaGain;
			rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );

			break;

		case SET_PIPE_PRESENT:
			pCmd->DataHead.bMsgID = SET_INSPECT_ENABLE;
			pCmd->wData[0] = pWArg[0];
			rc = send( g_SocketSlave[nWhichSlave], (char *) pCmd, CmdPacketLength, 0 );

			break;

		default:
			rc = 1;
			break;
		}



		if ( rc <= 0 )
		{
			return FALSE;
		}
		return TRUE;
	}
	//printf("Master - Connection not established with Slave %d.\n\n", nWhichSlave+1);
	return FALSE;
}
#endif

/**************************************************************************** 
* 
* tInstMsgProcess - receive and process slave data .. formerly called tcpServerWorkTask
* passes pointer to CInstMsgProcess class instance, ie which class instance is servicing instrument n
* 
* RETURNS: 0 or 1. 
*/ 

UINT tInstMsgProcess (void *pCInstMsgProcess)
{
	CInstMsgProcess *pInstMsgProcess = (CInstMsgProcess *) pCInstMsgProcess;	// copy in ptr to the particular instance
	int tmpdbg = pInstMsgProcess->GetInstNumber();
	int *pSlave = &tmpdbg;
	int nSocket = pInstMsgProcess->GetSocket();
/**************************************************************************** 
* 
* tcpServerWorkTask - receive and process slave data 
* 
* RETURNS: 0 or 1. 
*/ 
// change task name from tcpServerWorkTask to tInstMsgProcess
#if 0
UINT tcpServerWorkTask (void *pSlave)
{
#endif
	I_MSG_RUN *pSendBuf;
	PEAK_DATA  ReadBuf;
	BYTE recvBuf[RawDataPacketLength];
	UDP_SLAVE_DATA SlvData;
	int i, ic;
	int nInspectMode;
	int nMotionTime;
	int xloc = 0;   /* temporary */
	DWORD  NextPacketNum = 0;
	int nReturn;
	BOOL  bGetSlaveRev = TRUE;
	SITE_SPECIFIC_DEFAULTS SiteDefault;
	int nSendCnt = 0;
	int nOldXloc=-1;
	SRawDataPacket *pNiosRawData = (SRawDataPacket *) recvBuf;
	static int nRecvCnt = 0;
	static short wClock_old = 100;

	int nSlave = pInstMsgProcess->GetInstNumber();		//*( (int *) pSlave);
	ReadBuf.RDP_number = nSlave;

	I_MSG_RUN SendBuf;
	I_MSG_RUN SendCalBuf;
	I_MSG_ASCAN *pAscanMsg = (I_MSG_ASCAN *) &SendBuf;
	pAscanMsg->MstrHdr.MsgId = ASCAN_MODE;


	while (1)
	{
		if (nSocket >= 0)		// g_SocketSlave[nSlave] >= 0)
		{
			//if (readn(g_SocketSlave[nSlave], (char *) recvBuf, RawDataPacketLength) <= 0)
			if (readn(nSocket, (char *) recvBuf, RawDataPacketLength) <= 0)
			{
#if 0
				closesocket (g_SocketSlave[nSlave]);
				g_SocketSlave[nSlave] = -1;
#endif
				delete pInstMsgProcess;	// closes open sockets
				g_pInstMsgProcess[nSlave] = NULL;

				return 0;
			}

			SetGetInspectMode_M (1 /* GET */, &nInspectMode, &nMotionTime);

			if (nInspectMode == NOP_MODE)
			{
				nRecvCnt = 0;
				//g_nOldMotionBus = 0x4010;
			}


			if (recvBuf[0] == RAW_DATA_ID)
			{
				NiosRawData_to_PeakData(pNiosRawData, &ReadBuf, nInspectMode);
				SlvData.PeakData = ReadBuf;

				if (nSlave == 0)
				{
					nRecvCnt++;
					if ( (nInspectMode == RUN_MODE) && (nMotionTime == 1) )  //inspect in time mode
					{
						g_nXloc = (int) ( (float) nRecvCnt * 128.0f / ((float) g_nPulserPRF * g_fTimePerInch) );
						if (g_nXloc >= g_nXscale)
							nRecvCnt = -2 * g_nPulserPRF / 128;    //halt for 2 seconds

						if (g_nXloc < 0)
						{
							g_nPipeStatus = PIPE_NOT_PRESENT;
							g_nMotionBus = 0x4010;
						}
						else
						{
							g_nPipeStatus = PIPE_PRESENT;
							g_nMotionBus = 0x4011;
						}

						if ( (nRecvCnt % 10) == 0 )
							g_bStartOfRevS1 = TRUE;
						else
							g_bStartOfRevS1 = FALSE;
					}
					else    //inspection in motion mode
					{
						g_nOldXloc = g_nXloc;
						g_nXloc = (int) ( pNiosRawData->DataHead.wLocation * g_fMotionPulseLen + 0.5f);  //0.495 for the encoder on the three array hand scanner

						if (pNiosRawData->DataHead.bDin & 0x04)
						{
							g_nPipeStatus = PIPE_PRESENT;
							//g_nMotionBus = 0x4001 | ((pNiosRawData->DataHead.bDin & 0x01) << 4);  //get the direction bit

							if (g_nOldXloc > g_nXloc)
								g_nMotionBus = 0x4001 | ((pNiosRawData->DataHead.bDin & 0x01) << 14);  //going backward //get the home/away bit;  
							else
								g_nMotionBus = 0x4011 | ((pNiosRawData->DataHead.bDin & 0x01) << 14);  //going forward //get the home/away bit;
						}
						else
						{
							g_nPipeStatus = PIPE_NOT_PRESENT;
							//g_nMotionBus = 0x4000 | ((pNiosRawData->DataHead.bDin & 0x01) << 4);  //get the direction bit

							if (g_nOldXloc > g_nXloc)
								g_nMotionBus = 0x4000 | ((pNiosRawData->DataHead.bDin & 0x01) << 14);  //going backward //get the home/away bit;
							else
								g_nMotionBus = 0x4010 | ((pNiosRawData->DataHead.bDin & 0x01) << 14);  //going forward //get the home/away bit;
						}

						if ( ((pNiosRawData->DataHead.wClock / 10) == 0) && ((wClock_old / 10) != 0) )
							g_bStartOfRevS1 = TRUE;
						else
							g_bStartOfRevS1 = FALSE;
						
						wClock_old = pNiosRawData->DataHead.wClock;
						g_nPeriod = pNiosRawData->DataHead.wPeriod;
					}

					Inspection_Process_Control();
				}
			}
			else if (recvBuf[0] == ASCAN_ID)
			{
				for (i=0; i<1024; i++)
					pAscanMsg->Ascan[i] = recvBuf[i+DataHeadLength];

				memcpy( (void *) &(pAscanMsg->MstrHdr.MmiStat), (void *) recvBuf, DataHeadLength);

				/* send the packet data to MMI */
				SendMmiMsg (g_nMmiSocket, &SendBuf);

				ReadBuf.wLineStatus = 0;   //force to forgo the following inspection data process step
				ReadBuf.RdpStatus = 1;
			}
			else  //keep alive message
			{
				ReadBuf.wLineStatus = 0;   //force to forgo the following inspection data process step
				ReadBuf.RdpStatus = 1;
			}

			//g_bConnected[nSlave+1] = 1;  /* This slave is alive since we received a message from it.*/
			pInstMsgProcess->m_bConnected = 1;

			if (ReadBuf.wLineStatus != 0)
			{
				ReadBuf.RDP_number = nSlave;

				SlvData.PeakData = ReadBuf;			
			}

			if (nMotionTime == 1)   //inspection in time mode
			{
				SlvData.PeakData.xloc = g_nXloc;
			}

			if (nSlave > 0)   //if endcoder is only connected to the first array
			{
				SlvData.PeakData.xloc = g_nXloc;
			}

			SetGetSiteDefaults (1 /*GET*/, &SiteDefault);

#if 1
			switch (nInspectMode)
			{
			case RUN_MODE:
				if ( g_nPipeStatus != PIPE_PRESENT )  /* If no pipe in the system, do nothing. */
					break;
				if ( ReadBuf.wLineStatus == 0 )     /* If not inspection data, do nothing. */
					break;

				nReturn = BuildImageMap(&SlvData, nSlave, &g_bStartOfRevS1);

				if ( nReturn )
				{
					nSendCnt = 0;
					if (nReturn != IMAGE_BUF_DIM/*was == 1*/)
					{
						for (i=0; i<nReturn; i++)
						{
							pSendBuf = GetNextImageBuf();

							if ( pSendBuf != NULL )
							{
								pSendBuf->MstrHdr.MsgId = RUN_MODE;
								pSendBuf->InspHdr.Period = g_nPeriod;
								pSendBuf->InspHdr.VelocityDt = g_nVelocityDt;
								pSendBuf->InspHdr.wLineStatus = PIPE_PRESENT | INSPECT_ENABLE_TRUSCOPE;
								if (g_bRunCalJoint)
									pSendBuf->InspHdr.status[1] |= CALIBRATION_JOINT;
								pSendBuf->InspHdr.status[2] = g_nMotionBus;
								pSendBuf->InspHdr.NextJointNum = g_nJointNum;
								pSendBuf->InspHdr.JointLength = ( g_nXloc + SiteDefault.nDefaultXOffset ) % g_nXscale;
								pSendBuf->InspHdr.JointNumber = g_nStation1JointNum;
								pSendBuf->MstrHdr.nWhichWindow = g_nStation1Window;
								ComputeEcc(pSendBuf, &g_AllTholds);

								if ( (pSendBuf->InspHdr.xloc >= 0)  && g_bAnyShoeDown )
								{
									/* send the IData to MMI */
									if ( SendMmiMsg (g_nMmiSocket, pSendBuf) == FALSE )
										break;//return ERROR;
								}
								else
									SendPipeLocation(0, &SiteDefault);

								nSendCnt++;
							}
						}
					}

					/* send real time messages */
					//nSendCnt = 0;
					//printf ("***** Real time message *****\n");
					while ( GetNextRealTimeImageBuf(&pSendBuf) )
					{
						//printf ("real time message 1\n\n");
						if (pSendBuf != NULL)
						{
							//printf ("real time message 2\n\n");
							pSendBuf->MstrHdr.MsgId = RUN_MODE;
							pSendBuf->InspHdr.wLineStatus = PIPE_PRESENT | INSPECT_ENABLE_TRUSCOPE | REAL_TIME_DATA;
							pSendBuf->InspHdr.Period = g_nPeriod;
							pSendBuf->InspHdr.VelocityDt = g_nVelocityDt;
							pSendBuf->InspHdr.JointNumber = g_nStation1JointNum;
							pSendBuf->MstrHdr.nWhichWindow = g_nStation1Window;
							if (g_bRunCalJoint)
								pSendBuf->InspHdr.status[1] |= CALIBRATION_JOINT;
							pSendBuf->InspHdr.status[2] = g_nMotionBus;
							pSendBuf->InspHdr.NextJointNum = g_nJointNum;
							pSendBuf->InspHdr.JointLength = ( g_nXloc + SiteDefault.nDefaultXOffset ) % g_nXscale;
							if (pSendBuf->InspHdr.xloc >= 0)
							{
								//logMsg("wLinStatus = 0x%08X\n",pSendBuf->InspHdr.wLineStatus,0,0,0,0,0);
								//printf("xloc = %d, EchoBit = 0x%08X\n",pSendBuf->InspHdr.xloc,pSendBuf->InspHdr.EchoBit[0]);
								//printf("SegWallMin[25] = %d, SegMinChnl[25] = %d\n\n", pSendBuf->UtInsp.SegWallMin[25],pSendBuf->UtInsp.SegMinChnl[25]);
								/* send the IData to MMI */

								//if ( SendMmiMsg (g_nMmiSocket, pSendBuf) == FALSE )
									//break;//return ERROR;

								for (i=0; i<MAX_SHOES; i++)
									pSendBuf->InspHdr.EchoBit[i] = 0x0000;

								//nSendCnt++;
							}
						}
					}
					if (nSendCnt == 0)
						SendPipeLocation(0, &SiteDefault);
					//printf ("******************************\n\n");
				}
				break;

			case CAL_MODE:
				if (ReadBuf.wLineStatus == 0)
					break;
				BuildCalMsg (&SlvData, nSlave);
				if (nSlave == 0)
				{
					if (nRecvCnt >= g_nPulserPRF/1000)
					{
						nRecvCnt = 0;

						memcpy ((void *) &SendCalBuf, (void *) &CalBuf, sizeof(I_MSG_CAL));

						/* send the calibration data to MMI */
						SendMmiMsg (g_nMmiSocket, &SendCalBuf);

						/* clear CalBuf */
						memset( (void *) &CalBuf, 0, sizeof(I_MSG_CAL));
						for (i=0; i<MAX_SHOES; i++)
							for (ic=0; ic<10; ic++)
								CalBuf.Shoe[i].MinWall[ic] = 0x3FFF;
					}
				}
				break;

			default:
				break;
			}
				
#endif
			
		}

	}
	
	return 0;
}




/**************************************************************************** 
* 
* tcpServerWorkTask_WD - receive and process slave data
* This doesn't go anywhere
* 
* RETURNS: 0 or 1. 
*/ 
UINT tcpServerWorkTask_WD (void *pSlave)
{
	BYTE recvBuf[1024];
	int nSlave = *( (int *) pSlave);

	while (1)
	{
		if (g_socketWallDisplay >= 0)
		{
			if (readn(g_socketWallDisplay, (char *) recvBuf, 1024) <= 0)
			{
				closesocket (g_socketWallDisplay);
				g_socketWallDisplay = -1;

				return 0;
			}
		}

	}
	
	return 0;
}


// a thread to send wall data to the Wall Bar Display GUI
// gets wall bar data thru a pipe from NiosRawData_to_PeakData
// which was called by tcpServerWorkTask (thread)

UINT tSendWallDisplayMsg(void *dummy)
{
	int rc;
//	I_MSG_RUN SendBuf;
	I_MSG_WD BufSend;
//	I_MSG_RUN *pSendBuf = (I_MSG_RUN *) &SendBuf;
	DWORD nMsgNum = 1;
	BYTE  *pByte = (BYTE *) &BufSend.status[0];
	WORD  readBuf[500];

	while (1)
	{
		if (g_nRunSendWallDisplayMsgThread == 0)
			return 0;

		rc = readnpipe(g_hPipeWallDisplay[READ], (char *) &readBuf, 1000);	// move 1000 bytes from pipe to readBuf

		if (rc == 1000)
		{	// pipe read 1000 bytes
			memcpy ( (void *) BufSend.Wall, (void *) readBuf, 1000);	// copy readbuf to BufSend

			BufSend.nNorWall = (WORD) (g_AllTholds.fWall * 1000.0f + 0.5f);  //nominal wall
			pByte[0] = (BYTE) (10 * g_AllTholds.TholdWall[0]);  //min wall threshold
			pByte[1] = (BYTE) (10 * g_AllTholds.TholdWall[1]);  //min wall threshold
		

			//add IDs
			//BufSend.Mach = TRUSCAN_MACH_ID;
			//BufSend.TruscanMsgId = TRUSCAN_MSG_ID;
			BufSend.MsgNum = nMsgNum;

			//BufSend.MsgId = pSendBuf->MstrHdr.MsgId;
			//BufSend.wLineStatus = pSendBuf->InspHdr.wLineStatus;
			//BufSend.xloc = pSendBuf->InspHdr.xloc;
			//BufSend.Period = pSendBuf->InspHdr.Period;
			//BufSend.JointNumber = pSendBuf->InspHdr.JointNumber;
			//BufSend.MinWall = pSendBuf->UtInsp.MinWall;
			//BufSend.MaxWall = pSendBuf->UtInsp.MaxWall;
			//BufSend.MinWallClk = pSendBuf->UtInsp.MinWallClk;
			//BufSend.MaxWallClk = pSendBuf->UtInsp.MaxWallClk;

			BufSend.Num_of_Beams = 25;
			BufSend.Num_of_Scans = 20;
			BufSend.status[2] = g_nShowWallBars;

			if (g_socketWallDisplay >= 0)
			{
				//rc = send( g_socketAGS, (char *) pSendBuf, sizeof(I_MSG_RUN), 0 );
				rc = send( g_socketWallDisplay, (char *) &BufSend, sizeof(I_MSG_WD), 0 );

				if ( rc <= 0 )
				{
					nMsgNum = 1;
					_close(g_socketWallDisplay);
					g_socketWallDisplay = -1;
					::Sleep(1000);
				}
				else if ( rc < sizeof(I_MSG_WD) )
					return 0;
				else
				{	// 1 whole msg sent
					nMsgNum++;
				}
			}	// g_socketWallDisplay >= 0
			else
			{	// no socket to WallDisplay program yet
				nMsgNum = 1;
				::Sleep(2000);
			}
		}	//	// pipe read 1000 bytes
		else if (rc >= 0)
		{
			;	// something in pipe but not the full message
		}
		else
		{
			return 0;
		}
	}	// while 1

	return 0;
}



/********** Control inspection process *************************************
*  Respond to pipe in and out
*/
void Inspection_Process_Control()
{
	switch (g_nOldMotionBus & 0x00000001)    /* look at the old pipe present status */
	{
	case 0:		/* pipe is not present */
		switch (g_nMotionBus & 0x00000001)     /* look at the new pipe present status */
		{
		case 0:		/* pipe is not present */
			break;

		case 1:		/* pipe is present. */
			PipeInProcess();
			g_nStation1Window = g_nNextWindow;
			g_nStation1JointNum = g_nJointNum;
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

	g_nOldMotionBus = g_nMotionBus;
}


/****************************************************************************
* Helper:  send messages to MMI over TCP socket
*
*/
BOOL SendMmiMsg(int socketFD, I_MSG_RUN *pSendBuf)
{
	if (socketFD < 0)
	{
		return FALSE;
	}


	if ( (pSendBuf->MstrHdr.MsgId != ASCAN_MODE) && (pSendBuf->MstrHdr.MsgId != RAW_FLAW_MODE) )
	{
		pSendBuf->InspHdr.status[2] = g_nMotionBus;  /* attach motion bus */
	}


	int rc;
	static DWORD nMsgNum = 1;


	//add IDs
	pSendBuf->MstrHdr.Mach = TRUSCAN_MACH_ID;
	//pSendBuf->UtInsp.TruscanMsgId = TRUSCAN_MSG_ID;
	pSendBuf->MstrHdr.MsgNum = nMsgNum;

	if (g_nMmiSocket >= 0)
	{
		rc = send( g_nMmiSocket, (char *) pSendBuf, sizeof(I_MSG_RUN), 0 );
		if ( rc <= 0 )
		{
			nMsgNum = 1;
			return FALSE;
		}
		else if ( rc < sizeof(I_MSG_RUN) )
		{
			return FALSE;
		}
		else
		{
			nMsgNum++;
		}

	}
	else
	{
		nMsgNum = 1;
		return FALSE;
	}

	return TRUE;
}


void BuildCalMsg(UDP_SLAVE_DATA *pSlvData, int nSlave)
{
	int ci, nDispCh, nS, nC;		/* chnl index */
	int ct;		/* channel type (wall/long/tran/oblq) */
	WORD  wall;		/* temp wall value*/
	BYTE   flaw;
	CHANNEL_CONFIG2 ChannelCfg;

	I_MSG_CAL *pCalBuf = &CalBuf;

	pCalBuf->InspHdr.wLineStatus = 1;
	pCalBuf->MstrHdr.MsgId = CAL_MODE;
	pCalBuf->InspHdr.status[1] = WALL_STATUS_NOT_EU;

	pCalBuf->InspHdr.NextJointNum = g_nJointNum;

	SetGetChannelCfg (1 /* GET */, &ChannelCfg, nSlave);

	for ( ci = 0; ci < g_ArrayScanNum[nSlave]; ci++)
	{	/* look at all chnls from slave */
		nDispCh = FindDisplayChannel(nSlave, ci);
		nS = nDispCh / MAX_CHANNEL_PER_INSTRUMENT;
		nC = nDispCh % MAX_CHANNEL_PER_INSTRUMENT;

		ct = ChannelCfg.Ch[nS][nC].Type;

		switch ( ct)
		{	/* chnl type */
		default:
			break;

		case IS_WALL:
			wall = pSlvData->PeakData.MinWall[ci];
			if (pCalBuf->Shoe[nS].MinWall[nC] > wall)
				pCalBuf->Shoe[nS].MinWall[nC] = wall;
			wall = pSlvData->PeakData.MaxWall[ci];
			if (pCalBuf->Shoe[nS].MaxWall[nC] < wall)
				pCalBuf->Shoe[nS].MaxWall[nC] = wall;
			flaw = (BYTE) pSlvData->PeakData.Amp[ci][0];
			if (pCalBuf->Shoe[nS].Amp[nC][0] < flaw)
				pCalBuf->Shoe[nS].Amp[nC][0] = flaw;

			flaw = (BYTE) pSlvData->PeakData.Amp[ci][1];
			if (pCalBuf->Shoe[nS].Amp[nC][1] < flaw)
				pCalBuf->Shoe[nS].Amp[nC][1] = flaw;
			break;

		case IS_NOTHING:
		case IS_LONG:
		case IS_TRAN:
		case IS_OBQ1:
		case IS_OBQ2:
		case IS_OBQ3:
			flaw = (BYTE) pSlvData->PeakData.Amp[ci][0];
			if (pCalBuf->Shoe[nS].Amp[nC][0] < flaw)
				pCalBuf->Shoe[nS].Amp[nC][0] = flaw;

			flaw = (BYTE) pSlvData->PeakData.Amp[ci][1];
			if (pCalBuf->Shoe[nS].Amp[nC][1] < flaw)
				pCalBuf->Shoe[nS].Amp[nC][1] = flaw;
			break;
		
		}	/* chnl type */
	}	/* look at all 10 chnls from slave */

}	/* CalpMsg */



void SetGetChannelCfg (int nSetGet /* 0=SET, 1=Get */, CHANNEL_CONFIG2 *pChnlCfg, int nSlave)
{
	static CHANNEL_CONFIG2 ChannelCfg;
	int i;
    static int semM = 0; 

    if (semM == 0)
	{
		semM = 1;
		memset ( (void *) &ChannelCfg, 0, sizeof(CHANNEL_CONFIG2) );
	}

	switch(nSetGet)
	{
	case 0:  /* SET */
		for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
		{
			ChannelCfg.Ch[nSlave][i].Type = pChnlCfg->Ch[nSlave][i].Type;
			ChannelCfg.Ch[nSlave][i].cXOffset = pChnlCfg->Ch[nSlave][i].cXOffset;
			ChannelCfg.Ch[nSlave][i].cWOffset = pChnlCfg->Ch[nSlave][i].cWOffset;
		}
		ChannelCfg.cClockOffset = pChnlCfg->cClockOffset;
		break;
	case 1:  /* GET */
		memcpy ( (void *) pChnlCfg, (void *) &ChannelCfg, sizeof (CHANNEL_CONFIG2) );
		break;
	default:
		break;
	}

}


/********************************************************
* Convert phased array Nios raw data packet SRawDataPacket to PEAK_DATA
* Called from the thread tcpServerWorkTask. Uses pipes to "send" Wall bar data to another thread
*/
void NiosRawData_to_PeakData(SRawDataPacket *pInspData, PEAK_DATA *pPeakData, int nInspectMode)
{
	int i, j, k, nSeq;  /* generic looper */
	BYTE RcvrSequence[MAX_CHANNEL_PER_INSTRUMENT];   /* receiver sequence enable status.  1=enabled, 0=no */
	BYTE AmpID[MAX_CHANNEL_PER_INSTRUMENT];  /* hold ID amplitude for current A-scan */
	BYTE AmpOD[MAX_CHANNEL_PER_INSTRUMENT];  /* hold OD amplitude for current A-scan */
	BYTE FlawID[MAX_CHANNEL_PER_INSTRUMENT];  /* hold maximum ID flaw amplitude for current 12 degree segment */
	BYTE FlawOD[MAX_CHANNEL_PER_INSTRUMENT];  /* hold maximum OD flaw amplitude for current 12 degree segment */
	WORD Wall[MAX_CHANNEL_PER_INSTRUMENT];   /* hold wall reading for current A-scan */
	WORD WallMin[MAX_CHANNEL_PER_INSTRUMENT]; /* hold minimum wall reading for current 12 degree segment */
	WORD WallMax[MAX_CHANNEL_PER_INSTRUMENT]; /* hold maximum wall reading for current 12 degree segment */
	static WORD Old_WallMin[NUM_OF_SLAVES][MAX_CHANNEL_PER_INSTRUMENT]; /* hold minimum wall reading for previous 12 degree segment */
	static WORD Old_WallMax[NUM_OF_SLAVES][MAX_CHANNEL_PER_INSTRUMENT]; /* hold maximum wall reading for previous 12 degree segment */
	CHANNEL_INFO ChannelInfo[MAX_CHANNEL_PER_INSTRUMENT];
	BOOL bFirstAscan = TRUE;
	float ascan_delta_t = 1.0f / ((float) g_nPulserPRF);
	int    nSlave = pPeakData->RDP_number;
	float  fWallSlope = g_WallCoef.fWallSlope[nSlave];
	int    nWallOffset = g_WallCoef.WallOffset[nSlave];
	CHANNEL_CONFIG2 ChannelCfg;
	short    nTick, nClock=pInspData->DataHead.wClock;
	short    nPeriod = pInspData->DataHead.wPeriod;
	static short nOldClock[NUM_OF_SLAVES];
	static short nOldTick;
	BOOL     bTopPipe = FALSE;
	int      nDispCh;

	static WORD  WallDisplay[500];
	static int   nWD = 0;
	static int   nScan = 0;
	static int   nTotalScan = 0;
	int   nDivider = g_nPulserPRF / (25*40);
	if (nDivider < 1) nDivider = 1;

	SetGetChannelCfg (1 /* GET */, &ChannelCfg, nSlave);

	pPeakData->RdpStatus = 1;
	//pPeakData->RDP_number = nSlave;
	pPeakData->wLineStatus = PIPE_PRESENT | INSPECT_ENABLE_TRUSCOPE;
	pPeakData->xloc = (short) ( pInspData->DataHead.wLocation * g_fMotionPulseLen + 0.5f);   //0.495 for the encoder on the three array hand scanner
	if (nPeriod > 0)
		pPeakData->ShoeAngle = pInspData->DataHead.wClock * 180 / nPeriod;
	pPeakData->bNewData = TRUE;

	for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
	{
		nDispCh = FindDisplayChannel(nSlave, i);

		if (i < g_ArrayScanNum[nSlave])
			ChannelInfo[i].channel_type = ChannelCfg.Ch[nDispCh/MAX_CHANNEL_PER_INSTRUMENT][nDispCh%MAX_CHANNEL_PER_INSTRUMENT].Type;
		else
			ChannelInfo[i].channel_type = IS_NOTHING;

		switch (ChannelInfo[i].channel_type)
		{
		case IS_LONG:
			ChannelInfo[i].id_thold = g_AllTholds.TholdLong[0];
			ChannelInfo[i].od_thold = g_AllTholds.TholdLong[1];
			ChannelInfo[i].nc_for_id = g_NcNx.Long[0];
			ChannelInfo[i].nc_for_od = g_NcNx.Long[1];
			break;
		case IS_TRAN:
			ChannelInfo[i].id_thold = g_AllTholds.TholdTran[0];
			ChannelInfo[i].od_thold = g_AllTholds.TholdTran[1];
			ChannelInfo[i].nc_for_id = g_NcNx.Tran[0];
			ChannelInfo[i].nc_for_od = g_NcNx.Tran[1];
			break;
		case IS_OBQ1:
			ChannelInfo[i].id_thold = g_AllTholds.TholdOblq1[0];
			ChannelInfo[i].od_thold = g_AllTholds.TholdOblq1[1];
			ChannelInfo[i].nc_for_id = g_NcNx.Oblq1[0];
			ChannelInfo[i].nc_for_od = g_NcNx.Oblq1[1];
			break;
		case IS_OBQ2:
			ChannelInfo[i].id_thold = g_AllTholds.TholdOblq2[0];
			ChannelInfo[i].od_thold = g_AllTholds.TholdOblq2[1];
			ChannelInfo[i].nc_for_id = g_NcNx.Oblq2[0];
			ChannelInfo[i].nc_for_od = g_NcNx.Oblq2[1];
			break;
		case IS_OBQ3:
			ChannelInfo[i].id_thold = g_AllTholds.TholdOblq3[0];
			ChannelInfo[i].od_thold = g_AllTholds.TholdOblq3[1];
			ChannelInfo[i].nc_for_id = g_NcNx.Oblq3[0];
			ChannelInfo[i].nc_for_od = g_NcNx.Oblq3[1];
			break;
		case IS_WALL:
			ChannelInfo[i].id_thold = g_AllTholds.TholdLamin[0];
			ChannelInfo[i].od_thold = g_AllTholds.TholdLamin[1];
			ChannelInfo[i].TholdWallThds[0] = g_AllTholds.TholdWallThds[0];
			ChannelInfo[i].TholdWallThds[1] = g_AllTholds.TholdWallThds[1];
			ChannelInfo[i].nc_for_id = g_NcNx.Lamin[0];
			ChannelInfo[i].nc_for_od = g_NcNx.Lamin[1];
			ChannelInfo[i].nx_for_wall = g_NcNx.Wall[0];
			break;
		default:
			break;
		}

		if (nInspectMode == CAL_MODE)
		{
			ChannelInfo[i].id_thold = 5;
			ChannelInfo[i].od_thold = 5;
			ChannelInfo[i].nc_for_id = 1;
			ChannelInfo[i].nc_for_od = 1;
		}
	}

	for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
	{
		FlawID[i] = 0;
		FlawOD[i] = 0;
		WallMin[i] = 0xFFFF;
		WallMax[i] = 0;
	}
	pPeakData->EchoBit = 0x0000;

	for (j=0; j<128; j++)  /* loop through A-scans */
	{	/* loop through A-scans */
		for (k=0; k<MAX_CHANNEL_PER_INSTRUMENT; k++)
			RcvrSequence[k] = 0;  //reset

 		//nSeq = ( g_SequenceLength + pInspData->DataHead.bSeq - (127 - j) % g_SequenceLength ) % g_SequenceLength;  //actual sequence if bSeq is the last one
 		nSeq = ( g_SequenceLength[nSlave] + pInspData->DataHead.bSeq + j % g_SequenceLength[nSlave] ) % g_SequenceLength[nSlave];  //actual sequence if bSeq is the first one

		if (g_nPhasedArrayScanType[nSlave] == LONG_24_BEAM_800)
		{
			if (nSeq > 7)
				continue;
		}

		if (g_nPhasedArrayScanType[nSlave] == LONG_24_BEAM_080)
		{
			if ( (nSeq < 8) || (nSeq > 15) )
				continue;
		}

		i = nSeq % g_ArrayScanNum[nSlave];
  		
		RcvrSequence[i] = 0x80;  /* get receiver sequence bit */

		AmpID[i] = pInspData->RawData[j].bAmp2 / 2;
		AmpOD[i] = pInspData->RawData[j].bAmp3 / 2;

		Wall[i] = (WORD) ((float) pInspData->RawData[j].wTof4 * 2.904f * 0.5f);  //raw data in 12.5ns

		Wall[i] = (WORD) (  ((float) Wall[i])*fWallSlope + (float) nWallOffset ); 

		if (nPeriod > 0)
		{
  			if ( nClock >= nOldClock[nSlave] )
				nTick = (nClock + (nClock - nOldClock[nSlave]) * j / 127) * 180 / nPeriod;
			else
				nTick = (nClock + (nPeriod - nOldClock[nSlave] + nClock) * j / 127) * 180 / nPeriod;
		}

		if (nSlave == 0)
		{
			if ( ((nTick / 5) == 0) && ((nOldTick / 5) != 0) )
			{
				g_nRawFlawBuffer = (g_nRawFlawBuffer + 1) % 2;

				memset( (void *) &g_RawFlaw[g_nRawFlawBuffer], 0, sizeof(CSCAN_REVOLUTION) );
				
				g_RawFlaw[g_nRawFlawBuffer].nXloc = pInspData->DataHead.wLocation;
				g_RawFlaw[g_nRawFlawBuffer].nMotionBus = (WORD) pInspData->DataHead.bDin;
				//SendRawFlawToMMI();
				g_bSendRawFlawToMMI = TRUE;
			}

			nOldTick = nTick;
		}

		if (g_nPhasedArrayScanType[nSlave] == WALL_25_BEAM_90_DEGREE_PROBE)
		{

			if ( (nTotalScan % nDivider) == 0 )
			{
				nWD = nScan * 25 + nSeq;
				if (nSeq == 24)
					nScan++;

				if (g_nShowWallBars > 0)
				{
					if (Wall[i] < WALL_HIGH_LIMIT)
						WallDisplay[nWD%500] = Wall[i];
					else
						WallDisplay[nWD%500] = 0;
				}
				else
				{
					WallDisplay[nWD%500] = AmpID[i];
				}

				if (nWD == 499)
				{
					nScan = 0;
					nTotalScan = 0;
					
					if (g_socketWallDisplay >= 0)
						_write ( g_hPipeWallDisplay[WRITE], (void *) WallDisplay, 1000);
				}
			}

			if (nSeq == 24)
				nTotalScan++;
		}


		NC_Process (RcvrSequence, AmpID, AmpOD, FlawID, FlawOD, ChannelInfo, nSlave, nTick, nSeq);
		NX_Process (RcvrSequence, Wall, WallMin, WallMax, ascan_delta_t, ChannelInfo, bFirstAscan, nSlave, nTick, pInspData->DataHead.wLocation, nSeq);
		bFirstAscan = FALSE;  /* We have done with the first A-scan of this pipe */
	}	// /* loop through A-scans */

	for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
	{
		if (WallMin[i] == 0xFFFF)
		{
			WallMin[i] = Old_WallMin[nSlave][i];
			WallMax[i] = Old_WallMax[nSlave][i];
		}

		pPeakData->Amp[i][0] = FlawID[i];
		pPeakData->Amp[i][1] = FlawOD[i];
		pPeakData->MinWall[i] = WallMin[i];
		pPeakData->MaxWall[i] = WallMax[i];

		switch (ChannelInfo[i].channel_type)
		{
		case IS_LONG:
		case IS_TRAN:
		case IS_OBQ1:
		case IS_OBQ2:
		case IS_OBQ3:
			if (FlawID[i] > 0 || FlawOD[i] > 0)
				pPeakData->EchoBit |= ((WORD) 0x0001) << i;
			break;
		case IS_WALL:   /* for wall channel, it is for lamination */
			if ( (FlawID[i] > 0) || (FlawOD[i] > 0) || (WallMin[i] < ChannelInfo[i].TholdWallThds[0]) || (WallMax[i] > ChannelInfo[i].TholdWallThds[1]) )
				pPeakData->EchoBit |= ((WORD) 0x0001) << i;
			break;
		default:
			break;
		}

		Old_WallMin[nSlave][i] = WallMin[i];
		Old_WallMax[nSlave][i] = WallMax[i];
	}

	nOldClock[nSlave] = nClock;
}


/******************************************************
* NC process
*
* perform the n ouf of m algorithm.
* If there are NC_ABOVE A-scans that are above the threshold
* out of NC_TOTAL consecutive A-scans, we get a flaw. 
*
*	AmpID[MAX_CHANNEL_PER_INSTRUMENT];   // hold ID amplitude for current A-scan 
*	AmpOD[MAX_CHANNEL_PER_INSTRUMENT];   // hold OD amplitude for current A-scan 
*	FlawID[MAX_CHANNEL_PER_INSTRUMENT];  // hold maximum ID flaw amplitude for current 12 degree segment 
*	FlawOD[MAX_CHANNEL_PER_INSTRUMENT];  // hold maximum OD flaw amplitude for current 12 degree segment 
*/
void NC_Process (BYTE *RcvrSequence, BYTE *AmpID, BYTE *AmpOD, BYTE *FlawID, BYTE *FlawOD, CHANNEL_INFO *ChannelInfo, int nSlave, int nTick, int nSeq)
{
	static BYTE m_AmpID[NUM_OF_SLAVES][MAX_CHANNEL_PER_INSTRUMENT][NC_TOTAL];  /* NC_TOTAL consecutive ID amplitudes */
	static BYTE m_AmpOD[NUM_OF_SLAVES][MAX_CHANNEL_PER_INSTRUMENT][NC_TOTAL];  /* NC_TOTAL consecutive OD amplitudes */
	int nChannel;  /* channel looper */
	int i;  /* generic looper */
	BYTE maxID, maxOD;
	int nc_cnt_id, nc_cnt_od;
	int iChannelCnt;

	/* current A-scan is the last one of NC_TOTAL consecutive A-scans */
	for (nChannel=0; nChannel<MAX_CHANNEL_PER_INSTRUMENT; nChannel++)
	{
		if (RcvrSequence[nChannel] & 0x80)
		{
			m_AmpID[nSlave][nChannel][NC_TOTAL-1] = AmpID[nChannel];
			m_AmpOD[nSlave][nChannel][NC_TOTAL-1] = AmpOD[nChannel];

			iChannelCnt = nSeq / g_ArrayScanNum[nSlave];
			if ( ( (nChannel == 0) || (nChannel == 1) ) && (iChannelCnt < 16) && (nSlave == 0) )
			{
				if ( g_RawFlaw[g_nRawFlawBuffer].Amp[nChannel][iChannelCnt][0][nTick%180] < AmpID[nChannel] )
					g_RawFlaw[g_nRawFlawBuffer].Amp[nChannel][iChannelCnt][0][nTick%180] = AmpID[nChannel];

				if ( g_RawFlaw[g_nRawFlawBuffer].Amp[nChannel][iChannelCnt][1][nTick%180] < AmpOD[nChannel] )
					g_RawFlaw[g_nRawFlawBuffer].Amp[nChannel][iChannelCnt][1][nTick%180] = AmpOD[nChannel];
			}
		}
	}

	/* perform CHANNEL_INFO.nc_for_id (od) out of NC_TOTAL algorithm for each channel */
	for (nChannel=0; nChannel<MAX_CHANNEL_PER_INSTRUMENT; nChannel++)
	{
		nc_cnt_id = 0;
		nc_cnt_od = 0;
		maxID = 0;
		maxOD = 0;
		if (RcvrSequence[nChannel] & 0x80)
		{
			switch(ChannelInfo[nChannel].channel_type)
			{
				case IS_LONG:
				case IS_TRAN:
				case IS_OBQ1:
				case IS_OBQ2:
				case IS_OBQ3:
				case IS_WALL:   /* for wall channel, do NC for lamination */
					for (i=0; i<NC_TOTAL; i++)
					{
						if (m_AmpID[nSlave][nChannel][i] >= ChannelInfo[nChannel].id_thold) nc_cnt_id++;
						if (maxID < m_AmpID[nSlave][nChannel][i]) maxID =  m_AmpID[nSlave][nChannel][i];
						if (m_AmpOD[nSlave][nChannel][i] >= ChannelInfo[nChannel].od_thold) nc_cnt_od++;
						if (maxOD < m_AmpOD[nSlave][nChannel][i]) maxOD =  m_AmpOD[nSlave][nChannel][i];
					
						/* discard the earliest A-scan and shift the remaining NC_TOTAL-1 A-scans one A-scan toward left */
						/* Next time we call this function, we already have NC_TOTAL-1 previous A-scans */
						if (i<NC_TOTAL-1)
						{
							m_AmpID[nSlave][nChannel][i] = m_AmpID[nSlave][nChannel][i+1];
							m_AmpOD[nSlave][nChannel][i] = m_AmpOD[nSlave][nChannel][i+1];
						}
					}
					/* if n out of m, do peak hold */
					if (nc_cnt_id >= ChannelInfo[nChannel].nc_for_id  && FlawID[nChannel] < maxID)
						FlawID[nChannel] = maxID;
					if (nc_cnt_od >= ChannelInfo[nChannel].nc_for_od  && FlawOD[nChannel] < maxOD)
						FlawOD[nChannel] = maxOD;
					break;
				default:
					break;
			}
		}
	}
	
}


/******************************************************
* NX process
// inputs
*
* Moving average of wall readings
*	InputWallReading[MAX_CHANNEL_PER_INSTRUMENT];	 // inputs wall reading for current A-scan
*	WallMin[MAX_CHANNEL_PER_INSTRUMENT]; // returns minimum averaged wall reading for current 12 degree segment 
*	WallMax[MAX_CHANNEL_PER_INSTRUMENT]; // returns maximum averaged wall reading for current 12 degree segment
*	ascan_delta_t;                       // time spanned by one A-scan in seconds.  Should be 1.0/PRF
BYTE *RcvrSequence
CHANNEL_INFO *ChannelInfo
BOOL bFirstAscan
int nSlave
int nTick
int nXloc
int nSeq

// internal variables
WORD m_WallAveragingFifo
*/
void NX_Process (BYTE *RcvrSequence, WORD *InputWallReading, WORD *WallMin, WORD *WallMax, 
				 float ascan_delta_t, CHANNEL_INFO *ChannelInfo, BOOL bFirstAscan, 
				 int nSlave, int nTick, int nXloc, int nSeq)
	{
	static WORD m_WallAveragingFifo[NUM_OF_SLAVES][MAX_CHANNEL_PER_INSTRUMENT][NX_TOTAL];  /* NX_TOTAL consecutive wall readings */
	static WORD m_FindMinWallFifo[NUM_OF_SLAVES][MAX_CHANNEL_PER_INSTRUMENT][WALL_BUF_SIZE];  /*  consecutive wall readings for finding maximum wall */
	int nChannel;  /* channel looper */
	int i, j, ic;  /* generic looper */
	static int ascan_cnt[NUM_OF_SLAVES][MAX_CHANNEL_PER_INSTRUMENT];  /* number of valid wall readings since the last invalid wall readings */
	WORD avg_wall;  /* average of NX_TOTAL  consecutive wall readings including current wall readings */
	float sum_wall;
	static float invalid_wall_t[NUM_OF_SLAVES][MAX_CHANNEL_PER_INSTRUMENT];  /* time in seconds a loss-of-coupling (invalid wall reading) event has lasted */
	int nx_for_wall;
	WORD min_max_wall;
	static BOOL bFirstCall = TRUE;

	int  iChannelCnt = 0;
	static WORD AverageWall[NUM_WALL_CHANNEL*2];  /* average of NX_TOTAL  consecutive wall readings including current wall readings */
	int   iWallChannel=0, nDeltaRev=0;
	int  nScanType;
	static int nTickOld=100;
	float fWallInch;

	/* If this is the first A-scan of current pipe, do the following initialization */
	if (bFirstCall)
	{
	    bFirstCall = FALSE;

	    for (j=0; j<NUM_OF_SLAVES; j++)
		for (nChannel=0; nChannel<MAX_CHANNEL_PER_INSTRUMENT; nChannel++)
		{
			ascan_cnt[j][nChannel] = 0;
			invalid_wall_t[j][nChannel] = 0.0;

			WallMin[nChannel] = 0xFFFF;
			WallMax[nChannel] = 0;

			for (i=0; i<NX_TOTAL; i++)
			{
				m_WallAveragingFifo[j][nChannel][i] = 0;
			}

			for (i=0; i<WALL_BUF_SIZE; i++)
			{
				m_FindMinWallFifo[j][nChannel][i] = 0;
			}
		} //for (j=0; j<NUM_OF_SLAVES; j++) & nChannel<MAX_CHANNEL_PER_INSTRUMENT
	}

	
	/* compute average of CHANNEL_INFO.nx_for_wall wall readings for each channel */
	for (nChannel=0; nChannel<MAX_CHANNEL_PER_INSTRUMENT; nChannel++)
	{
	    sum_wall = 0.0;
	    nx_for_wall = ChannelInfo[nChannel].nx_for_wall;
	    invalid_wall_t[nSlave][nChannel] += ascan_delta_t;

		iChannelCnt = (nSeq % g_SequenceLength[nSlave]) / g_ArrayScanNum[nSlave];

	    //if (RcvrSequence[nChannel])
	    if (RcvrSequence[nChannel] & 0x80)
	    {
		switch(ChannelInfo[nChannel].channel_type)
		{	// switch on chnl type
			case IS_WALL:
				switch (g_nPhasedArrayScanType[nSlave])
				{
					case THREE_SCAN_LRW_8_BEAM:
					case THREE_SCAN_LRW_8_BEAM_FOCUS:
						nScanType = 1;
						break;

					case THREE_SCAN_LRW_16_BEAM:
						nScanType = 2;
						break;

					default:
						nScanType = 0;
				}


				/* If current A-scan is a valid wall reading, do the average and window shifting */
				if ( ((RcvrSequence[nChannel] & 0x01) == 0) && (InputWallReading[nChannel] > WALL_LOW_LIMIT) && (InputWallReading[nChannel] < WALL_HIGH_LIMIT) )  /* valid wall reading */
				//if ( (InputWallReading[nChannel] > WALL_LOW_LIMIT) && (InputWallReading[nChannel] < WALL_HIGH_LIMIT) )  /* valid wall reading */
				//if ( (RcvrSequence[nChannel] & 0x01) == 0 )  /* no wall alarm, valid wall reading */
				{	// case is wall, do averaging
					m_WallAveragingFifo[nSlave][nChannel][nx_for_wall-1] = InputWallReading[nChannel];  /* current A-scan is the last one of nx_for_wall consecutive A-scans */
					m_FindMinWallFifo[nSlave][nChannel][WALL_BUF_SIZE-1] = InputWallReading[nChannel];  /* current A-scan is the last one of WALL_BUF_SIZE consecutive A-scans */

					/* moving average */
					for (i=0; i<nx_for_wall; i++)
					{
					
						sum_wall += (float) m_WallAveragingFifo[nSlave][nChannel][i];
					
						/* discard the earliest A-scan and shift the remaining NX_TOTAL-1 A-scans one A-scan toward left */
						/* Next time we call this function, we already have NX_TOTAL-1 previous A-scans */
						if (i<nx_for_wall-1)
						{
							m_WallAveragingFifo[nSlave][nChannel][i] = m_WallAveragingFifo[nSlave][nChannel][i+1];
						}
					}

					/* minimum of g_nMinMaxWallLen consecutive walls for maximum wall traces */
					min_max_wall = m_FindMinWallFifo[nSlave][nChannel][WALL_BUF_SIZE-1];
					for (i=0; i<g_nMaxWallWindowSize; i++)
					{
					
						if ( min_max_wall > m_FindMinWallFifo[nSlave][nChannel][WALL_BUF_SIZE-1-i] )
							min_max_wall = m_FindMinWallFifo[nSlave][nChannel][WALL_BUF_SIZE-1-i];
					
					}
					if (WallMax[nChannel] < min_max_wall) WallMax[nChannel] = min_max_wall;
					/* discard the earliest A-scan and shift the remaining WALL_BUF_SIZE-1 A-scans one A-scan toward left */
					/* Next time we call this function, we already have WALL_BUF_SIZE-1 previous A-scans */
					for (i=0; i<WALL_BUF_SIZE-1; i++)
					{
						m_FindMinWallFifo[nSlave][nChannel][i] = m_FindMinWallFifo[nSlave][nChannel][i+1];
					}


					if (ascan_cnt[nSlave][nChannel] < ChannelInfo[nChannel].nx_for_wall) ascan_cnt[nSlave][nChannel]++;
					if (ascan_cnt[nSlave][nChannel] == ChannelInfo[nChannel].nx_for_wall)
					{
						avg_wall = (WORD) (sum_wall/ChannelInfo[nChannel].nx_for_wall);
						if (WallMin[nChannel] > avg_wall) WallMin[nChannel] = avg_wall;
						/* if (WallMax[nChannel] < avg_wall) WallMax[nChannel] = avg_wall; */

						if ( (nScanType) && (nSlave == 0) )
						{
							if ( (nChannel == 2) && ((iChannelCnt%nScanType) == 0) )
								AverageWall[iChannelCnt/nScanType] = InputWallReading[nChannel];
						}

					}
					invalid_wall_t[nSlave][nChannel] = 0.0;
				}		// case is wall, do averaging
				else   /* invalid wall reading */
				{	// case is wall, skip averaging
					if (invalid_wall_t[nSlave][nChannel] > g_WallDropTime)  /* let the chart show zero wall reading */
					{
						WallMin[nChannel] = 0;
						WallMax[nChannel] = 0;
						invalid_wall_t[nSlave][nChannel] = 0.0;

						if ( (nScanType) && (nSlave == 0) )
						{
							if ( (nChannel == 2) && ((iChannelCnt%nScanType) == 0) )
								AverageWall[iChannelCnt/nScanType] = 0;
						}
					}
				} // case is wall, skip averaging

			// case is wall, with or without averaging
				if ( (nScanType) && (nSlave == 0) && (g_nPipeStatus == PIPE_PRESENT) )
				{	// (nScanType) && (nSlave == 0) && (g_nPipeStatus == PIPE_PRESENT)
					if ( (nChannel == 2) && ((iChannelCnt%nScanType) == 0) )
					{	// chnl is 2 and chnlcount is multiple of scantype
						if (iChannelCnt == 0)
						{	// (iChannelCnt == 0)
							if ( (nTick/6) == 0 )
							{
								if ( (nTick/6) != (nTickOld/6) )
								{
									g_nWallRevCnt += 2;
									g_nXlocRevStart = (int) (nXloc * 1.012658f + 0.5f);
									for (ic=0; ic<NUM_WALL_CHANNEL*2; ic++)
									{
										g_nWallAscanCnt[ic] = 0;
									}
								}
							}
							nTickOld = nTick;
						}	//(iChannelCnt == 0)

						switch (iChannelCnt/nScanType)
						{
						case 0:
							iWallChannel = 0;
							nDeltaRev = 0;
							break;
						case 1:
							iWallChannel = 1;
							nDeltaRev = 0;
							break;
						case 4:
							iWallChannel = 2;
							nDeltaRev = 0;
							break;
						case 5:
							iWallChannel = 3;
							nDeltaRev = 0;
							break;
						case 2:
							iWallChannel = 0;
							nDeltaRev = 1;
							break;
						case 3:
							iWallChannel = 1;
							nDeltaRev = 1;
							break;
						case 6:
							iWallChannel = 2;
							nDeltaRev = 1;
							break;
						case 7:
							iWallChannel = 3;
							nDeltaRev = 1;
							break;
						}

						if ( (iChannelCnt/nScanType) < NUM_WALL_CHANNEL*2 )
						{
							if ( (g_nWallRevCnt >= 0) && (g_nWallRevCnt < NUM_MAX_REVOL-1) && (g_nWallAscanCnt[iChannelCnt/nScanType] >= 0) && (g_nWallAscanCnt[iChannelCnt/nScanType] < NUM_MAX_ASCAN) )
							{
								g_RawWall[g_nWallRevCnt+nDeltaRev].WallAscan[g_nWallAscanCnt[iChannelCnt/nScanType]].Wall[iWallChannel] = AverageWall[iChannelCnt/nScanType];
								g_RawWall[g_nWallRevCnt+nDeltaRev].nNumAscan[iWallChannel] = g_nWallAscanCnt[iChannelCnt/nScanType] + 1;
								g_RawWall[g_nWallRevCnt+nDeltaRev].nXloc = (short) (g_nXlocRevStart + nDeltaRev * (0.31f / 0.5f) + 0.5f);
								fWallInch = (float) AverageWall[iChannelCnt/nScanType]/1000.0f;
								g_RawWall[g_nWallRevCnt+nDeltaRev].fCrossArea[iWallChannel] += fWallInch*(g_AllTholds.fOD-g_AllTholds.fWall);  /* compute cross-sectional area t*(OD-t) in squared inch */
								g_RawWallHead.nNumRev = g_nWallRevCnt + 2;
								g_nWallAscanCnt[iChannelCnt/nScanType]++;
							}
							if ( (g_nMotionBus & 0x0001) == 0x0001 )
							{
								g_RawWallHead.nMotionBus = g_nMotionBus;
								g_RawWallHead.JobRec.nStopXloc = (short) (g_nXlocRevStart + nDeltaRev * (0.31f / 0.5f) + 0.5f);
							}
						}
					}  // if // chnl is 2 and chnlcount is multiple of scantype
				} // (nScanType) && (nSlave == 0) && (g_nPipeStatus == PIPE_PRESENT)

				break;

			case IS_LONG:
			case IS_TRAN:
			case IS_OBQ1:
			case IS_OBQ2:
			case IS_OBQ3:
			default:
				break;
		}
	    }
	}
	}



void SetGetSiteDefaults (int nSetGet /* 0=SET, 1=Get */, SITE_SPECIFIC_DEFAULTS *pSiteDef)
{
	static SITE_SPECIFIC_DEFAULTS SiteDefault;
	static BOOL bFirstCall = TRUE;

    	if (bFirstCall)
	{
		bFirstCall = FALSE;
		memset ( (void *) &SiteDefault, 0, sizeof(SITE_SPECIFIC_DEFAULTS) );
	}

	switch(nSetGet)
	{
	case 0:  /* SET */
		memcpy ( (void *) &SiteDefault, (void *) pSiteDef, sizeof (SITE_SPECIFIC_DEFAULTS) );
		break;
	case 1:  /* GET */
		memcpy ( (void *) pSiteDef, (void *) &SiteDefault, sizeof (SITE_SPECIFIC_DEFAULTS) );
		break;
	default:
		break;
	}
}

/**********************************************Build Image Buf***************************************************/

I_MSG_RUN* GetNextImageBuf(void)
{
	I_MSG_RUN* pIBuf;

	if ( nBufcnt > 0 ) 
	{
		pIBuf = &ImageBuf[nBufout];
		nBufout++;
		nBufout &= IMAGE_BUF_DIM - 1;
		nBufcnt--;
		return pIBuf;
		/* Caller should then reset this array in the image buf structure */
	}
	return NULL;
}


/******************************************************************
*  Added Feb 7, 2003 by Y.Guo
*  Every time receive a motion pulse, check all ImageBuf.
*  If there is any flaw, send it to MMI immediately.
*/
BOOL GetNextRealTimeImageBuf(I_MSG_RUN** pIBuf)
{
	static int nFirstCall = 1;
	static int nRemainBufCnt = 0;
	static int nMaxXOffset;
	int nCurBufIndex;
	int i;

	if (nFirstCall)
	{
		nFirstCall = 0;
		nMaxXOffset = GetMaxXOffset();
		nRemainBufCnt = nMaxXOffset - GetMinXOffset() + 5;
	}

	*pIBuf = NULL;
		
	if ( nRemainBufCnt > 0 ) 
	{
		nCurBufIndex = (nBufin + nMaxXOffset + 1 - nRemainBufCnt) & (IMAGE_BUF_DIM - 1);

		for (i=0; i<MAX_SHOES; i++)
		{
			if (ImageBuf[nCurBufIndex].InspHdr.EchoBit[i] > 0)
			{
				//printf ("nBufout = %d, bBufin = %d, nRealbuf = %d\n", nBufout,nBufin,nCurBufIndex);
				*pIBuf = &ImageBuf[nCurBufIndex];
				break;
			}
		}
		nRemainBufCnt--;
		return TRUE;
	}
	nFirstCall = 1;
	return FALSE;
}


void InitImageBufArray(void)
{
	int i, nSlave, ct,ci;
	int MaxXOffset;
	CHANNEL_CONFIG2 ChannelCfg;
	WORD *pWord;

	MaxXOffset = GetMaxXOffset();
	//MaxXOffset = 0;

	memset( (void *) &ImageBuf, 0, sizeof(ImageBuf));

	nBufout = nPreviousX = nMaxX = 0;
	nBufin = IMAGE_BUF_OUTPUT_DELAY;
	nBufcnt = IMAGE_BUF_OUTPUT_DELAY + MaxXOffset + 1;

	for (nSlave=0; nSlave<4; nSlave++)
		SetGetChannelCfg (1 /* GET */, &ChannelCfg, nSlave);

	for ( i = 0; i < IMAGE_BUF_DIM; i++)
	{
		ImageBuf[i].InspHdr.nStation = 0;
		ImageBuf[i].UtInsp.MinWall = 0x3fff;
		memset ( (void *) &ImageBuf[i].UtInsp.SegWallMin[0], 0x3f, 2*N_SEG);
		ImageBuf[i].InspHdr.status[1] |= WALL_INCLUDED;

		for (nSlave=0; nSlave<4; nSlave++)
		{
			for (ci=0; ci<10; ci++)
			{
				ct = ChannelCfg.Ch[nSlave][ci].Type;

				switch ( ct)
				{	/* chnl type */
				case IS_NOTHING:
				default:
					break;

				case IS_WALL:
					pWord = (WORD *) &ImageBuf[i].UtInsp.GateMaxAmp[nSlave*20+ci*2];
					*pWord = 0x3fff;
					break;
				}
			}
		}
	}
}


void EraseImageBuf(I_MSG_RUN *pIB)
{
	CHANNEL_CONFIG2 ChannelCfg;
	int nSlave, ci, ct;
	WORD *pWord;

	for (nSlave=0; nSlave<4; nSlave++)
		SetGetChannelCfg (1 /* GET */, &ChannelCfg, nSlave);

	/* reset/erase a 1 inch segment in the image buf ARRAY*/
	memset( (void *) pIB, 0, sizeof(I_MSG_RUN));
	pIB->UtInsp.MinWall = 0x3fff;
	memset ( (void *) &pIB->UtInsp.SegWallMin[0], 0x3f, 2*N_SEG);
	pIB->InspHdr.status[1] |= WALL_INCLUDED;

	for (nSlave=0; nSlave<4; nSlave++)
	{
		for (ci=0; ci<10; ci++)
		{
			ct = ChannelCfg.Ch[nSlave][ci].Type;

			switch ( ct)
			{	/* chnl type */
			case IS_NOTHING:
			default:
				pIB->UtInsp.GateMaxAmp[nSlave*20+ci*2] = 0;
				pIB->UtInsp.GateMaxAmp[nSlave*20+ci*2+1] = 0;
				break;

			case IS_WALL:
				pWord = (WORD *) &pIB->UtInsp.GateMaxAmp[nSlave*20+ci*2];
				*pWord = 0x3fff;
				break;
			}
		}
	}
}

void BackwardEraseImageBuf(int nBufStart)
{
	int i;
	CHANNEL_CONFIG2 ChannelCfg;
	int nSlave, ci, ct;
	WORD *pWord;

	for (nSlave=0; nSlave<4; nSlave++)
		SetGetChannelCfg (1 /* GET */, &ChannelCfg, nSlave);

	for ( i = nBufStart; i < IMAGE_BUF_DIM; i++)
	{
		memset( (void *) &ImageBuf[i], 0, sizeof(I_MSG_RUN));
		ImageBuf[i].InspHdr.nStation = 0;
		ImageBuf[i].UtInsp.MinWall = 0x3fff;
		memset ( (void *) &ImageBuf[i].UtInsp.SegWallMin[0], 0x3f, 2*N_SEG);
		ImageBuf[i].InspHdr.status[1] |= WALL_INCLUDED;

		for (nSlave=0; nSlave<4; nSlave++)
		{
			for (ci=0; ci<10; ci++)
			{
				ct = ChannelCfg.Ch[nSlave][ci].Type;

				switch ( ct)
				{	/* chnl type */
				case IS_NOTHING:
				default:
					ImageBuf[i].UtInsp.GateMaxAmp[nSlave*20+ci*2] = 0;
					ImageBuf[i].UtInsp.GateMaxAmp[nSlave*20+ci*2+1] = 0;
					break;

				case IS_WALL:
					pWord = (WORD *) &ImageBuf[i].UtInsp.GateMaxAmp[nSlave*20+ci*2];
					*pWord = 0x3fff;
					break;
				}
			}
		}
	}

}


int	BuildImageMap(UDP_SLAVE_DATA *pSlvData, int si, BOOL *bStartOfRev)
{	
	/* Build or Update the image buffer */
	/* pass in peak data from 1 slave and the slave (shoe) number */
	/* if pipe moved one x increment, buid new map.  Location
	   is relative to Center line of 1st head (ut12)
	   if Flush is true, done with pipe.  Ship MMI what we have */

	int x, dx;	/* x in inches,*/
	int y;		/* y in radial segments up to N_SEG */
	int nClk;	/* y expressed as a clock [0-11] */
	int ci, cibase;		/* chnl index and start chnl offset */
	int ct, i;		/* chnl index, channel type (wall/long/tran/oblq) */
	
	int nReturn;	/* Non zero when pipe moves 1 inch */
	int nSegDiv;	/* Segment divider.  180/N_SEG; */

	short  wall;		/* temp wall value*/
	BYTE   flaw;
	WORD	wLineStatus;	/* local */
	int	nXloc;		/* local copy of center of shoe loc */
	WORD *pWord;
	int nInspectMode, nMotionTime;
	int nChnlStart=0, nChnlStop=10, nS, nC;

	nChnlStart = FindDisplayChannel(si, 0);
	nChnlStop = nChnlStart + g_ArrayScanNum[si] - 1;

	CHANNEL_CONFIG2 ChannelCfg;
	SITE_SPECIFIC_DEFAULTS SiteDefault;

	SetGetChannelCfg (1 /* GET */, &ChannelCfg, si);
	SetGetSiteDefaults (1 /*GET*/, &SiteDefault);
	ChannelCfg.cClockOffset = SiteDefault.nDefaultClockOffset * 15;
	SetGetInspectMode_M(1 /*GET*/, &nInspectMode, &nMotionTime);
	
	nReturn =  0;
	if (*bStartOfRev)
	{
		nReturn = IMAGE_BUF_DIM;    //2;
		*bStartOfRev = FALSE;
	}
		 
	cibase = 0; //10*si;		/* starting chnl number for each shoe */
	
	
	nSegDiv = 180/N_SEG;	/* FOR 30 SEGS, 6 tick increments */

	wLineStatus = pSlvData->PeakData.wLineStatus;

	// At sheldon, found all flaws 3 inches short of true location. 5/02/02
	// Default shoe offset defined in global.h

	nXloc = pSlvData->PeakData.xloc + SiteDefault.nDefaultXOffset;/* + nShoeXOffset;*/


	if ( nPreviousX == 0 )
	{
		nPreviousX = nXloc;
		nMaxX = nXloc;
		nReturn = IMAGE_BUF_DIM;
	}

	if ( nPreviousX != nXloc) 
	{
		if (nXloc > nMaxX)
		{
			nBufcnt += nXloc - nMaxX;	/* image buffer management */
			nReturn =  nXloc - nMaxX;   //1;
			nMaxX = nXloc;
		}
		else
			nReturn = IMAGE_BUF_DIM;   //2  /* send real time messages only */
		nBufin += nXloc - nPreviousX;
		nPreviousX = nXloc;
		nBufin &= IMAGE_BUF_DIM - 1;	/* index from 0-IMAGE_BUF_DIM - 1 */
		ImageBuf[nBufin].InspHdr.xloc =  nXloc;

		if ( (g_nMotionBus & 0x0010) != 0x0010 )  /* going backward */
		{
			nMaxX = ImageBuf[nBufin].InspHdr.xloc;
			if ( nBufin < nBufout )
				nBufout = nBufin;
			if ( (nBufin-nBufout) <= (g_nMaxXSpan+2) )
				nMaxX = ImageBuf[nBufout].InspHdr.xloc + g_nMaxXSpan + 2;
			BackwardEraseImageBuf(nBufin+1);

			return nReturn;
		}
	}

	if ( (g_nMotionBus & 0x0010) != 0x0010 )  /* going backward */
		return nReturn;

	if ( !BoxIsEnabled(si) )
		return nReturn;

	/* build image for wall, long, tran and obq flaws based on xdcr type */

	if  (nXloc < 1)  return 0;

	if (pSlvData->PeakData.wLineStatus & PIPE_PRESENT )
	{	/* update structure in 1 inch bands */

		for ( ci = nChnlStart; ci <= nChnlStop; ci++)  /* chnl loop */
		{
			nS = ci / MAX_CHANNEL_PER_INSTRUMENT;
			nC = ci % MAX_CHANNEL_PER_INSTRUMENT;

			/*  Chnl indx fixes x loc */
			if ( g_nMotionBus & 0x4000 )			/* Away */
				dx = ChannelCfg.Ch[nS][nC].cXOffset;	/* true index in image buf   */
			else						/* Toward */
				dx = 0 - ChannelCfg.Ch[nS][nC].cXOffset - g_nMaxXSpan;	/* true index in image buf   */

			if (nMotionTime == 0)  //inspection in motion mode
				x = (nBufin + dx) & (IMAGE_BUF_DIM - 1);	/* true index in image buf   */
			else		       //inspection in time mode
				x = (nBufout + 2) & (IMAGE_BUF_DIM - 1);	/* true index in image buf   */

			y = (pSlvData->PeakData.ShoeAngle 
				+ ChannelCfg.Ch[nS][nC].cWOffset*3    /* 1 count = 0.2 hour */
				+ ChannelCfg.cClockOffset + 180 + 90) % 180;

			if (y<0) y = 0;

			nClk = y/15;	/* 0-11 clks */
			y /= nSegDiv;
	
			ct = ChannelCfg.Ch[nS][nC].Type;


			if ( ct != IS_NOTHING )
			{
			
				ImageBuf[x].InspHdr.wLineStatus |= wLineStatus;
				ImageBuf[x].InspHdr.xloc = nXloc + dx;
				//ImageBuf[x].InspHdr.JointLength = nXloc;

				//logMsg("x = %d\n\n", x,0,0,0,0,0);
				//logMsg("xloc = %d\n\n", ImageBuf[x].InspHdr.xloc,0,0,0,0,0);

				ImageBuf[x].InspHdr.EchoBit[si] |= ( pSlvData->PeakData.EchoBit & (0x0001 << (ci-nChnlStart)) );
			}

			/* if inspecting, update data values, else go to next channel */
			
			if (pSlvData->PeakData.wLineStatus & INSPECT_ENABLE_TRUSCOPE)
			switch ( ct )
			{	/* chnl type */


			case IS_NOTHING:
			default:
				break;

			case IS_WALL:
			/* min wall operations */
				wall = (short) pSlvData->PeakData.MinWall[ci-nChnlStart];
				if (ImageBuf[x].UtInsp.SegWallMin[y] > wall)
					{
					ImageBuf[x].UtInsp.SegWallMin[y] = wall;
					ImageBuf[x].UtInsp.SegMinChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MinWall > wall)
					{
					ImageBuf[x].UtInsp.MinWall = wall;
					ImageBuf[x].UtInsp.MinWallChnl = cibase+ci;
					ImageBuf[x].UtInsp.MinWallClk = nClk;
					}

				//all flaw, do wall min only
				if (ci < 40)
				{
					pWord = (WORD *) &ImageBuf[x].UtInsp.GateMaxAmp[ci*2];
					if (*pWord > wall)
					{
						*pWord = wall;
						ImageBuf[x].UtInsp.GateMaxClk[ci*2] = y;
					}
				}

				/* max wall operations */
				wall = (short) pSlvData->PeakData.MaxWall[ci-nChnlStart];
				
				if (ImageBuf[x].UtInsp.SegWallMax[y] < wall)
					{
					ImageBuf[x].UtInsp.SegWallMax[y] = wall;
					ImageBuf[x].UtInsp.SegMaxChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxWall < wall)
					{
					ImageBuf[x].UtInsp.MaxWall = wall;
					ImageBuf[x].UtInsp.MaxWallChnl = cibase+ci;
					ImageBuf[x].UtInsp.MaxWallClk = nClk;
					}

				/* ID gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][0];
				if (ImageBuf[x].UtInsp.SegLamLFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegLamLFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegLamLChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxLamLFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxLamLFlaw = flaw;
					ImageBuf[x].UtInsp.LamLChnl= cibase+ci;
					ImageBuf[x].UtInsp.LamLClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 0
				/* Store Max sig amp for each chnl and its clock loc */
				i = (cibase+ci) << 1;	/* dbl start loc for byte array */
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = nClk;	/* convert to clock */
					}
				i++;	/* index next gate */
#endif
				/* OD gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][1];
				if (ImageBuf[x].UtInsp.SegLamAFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegLamAFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegLamAChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxLamAFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxLamAFlaw = flaw;
					ImageBuf[x].UtInsp.LamAChnl= cibase+ci;
					ImageBuf[x].UtInsp.LamAClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 0
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = nClk;	/* convert to clock */
					}
#endif
				break;
			case IS_OBQ3:     /* use oblique3 to do lamination */
				/* ID gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][0];
				if (ImageBuf[x].UtInsp.SegQ3idFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegQ3idFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegQ3idChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxQ3idFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxQ3idFlaw = flaw;
					ImageBuf[x].UtInsp.Q3idChnl= cibase+ci;
					ImageBuf[x].UtInsp.Q3idClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 1
				/* Store Max sig amp for each chnl and its clock loc */
				i = (cibase+ci) << 1;	/* dbl start loc for byte array */
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = y;	/* convert to clock */
					}
				i++;	/* index next gate */
#endif
				/* OD gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][1];
				if (ImageBuf[x].UtInsp.SegQ3odFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegQ3odFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegQ3odChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxQ3odFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxQ3odFlaw = flaw;
					ImageBuf[x].UtInsp.Q3odChnl= cibase+ci;
					ImageBuf[x].UtInsp.Q3odClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 1
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = y;	/* convert to clock */
					}
#endif
				break;

			case IS_LONG:
				/* ID gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][0];

				if (ImageBuf[x].UtInsp.SegLidFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegLidFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegLidChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxLidFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxLidFlaw = flaw;
					ImageBuf[x].UtInsp.LidChnl= cibase+ci;
					ImageBuf[x].UtInsp.LidClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 1
				/* Store Max sig amp for each chnl and its clock loc */
				i = (cibase+ci) << 1;	/* dbl start loc for byte array */
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = y;	/* convert to clock */
					}
				i++;	/* index next gate */
#endif

				/* OD gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][1];
				if (ImageBuf[x].UtInsp.SegLodFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegLodFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegLodChnl[y] = cibase+ci;
					}
				if (ImageBuf[x].UtInsp.MaxLodFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxLodFlaw = flaw;
					ImageBuf[x].UtInsp.LodChnl= cibase+ci;
					ImageBuf[x].UtInsp.LodClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 1
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = y;	/* convert to clock */
					}
#endif
				break;

			case IS_TRAN:
				/* ID gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][0];
				if (ImageBuf[x].UtInsp.SegTidFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegTidFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegTidChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxTidFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxTidFlaw = flaw;
					ImageBuf[x].UtInsp.TidChnl= cibase+ci;
					ImageBuf[x].UtInsp.TidClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 1
				/* Store Max sig amp for each chnl and its clock loc */
				i = (cibase+ci) << 1;	/* dbl start loc for byte array */
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = y;	/* convert to clock */
					}
				i++;	/* index next gate */
#endif

				/* OD gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][1];
				if (ImageBuf[x].UtInsp.SegTodFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegTodFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegTodChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxTodFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxTodFlaw = flaw;
					ImageBuf[x].UtInsp.TodChnl= cibase+ci;
					ImageBuf[x].UtInsp.TodClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 1
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = y;	/* convert to clock */
					}
#endif
				break;

			case IS_OBQ1:
				/* ID gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][0];
				if (ImageBuf[x].UtInsp.SegQ1idFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegQ1idFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegQ1idChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxQ1idFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxQ1idFlaw = flaw;
					ImageBuf[x].UtInsp.Q1idChnl= cibase+ci;
					ImageBuf[x].UtInsp.Q1idClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 1
				/* Store Max sig amp for each chnl and its clock loc */
				i = (cibase+ci) << 1;	/* dbl start loc for byte array */
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = y;	/* convert to clock */
					}
				i++;	/* index next gate */
#endif
				/* OD gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][1];
				if (ImageBuf[x].UtInsp.SegQ1odFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegQ1odFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegQ1odChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxQ1odFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxQ1odFlaw = flaw;
					ImageBuf[x].UtInsp.Q1odChnl= cibase+ci;
					ImageBuf[x].UtInsp.Q1odClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 1
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = y;	/* convert to clock */
					}
#endif
				break;

			case IS_OBQ2:
				/* ID gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][0];
				if (ImageBuf[x].UtInsp.SegQ2idFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegQ2idFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegQ2idChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxQ2idFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxQ2idFlaw = flaw;
					ImageBuf[x].UtInsp.Q2idChnl= cibase+ci;
					ImageBuf[x].UtInsp.Q2idClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 1
				/* Store Max sig amp for each chnl and its clock loc */
				i = (cibase+ci) << 1;	/* dbl start loc for byte array */
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = y;	/* convert to clock */
					}
				i++;	/* index next gate */
#endif
				/* OD gate signal */
				flaw = (BYTE) pSlvData->PeakData.Amp[ci-nChnlStart][1];
				if (ImageBuf[x].UtInsp.SegQ2odFlaw[y] < flaw)
					{
					ImageBuf[x].UtInsp.SegQ2odFlaw[y] = flaw;
					ImageBuf[x].UtInsp.SegQ2odChnl[y] = cibase+ci;
					}

				if (ImageBuf[x].UtInsp.MaxQ2odFlaw < flaw)
					{
					ImageBuf[x].UtInsp.MaxQ2odFlaw = flaw;
					ImageBuf[x].UtInsp.Q2odChnl= cibase+ci;
					ImageBuf[x].UtInsp.Q2odClk = nClk;
					ImageBuf[x].UtInsp.FlawDetected |= 1;	/* non zero flaw value means flaw exists */
					}
#if 1
				if (ImageBuf[x].UtInsp.GateMaxAmp[i] < flaw)
					{
					ImageBuf[x].UtInsp.GateMaxAmp[i] = flaw;
					ImageBuf[x].UtInsp.GateMaxClk[i] = y;	/* convert to clock */
					}
#endif
				break;


			}	/* chnl type  */
		}	/* chnl loop */

		return nReturn;
		
	}	/* update structure in 1 inch bands */


	return 0;	/* have not moved 1 inch since last image */
}	/* Build or Update the image buffer */


void ComputeEcc(I_MSG_RUN *pRun, C_MSG_ALL_THOLD *pThold)
	{
	/* Must wait till msg is ready to be sent before calculating ecc */
	int i, j;
	int nMaxEcc, nTmpEcc;
	int nSumEcc, nSumMinWall, nSumWall;
	int nCntEcc, nCntMinWall, nCntWall;
	int nEccLimit;		/* Do not process Ecc > 0.5* nominal wall */
	int nWallSum, nEcc;       /* the sum of the wall readings opposite to each other */
	int nWallNorm = (int) (pThold->fWall*1000.0);
	short SegMin, SegMax;

	/* Invalid min walls have value 0x3f3f.  Invalid Max walls have value 0 */


	nEccLimit = (int) (pThold->fWall*1000.0 / 2.0);

	nMaxEcc = nSumEcc = nSumMinWall = nSumWall = 0;
	nCntEcc = nCntMinWall = nCntWall = 0;

	for ( i = 0; i < N_SEG; i++)
	{	/* loop thru all segments */
		j = (i+(N_SEG/2)) % N_SEG;	/* index of wall 180 degrees from i index */

		nTmpEcc = abs(pRun->UtInsp.SegWallMax[i] - pRun->UtInsp.SegWallMin[j] );
		nWallSum = pRun->UtInsp.SegWallMax[i] + pRun->UtInsp.SegWallMin[j];
		if ( (nTmpEcc < nEccLimit)  && (nWallSum > 0) )
		{
			nEcc = (nTmpEcc * 100 + nWallSum/2) / nWallSum;
			if ( nMaxEcc < nEcc) nMaxEcc = nEcc;

			nSumEcc += nEcc;
			nCntEcc++;
		}

		SegMin = pRun->UtInsp.SegWallMin[i];
		SegMax = pRun->UtInsp.SegWallMax[i];
		if ( (SegMin>nWallNorm/2)  && (SegMin<nWallNorm*3/2) && (SegMax>nWallNorm/2)  && (SegMax<nWallNorm*3/2) )
		{
			nSumMinWall += pRun->UtInsp.SegWallMin[i];
			nCntMinWall++;

			nSumWall += (pRun->UtInsp.SegWallMin[i] + pRun->UtInsp.SegWallMax[i]);
			nCntWall += 2;	/* max and min add toward average */
		}
	}	/* loop thru all segments */

	if (nCntEcc == 0) nCntEcc = 1;
	if (nCntMinWall == 0) nCntMinWall = 1;
	if (nCntWall == 0) nCntWall = 1;

	pRun->UtInsp.AvgEcc = nSumEcc/nCntEcc;
	pRun->UtInsp.AvgMinWall= nSumMinWall/nCntMinWall;
	pRun->UtInsp.AvgWall = nSumWall/nCntWall;
	pRun->UtInsp.MaxEcc = nMaxEcc;
	}



int GetMaxXOffset(void)
{
	CHANNEL_CONFIG2 ChannelCfg;
	int nSlave;
	int i;
	int MaxXOffset = -1000;

	for (nSlave=0; nSlave<MAX_SHOES; nSlave++)
	{
		SetGetChannelCfg (1 /* GET */, &ChannelCfg, nSlave);

		for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
		{
			switch ( ChannelCfg.Ch[nSlave][i].Type )
			{
			case IS_WALL:
			case IS_LONG:
			case IS_TRAN:
			case IS_OBQ1:
			case IS_OBQ2:
			case IS_OBQ3:
			case IS_NOTHING:
				if ( ChannelCfg.Ch[nSlave][i].cXOffset > MaxXOffset )
					MaxXOffset = ChannelCfg.Ch[nSlave][i].cXOffset;
				break;

			default:
				break;
			}
		}
	}

	return MaxXOffset;
}



int GetMinXOffset(void)
{
	CHANNEL_CONFIG2 ChannelCfg;
	int nSlave;
	int i;
	int MinXOffset = 1000;

	for (nSlave=0; nSlave<MAX_SHOES; nSlave++)
	{
		SetGetChannelCfg (1 /* GET */, &ChannelCfg, nSlave);

		for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
		{
			switch ( ChannelCfg.Ch[nSlave][i].Type )
			{
			case IS_WALL:
			case IS_LONG:
			case IS_TRAN:
			case IS_OBQ1:
			case IS_OBQ2:
			case IS_OBQ3:
			case IS_NOTHING:
				if ( ChannelCfg.Ch[nSlave][i].cXOffset < MinXOffset )
					MinXOffset = ChannelCfg.Ch[nSlave][i].cXOffset;
				break;

			default:
				break;
			}
		}
	}

	return MinXOffset;
}



/**********************************************************************************
* Compute the distance between the leftmost transducer and the rightmost transducer
*/
int GetMaxXSpan(void)
{
	int nMaxXSpan;

	nMaxXSpan = GetMaxXOffset() - GetMinXOffset();

	return nMaxXSpan;
}


/*******************************************************************
* Use static variable to hold current inspection mode:
*
*  RUN_MODE, CAL_MODE, PKT_MODE, NOP_MODE
*/
void SetGetInspectMode_M (int nSetGet/* 0=SET, 1=GET */, int *nMode, int *nMotionTime)
{
    static int nInspectMode = NOP_MODE;
    static int nTimeMotion = 0;   /* 0=motion, 1=time */


    switch (nSetGet)
    {
	case 0:  /* SET */
		nInspectMode = *nMode;
		nTimeMotion = *nMotionTime;
		break;
	case 1:  /* GET */
		*nMode = nInspectMode;
		*nMotionTime = nTimeMotion;
		break;
	default:
		break;

    }

}



/**********************************************************************
* FlushImageBufArray
* After the pipe exits, send all remaining Idata in the image buffer to MMI
*/
void FlushImageBufArray (void)
{
	int nInspectMode;
	I_MSG_RUN *pSendBuf;
	int nFlushCnt = 0;
	int nMotionTime;
	SITE_SPECIFIC_DEFAULTS SiteDefault;

	SetGetInspectMode_M (1 /* GET */, &nInspectMode, &nMotionTime);
	if (nInspectMode != RUN_MODE) return;

	SetGetSiteDefaults (1 /*GET*/, &SiteDefault);

	while ( (pSendBuf = GetNextImageBuf()) != NULL )
	{
		pSendBuf->MstrHdr.MsgId = RUN_MODE;
		pSendBuf->InspHdr.wLineStatus = PIPE_PRESENT | INSPECT_ENABLE_TRUSCOPE;
		pSendBuf->InspHdr.Period = g_nPeriod;
		pSendBuf->InspHdr.VelocityDt = g_nVelocityDt;
		pSendBuf->InspHdr.JointNumber = g_nStation1JointNum;
		pSendBuf->InspHdr.NextJointNum = g_nJointNum;
		pSendBuf->InspHdr.status[2] = g_nMotionBus;
		pSendBuf->MstrHdr.nWhichWindow = g_nStation1Window;
		pSendBuf->InspHdr.JointLength = g_nXloc + SiteDefault.nDefaultXOffset;

		ComputeEcc(pSendBuf, &g_AllTholds);

//		if (pSendBuf->InspHdr.xloc > 0);	// notice that this does nothing
		{
			/* send the IData to MMI */
			if ( SendMmiMsg (g_nMmiSocket, pSendBuf) == FALSE )
				return;
		}

		/* reset image buf once sent */
		/* EraseImageBuf(pSendBuf); */

		nFlushCnt++;
	}

	/* logMsg ("nFlushCnt = %d\n\n", nFlushCnt, 0,0,0,0,0); */
}



void PipeInProcess (void)
{
	InitImageBufArray();
	g_bAnyShoeDown = TRUE;
	InitRawWallBuf();
}


void PipeOutProcess (void)
{
	I_MSG_RUN  SendBuf;
	static ULONG  nPipeOutTimer = 0;  /* For raw wall system, the time interval between two consecutive pipe out 
						messages has to be greater than some number (20 seconds) */

	SITE_SPECIFIC_DEFAULTS SiteDefault;

	if (!g_bAnyShoeDown)
		return;

	SetGetSiteDefaults (1 /*GET*/, &SiteDefault);

	FlushImageBufArray();

	memset ( (void *) &SendBuf, 0, sizeof(I_MSG_RUN) );
	SendBuf.MstrHdr.MsgId = RUN_MODE;
	SendBuf.InspHdr.Period = g_nPeriod;
	SendBuf.InspHdr.VelocityDt = g_nVelocityDt;
	SendBuf.MstrHdr.nWhichWindow = g_nStation1Window;
	SendBuf.InspHdr.wLineStatus = 0;
	SendBuf.InspHdr.JointNumber = g_nStation1JointNum;
	SendBuf.InspHdr.NextJointNum = g_nJointNum;
	SendBuf.InspHdr.status[2] = g_nMotionBus;
	SendBuf.InspHdr.nStation = 0;
	SendBuf.InspHdr.status[1] |= WALL_INCLUDED;
	if (g_bRunCalJoint)
		SendBuf.InspHdr.status[1] |= CALIBRATION_JOINT;


	memset ( (void *) &SendBuf.InspHdr.EchoBit[0], 0, MAX_SHOES*2 );

	SendBuf.InspHdr.status[2] = (WORD) (g_nOldMotionBus & 0x0000FFFF);
	if ( g_nOldMotionBus & 0x00004000 )  /* away */
		SendBuf.InspHdr.JointLength = ( g_nXloc + SiteDefault.nDefaultXOffset ) % g_nXscale;
	else
		SendBuf.InspHdr.JointLength = ( g_nXloc + SiteDefault.nDefaultXOffset ) % g_nXscale;

	/* send a dummy IData to MMI so it can erase the traces automatically */
	if ( SendMmiMsg (g_nMmiSocket, &SendBuf) == FALSE )
		return;


	g_nJointNum++;

	g_bAnyShoeDown = FALSE;

	if (g_nRecordWallData)
	{
		if (!g_bWriteWallDataFile)
			g_bWriteWallDataFile = TRUE;
	}
}


void SendPipeLocation(int nStation, SITE_SPECIFIC_DEFAULTS *pSiteDefault)
{
	I_MSG_RUN SendBuf;

	EraseImageBuf(&SendBuf);

	SendBuf.MstrHdr.MsgId = RUN_MODE;
	SendBuf.InspHdr.wLineStatus = PIPE_PRESENT | INSPECT_ENABLE_TRUSCOPE | REAL_TIME_DATA;
	SendBuf.InspHdr.Period = g_nPeriod;
	SendBuf.InspHdr.VelocityDt = g_nVelocityDt;
	if (g_bRunCalJoint)
		SendBuf.InspHdr.status[1] |= CALIBRATION_JOINT;
	SendBuf.InspHdr.status[2] = g_nMotionBus;
	SendBuf.InspHdr.NextJointNum = g_nJointNum;
	SendBuf.InspHdr.nStation = 0;

	SendBuf.InspHdr.JointNumber = g_nStation1JointNum;
	SendBuf.MstrHdr.nWhichWindow = g_nStation1Window;
	SendBuf.InspHdr.JointLength = g_nXloc + pSiteDefault->nDefaultXOffset;

	/* send the IData to MMI */
	SendMmiMsg (g_nMmiSocket, &SendBuf);
}


void SendRawFlawToMMI()
{
	I_MSG_RUN SendBuf;
	I_MSG_RAW_FLAW *pRawFlaw = (I_MSG_RAW_FLAW *) &SendBuf;
	int i;
	int nBuffer = (g_nRawFlawBuffer + 1) % 2;  //send buffer
	BYTE *pByte = (BYTE *) g_RawFlaw[nBuffer].Amp;

	pRawFlaw->MstrHdr.MsgId = RAW_FLAW_MODE;
	pRawFlaw->MstrHdr.IdataStat.wLast = g_RawFlaw[nBuffer].nXloc;
	pRawFlaw->MstrHdr.IdataStat.nDup = g_RawFlaw[nBuffer].nMotionBus;
	pRawFlaw->MstrHdr.IdataStat.nRcv = (int) g_nJointNum;

	for (i=0; i<10; i++)
	{
		pRawFlaw->MstrHdr.nSlave = i;
		memcpy( (void *) pRawFlaw->Buf, (void *) &pByte[1152*i], 1152);

		SendMmiMsg (g_nMmiSocket, &SendBuf);
	}
}


UINT tSendRawFlawToMMI(void *dummy)
{
	while (g_nRunSendRawFlawToMMIThread == 1)
	{
		if (g_bSendRawFlawToMMI)
		{
			SendRawFlawToMMI();
			g_bSendRawFlawToMMI = FALSE;
		}

		::Sleep(100);
	}

	return 1;
}


void InitRawWallBuf(void)
{
	int i, j;

	memset ( (void *) g_RawWall, 0, sizeof(g_RawWall) );
	g_RawWallHead.nNumRev = 0;

	for (i=0; i<NUM_MAX_REVOL; i++)
	{
		for (j=0; j<NUM_WALL_CHANNEL; j++)
		{
			g_RawWall[i].fCrossArea[j] = 0.0f;
		}
	}

	g_nWallRevCnt = -2;
	for (i=0; i<NUM_WALL_CHANNEL*2; i++)
	{
		g_nWallAscanCnt[i] = 0;
	}

}



UINT tWriteWallDataToFile (void *dummy)
{
	char        sFileName[200];
	char	    sFileTemp[200];
	int         fDX, i, j;
//	CONFIG_REC  *pConfigRec;
	float       fNumAscan;
	int         nTryCnt=0;
	SITE_SPECIFIC_DEFAULTS SiteDefault;


	while (g_nRunWriteWallDataToFileThread == 1)
	{
		if (g_bWriteWallDataFile)
		{
			memcpy ( (void *) &(g_RawWallHead.JobRec), (void *) &g_JobRec, sizeof(JOB_REC)-8 );
			g_RawWallHead.fWall = g_JobRec.Wall;
			g_RawWallHead.fOD = g_JobRec.OD;
			g_RawWallHead.nJointNum = g_nJointNum-1;

			SetGetSiteDefaults (1 /*GET*/, &SiteDefault);
			g_RawWallHead.JobRec.nHomeXOffset = SiteDefault.nDefaultXOffset * 2;   //two counts per inch
			g_RawWallHead.JobRec.nAwayXOffset = SiteDefault.nDefaultXOffset2 * 2;   //two counts per inch

			sprintf(g_RawWallHead.JobRec.Version, "TruscanWD200501");
			g_RawWallHead.JobRec.fMotionPulseLen = g_fMotionPulseLen;  //0.5f;

			g_RawWallHead.JobRec.fChnlOffset[0] = 0.0f;
			g_RawWallHead.JobRec.fChnlOffset[1] = 0.16f;
			g_RawWallHead.JobRec.fChnlOffset[2] = 0.62f;
			g_RawWallHead.JobRec.fChnlOffset[3] = 0.78f;

#if 1
			/* nomalize the cross-sectional area */
			for (i=0; i<g_RawWallHead.nNumRev; i++)
			{
				for (j=0; j<NUM_WALL_CHANNEL; j++)
				{
					fNumAscan = (float) g_RawWall[i].nNumAscan[j];
					if ( (fNumAscan > 0.0f) && (g_RawWall[i].fCrossArea[j] > 0.0f)  && (i<NUM_MAX_REVOL) )
						g_RawWall[i].fCrossArea[j] *= (float) 3.141593/fNumAscan;
				}
			}
#endif
			/* make a copy so next joint can use g_RawWallHead and g_RawWall buffers */
			memcpy( (void *) &g_RawWallHeadCopy, (void *) &g_RawWallHead, sizeof(RAW_WALL_HEAD) );
			memcpy( (void *) g_RawWallCopy, (void *) g_RawWall, sizeof(g_RawWall) );
			//* InitRawWallBuf(); */

			// write wall data to file

			if ( strlen(g_JobRec.WO) )
				sprintf (sFileName, "C:/PhasedArray/WallData/%s/%s_%d", g_RawWallHeadCopy.JobRec.WO,g_RawWallHeadCopy.JobRec.WO,g_nJointNum-1);
			else
				sprintf (sFileName, "C:/PhasedArray/WallData/Unknown_%d", g_nJointNum-1);

			/* if the file name already exists, change it */
			i = 2;
			sprintf (sFileTemp, "%s.dat", sFileName);
			while (1)
			{
				fDX = _open (sFileTemp, _O_RDONLY);
				if (fDX != -1)  /* exist */
				{
					//printf("open ERROR = %d;  fDX = %d\n", ERROR, fDX);
					sprintf (sFileTemp, "%s_%d.dat", sFileName, i);
					i++;
					_close (fDX);
				}
				else
				{
					//printf("open else ERROR = %d;  fDX = %d\n", ERROR, fDX);
					break;
				}
			}
		
#if 1
			i = 0;
			while (1)
			{
				fDX = _open (sFileTemp, _O_RDWR | _O_CREAT | _O_BINARY);
				//fDX = open (sFileTemp, O_CREAT | O_RDWR, 0666);
				/* if we fail to open the file, abort this operation */
				if (fDX == -1)
				{
					//printf("TcuSlave - creat failed to open %s  on attempt %d\n", sFileTemp, i);
					if ( i > 3)
					{
						g_bWriteWallDataFile = FALSE;
						goto sleep_a_while;
					}
					::Sleep(500);	/* wait a second and try again */
					i++;
				}
				else
				{
					//printf("creat ERROR = %d;  fDX = %d\n", ERROR, fDX);
					break;
				}
			}

			i = _write (fDX, (void *) &g_RawWallHeadCopy, sizeof (g_RawWallHeadCopy));

			//printf ("Write wall data to file %s......", sFileName);
		
			i = _write (fDX, (void *) g_RawWallCopy, g_RawWallHeadCopy.nNumRev*sizeof(WALL_REVOLUTION));

			_close (fDX);

			g_bWriteWallDataFile = FALSE;
#endif
		}	//if g_bWriteWallDataFile==TRUE

sleep_a_while:

		::Sleep(500);
	}

	return 1;
}



int FindWhichSlave(int nChannel)
{
	int sum=0, nSlave=0, i;

	for (i=0; i<10; i++)
	{
		sum += g_ArrayScanNum[i];

		if ( (nChannel / sum) == 0)
		{
			nSlave = i;
			break;
		}
	}

	return nSlave;
}


int FindSlaveChannel(int nChannel)
{
	int sum=0, nSlaveCh=0, i;

	for (i=0; i<10; i++)
	{
		sum += g_ArrayScanNum[i];

		if ( (nChannel / sum) == 0)
		{
			nSlaveCh = nChannel - (sum - g_ArrayScanNum[i]);
			break;
		}
	}

	return nSlaveCh;
}


int FindDisplayChannel(int nArray, int nArrayCh)
{
	int nDispCh = nArrayCh, i;

	for (i=0; i<nArray; i++)
	{
		nDispCh += g_ArrayScanNum[i];
	}

	return nDispCh;
}

#if 1
void ComputeTranFocusDelay(float thickness, float zf_value, float water_path, float incident_angle, WORD *td)
{
     const double PI=3.1415926535897932;
     double v21,h,z[16],zf,u0,bet8,tilt;
     double rr,r0,r1,u,lhs,tmax;
     double r[16],alp[16],bet[16],time[16];
	 double v1=1.483;
	 int j;

/*
     switch (m_wave_value)
     {
     case 0:
           v2=3.2;
           wword="T-wave";
           break;
     case 1:
           v2=5.9;
           wword="L-wave";
           break;
     }
*/
     double v2=3.2; //Simply set velocity for shear wave

     v21=v2/v1;

     float p = 0.6f; // center to center distance between two neighboring elements

     h = thickness * 25.4f; //pipe thickness converted to mm from inches
     zf = zf_value * 25.4f - h; //z-coordinate of focus point <--input. ID: zf_value=0; OD: zf_value=h.
     tilt=0; //probe tilt angle //Simply set tilt = 0
     tilt=tilt*asin(1.)/90;

	 float m_distance_value = water_path * 25.4f;  // <--input

     bet8 = asin( sin(incident_angle * PI / 180.f) * v2 / v1) * 180.0 / PI;   //m_ref_angle; // refraction angle [35,45] <--input  
     bet8=bet8*asin(1.)/90;

//   u0: x-coordinate of element #0
	 int m_probe_orientation = 3;
     switch (m_probe_orientation)
     {
     case 0:
           z[0]=m_distance_value; //water path for element #0
           for(j=1;j<16;j++)
                z[j]=z[0]+j*p*sin(tilt);
           u0=((2*h+zf)*tan(bet8)+z[8]*tan(asin(sin(bet8)/v21)))+8*p*cos(tilt); //default value: set angle bet8 for element #8       
           break;
     case 1:
           z[15]=m_distance_value;
           for(j=14;j>=0;j--)
                z[j]=z[15]+(15-j)*p*sin(tilt);
           u0=-((2*h+zf)*tan(bet8)+z[8]*tan(asin(sin(bet8)/v21)))+8*p*cos(tilt);
           break;
     case 2:
           z[15]=m_distance_value;
           for(j=14;j>=0;j--)
                z[j]=z[15]+(15-j)*p*sin(tilt);
           u0=((2*h+zf)*tan(bet8)+z[8]*tan(asin(sin(bet8)/v21)))-7.5*p*cos(tilt);
           break;
     case 3:
           z[0]=m_distance_value;
           for(j=1;j<16;j++)
                z[j]=z[0]+j*p*sin(tilt);
           u0=-((2*h+zf)*tan(bet8)+z[8]*tan(asin(sin(bet8)/v21)))-7.5*p*cos(tilt);
           break;
     }    

/*
     m_focus_value=u0;

     result.Format("%s: h=%5.2f, z_min=%5.2f (tilt=%4.1f), u0=%5.2f (bet8=%4.1f), zf=%5.2f\n",
           wword,h,m_distance_value,m_probe_tilt,u0,m_ref_angle,zf);
     m_result_control.AddString(result);
*/

     tmax=0;
     for(j=0;j<16;j++)
     {
           if(m_probe_orientation==0 || m_probe_orientation==1)
                u=u0-j*p*cos(tilt);
           else
                u=u0+j*p*cos(tilt);

           if(u<0.) u=fabs(u);
           else u=fabs(u-0.);

           if(h==0.)
                rr=u; //when h=0 emergence point coincides with focal point
           //simulate a curved transducer focusing at (x_F,0)
           else 
           {
           r0=0.;
           r1=u;
     loop:
           rr=(r0+r1)/2;
           lhs=(v21*v21-1)*rr*rr*rr*rr-2*u*(v21*v21-1)*rr*rr*rr+
                ((v21*v21-1)*u*u+v21*v21*(2*h+zf)*(2*h+zf)-z[j]*z[j])*rr*rr+
                2*u*z[j]*z[j]*rr-u*u*z[j]*z[j];
           if(lhs<0) r0=rr;
           else r1=rr;
           if((r1-r0)>1.e-5) goto loop;}

           r[j]=rr;
           time[j]=sqrt(r[j]*r[j]+z[j]*z[j])/v1+sqrt((u-r[j])*(u-r[j])+(2*h+zf)*(2*h+zf))/v2;
           alp[j]=atan(r[j]/z[j])/PI*180;
           bet[j]=asin(v21*r[j]/sqrt(r[j]*r[j]+z[j]*z[j]))/PI*180;

           if(time[j]>tmax) tmax=time[j];
     }

     for(j=0;j<16;j++)
	 {
           td[j]= (WORD)((tmax-time[j])*1000.0);
		   //printf("td %d = %d\r\n", j, td[j]);
	 }


/*
     for(j=0;j<16;j++)
     {
           result.Format("j=%2d, r=%6.3f, alp=%5.2f, bet=%5.2f, time=%6.3f, time_delay=%d ns\n",
           j,r[j],alp[j],bet[j],time[j],(int)((tmax-time[j])*1000));

//         result.Format("j=%2d, time_delay=%d ns\n",j,td[j]);
           m_result_control.AddString(result);

     }
*/
}

#endif


void ShutDownSystem(  )
{

	char pName[ 128 ] = "Celle";
	char pPasswd[ 128 ] = "pxi";
	char pDomain[ 128 ] = "PXI";

	char pRemoteName[ 260 ] = "PhasedArray";
	char pMessage[ 512 ] = { 0 };

	HANDLE hLogonToken;
	HANDLE hAdminToken;
	HANDLE hThreadToken;
	TOKEN_PRIVILEGES tkp;


	if( FALSE == LogonUser( pName, pDomain, pPasswd,
							LOGON32_LOGON_INTERACTIVE,
							LOGON32_PROVIDER_DEFAULT,
							&hLogonToken ) )
	{	
		return;
	}

	if( FALSE == DuplicateTokenEx( hLogonToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &hAdminToken ) )
		return;

	if( FALSE == ImpersonateLoggedOnUser( hAdminToken ) )
		return;

 	if( FALSE == OpenThreadToken( GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, TRUE, &hThreadToken ) )
	{
		RevertToSelf();
		return;
	}

	// Get the LUID for shutdown privilege.
	LookupPrivilegeValue( NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid );

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get shutdown privilege for this thread.
	AdjustTokenPrivileges( hThreadToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0 ); 
	if( GetLastError() != ERROR_SUCCESS )
	{
		RevertToSelf();
		return;
	}


	if( 1 )
	{
		//if( FALSE == InitiateSystemShutdown( pRemoteName, pMessage, pDlg->m_timeOut, FALSE, FALSE ) )
		if( FALSE == InitiateSystemShutdown( NULL, NULL, 0, TRUE, FALSE ) )
		{
			RevertToSelf();
			return;
		}
	}
	else
	{
		if( FALSE == AbortSystemShutdown( pRemoteName ) )
		{
			RevertToSelf();
			return;
		}
	}

	// Disable shutdown privilege.
	tkp.Privileges[0].Attributes = 0;
	AdjustTokenPrivileges( hThreadToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0 );
	if( GetLastError() != ERROR_SUCCESS )
	{
		RevertToSelf();
		return;
	}

	RevertToSelf();
}
