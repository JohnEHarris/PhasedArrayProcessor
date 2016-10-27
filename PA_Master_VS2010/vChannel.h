/*
Author: jeh
Date:	2016-05-31
Purpose:Simulate hardware processing of wall and flaw channels. One class instance for each channel
		Will perform Nc and Nx operations on a give Ascan out of 32 (at this time).
		Over 16 complete sequences of time will generate the Nc out put for a flaw channel and the Nx wall reading
		providing both max and min wall reading for the interval.
		1  1  1  1 -> processed value for Nx/Nx for first "channel" sequence
		2  2  2  2 ->
		:
		32 32 32 32->

		The receiver thread in the Phased Array Master will use pointers to this class to characterize the operation
		of the ut instrument. The will be an instantiated class instance for each "channel" based on its position
		in the sequence of Ascans.

*/

#ifndef V_CHANNEL_H
#define V_CHANNEL_H

#include "ServiceApp.h"

class CvChannel
	{
public:
	CvChannel(int nInst, int nSeq, int nChnl);
	virtual ~CvChannel();

	/*********************** Flaw processing routines ***********************/
	// Nc_FIFO is defined in PA2Struct.h
	Nc_FIFO NcFifo[2];	// id=0, od=1

	// An amplitued is input and an Nc qualified reading is returned.
	// bIdOd selects which FIFO id=0, od=1,bAmp is the input to the fifo
	BYTE InputFifo(BYTE bIdOd,BYTE bAmp);

	// bMod = bNc when SetNc called.
	void SetNc(BYTE bIdOd, BYTE bNc);

	// bMod is the 'm' when Nc func is n out of m ascans above thold
	// for example 2 out of 3 Ascans above thold to qualify bMod = 3, Nc = 2
	// bMod is >= Nc
	void FifoInit(BYTE bIdOd, BYTE bNc, BYTE bThld, BYTE bMod);
	void FifoClear(BYTE bIdOd);	// zero fifo entries, keep other parameters
	BYTE bGetIdGateMax(void)	{ return NcFifo[0].bMaxFinal;	}
	BYTE bGetOdGateMax(void)	{ return NcFifo[1].bMaxFinal;;	}

	BYTE bGetNcId(void)			{ return NcFifo[0].bNc;	}
	BYTE bGetNcOd(void)			{ return NcFifo[1].bNc;	}
	BYTE bGetMId(void)			{ return NcFifo[0].bMod;	}
	BYTE bGetMOd(void)			{ return NcFifo[1].bMod;	}
	BYTE bGetThldId(void)		{ return NcFifo[0].bThold;	}
	BYTE bGetThldOd(void)		{ return NcFifo[1].bThold;	}
		
	/*********************** Flaw processing routines ***********************/
	/*======================================================================*/

	/*********************** Wall processing routines ***********************/
	Nx_FIFO NxFifo;	// only 1 wall reading per channel

	WORD InputWFifo(WORD wWall);	// input hw wall reading, returns bad wall count if failure, 0 on success
	// 0 on success does not mean there have not been bad readings, only that the current one was ok

	// Max and min allowed wall are expressed in hardware counts
	void WFifoInit(BYTE bNx, WORD wMax, WORD wMin, WORD wDropOut);

	WORD wGetWallSum(void)			{ return NxFifo.uSum;	}
	WORD wGetBadWallCount(void)		{ return NxFifo.wBadWall;	}
	void ClearBadWallCount(void)	{ NxFifo.wBadWall = 0;	}
	WORD wGetGoodWallCount(void)	{ return NxFifo.wGoodWall;	};
	WORD wGetMaxWall(void);		// returns largest sum seen over 16 Ascans
	WORD wGetMinWall(void);		// returns smallest sum seen over 16 Ascans
	void ResetGatesAndWalls(void);	// Initialize max and min for next sampling period
	float GetWallScaler(WORD Nx);	// return the wall scaler/Nx for deriving Max/Min wall from integer sums
	void SetNx(BYTE bNx);
	BYTE bGetNx(void)					{ return NxFifo.bNx;		  }
	void SetDropCount(WORD wDropOut)	{ NxFifo.wDropOut = wDropOut; }
	WORD GetDropCount(void)				{ return NxFifo.wDropOut;	  }
	WORD wGetMaxWallLimit(void)			{ return NxFifo.wWallMax;	}
	WORD wGetMinWallLimit(void)			{ return NxFifo.wWallMin;	}
	WORD wGetBadInARow(void)			{ return m_wBadInARow;		}
	BYTE bGetAscansInFifo(void)			{ return m_bInputCnt;		}

	/*********************** Wall processing routines ***********************/
	/*********************** Result FIFO routines ***********************/
	// Not a fifo at 2016-10-05. May grow to be one
	// 10-27-16 change bStatus to wStatus
	stPeakData m_PeakData;
	void SetBadWall(BYTE badWall);
	void SetDropOut(void)			{ m_PeakData.wStatus |= SET_DROPOUT;	}
	void ClearDropOut(void)			{ m_PeakData.wStatus &= CLR_DROPOUT;	}
	void SetOverRun(void)			{ m_PeakData.wStatus |= SET_OVERRUN;	}
	void ClearOverRun(void)			{ m_PeakData.wStatus &= CLR_OVERRUN;	}
	void SetPeakDataReady(void)		{ m_PeakData.wStatus |= DATA_READY;	}
	void ClearPeakDataReady(void)	{ m_PeakData.wStatus &= CLR_DATA_READY;	}
	void SetRead(void)				{ m_PeakData.wStatus |= SET_READ;		}
	void ClrRead(void)				{ m_PeakData.wStatus &= CLR_READ;		}
	void GetPeakData(void);
	void PeakDataClear(void);		// Once PAP copies data into Ethernet Packet, clear PeakData
	// pOut is a slot in the ethernet packet to be sent
	void CopyPeakData(stPeakData *pOut);
	//BYTE GetAscanCounter(void)		{ return m_bInputCnt;				}
	BYTE AscanInputDone(void)		{ return (m_bInputCnt == 0);		}	// 0 if not all 16 Ascans

	
	
	/*********************** Result FIFO routines ***********************/

	//BYTE m_GateID, m_GateOD;			// Nc qualified max gate value over data sampling period (16 Frames)
	WORD m_wTOFMaxSum, m_wTOFMinSum;	// Max, min wall sum reading over data sampling period
	WORD m_wTOFMax, m_wTOFMin;
	float m_fWallScaler;				// hardware count to 0.001 inch divided by Nx approx 1.452/Nx
	BYTE m_bInputCnt;					// count the number of inputs. Generate max/min sums on every 16th input count
										// then reset max min sum values
	WORD m_wBadInARow;					// consecutive bad wall reading. Carries over 16 sample boundaries. Cleared
										// with one reading within upper/lower limits.
	BYTE m_bChnl;
	BYTE m_bSeq;
	};	

#endif