// Set the global inspection state - packet info type,
// Run mode, either motion or time
// InstState.h
// jeh, 24-Apr-2012
//
#include "PA2WinDlg.h"

#ifndef INSP_STATE_H
#define INSP_STATE_H


// This class is instantiated only once, on the stack of the ServiceApp class which
// is the running program. Hence this class is persistent as are its variables.
//
class CInspState
	{
public:

	CInspState(void);
	virtual ~CInspState();

	int GetInspectMode(void)		{	return m_nInspectMode;	}
	void SetInspectMode(int nMode)	{	m_nInspectMode = nMode;	}
	int GetMotionMode(void)			{	return m_nMotionTime;	}
	void SetMotionMode(int nMode)	{	m_nMotionTime = nMode;	}
#if 0
	void SetChannelConfig(CHANNEL_CONFIG2 *pChnlCfg, int nSlave);
	void GetChannelConfig(CHANNEL_CONFIG2 *pChnlCfg);	// , int nSlave);

	void SetSiteDefaults(SITE_SPECIFIC_DEFAULTS *pSiteDef);
	void GetSiteDefaults(SITE_SPECIFIC_DEFAULTS *pSiteDef);
#endif

private:
	int m_nInspectMode;
	int m_nMotionTime;
	//CHANNEL_CONFIG2 m_ChannelCfg;		// configuration for all slave instruments for all channels
	// m_ChannelCfg could have been implemented as a global c structure.
	//SITE_SPECIFIC_DEFAULTS m_SiteDefaults;	// normal setting for this inspection unit
	};


#endif