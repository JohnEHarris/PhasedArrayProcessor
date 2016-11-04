/*
Author: jeh
Date:	2016-11-03
Purpose:TCP/IP Can fragment messages and can combine them. In order to extract just the message amount (particularly
		for the NcNx commands) this FIFO structure is being implemented.

*/

#ifndef CMD_FIFO_H
#define CMD_FIFO_H

//#include "ServiceApp.h" must omit.. this file included in ServiceApp.h

class CCmdFifo
	{
	private:
		BYTE m_Mem[0x4000];		//16k for FIFO
		int m_In;		// index in m_Mem where next set of input bytes will start
		int m_Out;		// index in m_Mem where next output from FIFO will start
		int m_Size;		// number of bytes unread in FIFO
		int m_PacketSize;	// FIFO holds bytes but bytes should only be made available in PacketSize chunks

	public:
		CCmdFifo(int PacketSize);
		virtual ~CCmdFifo();
		void Reset(void);
		BYTE *GetInLoc(void) { return (&m_Mem[m_In]); }	// starting point where next packet will be stored
		BYTE *GetNextPacket(void);
		int  GetSizeBytes(void) { return m_Size; }
		void AddBytesToFifo(int n);	// moves In back by n, increases size by n
		int GetPacketSize(void) { return m_PacketSize; }
		void SetPacketSize(int n) { m_PacketSize = n; }

	};

#endif

