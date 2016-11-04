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

Revision:	2016-07-11 a dummy class to satisfy ST_SERVERS_CLIENT_CONNECTION pointers to CvChannel[]

*/
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef V_CHANNEL_H
#define V_CHANNEL_H

//#include "ServiceApp.h"

class CvChannel
	{
public:
	CvChannel(int nInst, int nChnl);
	virtual ~CvChannel();

	};	

#endif