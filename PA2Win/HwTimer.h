/** Copyright (C) 2008 Tuboscope, a National Oilwell Varco Company. All rights reserved  **/
#ifndef CHWTIMER_H
#define CHWTIMER_H
enum enumTimerMode
	{
	enumRawTimers,
	enumMsTimers
	};

class CHwTimer //: public CObject for truwall project, not an object
{
public:
	__int64 m_liStopTime[16], m_nStop;
	__int64 m_liStartTime[16], m_nStart;
	__int64 HiResClksPerMilliSecond;
	int m_indx;	// index into elpase time array
	int m_nElapseMs[16];	// elapse ms
	int m_nCalled;			// number of times called
	int m_nMax128;		// max time in 128 calls
//	char where[16];		// where in code is this timing done... debugging
//	CString where;		// where in code is this timing done... debugging
	// 10-Jun-08 jeh
	char tag[128];		// tell who created and who destroyed.
	unsigned int m_uMaxDeltaT;
	float m_fScaleToMicroseconds;

	CHwTimer();		// constructor
	~CHwTimer();	// destructor
	void HwStartTime( void);
	void HwStopTime(void);
	void HwStopTime(enumTimerMode);
	int HwGetTimesCalled(void)			{ return m_nCalled;		};
	unsigned int GetMaxDeltaT(void)		{ return m_uMaxDeltaT;	}
	void GetDeltaTimeArray(int *pArray);	// return the elapse time for last 16 intervals in caller supplied array
	void CHwTimer::Start(void);
	int CHwTimer::Stop(void);

};
// jeh

#endif
