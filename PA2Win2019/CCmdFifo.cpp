/*
A class to simulate a FIFO specifically for commands from the PAG
Unlike a normal FIFO this one should only be 'read' when there are enough
bytes in it to make a whole packet.

Author:		jeh
Date:		2016-11-03
Revised:	2018-10-22 add character for Client or Server and counter for which client or which server instance
			Instance of CCmdFifo in ServerSocket and ClientSocket classes.

*/



#include "stdafx.h"
#ifdef I_AM_PAG
#include "PA2WinDlg.h"
#else
#include "PA2WinDlg.h"
#endif


// insert debug info for fifos
// C = this fifo used by CCM, S = SRvConnThread, which client_com or which server, and for a server, which client to the server
CCmdFifo::CCmdFifo(int PacketSize, char CS, int nWhichCS, int nClient)
	{
	//m_PacketSize = PacketSize;
	m_In = m_Out = m_Size = m_nLostSyncCnt = m_PhysicalShiftCount = 0;
	m_nFifoEmptyCount = m_nMaxDepth = 0;
	m_FlushCount = 0;
	Reset();
	m_nFifoCnt = ++gnFifoCnt;
	if (toupper(CS) == 'C') m_Type = 'C';
	else m_Type = 'S';
	m_CSnum = nWhichCS;
	m_SrvClientNum = nClient;
	m_pCS = new CRITICAL_SECTION();
	InitializeCriticalSectionAndSpinCount(m_pCS, 4);
	};

CCmdFifo::~CCmdFifo()
	{
	delete m_pCS;
	};

void CCmdFifo::SetClientNumber(int nClient)
	{
	m_SrvClientNum = nClient;
	}

// What if fragment of packet still in FIFO	
// With the cases listed below (size>packet, size>0, size==0) we can call Reset after every packet access.
// Or get the size and only call reset when size < packet size.
void CCmdFifo::Reset(void)
	{
	if (m_In == m_Out)	// input pt = output pt, size is 0 ... reset to front of buffer
		{
		m_In = m_Out = m_Size = 0;
		return;
		}

	// move current output location to front of FIFO
	// if here, m_In > m_Out
	if (m_In > (CMD_FIFO_MEM_SIZE - 0x1000))		//  near the end of memory buffer
		{
		if (m_Out > 1460)
			{
			memcpy(&m_Mem[0], &m_Mem[m_Out], m_Size);
			m_In = m_Size;
			m_Out = 0;
			return;
			}
		}
	//if ( m_Size >= 1460)	return;		// cant reset now, still have at least 1 packet in FIFO

	// ELSE Fifo is empty
	///m_In = m_Out = m_Size = 0;
	}

// A real hardware FIFO would shift data to the output side automatically
// assume we have a really big fifo (400000H) and eventually the output catches the input ptr
void  CCmdFifo::Shift(void)
	{
	EnterCriticalSection(m_pCS);
	if (m_Size == 0)	// nothing in the FIFO
		{
		m_In = m_Out = 0;
		m_nFifoEmptyCount++;
		LeaveCriticalSection(m_pCS);
		return;
		}
	// since there is something in the fifo, shift it to the very front.
	// when we get close to the physical end of memory
	if (m_Out > (CMD_FIFO_MEM_SIZE - 0x10000))
		{
		memcpy(&m_Mem[0], &m_Mem[m_Out], m_Size);
		m_In = m_Size;
		m_Out = 0;	// point to the beginning of the fifo
		m_PhysicalShiftCount++;
		}
	LeaveCriticalSection(m_pCS);
	}
BYTE * CCmdFifo::GetOutLoc(void)
	{
	return &m_Mem[m_Out];
	}

BYTE * CCmdFifo::GetInLoc(void)
	{
	CString s;
	BYTE *pMem;
	EnterCriticalSection(m_pCS);
	if (m_In < 0)
		{
		m_In = m_Out = m_Size = 0;
		m_bError = 1;
		// SET buffer overflow bit
		s = _T( "CCmdFifo: m_In < 0\n" );
		pMainDlg->SaveDebugLog(s);

		}
	if (m_In >= CMD_FIFO_MEM_SIZE - 0x8000)
		{	// getting close to end of FIFO
		if (m_Out < m_In)	// empty space at front of buffer
			{
			if (m_Out)
				memcpy(&m_Mem[0], &m_Mem[m_Out], m_Size);	//skip if m_Out == 0
			m_In = m_Size;
			m_Out = 0;
			m_PhysicalShiftCount++;
			}
		else
			{	// flush the buffer and set error bit in Idata
			m_In = m_Out = m_Size = 0;
			m_bError = 1;	// overflow
			m_FlushCount++;
			s.Format( _T( "CCmdFifo: Buffer Overflow %d\n"), m_FlushCount);
			pMainDlg->SaveDebugLog(s);
			TRACE(s);
			}
		}
	pMem = (BYTE *)&m_Mem[m_In];
	LeaveCriticalSection(m_pCS);
	return pMem;
	}

// Normally we would store individual byte into the FIFO one byte at a time
// But here we will use the OnReceive operation to copy bytes into a pointer
// location. That location will be m_Mem[m_In]. This function updates size 
// and the next input location
void CCmdFifo::AddBytesToFifo(int n)
	{
	CString s;
	if (m_pCS)
		{
		EnterCriticalSection(m_pCS);
		m_In += n;
		m_Size += n;
		if (m_Size > m_nMaxDepth)
			m_nMaxDepth = m_Size;
		LeaveCriticalSection(m_pCS);
		}
	else
		{
		s = _T("AddBytesToFifo critical section ptr is 0\n");
		TRACE(s);
		pMainDlg->SaveDebugLog(s);
		}
	}

// after 2016-12-13 packet size is not fixed.
// This will flush the input buffer if the sync is bad or the
// amount of data is larger than the memory buffer size of CMD_FIFO_MEM_SIZE = 0x4000
int CCmdFifo::GetPacketSize(void)
	{
	/*
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD
	*/
	int i = 0;
	EnterCriticalSection(m_pCS);
	CString s,t;
	WORD *pW = (WORD *)&m_Mem[m_Out];// debugging
	GenericPacketHeader *pHeader;
	IDATA_FROM_HW *pIdata;
	pHeader = (GenericPacketHeader *)pW;		// &m_Mem[m_Out];
	pIdata = (IDATA_FROM_HW *)pW;			// pHeader;
	//if ((pHeader->uSync != SYNC) || (pHeader->wByteCount > sizeof(IDATA_PAP)))	// 1064
	if ((pHeader->uSync != SYNC) || (pHeader->wByteCount > 0x2000))	// 0x4000
		{	// we are lost in the data, reset the FIFO and set an error bit
		m_In = 0;
		m_Out = 0;
		m_Size = 0;
		m_bError = 2;	// lost sync
		// POST thread message to ServerSocketOwner thread to flush the command queue and send reset msg to
		// ADC or Pulser board. Wiznet looses sync and dma's data on the back of another data packet
		// which data stream is this?

		s = _T("");
		t = _T("No OverFlow ");
		if (pHeader->uSync != SYNC)
			s = _T( "Lost Sync .. " );
		if (pHeader->wByteCount > 0x2000)
			{
			t = _T("FIFO OVERFLOW ");
			}
		s += t;
		if (m_Type == 'C')
			{
			t.Format(_T("Client[%d]\n"), m_CSnum);
			}
		else
			{
			t.Format(_T("Srv[%d].Client[%d]\n"), m_CSnum, m_SrvClientNum);
			if (stSCM[m_CSnum].pClientConnection[m_SrvClientNum]->pSocket)
				stSCM[m_CSnum].pClientConnection[m_SrvClientNum]->pSocket->Close();

			//CServerSocketOwnerThread *pThread = stSCM[m_CSnum].pClientConnection[m_SrvClientNum]->pServerSocketOwnerThread;
			//pThread->PostThreadMessage(WM_USER_SERVER_FLUSH_CMD_PACKETS, (WORD)m_SrvClientNum, 0L);
			}
		s += t;
		TRACE(s);
		pMainDlg->SaveDebugLog(s);
		// 2018-10-15 count lost sync's. 4 in a row, send packet to reset wiznet
		m_nLostSyncCnt++;
#if 1
		if (m_nLostSyncCnt >= 4)
			{
			m_Out = m_In = m_Size = m_nLostSyncCnt = 0;
			s = _T("Reinitialize FIFO\n");
			TRACE(s);
			pMainDlg->SaveDebugLog(s);
			}
#endif
		LeaveCriticalSection(m_pCS);
		return 0;
		}
	m_PacketSize = pHeader->wByteCount;
	// debugging
	if ((m_PacketSize < 24) || (m_PacketSize > 1460))
		{
		m_bError = 4;	// wrong packet size
		s = _T( "Wrong Packet Size\n" );
		TRACE(s);
		pMainDlg->SaveDebugLog(s);
		LeaveCriticalSection(m_pCS);
		return 0;
		}
			// See if we are losing a real packet
#if 0
	// these checks made in OnReceive of both Server and Client
	// comment out when running with simulator since simulator sends 2 packets from same IP address
	// using same command fifo.
	if (pIdata->wMsgSeqCnt != m_wMsgSeqCnt )
		{
		i = pIdata->wMsgSeqCnt - m_wMsgSeqCnt;	// debugging
		// set error bit for lost msg
		s.Format( _T( "MsgCnt = %d, expected = %d \n" ),pIdata->wMsgSeqCnt, m_wMsgSeqCnt);
		pMainDlg->SaveDebugLog(s);
		TRACE(s);
		}
#endif
	m_wMsgSeqCnt = (pIdata->wMsgSeqCnt) & 0xffff;
	m_nLostSyncCnt = 0;
	LeaveCriticalSection(m_pCS);
	return m_PacketSize;
	}

// Maybe we don't want to remove the packet just yet
// Peek at the packet for doin all-wall processing

BYTE* CCmdFifo::PeakNextPacket(void)
	{
	CString s;
	int i = 10;	//debug
	EnterCriticalSection(m_pCS);
	BYTE *pStart = &m_Mem[m_Out];		// beginning of NEXT whole packet(s) memory
	GenericPacketHeader *pHeader = (GenericPacketHeader *)pStart;
	if ((pHeader->uSync != SYNC) || (pHeader->wByteCount > sizeof(IDATA_PAP)))
		{	// we are lost in the data, reset the FIFO and set an error bit
		m_In = 0;
		m_Out = 0;
		m_Size = 0;
		s = _T("Lost Sync or wrong packet size\n");
		pMainDlg->SaveDebugLog(s);
		TRACE(s);
		LeaveCriticalSection(m_pCS);
		return NULL;
		}
	LeaveCriticalSection(m_pCS);
	return pStart;
	}


// Remove a PacketSize chunk from the FIFO. May leave a portion packet in the FIFO
// If the input ptr aproaches the end of the memory, reset the FIFO to avoid wrap around problems associated
// with typical software FIFO's
// 2016-12-12 Allow variable size packets. Cmd and data packets will have the sync word and a 
// byte count at a fixed location in the packet header. If the sync word is not found, the FIFO will be
// flushed and a status bit in the Idata will be set to inform the down stream processors that the 
// command queue has been flushed by the PAP.
BYTE *CCmdFifo::GetNextPacket(void)
	{ 
	/*
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD
	*/
	CString s;
	int i = 10;	//debug
	EnterCriticalSection(m_pCS);
	BYTE *pStart = &m_Mem[m_Out];		// beginning of NEXT whole packet(s) memory
	GenericPacketHeader *pHeader = (GenericPacketHeader *)pStart;
	if ((pHeader->uSync != SYNC) || (pHeader->wByteCount > sizeof(IDATA_PAP) ) )
		{	// we are lost in the data, reset the FIFO and set an error bit
		m_In = 0;
		m_Out = 0;
		m_Size = 0;
		s = _T( "Lost Sync or wrong packet size\n" );
		pMainDlg->SaveDebugLog(s);
		TRACE(s);
		// If this happens twice in a row, send message to adc board to reset Wiznet
		LeaveCriticalSection(m_pCS);
		return NULL;
		}
	m_PacketSize = pHeader->wByteCount;
	m_Out  += m_PacketSize;	// move to next packet
	m_Size -= m_PacketSize;	// reduce size of unread data in FIFO.;
	if (m_Size < 0)
		{
		m_In = 0;
		m_Out = 0;
		m_Size = 0;
		s = _T( "CCmdFifo: m_Size < 0\n" );
		pMainDlg->SaveDebugLog(s);
		TRACE(s);
		LeaveCriticalSection(m_pCS);
		return NULL;
		}
	if ((m_Size > 0) && (m_Size < 16))
		i = 2;
	else i = 0;
	// can't reset the FIFO here because the caller has not yet used the data
	// we are only returning a pointer to the data. Can't change FIFO until the data is used/moved.
	// caller will have to check size after extracting the packet and call reset
	// when the size is 0
	// If caller fails to check for size then we will overflow the buffer. No wrap around provided here
#if DEBUG_TCPIP_FROM_INSTRUMENT
	s.Format(_T("FIFO in, out, size Sync = %5d %5d %5d 0x%08x\n"), 
		m_In, m_Out, m_Size, pHeader->uSync);
	TRACE(s);
#endif
	LeaveCriticalSection(m_pCS);
	return pStart;
	}

// Return the error status of the fifo and reset the error flag
// bit 0= overflow, 1=Lost Sync
BYTE CCmdFifo::GetError( void )
	{
	BYTE tmp;
	EnterCriticalSection(m_pCS);
	tmp = m_bError;
	m_bError = 0;
	LeaveCriticalSection(m_pCS);
	return tmp;
	}