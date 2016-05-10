// DrawTraces.h: interface for the CDrawTraces class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWTRACES_H__55681E06_DDB2_4435_B0C5_D550417101B6__INCLUDED_)
#define AFX_DRAWTRACES_H__55681E06_DDB2_4435_B0C5_D550417101B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Afxtempl.h"

#define SELECT_GATE_DELAY 1
#define SELECT_GATE_RANGE 2
#define SELECT_TCG_CURVE  3
#define SELECT_RCVR_GAIN  4

typedef struct
{
	short nWhichGate;   //-1: none; 0: all; 1: Gate 1; etc.
	short GateStart[4]; //in logical coordinate of the current device context
	short GateStop[4];  //in logical coordinate of the current device context
	short GateLevel[4];
	BYTE  bGateSelect[4];
} ASCAN_GATES;

class CDrawTraces  
{
public:
	void DrawHLine();
	CPen RedPen;
	CPen BluePen;
	CPen GreecPen;
	CPen WhitePen;
	int m_iCurrentPoint;
	CClientDC *pccDC;
	void DrawXYInit();
	void DrawXYPoint(int x, int y1, int y2);
	void DrawXYPointXORPen(int x, int y, COLORREF color);
	void DrawAscanBscan(CPoint *pCPoint, int nCount, COLORREF bkcolor, COLORREF tracecolor, BOOL bDrawScale, int nBaseline, ASCAN_GATES *pAscanGates, CPoint *pTcg, int nTcgCnt, BOOL bDrawTcg, BOOL bShowBscan, BOOL bGrayscale, BOOL bLinearScale, BOOL bClearBscan, BYTE nLiveFrozenPeak);
	int m_iYDiv;
	short m_sLastY1Value;
	short m_sLastY2Value;
	short m_sLastXValue;
	void DrawNext(short, short);
	void DrawGate(short, short,short, short,short, short);
	void DrawFrame();
	void Draw();
	void DrawDisplayed();
	CList<CPoint, CPoint&> m_listPointsDisplayed;
	short *m_psTrace1;
	short *m_psTrace2;
	short *m_psTraceX;
	short m_sMaxYValue;
	short m_sMinYValue;
	int m_iTraceLength;
	CDrawTraces(CButton *pcButton);
	virtual ~CDrawTraces();
	CButton *m_pcButton;

	CBitmap  m_bitmap;

	COLORREF m_Pallet64Color[64];
};

#endif // !defined(AFX_DRAWTRACES_H__55681E06_DDB2_4435_B0C5_D550417101B6__INCLUDED_)
