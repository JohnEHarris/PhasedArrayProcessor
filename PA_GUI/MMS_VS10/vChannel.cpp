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


CvChannel::CvChannel(int nInst, int nChnl)
	{
	// id/od, Nc, Thold, bMod

	};

CvChannel::~CvChannel()
	{
	};

