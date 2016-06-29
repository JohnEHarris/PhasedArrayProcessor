#if !defined(AFX_INSPECT_H__75A79FC0_C9F3_11D3_9B4C_00A0C91FC401__INCLUDED_)
#define AFX_INSPECT_H__75A79FC0_C9F3_11D3_9B4C_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Inspect.h : header file
// 12/18/00 replace wall stat variable with wall stat structures

#include "TcpThreadRxList.h"
#include "Ftext.h"
#include "Ftext2.h"

// Taken from WellChek project

#define INSPECT_STOP			0
#define INSPECT_RUN				1
#define INSPECT_PLAY			2
#define INSPECT_RECORD			3


#define INSPECT_TIME			1
#define INSPECT_MOTION			0

#define INSPECT_SCALE_50		0
#define INSPECT_SCALE_75		1

#define MAP_RECT_LIMIT          5000

typedef struct
{
	short x;
	BYTE  y;
	BYTE  nType;  // 0=wall, 1=long, 2=tran, 3=obq1, 4=obq2, 5=obq3
	BYTE  bIdOd;  // 0: ID,  1: OD
}   EXIST_MAP_RECT;

/////////////////////////////////////////////////////////////////////////////
// CInspect dialog

class CInspect : public CDialog
{
// Construction
public:
	int GetTotalDefects();
	void OnUpdateData(void);	
	void ExitNopMode();
	WORD m_wOpModeTmp;
	void EnterNopMode();
	int m_nDelayedErase;
	void DelayedErase();
	void DrawOblq3();
	void DrawOblq2();
	void DrawLamin();
	void AccumulateTuboWallStats(I_MSG_RUN *pImsg01);
	void InitTuboStats();
	void InitWallStatistics();
	void GetEchoBits( ECHO_BITS *Eb);
	void UpdateWallStatFile(WORD JointLength);
	void AddWallStatsToDib(CDC *pDC, int cxChar, int cyChar, int dy, int LineNum);
	void ExxonWallMovingAverage(I_MSG_RUN *pImsg01);
	void InitExxonMovingAverages(void);

	void UpdateDisplay();

	// Variables used for wall statistics for Exxon
	EXXON_WALL_STAT mEx;
	TUBO_WALL_STAT mTubo;

	int m_nWallNom;	// nominal wall in 1/1000thds
	int m_nWall90, m_nWall120;	// 90% & 120% nominal
	int m_nJointLength;	// inches

	int m_nID;  // There are two inspect window, m_nID used to identify them
			    // and 0 is inspect window 1, 1 is inspect window 2
	void DisableControls();

	BOOL m_bHome;
	void DrawOblq1(void);
//	BOOL AddFlaw(short x,  int radial, int y, 
//			BYTE ChnlType, BYTE ChnlNum);
	BOOL AddFlaw2(short x,  int radial, int y, 
			BYTE ChnlType, BYTE ChnlNum, I_MSG_RUN *pImsg01);
	BOOL AddAllFlaws(short x,  int radial, int y, 
			BYTE ChnlType, BYTE ChnlNum, I_MSG_RUN *pImsg01);
	void InspOblq1Erase();
	void InspOblq2Erase();
	void InspOblq3Erase();
	void InspLaminErase();
	WORD m_wInspEnable;
	void SwapFlaws(FLAW_REC *p1, FLAW_REC *p2);
	BOOL IsSameFlaw(FLAW_REC *p1, FLAW_REC *p2);
	void CompressFlawList(CPtrList *pL);
	void GetConfigRecValues();
	void InspEraseMach(WPARAM w, LPARAM lparam);
	BOOL m_nRefresh , bNoPrnt;
	DWORD m_dwCurrentJoint;
	void PlayOnePipe();
	void OnAutoPrint2();
	BOOL GetAutoPrintFlag();
	CFile* GetDataFile();
	CFile* GetIndexFile();
	void UpdateStatusQRTotals();
	int GetMinXFlaw(FLAW_REC *f1, FLAW_REC *f2, 
					FLAW_REC *f3, FLAW_REC *f4,
					FLAW_REC *f5, FLAW_REC *f6,
					FLAW_REC *f7);

	void GetNextFlaw (FLAW_REC *pF, int i);
	void EraseFlawLists();
    int AddFlawTextToDib (CDC *pDC, int cxChar, int cyChar, int dy, int nGraphics, int PageNum);
	void ShowDlgTitle();
	CInspect(CWnd* pParent = NULL, int nID = 0); // standard constructor
	~CInspect();					// our destructor
    void PlotExistingWall(CDC *pdc, CRect InspRect );
    void PlotExistingTran(CDC *pdc, CRect InspRect );
    void PlotExistingLong(CDC *pdc, CRect InspRect );
    void PlotExistingObq1(CDC *pdc, CRect InspRect );
    void PlotExistingObq2(CDC *pdc, CRect InspRect );
    void PlotExistingObq3(CDC *pdc, CRect InspRect );
	void PlotExistingLamin(CDC *pdc, CRect InspRect );
    void PlotExistingMap(CDC *pdc, CRect MapRect);
    void PlotMapScales(CDC *pdc, CRect MapRect);
    void OnFilePrintScreen() ;
    void  DrawExistWall1();
    //static CInspect *m_pDlg;        // ptr to this dialog..always use
									// the name m_pDlg by convention
	int m_nNoPipeCnt;		// motion pulses w/o pipe in system
	DWORD m_dwJointNum;
	int m_nWallMaxCnt,m_nWallMinCnt;
	int m_nLodFlawCnt,m_nLidFlawCnt;
	int m_nTodFlawCnt,m_nTidFlawCnt;
	int m_nQ1odFlawCnt,m_nQ1idFlawCnt;
	int m_nQ2odFlawCnt,m_nQ2idFlawCnt;
	int m_nQ3odFlawCnt,m_nQ3idFlawCnt;
	int m_nLamodFlawCnt,m_nLamidFlawCnt;
	int m_nTotalFlawCnt,m_nTotalWallCnt;


	CString m_DataPathName;		// name of data file
	CFile *m_pDataFile;			// ptr to data file on hard disk
	CFile *m_pDataIndexFile;	// ptr to data index file
	CFile *m_pDataCsvFile;		// ptr to csv file of wall stats
	CFile *m_pDataFileCD;		// ptr to data file on CD-R
	time_t m_tTimeNow;
	struct tm m_stTime;
	CString m_tTime;
	CString m_sJointNbr;

	static ECHO_BITS m_EchoBits;

	void KillMe();					// another way to die
	void StopTimer(void);
	void StartTimer(void);

	void DrawThold(int nGraph); 
//	void ShowTholdText(void);

	// Wrappers to convert VC6 to Visual Studio 2010
	afx_msg LRESULT VS10_OnCancel(WPARAM, LPARAM);
	afx_msg LRESULT VS10_OnInspEraseAll(WPARAM, LPARAM);
	afx_msg LRESULT VS10_OnStop(WPARAM, LPARAM);
	afx_msg LRESULT VS10_PlayOnePipe(WPARAM, LPARAM);
	afx_msg LRESULT VS10_OnUpdateData(WPARAM, LPARAM);

	// Dialog Data
	//{{AFX_DATA(CInspect)
	enum { IDD = IDD_INSPECT };
	CButton	m_cbInspLamin;
	CButton m_bAuto;
	CStatic	m_stStation2;
	CStatic	m_stStation1;
	CButton	m_cbInspOblq3;
	CButton	m_cbInspOblq2;
	CButton	m_cbInspOblq1;
	CButton	m_cbInspWall;
	CButton	m_cbInspTran;
	CButton	m_cbInspLong;
	CEdit	m_ceCursorY;
	CEdit	m_ceCursorX;
	CButton	m_cbMap;
	int		m_nControlSel;
	int		m_nTimeMotion;
	int		m_nScaleSel;
	BOOL	m_bIdCheck;
	BOOL	m_bOdCheck;
	BOOL	m_bMetric;
	BOOL	m_bPrint;
	BOOL	m_bLongCheck;
	BOOL	m_bTranCheck;
	BOOL	m_bWallCheck;
	int		m_nTholdLid;
	int		m_nTholdLod;
	int		m_nTholdTid;
	int		m_nTholdTod;
	int		m_nTholdThick;
	int		m_nTholdThin;
	BOOL	m_bNoClock;
	BOOL	m_bOblq1Check;
	BOOL	m_bOblq2Check;
	BOOL	m_bOblq3Check;
	int		m_nTholdQ1id;
	int		m_nTholdQ2id;
	int		m_nTholdQ3id;
	int		m_nTholdQ3od;
	int		m_nTholdQ1od;
	int		m_nTholdQ2od;
	int		m_nNcLid;
	int		m_nNcLod;
	int		m_nNx;
	int		m_nNcQ1id;
	int		m_nNcQ1od;
	int		m_nNcQ2id;
	int		m_nNcQ2od;
	int		m_nNcQ3id;
	int		m_nNcQ3od;
	int		m_nNcTid;
	int		m_nNcTod;
	BOOL	m_bPrntFlw;
	int		m_bAutoPrint;
	BOOL	m_bNoPrnt;
	BOOL	m_bRunCalJoint;
	BOOL	m_bAllFlaw;
	int		m_nTholdLamid;
	int		m_nTholdLamod;
	int		m_nNcLamid;
	int		m_nNcLamod;
	BOOL	m_bLaminCheck;
	BOOL	m_bRealTimeReport;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	CPtrList m_plistUtData;
	CPtrList m_plistWallFlaw;	// Analog reject flaws for printing report
	CPtrList m_plistLongFlaw;
	CPtrList m_plistTranFlaw;	// Transverse reject flaws for printing report
	CPtrList m_plistOblq1Flaw;
	CPtrList m_plistOblq2Flaw;
	CPtrList m_plistOblq3Flaw;
	CPtrList m_plistLaminFlaw;

	CPtrList m_plistWallFlawCopy;	// Analog reject flaws for printing report
	CPtrList m_plistLongFlawCopy;
	CPtrList m_plistTranFlawCopy;	// Transverse reject flaws for printing report
	CPtrList m_plistOblq1FlawCopy;
	CPtrList m_plistOblq2FlawCopy;
	CPtrList m_plistOblq3FlawCopy;
	CPtrList m_plistLaminFlawCopy;
	int m_nTotalFlawCntCopy,m_nTotalWallCntCopy;

	void CopyFlawLists();
	void RestoreFlawLists();
	void SortFlawList(CPtrList *pFlawList);


	POSITION m_rWallFlawPos;
	POSITION m_rLongFlawPos;
	POSITION m_rTranFlawPos;
	POSITION m_rObq1FlawPos;
	POSITION m_rObq2FlawPos;
	POSITION m_rObq3FlawPos;
	POSITION m_rLaminFlawPos;

	BOOL SendMsg(int MsgNum);

	// Generated message map functions
	//{{AFX_MSG(CInspect)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnInspEraseAll();
	afx_msg void OnButtonErase();
	afx_msg void OnMotion();
	afx_msg void OnPlay();
	afx_msg void OnRecord();
	afx_msg void OnRun();
	afx_msg void OnScale50();
	afx_msg void OnScale75();
	afx_msg void OnStop();
	afx_msg void OnTime();
	afx_msg void OnIdCheck();
	afx_msg void OnOdCheck();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnTholds();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCkMetric();
    afx_msg void OnCkMetric1();
	afx_msg void OnAutoPrint();
	afx_msg void OnInspRefresh();
	afx_msg void OnCkLong();
	afx_msg void OnCkOblq1();
	afx_msg void OnCkOblq2();
	afx_msg void OnCkOblq3();
	afx_msg void OnCkTran();
	afx_msg void OnCkWall();
	afx_msg void OnNcnx();
	afx_msg void OnIpxstats();
	afx_msg void OnNoClock();
	afx_msg void OnMemBn();
	afx_msg void OnChartAssign();
	afx_msg void OnPrintFlw();
	afx_msg void OnPrint();
	afx_msg void OnPaint();
	afx_msg void OnBtnRefresh();
    afx_msg void OnINSPreport();
	afx_msg void OnCheckCalRun();
	afx_msg void OnAllFlaw();
	afx_msg void OnCkLamin();
	afx_msg void OnCheckRealTimeReport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	void DrawMap();
	void DrawWall();
	void DrawLong();
	void DrawTran();
	void DrawXTicks();

	void InspMapErase();
	void InspWallErase();
	void InspLongErase();
	void InspTranErase();
	void MakeMouseRect();


	int	m_nControlSelLast;

	int m_nScale;		// full scale length in INCHES
	CPoint m_ptStartMap;
	CPen penBlack2;
	CPen penBlack3;
	CPen penRed4;
	CPen penBlue4;
	CPen penMagenta4;
	CPen penBlue2;
	CPen penBlack1;	// reject thold level
	CPen penBlack1Dash;	// ques thold level
	CPen penMagenta2;
	CPen penWhite2;
	CPen penRed1;
	CPen penRed2;
    CPen penBlue1,penBlack1Dot,penRed1Dash,penBlue1Dash;
	CBrush m_hWhiteBrush;
	int pkholdLi,pkholdLo,pkholdTi,pkholdTo,pkholdWi,pkholdWo;
	int pkholdQ1i,pkholdQ1o,pkholdQ2i,pkholdQ2o,pkholdQ3i,pkholdQ3o;
	// Store coord locations of graph rect's relative to containing
	// Dlg window, InspDlg
	CRect m_MouseMapRect;
	CRect m_MouseFlawRect[7];

private:
	UINT m_uIpxTimer;
	BOOL m_bDrawTicks;
	CPoint m_ptLastWallMax, m_ptLastWallMin;
	CPoint m_ptLastLOd, m_ptLastLId;
	//CPoint m_ptLastIOd, m_ptLastIId;
	CPoint m_ptLastQ1od, m_ptLastQ1id;
	CPoint m_ptLastQ2od, m_ptLastQ2id;
	CPoint m_ptLastQ3od, m_ptLastQ3id;
	CPoint m_ptLastLamod, m_ptLastLamid;
	CPoint m_ptLastTod, m_ptLastTid;
	UINT	m_uTimerTick;
	UINT	m_uNoPipeCount;		// USE TO AVOID RECORDING no pipe condition

	int		m_nTholdWodLast;
	int		m_nTholdWidLast;
	int		m_nTholdLidLast;
	int		m_nTholdLodLast;
	int		m_nTholdTodLast;
	int		m_nTholdTidLast;
	int		m_nTholdQ1odLast;
	int		m_nTholdQ1idLast;
	int		m_nTholdQ2odLast;
	int		m_nTholdQ2idLast;
	int		m_nTholdQ3odLast;
	int		m_nTholdQ3idLast;
	int		m_nTholdLamodLast;
	int		m_nTholdLamidLast;

	BOOL    m_bRcvrStation1;
	BOOL    m_bRcvrStation2;
	BOOL    m_bShowStation1;
	BOOL    m_bShowStation2;
	void    ShowRcvrStation(short xloc1, short xloc2);

	static  WORD  m_nMotionBus;
	WORD    m_nHomeAway;
	int		m_nAwayStopXloc;

	WORD	m_wOldStatMap;
	WORD	m_wOldStatWall;
	WORD	m_wOldStatLong;
	WORD	m_wOldStatTran;
	WORD	m_wOldStatObq1;
	WORD	m_wOldStatObq2;
	WORD	m_wOldStatObq3;
	WORD	m_wOldStatLamin;

	void CreateRecordFile(CString sFileName);
	void BackwardEraseMap(CDC *pDC, int nXlocS1, int nXlocS2);
	void BackwardEraseWall(CDC *pDC, int nXloc);
	void BackwardEraseLong(CDC *pDC, int nXloc);
	void BackwardEraseTran(CDC *pDC, int nXloc);
	void BackwardEraseOblq1(CDC *pDC, int nXloc);
	void BackwardEraseOblq2(CDC *pDC, int nXloc);
	void BackwardEraseOblq3(CDC *pDC, int nXloc);
	void BackwardEraseLamin(CDC *pDC, int nXloc);
	BOOL m_bBackward;

	BOOL m_bWriteToFile;
	CPtrList m_plistUtDataFile;
	void WriteJointToFile();  //write one joint to the record file when it leaves the system
	void CreateRecordFileOnCD(CString sFileName);
	void CheckFreeSpaceOnCD();

	void UnCheckCalRun();

	BOOL m_bPipeJustLeft;
	void PrintReportNow();
	float m_nWallClassification;
	int  m_nInvalidWallTimer;
	void ShowInvalidWallClassification();

	void ReverseReport(short nReverseOffset, short nAwayOffset);
	void ReverseExistMap(short nReverseOffset, short nAwayOffset);
	void ReverseExistWall(short nReverseOffset, short nAwayOffset);
	void ReverseExistLong(short nReverseOffset, short nAwayOffset);
	void ReverseExistTran(short nReverseOffset, short nAwayOffset);
	void ReverseExistOblq1(short nReverseOffset, short nAwayOffset);
	void ReverseExistOblq2(short nReverseOffset, short nAwayOffset);
	void ReverseExistOblq3(short nReverseOffset, short nAwayOffset);
	void ReverseExistLamin(short nReverseOffset, short nAwayOffset);
	void ReverseFlawLists(short nReverseOffset, short nAwayOffset);
	void ReverseWallStats(short nReverseOffset, short nAwayOffset);

	int  m_nMapRect;

	void SetPdfFileName_Run();

	short m_nWallDifference;

public:
	CTcpThreadRxList* m_pTcpThreadRxList;
	CPoint m_ptExistLong[900][2];   // ID in 0, OD in 1
	CPoint m_ptExistTran[900][2];   // ID in 0, OD in 1
	CPoint m_ptExistOblq1[900][2];   // ID in 0, OD in 1
	CPoint m_ptExistOblq2[900][2];   // ID in 0, OD in 1
	CPoint m_ptExistOblq3[900][2];   // ID in 0, OD in 1
	CPoint m_ptExistLamin[900][2];   // ID in 0, OD in 1
	CPoint m_ptExistWall[900][2];   // Min in 0, Max in 1
	CPtrList m_pListExistMapRect;
	int    m_nNumExistWall;        // number of points the existing wall traces has.
	void  DrawExistLong();
	void  DrawExistTran();
	void  DrawExistOblq1();
	void  DrawExistOblq2();
	void  DrawExistOblq3();
	void  DrawExistLamin();
	void  DrawExistWall();
	void  DrawExistMap();
	void  EraseExistPts();
	void  RefreshWindow();

	int   m_nProgressS1;
	int   m_nProgressS2;
	int   m_nLastProgressS1;
	int   m_nLastProgressS2;
	BOOL  m_bDrawProgressS1Once;
	BOOL  m_bDrawProgressS2Once;
	void  DrawProgressLine(CClientDC *pDC, int nXloc, int nSattion, BOOL bWall);
	int   m_nProgressTran;
	int   m_nProgressObq2;
	int   m_nLastProgressTran;
	int   m_nLastProgressObq2;
	BOOL  m_bDrawProgressTranOnce;
	BOOL  m_bDrawProgressObq2Once;

	Ftext   *m_pFlawReport;
	Ftext2  *m_pFlawReport2;
	void  MoveReportWindow(int x, int y);

	BOOL FindMaxJointNum(CFile *pDataFile);

	void  SetOblqName();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSPECT_H__75A79FC0_C9F3_11D3_9B4C_00A0C91FC401__INCLUDED_)