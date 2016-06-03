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
	CvChannel(int nInst, int nChnl);
	virtual ~CvChannel();

	/*********************** Flaw processing routines ***********************/
	// Nc_FIFO is defined in PA2Struct.h
	Nc_FIFO NcFifo[2];	// id=0, od=1

	// An amplitued is input and an Nc qualified reading is returned.
	// bIdOd selects which FIFO id=0, od=1,bAmp is the input to the fifo
	BYTE InputFifo(BYTE bIdOd,BYTE bAmp);


	// Nc_FIFO is defined in PA2Struct.h
	Nc_FIFO NcFifo[2];	// id=0, od=1

	// bMod = bNc when SetNc called.
	void SetNc(BYTE bIdOd, BYTE bNc);

	// bMod is the 'm' when Nc func is n out of m ascans above thold
	// for example 2 out of 3 Ascans above thold to qualify bMod = 3, Nc = 2
	// bMod is >= Nc
	void FifoInit(BYTE bIdOd, BYTE bNc, BYTE bThld, BYTE bMod);
	void FifoClear(BYTE bIdOd);	// zero fifo entries, keep other parameters
		
	/*********************** Flaw processing routines ***********************/
	/*======================================================================*/

	/*********************** Wall processing routines ***********************/
	Nx_FIFO NxFifo;	// only 1 wall reading per channel
	//Nx_FIFO_RETURN NxFifoReturn;	// instance of return values from wall FIFO

	WORD InputWFifo(WORD wWall);	// input hw wall reading, returns bad wall count if failure, 0 on success
	// 0 on success does not mean there have not been bad readings, only that the current one was ok

	// Max and min allowed wall are expressed in hardware counts
	void WFifoInit(BYTE bNx, WORD wMax, WORD wMin, WORD wDropOut);

	WORD wGetWallSum(void)	{ return NxFifo.uSum;	}
	WORD wGetBadWallCount(void)	{ return NxFifo.wBadWall;	}
	void ClearBadWallCount(void)	{ NxFifo.wBadWall = 0;	}
	WORD wGetGoodConsecutiveCount(void)	{ return NxFifo.wGoodWall;	};

	/*********************** Wall processing routines ***********************/

	};	

#endif