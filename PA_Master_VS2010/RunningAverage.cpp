/*
A class to perform running average on 16 bit WORDS
There shoud be a class instance for every wall channel in a UT instrument
Author:		jeh
Date:		22-Mar-2012
Revised:

*/

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
#include "InstMsgProcess.h"
#include "RunningAverage.h"

// Only constructor
// Must specify the length of the running average at class creation time
// Has one fifo for computing running average, another fifo for finding min wall out of
// last m_nMinWallSearchRange number of wall readings.
CRunningAverage::CRunningAverage(int nNx)
	{
	m_nNx = nNx;
	if (m_nNx > NX_TOTAL)	m_nNx = NX_TOTAL;
	if ( m_nNx < 1)			m_nNx = 1; 
	m_nNew = 0;				// insertion point in FIFO
	m_nNewOfLastN = 0;		// insertion point for searching for min wall 
							// out of last N readings
	m_nMinWallSearchRange = nNx;	// default to same size as filter
	m_wLowWallLimit = WALL_LOW_LIMIT;
	m_nDropPktQty = 0;
	memset (&m_wRunningAvgFIFO, 0, 2*NX_TOTAL);		// zero AVERAGING FIFO
	memset (&m_wMinWallFIFO, 0, 2*WALL_BUF_SIZE);	// zero search FIFO
	m_invalid_wall_t = m_ascan_delta_t = 0.0f;
	m_nMySlave = m_nMyChnl = 0;		// defaults to first slave instrument and first channel
	}

CRunningAverage::~CRunningAverage()
	{
	m_nNx = 0;	//just do something for now
	}

// Input a wall reading - the current wall
// Return the integer running average with rounding
// If a bad wall reading, hold the last good wall until some specified 
// wall drop out period has elapsed. Then drop the wall to 0.
// The drop out interval is express in seconds and is a float type.
// Drop out interval will be changed to a corresponding number of Ascans
// which match the drop out time in seconds.
WORD CRunningAverage::Input(WORD wCurrentWall)
	{
	int i, sum;
	if (wCurrentWall <= m_wLowWallLimit)
		{
		//m_invalid_wall_t += ascan_delta_t;
		// if (m_invalid_wall_t > g_WallDropTime)	m_wLastGoodAvg = 0;
		m_nDropPktQty++;
		if (m_nDropPktQty > m_nDropPktThold)		m_wLastGoodAvg = 0;
		return m_wLastGoodAvg;
		}
	if (wCurrentWall >= WALL_HIGH_LIMIT)
		{
		//m_invalid_wall_t += ascan_delta_t;
		// if (m_invalid_wall_t > g_WallDropTime)	m_wLastGoodAvg = 0;
		m_nDropPktQty++;	// count the number of wall drop outs
		if (m_nDropPktQty > m_nDropPktThold)		m_wLastGoodAvg = 0;
		return m_wLastGoodAvg;
		}

	// If we are here then the wall reading is within limits and this is
	// assumed to be a good wall reading
	// m_invalid_wall_t = 0.0f;
	m_nDropPktQty = 0;
	m_wRunningAvgFIFO[m_nNew++] = wCurrentWall;		// overwrite old sample
	if (m_nNew >= m_nNx)	m_nNew = 0;		// wrap when Nx length reached
	sum = m_wRunningAvgFIFO[0];
	for ( i = 1; i < m_nNx; i++)	sum += m_wRunningAvgFIFO[i];
	sum += (m_nNx/2);	//rounding
	m_wLastGoodAvg = sum/m_nNx;

	if (m_wLastGoodAvg < 0)	Reset();	// error condition
	if (m_wLastGoodAvg > WALL_HIGH_LIMIT)	Reset();	// error condition
	return (m_wLastGoodAvg);
	}

void CRunningAverage::Reset(void)
	{
	m_nNew = m_wLastGoodAvg = 0;
	memset (&m_wRunningAvgFIFO, 0, 2*NX_TOTAL);	// zero all word readings
	}

// Set the number of previous wall reading to examine to find the minimum
void CRunningAverage::SetMinWallSearchRange(int N)	
	{ 
	m_nMinWallSearchRange = N;
	if (0 == m_nMinWallSearchRange)	m_nMinWallSearchRange = 1;
	if ( m_nMinWallSearchRange > WALL_BUF_SIZE) m_nMinWallSearchRange = WALL_BUF_SIZE;
	}

// After inputting the current wall reading, find the minimum wall from 
// the last "N" inputs. "N" is set with SetMinWallSearchRange(xx) above
WORD CRunningAverage::GetMinWallOfLastN(WORD wCurrentWall)
	{
	WORD wReturn;
	int i;
	// overwrite the oldest element in the FIFO
	m_wMinWallFIFO[m_nNewOfLastN++] = wReturn = wCurrentWall;
	if (m_nNewOfLastN >= m_nMinWallSearchRange) m_nNewOfLastN = 0;
	// before scan, return 0 if last input is 0
	if (0 == wReturn)	return wReturn;

	for ( i = 0; i < m_nMinWallSearchRange; i++)
		{
		if (wReturn > m_wMinWallFIFO[i])	wReturn = m_wMinWallFIFO[i];
		}
	return wReturn;
	}
