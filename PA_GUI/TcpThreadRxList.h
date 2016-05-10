// TcpThreadRxList.h: interface for the CTcpThreadRxList class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _TCPTHREADRXLIST_H_
#define _TCPTHREADRXLIST_H_

#pragma once

#include "..\include\cfg100.h"
#include "..\include\udp_msg.h"			//;  Instdata.h included

typedef struct sThreadData
{
	CPtrList *plistDataIn;
	int iSampleTimeMs;
	int iNumberOfUTBoards;
	int iDummy[16];
}THREAD_DATA;



class CTcpThreadRxList  
{
public:
	void SetSamplingTimeMs(int iMilliSec);
	CTcpThreadRxList();
	virtual ~CTcpThreadRxList();

	CPtrList m_plistUtData;	//This attribute contain the data list

	CONFIG_REC* m_pConfigRec;
	CONFIG_REC GetConfigRec();
	bool InitializeConfigRec(CONFIG_REC *pConfigRec, bool bWriteToBoard);

protected:
	THREAD_DATA m_threadBuf;
	HANDLE m_hAcqDataThread;


protected:

	void WaitMicroseconds(int nMicroseconds);
	WORD m_nMotionBus;
	DWORD m_nNextJointNumber;

public:

	void ClearListUtData();
	void StartTcpAcqDataThread();
	void StopTcpAcqDataThread();

	int m_nSocket;
	bool ServerSocketInit();
	HANDLE m_hSocketThread;
	void StartServerSocketThread();
	void StopServerSocketThread();
	int readn( int fd, char *bp, int len); // read exact n bytes from socket fd.

	BOOL SendSlaveMsg(WORD nMsgId, WORD par1, WORD par2, WORD par3, WORD par4, WORD par5, WORD par6);

	// Steal the old socket send operation and replace with a new one using ServerConnectionManagement components
	BOOL NewSendFunction( BYTE *pBuf, int nSize, int nDeleteFlag);
/*****************************************************************************/

public:

	WORD m_wMsgNum;
	int m_nMsgBlock;	// count number of cmds sent to PAG before sleeping main dialog.

	/* scaling factors for the new 2-channel-4-gate boards */
	int m_DelayScaling; // Scale factor to transfer the delay in microseconds to delay in counts
	int m_RangeScaling; // Scale factor to transfer the range in microseconds to range in counts
	int m_BlankScaling; // Scale factor to transfer the blanking in microseconds to blanking in counts
	int m_LevelScaling; // Scale factor to transfer the threshold in percentage to threshold in counts
	int m_GainScaling;  // Scale factor to transfer the gain in dB to gain in counts
	int m_DelayOffset; // Scale factor to transfer the delay in microseconds to delay in counts
	int m_RangeOffset; // Scale factor to transfer the range in microseconds to range in counts
	int m_BlankOffset; // Scale factor to transfer the blanking in microseconds to blanking in counts
	int m_LevelOffset; // Scale factor to transfer the threshold in percentage to threshold in counts
	int m_GainOffset;  // Scale factor to transfer the gain in dB to gain in counts

	/* scaling factors for the old Truscope instrument */
	int m_TruscopDelayScaling; // Scale factor to transfer the delay in microseconds to delay in counts
	int m_TruscopRangeScaling; // Scale factor to transfer the range in microseconds to range in counts
	int m_TruscopBlankScaling; // Scale factor to transfer the blanking in microseconds to blanking in counts
	int m_TruscopLevelScaling; // Scale factor to transfer the threshold in percentage to threshold in counts
	int m_TruscopGainScaling;  // Scale factor to transfer the gain in dB to gain in counts
	int m_TruscopDelayOffset; // Scale factor to transfer the delay in microseconds to delay in counts
	int m_TruscopRangeOffset; // Scale factor to transfer the range in microseconds to range in counts
	int m_TruscopBlankOffset; // Scale factor to transfer the blanking in microseconds to blanking in counts
	int m_TruscopLevelOffset; // Scale factor to transfer the threshold in percentage to threshold in counts
	int m_TruscopGainOffset;  // Scale factor to transfer the gain in dB to gain in counts

	int m_nActiveChannel;  // 0 or 1, active channel within this board
	int m_nActiveGate;
	int m_nNumberOfBoards;
	bool m_bScopeEnabled;
	bool WriteConfigToBoard();  //This function is platform dependent
	bool WriteScopeSettingsToBoard(); //This function is platform dependent
	bool WriteScopeSettingsToRioBoard();
	bool WriteTcgStepTrigSel(short nChannel);

public:
	bool m_bRioBoard;
	bool WritePulserConfigToRioBoard();
	bool SetScopeTrace2ShuntGateDac(short nChannel, int nShuntGateSel);
//	HANDLE m_hThread;

public:
	void SetScalingFactors(int DelayScaling, int DelayOffset,
						   int RangeScaling, int RangeOffset,
						   int BlankScaling, int BlankOffset,
						   int LevelScaling, int LevelOffset,
						   int GainScaling,  int GainOffset );
	bool SetGateDelay(short nChannel, short nGate, float Delay); //This function is platform independent
	bool SetGateRange(short nChannel, short nGate, float Range); //This function is platform independent
	bool SetGateBlank(short nChannel, short nGate, float Blank); //This function is platform independent
	bool SetGateLevel(short nChannel, short nGate, float Level); //This function is platform independent
	bool SetGateTrigMode(short nChannel, short nGate, short nTrigMode); //This function is platform independent
	bool SetGateTofTrig(short nChannel, short nGate, short nTofTrig); //This function is platform independent
	bool SetGateTofStopon(short nChannel, short nGate, short nTofStopon); //This function is platform independent
	bool SetGateDetectMode(short nChannel, short nGate, short nDetectMode); //This function is platform independent
	bool SetGatePolarity(short nChannel, short nGate, short nGatePol); //This function is platform independent
	bool SetRcvrGain(short nChannel, float Gain); //This function is platform independent
	bool SetRcvrFilter(short nChannel, short nFilterSel); //This function is platform independent
	bool SetRcvrOffset(short nChannel, short nRcvrOffset); //This function is platform independent
	bool SetPulserPRF(short nPRF);
	bool SetPulserWidth(short nChannel, short nPulseWidth);
	bool SetPulserMode(BYTE nMode);
	bool SetPrfIntExt(short nSlave);
	bool SetScopeTrace1DisplaySel(short nChannel, unsigned short Trace1DispSel);
	bool SetScopeTrace1TestSel(short nChannel, unsigned short Trace1TestSel);
	bool SetScopeTrace2DisplaySel(short nChannel, unsigned short Trace2DispSel);
	bool SetScopeTrace2TestSel(short nChannel, unsigned short Trace2TestSel);
	bool SetScopeTrace2GateSel(short nChannel, short nGate, unsigned short Trace2GateSel);
	bool SetScopeTrace2DiagnosSel(short nChannel, unsigned short Trace2TestSel);
	bool SetScopeTrigger(short nChannel, unsigned short nTrigSel);
	bool SetScopeTrace2CalData(int nStep, int nValue);
	BOOL SetScopeTrace1CalData(WORD nInstrument, WORD nChannel, WORD nStep, WORD nValue, BOOL bDo90);
	WORD GetScopeTrace1CalData(WORD nInstrument, WORD nChannel, WORD nStep, BOOL bDo90);
	BOOL SetScopeTrace1Mdac(WORD nInstrument, WORD nValue);
	void EnableScopeDisplay(bool bEnabled, short nChannel, short nGate);
	bool ReadAmplitudeTOF();
	bool SetTcgGainTable(short nSeqNum, short nChannel, short nGate, float Gain, int nTcgMode);
	bool SendTcgGainTable(short nSeqNum);
	void ReSetTcgGainTable(short nChannel, BOOL bSend);
	void SendTcgFunction(short nChannel);
	unsigned long m_bufGainTable[256];
	void ComputeTcgGainTable(short nChannel);
	float m_TcgGain[MAX_CHANNEL][4];
	float m_CurveCoef[MAX_CHANNEL][5];
	int   m_LastTcgGateStart;
	int   m_LastTcgGateStop;

	/* auto calibration related functions and variables */
	float m_fIdTcgGain[MAX_CHANNEL];
	float m_fOdTcgGain[MAX_CHANNEL];
	void  ComputeIdTcgGain(short nChannel);
	void  ComputeOdTcgGain(short nChannel);
	void  ComputeGateTcgGain(short nChannel, short nGate);
	void  SetIdTcgGain(short nChannel, float fGain);
	void  SetOdTcgGain(short nChannel, float fGain);
	float GetIdTcgGain(short nChannel);
	float GetOdTcgGain(short nChannel);
	float GetGainOfGate(short nChannel, short nGate);

	float GetGateDelay(short nChannel, short nGate);
	float GetGateRange(short nChannel, short nGate);
	float GetGateLevel(short nChannel, short nGate);
	float GetGateBlank(short nChannel, short nGate);
	short GetGateTrigMode(short nChannel, short nGate); //This function is platform independent
	short GetGateTofTrig(short nChannel, short nGate); //This function is platform independent
	short GetGateTofStopon(short nChannel, short nGate); //This function is platform independent
	short GetGateDetectMode(short nChannel, short nGate); //This function is platform independent
	short GetGatePolarity(short nChannel, short nGate); //This function is platform independent
	float GetRcvrGain(short nChannel);
	short GetRcvrFilter(short nChannel);
	short GetRcvrOffset(short nChannel);
	unsigned short GetScopeTrace1DisplaySel(short nChannel);
	unsigned short GetScopeTrace1TestSel(short nChannel);
	unsigned short GetScopeTrace2DisplaySel(short nChannel);
	unsigned short GetScopeTrace2TestSel(short nChannel);
	unsigned short GetScopeTrace2DiagnosSel(short nChannel);
	unsigned short GetScopeTrigger(short nChannel);
	unsigned short GetScopeTrace2GateSel(short nChannel, short nGate);
	short GetPulserPRF();
	unsigned char GetPulserMode();

	float m_Amplitude[2][4];  // Amplitude read from the board.  The first subscript is zero-based
							  // channel number, and the second subscript is zero-based gate number.
	float m_TOF[2][4];  // Time-of-flight read from the board.  The first subscript is zero-based
							  // channel number, and the second subscript is zero-based gate number.
	float m_MaxAmp[2][4], m_MaxTOF[2][4];
	float m_AmpDisp[401][2][4];
	float m_TOFDisp[401][2][4];
	unsigned long m_AlarmBitFlag;
	bool SetAlarmBitsOnRioBoard(unsigned long AlarmBitFlag);

	bool SetSequenceLength(short nChannel);
	bool EnableFireSequence(short nChannel, bool bEnabled);
	bool EnableReceiveSequence(short nChannel, bool bEnabled);
	bool SetTcgUpdateRate(short nChannel, unsigned short nUpdateRate);
	bool SetTcgTrigSel(short nChannel, unsigned short nTrigSel);
	unsigned short GetTcgUpdateRate(short nChannel);
	unsigned short GetTcgTrigSel(short nChannel);
	void SetTcgAarg(short nChannel, short nTcgAarg);
	void SetTcgBarg(short nChannel, short nTcgBarg);
	BOOL SetAllTholds();
	BOOL SendChnlCfgMsg();
	BOOL SendJobRec();
	BOOL SetSiteDefaults(SITE_SPECIFIC_DEFAULTS *pSiteDef);

	void LoadShuntDac();  /* might be temporary */
	BOOL SendMDAC();   /* might be temporary */

	/* alarm set up */
	BOOL SetGateAlarm(short nChannel, short nGate, short nOnOff);
	BOOL SetGateAlarmLogic(short nChannel, short nGate, short nPolarity);
	BOOL SetWallAlarm(short nChannel, short nGate, short nOnOff);

	// wall calibration related functions
	BOOL SetWallCoefs();

	BOOL SetNcNx();

	// Master ADI related functions
	BOOL SetAdiInputInvert(DWORD nInputInv);
	DWORD GetAdiInputInvert();

	BOARD_REV m_nBoardRevision;
	void SetBoardRevision(I_MSG_NET *pNetBuf);
	void GetBoardRevision(BOARD_REV *pRev);

	void SetMotionBus(WORD nMotionBus);
	WORD GetMotionBus();

	void SetNextJointNumber(DWORD nNextJointNumber);
	DWORD GetNextJointNumber();

	BOOL SendLastJointNum(DWORD nLastRealJoint, DWORD nLastCalJoint);
	BOOL SendCalRunMsg(BOOL bRunCalJoint);

	BOOL SetAscanRegisters(short nChannel);

	bool SetReceiverProcess(short nChannel);
};


#endif // _TCPTHREADRXLIST_H_