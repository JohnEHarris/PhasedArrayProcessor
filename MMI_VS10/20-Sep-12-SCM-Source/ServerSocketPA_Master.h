#pragma once
// Created with Add Class menu from Project menu.
// Initially based on CAsyncSocket but manually changed to CServerSocket
// CServerSocketPA_Master command target
#define MAIN_DLG_NAME	CTscanDlg

#ifndef SERVER_CONNECTION_MANAGEMENT_H
#include "ServerConnectionManagement.h"
#endif

#ifndef SERVER_SOCKET_H
#include "ServerSocket.h"
#endif

class CServerConnectionManagement;
class CServerCommunicationThread;

class CServerSocketPA_Master : public CServerSocket
	{
public:
	CServerSocketPA_Master();
	CServerSocketPA_Master(CServerConnectionManagement *pSCM);
	virtual ~CServerSocketPA_Master();
	virtual void OnReceive(int nErrorCode);		// added jeh

	void DistributeMessage(I_MSG_RUN *pReadBuf, CTcpThreadRxList *pTcpThreadRxList);	// stolen from CTcpThreadRxList


	};


