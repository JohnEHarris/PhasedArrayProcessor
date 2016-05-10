/*
A class to perform running average on 16 bit WORDS
There shoud be a class instance for every wall channel in a UT instrument
Author:		jeh
Date:		22-Mar-2012
Revised:

*/

#ifndef RUNNING_AVERAG_H
#define RUNNING_AVERAG_H
#include "../include/cfg100.h"
class CInstMsgProcess;

class CRunningAverage
{
public:
	CRunningAverage(int nNx);
	virtual ~CRunningAverage();
	WORD Input(WORD wCurrentWall);	// input wall reading, return the running average for nNx wall samples
	WORD GetMinWallOfLastN(WORD wCurrentWall); // look at last N wall inputs and return the minimum
	void Reset(void);				// Resets array of recent wall readings
	void SetNx(int nNx)		{ m_nNx = nNx;	}	// wall averaging "filter" length
	int  GetNx(void)		{ return m_nNx;	}
	void SetMinWallSearchRange(int N);
	int  GetMinWallSearchRange(void)	{ return m_nMinWallSearchRange;	}
	// Several class instances of this class can belong to an instrument
	void SetSlaveNumber(int n)		{ m_nMySlave = n;	}
	int GetSlaveNumber(void)		{ return m_nMySlave;}
	// Each running average class is associated with a specific channel
	void SetChannelNumber(int n)	{ m_nMyChnl = n;	}
	int GetChannelNumber(void)		{ return m_nMyChnl;	}
	void SetAscanDeltaT(float t)	{ m_ascan_delta_t = t;		}
	float GetAscanDeltaT(void)		{ return m_ascan_delta_t;	}
	// Another way to compute dropout operation and dropout thereshold
	void SetDropOutThreshold(int nTh)	{ m_nDropPktThold = nTh;	}
	int GetDropOutThreshold(void)		{ return m_nDropPktThold;	}
	void SetLowWallLimit(WORD wLowLimit){ m_wLowWallLimit = wLowLimit;}
	WORD GetLowWallLimit(void)			{ return m_wLowWallLimit;	}
	void DefaultLowWallLimit(void)		{ m_wLowWallLimit = WALL_LOW_LIMIT;	}

	// class data variables

	// a buffer large enough to hold wall reading for the largest Nx allowed
	WORD m_wRunningAvgFIFO[NX_TOTAL];
	// a buffer large enough to hold WALL_BUF_SIZE readings. Search the buffer for smallest reading
	WORD m_wMinWallFIFO[WALL_BUF_SIZE];	// circular buffer of recent wall readings
	// works like m_wRunningAvgFIFO above only with different wrap around point
	int m_nNx;			// what is the filter length (Nx)
	int m_nNew;			// where the newest wall input is inserted in running avg filter
	int m_nNewOfLastN;	// where the newest wall input is inserted for searching for Min of N
	int m_nMinWallSearchRange;	// how many of last inputs to search for minimum wall reading
	int m_nMySlave;		// which slave instrument is using this class
	int m_nMyChnl;		// which channel in the slave instrument is using this class
	WORD m_wLastGoodAvg;	// last computed average within allowable wall limits
	float m_invalid_wall_t;	// count amount of time when no valid wall reading obtained
	float m_ascan_delta_t;	// time interval over which last wall reading was gathered, ie, 1/prf
	// another way to compute drop out is to determine the packet drop count as
	// drop out time in seconds/time of 1 packet = dt/(1/prf) = dt*prf
	// if drop out time is .3 sec and prf = 10k, the dt*prf = .3*10000 = 3000
	int m_nDropPktThold;	// when we get this many consecutive drop outs we quit holding wall reading
	int m_nDropPktQty;		// how many dropped packet in a row
	WORD m_wLowWallLimit;

	CInstMsgProcess *m_pMyProcess;	// who created me?
	CInstMsgProcess * GetMyInstMsgProcess(void)		{ return m_pMyProcess;	}
	void SetMyInstMsgProcess(CInstMsgProcess * pProcess)	{ m_pMyProcess = pProcess;	}
	};

#endif
