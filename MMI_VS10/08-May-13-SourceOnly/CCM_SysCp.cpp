
/*******************************************************
Author: jeh
Date:	10-Jul-2012
Purpose:	Derive a specific class from the general base class ClientConnectionManagement
			Since all mill console programs have to talk to the SysCp, this is a good place
			to begin with children of the base CCM class. This class handles tcp/ip messages.

********************************************************/

			
#include "stdafx.h"

#include "Truscan.h"
#include "TscanDlg.h"
//#include "MC_SysCPTestClient.h"
//#include "MC_SysCPTestClientDlg.h"

#include "ClientSocket.h"
#include "ClientConnectionManagement.h"
#include "ClientCommunicationThread.h"
//#include "TCPCommunicationDlg.h"
#include "time.h"

extern THE_APP_CLASS theApp;		

#define I_AM_CCM_SYSCP
#include "CCM_SysCp.h"

// nMyConnection selects which one of MAX_CLIENTS connections we are managing with this instance
// wOriginator selects family type from Constants, eg. TRUSCOPE_COMMAND_PROCESSOR
// This class is a child of the base class CClientConnectionManagement. It was crafted specifically
// for handling communications with the SysCp

CCCM_SysCp::CCCM_SysCp(int nMyConnection, USHORT wOriginator) : CClientConnectionManagement(nMyConnection, wOriginator)
	{
	// call the base constructor explicitly above
	TRACE("CCM_SysCp Constructor called\n");
	CString stmp;
	m_pstCCM->pCCM = this;
	memset(&ClientIdentity, 0, sizeof(ClientIdentity));
	memset(&m_StatusDetail, 0, sizeof(m_StatusDetail));
	memset(&m_AlarmDetail, 0, sizeof(m_AlarmDetail));
	memset(&m_GDPParameterDetail, 0, sizeof(m_GDPParameterDetail));
	memset(&m_PipeNumberDetail, 0, sizeof(m_PipeNumberDetail));
	// Gather up all we know about this client -namely Phased Array Wall Processor- of the SysCp
#ifdef OLD_CODE
	if (m_pstCCM->pClientIdentity)
		{
		m_pstCCM->pClientIdentity->Originator		= wOriginator;
		m_pstCCM->pClientIdentity->Target			= SYSTEM_CONTROL_PROCESSOR;
		m_pstCCM->pClientIdentity->MessageType		= DATA_MESSAGE;
		m_pstCCM->pClientIdentity->MessageContent	= CLIENT_IDENTITY_DETAIL_MESSAGE;
		stmp.LoadString(IDS_FAMILY_NAME_1800);
		MakeEquipmentDetail(_T("1800000"), stmp, DESCRIPTION, LOCATION, LINE);
		MakeVersionDetail();
		}
#else
	ClientIdentity.Originator		= wOriginator;
	ClientIdentity.Target			= SYSTEM_CONTROL_PROCESSOR;
	ClientIdentity.MessageType		= DATA_MESSAGE;
	ClientIdentity.MessageContent	= CLIENT_IDENTITY_DETAIL_MESSAGE;
	stmp.LoadString(IDS_FAMILY_NAME_1800);
	MakeEquipmentDetail(_T("1800000"), stmp, DESCRIPTION, LOCATION, LINE);
	MakeVersionDetail();
#endif
	m_StatusDetail.Originator		= wOriginator;	// this client
	m_StatusDetail.Target			= SYSTEM_CONTROL_PROCESSOR;
	m_StatusDetail.MessageType		= STATUS_MESSAGE;
	m_StatusDetail.MessageContent	= STATUS_DETAIL_MESSAGE;

	m_AlarmDetail.Originator		= wOriginator;	// this client
	m_AlarmDetail.Target			= SYSTEM_CONTROL_PROCESSOR;
	m_AlarmDetail.MessageType		= ALARM_MESSAGE;
	m_AlarmDetail.MessageContent	= ALARM_DETAIL_MESSAGE;
	_tcscpy_s(m_AlarmDetail.EquipmentNumber, MAX_CID_CHAR_LENGTH, ClientIdentity.EquipmentNumber);
	_tcscpy_s(m_AlarmDetail.FamilyName, MAX_CID_CHAR_LENGTH, ClientIdentity.FamilyName);

	m_GDPParameterDetail.Originator	= wOriginator;
	m_GDPParameterDetail.Target		= SYSTEM_CONTROL_PROCESSOR;
	m_GDPParameterDetail.MessageType= GDP_PARAMETER_MESSAGE;
	m_GDPParameterDetail.MessageContent	= GDP_PARAMETER_DETAIL_MESSAGE;

	m_PipeNumberDetail.Originator	= wOriginator;
	m_PipeNumberDetail.Target		= SYSTEM_CONTROL_PROCESSOR;
	m_PipeNumberDetail.MessageType	= PIPE_NUMBER_MESSAGE;
	m_PipeNumberDetail.MessageContent	= PIPE_NUMBER_DETAIL_MESSAGE;

	}

CCCM_SysCp::~CCCM_SysCp(void)
	{
	TRACE("CCM_SysCp Destructor called\n");
	// base destructor called implicity after this destructor runs
	}

// This code is specific to communications with the SysCp

// We finally get around to processing the message.
// Liberally stolen from Randy's example

void CCCM_SysCp::DoCommandMessage(TCPDUMMY * tcpDummy)
	{
	USES_CONVERSION;

	CString stmp0 = _T("");	// temporary string holder
	CString stmp1 = _T(""); // temporary string holder
	CString stmp2 = _T("");	// temporary string holder

	// make access to the main dialog -- this code varies with the name of the main dialog
    MAIN_DLG_NAME * pDlg = NULL;
    pDlg = (MAIN_DLG_NAME *) AfxGetMainWnd();
    ASSERT (pDlg != NULL);


	COMMAND_MSG * CommandMessage = (COMMAND_MSG *) tcpDummy; 
	ASSERT (CommandMessage != NULL);

	// this part parses the command
	// so that some action can be
	// taken by the client.  Clients,
	// of course can ignore commands,
	// they aren't interested in
	switch (CommandMessage->Command)
		{
	case RECONFIGURE: // client should reconfigure itself
		stmp0.LoadString(IDS_RCVD_CMD_RECONFIGURE);
		break;

	case PRINT_CONFIGURATION:	// client should print its configuration
		stmp0.LoadString(IDS_RCVD_CMD_PRINT_CONFIGURATION);
		break;

	case EXPORT_CONFIGURATION: // client should export its configuration
		stmp0.LoadString(IDS_RCVD_CMD_EXPORT_CONFIGURATION);
 		break;

	case START_INSPECTION: // client should start inspection
		stmp0.LoadString(IDS_RCVD_CMD_START_INSPECTION);

		// grab the UNIQUE_ID_DETAIL items of interest
		// an append to debug string

		// the server assigned a threadID to a handler
		// so remember it.  this is how the server
		// keeps track of connections
		m_StatusDetail.ThreadID = CommandMessage->Parameter1;

		// remember who sent this command so we can send our
					// response back to the originator
		m_StatusDetail.Target = (USHORT) CommandMessage->Originator; 

		// indicate run state
		m_StatusDetail.Status = STATION_IS_RUNNING;
		SendPacket((BYTE *) &m_StatusDetail, sizeof(m_StatusDetail), 0);	// don't delete m_StatusDetail
  		break;

	case STOP_INSPECTION: // client should stop inspection
		stmp0.LoadString(IDS_RCVD_CMD_STOP_INSPECTION);

		// the server assigned a threadID to a handler
		// so remember it.  this is how the server
		// keeps track of connections
		m_StatusDetail.ThreadID = CommandMessage->Parameter1;

		// remember who sent this command so we can send our
		// response back to the originator
		m_StatusDetail.Target = (USHORT) CommandMessage->Originator; 

		// indicate stopped state
		m_StatusDetail.Status = STATION_IS_STOPPED;

		// respond with a status detail structure
		SendPacket((BYTE *) &m_StatusDetail, sizeof(m_StatusDetail), 0);	// don't delete m_StatusDetail
  		break;

	case PRINT_LOGFILE:	// client should print its logfile
		stmp0.LoadString(IDS_RCVD_CMD_PRINT_LOGFILE);
 		break;

	case EXPORT_LOGFILE: // client should export its logfile
		stmp0.LoadString(IDS_RCVD_CMD_EXPORT_LOGFILE);
  		break;

	case WHO_GOES_THERE: // client should identify itself to server
		stmp0.LoadString(IDS_RCVD_CMD_WHO_GOES_THERE);

		// the server assigned a threadID to a handler
		// so remember it.  this is how the server
		// keeps track of connections
								 
		m_StatusDetail.ThreadID			= 
		m_AlarmDetail.ThreadID			=
		m_PipeNumberDetail.ThreadID		=
		m_GDPParameterDetail.ThreadID	= ClientIdentity.ThreadID = CommandMessage->Parameter1;
		// remember who sent this command so we can send our
		// response back to the originator .. redundant since already set in constructor - but no harm
		m_StatusDetail.Target			=
		m_AlarmDetail.Target			=
		m_PipeNumberDetail.Target		=
		m_GDPParameterDetail.Target		= ClientIdentity.Target = (USHORT) CommandMessage->Originator; 

		// respond with a connection detail structure
		SendPacket((BYTE *) &ClientIdentity, sizeof(CLIENT_IDENTITY_DETAIL), 0);	// don't delete pClientIdentity
		break;

	default: // ignore anything we don't know about
		stmp0 = "Received unknown command";
		break;
		} // switch

	// show the command parameters

#if 0
	// if doing debug/development, fill in list box. In production, omit this code.
	pDlg->m_list_CommandParameters.ResetContent();
	pDlg->m_list_CommandParameters.AddString(stmp0);
	stmp2.Format(_T("Param 1 = %d"), CommandMessage->Parameter1);
	pDlg->m_list_CommandParameters.AddString(stmp2);
	stmp2.Format(_T("Param 2 = %d"), CommandMessage->Parameter2);
	pDlg->m_list_CommandParameters.AddString(stmp2);
	stmp2.Format(_T("Param 3 = %d"), CommandMessage->Parameter3);
	pDlg->m_list_CommandParameters.AddString(stmp2);
	stmp2.Format(_T("Param 4 = %d"), CommandMessage->Parameter4);
	pDlg->m_list_CommandParameters.AddString(stmp2);
	stmp2.Format(_T("Param 5 = %s"), CommandMessage->Parameter5);
	pDlg->m_list_CommandParameters.AddString(stmp2);
	stmp2.Format(_T("Param 6 = %s"), CommandMessage->Parameter6);
	pDlg->m_list_CommandParameters.AddString(stmp2);
	stmp2.Format(_T("Param 7 = %s"), CommandMessage->Parameter7);
	pDlg->m_list_CommandParameters.AddString(stmp2);
	stmp2.Format(_T("Param 8 = %s"), CommandMessage->Parameter8);
	pDlg->m_list_CommandParameters.AddString(stmp2);

	// make the horizontal scroll bar work properly
	CString str = _T("");
	CSize sz = 0;
	int i;
	int dx = 0;
	CDC * pDC = pDlg->m_list_CommandParameters.GetDC();
	ASSERT (pDC != NULL);

	for (i=0; i<pDlg->m_list_CommandParameters.GetCount(); i++)
		{
		pDlg->m_list_CommandParameters.GetText(i, str);
		sz = pDC->GetTextExtent(str);

		if (sz.cx > dx)
			dx = sz.cx;
		}
	pDlg->m_list_CommandParameters.ReleaseDC(pDC);
	pDlg->m_list_CommandParameters.SetHorizontalExtent(dx);
#endif

	}


void CCCM_SysCp::DoPipeNumberMessage(TCPDUMMY * tcpDummy)
	{
	USES_CONVERSION;

	CString stmp0 = _T("");	// temporary string holder
	CString stmp1 = _T("");	// temporary string holder

    MAIN_DLG_NAME * pDlg = NULL;
	pDlg = (MAIN_DLG_NAME *) AfxGetMainWnd();
    ASSERT (pDlg != NULL);

#if 1
	if (tcpDummy->MessageType == PIPE_NUMBER_MESSAGE)
		{
		// display the pipe number detail
		PIPE_NUMBER_DETAIL * pPipeNumberDetail = (PIPE_NUMBER_DETAIL *) tcpDummy; 
		ASSERT (pPipeNumberDetail != NULL);

		// remember the current pipe number ID
		pDlg->m_sCurrentPipeNumberID = pPipeNumberDetail->PipeNumberID;

		// workorder, heat, lot
		stmp0.Empty();
		stmp0.LoadString(IDS_RCVD_PIPE_NUMBER);
		stmp0 += " ";
		stmp0 += pPipeNumberDetail->PipeNumberWorkOrder;
		stmp0 += "_";
		stmp0 += pPipeNumberDetail->PipeNumberHeat;
		stmp0 += "_";
		stmp0 += pPipeNumberDetail->PipeNumberLot;
		stmp0 += "_";

		// pipe sequence, suffix, type
		stmp1.Format(_T("%05d"), pPipeNumberDetail->PipeNumberSequence);
		stmp0 += stmp1;

		stmp0 += "_";
		stmp0 += pPipeNumberDetail->PipeNumberSuffix;
		stmp0 += "_";
		stmp0 += pPipeNumberDetail->PipeNumberType;

		stmp0 += " {";
		stmp0 += pPipeNumberDetail->PipeNumberID;
		stmp0 += "}";
		if (!stmp0.IsEmpty())							
			pDlg->AddDebugMessage(stmp0);
		} // pipe number message
	TRACE("Processed PipeNumber Message\n");

#endif	
	}

void CCCM_SysCp::DoGdpParameterMessage(TCPDUMMY * tcpDummy)
	{
	USES_CONVERSION;
	time_t now;
	struct tm * timeinfo;
	char buffer [512];

	time ( &now );
	timeinfo = localtime ( &now );

	strftime (buffer,80,"%Y_%m_%d %H:%M:%S",timeinfo);

	CString stmp0 = _T("");	// temporary string holder
	CString stmp1 = _T("");	// temporary string holder

    MAIN_DLG_NAME * pDlg = NULL;
    pDlg = (MAIN_DLG_NAME *) AfxGetMainWnd();
    ASSERT (pDlg != NULL);

	GDP_PARAMETER_DETAIL * pGDPParameterDetail = (GDP_PARAMETER_DETAIL *) tcpDummy; 
	ASSERT (pGDPParameterDetail != NULL);
	memcpy((void *) &m_GDPParameterDetail, (void *) pGDPParameterDetail, sizeof(GDP_PARAMETER_DETAIL));

	stmp0.Empty();
	stmp0.LoadString(IDS_RCVD_GDP_PARAMETER);
	stmp0 += " ";

	stmp1.Empty();
	stmp1.Format(_T("(Equipment Number = %07d) "), pGDPParameterDetail->EquipmentNumber);
	stmp0 += stmp1;

	stmp1.Empty();
	switch(pGDPParameterDetail->MeasurementUnits)
		{
	case 0: stmp1.LoadString(IDS_IMPERIAL); break;
	case 1: stmp1.LoadString(IDS_METRIC); break;
	default: stmp1.LoadString(IDS_UNKNOWN); break;
		}

	stmp0 += stmp1;

	stmp1.Empty();
	switch(pGDPParameterDetail->GraphRange)
		{
	case 0: stmp1.LoadString(IDS_LOGARITHMIC); break;
	case 1: stmp1.LoadString(IDS_LINEAR); break;
	default: stmp1.LoadString(IDS_UNKNOWN); break;
		}
	stmp0 += ", ";
	stmp0 += stmp1;
	if (!stmp0.IsEmpty())							
		pDlg->AddDebugMessage(stmp0);
	// gdp parameter message
	stmp0 = buffer;
	stmp1.Format(_T("Processed GdpParameter Message at %s\n"),stmp0);
	TRACE(stmp1);
	}

void CCCM_SysCp::DoAlarmMessage(TCPDUMMY * tcpDummy)
	{
	USES_CONVERSION;

	CString stmp0 = _T("");	// temporary string holder
	CString stmp1 = _T("");	// temporary string holder

    MAIN_DLG_NAME * pDlg = NULL;
    pDlg = (MAIN_DLG_NAME *) AfxGetMainWnd();
    ASSERT (pDlg != NULL);
	stmp0.LoadString(IDS_RCVD_MODIFIED_ALARM_DETAIL);
	if (!stmp0.IsEmpty())							
		pDlg->AddDebugMessage(stmp0);
	TRACE("Processed Alarm Message\n");
	}

void CCCM_SysCp::DoReportGeneratorMessage(TCPDUMMY * tcpDummy)
	{
	USES_CONVERSION;

	CString stmp0 = _T("");	// temporary string holder
	CString stmp1 = _T("");	// temporary string holder

    MAIN_DLG_NAME * pDlg = NULL;
    pDlg = (MAIN_DLG_NAME *) AfxGetMainWnd();
    ASSERT (pDlg != NULL);
	REPORT_GENERATOR_DETAIL * pRGD = NULL;
	pRGD = (REPORT_GENERATOR_DETAIL *) tcpDummy;

	stmp0.LoadString(IDS_RCVD_RPG_MESSAGE);
	stmp1.Format(_T(" {%s}"), pRGD->PipeNumberID);
	stmp0 += stmp1;	if (!stmp0.IsEmpty())							
		pDlg->AddDebugMessage(stmp0);
	TRACE("Processed ReportGenerator Message\n");
	}

void CCCM_SysCp::DoAckNakMessage(TCPDUMMY * tcpDummy)
	{
	USES_CONVERSION;

	CString stmp0 = _T("");	// temporary string holder
	CString stmp1 = _T("");	// temporary string holder

    MAIN_DLG_NAME * pDlg = NULL;
    pDlg = (MAIN_DLG_NAME *) AfxGetMainWnd();
    ASSERT (pDlg != NULL);
	MESSAGE_ACK_NAK * pAK = (MESSAGE_ACK_NAK *) tcpDummy; 
	stmp0.LoadString(IDS_RCVD_ACK_NAK_MESSAGE);
	stmp1.Format(_T(" Response = %d"), pAK->Response);
	stmp0 += stmp1;

	if (!stmp0.IsEmpty())							
		pDlg->AddDebugMessage(stmp0);
	TRACE("Processed AckNak Message\n");
	}


// For this class , the received message comes from the SysCp
// Must service all message types shown below
// Over rides base class ProcessReceivedMessage()
// SysCp messages are variable length - must handle each as it arrives and assume
// the whole message arrives in one packet - assumption is that it works like UDP
void CCCM_SysCp::ProcessReceivedMessage(void)
	{
	USES_CONVERSION;

	ASSERT(m_pstCCM);
	if (NULL == m_pstCCM)	return;		// something wrong here
	if (m_pstCCM->pRcvPktPacketList->IsEmpty())		return;	// shouldn't have been called, but no harm

	CString stmp0 = _T("");	// temporary string holder
	CString stmp1 = _T(""); // temporary string holder
	CString stmp2 = _T("");	// temporary string holder

#if 1
	// make access to the main dialog -- this code varies with the name of the main dialog
    MAIN_DLG_NAME * pDlg = NULL;
    pDlg = (MAIN_DLG_NAME *) AfxGetMainWnd();
    ASSERT (pDlg != NULL);
#endif
	 			
	
	
	TCPDUMMY * tcpDummy = NULL;					
	// Access the linked list and process all msgs there
	while (m_pstCCM->pRcvPktPacketList->GetCount() > 0)
		{
		stmp0.Empty();
		stmp1.Empty();
		LockRcvPktList();
		tcpDummy = (TCPDUMMY *) m_pstCCM->pRcvPktPacketList->RemoveHead();
		UnLockRcvPktList();

		// if not for our application,  delete msg

		if ( (tcpDummy->Target != ClientIdentity.Originator) &&	// not our message
			 (tcpDummy->Target != UNKNOWN_COMMAND_PROCESSOR) ) 	// who goes there means SysCp hasn't ID'ed us yet.. we are unknown to syscp
			{
			delete tcpDummy;
			stmp0 = _T("Target not Phase Array Wall");
			goto END_OF_WHILE;
			}

		// Since this class handles the communication with the SysCp, any message not
		// from the SysCp has been directed to the wrong class instance.

		if (tcpDummy->Originator != SYSTEM_CONTROL_PROCESSOR)
			{
			delete tcpDummy;
			stmp0.Format(_T("Message not from SysCp, from 0x%x "), tcpDummy->Originator);
			goto END_OF_WHILE;
			}

		/***************************************************************/

		switch (tcpDummy->MessageType)
			{
		case COMMAND_MESSAGE:			DoCommandMessage(tcpDummy);			break;
		case PIPE_NUMBER_MESSAGE:		DoPipeNumberMessage(tcpDummy);		break;
		case GDP_PARAMETER_MESSAGE:		DoGdpParameterMessage(tcpDummy);	break;
		case ALARM_MESSAGE:				DoAlarmMessage(tcpDummy);			break;
		case REPORT_GENERATOR_MESSAGE:	DoReportGeneratorMessage(tcpDummy);	break;
		case ACK_NAK_MESSAGE:			DoAckNakMessage(tcpDummy);			break;

		default:
			stmp0.Format( _T("Unknown SysCp Message = 0x%0x"),tcpDummy->MessageType);
			break;
			}	//  switch on MessageType

		if (tcpDummy) delete tcpDummy;

	END_OF_WHILE:
			if (!stmp0.IsEmpty())							
				pDlg->AddDebugMessage(stmp0);		

		}	// while (m_pstCCM->pRcvPktPacketList->GetCount())

	}	// ProcessReceivedMessage(void)


void CCCM_SysCp::MakeEquipmentDetail(CString EquipmentNumber,
											CString FamilyName,
											CString EquipmentDescription,
											CString EquipmentLocation,
											CString EquipmentInspectionLine)
	{
	if (m_pstCCM == NULL)	return;
	//if (m_pstCCM->pClientIdentity == NULL)	return;
	//CLIENT_IDENTITY_DETAIL *pCid = m_pstCCM->pClientIdentity;	// reduce typing
	// fill in the equipment detail part of the structure
    _tcscpy_s(ClientIdentity.EquipmentNumber, MAX_CID_CHAR_LENGTH, EquipmentNumber);
    _tcscpy_s(ClientIdentity.FamilyName, MAX_CID_CHAR_LENGTH, FamilyName);
    _tcscpy_s(ClientIdentity.EquipmentDescription, MAX_CID_CHAR_LENGTH, EquipmentDescription);
	_tcscpy_s(ClientIdentity.EquipmentLocation, MAX_CID_CHAR_LENGTH, EquipmentLocation);
	_tcscpy_s(ClientIdentity.EquipmentInspectionLine, MAX_CID_CHAR_LENGTH, EquipmentInspectionLine);

	}

void CCCM_SysCp::MakeVersionDetail(void)
	{
 	CString stmp[9];	// an array of string holder
    int i;
	if (m_pstCCM == NULL)	return;
	//if (m_pstCCM->pClientIdentity == NULL)	return;
	//CLIENT_IDENTITY_DETAIL *pCid = m_pstCCM->pClientIdentity;	// reduce typing

	for (i=0; i<9; i++)
	{
		stmp[i].LoadString(IDS_UNKNOWN);
	}
	
	stmp[0] = RetrieveKeyValue("CompanyName");
	lstrcpyn(ClientIdentity.Company, 
		     stmp[0],
			 stmp[0].GetLength() + sizeof(TCHAR));

	stmp[1] = RetrieveKeyValue("FileDescription");
	lstrcpyn(ClientIdentity.Description, 
		     stmp[1],
			 stmp[1].GetLength() + sizeof(TCHAR));

	stmp[2] = RetrieveKeyValue("FileVersion");
    lstrcpyn(ClientIdentity.ProgramVersion, 
		     stmp[2],
			 stmp[2].GetLength() + sizeof(TCHAR));

	stmp[3] = RetrieveKeyValue("InternalName");
	lstrcpyn(ClientIdentity.ProjectName, 
		     stmp[3],
			 stmp[3].GetLength() + sizeof(TCHAR));

	stmp[4] = RetrieveKeyValue("LegalCopyright");
	lstrcpyn(ClientIdentity.CopyrightNotice, 
		     stmp[4],
			 stmp[4].GetLength() + sizeof(TCHAR));

	stmp[5] = RetrieveKeyValue("OriginalFileName");
	lstrcpyn(ClientIdentity.Executable, 
		     stmp[5],
			 stmp[5].GetLength() + sizeof(TCHAR));

	stmp[6] = RetrieveKeyValue("ProductName");
	lstrcpyn(ClientIdentity.ProductName, 
		     stmp[6],
			 stmp[6].GetLength() + sizeof(TCHAR));

	stmp[7] = RetrieveKeyValue("ProductVersion");
	lstrcpyn(ClientIdentity.ProductVersion, 
		     stmp[7],
			 stmp[7].GetLength() + sizeof(TCHAR));

	stmp[8] = RetrieveKeyValue("---");
	lstrcpyn(ClientIdentity.DevelopmentTeam, 
		     stmp[8],
			 stmp[8].GetLength() + sizeof(TCHAR));
}

void CCCM_SysCp::MakeConnectionDetail(CString ServerIP, 
										UINT ServerPort, 
										CString ClientIP, 
										UINT ClientPort)
	{
	if (NULL == m_pstCCM)	return;

	// fill in the connection detail part of the structure
#if 0
	SetClientIp(ClientIP);
	SetServerIp(ServerIP);
#endif
	_tcscpy_s(ClientIdentity.ServerIP, MAX_CID_IP_CHAR_LENGTH, ServerIP);
	ClientIdentity.ServerPort = ServerPort;
	_tcscpy_s(ClientIdentity.ClientIP, MAX_CID_IP_CHAR_LENGTH, ClientIP);
	ClientIdentity.ClientPort = ClientPort;
	m_pstCCM->sServerIP4 = ServerIP;
	m_pstCCM->sServerIP4 = ClientIP;
	m_pstCCM->uClientPort = ClientPort;
	m_pstCCM->uServerPort = ServerPort;
	ClientIdentity.ThreadID = 0;
	}

	// not part of the base class... debug
void CCCM_SysCp::UniqueProc(void)
	{
	TRACE("Unique to the child class\n");
	}