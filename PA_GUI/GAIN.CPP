// Gain.cpp : implementation file
/*
copied from acal.cpp.  Takes out the gain calculation functions and
makes them external procedures, NOT in any class.
Acal class members such as GetBandGain become c functions gGetBandGain
in this file.

02/15/00
Revised:	2/16/00 Add sonoscope
			03/26/00 Consider chnl on in computing/setting gains
			05/04/00 Scale cascaded gains by CASCADE_SCALE_FACTOR


*/
//

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

#include "Extern.h"


//  in config.h 
//#define CASCADE_SCALE_FACTOR		300.0f
// original scaling by 100.0f

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define FAKE_DATA		0

// Od is on top or first in displays in this dialog.  therefore,
// od = 0, id = 1.  
#define	ACAL_OD			0
#define	ACAL_ID			1


/***************************************************************/
/*
GetxxxGain commands vary only by frequency.  High pass (OD)
	is a single stage gain.  Low Pass (ID) is cascaded and is the
	product of abs gain and differential gain.

SetxxxGain commands vary only by abs/proportional.  Differential
	and absolute gain operations have different commands.  The 
	user can not set the differential gain directly except with
	pre setgain.  The cal operation takes care of differential
	gain adjustment.

*/
/***************************************************************/

BOOL gIsChnlOn(int mach, int shoe, int chnl)
	{
#if 0
	WORD wMask;
	
	shoe &= 1;
	chnl &= 0xf;	// up to 16 chnls per shoe
	wMask = 1 << chnl;
	
	if ( !mach)
		{	// Amalog
		return (ConfigRec.AmaRec.ChnlOn[shoe] & wMask);
		}
	else
		{	// Sonoscope
		shoe = 0;	// special for tamsa
		return (ConfigRec.SonoRec.ChnlOn[shoe] & wMask);
		}
#endif
	return TRUE;
	}

int gGetBandGain(int mach, int band, int mode)
	{
#if 0
	// 1 = lp, 0 = hp
	// 0 = abs, 1 = prop
	// return gain value  0-255
	int ic, is, nResult;
	int nActive[2];
	float fsum[2], fbsum;	// one for each shoe
	float fabs, fdif, fprd, ftmp;


	fbsum = 0.0f;

	if (!mach)
		{	// amalog
		if ( band == ACAL_OD)
			{	// single stage gain only
			for ( is = 0; is < 2; is++)
				{
				fsum[is] = 0.0f;
				nActive[is] = 0;
				for ( ic = 0; ic < 16; ic++)
					{
					if (gIsChnlOn(mach,is,ic))
						{
						fsum[is] += 
							ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].fGain;
						nActive[is]++;
						}
					}

				fbsum += fsum[is];
				if ( nActive[is])ftmp = fsum[is]/(float) nActive[is];
				else ftmp = 0.0f;
				ConfigRec.AmaRec.Band[band].Sh[is].fGain = ftmp;
				ConfigRec.AmaRec.Band[band].Sh[is].Gain = (BYTE) (ftmp + 0.5f);
				}	// is loop
			}
		else
			{	// abs gain * differential
			for ( is = 0; is < 2; is++)
				{
				fsum[is] = 0.0f;
				nActive[is] = 0;
				for ( ic = 0; ic < 16; ic++)
					{
					if (gIsChnlOn(mach,is,ic))
						{
						fabs = ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].fGain;
						fdif = ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].fDifGain;
						fprd = fabs*fdif/CASCADE_SCALE_FACTOR;
						fsum[is] += fprd;
						nActive[is]++;
						}
					}
				fbsum += fsum[is];
				if ( nActive[is]) ftmp = fsum[is]/(float) nActive[is];
				else ftmp = 0.0f;

				ConfigRec.AmaRec.Band[band].Sh[is].fGain = ftmp;
				nResult = (int) (ftmp + 0.5);
				ConfigRec.AmaRec.Band[band].Sh[is].Gain = nResult;
				}	// low pass ... cascaded with differential
			}	// abs gain * differential

		if ( (nActive[0] + nActive[1]) == 0) fbsum = 0.0f;
		else 	fbsum = fbsum /(float) (nActive[0] + nActive[1]);
		ConfigRec.AmaRec.Band[band].fGain = fbsum;
		ConfigRec.AmaRec.Band[band].Gain = nResult = (BYTE) (fbsum + 0.5f);
		return nResult;

		}

	else
		{	// sonoscope
		// no id, NO differential
		is = 0;
		fsum[is] = 0.0f;
		nActive[is] = 0;
		for ( ic = 0; ic < 16; ic++)
			{
			if (gIsChnlOn(mach,is,ic))
				{
				fsum[is] += 
					ConfigRec.SonoRec.Band[band].Sh[is].Ch[ic].fGain;
				nActive[is]++;
				}
			}

		fbsum += fsum[is];
		if ( nActive[is]) ftmp = fsum[is]/(float) nActive[is];
		else ftmp = 0.0f;

		ConfigRec.SonoRec.Band[band].Sh[is].fGain = ftmp;
		ConfigRec.SonoRec.Band[band].Sh[is].Gain = (BYTE) (ftmp + 0.5f);
		if ( nActive[is]) fbsum = fbsum /(float)nActive[is];
		ConfigRec.SonoRec.Band[band].fGain = fbsum;
		ConfigRec.SonoRec.Band[band].Gain = nResult = (BYTE) (fbsum + 0.5f);
		return nResult;
		}
#endif
	return 1;
	
	}

BOOL gSetBandGain(int mach, int band, int mode, int gain)
	{
#if 0
	// 0 = lp, 1 = hp
	// 0 = abs, 1 = prop
	// 0 if fails to set gain, !0 if gain is set

	int ic, is;
	float fgtmp[2][16];		// attempt to set gain on temp array 1st
	float fold, fratio;		// starting shoe gain, new/old ratio
	float fchk;
	BYTE undo[16];
	float fundo[16];

	if (!mach)
		{	// amalog
		if ( mode == ABS_GAIN)
			{
			for ( ic = 0; ic < 16; ic++)
				{
				undo[ic] = ConfigRec.AmaRec.Band[band].Sh[0].Ch[ic].Gain;
				fundo[ic] = ConfigRec.AmaRec.Band[band].Sh[0].Ch[ic].fGain;
				}
			if (!gSetShoeGain(mach, band, 0, mode, gain) ) return 0;
			if (!gSetShoeGain(mach, band, 1, mode, gain) )
				{
				// Shoe 1 made it, shoe 2 did not, restore 1
				for ( ic = 0; ic < 16; ic++)
					{
					ConfigRec.AmaRec.Band[band].Sh[0].Ch[ic].Gain = undo[ic];
					ConfigRec.AmaRec.Band[band].Sh[0].Ch[ic].fGain = fundo[ic];
					}
				return 0;
				}
			}
		else
			{	// proportional
			gGetBandGain(mach,band, mode);	// make sure we have latest info to start
			fold = ConfigRec.AmaRec.Band[band].fGain;
			if ( fold < 2.0f) fold = 2.0f;
			fratio = ((float) gain ) / fold;
			// copy existing gains to fgtmp
			for ( is = 0; is < 2; is++)
				for ( ic = 0; ic < 16; ic++)
					{
					fgtmp[is][ic] = 
						fratio * ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].fGain;
					if ( fgtmp[is][ic] < 2.0f ) return 0;	// nope, can't do this
					if ( fgtmp[is][ic] > 254.5f) return 0;
					}
				// Since we are here, the new gain may fit.  If differential, need to see
				// if total overall gain <= 255.  oNLY iD IS DIFFERENTIAL.

				if ( band == ACAL_ID) 	for (is = 0; is < 2; is++)
					for ( ic = 0; ic < 16; ic++)
						{
						fchk = 
							fgtmp[is][ic] * ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].fDifGain / CASCADE_SCALE_FACTOR;
						if ( fchk < 2.0f) return 0;	//failed
						if ( fchk > 254.5f) return 0;
						}

			// Success
			for ( is = 0; is < 2; is++)
				for ( ic = 0; ic < 16; ic++)
					{
					ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].fGain = fgtmp[is][ic];
					ConfigRec.AmaRec.Band[band].Sh[is].Ch[ic].Gain = 
						(BYTE) (fgtmp[is][ic] + 0.5f);
					}

			
			}	// proportional
		}
	else
		{	// sonoscope
		if ( mode == ABS_GAIN)
			{
			gSetShoeGain(mach, band, 0, mode, gain);
//			gSetShoeGain(mach, band, 1, mode, gain);
			}
		else
			{	// proportional
			is = 0;	// no shoe for sonoscope
			gGetBandGain(mach,band, mode);	// make sure we have latest info to start
			fold = ConfigRec.SonoRec.Band[band].Sh[is].fGain;
			if ( fold < 2.0f) fold = 2.0f;
			fratio = ((float) gain ) / fold;
			// copy existing gains to fgtmp
			for ( ic = 0; ic < 16; ic++)
				{
				fgtmp[is][ic] = 
					fratio * ConfigRec.SonoRec.Band[band].Sh[is].Ch[ic].fGain;
				if ( fgtmp[is][ic] < 2.0f ) return 0;	// nope, can't do this
				if ( fgtmp[is][ic] > 254.5f) return 0;
				}
			// Since we are here, the new gain may fit.  If differential, need to see
			// if total overall gain <= 255.  oNLY iD IS DIFFERENTIAL.


			// Success
			for ( ic = 0; ic < 16; ic++)
				{
				ConfigRec.SonoRec.Band[band].Sh[is].Ch[ic].fGain = fgtmp[is][ic];
				ConfigRec.SonoRec.Band[band].Sh[is].Ch[ic].Gain = 
					(BYTE) (fgtmp[is][ic] + 0.5f);
				}

			}	// proportional
		}	// sonoscope

#endif
	return 1;

	}

int gGetShoeGain(int mach, int band, int shoe, int mode)
	{
#if 0
	// 0 = sh1, 1 = sh2
	// 0 = abs, 1 = prop
	// return gain value  0-255
	int ic, nresult = 0;
	int nActive = 0;
	float fsum = 0.0f;
	float fabs, fdif, fprd;


	if (!mach)
		{	// amalog
		if ( band == ACAL_OD)
			{	// NOT CASCADED
			for ( ic = 0; ic < 16; ic++)
				{
				if (gIsChnlOn(mach,shoe,ic))
					{	// active channel
					fsum += ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ic].fGain;
					nActive++;
					}
				}
			}
		else
			{	// low pass ... cascaded with differential
			for ( ic = 0; ic < 16; ic++)
				{
				if (gIsChnlOn(mach,shoe,ic))
					{	// active channel
					fabs = ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ic].fGain;
					fdif = ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ic].fDifGain;
					fprd = fabs*fdif/CASCADE_SCALE_FACTOR;
					fsum += fprd;
					nActive++;
					}
				}
			}	// low pass ... cascaded with differential

		if (nActive)
			{	// at least one chnl turned on
			ConfigRec.AmaRec.Band[band].Sh[shoe].fGain = fsum/(float) nActive;
			nresult = (int) ((fsum / (float) nActive )+ 0.5);
			ConfigRec.AmaRec.Band[band].Sh[shoe].Gain = nresult;
			}
		return nresult;
		}

	else
		{	// sonoscope
		for ( ic = 0; ic < 16; ic++)
			{
			if (gIsChnlOn(mach,shoe,ic))
				{	// active channel
				fsum += ConfigRec.SonoRec.Band[band].Sh[0].Ch[ic].fGain;
				nActive++;
				}
			}

		if (nActive)
			{	// at least one chnl turned on
			ConfigRec.SonoRec.Band[band].Sh[0].fGain = fsum/(float) nActive;
			nresult = (int) ((fsum / (float) nActive )+ 0.5);
			ConfigRec.SonoRec.Band[band].Sh[0].Gain = nresult;
			}
		return nresult;
		}
#endif
	return 1;

	}

BOOL gSetShoeGain(int mach, int band, int shoe, int mode, int gain)
	{
#if 0
	// 0 = sh1, 1 = sh2
	// 0 = abs, 1 = prop
	// 0 if fails to set gain, !0 if gain is set

	int ic;
	float fgtmp[16];		// attempt to set gain on temp array 1st
	float fold, fratio;		// starting shoe gain, new/old ratio
	float fchk;
	BYTE undo[16];
	float fundo[16];

	if ( gain > 255) return 0;	// max gain is 255

	if (!mach)
		{	// amalog
		if ( mode == ABS_GAIN)
			{
			for ( ic = 0; ic < 16; ic++)
				{
				undo[ic] = ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ic].Gain;
				fundo[ic] = ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ic].fGain;
				}

			fchk = (float) gain;
			for ( ic = 0; ic < 16; ic++)
				{
				if (gIsChnlOn(mach,shoe,ic))
					{
					if (!gSetChnlGain(mach, band, shoe, ic, gain) )
						{	// cant set gains, undo the entire operation
						for ( ic = 0; ic < 16; ic++)
							{
							ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ic].Gain = undo[ic];
							ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ic].fGain = fundo[ic];
							}
						return 0;	//failed

						}
					}
				}
			ConfigRec.AmaRec.Band[band].Sh[shoe].Gain = (BYTE) gain;
			ConfigRec.AmaRec.Band[band].Sh[shoe].fGain = fchk;


			}
		else
			{	// proportional
			fold = ConfigRec.AmaRec.Band[band].Sh[shoe].fGain;
			if ( fold < 2.0f) fold = 2.0f;
			fratio = ((float) gain ) / fold;
			// copy existing gains to fgtmp
			for ( ic = 0; ic < 16; ic++)
				{
				if (gIsChnlOn(mach,shoe,ic))
					{	// active channel
					fgtmp[ic] = fratio * ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ic].fGain;
					if ( fgtmp[ic] < 2.0f ) return 0;	// nope, can't do this
					if ( fgtmp[ic] > 254.5f) return 0;
					}
				}
			// Since we are here, the new gain may fit.  If differential, need to see
			// if total overall gain <= 255.  oNLY iD IS DIFFERENTIAL.

			if ( band == ACAL_ID) 	for ( ic = 0; ic < 16; ic++)
				{
				if (gIsChnlOn(mach,shoe,ic))
					{	// active channel
					fchk = fgtmp[ic] * ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ic].fDifGain / CASCADE_SCALE_FACTOR;
					if ( fchk < 2.0f) return 0;	//failed
					if ( fchk > 254.5f) return 0;
					}
				}

			// Success
			for ( ic = 0; ic < 16; ic++)
				{
				if (gIsChnlOn(mach,shoe,ic))
					{	// active channel
					ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ic].fGain = fgtmp[ic];
					ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ic].Gain = (BYTE) (fgtmp[ic] + 0.5f);
					}
				}
//			GetShoeGain(band,shoe,mode);
			return 1;
			}

		}

	else
		{	// sonoscope
		if ( mode == ABS_GAIN)
			{
			fchk = (float) gain;
			for ( ic = 0; ic < 16; ic++)
				{
				if (gIsChnlOn(mach,shoe,ic))
					{	// active channel
					ConfigRec.SonoRec.Band[band].Sh[0].Ch[ic].Gain =
						(BYTE) gain;
					ConfigRec.SonoRec.Band[band].Sh[0].Ch[ic].fGain = fchk;
					}
				}
			ConfigRec.SonoRec.Band[band].Sh[0].Gain = (BYTE) gain;
			ConfigRec.SonoRec.Band[band].Sh[0].fGain = fchk;


			}
		else
			{	// proportional
			fold = ConfigRec.SonoRec.Band[band].Sh[0].fGain;
			if ( fold < 2.0f) fold = 2.0f;
			fratio = ((float) gain ) / fold;
			// copy existing gains to fgtmp
			for ( ic = 0; ic < 16; ic++)
				{
				if (gIsChnlOn(mach,shoe,ic))
					{	// active channel
					fgtmp[ic] = fratio * ConfigRec.SonoRec.Band[band].Sh[0].Ch[ic].fGain;
					if ( fgtmp[ic] < 2.0f ) return 0;	// nope, can't do this
					if ( fgtmp[ic] > 254.5f) return 0;
					}
				}
			// Since we are here, the new gain may fit.  If differential, need to see
			// if total overall gain <= 255.  oNLY iD IS DIFFERENTIAL.


			// Success
			for ( ic = 0; ic < 16; ic++)
				{
				if (gIsChnlOn(mach,shoe,ic))
					{	// active channel
					ConfigRec.SonoRec.Band[band].Sh[0].Ch[ic].fGain = fgtmp[ic];
					ConfigRec.SonoRec.Band[band].Sh[0].Ch[ic].Gain = (BYTE) (fgtmp[ic] + 0.5f);
					}
				}
			return 1;
			}	// proportional

		}
#endif
	return 1;

	}


BOOL gSetChnlGain(int mach, int band, int shoe, int chnl, int gain)
	{
#if 0
	// Set is always absolute mode
	AMA_CHNL  *pAchId;
	float fprod;

	if (!gIsChnlOn(mach,shoe,chnl))	return FALSE;
	
	band &= 1;
	shoe &= 1;
	chnl &= 0xf;


	if ( !mach)
		{
		if ( band == ACAL_OD)
			{
			ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[chnl].Gain = (BYTE) gain;
			ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[chnl].fGain = (float) gain;
			}
		else	// id gain which is product of abs + dif gains
			{
			pAchId = &ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[chnl];
			fprod = ((float) gain) * CASCADE_SCALE_FACTOR/ pAchId->fDifGain;
			if ( fprod < 255.0f)
				{
				pAchId->fGain = fprod;
				pAchId->Gain = (BYTE) (pAchId->fGain + 0.5f);
				}
			else return 0;
			}
		// Send msg to instrument
		}

	else
		{	// sono
		ConfigRec.SonoRec.Band[band].Sh[0].Ch[chnl].Gain = (BYTE) gain;
		ConfigRec.SonoRec.Band[band].Sh[0].Ch[chnl].fGain = (float) gain;
		}	// sono
#endif
	return 1;
	}

int gGetChnlGain(int mach, int band, int shoe, int ch)
	{
#if 0
	float fg, fdg, fprod;

	band &= 1;
	shoe &= 1;
	ch &= 0xf;	// up to 16 chnls per shoe
	if ( !mach)
		{
		if (band == ACAL_OD)
			{	// hp
			return (int)ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ch].Gain;
			}
		else
			{	// lp .. differential
			fg = ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ch].fGain;
			fdg = ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[ch].fDifGain;
			fprod = fg*fdg / CASCADE_SCALE_FACTOR;	// dif gain as per cent
			return (int) (fprod + 0.5f);
			}

		}

	else
		{	// sono
		return (int)ConfigRec.SonoRec.Band[band].Sh[0].Ch[ch].Gain;
		}	// sono
#endif
	return 1;
	}

int gGetChnlAbsGain(int mach, int band, int shoe, int chnl)
	{
#if 0
	band &= 1;
	shoe &= 1;
	chnl &= 0xf;	// up to 16 chnls per shoe

	if ( !mach)
		{
		return (int) ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[chnl].Gain;
		}
	else
		{
		return (int) ConfigRec.SonoRec.Band[band].Sh[0].Ch[chnl].Gain;
		}
#endif
	return 1;
	}

BOOL gSetDifChnlGain(int mach, int band, int shoe, int chnl, int gain)
	{
#if 0

	if (!gIsChnlOn(mach,shoe,chnl)) return FALSE;	// no action

	band &= 1;
	shoe &= 1;
	chnl &= 0xf;	// up to 16 chnls per shoe
	if ( !mach)
		{
		ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[chnl].DifGain = 
			(BYTE) gain;
		ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[chnl].fDifGain = 
			(float) gain;
		}
	else
		{
//		ConfigRec.SonoRec.Band[band].Sh[]Ch[chnl].DifGain = (BYTE) gain;
//		ConfigRec.SonoRec.Band[band].Sh[]Ch[chnl].fDifGain = (float) gain;
		}
#endif
	return TRUE;
	}

int gGetDifChnlGain(int mach, int band, int shoe, int chnl)
	{
#if 0

//	if (!gIsChnlOn(mach,shoe,chnl)) return 0;	// no action
	
	band &= 1;
	shoe &= 1;
	chnl &= 0xf;	// up to 16 chnls per shoe
	if ( !mach)
		{
		return (int) ConfigRec.AmaRec.Band[band].Sh[shoe].Ch[chnl].DifGain;
		}
	else
		{
		return 0;	// (int) ConfigRec.SonoRec.Band[band].Sh[]Ch[chnl].Gain;
		}
#endif
		return 0;	// (int) ConfigRec.SonoRec.Band[band].Sh[]Ch[chnl].Gain;
	}
