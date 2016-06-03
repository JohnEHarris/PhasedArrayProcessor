/*
A class to simulate hardware for processing UT channels
There shoud be a class instance for every channel in a UT instrument
Author:		jeh
Date:		01-Jun-2016
Revised:	modeled somewhat like RunningAverage. The two may be merged in the future. Simulates
			FIFO's holding wall and flaw values. Used to compute Nc and Nx values

*/

#include "stdafx.h"
#include "ServiceApp.h"


#include "stdafx.h"
#include "ServiceApp.h"
//#include "math.h"

//#include <windows.h>
//#include <process.h>
//#include <stddef.h>
//#include <stdlib.h>
//#include <conio.h>
//#include <string.h>
//#include <stdio.h>
//#include "vChannel.h"
#include "InstMsgProcess.h"

CvChannel::CvChannel(int nInst, int nChnl)
	{
	FifoInit(0,1,0,1);	// id
	FifoInit(1,1,0,1);	// od

	// Wall processing routines
	WFifoInit(1,27,1377,8);
	};

CvChannel::~CvChannel()
	{
	};

/*********************** Flaw processing routines ***********************/
// Set all parameters necessary to enforce Nc rules
void CvChannel::FifoInit(BYTE bIdOd, BYTE bNc, BYTE bThld, BYTE bMod)
	{
	Nc_FIFO *pFifo;
	bIdOd &= 1;	// limit range to 0-1
	pFifo = &NcFifo[bIdOd];
	memset( (void *) pFifo,0, sizeof (Nc_FIFO));
	pFifo->bNc = bNc;
	if ( bThld < 3) bThld = 2;		// 2 % is minimum thold allowed.
	pFifo->bThold = bThld;
	pFifo->bMod = bMod;
	};

// An amplitued is input and an Nc qualified reading is returned.
// bIdOd selects which FIFO id=0, od=1
BYTE CvChannel::InputFifo(BYTE bIdOd,BYTE bAmp)
	{
	int i;
	Nc_FIFO *pFifo;
	bIdOd &= 1;	// limit range to 0-1
	pFifo = &NcFifo[bIdOd];

	i = pFifo->bInput;		// slot position in the fifo
	pFifo->bCell[i] = bAmp;	// replace oldest element
	pFifo->bInput++;		// advance to next oldest position
	if (pFifo->bInput >= pFifo->bMod)	pFifo->bInput = 0;	// fifo is only bMod deep

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
	else										pFifo->bMax = pFifo->bMaxTemp;

	return pFifo->bMax;
	};

void CvChannel::FifoClear(BYTE bIdOd)	// zero fifo entries/cells, keep other parameters
	{
	Nc_FIFO *pFifo;
	bIdOd &= 1;	// limit range to 0-1
	pFifo = &NcFifo[bIdOd];
	memset( (void*) &pFifo->bCell[0], 0, sizeof (pFifo->bCell));
	}

// bMod = bNc when SetNc called. No nc out of m ascans
void CvChannel::SetNc(BYTE bIdOd, BYTE bNc)
	{
	Nc_FIFO *pFifo;
	bIdOd &= 1;	// limit range to 0-1
	pFifo = &NcFifo[bIdOd];
	pFifo->bNc = pFifo->bMod = bNc;
	}
/*********************** Flaw processing routines ***********************/


/*********************** Wall processing routines ***********************/

void CvChannel::WFifoInit(BYTE bNx, WORD wMax, WORD wMin, WORD wDropOut)
	{
	memset((void *) &NxFifo,0, sizeof(NxFifo));
	NxFifo.bNx = bNx;
	NxFifo.wWallMax = wMax;
	NxFifo.wWallMin = wMin;
	}

WORD CvChannel::InputWFifo(WORD wWall)
	{
	int i;
	WORD wOldWall;
	Nx_FIFO *pFifo = &NxFifo;

	if ( (wWall < pFifo->wWallMin) || (wWall > pFifo->wWallMax))
		{
		pFifo->wBadWall++;
		pFifo->wGoodWall = 0;
		return pFifo->wBadWall;
		}

	pFifo->wGoodWall++;
	i = pFifo->bInput++;			// slot position in the fifo and increment to next
	if ( pFifo->bInput >= pFifo->bNx)	
		 pFifo->bInput = 0;
	wOldWall = pFifo->wCell[i];		// get oldest wall reading
	pFifo->wCell[i] = wWall;		// replace oldest element with this one
	pFifo->uSum += (wWall - wOldWall);	// change in sum is new - old
	return 0;
	}

