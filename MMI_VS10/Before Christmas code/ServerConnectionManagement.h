#if 0

Author:		JEH
Date:		14-Aug-2012
Purpose:	Collect resource necessary to support TCP/IP Send and Receive operations from a server.

Revised:	Adapted from ClientConnectionManagement for the same reason - to manage the resources of
			a TCP/Ip connetion. This connection is made by a server. It is assumed that all clients 
			on a given connection are requesting the same kind of service and supplying the same data
			set to the server.

#endif

#ifndef SERVER_CONNECTION_MANAGEMENT_H
#define SERVER_CONNECTION_MANAGEMENT_H
#pragma once

			
typedef struct
	{
	char Ip[16];	// dotted address eg., "192.168.10.10"
	UINT uPort;		// port to listen on
	int nPacketSize;	// Expected packet size from client...ie. how many bytes to receive in a packet
	}	SRV_SOCKET_INFO;	// Element of a server listener socket


// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 

#ifdef THIS_IS_SERVICE_APP
// These are the includes and defines for the Phased Array Master Instrument Service
#include "..\Include\MC_SysCp_constants.h"
#include "ServiceApp.h"

class CServiceApp;

enum {eInstrument, eOther};
enum {eMasterNotPresent, eMasterNotConnected, eMasterConnected };
// NotPresent means slot in SCM structure, but no hardware
// Not connected means master exists but has no data flow from any instrument it is managing
// Connected means at least one instrument managed by the master is providing data packets to the GUI
//
enum {eInstrumentNotPresent, eInstrumentNotConnected, eInstrumentConnected, eInstrumentConfigured };
// Configured means the instrument has received a configuration file.

// since we are working with a service and not a window, these terms don't apply
#define THE_APP_CLASS	NULL
#define MAIN_DLG_NAME	NULL

// edit this value if more client connections to servers are needed
#define	MAX_SERVERS							1
#define MAX_CLIENTS_PER_SERVER				8


#define INSTRUMENT_PACKET_SIZE				1040
#define MASTER_PACKET_SIZE					1260

#ifdef I_AM_SCM
// only defined in ServerConnectionManagement.cpp
// the IP address of our server, the port number for that server, and 
// the packet size it exects to receive from the client that connects.

SRV_SOCKET_INFO gServerArray[MAX_SERVERS] =
	{
		{ "192.168.10.10", 7502, INSTRUMENT_PACKET_SIZE}		// server IP and listening port for 1st phased array master
//		,{"192.168.10.10", 9997, 1040}		// dummy, 
//		,{"192.168.10.10", 9998, 1040}		// dummy, 
//		,{"192.168.10.10", 9999, 1040}		// dummy, 
	};

#else

extern SRV_SOCKET_INFO gServerArray[];

#endif

/******************* FOR PHASED ARRAY GUI OR OTHER SYSTEMS *********************/
/******************* FOR PHASED ARRAY GUI OR OTHER SYSTEMS *********************/
/******************* FOR PHASED ARRAY GUI OR OTHER SYSTEMS *********************/

#else
// These are the includes and defines for the Phased Array GUI

#include "Truscan.h"
#include "TscanDlg.h"
class CTscanDlg;

#define THE_APP_CLASS	CTscanApp
#define MAIN_DLG_NAME	CTscanDlg
// enum {eInstrument, eOther}; CHANGE FOR PAG

// edit this value if more client connections to servers are needed
#define	MAX_SERVERS							2
#define MAX_CLIENTS_PER_SERVER				8
extern THE_APP_CLASS theApp;


#define INSTRUMENT_PACKET_SIZE				1040
#define MASTER_PACKET_SIZE					1260

#ifdef I_AM_SCM
// only defined in ServerConnectionManagement.cpp
// the IP address of our server, the port number for that server, and 
// the packet size it exects to receive from the client that connects.

SRV_SOCKET_INFO gServerArray[MAX_SERVERS] =
	{
		{ "192.168.10.10", 7501, MASTER_PACKET_SIZE}		// MMI to 1st phased array master
		,{"192.168.10.10", 9999, MASTER_PACKET_SIZE}		// dummy, 

	};

#else

extern SRV_SOCKET_INFO gServerArray[];

#endif

#endif




class CServerConnectionManagement;	// we're going to make a ptr to ourselves in the class so we have to define the class here
class CServerListenThread;			// Thread to listen for connection on a given server port (a given service type)
class CServerSocket;				// our specific implementation of an ASync socket
class CServerSocketOwnerThread;	// a thread to control the resource of the management class and the dialog
class CAsyncSocket;
class CServerRcvListThreadBase;			// a thread to read the linked list filled from the data received from the client
class CServerRcvListThread;			// a thread to read the linked list filled from the data received from the client

/** =============================================================================**/
// A structure to define the operation of the server with one particular client.
//
enum { eNotConnected, eNotConfigured, eConfigured };
typedef struct
	{
	CString szSocketName;			// name of this socket
	CString sClientName;			// symbolic name of the client network address,  eg., MC_ACP_HOSTNAME = "mc-acp"
	CString sClientIP4;				// IP4 dotted address of client, normally this computers NIC address, eg., 192.168.10.10 
	UINT uClientPort;				// set when connection to server made
	CRITICAL_SECTION *pCSSendPkt;	// control access to output (send) list
	CPtrList* pSendPktList;			// list containing packets to send
	CRITICAL_SECTION *pCSRcvPkt;	// control access to input (receive) list
	CPtrList* pRcvPktList;			// list containing packets received
	CServerSocket * pSocket;		// ASync socket fills RcvPktList with OnReceive method.
									// same socket is used to send packets to CLIENT
									// This socket is owned by ServerSocketOwnerThread
	CServerSocketOwnerThread *pServerSocketOwnerThread;	// thread to control sending to a connected client
	CServerRcvListThreadBase *pServerRcvListThread;	// a thread to process data from the connected client
	DWORD ServerRcvListThreadID;		// the ID of the rcv thread... allows posting messages w/o thread ptr
	int m_nMyThreadIndex;			// which instance of this we are?
	BYTE bStopSendRcv;				// have socket throw away all input/output data

	BYTE bConnected;				// eNotConnected, eNotConfigured, eConfigured .. detected in receive thread.
	UINT uPacketsReceived;
	UINT uBytesReceived;
	UINT uPacketsPerSecond;			// received packets
	UINT uMaxPacketReceived;		// how big was biggest received packet
	UINT uInvalidPacketReceived;	// we don't know what this packet is
	UINT uLostReceivedPackets;		// count the jumps in message sequence count
	UINT uDuplicateReceivedPackets;	// same packet received again
	UINT uBytesSent;
	UINT uPacketsSent;

	} ST_SERVERS_CLIENT_CONNECTION;	// Name means for a given server what are the properties of each connected client

/** =============================================================================**/

//enum {eInstrument, eOther};

// A structure to define the operation of a single server. A single server may have multiple clients connected.
// MAX_CLIENTS_PER_SERVER defines how many clients can be connected to a server.
// Each connecte client has a structure (ST_SERVERS_CLIENT_CONNECTION) of regulate the sending and receiving
// of packet with that client.

typedef struct
	{
	HWND hWnd;						// window handle of the primary dialog which sends/receives on this connection
	CString sServerName;			// symbolic name of the server network address we want to connect to with this connection, eg., mc-scp 
	CString sServerIP4;				// IP4 dotted address of server eg., 192.168.10.50
	UINT uServerPort;				// where the server is listening
	int nServerType;				// Phased array master, Wall display, etc
//	int nInstrumentSelector;		// 0 = amalog, 1 = sonoscope, 2 = tbd - or instances of the same type inspection machine
	int nProtocolSelector;			// 0 = UDP, 1 = TCPIP, initially just tcpip since udp does not have servers and clients, only peers.
	int nSeverShutDownFlag;			// 1 if sever is shutting down. Do not receive or send any more packets

	CServerListenThread *pServerListenThread;
	DWORD ListenThreadID;			// thread ID of the listening thread... path to knowing which SCM we belong to.
	//HANDLE hListenExit;				// listener thread exits when this event is set
	CServerSocket *pServerListenSocket;	// ServerListenThread created, stored here

	int nServerThreadPriority;		// should normally be THREAD_PRIORITY_ABOVE_NORMAL
	HWND hServerDlg;				// windows handle to the Server Dialog if one exists

	ST_SERVERS_CLIENT_CONNECTION *pClientConnection[MAX_CLIENTS_PER_SERVER];	// an array of pointers to 
									// information about individual clients connected to this server.
									// not all potential clients may be connected. These client connections will be
									// some way indicative of the IP address of the Phased Array Masters connected
	int nComThreadExited[MAX_CLIENTS_PER_SERVER];	// when nonZero indicates thread has exited

	// VARIOUS other controls common to the server	

	CPtrList* pInDebugMessageList;	// input operations put messages into this list.. common for all connections
	CPtrList* pOutDebugMessageList;
	UINT uIdataAcksSent;			// Only for UDP protocol..count idata packets acks.. 10-Jan-11
	CRITICAL_SECTION *pCSDebugIn;
	CRITICAL_SECTION *pCSDebugOut;

	CServerConnectionManagement *pSCM;	// point to ourself. This is important.

	}	ST_SERVER_CONNECTION_MANAGEMENT;

/** =============================================================================**/

// when this file is included by it partner cpp file, I_AM_SCM is defined
// All other cpp files which include this one should not define I_AM_SCM


#ifdef I_AM_SCM
ST_SERVER_CONNECTION_MANAGEMENT stSCM[MAX_SERVERS];		// a global, static array of SCM structs
#else
extern ST_SERVER_CONNECTION_MANAGEMENT stSCM[MAX_SERVERS];		// a global, static array of SCM structs
#endif

// Rather than having to set pointer to important information in every instance of the various classes, the information
// can be shared in a global static array when a piece (class or thread or socket) knows which items to the static array belong to it.
// Also IMHO makes debugging easier to see at a glance.

//enum {eMasterNotPresent, eMasterNotConnected, eMasterConnected };
// NotPresent means slot in SCM structure, but no hardware
// Not connected means master exists but has no data flow from any instrument it is managing
// Connected means at least one instrument managed by the master is providing data packets to the GUI
//
//enum {eInstrumentNotPresent, eInstrumentNotConnected, eInstrumentConnected, eInstrumentConfigured };
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

	void LockDebugIn(void)			{ EnterCriticalSection(m_pstSCM->pCSDebugIn );	}
	//void AddTailDebugIn(CString s)	{ m_pstCCM->pInDebugMessageList->AddTail(&s);	}
	void UnLockDebugIn(void)		{ LeaveCriticalSection(m_pstSCM->pCSDebugIn );	}

	void LockDebugOut(void)			{ EnterCriticalSection(m_pstSCM->pCSDebugOut );	}
	//void AddTailDebugOut(CString s)	{ m_pstCCM->pOutDebugMessageList->AddTail(&s);	}
	void UnLockDebugOut(void)		{ LeaveCriticalSection(m_pstSCM->pCSDebugOut );	}


	int StartListenerThread(int nMyServer);
	int StopListenerThread(int nMyServer);
	int ServerShutDown(int nMyServer);
	void DoNothing(void);
	
	CServerSocket* GetListenerSocket(void)	{ ( m_pstSCM ? m_pstSCM->pServerListenSocket : NULL );	}
	void SetListenerSocket(CServerSocket *psock)	{ m_pstSCM->pServerListenSocket = psock;	}
	void SetListenThreadID(DWORD Id)		{ m_pstSCM->ListenThreadID = Id;	}
	DWORD GetListenThreadID(void)			{ ( m_pstSCM ? m_pstSCM->ListenThreadID : NULL );	}

	//int StopListeningThread(int nMyServer);

	// Assume the routine sending knows the target client for the TCP/IP connection by a client index
	// This routine places the packet into a linked list and posts a thread message to the appropriate
	// thread to unpack and send the message at the thread's priority level.
	int SendPacket(int nClientIndex, BYTE *pB, int nBytes, int nDeleteFlag);

	/**************************************************************************************/

	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// pointer to my global structure instance 
	int m_nMyServer;		// which one of the global ST_SERVER_CONNECTION_MANAGEMENT is mine

	};	
// End of CServerConnectionManagement class declaration.


// declared public here and external in all other files.
// The main dialog or some other global memory operating agent declares an array of
// pointer to ST_SERVER_CONNECTION_MANAGEMENT. The size of the array is MAX_SERVERS
//
#ifdef I_AM_SCM
CServerConnectionManagement *pSCM[MAX_SERVERS];	// global, static ptrs to class instances define outside of the class definition.
#else
CServerConnectionManagement *pSCM[];
#endif



#endif
// SERVER_CONNECTION_MANAGEMENT_H
