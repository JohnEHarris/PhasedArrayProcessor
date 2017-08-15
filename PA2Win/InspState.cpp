// Hold the state variables determining motion/time and inpsect/cal/nop states
// Also holds a permananent copy of the channel configuration data.
// InspState.cpp
// jeh 24-Apr-12

#include "stdafx.h"
#include "PA2WinDlg.h"

// constructor
CInspState::CInspState(void)
	{
#if 0
	m_nInspectMode = NOP_MODE;
	m_nMotionTime  = TIME_BASED;
	memset((void *) &m_ChannelCfg, 0, sizeof (CHANNEL_CONFIG2));
	memset ( (void *) &m_SiteDefaults, 0, sizeof(SITE_SPECIFIC_DEFAULTS) );
#endif
	}

// destructor
CInspState::~CInspState()
	{
	//m_nInspectMode = NOP_MODE;	//just do something for now
	}

// State information about channel configuration loaded by user with config file
//
// only sets the values in one instrument 'nSlave' in system state variable
// -->  m_ChannelCfg <--
// pChnlCfg is a ptr to a local instance of CHANNEL_CONFIG2 which is filled
// by the local caller. Just the 'nSlave' portion of the pChnlCfg is copied into
// the "static" class member m_ChannelCfg
//
#if 0
void CInspState::SetChannelConfig(CHANNEL_CONFIG2 *pChnlCfg, int nSlave)
	{
	int i;
	for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
		{
		m_ChannelCfg.Ch[nSlave][i].Type = pChnlCfg->Ch[nSlave][i].Type;
		m_ChannelCfg.Ch[nSlave][i].cXOffset = pChnlCfg->Ch[nSlave][i].cXOffset;
		m_ChannelCfg.Ch[nSlave][i].cWOffset = pChnlCfg->Ch[nSlave][i].cWOffset;
		}
	m_ChannelCfg.cClockOffset = pChnlCfg->cClockOffset;
	}
#endif

// as of 4-24-12 gets the value of all chnls for all slaves (instruments).
// Could return a pointer to just the slave desired with GetChannelConfigPtr
// typically pChnlCfg is a ptr to the callers local copy of the channel configuration
// This call copies the state variable into the local copy
#if 0
void CInspState::GetChannelConfig(CHANNEL_CONFIG2 *pChnlCfg)	//, int nSlave)
	{
	memcpy ( (void *) pChnlCfg, (void *) &m_ChannelCfg, sizeof (CHANNEL_CONFIG2) );
	}

// Copy a structure containing the site defaults into the permanent structure for site defaults
void CInspState::SetSiteDefaults(SITE_SPECIFIC_DEFAULTS *pSiteDef)
	{
	memcpy ( (void *) &m_SiteDefaults, (void *) pSiteDef, sizeof (SITE_SPECIFIC_DEFAULTS) );
	}

// Copy the permanent structure containing the site defaults into a new structure of site defaults
void CInspState::GetSiteDefaults(SITE_SPECIFIC_DEFAULTS *pSiteDef)
	{
	memcpy ( (void *) pSiteDef, (void *) &m_SiteDefaults, sizeof (SITE_SPECIFIC_DEFAULTS) );
	}

#endif

