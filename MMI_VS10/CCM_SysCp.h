/*******
Author: jeh
Date:	10-Jul-2012
Purpose:	Derive a specific class from the general base class ClientConnectionManagement
			Since all mill console programs have to talk to the SysCp, this is a good place
			to begin with children of the base CCM class

*******/

#ifndef CCM_SYSCP_H_2012
#define CCM_SYSCP_H_2012
#include "ClientConnectionManagement.h"

#define DESCRIPTION _T("Stand Alone")
#define	LOCATION	_T("Houston, TX")
#define LINE		_T("Engineering")


// Only the base class cpp file should define I_AM_CCM
#ifdef I_AM_CCM
#define PubExt1	
#else
#define PubExt1	extern
#endif

PubExt1 CClientConnectionManagement *pCCM[MAX_CLIENTS];

class CCCM_SysCp: public CClientConnectionManagement
	{
public:
	CCCM_SysCp(int nMyConnection, USHORT wOriginator);		// the only constructor
	virtual ~CCCM_SysCp(void);
	void ProcessReceivedMessage(void);
	void UniqueProc(void);
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
	};

// End of Class declaration
// global variables below

#ifdef I_AM_CCM_SYSCP
CCCM_SysCp *pCCM_SysCp;		// global ptr to the single instance of this class
#else
extern CCCM_SysCp *pCCM_SysCp;
#endif

#endif
