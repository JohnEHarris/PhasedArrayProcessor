/** Copyright (C) 2008 Tuboscope, a National Oilwell Varco Company. All rights reserved  **/

/**==========================================================================

                C l a s s    D e f i n i t i o n

===========================================================================*/
/*============================================================================*/
// jeh 9-19-02 for timing test using pentium hardware timer
// stores most recent 16 elapse timer deltas in milliseconds
// jeh 11-13-02 allow stop time to convert raw timers to ms

#include "stdafx.h"
#include "hwtimer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CHwTimer :: CHwTimer()	// constructor
	{
	// Init timing variables for r/t metrics
	QueryPerformanceFrequency((LARGE_INTEGER *)&HiResClksPerMilliSecond);
	if (HiResClksPerMilliSecond)
		m_fScaleToMicroseconds = 1000000.0f/(float)HiResClksPerMilliSecond;
	else
		m_fScaleToMicroseconds = 1.0f;
	// clock = 3,579,545 on Dell Inspiron 7500.... 8/05/2002
	// 2,728,203 2017/01/09
	HiResClksPerMilliSecond = HiResClksPerMilliSecond/1000;	// clks per 1.0 millisecond
	if (HiResClksPerMilliSecond == 0) HiResClksPerMilliSecond = 1;
	m_indx = m_nCalled = m_nMax128 = m_uMaxDeltaT = 0;
//		strcpy(where,"N/A");	// remember, only 16 character string
//		where = _T("N/A");
	int nId = AfxGetThread()->m_nThreadID;
	CString s;
	s.Format(_T("New HwTimer at 0x%08x created by thread ID = 0x%04x\n"), this, nId);
	TRACE(s);
	strcpy(tag, " ");
	}


CHwTimer::~CHwTimer()	// destructor
	{
	int nId = AfxGetThread()->m_nThreadID;
	CString s;
	s.Format(_T("HwTimer destructor run by thread ID = 0x%04x\n"), nId);
	TRACE(s);
	}


void CHwTimer ::	HwStartTime( void)
		{
		m_nCalled++;  
		m_indx = m_nCalled & 0xf;  
		QueryPerformanceCounter((LARGE_INTEGER *)&m_liStartTime[m_indx]);
		}

void CHwTimer ::HwStopTime(void)
		{
		unsigned int dt;
		QueryPerformanceCounter((LARGE_INTEGER *)&m_liStopTime[m_indx]);
		dt = (unsigned int)(m_liStopTime[m_indx] - m_liStartTime[m_indx]);
		if ( m_uMaxDeltaT < dt)		m_uMaxDeltaT = dt;

		m_nElapseMs[m_indx] = 
			(int) (dt/HiResClksPerMilliSecond);
		if (( m_nCalled & 0x7f) == 0) 
			{	m_nMax128 = m_uMaxDeltaT = 0;	}	
		if ( m_nMax128 < m_nElapseMs[m_indx]) m_nMax128 = m_nElapseMs[m_indx];
		}

// Start a uSecond timer. Just stores raw counts into a variable
void CHwTimer::Start(void)
	{
	QueryPerformanceCounter((LARGE_INTEGER *)&m_nStart);
	}

// return the difference between the time now and the time captured by Start converted to uSeconds
// in error if stop - start time > 20 minutes
int CHwTimer::Stop(void)
	{
	UINT uDelta;
	float fDelta;
	QueryPerformanceCounter((LARGE_INTEGER *)&m_nStop);
	uDelta = (UINT) (m_nStop - m_nStart);
	fDelta = (float) uDelta * m_fScaleToMicroseconds;
	return (int) (fDelta + 0.5f);	// round microseconds
	}

void CHwTimer::HwStopTime(enumTimerMode eTimerMode)
		{
		HwStopTime();

		if ( eTimerMode == enumMsTimers)
			{	// convert raw time number to ms
			m_liStopTime[m_indx] /= HiResClksPerMilliSecond;
			m_liStartTime[m_indx] /= HiResClksPerMilliSecond;
			}
		}

// A caller supplied array of int's is filled with the last 16 delta times
void CHwTimer::GetDeltaTimeArray(int *pArray)
	{
	int i;
	for ( i = 0; i < 16; i++) pArray[i] = (int)(m_liStopTime[i] - m_liStartTime[i]);
	}
