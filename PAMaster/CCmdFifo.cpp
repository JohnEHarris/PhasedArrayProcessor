/*
A class to simulate a FIFO specifically for commands from the PAG
Unlike a normal FIFO this one should only be 'read' when there are enough
bytes in it to make a whole packet.

Author:		jeh
Date:		2016-11-03
Revised:	


*/



#include "stdafx.h"
#include "ServiceApp.h"

CCmdFifo::CCmdFifo(int PacketSize)
	{
	m_PacketSize = PacketSize;
	m_Size	= 0;
	Reset();
	};

CCmdFifo::~CCmdFifo()
	{
	};

// What if fragment of packet still in FIFO	
// With the cases listed below (size>packet, size>0, size==0) we can call Reset after every packet access.
// Or get the size and only call reset when size < packet size.
void CCmdFifo::Reset(void)
	{
	//if ( m_Size >= m_PacketSize)	return;		// cant reset now, still have at least 1 packet in FIFO
	if (m_In == m_Out)	// input pt = output pt, size is 0 ... reset to front of buffer
		{
		m_In = m_Out = m_Size = 0;
		return;
		}

	//if here the size is < m_PacketSize
	// move fragment from current location to front of FIFO
	if ( /*(m_Size > 0) || */ 
		(m_In > (CMD_FIFO_MEM_SIZE - 0x1000)))		// fragment or near the end of buffer
		{
		memcpy(&m_Mem[0], &m_Mem[m_Out], m_Size);
		m_In	= m_Size;
		m_Out	= 0;
		return;
		}
	if (m_Size >= 1460)
		{
		return;		// cant reset now, still have at least 1 packet in FIFO
		}
	// ELSE Fifo is empty
	m_In = m_Out = m_Size = 0;
	}


BYTE * CCmdFifo::GetInLoc(void)
	{ 
	BYTE *pMem;
	if (m_In < 0)
		{
		m_In = m_Out = m_Size = 0;
		// SET buffer overflow bit
		}
	if (m_In >= CMD_FIFO_MEM_SIZE - 0x1800)
		{
		m_In = m_Out = m_Size = 0;
		}
	pMem = (BYTE *)&m_Mem[m_In];
	return pMem;
	}

// Normally we would store individual byte into the FIFO one byte at a time
// But here we will use the OnReceive operation to copy bytes into a pointer
// location. That location will be m_Mem[m_In]. This function updates size 
// and the next input location
void CCmdFifo::AddBytesToFifo(int n)
	{
	m_In += n;
	m_Size += n;
	}

// after 2016-12-13 packet size is not fixed.
int CCmdFifo::GetPacketSize(void)
	{
	int i;
	GenericPacketHeader *pHeader = (GenericPacketHeader *)&m_Mem[m_Out];
	InputRawDataPacket *pIdata = (InputRawDataPacket *)pHeader;
	if ((pHeader->uSync != SYNC) || (pHeader->wByteCount > 1460) )
		{	// we are lost in the data, reset the FIFO and set an error bit
		m_In = 0;
		m_Out = 0;
		m_Size = 0;
		// See if we are losing a real packet
		if ((pIdata->wMsgSeqCnt) != (m_wMsgSeqCnt + 1))
			{
			i = pIdata->wMsgSeqCnt - m_wMsgSeqCnt;	// debugging
			}
		return 0;
		}
	m_wMsgSeqCnt = pIdata->wMsgSeqCnt;
	return pHeader->wByteCount;
	}
// Remove a PacketSize chunk from the FIFO. May leave a partion packet in the FIFO
// If the size falls to 0 bytes, reset the FIFO to avoid wrap around problems associated
// with typical software FIFO's
// 2016-12-12 Allow variable size packets. Cmd and data packets will have the sync word and a 
// byte count at a fixed location in the packet header. If the sync word is not found, the FIFO will be
// flushed and a status bit in the Idata will be set to inform the down stream processors that the 
// command queue has been flushed by the PAP.
BYTE *CCmdFifo::GetNextPacket(void)
	{ 
	BYTE *pEnd = &m_Mem[m_Out];		// beginning of NEXT whole packet(s) memory
	GenericPacketHeader *pHeader = (GenericPacketHeader *)pEnd;
	if ((pHeader->uSync != SYNC) || (pHeader->wByteCount > 1460) )
		{	// we are lost in the data, reset the FIFO and set an error bit
		m_In = 0;
		m_Out = 0;
		m_Size = 0;
		return NULL;
		}
	m_Size -= pHeader->wByteCount;	// m_PacketSize;
	if (m_Size <= 0)
		{
		m_In = m_Out = m_Size = 0;
		}
	else
		m_Out += pHeader->wByteCount;	// m_PacketSize move to next packet

	return pEnd;
	}
