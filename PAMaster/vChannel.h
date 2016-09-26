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
	BYTE bGetIdGateMax(void)	{ return m_GateID;	}
	BYTE bGetOdGateMax(void)	{ return m_GateOD;	}

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

	WORD wGetWallSum(void)	{ return NxFifo.uSum;	}
	WORD wGetBadWallCount(void)	{ return NxFifo.wBadWall;	}
	void ClearBadWallCount(void)	{ NxFifo.wBadWall = 0;	}
	WORD wGetGoodConsecutiveCount(void)	{ return NxFifo.wGoodWall;	};
	WORD wGetMaxWall(void);		// resets max wall to 0 in order to find a new max
	WORD wGetMinWall(void);		// resets min wall to 0xffff in order to find new min
	void ResetGatesAndWalls(void);	// Initialize max and min for next sampling period
	float GetWallScaler(WORD Nx);	// return the wall scaler/Nx for deriving Max/Min wall from integer sums
	void SetNx(BYTE bNx);
	BYTE bGetNx(void)					{ return NxFifo.bNx;		  }
	void SetDropCount(WORD wDropOut)	{ NxFifo.wDropOut = wDropOut; }
	WORD GetDropCount(void)				{ return NxFifo.wDropOut;	  }
	WORD wGetMaxWallLimit(void)			{ return NxFifo.wWallMax;	}
	WORD wGetMinWallLimit(void)			{ return NxFifo.wWallMin;	}

	/*********************** Wall processing routines ***********************/

	BYTE m_GateID, m_GateOD;			// Nc qualified max gate value over data sampling period (16 Frames)
	WORD m_wTOFMaxSum, m_wTOFMinSum;	// Max, min wall sum reading over data sampling period
	WORD m_wTOFMax, m_wTOFMin;
	float m_fWallScaler;				// hardware count to 0.001 inch divided by Nx approx 1.452/Nx
	};	

#endif