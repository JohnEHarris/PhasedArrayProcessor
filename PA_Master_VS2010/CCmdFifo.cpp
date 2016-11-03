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
	if ( m_Size >= m_PacketSize)	return;		// cant reset now, still have at least 1 packet in FIFO
	//if here the size is < m_PacketSize
	// move fragment from current location to front of FIFO
	if (m_Size > 0)		// fragment
		{
		memcpy(&m_Mem[0], &m_Mem[m_Out], m_Size);
		m_In	= m_Size;
		m_Out	= 0;
		return;
		}
	// Fifo is empty
	m_In = m_Out = 0;
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

// Remove a PacketSize chung from the FIFO. May leave a partion packet in the FIFO
// If the size falls to 0 bytes, reset the FIFO to avoid wrap around problems associated
// with typical software FIFO's
BYTE *CCmdFifo::GetNextPacket(void)
	{ 
	BYTE *pEnd = &m_Mem[m_Out];		// beginning of last whole packet memory
	m_Out += m_PacketSize;
	m_Size -= m_PacketSize;
	// can't reset the FIFO here because the caller has not yet used the data
	// caller will have to check size after extracting the packet and call reset
	// when the size is 0
	// If caller fails to check for size then we will overflow the buffer. No wrap around provided here
	return pEnd;
	}
