// DrawTraces.cpp: implementation of the CDrawTraces class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawTraces.h"
#include "math.h"
#include "..\include\cfg100.h"
#include "..\include\udp_msg.h"			//;  Instdata.h included
#include "..\include\nios_msg.h"			//;  Instdata.h included

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CSCAN_REVOLUTION g_RawFlawCscan[2];
extern int  g_nRawFlawBuffer;
extern SITE_SPECIFIC_DEFAULTS SiteDefault;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawTraces::CDrawTraces(CButton *pcButton)
{
	m_pcButton = pcButton;
	m_iTraceLength = 0;
	m_psTrace1 = NULL;
	m_psTrace2 = NULL;

	COLORREF Pallet64Color[64]     =       {//R		//G		//B
									 0+  (  0<<8)+  (144<<16),
									 0+  (  0<<8)+  (160<<16),
									 0+  (  0<<8)+  (176<<16),
									 0+  (  0<<8)+  (192<<16),
									 0+  (  0<<8)+  (208<<16),
									 0+  (  0<<8)+  (224<<16),
									 0+  (  0<<8)+  (240<<16),
									 0+  (  0<<8)+  (255<<16),
									 0+  ( 16<<8)+  (255<<16),
									 0+  ( 32<<8)+  (255<<16),
									 0+  ( 48<<8)+  (255<<16),
									 0+  ( 64<<8)+  (255<<16),
									 0+  ( 80<<8)+  (255<<16),
									 0+  ( 96<<8)+  (255<<16),
									 0+  (112<<8)+  (255<<16),
									 0+  (128<<8)+  (255<<16),
									 0+  (144<<8)+  (255<<16),
									 0+  (160<<8)+  (255<<16),
									 0+  (176<<8)+  (255<<16),
									 0+  (192<<8)+  (255<<16),
									 0+  (208<<8)+  (255<<16),
									 0+  (224<<8)+  (255<<16),
									 0+  (240<<8)+  (255<<16),
									 0+  (255<<8)+  (255<<16),
									16+  (255<<8)+  (255<<16),
									32+  (255<<8)+  (240<<16),
									48+  (255<<8)+  (224<<16),
									64+  (255<<8)+  (208<<16),
									80+  (255<<8)+  (192<<16),
									96+  (255<<8)+  (176<<16),
								   112+  (255<<8)+  (160<<16),
								   128+  (255<<8)+  (144<<16),
								   144+  (255<<8)+  (128<<16),
								   160+  (255<<8)+  (112<<16),
								   176+  (255<<8)+  ( 96<<16),
								   192+  (255<<8)+  ( 80<<16),
								   208+  (255<<8)+  ( 64<<16),
								   224+  (255<<8)+  ( 48<<16),
								   240+  (255<<8)+  ( 32<<16),
								   255+  (255<<8)+  ( 16<<16),
								   255+  (255<<8)+  (  0<<16),
								   255+  (240<<8)+  (  0<<16),
								   255+  (224<<8)+  (  0<<16),
								   255+  (208<<8)+  (  0<<16),
								   255+  (192<<8)+  (  0<<16),
								   255+  (176<<8)+  (  0<<16),
								   255+  (160<<8)+  (  0<<16),
								   255+  (144<<8)+  (  0<<16),
								   255+  (128<<8)+  (  0<<16),
								   255+  (112<<8)+  (  0<<16),
								   255+  ( 96<<8)+  (  0<<16),
								   255+  ( 80<<8)+  (  0<<16),
								   255+  ( 64<<8)+  (  0<<16),
								   255+  ( 48<<8)+  (  0<<16),
								   255+  ( 32<<8)+  (  0<<16),
								   255+  ( 16<<8)+  (  0<<16),
								   255+  (  0<<8)+  (  0<<16),
								   240+  (  0<<8)+  (  0<<16),
								   224+  (  0<<8)+  (  0<<16),
								   208+  (  0<<8)+  (  0<<16),
								   192+  (  0<<8)+  (  0<<16),
								   176+  (  0<<8)+  (  0<<16),
								   160+  (  0<<8)+  (  0<<16),
								   144+  (  0<<8)+  (  0<<16),
								};
	for (int i=0; i<64; i++) {
		m_Pallet64Color[i] = Pallet64Color[i];
	}
}

CDrawTraces::~CDrawTraces()
{
	while (!m_listPointsDisplayed.IsEmpty())
	{
		m_listPointsDisplayed.RemoveHead();
	}

	//delete pccDC;
}

void CDrawTraces::DrawXYPoint(int x, int y1, int y2)
{
	short sYValue;

	if(m_psTrace1 != NULL )
	{
		if (y2 == 1) pccDC->SelectObject (RedPen);
		if (y2 == 2) pccDC->SelectObject (GreecPen);
		if (y2 == 3) pccDC->SelectObject (BluePen);
		if (y2 == 4) pccDC->SelectObject (WhitePen);
		pccDC->SelectObject (RedPen);
		sYValue = m_sLastY1Value;
		if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
		if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
		pccDC->MoveTo(m_sLastXValue,sYValue - m_sMinYValue);

		sYValue = y1;
		if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
		if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
		pccDC->LineTo(x, sYValue - m_sMinYValue);
		m_sLastY1Value = sYValue;
		CString str;
		str.Format(_T("%d"),y2);
		if(pccDC != NULL) AfxMessageBox(str);
		pccDC->MoveTo(0,0);
		pccDC->LineTo(500,-500);
	}

	if(m_psTrace2 != NULL )
	{
		pccDC->SelectObject (BluePen);
		sYValue = m_sLastY2Value;
		if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
		if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
		pccDC->MoveTo(m_sLastXValue,sYValue - m_sMinYValue);

		sYValue = y2;
		if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
		if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
		pccDC->LineTo(x, sYValue - m_sMinYValue);
		m_sLastY2Value = sYValue;
	}
	m_sLastXValue = x;
}

void CDrawTraces::DrawXYPointXORPen(int x, int y, COLORREF color)
{
	short sYValue;
	short sMaxYDisp;

	CRect Rect;
	CPen redPen;
	redPen.CreatePen(PS_SOLID, 0, color);
	CClientDC ccDC(m_pcButton);
	m_pcButton->GetClientRect(&Rect);
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	ccDC.SetMapMode(MM_ANISOTROPIC);
	sMaxYDisp = m_sMaxYValue - m_sMinYValue;
	ccDC.SetWindowExt(m_iTraceLength, sMaxYDisp);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	ccDC.SetViewportOrg(0, Rect.Height());
	ccDC.SetViewportExt(Rect.Width(), -Rect.Height());
	int nOldDrawMode = ccDC.SetROP2(R2_XORPEN);

	ccDC.SelectObject (redPen);
	sYValue = m_sLastY1Value;
	if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
	if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
	ccDC.MoveTo(m_sLastXValue,sYValue - m_sMinYValue);

	sYValue = y;
	if (x > m_iTraceLength) x = m_iTraceLength;
	if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
	if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
	ccDC.LineTo(x, sYValue - m_sMinYValue);
	m_sLastY1Value = sYValue;

	m_sLastXValue = x;

	ccDC.SetROP2(nOldDrawMode);
}

void CDrawTraces::DrawAscanBscan(CPoint *pCPoint, int nCount, COLORREF bkcolor, COLORREF tracecolor, 
									BOOL bDrawScale, int nBaseline, ASCAN_GATES *pAscanGates, CPoint *pTcg, int nTcgCnt, 
									BOOL bDrawTcg, BOOL bShowBscan, BOOL bGrayscale, BOOL bLinearScale,
									BOOL bClearBscan, BYTE nLiveFrozenPeak)
{
	short sMaxYDisp;

	CRect Rect;
	CClientDC ccDC(m_pcButton);
	CDC memDC;

	m_pcButton->GetClientRect(&Rect);

	ccDC.SetMapMode(MM_ANISOTROPIC);
	ccDC.SetWindowExt(Rect.Width(), Rect.Height());	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	if (bShowBscan)
		ccDC.SetViewportOrg(0, Rect.Height()/2);
	else
		ccDC.SetViewportOrg(0, Rect.Height());
	ccDC.SetViewportExt(Rect.Width(), -Rect.Height());

	sMaxYDisp = m_sMaxYValue - m_sMinYValue;

	float xScale = (float) Rect.Width() / (float) m_iTraceLength;  //remap x scale
	float yScale;
	if (bShowBscan)
		yScale = (float) 0.5f * Rect.Height() / (float) sMaxYDisp;      //remap y scale
	else
		yScale = (float) Rect.Height() / (float) sMaxYDisp;      //remap y scale

	int penWidth = 0;

	CPen tracePen;
	tracePen.CreatePen(PS_SOLID, penWidth, tracecolor);

	memDC.CreateCompatibleDC(&ccDC);
	memDC.SetMapMode(MM_ANISOTROPIC);
	memDC.SetWindowExt(Rect.Width(), Rect.Height());	// Define size of window in logical units
	if (bShowBscan)
		memDC.SetViewportOrg(0, Rect.Height()/2);
	else
		memDC.SetViewportOrg(0, Rect.Height());
	memDC.SetViewportExt(Rect.Width(), -Rect.Height());

	CBitmap *pOldBitmap = memDC.SelectObject(&m_bitmap);

	static int LastWidth;
	static int LastHeight;
	if (bShowBscan)
	{
		//memDC.StretchBlt(-1,0-Rect.Height()/2,Rect.Width(),Rect.Height(), &memDC, 0,0-Rect.Height()/2,Rect.Width(),Rect.Height(),SRCCOPY);
		//if (nLiveFrozenPeak != 1)  //not in frozen state
			//memDC.StretchBlt(-1,0-Rect.Height()/2,Rect.Width(),Rect.Height(), &memDC, 0,0-Rect.Height()/2,LastWidth,LastHeight,SRCCOPY);
		LastWidth = Rect.Width();
		LastHeight = Rect.Height();

		if (bClearBscan)
			memDC.FillSolidRect(0,0-Rect.Height()/2-1,Rect.Width(),Rect.Height()+1, bkcolor);
	}

	memDC.FillSolidRect(0,0,Rect.Width(),Rect.Height(), bkcolor);

	//draw scale lines
	if (bDrawScale)
	{
		int i, j, x, y, xV, yH;
		float ixDiv, iyDiv;

		ixDiv = (float) m_iTraceLength * xScale / 10.f;
		iyDiv = (float) sMaxYDisp * yScale / 10.f;
		m_iYDiv = (int) (iyDiv + 0.5f);
		COLORREF clr = RGB(0, 160, 0);
		for(i=1; i<10; i++)
		{
			xV = (int) (ixDiv * i + 0.5f);
			yH = (int) (iyDiv * i + 0.5f);
			for (j=1; j<50; j++)
			{
				y = (int) (iyDiv * j / 5.f + 0.5f);
				memDC.SetPixelV(xV, y, clr);
				x = (int) (ixDiv * j / 5.f + 0.5f);
				memDC.SetPixelV(x, yH, clr);
			}
		}
		if (bShowBscan)
		{
			CPen greenPen(PS_SOLID, 0, RGB(0, 160, 0));
			memDC.SelectObject (&greenPen);
			memDC.MoveTo(0, 0);
			memDC.LineTo(Rect.Width(), 0);
		}
	}

	//draw baseline
	CPen basePen(PS_SOLID, 0, RGB(120, 120, 120));
	memDC.SelectObject (&basePen);
	int nBase = (int) (nBaseline * yScale + 0.5f);
	memDC.MoveTo(0, nBase);
	memDC.LineTo(Rect.Width(), nBase);

	//Draw B-Scan
	int nWidth;
	if (bShowBscan)
	{
		int i, j, k, x, y;
		int height = Rect.Height() / 2;
		x = Rect.Width() - 1;
		int bscanclr;
		COLORREF ClrLast;
		int ylast = 0;
		int clrlast = 0;
		int dir, idod;

		switch (SiteDefault.nPhasedArrayScanType[0])
		{
		case THREE_SCAN_LRW_8_BEAM:
		case TWO_SCAN_LR_8_BEAM:
			nWidth = 8;
			break;

		default:
			nWidth = 16;
			break;
		}

		if (nLiveFrozenPeak != 1)  //not in frozen state
		{
			for (k=0; k<4; k++)
			{
				switch (k)
				{
				case 0:
					dir = 1;   //right
					idod = 1;  //OD
					break;

				case 1:
					dir = 1;   //right
					idod = 0;  //ID
					break;

				case 2:
					dir = 0;   //left
					idod = 1;  //OD
					break;

				case 3:
					dir = 0;   //left
					idod = 0;  //ID
					break;
				}

				for (j=0; j<nWidth; j++)
				{
					ylast = 0;
					clrlast = 0;
					x = Rect.Width() - 5 - k * (nWidth + 2) - (nWidth - 1 - j);
					for (i=0; i<180; i++)
					{
						y = 0 - i * height / 180;
						if (bLinearScale)
							bscanclr = (int) (g_RawFlawCscan[(g_nRawFlawBuffer+1)%2].Amp[dir][j][idod][i] * 2.55f);
						else
							bscanclr = (int) (log10((g_RawFlawCscan[(g_nRawFlawBuffer+1)%2].Amp[dir][j][idod][i]+1) * 127.5f));
						if (bscanclr < 0) bscanclr = 0;
						if (bscanclr > 255) bscanclr = 255;
						if ( (y == ylast) && (bscanclr < clrlast) )
							bscanclr = clrlast;
						else if ( (ylast - y) > 1 )  //moved more than one pixel
						{
							while ( (ylast - y) > 1 )
							{
								ylast--;
								memDC.SetPixelV(x, ylast, ClrLast);
							}
						}
						if (bGrayscale)  //grayscale
						{
							ClrLast = RGB(bscanclr, bscanclr, bscanclr);
							memDC.SetPixelV(x, y, ClrLast);
						}
						else  //color
						{
							memDC.SetPixelV(x, y, m_Pallet64Color[bscanclr/4]);
							ClrLast = m_Pallet64Color[bscanclr/4];
						}
						ylast = y;
						clrlast = bscanclr;
					}
				}
			}
		}

		//draw scale map
		int RectWidth = Rect.Width();
		for (i=0; i<Rect.Height()/2; i++)
		{
			y = i - Rect.Height()/2 + 1;
			bscanclr = i * 255 / (Rect.Height()/2-1);
			if (bGrayscale)  //grayscale
				ClrLast = RGB(bscanclr, bscanclr, bscanclr);
			else
				ClrLast = m_Pallet64Color[bscanclr/4];
			for (x=0; x<2; x++)
				memDC.SetPixelV(RectWidth - (14+ nWidth*4 - 1) - x, y, ClrLast);
		}
	}

	//draw the polyline
	//remap
	for (int i=0; i<nCount; i++)
	{
		pCPoint[i].x = (int) ( (float) pCPoint[i].x * xScale );
		pCPoint[i].y = (int) ( (float) pCPoint[i].y * yScale );
	}

	memDC.SelectObject (&tracePen);

	memDC.Polyline(pCPoint, nCount);

	//Draw TCG Curve
	if (bDrawTcg)
	{
		//remap
		for (int i=0; i<nTcgCnt; i++)
		{
			pTcg[i].x = (int) ( (float) pTcg[i].x * xScale );
			pTcg[i].y = (int) ( (float) pTcg[i].y * yScale );
		}

		COLORREF tcgClr1, tcgClr2;
		if (pAscanGates->bGateSelect[0] == SELECT_TCG_CURVE)
		{
			tcgClr1 = RGB(255, 0, 255);
			tcgClr2 = RGB(255, 0, 255);
		}
		else
		{
			tcgClr2 = RGB(160, 160, 0);
			tcgClr1 = RGB(255, 255, 0);
		}
		CPen tcgPen1(PS_SOLID, 2, tcgClr1);
		CPen tcgPen2(PS_SOLID, 2, tcgClr2);
		memDC.SelectObject (&tcgPen1);
		memDC.Polyline(pTcg, 100);
		memDC.MoveTo(pTcg[99]);
		memDC.LineTo(pTcg[100]);
		memDC.SelectObject (&tcgPen2);
		memDC.Polyline(&pTcg[100], nTcgCnt-100);

		COLORREF gainClr;
		if (pAscanGates->bGateSelect[0] == SELECT_RCVR_GAIN)
			gainClr = RGB(255, 0, 255);
		else
			gainClr = RGB(0, 255, 255);
		CPen gainPen(PS_SOLID, 2, gainClr);
		memDC.SelectObject (&gainPen);
		if (pTcg[0].x > 0)
		{
			memDC.MoveTo(0, pTcg[0].y);
			memDC.LineTo(pTcg[0].x, pTcg[0].y);
		}

		if ( pTcg[nTcgCnt-1].x < (Rect.Width()-1) )
		{
			memDC.MoveTo(pTcg[nTcgCnt-1].x, pTcg[nTcgCnt-1].y);
			memDC.LineTo(Rect.Width()-1, pTcg[nTcgCnt-1].y);
		}
	}

	// draw the baseline cursor
	CPoint polygon[3];
	polygon[0].x = 0;
	polygon[0].y = (int) (nBaseline * yScale + 0.5f);
	polygon[1].x = (int) (nCount / 60.f * xScale + 0.5f);
	polygon[1].y = (int) ((nBaseline - 2) * yScale + 0.5f);
	polygon[2].x = (int) (nCount / 60.f * xScale + 0.5f);;
	polygon[2].y = (int) ((nBaseline + 2) * yScale + 0.5f);

	CPen pen(PS_SOLID, 0, RGB(255,0,0));
	memDC.SelectObject (&pen);
	CBrush brush(RGB(255,0,0));
	CBrush *pOldBrush = memDC.SelectObject(&brush);
	memDC.Polygon(polygon, 3);

	CBrush brushBk(bkcolor);
	memDC.SelectObject(&brushBk);

	//draw gates
	if (pAscanGates->nWhichGate >= 0)
	{
		int i, x, y;
		COLORREF GateColor[4];
		GateColor[0] = RGB(0, 255, 0);
		GateColor[1] = RGB(255, 0, 0);
		GateColor[2] = RGB(0, 0, 255);
		GateColor[3] = RGB( ~(bkcolor & 0x000000FF), ~((bkcolor & 0x0000FF00)>>8), ~((bkcolor & 0x00FF0000)>>16) );
		CPen GatePen[4];
		for (i=0; i<4; i++)
			GatePen[i].CreatePen(PS_SOLID, 2, GateColor[i]);
		if (pAscanGates->nWhichGate == 0)  //draw all gates
		{
			for (i=0; i<4; i++)
			{
				memDC.SelectObject (&(GatePen[i]));
				x = (int) (pAscanGates->GateStart[i] * xScale + 0.5f);
				y = (int) (pAscanGates->GateLevel[i] * yScale + 0.5f);
				memDC.MoveTo(x, y);
				if (pAscanGates->bGateSelect[i] == 1)  //this gate is selected for move
					memDC.Ellipse(x-5, y-5, x+5, y+5);
				x = (int) (pAscanGates->GateStop[i] * xScale + 0.5f);
				memDC.LineTo(x, y);
				if (pAscanGates->bGateSelect[i]  == 1)  //this gate is selected for move
					memDC.Ellipse(x-5, y-5, x+5, y+5);
				else if (pAscanGates->bGateSelect[i]  == 2)  //this gate is selected for changing gate range
				{
					memDC.MoveTo(x, y-8);
					memDC.LineTo(x, y+8);
				}
			}
		}
		else  //draw active gate only
		{
			i = pAscanGates->nWhichGate - 1;
			memDC.SelectObject (GatePen[i]);
			x = (int) (pAscanGates->GateStart[i] * xScale + 0.5f);
			y = (int) (pAscanGates->GateLevel[i] * yScale + 0.5f);
			memDC.MoveTo(x, y);
			if (pAscanGates->bGateSelect[i] == 1)  //this gate is selected for move
				memDC.Ellipse(x-5, y-5, x+5, y+5);
			x = (int) (pAscanGates->GateStop[i] * xScale + 0.5f);
			memDC.LineTo(x, y);
			if (pAscanGates->bGateSelect[i] == 1)  //this gate is selected for move
				memDC.Ellipse(x-5, y-5, x+5, y+5);
			else if (pAscanGates->bGateSelect[i]  == 2)  //this gate is selected for changing gate range
			{
				memDC.MoveTo(x, y-8);
				memDC.LineTo(x, y+8);
			}
		}
	}

	//swap buffers
	int wd = 14 + nWidth * 4;
	int nW = Rect.Width() / wd;
	if (bShowBscan)
	{
		//ccDC.StretchBlt(0,0-Rect.Height()/2,Rect.Width(),Rect.Height(), &memDC, 0,0-Rect.Height()/2,Rect.Width(),Rect.Height(),SRCCOPY);
		ccDC.StretchBlt(0,0,Rect.Width(),Rect.Height()/2, &memDC, 0,0,Rect.Width(),Rect.Height()/2,SRCCOPY);
		ccDC.StretchBlt((Rect.Width()-nW*wd)/2,0-Rect.Height()/2,nW*wd,Rect.Height()/2, &memDC, Rect.Width()-wd,0-Rect.Height()/2,wd,Rect.Height()/2,SRCCOPY);
	}
	else
		ccDC.StretchBlt(0,0,Rect.Width(),Rect.Height(), &memDC, 0,0,Rect.Width(),Rect.Height(),SRCCOPY);

	memDC.SelectObject(pOldBrush);
	memDC.SelectObject(pOldBitmap);  //clean up
}

void CDrawTraces::DrawXYInit()
{
	short sMaxYDisp;

	CString s;
	CRect Rect;
	//CPen WhitePen;
	BluePen.CreatePen(PS_SOLID, 0, RGB(  0,   0, 255));
	GreecPen.CreatePen(PS_SOLID, 0, RGB(  0, 255,   0));
	RedPen.CreatePen(PS_SOLID, 0, RGB(255,   0,   0));
	WhitePen.CreatePen(PS_SOLID, 0, RGB(255,   255,   255));
	pccDC = new CClientDC(m_pcButton);
	m_pcButton->GetClientRect(&Rect);
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	pccDC->SetMapMode(MM_ANISOTROPIC);
	sMaxYDisp = m_sMaxYValue - m_sMinYValue;
	pccDC->SetWindowExt(m_iTraceLength, sMaxYDisp);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	pccDC->SetViewportOrg(0, Rect.Height());
	pccDC->SetViewportExt(Rect.Width(), -Rect.Height());
/*
	pccDC->FillSolidRect(0,0,m_iTraceLength,sMaxYDisp, RGB(0, 255, 0));
	ixDiv = m_iTraceLength/10;
	iyDiv = (int)((float)sMaxYDisp / 10. + .5);
	m_iYDiv = iyDiv;
	pccDC->SelectObject (WhitePen);
	for(i=1; i<10; i++)
	{
		pccDC->MoveTo(ixDiv*i, 0);
		pccDC->LineTo(ixDiv*i, sMaxYDisp);
		pccDC->MoveTo(0, iyDiv*i);
		pccDC->LineTo(m_iTraceLength, iyDiv*i);
	}
*/
}

void CDrawTraces::Draw()
{

	int i;
	int ixDiv, iyDiv;
	short sYValue;
	short sMaxYDisp;

	CString s;
	CRect Rect;
	CPen RedPen;
	CPen BluePen;
	CPen GreenPen;
	CPen WhitePen;
	BluePen.CreatePen(PS_SOLID, 0, RGB(  0,   0, 255));
	GreenPen.CreatePen(PS_SOLID, 0, RGB(  0, 255,   0));
	RedPen.CreatePen(PS_SOLID, 0, RGB(255,   0,   0));
	WhitePen.CreatePen(PS_SOLID, 0, RGB(255,   255,   255));
	CClientDC ccDC(m_pcButton);
	m_pcButton->GetClientRect(&Rect);
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	ccDC.SetMapMode(MM_ANISOTROPIC);
	sMaxYDisp = m_sMaxYValue - m_sMinYValue;
	ccDC.SetWindowExt(m_iTraceLength, sMaxYDisp);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	ccDC.SetViewportOrg(0, Rect.Height());
	ccDC.SetViewportExt(Rect.Width(), -Rect.Height());

	ccDC.FillSolidRect(0,0,m_iTraceLength,sMaxYDisp, RGB(0, 255, 0));
	ixDiv = m_iTraceLength/10;
	iyDiv = (int)((float)sMaxYDisp / 10. + .5);
	m_iYDiv = iyDiv;
	ccDC.SelectObject (WhitePen);
	for(i=1; i<10; i++)
	{
		ccDC.MoveTo(ixDiv*i, 0);
		ccDC.LineTo(ixDiv*i, sMaxYDisp);
		ccDC.MoveTo(0, iyDiv*i);
		ccDC.LineTo(m_iTraceLength, iyDiv*i);
	}

	if(m_psTrace1 != NULL && m_psTraceX != NULL)
	{
		ccDC.SelectObject (RedPen);
		sYValue = m_psTrace1[0];
		if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
		if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
		ccDC.MoveTo(0,sYValue - m_sMinYValue);
		for(i=1; i<m_iTraceLength; i++)
		{
			sYValue = m_psTrace1[i];
			if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
			if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
			ccDC.LineTo(i,sYValue - m_sMinYValue);

		}
	}

	if(m_psTrace2 != NULL && m_psTraceX != NULL)
	{
		ccDC.SelectObject (BluePen);
		sYValue = m_psTrace2[0];
		if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
		if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
		ccDC.MoveTo(0,sYValue - m_sMinYValue);

		for(i=1; i<m_iTraceLength; i++)
		{
			sYValue = m_psTrace2[i];
			if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
			if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
			ccDC.LineTo(i,sYValue - m_sMinYValue);
		}
	}
} 

void CDrawTraces::DrawDisplayed()
{

	int ixDiv, iyDiv;
	short sMaxYDisp;

	CString s;
	CRect Rect;
	//CPen RedPen;
	//CPen BluePen;
	CPen GreenPen;
	CPen LightGreenPen;
	//CPen WhitePen;
	BluePen.CreatePen(PS_SOLID, 0, RGB(  0,   0, 255));
	//GreenPen.CreatePen(PS_SOLID, 0, RGB(  0, 255,   0));
	LightGreenPen.CreatePen(PS_SOLID, 0, RGB(  0, 120,   0));
	//RedPen.CreatePen(PS_SOLID, 0, RGB(255,   0,   0));
	//WhitePen.CreatePen(PS_SOLID, 0, RGB(255,   255,   255));
	CClientDC ccDC(m_pcButton);
	m_pcButton->GetClientRect(&Rect);
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	ccDC.SetMapMode(MM_ANISOTROPIC);
	sMaxYDisp = m_sMaxYValue - m_sMinYValue;
	ccDC.SetWindowExt(m_iTraceLength, sMaxYDisp);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	ccDC.SetViewportOrg(0, Rect.Height());
	ccDC.SetViewportExt(Rect.Width(), -Rect.Height());

	//ccDC.FillSolidRect(0,0,m_iTraceLength,sMaxYDisp, RGB(0, 0, 0));
	ixDiv = m_iTraceLength/10;
	iyDiv = (int)((float)sMaxYDisp / 10. + .5);
	m_iYDiv = iyDiv;
	//ccDC.SelectObject (LightGreenPen);
	//for(i=1; i<10; i++)
	//{
	//	ccDC.MoveTo(ixDiv*i, 0);
	//	ccDC.LineTo(ixDiv*i, sMaxYDisp);
	//	ccDC.MoveTo(0, iyDiv*i);
	//	ccDC.LineTo(m_iTraceLength, iyDiv*i);
	//}

	CPen* pOldPen = ccDC.SelectObject(&GreenPen);
	POSITION pos = m_listPointsDisplayed.GetHeadPosition ();
	CPoint point;
	if (pos != NULL)
	{
		point = m_listPointsDisplayed.GetNext(pos);
		ccDC.MoveTo(point);
	}
	else
		return;
	while (pos != NULL)
	{
		CPoint point = m_listPointsDisplayed.GetNext(pos);
		if(point.y > m_sMaxYValue) point.y = m_sMaxYValue;
		if(point.y < m_sMinYValue) point.y = m_sMinYValue;
		ccDC.LineTo(point);
	}

	ccDC.SelectObject(pOldPen);
} 

void CDrawTraces::DrawFrame()
{
	int i, j;
	int ixDiv, iyDiv;
	short sMaxYDisp;

	CRect Rect;
	CPen LightGreenPen;
	LightGreenPen.CreatePen(PS_DOT, 0, RGB(0,   255,   0));
	CClientDC ccDC(m_pcButton);
	m_pcButton->GetClientRect(&Rect);
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	ccDC.SetMapMode(MM_ANISOTROPIC);
	sMaxYDisp = m_sMaxYValue - m_sMinYValue;
	ccDC.SetWindowExt(m_iTraceLength, sMaxYDisp);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	ccDC.SetViewportOrg(0, Rect.Height());
	ccDC.SetViewportExt(Rect.Width(), -Rect.Height());
	m_bitmap.Detach();
	//m_bitmap.CreateCompatibleBitmap(&ccDC, m_iTraceLength,sMaxYDisp);
	m_bitmap.CreateCompatibleBitmap(&ccDC, 2000,2000);

	ccDC.FillSolidRect(0,0,m_iTraceLength,sMaxYDisp, RGB(0, 0, 0));
	ixDiv = m_iTraceLength/10;
	iyDiv = (int)((float)sMaxYDisp / 10. + .5);
	m_iYDiv = iyDiv;
	ccDC.SelectObject (&LightGreenPen);
	COLORREF clr = RGB(0, 160, 0);
	for(i=1; i<10; i++)
	{
		for (j=1; j<50; j++)
		{
			ccDC.SetPixelV(ixDiv*i, iyDiv*j/5, clr);
			ccDC.SetPixelV(ixDiv*j/5, iyDiv*i, clr);
		}
		/*
		ccDC.MoveTo(ixDiv*i, 0);
		ccDC.LineTo(ixDiv*i, sMaxYDisp);
		ccDC.MoveTo(0, iyDiv*i);
		ccDC.LineTo(m_iTraceLength, iyDiv*i);
		*/
	}

}

void CDrawTraces::DrawNext(short y1, short y2)
{
	short sYValue;
	short sMaxYDisp;
	short sTemp;

	CString s;
	CRect Rect;
	CPen RedPen;
	CPen BluePen;
	CPen GreenPen;
	CPen WhitePen;
	BluePen.CreatePen(PS_SOLID, 0, RGB(  0,   0, 255));
	GreenPen.CreatePen(PS_SOLID, 0, RGB(  0, 255,   0));
	RedPen.CreatePen(PS_SOLID, 0, RGB(255,   0,   0));
	WhitePen.CreatePen(PS_SOLID, 0, RGB(255,   255,   255));
	CClientDC ccDC(m_pcButton);
	m_pcButton->GetClientRect(&Rect);
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	ccDC.SetMapMode(MM_ANISOTROPIC);
	sMaxYDisp = m_sMaxYValue - m_sMinYValue;
	ccDC.SetWindowExt(m_iTraceLength, sMaxYDisp);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	ccDC.SetViewportOrg(0, Rect.Height());
	ccDC.SetViewportExt(Rect.Width(), -Rect.Height());


	if(m_psTrace1 != NULL)
	{
		if (y2 == 1) ccDC.SelectObject (RedPen);
		if (y2 == 2) ccDC.SelectObject (GreenPen);
		if (y2 == 3) ccDC.SelectObject (BluePen);
		if (y2 == 4) ccDC.SelectObject (WhitePen);
		sYValue = y1;
		if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
		if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
		sTemp = m_sLastY1Value - m_sMinYValue;
		if(sTemp < 0) sTemp = 0;
		if(sTemp > sMaxYDisp) sTemp = sMaxYDisp;
		ccDC.MoveTo(m_sLastXValue, sTemp);

		sTemp = sYValue - m_sMinYValue;
		if(sTemp < 0) sTemp = 0;
		if(sTemp > sMaxYDisp) sTemp = sMaxYDisp;
		if (m_sLastXValue >= 0)
			ccDC.LineTo(m_sLastXValue + 1, sTemp);
		m_sLastY1Value = sYValue;

	}

	if(m_psTrace2 != NULL)
	{
		ccDC.SelectObject (BluePen);
		sYValue = y2;
		if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
		if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
		sTemp = m_sLastY2Value - m_sMinYValue;
		if(sTemp < 0) sTemp = 0;
		if(sTemp > sMaxYDisp) sTemp = sMaxYDisp;
		ccDC.MoveTo(m_sLastXValue, sTemp);

		sTemp = sYValue - m_sMinYValue;
		if(sTemp < 0) sTemp = 0;
		if(sTemp > sMaxYDisp) sTemp = sMaxYDisp;
		if (m_sLastXValue >= 0)
			ccDC.LineTo(m_sLastXValue + 1, sTemp);
		m_sLastY2Value = sYValue;
	}

	m_sLastXValue++;
	if(m_sLastXValue >= m_iTraceLength)m_sLastXValue = m_iTraceLength;
}

void CDrawTraces::DrawGate(short iGateStart, short iGateStop, short iLevel, 
						   short iAmp, short iTof, short nGateNumber)
{
	short sYValue;
	short sMaxYDisp;
	short sTemp;

	CRect Rect;
	CPen GatePen;
	CPen AmpPen;
	CClientDC ccDC(m_pcButton);
	m_pcButton->GetClientRect(&Rect);
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	ccDC.SetMapMode(MM_ANISOTROPIC);
	sMaxYDisp = m_sMaxYValue - m_sMinYValue;
	ccDC.SetWindowExt(m_iTraceLength, sMaxYDisp);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	ccDC.SetViewportOrg(0, Rect.Height());
	ccDC.SetViewportExt(Rect.Width(), -Rect.Height());
	int nOldDrawMode = ccDC.SetROP2(R2_XORPEN);

	//if(m_psTrace1 != NULL)
	//{
		if (nGateNumber == 1) 
		{
			GatePen.CreatePen(PS_SOLID, 0, RGB(0,   255,   0));
			AmpPen.CreatePen(PS_SOLID, 3, RGB(0,   255,   0));
		}
		if (nGateNumber == 2) 
		{
			GatePen.CreatePen(PS_SOLID, 0, RGB(255,   0,   0));
			AmpPen.CreatePen(PS_SOLID, 3, RGB(255,   0,   0));
		}
		if (nGateNumber == 3) 
		{
			GatePen.CreatePen(PS_SOLID, 0, RGB(0,   0,   255));
			AmpPen.CreatePen(PS_SOLID, 3, RGB(0,   0,   255));
		}
		if (nGateNumber == 4) 
		{
			GatePen.CreatePen(PS_SOLID, 0, RGB(255,   255,   255));
			AmpPen.CreatePen(PS_SOLID, 3, RGB(255,   255,   255));
		}
		//Draw gate
		CPen* pOldPen = ccDC.SelectObject (&GatePen);
		sYValue = iLevel;
		if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
		if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
		sTemp = sYValue - m_sMinYValue;
		if(sTemp < 0) sTemp = 0;
		if(sTemp > sMaxYDisp) sTemp = sMaxYDisp;
		ccDC.MoveTo(iGateStart, sTemp);
		ccDC.LineTo(iGateStop, sTemp);

		//Draw amplitude strip
		sYValue = iAmp;
		if(sYValue > m_sMaxYValue) sYValue = m_sMaxYValue;
		if(sYValue < m_sMinYValue) sYValue = m_sMinYValue;
		sTemp = sYValue - m_sMinYValue;
		if(sTemp < 0) sTemp = 0;
		if(sTemp > sMaxYDisp) sTemp = sMaxYDisp;
		ccDC.SelectObject (&AmpPen);
		ccDC.MoveTo(iTof, 1);
		ccDC.LineTo(iTof, sTemp);
		ccDC.SelectObject (pOldPen);
	//}
	ccDC.SetROP2(nOldDrawMode);
}

void CDrawTraces::DrawHLine()
{
	short sMaxYDisp;

	CRect Rect;
	CPen BlackPen;
	BlackPen.CreatePen(PS_SOLID, 0, RGB(0,   0,   0));
	CClientDC ccDC(m_pcButton);
	m_pcButton->GetClientRect(&Rect);
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	ccDC.SetMapMode(MM_ANISOTROPIC);
	sMaxYDisp = m_sMaxYValue - m_sMinYValue;
	ccDC.SetWindowExt(m_iTraceLength, sMaxYDisp);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	ccDC.SetViewportOrg(0, Rect.Height());
	ccDC.SetViewportExt(Rect.Width(), -Rect.Height());

	ccDC.SelectObject (BlackPen);
	ccDC.MoveTo(0, (int)(sMaxYDisp/2.+0.5));
	ccDC.LineTo(m_iTraceLength, (int)(sMaxYDisp/2.+0.5));
}
