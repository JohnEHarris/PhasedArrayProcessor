/*
A class to simulate hardware for processing UT channels
There shoud be a class instance for every channel in a UT instrument
Author:		jeh
Date:		01-Jun-2016
Revised:	modeled somewhat like RunningAverage. The two may be merged in the future. Simulates
			FIFO's holding wall and flaw values. Used to compute Nc and Nx values
			2016-07-11 a dummy class to satisfy ST_SERVERS_CLIENT_CONNECTION pointers to CvChannel[]

*/

#include "stdafx.h"
#include "vChannel.h"
#include "..\Include\Global.h"

#ifdef I_AM_PAP
//#include "ServiceApp.h"
//#include "InstMsgProcess.h"
//extern UINT uVchannelConstructor[MAX_CLIENTS_PER_SERVER][MAX_SEQ_COUNT][MAX_CHNLS_PER_MAIN_BANG];
#endif







// NcNx values are loaded by the ServiceApp program and are data structures in that program.
// When an instrument disconnects and then reconnects, these structures remain in place.
// 2017-04-18 now part of ClientConnection structure. Created when client connects,
// destroyed when client instrument disconnects
// Must reload from GUI or store last good Nc Nx info in a static table
//CvChannel::CvChannel(int nInst, int nSeq, int nChnl)
CvChannel::CvChannel(int nSeq, int nChnl)
	{
#if 1
	// id/od, Nc, Thold, bMod
	FifoInit(0,1,20,1);	// id default 0,1,20,1
	FifoInit(1,1,20,1);	// od default 1,1,20,1
	FifoInit(2,1,20,1);	// interface gate1
	
	m_bInputCnt = 0; 
	// Wall processing routines
	// Nx, Max allowed, Min, DropOut cnt
	WFifoInit(1,1377,27,10);	// nominal 1,1377,27,4
	//if ( nInst > 3) return;
	//if (nChnl > 39) return;
	// counter of how many time constructor runs for each chnl/instrument
	//uVchannelConstructor[nSeq][nChnl]++;
	memset(&m_PeakData,0, sizeof (stPeakChnlPAP));
	m_bChnl = nChnl;
	m_bSeq  = nSeq;
	m_PeakData.bChNum = 8*nSeq + nChnl;	//specific to Sam's hardware design
	w_DefaultConfig = DEFAULT_CFG;
#endif

	};

CvChannel::~CvChannel()
	{
	};

#if 1

// Data is collected over a number of frames or main-bangs (typically 16)
// After data is collected and reported out to the Mill Console system,
// Sampling states (peak holds) are reset
// The data within the FIFO's is not disturbed so max/min reading
// can migrate across 16 frame boundaries
// Since every channel does wall and flaw processing, only call this from wall input routine.
void CvChannel::ResetGatesAndWalls(void)
	{
	//m_GateID = m_GateOD = 0;	// byte values - range 0-127
	m_wTOFMaxSum = 0;
	m_wTOFMinSum = 0xffff;
	NxFifo.wBadWall = NxFifo.wGoodWall = 0;
	NcFifo[0].bMaxFinal = NcFifo[1].bMaxFinal = 0;
	}

/*********************** Flaw processing routines ***********************/
// Set all parameters necessary to enforce Nc rules
void CvChannel::FifoInit(BYTE bIdOd, BYTE bNc, BYTE bThld, BYTE bMod)
	{
	Nc_FIFO *pFifo;
	if (bIdOd > 2)	 bIdOd = 2;
	//bIdOd &= 1;	// limit range to 0-1
	pFifo = &NcFifo[bIdOd];
	memset( (void *) pFifo,0, sizeof (Nc_FIFO));
	if (bNc > 16)  bNc = 16;
	if (bMod > 16) bMod = 16;
	if (bMod < bNc) bMod = bNc;
	pFifo->bNc = bNc;
	if ( bThld < 3) bThld = 2;		// 2 % is minimum thold allowed.
	pFifo->bThold = bThld;
	pFifo->bMod = bMod;
	w_DefaultConfig = 0;	// not using default config
	// Constructor sets this true after calling FifoInit()
	// NcNx command does not.
	};

// An amplitued is input and an Nc qualified reading is returned.
// bIdOd selects which FIFO id=0, od=1
// If Nc is 0 return immediately
BYTE CvChannel::InputFifo(BYTE bIdOd,BYTE bAmp)
	{
	int i = 0;
	Nc_FIFO *pFifo;
	if (bIdOd > 2)	 bIdOd = 2;
	pFifo = &NcFifo[bIdOd];
	if (pFifo->bNc == 0) return 0;	// nothing or a wall channel only
	//if (bAmp > 0xc0)
	//	i = i+3;

	i = pFifo->bInPt;		// slot position in the fifo
	pFifo->bCell[i] = bAmp;	// replace oldest element
	pFifo->bInPt++;		// advance to next oldest position
	if (pFifo->bInPt >= pFifo->bMod)	pFifo->bInPt = 0;	// fifo is only bMod deep

	pFifo->bAboveThld = 0;	// get ready to count fifo entries above or equal to thold
	pFifo->bMaxTemp   = 0;	

	for ( i = 0; i < pFifo->bMod; i++)
		{
		if (pFifo->bCell[i] >= pFifo->bThold)	pFifo->bAboveThld++;
		if (pFifo->bMaxTemp < pFifo->bCell[i])	pFifo->bMaxTemp = pFifo->bCell[i];
		}

	if (pFifo->bAboveThld >= pFifo->bNc)		pFifo->bMax = pFifo->bMaxTemp;		// an Nc qualified above thold flaw

	// some above but not enough to call a flaw
	// we don't want to show a signal 1 or 2 % below thold ... avoid arguments with customer.
	// 1st case: 1 or a few above thold but doesn't meet nc criteria
	else if (pFifo->bMaxTemp >= pFifo->bThold)	pFifo->bMax = (pFifo->bThold*4)/5;	// 80 % to thold

	// 2nd case: just 1 or 2 % below thold, but close
	else if ((pFifo->bThold - pFifo->bMaxTemp) < 3)	pFifo->bMax = (pFifo->bThold*4)/5;

	// max is more than 2 % below thold
	else	pFifo->bMax = pFifo->bMaxTemp;

	if (pFifo->bMaxFinal < pFifo->bMax)
		{
		pFifo->bMaxFinal = pFifo->bMax;
		if (bIdOd == 0)
			m_PeakData.bId2 = pFifo->bMax;
		else if (bIdOd == 1)
			m_PeakData.bOd3 = pFifo->bMax;
		}
	return pFifo->bMaxFinal;
	};

void CvChannel::FifoClear(BYTE bIdOd)	// zero fifo entries/cells, keep other parameters
	{
	Nc_FIFO *pFifo;
	//bIdOd &= 1;	// limit range to 0-1
	if (bIdOd > 2)	 bIdOd = 2;
	pFifo = &NcFifo[bIdOd];
	memset( (void*) &pFifo->bCell[0], 0, sizeof (pFifo->bCell));
	}

// bMod = bNc when SetNc called. No nc out of m ascans
void CvChannel::SetNc(BYTE bIdOd, BYTE bNc)
	{
	Nc_FIFO *pFifo;
	//bIdOd &= 1;	// limit range to 0-1
	if (bIdOd > 2)	 bIdOd = 2;
	pFifo = &NcFifo[bIdOd];
	if (bNc > 16)  bNc = 16;
	pFifo->bNc = pFifo->bMod = bNc;
	}
/*********************** Flaw processing routines ***********************/


/*********************** Wall processing routines ***********************/

// bNx limited to 1-8
// wMax, wMin are allowed wall value limits. Wall outside these limits are discarded
// as erroneous readings
void CvChannel::WFifoInit(BYTE bNx, WORD wMax, WORD wMin, WORD wDropOut)
	{
	memset((void *) &NxFifo, 0, sizeof(NxFifo));
	if (bNx > 8) bNx = 8;
	//else if (bNx < 1) bNx = 1; // 2016-06-15 bNx = 0 -> not a wall channel
	NxFifo.bNx = bNx;
	NxFifo.wWallMax = wMax;	// maximum allowed value for a wall reading
	NxFifo.wWallMin = wMin;	// minimum allowed value for a wall reading
	m_wTOFMaxSum	= 0;
	m_wTOFMinSum	= 0xffff;
	NxFifo.wDropOut = wDropOut;
	m_fWallScaler	= GetWallScaler((WORD)bNx);
	m_bInputCnt		= 0;
	m_wBadInARow	= 0;
	}

// Wall data is summed over Nx samples. The sums are peak held for max wall and min held
// for minimum wall. After 16 accepted inputs, the Max and Min are reset.
// Min wall sum is only invalid for the first 16 inputs after the process starts. After that min
// wall sum is valid until the FIFO is reinitialized.
WORD CvChannel::InputWFifo( WORD wWall )
	{
	int i;
	WORD wOldWall;
	Nx_FIFO *pFifo = &NxFifo;

	if (pFifo->bNx == 0)
		m_PeakData.wTofMin = 10;	// not a wall channel

	/*******************************************************/
	else
		{	// is a wall channel
			// prevent out of range values from entering the fifo
		if ((wWall < pFifo->wWallMin) || (wWall > pFifo->wWallMax))
			{
			pFifo->wBadWall++;
			m_wBadInARow++;
			if (m_wBadInARow <= 0xf)
				{
				m_wStatus &= 0xfff0;	// preserve upper 3 nibbles
				m_wStatus |= m_wBadInARow;
				}
			else
				m_wStatus |= 0xf;	// limit to bits 0..3

			if (m_wBadInARow >= NxFifo.wDropOut)
				SetDropOut();
			//else ClearDropOut(); done in CServerRcvListThread:: AddToIdataPacket()
			//goto COUNT_INPUTS;
			}

		else
			{
			// wall within range


			pFifo->wGoodWall++;	// good wall + bad wall should == 16
			m_wBadInARow = 0;


			i = pFifo->bInPt++;			// slot position in the fifo and increment to next
			if (i >= pFifo->bNx)
				i = 0;
			if (pFifo->bInPt >= pFifo->bNx)
				pFifo->bInPt = 0;
			wOldWall = pFifo->wCell[i];			// get oldest wall reading
			pFifo->wCell[i] = wWall;			// replace oldest element with this one
			pFifo->uSum += (wWall - wOldWall);	// change in sum is new - old
			if (m_wTOFMaxSum < pFifo->uSum)
				{
				m_wTOFMaxSum = pFifo->uSum;
				}
				// Omitting the next line only results in a false min wall on the first sampling interval of 16
				// Thereafter the min wall sum is correct.
				//	if (m_bInputCnt >= pFifo->bNx)
			if (m_wTOFMinSum > pFifo->uSum)
				{
				m_wTOFMinSum = m_PeakData.wTofMin = pFifo->uSum;
				}
			}	// wall within range

		}		// is a wall channel
	/*******************************************************/

	return pFifo->uSum;
	}

// Increment an input counter until we get ASCANS_TO_AVG
#if 0
void CvChannel::CountInputs( void )
	{
	Nx_FIFO *pFifo = &NxFifo;
	BYTE dbgCh, dbgSeq, bx;
	dbgCh = m_bChnl;
	dbgSeq = m_bSeq;
	if ((dbgCh == 0) && (dbgSeq == 0))
		bx = 2;
	++m_bInputCnt;
	if (m_bInputCnt >= ASCANS_TO_AVG)
		{
		if ((m_wStatus & SET_READ) == 0)	// previous peak data has not been read yet
			SetOverRun();
		else ClearOverRun();
		ClrRead();
		/********  how we know we have enough data  *********/
		m_bInputCnt = 0;	// how we know we have enough data
		m_PeakData.bId2 = NcFifo[0].bMaxFinal;
		m_PeakData.bOd3 = NcFifo[1].bMaxFinal;
		// not a wall channel
		if (pFifo->bNx == 0)
			{
			m_PeakData.wTofMin = /*m_PeakData.wTofMan = */ 10;
			}
		else
			{
			m_PeakData.wTofMin = m_wTOFMinSum;
			}
		}

	}
#endif

BYTE CvChannel::AscanInputDone( void )
	{
	int ntmp;
	if ((m_bSeq == 0) && (m_bChnl == 0))
		ntmp = 2;	 // debug are we counting correctly?
	m_bInputCnt++;
	if (m_bInputCnt >= ASCANS_TO_AVG)
		{
		m_bInputCnt = 0;
		return 1;
		}

	return 0;
	}

// the scaler is applied to the wall sum in raw counts to obtain the calibrated wall reading as a word.
float CvChannel::GetWallScaler(WORD Nx)
	{
	float fNx = Nx;
	if ( Nx == 0)
		{
		m_fWallScaler = 1.452f;
		return m_fWallScaler;
		}
	m_fWallScaler = 1.452f/fNx;
	return m_fWallScaler;
	}

// Return harware count sums. Let windows apply scale factor and Nx average.
WORD CvChannel::wGetMaxWall(void)
	{
	m_wTOFMax = m_wTOFMaxSum;	//(WORD) ((float)m_wTOFMaxSum * m_fWallScaler );
	return m_wTOFMax;
	}

WORD CvChannel::wGetMinWall(void)
	{
	m_wTOFMin = m_wTOFMinSum;	//(WORD) ((float)m_wTOFMinSum * m_fWallScaler );
	return m_wTOFMin;
	}

void CvChannel::SetNx(BYTE bNx)
	{
	if (bNx > 8) bNx = 8;
	else if (bNx < 1) bNx = 1;
	NxFifo.bNx = bNx;
	}

// copy the bad wall amount into the PeakData structure
void CvChannel::SetBadWall(BYTE badWall)
	{
	BYTE bOld;	// get bottom 5 bits of wStatus
	bOld = m_wStatus & 0x1f;
	if ((badWall > bOld) && (badWall < 0x1f))
		m_wStatus |= badWall;
	}

// Once ServerRcvListThread has read the data, clear the structure for the next 16 Ascans
void CvChannel::CopyPeakData(stPeakChnlPAP *pOut)
	{
	CString s;
	// Check for default constructor before copying data
	if (w_DefaultConfig)	m_wStatus |= DEFAULT_CFG;	// Still using default values
	else					m_wStatus &= ~DEFAULT_CFG;	// clear default bit
	//memcpy( (void *)pOut, (void *) &m_PeakData, sizeof(m_PeakData));
	pOut->bId2 = bGetIdGateMax();
	pOut->bOd3 = bGetOdGateMax();
	pOut->wTofMin = wGetMinWall();
	pOut->wTofMax = wGetMaxWall();
	pOut->bChNum = m_bChnl; 
	pOut->bStatus = m_wStatus & 0xff;
	// debugging only
	s.Format( _T( "CopyPeak Id=%d, Min Wall=%d, Max Wall=%d, Ch=%d, Stat=%x\n" ), pOut->bId2, 
		pOut->wTofMin, pOut->wTofMax, m_bChnl, pOut->bStatus);
	TRACE( s );
	}

// this channel copies its information into the slot in output data reserved for it.
// More than just copying, it does a peak hold and records the sequence number for the peaking operation
// Sequence number will allow more precise pipe location since header applies to all sequences
// nSeq in formal arg ranges up to 31. It is the seq number of the input data. Not the unique seq data
// which defines the channel. If gModulo is 4, then there are 8 sets of 4 sequences(virtual channels)
// m_bSeq is a number from 0-3. But Idata has a nSeq value from 0-7
void CvChannel::CopyPeakToIdata(IDATA_PAP *pOut, int nSeq)
	{
	int nCh;
	int nDxgate2, nDxgate3, nDxMax, nDxMin;		// will become structure elements in stPeakChnlPAP eventually 2017-10-24
	//nCh = (nSeq  % gnSeqModulo)*gMaxChnlsPerMainBang + m_bChnl;
	nCh = m_PeakData.bChNum;
	nDxMax = 0;	// make compiler happy for now
	// Check for default constructor before copying data
	if (w_DefaultConfig)	pOut->PeakChnl[nCh].bStatus |= DEFAULT_CFG;	// Still using default values
	else					pOut->PeakChnl[nCh].bStatus &= ~DEFAULT_CFG;	// clear default bit
	pOut->PeakChnl[nCh].bId2 = bGetIdGateMax();
	pOut->PeakChnl[nCh].bOd3 = bGetOdGateMax();
	pOut->PeakChnl[nCh].wTofMin = wGetMinWall();
	pOut->PeakChnl[nCh].wTofMax = wGetMaxWall();
	pOut->PeakChnl[nCh].bChNum = nCh;

	//memcpy ((void*) &pOut->, &m_PeakData, sizeof(m_PeakData));
	//pOut->PeakChnl[nCh].bChNum = nCh;

#if 0		// maybe later
	if (pOut->PeakChnl[nCh].bId2 < m_PeakData.bId2)
		{
		pOut->PeakChnl[nCh].bId2 = m_PeakData.bId2;
		nDxgate2 = nSeq;
		}
	if (pOut->PeakChnl[nCh].bOd3 <  m_PeakData.bOd3)
		{
		pOut->PeakChnl[nCh].bOd3 = m_PeakData.bOd3;	//m_PeakData.bOd
		nDxgate3 = nSeq;
		}
	// wall peak holding. Idata pap has only min wall for each sequence
	if (pOut->PeakChnl[nCh].wTofMin == 0)	// initial condition when pOut created
		{
		//if ((m_PeakData.wTofMin == m_PeakData.wTofMAX) &&
		//	 (m_PeakData.wTofMin == 0)
				{
				pOut->PeakChnl[nCh].wTofMin = m_PeakData.wTofMin;
				nDxMin = nSeq;
				//pOut->PeakChnl[m_bChnl].wTofMax = m_PeakData.wTofMin;	// capture the max
				//nDxMax = nSeq;
				}
		}
	else
		{
		if (pOut->PeakChnl[nCh].wTofMin > wGetMinWall() )
			{
			pOut->PeakChnl[nCh].wTofMin = wGetMinWall();
			nDxMin = nSeq;
			}
			
		//if (pOut->PeakChnl[m_bChnl].wTofMax < m_PeakData.wTofMin)
		//	{
		//	pOut->PeakChnl[m_bChnl].wTofMax = m_PeakData.wTofMin;
		//	nDxMax = nSeq;  
		//	}
		}
#endif
	}

// copy FIFO variables to PeakData structure after the peak hold is complete
void CvChannel::GetPeakData(void)
	{
	m_PeakData.bId2 = NcFifo[0].bMaxFinal;
	m_PeakData.bOd3 = NcFifo[1].bMaxFinal;
	m_PeakData.wTofMin = m_wTOFMinSum;
	//m_PeakData.wTofMax = m_wTOFMaxSum;
	}


#endif