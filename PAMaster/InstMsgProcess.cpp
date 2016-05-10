/*
Instrument Message Process Class
InstMsgProcess.CPP

Author:		jeh
Date:		20-Mar-2012
Purpose:	Encapsulate the ethernet connection and the message sending/receiving from a phased array
			instrument
Revised:
*/

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

class CInstState;
extern  CInspState InspState;
extern  int g_ArrayScanNum[NUM_OF_SLAVES];
extern 	C_MSG_ALL_THOLD  g_AllTholds;
extern 	C_MSG_NC_NX g_NcNx;

int FindDisplayChannel(int nArray, int nArrayCh);

// Only constructor
CInstMsgProcess::CInstMsgProcess(int nInstrument)
	{
	int i;
	m_nWhichInstrument = nInstrument;
	m_nInstrumentSocket = -1;
	m_bConnected = 0;
	for ( i = 0; i < MAX_WALL_CHANNELS; i++)	m_pRunningAvg[i] = NULL;
	memset( (void *)&m_ChannelInfo[0],0, sizeof(CHANNEL_INFO)*MAX_CHANNEL_PER_INSTRUMENT);
#ifdef _DEBUG
	printf("CInstMsgProcess instance for instrument [%d] created\n", nInstrument);
#endif
	}

// Destructor
CInstMsgProcess::~CInstMsgProcess()
	{
	int i;
	if (m_nInstrumentSocket >= 0)
		{
		closesocket(m_nInstrumentSocket);
		//::Sleep(200);	call that issues delete on this class needs to sleep
		}
	m_bConnected = 0;
	m_nWhichInstrument = -1;
	m_nInstrumentSocket = -1;
	for ( i = 0; i < MAX_WALL_CHANNELS; i++)
		{
		if (m_pRunningAvg[i])
			{
			delete m_pRunningAvg[i];
			m_pRunningAvg[i] = NULL;
			}
		}
	}

// usually called when an MMI command is received to set channel config info
// Then SetChannelInfo will set the channel types for this instrument
// move this code into ServerRcvListThread
void CInstMsgProcess::SetChannelInfo(void)
	{
	CHANNEL_CONFIG2 ChannelCfg;
	int i, nDispCh;
	int nSlave = GetInstNumber();
	InspState.GetChannelConfig(&ChannelCfg);

	for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
		{
		nDispCh = FindDisplayChannel(nSlave, i);

		if (i < g_ArrayScanNum[nSlave])
			m_ChannelInfo[i].channel_type = ChannelCfg.Ch[nDispCh/MAX_CHANNEL_PER_INSTRUMENT][nDispCh%MAX_CHANNEL_PER_INSTRUMENT].Type;
		else
			m_ChannelInfo[i].channel_type = IS_NOTHING;

		switch (m_ChannelInfo[i].channel_type)
			{
		case IS_LONG:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdLong[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdLong[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Long[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Long[1];
			break;
		case IS_TRAN:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdTran[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdTran[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Tran[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Tran[1];
			break;
		case IS_OBQ1:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdOblq1[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdOblq1[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Oblq1[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Oblq1[1];
			break;
		case IS_OBQ2:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdOblq2[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdOblq2[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Oblq2[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Oblq2[1];
			break;
		case IS_OBQ3:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdOblq3[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdOblq3[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Oblq3[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Oblq3[1];
			break;
		case IS_WALL:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdLamin[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdLamin[1];
			m_ChannelInfo[i].TholdWallThds[0] = g_AllTholds.TholdWallThds[0];
			m_ChannelInfo[i].TholdWallThds[1] = g_AllTholds.TholdWallThds[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Lamin[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Lamin[1];
			m_ChannelInfo[i].nx_for_wall = g_NcNx.Wall[0];
			break;
		default:
			break;
			}

		}
	}
