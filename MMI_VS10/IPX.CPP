BOOL CTscanDlg::InitIPX()
	{	// on success, return 1, failure return 0

#ifdef TWO_CHANNEL_4_GATE_BOARD
	return (TRUE);
#endif
	
	// 05/08/00 try setting up tcpip as well
	int rv;
//	int protoList[] = { NSPROTO_IPX, IPPROTO_UDP, 0 };
	WSAPROTOCOL_INFO strProtoInfo[MAX_PROTOCOLS];
	ULONG nProtoInfo;
	int optval;
	int nSockCmndLen;
	int nSockDataLen;
	int nIpx;		// protocol index of each protocol
	int i;

	CString s,t;
	// copied in toto from wellchekdlg
		

//  Open a socket connection to the instrument.  Must have Winsock v2.0 or
//  higher for IPX support.

	nProtoInfo = sizeof(strProtoInfo);
//  strProtoInfo[0].dwMessageSize = 1500;  no effect, still 0x240
	rv = WSAEnumProtocols(NULL, strProtoInfo, &nProtoInfo);
//	rv = WSCEnumProtocols(protoList, strProtoInfo, &nProtoInfo, &ErrorNum);
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSockNA("EnumProtocol ");
		return 0;
		}

	nIpx = 1000;		// out of range number

	//FIND 1st protocol of each type
	for ( i = 0; i < rv; i++)
		{
		if (strProtoInfo[i].iProtocol == NSPROTO_IPX)
			{
			nIpx = i;
			break;
			}
		}

	
	if ( nIpx  > MAX_PROTOCOLS )
		{
		MessageBox("Did not find IPX protocol");
		return 0;
		}

//  Returned dwServiceFlags1 was:
//    XP1_CONNECTIONLESS     | XP1_MESSAGE_ORIENTED | XP1_SUPPORT_BROADCAST
//  | XP1_SUPPORT_MULTIPOINT | XP1_IFS_HANDLES
#if 0
	t.Format("\r\nProto %08X", strProtoInfo[0].dwServiceFlags1);
	s += t;
    m_ceStdOut.SetWindowText(s);

#endif

	//create a socket bound to a service provider transport
	sktIpxCmnd = WSASocket( AF_IPX, SOCK_DGRAM, NSPROTO_IPX+4,  //  Family, type, protocol
								&strProtoInfo[nIpx], 0,              //  Protocol info, group
								0                                 //  Flags
//      WSA_FLAG_OVERLAPPED
//    | WSA_FLAG_MULTIPOINT_C_LEAF 
//    | WSA_FLAG_MULTIPOINT_D_LEAF
    );
	if (sktIpxCmnd == INVALID_SOCKET)
		{ 
		ErrMsgSockNA("Socket");
		//  Need sock_na flag here
		return 0;
		}

//  Instrument will send broadcast messages so he does not need to know this
//  IP address or Ethernet adapter ID, only the socket number.  We will get
//  his address when he transmits.  

	
	optval = TRUE;
	rv = setsockopt( sktIpxCmnd, SOL_SOCKET, SO_BROADCAST,
					(char*)&optval, sizeof(BOOL) );
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Setopt ");
		return 0;
		}



//  Bind the cmnd  socket  to a local address
	memset(&SockCmndAddr, 0, sizeof(SockCmndAddr));
	SockCmndAddr.sa_family = AF_IPX;

//  sa_netnum is the network address (in network order) that was specified
//  for IPX/SPX compatible protocol advanced properties network address in
//  network setup.  It will be filled in by the driver.

//  SockCmndAddr.sa_nodenum[6] is the unique ID of the Ethernet adapter.
//  It will be filled in by the driver.

//  sa_socket is the port number for this application.
  // this is the IPX 'socket number'
#if 1
	SockCmndAddr.sa_socket = TscanCOMMAND;
	rv = bind(sktIpxCmnd, (SOCKADDR*)&SockCmndAddr, sizeof(SockCmndAddr));
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Bind ");
		// Already running 1 copy of program
		MessageBox("Tscan is already running.  Close this copy and Maximize the copy already Running");
		return 0;
		} 
#endif

//  Fill the address structure with the bound values
	nSockCmndLen = sizeof(SockCmndAddr);
	rv = getsockname(sktIpxCmnd, (struct sockaddr*)&SockCmndAddr, &nSockCmndLen);
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Name ");
		return 0;
		}

  	memset (&SockCmndAddr.sa_nodenum[0],0xff,6);

//  Bind the data  socket  to a local address
	sktIpxData = WSASocket( AF_IPX, SOCK_DGRAM, NSPROTO_IPX+4,  //  Family, type, protocol
								&strProtoInfo[nIpx], 0,              //  Protocol info, group
								0                                 //  Flags
//      WSA_FLAG_OVERLAPPED
//    | WSA_FLAG_MULTIPOINT_C_LEAF 
//    | WSA_FLAG_MULTIPOINT_D_LEAF
							);
	if (sktIpxData == INVALID_SOCKET)
		{ 
		ErrMsgSockNA("Socket");
//		Need sock_na flag here
		return 0;
		}

//  Instrument will send broadcast messages so he does not need to know this
//  IP address or Ethernet adapter ID, only the socket number.  We will get
//  his address when he transmits.  
	optval = TRUE;
	rv = setsockopt( sktIpxData, SOL_SOCKET, SO_BROADCAST, 
					(char*)&optval, sizeof(BOOL) );
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Setopt ");
		return 0;
		}

	memset(&SockDataAddr, 0, sizeof(SockDataAddr));
	SockDataAddr.sa_family = AF_IPX;

//  sa_netnum is the network address (in network order) that was specified
//  for IPX/SPX compatible protocol advanced properties network address in
//  network setup.  It will be filled in by the driver.

//  SockDataAddr.sa_nodenum[6] is the unique ID of the Ethernet adapter.
//  It will be filled in by the driver.

//  sa_socket is the port number for this application.
  // this is the IPX 'socket number'
#if 1
	SockDataAddr.sa_socket = TscanDATA;
	rv = bind(sktIpxData, (SOCKADDR*)&SockDataAddr, sizeof(SockDataAddr));
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Bind ");
		return 0;
		} 
#endif

//  Fill the address structure with the bound values
	nSockDataLen = sizeof(SockDataAddr);
	rv = getsockname(sktIpxData, (struct sockaddr*)&SockDataAddr, &nSockDataLen);
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Name ");
		return 0;
		}

//  This sets the socket to non-blocking mode and sends the WM_IPX_RXRDY msg to
//  this window when IPX data is available.  MESSAGE_MAP is configured to
//  call OnIpxRxRdy() when the msg is received.
	rv = WSAAsyncSelect( sktIpxData, m_hWnd, WM_IPX_RXRDY, FD_READ );  //  winuser.h
//		   ( sktIpxData, m_hWnd, WM_IPX_RXRDY, FD_READ | FD_WRITE );  //  winuser.h
    
	
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("AsyncSelect ");
 //		return 0;
		}
	return 1;
	}

