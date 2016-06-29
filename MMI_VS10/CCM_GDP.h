/*******
Author: jeh
Date:	26-Apr-2013
Purpose:	Derive a specific class from the general base class ClientConnectionManagement
			Since all mill console programs have to talk to the GDP, add a class to do it.
			Copied and modified from CCM_SysCp.h

*******/

#ifndef CCM_GDP_H_2013
#define CCM_GDP_H_2013
#include "ClientConnectionManagement.h"

			
// Only the base class cpp file should define I_AM_CCM
#ifdef I_AM_CCM
#define PubExt1	
#else
#define PubExt1	extern
#endif

PubExt1 CClientConnectionManagement *pCCM[MAX_CLIENTS];

class CCCM_GDP: public CClientConnectionManagement
	{
public:
	CCCM_GDP(int nMyConnection, USHORT wOriginator);		// the only constructor
	virtual ~CCCM_GDP(void);
#if 0
	void ProcessReceivedMessage(void);
	void DoCommandMessage(TCPDUMMY * tcpDummy);				// from Randy
	void DoPipeNumberMessage(TCPDUMMY * tcpDummy);
	void DoGdpParameterMessage(TCPDUMMY * tcpDummy);
	void DoAlarmMessage(TCPDUMMY * tcpDummy);
	void DoReportGeneratorMessage(TCPDUMMY * tcpDummy);
	void DoAckNakMessage(TCPDUMMY * tcpDummy);

	void MakeEquipmentDetail(CString EquipmentNumber,	CString FamilyName,	CString EquipmentDescription,
							 CString EquipmentLocation,	CString EquipmentInspectionLine);
	void MakeVersionDetail(void);
	void MakeConnectionDetail(CString ServerIP, UINT ServerPort, CString ClientIP, UINT ClientPort);

	void SetClientIp(CString s)	{_tcscpy_s(ClientIdentity.ClientIP, MAX_CID_IP_CHAR_LENGTH, s);}
	void SetServerIp(CString s)	{_tcscpy_s(ClientIdentity.ServerIP, MAX_CID_IP_CHAR_LENGTH, s);}

	CLIENT_IDENTITY_DETAIL ClientIdentity;
	STATUS_DETAIL m_StatusDetail;
	PIPE_NUMBER_DETAIL m_PipeNumberDetail;
	GDP_PARAMETER_DETAIL m_GDPParameterDetail;
	ALARM_DETAIL m_AlarmDetail;
#endif

	void UniqueProc(void);
	};

// End of Class declaration
// global variables below

#ifdef I_AM_CCM_GDP
CCCM_GDP *pCCM_GDP;		// global ptr to the single instance of this class
#else
extern CCCM_GDP *pCCM_GDP;
#endif


#endif

