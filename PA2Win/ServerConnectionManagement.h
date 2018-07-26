#if 0

Author:		JEH
Date:		14-Aug-2012
Purpose:	Collect resource necessary to support TCP/IP Send and Receive operations from a server.

Revised:	Adapted from ClientConnectionManagement for the same reason - to manage the resources of
			a TCP/Ip connetion. This connection is made by a server. It is assumed that all clients 
			on a given connection are requesting the same kind of service and supplying the same data
			set to the server.
			Since the socket belongs to this thread, it runs at the priority of the ServerOwner which is high.
			If it were in the global structure, it would run at the priority of the application.

#endif

#ifndef SERVER_CONNECTION_MANAGEMENT_H
#define SERVER_CONNECTION_MANAGEMENT_H
#pragma once

			
typedef struct
	{
	char Ip[16];			// dotted address eg., "192.168.10.10"
	UINT uPort;				// port to listen on
	int nPacketSize;		// Expected packet size from client...ie. how many bytes to receive in a packet
	char ClientBaseIp[16];	// Starting IP addres of a collection of clients
	}	SRV_SOCKET_INFO;	// Element of a server listener socket


// Change some eNums to defines for use in PAM/PAG 
// 		enum {eInstrument, eOther} in PAM, enum {ePhaseArrayMaster, eOther} in PAG

#define eInstrument			0
#define ePhaseArrayMaster	0
#define eOther				1

// these 3 describe the same machine with a varying name over time.

#define ePAP_Server			0	// the server for all Phased Array Processor computers
#define eInstrument_Server	0	// the server for all instrument connected to a PAP
#define ePAP_AllWall_server	1	// PAG server for all_wall data from PAP
#define ePAP_Pulser_server	1	// PAP server to supply commands to Pulser board.


// An instrument client can have up to this many virtual channels
#define MAX_CHNLS_PER_INSTRUMENT			32

#include "PA2WinDlg.h"
class CPA2WinDlg;
// since we are working with a service and not a window, these terms don't apply
#define THE_APP_CLASS	CPA2WinApp
#define MAIN_DLG_NAME	CPA2WinDlg

// edit this value if more client connections to servers are needed
#define	MAX_SERVERS						2
#define MAX_CLIENTS_PER_SERVER				8


// I_AM_PAP is defined in the PAP project under C++ | Preprocessor Definitions 

#ifdef I_AM_PAP
// These are the includes and defines for the Phased Array Master Instrument Service
//#include "..\Include\MC_SysCp_constants.h"
//#include "ServiceApp.h"

//class CServiceApp;




/******************* FOR PHASED ARRAY GUI OR OTHER SYSTEMS *********************/
/******************* FOR PHASED ARRAY GUI OR OTHER SYSTEMS *********************/
/******************* FOR PHASED ARRAY GUI OR OTHER SYSTEMS *********************/

#else
// These are the includes and defines for the Phased Array GUI

//#include "..\Include\MC_SysCp_constants.h"
//#include "tscandlg.h"
//#include "Truscan.h"

//class CTscanDlg;

//#define THE_APP_CLASS	CTscanApp
//#define MAIN_DLG_NAME	PA2WinDlg

// edit this value if more client connections to servers are needed
#define	MAX_SERVERS							2
#define MAX_CLIENTS_PER_SERVER				8
extern THE_APP_CLASS theApp;


#define PAG_CMD_PACKET_SIZE					1040	// irrelevant 2018-07-20

#endif

#ifdef I_AM_SCM
// only defined in ServerConnectionManagement.cpp
// the IP address of our server, the port number for that server, and 
// the packet size it expects to receive from the client that connects.

SRV_SOCKET_INFO gServerArray[MAX_SERVERS];	// =


#else

extern SRV_SOCKET_INFO gServerArray[];

#endif


class CServerConnectionManagement;	// we're going to make a ptr to ourselves in the class so we have to define the class here
class CServerListenThread;			// Thread to listen for connection on a given server port (a given service type)
class CServerSocket;				// our specific implementation of an ASync socket
class CServerSocketOwnerThread;		// a thread to control the resource of the management class and the dialog
class CAsyncSocket;
//class CServerRcvListThreadBase;	// a thread to read the linked list filled from the data received from the client
class CServerRcvListThread;			// a thread to read the linked list filled from the data received from the client
class CvChannel;					// array of ptrs of this type to logically connect channels of each instrument


/** =============================================================================**/
// A structure to define the operation of the server with one particular client.
//
enum {eMasterNotPresent, eMasterNotConnected, eMasterConnected };
// NotPresent means slot in SCM structure, but no hardware
// Not connected means master exists but has no data flow from any instrument it is managing
// Connected means at least one instrument managed by the master is providing data packets to the GUI
//
enum {eInstrumentNotPresent, eInstrumentNotConnected, eInstrumentConnected, eInstrumentConfigured };
// Configured means the instrument has received a configuration file.
enum { eNotConnected = 0, eNotConfigured, eConfigured };


/** =============================================================================**/
// A structure to define the operation of the server with one particular client.
//
// cp??? indicates a 'constant pointer' but not constant in the normal c++ sense. These are not created with 'new'
// but they point to pointers created with new.
typedef struct
	{
	CString szSocketName;			// name of this server socket
	CString sClientName;			// symbolic name of the client network address,  eg., MC_ACP_HOSTNAME = "mc-acp"
	CString sClientIP4;				// IP4 dotted address of client, normally this computers NIC address, eg., 192.168.10.10 
	UINT uClientPort;				// set when connection to server made
	CRITICAL_SECTION *pCSSendPkt;	// control access to output (send) list
	CPtrList* pSendPktList;			// list containing packets to send
	CRITICAL_SECTION *pCSRcvPkt;	// control access to input (receive) list
	CPtrList* pRcvPktList;			// list containing packets received from client
	CServerSocket * pSocket;		// ASync socket fills RcvPktList with OnReceive method.
									// same socket is used to send packets to CLIENT
									// This socket is owned by ServerSocketOwnerThread
	CServerSocketOwnerThread *pServerSocketOwnerThread;	// thread to control sending to a connected client
	CServerRcvListThread *pServerRcvListThread;		// a thread to process data from the connected client
	int nSSOwnerThreadPriority;
	int nSSRcvListThreadPriority;	// THREAD_PRIORITY_NORMAL

	DWORD ServerRcvListThreadID;	// the ID of the rcv thread... allows posting messages w/o thread ptr
	int m_nClientIndex;				// which instance of this we are?
	BYTE m_bIsClosing;				// got a tcpip close -- in progress
	BYTE bStopSendRcv;				// have socket throw away all input/output data

	BYTE bConnected;				// eNotConnected, eNotConfigured, eConfigured .. detected in receive thread.
	WORD wMsgSeqCnt;				// sequential number of sent message to this client
	UINT uPacketsReceived;
	UINT uBytesReceived;
	UINT uPacketsPerSecond;			// received packets
	UINT uMaxPacketReceived;		// how big was biggest received packet
	UINT uInvalidPacketReceived;	// we don't know what this packet is
	UINT uLostReceivedPackets;		// count the jumps in message sequence count
	UINT uDuplicateReceivedPackets;	// same packet received again
	UINT uBytesSent;
	UINT uPacketsSent;				// if this resets it means the instrument has disconnected/reconnected
	UINT uUnsentPackets;			// Sending was aborted w/o sending the packet

	UINT uLastTick;					// Use with main app uAppTimerTick value to provide keep alive messages
	// 2016-06-06 jeh
	// initialized in CServerSocket::OnAcceptInitializeConnectionStats
	CvChannel* pvChannel[MAX_SEQ_COUNT][MAX_CHNLS_PER_MAIN_BANG];	// array of ptrs to virtual channels associated 
							// with each client connection. Pointer is
	
	RAW_INSTRUMENT_STATUS InstrumentStatus;	// Status info which comes with each TCPIP packet from an instrument


	} ST_SERVERS_CLIENT_CONNECTION;	// Name means for a given server what are the properties of each connected client

/** =============================================================================**/


// A structure to define the operation of a single server. A single server may have multiple clients connected.
// MAX_CLIENTS_PER_SERVER defines how many clients can be connected to a server.
// Each connected client has a structure (ST_SERVERS_CLIENT_CONNECTION) of regulate the sending and receiving
// of packet with that client.
//
// The application populates and controls each SCM structure. Each SCM structure is static memory indexed by stSCM[]
// Pointer with names that begin with 'cp' point to static memory and are considered const pointers ie, not created with
// a new operator and not to be deleted.

typedef struct
	{
	HWND hWnd;						// window handle of the primary dialog which sends/receives on this connection
	CString sServerName;			// symbolic name of the server network address we want to connect to with this connection, eg., mc-scp 
	CString sServerDescription;		// The name of the server and its client-- a descriptive name for debugging only
	CString sServerIP4;				// IP4 dotted address of server eg., 192.168.10.50
	UINT uServerPort;				// where the server is listening
	int nServerType;				// Phased array master, Wall display, etc
	int nServerIsListening;			// listening socket is active
	CString sClientBaseIP;			// the IP address of the '1st' client. Assumes clients are ordered beginning
									// with a base address. nth client index is IP of nth client - client base ip
//	int nInstrumentSelector;		// 0 = amalog, 1 = sonoscope, 2 = tbd - or instances of the same type inspection machine
	int nProtocolSelector;			// 0 = UDP, 1 = TCPIP, initially just tcpip since udp does not have servers and clients, only peers.
	int nSeverShutDownFlag;			// 1 if sever is shutting down. Do not receive or send any more packets

	CServerListenThread *pServerListenThread;
	DWORD ListenThreadID;			// thread ID of the listening thread... path to knowing which SCM we belong to.
	int nListenThreadPriority;		// should normally be THREAD_PRIORITY_NORMAL
	CServerSocket *pServerListenSocket;	// ServerListenThread created, stored here

	int nServerThreadPriority;		// should normally be THREAD_PRIORITY_ABOVE_NORMAL
	HWND hServerDlg;				// windows handle to the Server Dialog if one exists

	// unfortunately it appears that pClientConnection needs it own array of critical sections to prevent races when accessing
	// lists controlled by pClientConnection - that is if ServiceApp and a clientConnection are both attempting to use
	// the list, there must be a common critical section. And ServiceApp can not tolerate the situation where
	// the clientConnection deletes the critical sections.
	// Remove this redundancy 2017-04-27
	//CRITICAL_SECTION *pCS_ClientConnectionSndList[MAX_CLIENTS_PER_SERVER];	// created by the app with new
	//CRITICAL_SECTION *pCS_ClientConnectionRcvList[MAX_CLIENTS_PER_SERVER];	// created by the app with new
	//CPtrList* pRcvPktList[MAX_CLIENTS_PER_SERVER];	// created by the app with new
	//CPtrList* pSendPktList[MAX_CLIENTS_PER_SERVER];	// created by the app with new
	ST_SERVERS_CLIENT_CONNECTION *pClientConnection[MAX_CLIENTS_PER_SERVER];	// an array of pointers to 
									// information about individual clients connected to this server.
									// not all potential clients may be connected. These client connections will be
									// some way indicative of the IP address of the Phased Array Masters connected
	int nComThreadExited[MAX_CLIENTS_PER_SERVER];	// when nonZero indicates thread has exited

	// VARIOUS other controls common to the server	

	UINT uIdataAcksSent;			// Only for UDP protocol..count idata packets acks.. 10-Jan-11
	CRITICAL_SECTION *pCSDebugIn;
	CRITICAL_SECTION *pCSDebugOut;
	CPtrList* pInDebugMessageList;	// input operations put messages into this list.. common for all connections
	CPtrList* pOutDebugMessageList;

	CServerConnectionManagement *pSCM;	// point to ourself. This is important. Pointer to a CLASS  -- not created with 'new'

	}	ST_SERVER_CONNECTION_MANAGEMENT;

/** =============================================================================**/

// when this file is included by its partner cpp file, I_AM_SCM is defined
// All other cpp files which include this one should not define I_AM_SCM


#ifdef I_AM_SCM
ST_SERVER_CONNECTION_MANAGEMENT stSCM[MAX_SERVERS];		// a global, static array of SCM structs
//int gnMaxServers;
//int gnMaxClientsPerServer;
short gnDefaultListenPort;
// C global function
ST_SERVER_CONNECTION_MANAGEMENT * GetPAM_SCM(void)	{	return &stSCM[0];	}

#else
extern ST_SERVER_CONNECTION_MANAGEMENT stSCM[MAX_SERVERS];		// a global, static array of SCM structs
//extern int gnMaxServers;
//extern int gnMaxClientsPerServer;
extern short gnDefaultListenPort;
// C global function
extern ST_SERVER_CONNECTION_MANAGEMENT * GetPAM_SCM(void);	//	{	return &stSCM[0];	}

#endif


// Rather than having to set pointer to important information in every instance of the various classes, the information
// can be shared in a global static array when a piece (class or thread or socket) knows which items in the static array belong to it.
// Also IMHO makes debugging easier to see at a glance.

// enum {eMasterNotPresent, eMasterNotConnected, eMasterConnected };
// NotPresent means slot in SCM structure, but no hardware
// Not connected means master exists but has no data flow from any instrument it is managing
// Connected means at least one instrument managed by the master is providing data packets to the GUI
//
// enum {eInstrumentNotPresent, eInstrumentNotConnected, eInstrumentConnected, eInstrumentConfigured };
// Configured means the instrument has received a configuration file.


/***************** CLASS DECLARATION **************************/
/***************** CLASS DECLARATION **************************/

class CServerConnectionManagement: public CObject
	{
public:
	CServerConnectionManagement(int nMyServerIndex);		// the only constructor
	virtual ~CServerConnectionManagement(void);

	void SetWndHandle(HWND h)		{	if (m_pstSCM)	m_pstSCM->hWnd = h;		}
	HWND GetWndHandle(void)			{ return ( m_pstSCM ? m_pstSCM->hWnd : NULL );	}
	void SetServerName(CString s)	{ if (m_pstSCM)	m_pstSCM->sServerName = s;	}	// symbolic name of the server -- "mc-scp"
	CString GetServerName(void)		{ return ( m_pstSCM ? m_pstSCM->sServerName : _T("") );	}

	CString GetConnectedClientIp4(int nClient);	// get Ip4 dotted address for client number nClient

	void SetServerIP4(CString s)	{ if (m_pstSCM)	m_pstSCM->sServerIP4 = s;	}	// eg., 192.168.10.10
	CString GetServerIP4(void)		{ return ( m_pstSCM ? m_pstSCM->sServerIP4 : _T("") );	}

	void SetServerPort(UINT p)		{ if (m_pstSCM)	m_pstSCM->uServerPort = p;	}	// listener port
	UINT GetServerPort(void)		{ return ( m_pstSCM ? m_pstSCM->uServerPort : 0 );	}

	void SetServerType(int t)		{ if (m_pstSCM)	m_pstSCM->nServerType = t;	}
	int GetServerType(void)			{ return ( m_pstSCM ? m_pstSCM->nServerType : -1 );	}

	void SetClientBaseIp(CString s) { if (m_pstSCM)	m_pstSCM->sClientBaseIP = s; }
	CString GetClientBaseIp(void)	{return ( m_pstSCM ? m_pstSCM->sClientBaseIP : _T("") );}

	void LockDebugIn(void)			{ EnterCriticalSection(m_pstSCM->pCSDebugIn );	}
	//void AddTailDebugIn(CString s)	{ m_pstCCM->pInDebugMessageList->AddTail(&s);	}
	void UnLockDebugIn(void)		{ LeaveCriticalSection(m_pstSCM->pCSDebugIn );	}

	void LockDebugOut(void)			{ EnterCriticalSection(m_pstSCM->pCSDebugOut );	}
	//void AddTailDebugOut(CString s)	{ m_pstCCM->pOutDebugMessageList->AddTail(&s);	}
	void UnLockDebugOut(void)		{ LeaveCriticalSection(m_pstSCM->pCSDebugOut );	}

	// returns 0 on failure
//	int LockClientConnectionSndList(int i)		{ return TryEnterCriticalSection(m_pstSCM->pCS_ClientConnectionSndList[i]);	}
//	void UnLockClientConnectionSndList(int i)	{ LeaveCriticalSection(m_pstSCM->pCS_ClientConnectionSndList[i]);	}
	
//	int LockClientConnectionRcvList(int i)		{ return TryEnterCriticalSection(m_pstSCM->pCS_ClientConnectionRcvList[i]);	}
//	void UnLockClientConnectionRcvList(int i)	{ LeaveCriticalSection(m_pstSCM->pCS_ClientConnectionRcvList[i]);	}

	int StartListenerThread(int nMyServer);
	int StopListenerThread(int nMyServer);
	// the socket associated with this connection calls the manager to create an owner thread
	// for the connection.

	int ServerShutDown(int nMyServer);
	// Called at shut down or when client socket executes OnClose
	void KillClientConnection(int nMyServer, int nClient);	// Called by ServerShutDown
	// wait for nWait*10 milliseconds for thread to close
	int KillServerRcvListThread( int nMyServer, int nClientIndex );
	// in shutdown, the socket kill will also kill the socket owner
	int KillServerSocket( int nMyServer, int nClientIndex, int nWait);
	//int KillServerSocketOwnerThread( int nMyServer, int nClientIndex, int nWait);
	int KillCmdProcessThread( int nMyServer, int nClientIndex );
	void DoNothing(void);
	
	CServerSocket* GetListenerSocket(void)	{ ( m_pstSCM ? m_pstSCM->pServerListenSocket : NULL );	}
	void SetListenerSocket(CServerSocket *psock)	{ m_pstSCM->pServerListenSocket = psock;	}
	void SetListenThreadID(DWORD Id)		{ m_pstSCM->ListenThreadID = Id;	}
	DWORD GetListenThreadID(void)			{ ( m_pstSCM ? m_pstSCM->ListenThreadID : NULL );	}
	int IsServerLIstenint(void)				{ (m_pstSCM->nServerIsListening ? 1 : 0 ); }

	//int StopListeningThread(int nMyServer);

	// Assume the routine sending knows the target client for the TCP/IP connection by a client index
	// This routine places the packet into a linked list and posts a thread message to the appropriate
	// thread to unpack and send the message at the thread's priority level.
#ifdef I_AM_PAP
	int SendPacket(int nClientIndex, BYTE *pB, int nBytes, int nDeleteFlag);
#endif

#ifdef I_AM_PAG
	int SendPacketToPAP(int nClientIndex, BYTE *pB, int nBytes, int nDeleteFlag);
#endif

	/**************************************************************************************/

	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// pointer to my global structure instance  -- not created with 'new'
	int m_nMyServer;		// which one of the global ST_SERVER_CONNECTION_MANAGEMENT is mine

	};	
// End of CServerConnectionManagement class declaration.


// declared public here and external in all other files.
// The main dialog or some other global memory operating agent declares an array of
// pointer to ST_SERVER_CONNECTION_MANAGEMENT. The size of the array is MAX_SERVERS
//
								//  -- not created with 'new'
extern CServerConnectionManagement *pSCM[];

// C functions for copying mostly pointers from ST_SERVERS_CLIENT_CONNECTION
//
void CopySCCStrings(ST_SERVERS_CLIENT_CONNECTION *pDest, ST_SERVERS_CLIENT_CONNECTION *pSrc);
void CopySCCPtrs(ST_SERVERS_CLIENT_CONNECTION *pDest, ST_SERVERS_CLIENT_CONNECTION *pSrc);


#endif
// SERVER_CONNECTION_MANAGEMENT_H
