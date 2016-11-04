#if 0
Author:	jeh
Date:	28-Aug-2012
Purpose:Define structures for static assignment of IP address and ports

#endif

#ifndef IP4_ASSIGNED_H
#define IP4_ASSIGNED_H
		
#define	MAX_SERVERS							2
#define MAX_CLIENTS_PER_SERVER				2

typedef struct
	{
	char Ip[16];	// dotted address eg., "192.168.10.10"
	UINT uPort;		// port to listen on
	}	SRV_SOCKET;	// Element of a server listener socket


#ifdef I_AM_SCM
// only defined in ServerConnectionManagement.cpp

SRV_SOCKET gServerArray[MAX_SERVERS] =
	{
		{ "192.168.10.10", 7501}		// MMI to 1st phased array master
		,{"192.168.10.10", 9999}		// dummy, 
	};

#else

extern SRV_SOCKET gServerArray[];

#endif

#endif		// IP4_ASSIGNED_H
