/*
Instrument Message Process Class
InstMsgProcess.h

Author:		jeh
Date:		20-Mar-2012
Purpose:	Encapsulate the ethernet connection and the message sending/receiving from a phased array
			instrument
Revised:
*/

#ifndef INSTMSGPROCESS_H
#define INSTMSGPROCESS_H



#endif		// INSTMSGPROCESS_H
//class CRunningAverage;

class CInstMsgProcess
{
public:
	CInstMsgProcess(int nInstrument);
	virtual ~CInstMsgProcess();
	CWinThread *GetThreadPtr(void)		{ return m_pReceiverProcessThread;	}
	void SetThreadPtr(CWinThread *pThread) { m_pReceiverProcessThread = pThread;	}
	CWinThread *m_pReceiverProcessThread;	// a thread to receive data from the instrument and process the data

	int m_nInstrumentSocket;	// this class owns a socket connected to an instrument
	int GetSocket(void)			{	return m_nInstrumentSocket;	}
	void SetSocket(int nSocket)	{	m_nInstrumentSocket = nSocket;	}

	int m_nWhichInstrument;		// which one of up to 32 instrument is this class servicing
	int GetInstNumber(void)		{	return m_nWhichInstrument;	}
	BYTE m_bConnected;			// are we connected to an instrument.
	// Each wall channel in an instrument has a class to process the wall data
	// There can be 4 channels of wall
//	CRunningAverage *m_pRunningAvg[MAX_WALL_CHANNELS];
	// Store info about Channel type assignments
	CHANNEL_INFO m_ChannelInfo[MAX_CHANNEL_PER_INSTRUMENT];
	// get the CHANNEL_CONFIG2 info from InspState.. usually when an MMI command is received
	// Then SetChannelInfo will set the channel types for this instrument
	void SetChannelInfo(void);
	// return a pointer to the channel info associated with this instrument
	CHANNEL_INFO *GetChannelInfoPtr(void)		{ return &m_ChannelInfo[0];	}
};

