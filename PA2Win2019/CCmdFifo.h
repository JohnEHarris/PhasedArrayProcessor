/*
Author: jeh
Date:	2016-11-03
Purpose:TCP/IP Can fragment messages and can combine them. In order to extract just the message amount (particularly
		for the NcNx commands) this FIFO structure is being implemented.

*/

#ifndef CMD_FIFO_H
#define CMD_FIFO_H
#include "../Include/PA2Struct.h"


class CCmdFifo
	{
	private:
		BYTE m_Mem[CMD_FIFO_MEM_SIZE];		//16k for FIFO * 256
		int m_In;		// index in m_Mem where next set of input bytes will start
		int m_Out;		// index in m_Mem where next output from FIFO will start
		int m_Size;		// number of bytes unread in FIFO
		int m_PacketSize;	// FIFO holds bytes but bytes should only be made available in PacketSize chunks
		BYTE m_bError;	// bit 0= overflow, bit 1=Lost Sync, bit 2=wrong size
		char m_Type;	// 'C' or 'S'
		short m_CSnum;	// which client or which server
		short m_SrvClientNum;	// Client number for this server
		int m_PhysicalShiftCount;	// how many times we move data to front of buffer
		int m_FlushCount;		// how many time we flushed the buffer
		int m_nFifoEmptyCount;
		int m_nMaxDepth;		// how much memory used 
		CRITICAL_SECTION *m_pCS;	// may not be needed
	public:
		// if a client fifo, which client do I belong to: CS='C', my client number, nClient = don't care 
		// if a server fifo, which server and which client is filling the fifo: CS='S', my server number, the client
		// for that server number
		CCmdFifo(int PacketSize, char CS, int nWhichCS, int nClient);	// old CCmdFifo(int PacketSize);
		virtual ~CCmdFifo();
		void Reset(void);
		BYTE *GetInLoc(void);	// { return (&m_Mem[m_In]); }	// starting point where next packet will be stored
		BYTE *GetOutLoc(void);	// { return (&m_Mem[m_Out]); } pointer to the next packet
		BYTE *GetNextPacket(void);
		BYTE* PeakNextPacket(void); // like GetNextPacket but doesn't advance ptr in buffer or change in/out/size
		int  GetFIFOByteCount(void) { return m_Size; }
		void AddBytesToFifo(int n);	// moves In back by n, increases size by n
		int GetPacketSize(void);	// { return m_PacketSize; }
		//void SetPacketSize(int n) { m_PacketSize = n; }
		WORD m_wMsgSeqCnt;
		void Shift(void);
		BYTE GetError( void );	// return error to caller and reset error byte

		void SetClientNumber(int nClient);
		int m_nFifoCnt;
		int m_nOwningThreadId;			// debugging
		char tag[128];		// tell who created and who destroyed.
		int m_nLostSyncCnt;
	};

#endif

