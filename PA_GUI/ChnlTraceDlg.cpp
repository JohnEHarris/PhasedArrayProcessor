// ChnlTraceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "ChnlTraceDlg.h"
#include "TscanDlg.h"
#include "ChnlRangeDlg.h"
#include "afxmt.h"
#include "extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DRAW_TIMER     1236  //timer ID for draw traces
#define REFRESH_WINDOW 1237  //timer ID for repaint window

extern CPtrList g_pTcpListUtDataChnls;  /* linked list holding the IData for the Channel Traces window */
extern CCriticalSection g_CriticalSectionChnls;
extern CONFIG_REC ConfigRec;

/////////////////////////////////////////////////////////////////////////////
// CChnlTraceDlg dialog


CChnlTraceDlg::CChnlTraceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChnlTraceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChnlTraceDlg)
	//}}AFX_DATA_INIT

#if (LOCATION_ID == BESSEMER)
	for (int i=0; i<30; i++)
#else
	for (int i=0; i<40; i++)
#endif
	{
		m_bChnlSelect[i] = 1;
		m_nNumExistWall[i] = 0;
	}

	m_nScale = 600;
}


void CChnlTraceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChnlTraceDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChnlTraceDlg, CDialog)
	//{{AFX_MSG_MAP(CChnlTraceDlg)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChnlTraceDlg message handlers

void CChnlTraceDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	KillTimer(DRAW_TIMER);

	SaveWindowPosition(_T("Channel Traces"));

	((CTscanDlg*) GetParent())->m_pChnlTraceDlg = NULL;

	DestroyWindow();
}

void CChnlTraceDlg::OnOK() 
{
	// TODO: Add extra validation here
	KillTimer(DRAW_TIMER);

	SaveWindowPosition(_T("Channel Traces"));

	((CTscanDlg*) GetParent())->m_pChnlTraceDlg = NULL;

	DestroyWindow();
}

void CChnlTraceDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();

	delete this;
}

void CChnlTraceDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	SetTimer(REFRESH_WINDOW, 50, NULL);	
	TRACE("CChnlTraceDlg::OnPaint\n");
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}

void CChnlTraceDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	SetTimer(REFRESH_WINDOW, 50, NULL);	
	TRACE("CChnlTraceDlg::OnSize\n");
	// TODO: Add your message handler code here
	
}

void CChnlTraceDlg::EraseTraces()
{
	int i, j;
	CString s;

	int nChnlCnt = 0;
	for (i=0; i<40; i++)
		if (m_bChnlSelect[i])
			nChnlCnt++;

    short WallRange = (short) (20.0f * ConfigRec.JobRec.Wall * 
                            (float) MAX_DELTA_WALL);

	CRect Rect;
	CPen BlackPen, LightGreenPen;
	BlackPen.CreatePen(PS_SOLID, 0, RGB(0,   0,   0));
	LightGreenPen.CreatePen(PS_SOLID, 0, RGB(0,   255,   0));
	CClientDC ccDC(this);

	GetClientRect(&Rect);
	ccDC.FillSolidRect(0,0,Rect.Width(),Rect.Height(), RGB(255, 255, 255));
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	ccDC.SetMapMode(MM_ANISOTROPIC);
	ccDC.SetWindowExt(m_nScale, nChnlCnt*200);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	ccDC.SetViewportOrg(0, Rect.Height());
	ccDC.SetViewportExt(Rect.Width(), -Rect.Height());

	//ccDC.FillSolidRect(0,0,m_nScale,nChnlCnt*200, RGB(255, 255, 255));
	ccDC.SelectObject (&BlackPen);

	for(i=1; i<nChnlCnt; i++)
	{
		ccDC.MoveTo(0, i*200);
		ccDC.LineTo(m_nScale, i*200);
	}
	ccDC.SelectObject (&LightGreenPen);
	for(i=1; i<=nChnlCnt; i++)
	{
		ccDC.MoveTo(0, i*200-100);
		ccDC.LineTo(m_nScale, i*200-100);
	}

	CPen penId, penOd;
    penOd.CreatePen(PS_DOT, 0, RGB(0,   0,   255));
    penId.CreatePen(PS_DOT, 0, RGB(255,   0,   0));

	CFont mFont;
	mFont.CreatePointFont(Rect.Height()/10,_T("Arial"), &ccDC);
	ccDC.SetBkMode(TRANSPARENT);
	ccDC.SelectObject(&mFont);
	j = 0;
	for (i=0; i<40; i++)
	{
		if (m_bChnlSelect[i])
		{
			int nID, nOD;

			if (nChnlCnt <= 10)  //draw threshold lines
			{
				switch (ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Type)
				{
				case IS_WALL:
					nID = ConfigRec.UtRec.TholdWall[0] * 100 / MAX_DELTA_WALL + 100 + j*200;
					nOD = ConfigRec.UtRec.TholdWall[1] * 100 / MAX_DELTA_WALL + 100 + j*200;
					break;

				case IS_LONG:
					nID = ConfigRec.UtRec.TholdLong[0][0] + j*200;
					nOD = ConfigRec.UtRec.TholdLong[0][1] + 100 + j*200;
					break;

				case IS_TRAN:
					nID = ConfigRec.UtRec.TholdLong[1][0] + j*200;
					nOD = ConfigRec.UtRec.TholdLong[1][1] + 100 + j*200;
					break;

				case IS_OBQ1:
					nID = ConfigRec.UtRec.TholdLong[2][0] + j*200;
					nOD = ConfigRec.UtRec.TholdLong[2][1] + 100 + j*200;
					break;

				case IS_OBQ2:
					nID = ConfigRec.UtRec.TholdLong[3][0] + j*200;
					nOD = ConfigRec.UtRec.TholdLong[3][1] + 100 + j*200;
					break;

				case IS_OBQ3:
					nID = ConfigRec.UtRec.TholdLong[4][0] + j*200;
					nOD = ConfigRec.UtRec.TholdLong[4][1] + 100 + j*200;
					break;

				default:
					nID = 50 + j*200;
					nOD = 50 + 100 + j*200;
					break;
				}
			
				ccDC.SelectObject (&penId);
				ccDC.MoveTo(0, nID);
				ccDC.LineTo(m_nScale, nID);
				ccDC.SelectObject (&penOd);
				ccDC.MoveTo(0, nOD);
				ccDC.LineTo(m_nScale, nOD);
			}
			int nType, n;

			nType = ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Type ;

			switch(nType)
			{
				case IS_NOTHING:
				default:		n = 0;		break;

				case IS_WALL:	n = 1;		break;

				case IS_LONG:	n = 2;		break;

				case IS_TRAN:	n = 3;		break;

				case IS_OBQ1:	n = 4;		break;
				case IS_OBQ2:	n = 5;		break;
				case IS_OBQ3:	n = 6;		break;
				case IS_LAM:		n = 7;				break;		

			}

			s.Format(_T("%2d-%s"), i+1, XdcrText[n]);
			ccDC.TextOut(1, 140+200*j, s);

			j++;
		}
	}

	DrawXTicks();
}

void CChnlTraceDlg::DrawXTicks()
{
        // x axis ticks drawn on bottom of image map
    CPen *penOld;
    CPen penBlue(PS_SOLID,1, RGB(0,255,0));

    CClientDC dc(this); // get drawing context for 

    int i,m;
    CString s;
    float fmeter;
	int nChnlCnt = 0;
	for (i=0; i<40; i++)
		if (m_bChnlSelect[i])
			nChnlCnt++;

	CRect Rect;

	GetClientRect(&Rect);
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(m_nScale, nChnlCnt*200);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	dc.SetViewportOrg(0, Rect.Height());
	dc.SetViewportExt(Rect.Width(), -Rect.Height());

    penOld = dc.SelectObject(&penBlue);


	BYTE bMetric;
	if (ConfigRec.bEnglishMetric == ENGLISH)
		bMetric = 0;
	else
		bMetric = 1;

    switch (bMetric)
    {
      case 0:
        // English
        // draw ticks and number every 10 ft., ie., 10, 20, 30 etc
        for ( i = 120; i < m_nScale; i += 120)
        {
            dc.MoveTo(i,90);
            dc.LineTo(i,110);
            dc.MoveTo(i,90+(nChnlCnt-1)*200);
            dc.LineTo(i,110+(nChnlCnt-1)*200);
        }
        // draw 2 ft marks
        for ( i = 24; i < m_nScale; i += 24)
        {
            dc.MoveTo(i,95);
            dc.LineTo(i,105);
            dc.MoveTo(i,95+(nChnlCnt-1)*200);
            dc.LineTo(i,105+(nChnlCnt-1)*200);
        }
        break;

      case 1:
        // Metric
        // draw ticks and number every 5 meters ... still 50 or 75 ft scale
        fmeter = 5.0f;
        m = 5;
        i = (int) (fmeter * 39.37f  + 0.5f);
        while ( i < m_nScale)
        {
            dc.MoveTo(i,90);
            dc.LineTo(i,110);
            dc.MoveTo(i,90+(nChnlCnt-1)*200);
            dc.LineTo(i,110+(nChnlCnt-1)*200);
            fmeter += 5.0f;
            m += 5;
            i = (int) (fmeter * 39.37f  + 0.5f);
        }

        // draw 1 meter marks
        fmeter = 1.0f;
        m = 1;
        i = (int) (fmeter * 39.37f  + 0.5f);
        while ( i < m_nScale)
        {
            dc.MoveTo(i,95);
            dc.LineTo(i,105);
            dc.MoveTo(i,95+(nChnlCnt-1)*200);
            dc.LineTo(i,105+(nChnlCnt-1)*200);
            fmeter += 1.0f;
            m += 1;
            i = (int) (fmeter * 39.37f  + 0.5f);
        }
        break;

      default:
        break;
    }  //end switch

    dc.SelectObject(penOld);
}

void CChnlTraceDlg::DrawTraces()
{
	int i, j;
    CPoint ptNewOd, ptNewId;    // new  values for Sonoscope id/od
    CPen *penOld;
    CPen penId, penOd;
    WORD PipeStatus;

    if (m_plistUtData.IsEmpty() ) return ;

    short NomWall = (short) (1000.0f * ConfigRec.JobRec.Wall + 0.5f);
    short WallRange = (short) (20.0f * ConfigRec.JobRec.Wall * 
                            (float) MAX_DELTA_WALL);

	int nChnlCnt = 0;
	for (i=0; i<40; i++)
		if (m_bChnlSelect[i])
			nChnlCnt++;

    CClientDC dc(this);    // get drawing context for  Long graph
    //  Set up reference area for graphing

    I_MSG_RUN *pImsg01;

    // Get the client rectangle boundaries for the Image Map graph (Button)
	CRect Rect;

	GetClientRect(&Rect);
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(m_nScale, nChnlCnt*200);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	dc.SetViewportOrg(0, Rect.Height());
	dc.SetViewportExt(Rect.Width(), -Rect.Height());

    POSITION rPosition = m_plistUtData.GetHeadPosition();

    penOd.CreatePen(PS_SOLID, 2, RGB(0,   0,   255));
    penId.CreatePen(PS_SOLID, 2, RGB(255,   0,   0));

    penOld = dc.SelectObject(&penOd);    // OD is blue


    while (1)
    {
        pImsg01 = (I_MSG_RUN *) m_plistUtData.GetNext(rPosition);
        if (pImsg01->MstrHdr.MsgId != RUN_MODE) goto ENDLOOP;
        // Only if Inspect status is true
        // Check line status to determine when to erase
        // if inspect enable just went true, erase graph

        PipeStatus = 0;
        if (pImsg01->InspHdr.wLineStatus & PIPE_PRESENT) PipeStatus = 1; 
        if (m_wOldPipeStatus & PIPE_PRESENT) PipeStatus |= 2;
        m_wOldPipeStatus = pImsg01->InspHdr.wLineStatus;

        switch ( PipeStatus)
        {   // pipe transitions
			case 0:
				break;  // no pipe in system
			case 1:
				EraseTraces();
				EraseExistingPts();
				break;  // just arrived
			case 2:
				break;  // just left system
			case 3:
				break;  // been here for a while
        } //end switch


        // Only if Inspect status is true
        if (!(pImsg01->InspHdr.wLineStatus & INSPECT_ENABLE_TRUSCOPE) )
        {
            //  get next in data list
            goto ENDLOOP;
        }

        // Same x Location for all radial elements of UT
        ptNewOd.x = ptNewId.x = (int) pImsg01->InspHdr.xloc % m_nScale;

		j = 0;
		for (i=0; i<40; i++)  //channel loop
		{
			if (ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Type != IS_WALL)  //flaw channel
			{
				if ( ptNewOd.x > 0 )
				{
					// Look at OD
					ptNewOd.y = min(pImsg01->UtInsp.GateMaxAmp[i*2+1], 100)+100;
					if (ptNewOd.y > 100) 
					{
						if (m_bChnlSelect[i])
						{
							dc.SelectObject(&penOd);
							dc.MoveTo(ptNewOd.x,100+j*200);
							dc.LineTo(ptNewOd.x, ptNewOd.y+j*200);
						}
						if ( (ptNewOd.x < 900) && (ptNewOd.x >= 0) )
							m_ptExist[i][ptNewOd.x][1] = ptNewOd;
					}

				}

				// Look at ID

				if ( ptNewId.x > 0 ) 
				{
					ptNewId.y = min(pImsg01->UtInsp.GateMaxAmp[i*2], 100);
					if (ptNewId.y > 0)
					{
						if (m_bChnlSelect[i])
						{
							dc.SelectObject(&penId);
							dc.MoveTo(ptNewId.x,j*200);
							dc.LineTo(ptNewId.x, ptNewId.y+j*200);
						}
						if ( (ptNewId.x < 900) && (ptNewId.x >= 0) )
							m_ptExist[i][ptNewId.x][0] = ptNewId;
					}

				}
			}
			else  //wall channel
			{
				if (  (pImsg01->InspHdr.wLineStatus & REAL_TIME_DATA) ||
					  ( (pImsg01->InspHdr.status[1] & WALL_INCLUDED) != WALL_INCLUDED ) )
				{
					if (m_bChnlSelect[i])
						j++;
					continue;
				}

				// Look at Wall min trace

				if ( ptNewId.x > 0 )   
				{
					dc.SelectObject(&penId);

					ptNewId.y = min( *((WORD *) &pImsg01->UtInsp.GateMaxAmp[i*2]) - NomWall,WallRange/2 );
					if ( *((WORD *) &pImsg01->UtInsp.GateMaxAmp[i*2]) == 0x3FFF )
						ptNewId.y = 0;
					ptNewId.y = max(ptNewId.y,-WallRange/2);

					if ( m_bChnlSelect[i] && (m_nNumExistWall[i] > 0) )
					{
						dc.MoveTo(m_ptExist[i][m_nNumExistWall[i]-1][0].x, m_ptExist[i][m_nNumExistWall[i]-1][0].y*100/(WallRange/2) +100+j*200);
						dc.LineTo(ptNewId.x, ptNewId.y*100/(WallRange/2) +100+j*200);
					}

					if (m_nNumExistWall[i]<900) 
					{
						m_ptExist[i][m_nNumExistWall[i]][0] = ptNewId;
						m_nNumExistWall[i]++;
					}
				}
			}
					
			if (m_bChnlSelect[i])
				j++;
		}


ENDLOOP:

        if (rPosition == NULL ) break;
  
    }   //  End while (1)

    dc.SelectObject(penOld);

}

void CChnlTraceDlg::DrawExistingTraces()
{
	int i, j, k;
    CPoint ptNewOd, ptNewId;    // new  values for Sonoscope id/od
    CPen *penOld;
    CPen penId, penOd;

    short WallRange = (short) (20.0f * ConfigRec.JobRec.Wall * 
                            (float) MAX_DELTA_WALL);

	int nChnlCnt = 0;
	for (i=0; i<40; i++)
		if (m_bChnlSelect[i])
			nChnlCnt++;

    CClientDC dc(this);    // get drawing context for  Long graph
    //  Set up reference area for graphing

    // Get the client rectangle boundaries for the Image Map graph (Button)
	CRect Rect;

	GetClientRect(&Rect);
	//Rect.NormalizeRect();
	// Move the coordinate origin to the lower left corner
	// Set mapping mode to isotropic, deal in logical units rather than pixels
	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(m_nScale, nChnlCnt*200);	// Define size of window in logical units
	// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
	// Tell windows how big this box is physically in pixels
	dc.SetViewportOrg(0, Rect.Height());
	dc.SetViewportExt(Rect.Width(), -Rect.Height());

    penOd.CreatePen(PS_SOLID, 2, RGB(0,   0,   255));
    penId.CreatePen(PS_SOLID, 2, RGB(255,   0,   0));

    penOld = dc.SelectObject(&penOd);    // OD is blue

	j = 0;
	for (i=0; i<40; i++)  //channel loop
	{
		if (m_bChnlSelect[i])
		{
			if (ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Type != IS_WALL)
			{
				for (k=0; k<m_nScale; k++)
				{
					// Look at OD
					ptNewOd = m_ptExist[i][k][1];
					if (ptNewOd.y > 100)
					{
						dc.SelectObject(&penOd);
						dc.MoveTo(ptNewOd.x,100+j*200);           
						dc.LineTo(ptNewOd.x, ptNewOd.y+j*200);
					}

					// Look at ID
					ptNewId = m_ptExist[i][k][0];
					if (ptNewId.y > 0)
					{
						dc.SelectObject(&penId);
						dc.MoveTo(ptNewId.x,j*200);
						dc.LineTo(ptNewId.x, ptNewId.y+j*200);
					}

				}   //  End k loop
			}
			else
			{
				dc.SelectObject(&penId);

				if (m_nNumExistWall[i] > 900)
					m_nNumExistWall[i] = 900;

				for (k=1; k<m_nNumExistWall[i]; k++)
					if ( (m_ptExist[i][k-1][0].x>=0) && (m_ptExist[i][k][0].x>=0) )
					{
						dc.MoveTo(m_ptExist[i][k-1][0].x, m_ptExist[i][k-1][0].y*100/(WallRange/2)+100+j*200);           
						dc.LineTo(m_ptExist[i][k][0].x, m_ptExist[i][k][0].y*100/(WallRange/2)+100+j*200);
					}
			}

			j++;
		}
	}

    dc.SelectObject(penOld);

}

void CChnlTraceDlg::EraseExistingPts()
{
	CPoint ptOd;
	ptOd.x = 0;
	ptOd.y = 100;

	for (int i=0; i<40; i++)
	{
		for (int j=0; j<900; j++)
		{
			m_ptExist[i][j][0] = (0, 0);
			m_ptExist[i][j][1] = ptOd;
		}

		m_nNumExistWall[i] = 0;
	}
}


void CChnlTraceDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	CMenu Menu;
	CPoint pt;
	CRect rect;

	// Preserve the system menu properties
	pt = point;
	GetClientRect(&rect);
	ScreenToClient(&pt);

	// Was the mouse clicked on the client area or system area?
	if ( rect.PtInRect (pt) )
	{	// client area
		Menu.LoadMenu(IDR_MENU_CHNLRANGE);

		CMenu *pContextMenu = Menu.GetSubMenu(0);
		int nCmd = pContextMenu->TrackPopupMenu( TPM_LEFTALIGN |
						TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
						point.x, point.y, this, NULL);
		
		CChnlRangeDlg dlg;

		switch (nCmd)
		{
		case ID_MENU_SETRANGE:
			if (dlg.DoModal() == IDOK)
			{
				for (int i=0; i<MEM_MAX_CHANNEL; i++)
				{
					m_bChnlSelect[i] = dlg.m_bChnlSelect[i];
				}
				EraseTraces();
				DrawExistingTraces();
			}
			break;

		case ID_MENU_PRINTTRACES:
			PrintChnlTraces();
			break;

		default:
			return;
			break;
		}
	}
	else
		CWnd::OnContextMenu(pWnd,point);	// system area
}

void CChnlTraceDlg::SaveWindowPosition(LPCTSTR lpszProfileName)
{
    // place your SaveState or GlobalSaveState call in
    // CMainFrame's OnClose() or DestroyWindow(), not in OnDestroy()
    ASSERT_VALID(this);
    ASSERT(GetSafeHwnd());

    CWinApp* pApp = AfxGetApp();

	CRect rect;
	GetWindowRect(&rect);

    pApp->WriteProfileInt(lpszProfileName, _T("Left"), rect.left);
    pApp->WriteProfileInt(lpszProfileName, _T("Top"), rect.top);
    pApp->WriteProfileInt(lpszProfileName, _T("Width"), rect.Width());
	pApp->WriteProfileInt(lpszProfileName, _T("Height"), rect.Height());
}

void CChnlTraceDlg::RestoreWindowPosition(LPCTSTR lpszProfileName)
{
    // place your SaveState or GlobalSaveState call in
    // CMainFrame's OnClose() or DestroyWindow(), not in OnDestroy()
    ASSERT_VALID(this);
    ASSERT(GetSafeHwnd());

    CWinApp* pApp = AfxGetApp();

	CRect rect;
	GetWindowRect(&rect);
	int x = rect.left;
	int y = rect.top;
	int cx, cy, width, height;

    cx = pApp->GetProfileInt(lpszProfileName, _T("Left"), x);
    cy = pApp->GetProfileInt(lpszProfileName, _T("Top"), y);
    width = pApp->GetProfileInt(lpszProfileName, _T("Width"), rect.Width());
    height = pApp->GetProfileInt(lpszProfileName, _T("Height"), rect.Height());

	MoveWindow(cx, cy, width, height);
}

BOOL CChnlTraceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	RestoreWindowPosition(_T("Channel Traces"));

	EraseTraces();
	SetTimer(DRAW_TIMER, 330, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChnlTraceDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if ( nIDEvent == REFRESH_WINDOW )
	{
		KillTimer(REFRESH_WINDOW);
		EraseTraces();
		DrawExistingTraces();
		CDialog::OnTimer(nIDEvent);
		TRACE("REFRESH_WINDOW\n");
		return;
	}

    // TODO: Add your message handler code here and/or call default

    // copy inspection data from g_pListUtData to plistUtdata
    I_MSG_RUN *pImsgRun;

	int nPipeGoneCnt = 0;
    g_CriticalSectionChnls.Lock();
    while (!g_pTcpListUtDataChnls.IsEmpty() )
    {
        pImsgRun = (I_MSG_RUN *) g_pTcpListUtDataChnls.RemoveHead();
		if (nPipeGoneCnt && (pImsgRun->InspHdr.wLineStatus & PIPE_PRESENT) )
		{
			g_pTcpListUtDataChnls.AddHead((void *) pImsgRun);
			break;
		}
		m_plistUtData.AddTail((void *) pImsgRun);
		if ( (pImsgRun->InspHdr.wLineStatus & PIPE_PRESENT) == 0 )
			nPipeGoneCnt++;
    }
    g_CriticalSectionChnls.Unlock();

    if ( m_plistUtData.IsEmpty() ) 
	{
		CDialog::OnTimer(nIDEvent);
		return ;
	}

	DrawTraces();

    while (!m_plistUtData.IsEmpty() )
    {
        pImsgRun = (I_MSG_RUN *)m_plistUtData.RemoveHead();
		delete pImsgRun;
    }

	TRACE("OnTimer\n");
	CDialog::OnTimer(nIDEvent);
}


void CChnlTraceDlg::PrintChnlTraces() 
{
	// TODO: Add your control notification handler code here
	CDC dc,*pDC;
    CRect DrawRect(0,0,0,0);
    CRect CalRect(0,0,1000,200);  // where to draw the graph
    CRect MapRect(0,0,1000,400);  
    CTime   theTime = CTime::GetCurrentTime();
    CFont oFont,*pFont;
    LOGFONT lfFont;
    CPoint  returnLocation;
    CSize size;
    CPen  pen(PS_SOLID, 0, RGB(0,0,0));  // BLACK
    int nHeight,RowHeight_h;
    int temp;
    int channel = gChannel;
    int Xoffset = 100;
    int Yoffset = 20;
	TEXTMETRIC tm;					// struct which hold characteristics of screen
    BOOL NewPage = FALSE;
	int cxChar, cyChar;				// char size in pixels
	CString ss;

	CPrintDialog dlgp(FALSE);
	dlgp.GetDefaults();		//dlg.GetPrinterDC();
	dc.Attach(dlgp.GetPrinterDC());
	pDC = &dc;

	if ( *pDC == NULL )
	{
		AfxMessageBox(_T("No Default Printer Available, Please Setup a Default Printer"));	
			return;  // no printer or print cancelled
	}

    DOCINFO di;   // setup DOCINFO structure
    ::ZeroMemory(&di, sizeof(DOCINFO));
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = _T("Flaw Map");  // need date & wo info here ????
	int pageWidth = dc.GetDeviceCaps(HORZRES);
    int pageHeight = dc.GetDeviceCaps(VERTRES);
	int color = dc.GetDeviceCaps(NUMCOLORS);

	nHeight = -((pDC->GetDeviceCaps(LOGPIXELSY))/72);

    pFont = pDC->GetCurrentFont();
    pFont->GetLogFont( &lfFont );
    RowHeight_h = (int)(1.1 * abs(lfFont.lfHeight));
    CPoint print_origin(Xoffset,Yoffset);
    BOOL bContinue = TRUE;
	pDC->GetTextMetrics (&tm);	// put after setting viewport/window ext
	//xChar = tm.tmAveCharWidth;
	cyChar = 3*(tm.tmHeight + tm.tmExternalLeading)/4;
//	yChar = tm.tmHeight;
	cxChar = 3*tm.tmAveCharWidth/2;
	print_origin.x = pageWidth/20;
	print_origin.y = pageHeight/20 ;

    if ( pDC->StartDoc(&di) > 0 )
	{ 
	    pDC->StartPage();

	    pDC->SetMapMode(MM_TEXT );
	    pDC->SetTextAlign(TA_LEFT);

	    ss.Format(_T("TRUSCAN Inspection Channel Traces"));

	    temp = CViewcfg::PrintHeading(pDC, print_origin, pageWidth, nHeight, 
						ss, "");

        print_origin.y = temp;

		int i, j, k;
		CPoint ptNewOd, ptNewId;    // new  values for Sonoscope id/od
		CPen *penOld;
		CPen penId, penOd, BlackPen, LightGreenPen;

		short WallRange = (short) (20.0f * ConfigRec.JobRec.Wall * 
								(float) MAX_DELTA_WALL);

		int nChnlCnt = 0;
		for (i=0; i<40; i++)
			if (m_bChnlSelect[i])
				nChnlCnt++;

		// Get the client rectangle boundaries for the Image Map graph (Button)
		CRect Rect;

        Rect.SetRect(0,0, pageWidth*9/10, (int)(-pageHeight*3.f/4.f) );
        Rect.NormalizeRect();

		// Move the coordinate origin to the lower left corner
		// Set mapping mode to isotropic, deal in logical units rather than pixels
		dc.SetMapMode(MM_ANISOTROPIC);
		dc.SetWindowExt(m_nScale, nChnlCnt*200);	// Define size of window in logical units
		// m_iTraceLength on x axis, 0-m_sMaxYValue on y axis
		// Tell windows how big this box is physically in pixels
		int wallorg	= (int)(pageHeight*0.95f);
		pDC->SetViewportOrg( print_origin.x,wallorg);
		pDC->SetViewportExt( Rect.Width(), -Rect.Height());

		penOd.CreatePen(PS_SOLID, 2, RGB(0,   0,   255));
		penId.CreatePen(PS_SOLID, 2, RGB(255,   0,   0));
		BlackPen.CreatePen(PS_SOLID, 2, RGB(0,   0,   0));
		LightGreenPen.CreatePen(PS_SOLID, 0, RGB(0,   255,   0));

		penOld = dc.SelectObject(&BlackPen);    // OD is blue

		//Draw frame
		dc.MoveTo(0, 0);
		dc.LineTo(m_nScale, 0);
		dc.LineTo(m_nScale, nChnlCnt*200);
		dc.LineTo(0, nChnlCnt*200);
		dc.LineTo(0, 0);

		for(i=1; i<nChnlCnt; i++)
		{
			dc.MoveTo(0, i*200);
			dc.LineTo(m_nScale, i*200);
		}
		dc.SelectObject (&LightGreenPen);
		for(i=1; i<=nChnlCnt; i++)
		{
			dc.MoveTo(0, i*200-100);
			dc.LineTo(m_nScale, i*200-100);
		}

		CFont mFont;
		mFont.CreatePointFont(Rect.Height()/100,_T("Arial"), &dc);
		dc.SetBkMode(TRANSPARENT);
		dc.SelectObject(&mFont);
		CString s;
		CPen penIdT, penOdT;
		penOdT.CreatePen(PS_DOT, 0, RGB(0,   0,   255));
		penIdT.CreatePen(PS_DOT, 0, RGB(255,   0,   0));
		j = 0;
		for (i=0; i<40; i++)
		{
			if (m_bChnlSelect[i])
			{
				int nID, nOD;

				if (nChnlCnt <= 10)  //draw threshold lines
				{
					switch (ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Type)
					{
					case IS_WALL:
						nID = ConfigRec.UtRec.TholdWall[0] * 100 / MAX_DELTA_WALL + 100 + j*200;
						nOD = ConfigRec.UtRec.TholdWall[1] * 100 / MAX_DELTA_WALL + 100 + j*200;
						break;

					case IS_LONG:
						nID = ConfigRec.UtRec.TholdLong[0][0] + j*200;
						nOD = ConfigRec.UtRec.TholdLong[0][1] + 100 + j*200;
						break;

					case IS_TRAN:
						nID = ConfigRec.UtRec.TholdLong[1][0] + j*200;
						nOD = ConfigRec.UtRec.TholdLong[1][1] + 100 + j*200;
						break;

					case IS_OBQ1:
						nID = ConfigRec.UtRec.TholdLong[2][0] + j*200;
						nOD = ConfigRec.UtRec.TholdLong[2][1] + 100 + j*200;
						break;

					case IS_OBQ2:
						nID = ConfigRec.UtRec.TholdLong[3][0] + j*200;
						nOD = ConfigRec.UtRec.TholdLong[3][1] + 100 + j*200;
						break;

					case IS_OBQ3:
						nID = ConfigRec.UtRec.TholdLong[4][0] + j*200;
						nOD = ConfigRec.UtRec.TholdLong[4][1] + 100 + j*200;
						break;

					default:
						nID = 50 + j*200;
						nOD = 50 + 100 + j*200;
						break;
					}
				}
				dc.SelectObject (&penIdT);
				dc.MoveTo(0, nID);
				dc.LineTo(m_nScale, nID);
				dc.SelectObject (&penOdT);
				dc.MoveTo(0, nOD);
				dc.LineTo(m_nScale, nOD);

				int nType, n;

				nType = ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Type ;

				switch(nType)
				{
					case IS_NOTHING:
					default:		n = 0;		break;

					case IS_WALL:	n = 1;		break;

					case IS_LONG:	n = 2;		break;

					case IS_TRAN:	n = 3;		break;

					case IS_OBQ1:	n = 4;		break;
					case IS_OBQ2:	n = 5;		break;
					case IS_OBQ3:	n = 6;		break;
					case IS_LAM:		n = 7;				break;		

				}

				s.Format(_T("%2d-%s"), i+1, XdcrText[n]);
				dc.TextOut(1, 140+200*j, s);

				j++;
			}
		}

		//draw x ticks
		int m;
		float fmeter;
		CPen penTick;
		penTick.CreatePen(PS_DOT, 1, RGB(0,   255,   0));
		BYTE bMetric;
		if (ConfigRec.bEnglishMetric == ENGLISH)
			bMetric = 0;
		else
			bMetric = 1;
		dc.SelectObject (&penTick);
		switch (bMetric)
		{
		  case 0:
			// English
			// draw ticks and number every 10 ft., ie., 10, 20, 30 etc
			for ( i = 120; i < m_nScale; i += 120)
			{
				dc.MoveTo(i,90);
				dc.LineTo(i,110);
				dc.MoveTo(i,90+(nChnlCnt-1)*200);
				dc.LineTo(i,110+(nChnlCnt-1)*200);
			}
			// draw 2 ft marks
			for ( i = 24; i < m_nScale; i += 24)
			{
				dc.MoveTo(i,95);
				dc.LineTo(i,105);
				dc.MoveTo(i,95+(nChnlCnt-1)*200);
				dc.LineTo(i,105+(nChnlCnt-1)*200);
			}
			break;

		  case 1:
			// Metric
			// draw ticks and number every 5 meters ... still 50 or 75 ft scale
			fmeter = 5.0f;
			m = 5;
			i = (int) (fmeter * 39.37f  + 0.5f);
			while ( i < m_nScale)
			{
				dc.MoveTo(i,90);
				dc.LineTo(i,110);
				dc.MoveTo(i,90+(nChnlCnt-1)*200);
				dc.LineTo(i,110+(nChnlCnt-1)*200);
				fmeter += 5.0f;
				m += 5;
				i = (int) (fmeter * 39.37f  + 0.5f);
			}

			// draw 1 meter marks
			fmeter = 1.0f;
			m = 1;
			i = (int) (fmeter * 39.37f  + 0.5f);
			while ( i < m_nScale)
			{
				dc.MoveTo(i,95);
				dc.LineTo(i,105);
				dc.MoveTo(i,95+(nChnlCnt-1)*200);
				dc.LineTo(i,105+(nChnlCnt-1)*200);
				fmeter += 1.0f;
				m += 1;
				i = (int) (fmeter * 39.37f  + 0.5f);
			}
			break;

		  default:
			break;
		}  //end switch

		//draw traces
		j = 0;
		for (i=0; i<40; i++)  //channel loop
		{
			if (m_bChnlSelect[i])
			{
				if (ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Type != IS_WALL)
				{
					for (k=0; k<m_nScale; k++)
					{
						// Look at OD
						ptNewOd = m_ptExist[i][k][1];
						if (ptNewOd.y > 100)
						{
							dc.SelectObject(&penOd);
							dc.MoveTo(ptNewOd.x,100+j*200);           
							dc.LineTo(ptNewOd.x, ptNewOd.y+j*200);
						}

						// Look at ID
						ptNewId = m_ptExist[i][k][0];
						if (ptNewId.y > 0)
						{
							dc.SelectObject(&penId);
							dc.MoveTo(ptNewId.x,j*200);
							dc.LineTo(ptNewId.x, ptNewId.y+j*200);
						}

					}   //  End k loop
				}
				else
				{
					dc.SelectObject(&penId);

					if (m_nNumExistWall[i] > 900)
						m_nNumExistWall[i] = 900;

					for (k=1; k<m_nNumExistWall[i]; k++)
						if ( (m_ptExist[i][k-1][0].x>=0) && (m_ptExist[i][k][0].x>=0) )
						{
							dc.MoveTo(m_ptExist[i][k-1][0].x, m_ptExist[i][k-1][0].y*100/(WallRange/2)+100+j*200);           
							dc.LineTo(m_ptExist[i][k][0].x, m_ptExist[i][k][0].y*100/(WallRange/2)+100+j*200);
						}
				}

				j++;
			}
		}

		dc.SelectObject(penOld);

        pDC->EndPage();
	}

	pDC->EndDoc();
	dc.Detach();
}	
