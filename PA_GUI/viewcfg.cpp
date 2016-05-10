// CViewcfg.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
// include only the parent dialog header asidlg.h for all child dialogs
#include "TscanDlg.h"
#include "Extern.h"

// declared in Lang.h

extern char *recv_filter[];
extern char *recv_detector[];
extern char *st_na, *st_high, *st_low;
extern char *st_On, *st_Off ,*st_ON, *st_OFF, *ama_logic[] ;
extern char *tcg_trigger[];
extern char *tcg_step[];
extern char *tcg_fn[];
extern char *tof_start_g1[];
extern char *tof_start_g2[];
extern char *tof_start_g1_print[];
extern char *tof_stop_g1[];
extern char *tof_stop_g2[];
extern char *tof_stop_g1_print[];
extern char *gate_trigger_g1[];

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCViewcfg dialog


CViewcfg::CViewcfg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewcfg::IDD, pParent)
{
	int m_Page = 0;
	//{{AFX_DATA_INIT(CCViewcfg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CViewcfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewcfg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewcfg, CDialog)
	//{{AFX_MSG_MAP(CViewcfg)
	ON_BN_CLICKED(IDC_CFG_NEXT, OnCfgNext)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CFG_PREVIOUS, OnCfgPrevious)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewcfg message handlers

BOOL CViewcfg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	
	GetWindowPlacement(&wp);
	dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	pCTscanDlg->GetWindowLastPosition(_T("ConfigurationDisp"), &rect);
	if ( ( (rect.right - rect.left) >= dx ) &&
		( (rect.bottom - rect.top) >= dy ))
	{
		wp.rcNormalPosition = rect;
		SetWindowPlacement(&wp);
	}
	m_Page = 0;	
	GetDlgItem(IDC_CFG_NEXT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CFG_PREVIOUS)->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**************************************************************************

Description:
	Make a string of 1 and 0 from the input word 

****************************************************************************/

CString BitString(BYTE length, WORD input, CString &result)
{
	result = _T("");
	for (int i=0; i<length; i++)
	{
		if ( input & (1 << i) )
			result += _T("1");
		else
			result += _T("0");
	}
	return result;
}

/**************************************************************************/
int CViewcfg::GetChnlTypeIndex(int Chnl)
{
	// Given the chnl number selected, return the index of the chnl type
	BYTE bType;
	
	bType = ConfigRec.UtRec.Shoe[Chnl/10].Ch[Chnl % 10].Type;
	switch (bType)
	{
	case IS_NOTHING:
	default:
		return NO_TYPE;
		
	case IS_WALL:
		return WALL_TYPE;

	case IS_LONG:
		return LONG_TYPE;
		
	case IS_TRAN:
		return TRAN_TYPE;
		
	case IS_OBQ1:
		return OBQ1_TYPE;
		
	case IS_OBQ2:
		return OBQ2_TYPE;
		
	case IS_OBQ3:
		return OBQ3_TYPE;
	}
}

void CViewcfg::OnOK() 
{
	
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition(_T("ConfigurationDisp"), &wp);
	CDialog::OnOK();
	CDialog::DestroyWindow();
}

void CViewcfg::OnCancel() 
{
	CDialog::OnCancel();
	CDialog::DestroyWindow();
}

CViewcfg::~CViewcfg()
{

}

void CViewcfg::PostNcDestroy() 
{
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
}

void CViewcfg::OnPaint() 
{

	CPaintDC dc1(this); // device context for painting
	CPoint returnPoint;
	CPoint point1(50,20);  // where to draw rect1 on screen
	CPoint point2(340,20);  // where to draw rect 2 on screen
	CPoint point3(50,20);  // where to draw rect 3 on secreen
	CRect DrawRect1(-20,-5,250,600);  // this where the first box get drawn (Channel independant items)
	CRect DrawRect2(-20,-5,580,600);  // channel dependent items
	CRect DrawRect3(-20,-5,870,600);  // sensor table and demux table
	DrawRect1.NormalizeRect();
	DrawRect2.NormalizeRect();
	DrawRect3.NormalizeRect();
	
	CDC cdc1;
	cdc1.Attach(dc1);
	
	switch (m_Page)
	{
	case 0:
		returnPoint = MakeCfgTableHead(&cdc1, point1, gChannel, DrawRect1, SCREEN);
		point1.x = returnPoint.x + 170;
		returnPoint = MakeCfgTable( &cdc1, point2, gChannel, DrawRect2, SCREEN);
		break;
	case 1:
		returnPoint = MakeCfgTableSensor(&cdc1, point1, DrawRect3, SCREEN);
		break;
	case 2:
		returnPoint = MakeCfgTableDmx(&cdc1, point3, gChannel, DrawRect3, SCREEN);
		break;
	default:
		// no action
		;
	}
	// Do not call CDialog::OnPaint() for painting messages
}

void CViewcfg::OnCfgNext() 
{
	m_Page++;
	if ( m_Page == 3 )
		m_Page = 0;

	GetDlgItem(IDC_CFG_PREVIOUS)->EnableWindow(TRUE);
	if ( m_Page == 2)
	{
		GetDlgItem(IDC_CFG_NEXT)->EnableWindow(FALSE);
	}
	Invalidate();
}

void CViewcfg::OnCfgPrevious() 
{
	m_Page--;
	if ( m_Page == -1 )
		m_Page = 0;

	GetDlgItem(IDC_CFG_NEXT)->EnableWindow(TRUE);
	if ( m_Page == 0)
	{
		GetDlgItem(IDC_CFG_PREVIOUS)->EnableWindow(FALSE);
	}
	Invalidate();
	
}

/******************************************************************/

// return row position where next text can appear
// Title
// Date, Time
// Joint
// Operator
// Heat number
// Customer

//	ConfigRec.JobRec.BusUnit;
//	ConfigRec.JobRec.Comment
//	ConfigRec.JobRec.Customer
//	ConfigRec.JobRec.Date
//	ConfigRec.JobRec.Location
//	ConfigRec.JobRec.Grade
//	ConfigRec.JobRec.Operator
//	ConfigRec.JobRec.WellFoot
//	ConfigRec.JobRec.WellName
//	ConfigRec.JobRec.WO

int CViewcfg::PrintHeading(CDC *pDC, CPoint print_origin, int pageWidth, int height,
						CString title, CString comment)
{
	CFont   oFont,oHeadFont; // font to use
	CSize   oFontSize;      // size of the font
	LOGFONT	lfFont;         // font characteristic structure
	CString s;              // general purpose string used to format output
	CTime   theTime = CTime::GetCurrentTime();
	CSize	size;
	int		vpos = 0;       // vertical position
	double  TextHeight;     // height of curreent font + row spacing
	double  hpos = 0;       // horizontal position
	int     iRow = 0;       // row number
	int		hori_offset2;   // column 2
	int		width;
    int pageHeight = pDC->GetDeviceCaps(VERTRES);

	width = pageWidth * 90 / 100;
	hori_offset2 = width/2;
	    oFont.CreateFont((int)(9* (height)),0,0,0,FW_NORMAL,0,0,0,
        DEFAULT_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,
        DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE,  _T("Arial"));

//    oFont.CreatePointFont(120,"Arial", pDC);
	oFont.GetLogFont(&lfFont);
	lfFont.lfWeight = 700;
	TextHeight = lfFont.lfHeight * -1.1;  // font height + 10%
	oHeadFont.CreateFontIndirect(&lfFont);
	pDC->SelectObject(&oHeadFont);
	vpos = (int)(print_origin.y+iRow*TextHeight);
	pDC->TextOut(print_origin.x,+vpos,title );              // input param string 1
	s = theTime.Format( " %A, %B %d, %Y, %I:%M %p" );
	size = pDC->GetOutputTextExtent(s);
	pDC->TextOut(pageWidth-(size.cx+print_origin.x), print_origin.y, s );  // date & time
	pDC->SelectObject(&oFont);
	iRow++;
	iRow++;
	vpos = (int)(print_origin.y+iRow*TextHeight);

    s.Format(_T("OD:%8.5g %s"), ConfigRec.JobRec.OD * SCALE, TXTSCALE );  // OD
	pDC->TextOut(print_origin.x,+vpos, s );
	s.Format(_T("Customer: %s"), ConfigRec.JobRec.Customer);   // Customer
	pDC->TextOut(hori_offset2,+vpos, s );
	iRow++;
	vpos = (int)(print_origin.y+iRow*TextHeight);

	s.Format(_T("Wall Thickness: %5.4g %s"), ConfigRec.JobRec.Wall * SCALE, TXTSCALE);  // Wall
	pDC->TextOut(print_origin.x,+vpos, s );
	s.Format(_T("Operator: %s"), ConfigRec.JobRec.Operator);   // operator
	pDC->TextOut(hori_offset2,+vpos, s );
	iRow++;
	vpos = (int)(print_origin.y+iRow*TextHeight);

	s.Format(_T("Work Order: %s"), ConfigRec.JobRec.WO);        // Work Order
	pDC->TextOut(print_origin.x,+vpos, s );
	s.Format(_T("Business Unit: %s"), ConfigRec.JobRec.BusUnit); // Buss. unit
	pDC->TextOut(hori_offset2,+vpos, s );
	iRow++;
	vpos = (int)(print_origin.y+iRow*TextHeight);

	s.Format(_T("Grade: %s"), ConfigRec.JobRec.Grade);	       // Grade
	pDC->TextOut(print_origin.x,+vpos, s );
	s.Format(_T("Lot Number: %s"), ConfigRec.JobRec.LotNum);  // Lot 
	pDC->TextOut(hori_offset2,+vpos, s );
	iRow++;
	vpos = (int)(print_origin.y+iRow*TextHeight);

	s.Format(_T("Heat: %s"), ConfigRec.JobRec.Heat);           // heat
	pDC->TextOut(print_origin.x,+vpos, s );
	s.Format(_T("Joint Number: %d"), pCTscanDlg->m_uJntNum);  // joint
	pDC->TextOut(hori_offset2,+vpos, s );
	iRow++;
	vpos = (int)(print_origin.y+iRow*TextHeight);

	s.Format(_T("Location: %s"), ConfigRec.JobRec.Location);  // Location
	pDC->TextOut(print_origin.x,+vpos, s );
	if ( pCTscanDlg->m_pInspectDlg1 )
		s.Format(_T("Inspection Time:  %s"),pCTscanDlg->m_pInspectDlg1->m_tTime );
	else
		s.Format(_T("Inspection Time: ") );
	pDC->TextOut(hori_offset2,+vpos, s);
	iRow++;
	vpos = (int)(print_origin.y+iRow*TextHeight);

	s.Format(_T("RB Number: %s"), ConfigRec.JobRec.RbNum);    // Comment
	pDC->TextOut(print_origin.x,+vpos, s );
	if ( pCTscanDlg->m_pInspectDlg1 )
		s.Format(_T("Replay Record: %s "),pCTscanDlg->m_pInspectDlg1->m_DataPathName);
	else
		s.Format(_T("Replay Record: "));
    pDC->TextOut(hori_offset2,+vpos, s );
	iRow++;
	vpos = (int)(print_origin.y+iRow*TextHeight);

    s.Format(_T("Configuration File: %s"),ConfigRec.JobRec.CfgFile); // config rec
	pDC->TextOut(print_origin.x,+vpos, s );
	iRow++;
	vpos = (int)(print_origin.y+iRow*TextHeight);

	pDC->TextOut(print_origin.x,+vpos, comment );                // string comment
	iRow++;
	vpos = (int)(print_origin.y+iRow*TextHeight);             

	return (int)(iRow*TextHeight);
}

CPoint CViewcfg::MakeCfgTableSensor(CDC *pdc, CPoint printOrigin, CRect Rect, int device)
{
	int hpos1,hpos[MAX_SHOES];   // Display,NC/NX,Thold, cal
	int rpos,rpos1,rpos2;
	int RowHeight;
	double sense;
	CString s,t;
	CFont *pFont, oFont,oBoldFont;
	LOGFONT  lfFont;
	CSize stringSize;
	CPoint returnPoint = printOrigin;
	int i, xdcr;

	if ( device != PRINTER )
	{
		CBrush brush(RGB(128,128,128));
		Rect += printOrigin;
		//pdc->FillRect(&Rect,&CBrush(pdc->GetBkColor()));
		//pdc->FillRect(&Rect,&brush);
		//pdc->DrawEdge(&Rect,EDGE_BUMP,BF_RECT  );
		hpos1  = printOrigin.x;
		for (i = 0; i < MAX_SHOES; i++)
			hpos[i] = printOrigin.x+200+i*100;
		rpos  = printOrigin.y;
		sense = 1.0;
	}
	else
	{
		hpos1  = printOrigin.x;
		for (  i = 0; i < MAX_SHOES; i++)
			hpos[i] = printOrigin.x+300+i*400;
		rpos  = printOrigin.y;
		sense = 1.0;
	}
	pFont = pdc->GetCurrentFont();
	pFont->GetLogFont( &lfFont );
	RowHeight = (int)(sense * 1.0 * abs((lfFont.lfHeight)));
 	lfFont.lfWeight = 700;
	lfFont.lfHeight = lfFont.lfHeight*(long)1.2;
	oBoldFont.CreateFontIndirect(&lfFont);
   
	pdc->SetBkMode(TRANSPARENT);

	pdc->SelectObject(&oBoldFont);
	s.Format(_T("Sensor - related items")); 
	pdc->TextOut(hpos1,rpos,s);
	pdc->SelectObject(pFont);
	rpos += RowHeight;
	rpos += RowHeight;
	s.Format(_T("Disp"));   pdc->TextOut(hpos[0],rpos,s);
	s.Format(_T("NX"));		pdc->TextOut(hpos[1],rpos,s);
	s.Format(_T("Threshold %%"));	pdc->TextOut(hpos[2],rpos,s);
	s.Format(_T("Calib Level"));	pdc->TextOut(hpos[3],rpos,s);

	rpos += RowHeight;
	s.Format(_T("Chan"));   pdc->TextOut(hpos[0],rpos,s);
	s.Format(_T("NC"));		pdc->TextOut(hpos[1],rpos,s);
	s.Format(_T("ID"));	pdc->TextOut(hpos[3],rpos,s);
	s.Format(_T("OD"));	pdc->TextOut(hpos[4],rpos,s);
	//s.Format(_T("Gate 3"));	pdc->TextOut(hpos[5],rpos,s);
	//s.Format(_T("Gate 4"));	pdc->TextOut(hpos[6],rpos,s);


	s.Format(_T("Wall (Max)"));  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	rpos1 = rpos;
	s.Format(_T("Wall (Min)"));  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("Long ID"));  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("Long OD"));  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("Tran ID"));  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("Tran OD"));  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("OBQ1 ID"));  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("OBQ1 OD"));  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("OBQ2 ID"));  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("OBQ2 OD"));  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("LAM"));  	    rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);


	// Display channels	
	rpos2 = rpos1;
	for (  i=0; i < 6; i++ )
	{
		//s.Format((ConfigRec.UtRec.DisplayChannel[i]== 0xff) ? "ALL" :"%d", 
			//ConfigRec.UtRec.DisplayChannel[i]+1);  // disp
		s.Format(_T("ALL"));
		pdc->TextOut(hpos[0],rpos2,s);
		rpos2 += RowHeight*2;
	}

	// NX / NC
	rpos2 = rpos1;
	for ( int surface=0; surface < 2; surface++)  // IF = gate 0, ID= gate 1, OD=gate 2, Wall = gate 3 
	{
		s.Format(_T("%d"), ConfigRec.UtRec.NxWall[0]); // NX [0] = min, [1] = max
		pdc->TextOut(hpos[1],rpos2,s);
		rpos2 += 2*RowHeight;
		s.Format(_T("%d"), ConfigRec.UtRec.NcLong[surface]); // NC
		pdc->TextOut(hpos[1],rpos2,s);
		rpos2 += 2*RowHeight;
		s.Format(_T("%d"), ConfigRec.UtRec.NcTran[surface]); // NC
		pdc->TextOut(hpos[1],rpos2,s);
		rpos2 += 2*RowHeight;
		s.Format(_T("%d"), ConfigRec.UtRec.NcOblq1[surface]); // NC
		pdc->TextOut(hpos[1],rpos2,s);
		rpos2 += 2*RowHeight;
		s.Format(_T("%d"), ConfigRec.UtRec.NcOblq2[surface]); // NC
		pdc->TextOut(hpos[1],rpos2,s);
		rpos2 += 2*RowHeight;
		s.Format(_T("%d"), ConfigRec.UtRec.NcOblq3[surface]); // NC
		pdc->TextOut(hpos[1],rpos2,s);
		rpos2 = rpos1+RowHeight;
	}

	// thresholds
	rpos2 = rpos1;
	s.Format(_T("%d"), ConfigRec.UtRec.TholdWall[1]); // Thold
	pdc->TextOut(hpos[2],rpos2,s);
	rpos2 += RowHeight;
	s.Format(_T("%d"), ConfigRec.UtRec.TholdWall[0]); // Thold
	pdc->TextOut(hpos[2],rpos2,s);
	rpos2 += RowHeight;

	for ( xdcr =0; xdcr < 5; xdcr++)
	{
		s.Format(_T("%d"), ConfigRec.UtRec.TholdLong[xdcr][0]); // Thold
		pdc->TextOut(hpos[2],rpos2,s);
		rpos2 += RowHeight;
		s.Format(_T("%d"), ConfigRec.UtRec.TholdLong[xdcr][1]); // Thold
		pdc->TextOut(hpos[2],rpos2,s);
		rpos2 += RowHeight;
	}

	for ( int gate=0; gate < 2; gate++)
	{
		rpos2 = rpos1+2*RowHeight;
		for (xdcr =0; xdcr < 5; xdcr++)
		{
			s.Format(_T("%d"), ConfigRec.UtRec.bCalLvlL[xdcr][gate]); 
			pdc->TextOut(hpos[gate+3],rpos2,s);
			rpos2 += RowHeight;
			rpos2 += RowHeight;
		}
	}

	rpos += RowHeight;
	rpos += RowHeight;
//	for (int shoe=0; shoe < MAX_SHOES/2; shoe++) 
//	{
//		s.Format(_T("Shoe %d"),shoe+1);		pdc->TextOut(hpos[shoe],rpos,s);
//		s.Format(_T("Shoe %d"),shoe+1+MAX_SHOES/2);		pdc->TextOut(hpos[shoe],rpos+RowHeight*3,s);
//	}

	for ( i = 0; i < MAX_SHOES; i++)
		hpos[i] += 150;

	s.Format(_T("Wall Slope 1-%d"),MAX_SHOES/2);  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	rpos2 = rpos;
	s.Format(_T("Wall Offset"));  	rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);

	s.Format(_T("Wall Slope %d-%d"),MAX_SHOES/2+1,MAX_SHOES);  	; pdc->TextOut(hpos1,rpos2+RowHeight*3,s);
	s.Format(_T("Wall Offset"));  	 pdc->TextOut(hpos1,rpos2+RowHeight*4,s);

	for (int shoe=0; shoe < MAX_SHOES/2; shoe++)
	{
		s.Format(_T("%7.5f"), ConfigRec.UtRec.fWallSlope[shoe]);
		pdc->TextOut(hpos[shoe],rpos2,s);
		s.Format(_T("%d"), ConfigRec.UtRec.WallOffset[shoe]);
		pdc->TextOut(hpos[shoe],rpos2+RowHeight,s);

		s.Format(_T("%7.5f"), ConfigRec.UtRec.fWallSlope[shoe+MAX_SHOES/2]);
		pdc->TextOut(hpos[shoe],rpos2+RowHeight*3,s);
		s.Format(_T("%d"), ConfigRec.UtRec.WallOffset[shoe+MAX_SHOES/2]);
		pdc->TextOut(hpos[shoe],rpos2+RowHeight*4,s);
	}
	rpos += RowHeight*5;
	
	s.Format(_T("Wall drop out")); rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%d ms"), SiteDefault.nWallDropTime); pdc->TextOut(hpos[0],rpos,s);
	s.Format(_T("Wall Thick Ref.")); rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%d"), ConfigRec.UtRec.WallThkRef); pdc->TextOut(hpos[0],rpos,s);
	s.Format(_T("Wall Thin Ref.")); rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%d"), ConfigRec.UtRec.WallThnRef); pdc->TextOut(hpos[0],rpos,s);

	s.Format(_T("Comp Velocity")); rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%6.4f"), ConfigRec.UtRec.CompVelocity); pdc->TextOut(hpos[0],rpos,s);
	s.Format(_T("Shear Velocity")); rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%6.4f"), ConfigRec.UtRec.ShearVelocity); pdc->TextOut(hpos[0],rpos,s);

	s.Format(_T("NC Tolerance")); rpos += RowHeight; pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%d"), ConfigRec.UtRec.Tol ); pdc->TextOut(hpos[0],rpos,s);
	s.Format(_T("MMI Version")); rpos += RowHeight;	pdc->TextOut(hpos1,rpos,s);
	s.Format(TRUSCAN_VERSION ); pdc->TextOut(hpos[0],rpos,s);

	returnPoint.x = hpos[3];
	returnPoint.y = rpos+RowHeight;
	return returnPoint;
}

// pdc = pointer to device context
// channel = starting channel 
// DrawRect = outlines of the drawable area
// next line to print

CPoint CViewcfg::MakeCfgTableHead(CDC *pdc, CPoint printOrigin, int channel, CRect Rect, int device)
{
	int hpos,hpos2,hpos3;
	int rpos;
	int RowHeight;
	double sense;
	CString s,t;
	CFont *pFont, oFont,oBoldFont;
	LOGFONT  lfFont;
	CSize stringSize;
	CPoint returnPoint = printOrigin;

	pFont = pdc->GetCurrentFont();
	pFont->GetLogFont( &lfFont );
	RowHeight = abs((lfFont.lfHeight));

 	lfFont.lfWeight = 700;
 	lfFont.lfHeight = lfFont.lfHeight*(long)1.2;
	oBoldFont.CreateFontIndirect(&lfFont);
 
	pdc->SetBkMode(TRANSPARENT);
	if ( device != PRINTER )
	{
		CBrush brush(RGB(128,128,128));
		Rect += printOrigin;
		//pdc->FillRect(&Rect,&CBrush(pdc->GetBkColor()));
		//pdc->FillRect(&Rect,&brush);
		//pdc->DrawEdge(&Rect,EDGE_BUMP,BF_RECT  );
		hpos  = printOrigin.x;
        hpos2 = printOrigin.x+175;
		hpos3 = printOrigin.x+125;
		rpos  = printOrigin.y;
		sense = 1.0;
	}
	else
	{
		hpos  = printOrigin.x;
		hpos2 = printOrigin.x+pdc->GetOutputTextExtent("Channel dependent items for channel").cx;
		hpos3 = printOrigin.x+pdc->GetOutputTextExtent("Channel dependent").cx;
		rpos  = printOrigin.y;
		sense = 1.0;
	}

	pdc->SelectObject(&oBoldFont);
    s.Format(_T("Channel-Independent Items")); pdc->TextOut(hpos,rpos,s);
	pdc->SelectObject(pFont);
	rpos += RowHeight;
	s.Format(_T("TCG Warn"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	s.Format(_T("%s"), ConfigRec.receiver.tcg_warn ? "On":"Off"); pdc->TextOut(hpos2,rpos,s);
	s.Format(_T("TCG System"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	s.Format(_T("%s"), ConfigRec.receiver.tcg_system ? "On":"Off"); pdc->TextOut(hpos2,rpos,s);
	
	s.Format(_T("Sequence Length"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	s.Format(_T("%d"), ConfigRec.UtRec.sequence_length); pdc->TextOut(hpos2,rpos,s);
    s.Format(_T("Slave A Source"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	s.Format(_T("%d"), ConfigRec.UtRec.trigger_a); pdc->TextOut(hpos2,rpos,s);
    s.Format(_T("Slave B Source"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	s.Format(_T("%d"), ConfigRec.UtRec.trigger_b); pdc->TextOut(hpos2,rpos,s);
	
    s.Format(_T("Pulse Mode"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	s.Format(_T("%s"), ConfigRec.pulser.mode ? "Density":"PRF"); pdc->TextOut(hpos2,rpos,s);
	if ( ConfigRec.pulser.mode ) 
	{
		s.Format(_T("Density"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("0.040\")")); pdc->TextOut(hpos3,rpos,s);
	}
	else
	{
		s.Format(_T("PRF"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%d Hz"), ConfigRec.pulser.prf*10); pdc->TextOut(hpos2,rpos,s);
        s.Format(_T("PRF Source"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	}
    s.Format(_T("Wall Stat Mode"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	switch(ConfigRec.JobRec.WallStatFlag) 
	{
	case 0:
		s.Format(_T("None"));
		break;
	case 1:
		s.Format(_T("Exxon/Mobil"));
		break;
	case 2:
		s.Format(_T("Tuboscope"));
		break;
	case 3:
		s.Format(_T("Invalid"));
	}
	pdc->TextOut(hpos2,rpos,s);
	s.Format(_T("Default Clock Offset"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	s.Format(_T("%d"), SiteDefault.nDefaultClockOffset); pdc->TextOut(hpos2,rpos,s);
	s.Format(_T("Default Away Offset"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	s.Format(_T("%d"), SiteDefault.nDefaultXOffset); pdc->TextOut(hpos2,rpos,s);
	s.Format(_T("Default Toward Offset"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	s.Format(_T("%d"), SiteDefault.nDefaultXOffset2); pdc->TextOut(hpos2,rpos,s);

	returnPoint.x = hpos2+pdc->GetOutputTextExtent("Threshold DetectXXXX").cx;
	returnPoint.y = rpos+RowHeight;
	return returnPoint;
}

// pdc = pointer to device context
// channel = starting channel 
// DrawRect = outlines of the drawable area
// Return next channel to print

CPoint CViewcfg::MakeCfgTable(CDC *pdc, CPoint point, int channel, CRect Rect, int device)
{
	int hpos,hpos1,hpos2,hpos3,hpos4,hpos_center,rpos;
	int RowHeight;
	int LJustify;   /* this is width of "-" */
	double sense;
	CString s,t;
	CFont *pFont, oFont,oBoldFont;
	LOGFONT  lfFont;
	CPoint returnPos;
	float CurveRight;

	if ( device != PRINTER )
	{
		CBrush brush(RGB(128,128,128));
		Rect += point;
		//pdc->FillRect(&Rect,&CBrush(pdc->GetBkColor()));
		//pdc->FillRect(&Rect,&brush);
		//pdc->DrawEdge(&Rect,EDGE_BUMP,BF_RECT  );
		hpos  = point.x;			// left edge of column 1
		hpos1 = point.x+100;			// left edge of gate 1
		hpos2 = hpos1+(hpos1-hpos);		// left edge of gate 2
		hpos3 = hpos2+(hpos1-hpos);		// left edge of gate 3
		hpos4 = hpos3+(hpos1-hpos);		// left edge of gate 4

		hpos_center = hpos2;		// left edge of none gate stuff
		rpos  = point.y;
		sense = 1.0;
	}
	else
	{
		hpos  = point.x;
		hpos1 = point.x+pdc->GetOutputTextExtent("Alarm Polarity").cx;
		hpos2 = hpos1+pdc->GetOutputTextExtent("THRESHOLD DETECT").cx+3;
		hpos3 = hpos2+(hpos2-hpos1);
		hpos4 = hpos3+(hpos2-hpos1);
		hpos_center = hpos2;
		rpos  = point.y;
		sense = 1.0;
	}
	pFont = pdc->GetCurrentFont();
	pFont->GetLogFont( &lfFont );
	RowHeight = (int)(sense * 1.1 * abs(lfFont.lfHeight));
	lfFont.lfWeight = 700;
	lfFont.lfHeight = lfFont.lfHeight*(long)1.2;
	oBoldFont.CreateFontIndirect(&lfFont);

	pdc->SetBkMode(TRANSPARENT);
	LJustify = pdc->GetOutputTextExtent("-").cx;

	for (int chan = channel; chan <= channel; chan++)
	{
		pdc->SelectObject(&oBoldFont);
        s.Format(_T("Channel-Dependent Items for Channel %d [S%d:Ch%d],   ( %s )"),chan+1,(chan)/10+1,(chan)%10+1,
			XdcrText[GetChnlTypeIndex(chan)]); 
		pdc->TextOut(hpos,rpos,s);
		pdc->SelectObject(pFont);
		rpos += RowHeight/2;
		s.Format(_T("Base Channel Gain")); rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%5.1f dB"), ConfigRec.receiver.gain[chan]/10.0f); pdc->TextOut(hpos_center-LJustify,rpos,s);
//		s.Format(_T("RF/FW"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
//		s.Format(_T("%s"), ConfigRec.receiver.det_option[chan] ? "FW":"RF"); pdc->TextOut(hpos_center,rpos,s);
//		s.Format(_T("Polarity"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
//		s.Format(_T("%s"), ConfigRec.receiver.polarity[chan] ? "+" : "-"); pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("Filter"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), recv_filter[ConfigRec.receiver.fil_option[chan]]); pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("PRF Source"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), ConfigRec.pulser.LocalPRF[chan/MAX_CHANNEL_PER_INSTRUMENT] ? "Master" : "Slave"); pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("Offset"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%d"), ConfigRec.receiver.offset[chan]);pdc->TextOut(hpos_center,rpos,s);

		s.Format(_T("TCG"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), ConfigRec.receiver.tcg_trigger[chan]? "On":"Off"); pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("TCG Curve Left"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%4.1f"), ConfigRec.receiver.TcgRec[chan].CurveCoef[0]); pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("TCG Curve 1 Right"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		CurveRight = ConfigRec.receiver.TcgRec[chan].CurveCoef[0] + ConfigRec.receiver.TcgRec[chan].CurveCoef[1] * 247;
		s.Format(_T("%4.1f"), CurveRight); pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("TCG Curve 2 Right"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		if ( ConfigRec.receiver.TcgRec[chan].Curve2On == 0 )
		{
			s.Format(_T("Off")); pdc->TextOut(hpos_center,rpos,s);
		}
		else
		{
			CurveRight = ConfigRec.receiver.TcgRec[chan].Curve2Coef[0] + ConfigRec.receiver.TcgRec[chan].Curve2Coef[1] * 94;
			s.Format(_T("%4.1f"), CurveRight); pdc->TextOut(hpos_center,rpos,s);
		}
		s.Format(_T("TCG Trigger"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), tcg_trigger[ConfigRec.receiver.tcg_trigger[chan]]); pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("TCG Step"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), tcg_step[ConfigRec.receiver.tcg_step[chan]]); pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("X Offset (inch)"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);

		s.Format(_T("%d"), ConfigRec.UtRec.Shoe[chan/10].Ch[chan%10].cXOffset); pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("Angle Offset (Hour)"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%3.1f"), ConfigRec.UtRec.Shoe[chan/10].Ch[chan%10].cWOffset*0.2f); pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("Transmit Sequence [%d]"),ConfigRec.UtRec.Shoe[chan/10].sequence_length);  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), BitString(ConfigRec.UtRec.Shoe[chan/10].sequence_length,
				ConfigRec.UtRec.Shoe[chan/10].Ch[chan%10].Fire_Sequence,s)); pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("Receive Sequence [%d]"),ConfigRec.UtRec.Shoe[chan/10].sequence_length);  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), BitString(ConfigRec.UtRec.Shoe[chan/10].sequence_length,
				 ConfigRec.UtRec.Shoe[chan/10].Ch[chan%10].Rcvr_Sequence,s) ); pdc->TextOut(hpos_center,rpos,s);


//		s.Format(_T("Laminar Alarm"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
//		s.Format(_T("%2d"), ConfigRec.alarm.laminar[chan][0]);pdc->TextOut(hpos1,rpos,s);
//		s.Format(_T("%2d"), ConfigRec.alarm.laminar[chan][1]);pdc->TextOut(hpos2,rpos,s);

		s.Format(_T("Pulser"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s%s%s"), ConfigRec.pulser.pulse_chn_on[chan]&XMT_BITS ? "XMT ":"",
			ConfigRec.pulser.pulse_chn_on[chan]&RCV_BITS ? "RCV ":"",
			ConfigRec.pulser.pulse_chn_on[chan]&PLS_ON_BIT ? "PLS":"");
			pdc->TextOut(hpos_center,rpos,s);
		s.Format(_T("Pulse Width"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%3d ns"), ConfigRec.pulser.pulse_width[chan]*10);pdc->TextOut(hpos_center,rpos,s);
		rpos += (int)(RowHeight*1.5);
		pdc->SelectObject(&oBoldFont);
 		s.Format(_T("Gate 1"));pdc->TextOut(hpos1,rpos,s);
		s.Format(_T("Gate 2"));pdc->TextOut(hpos2,rpos,s);
		s.Format(_T("Gate 3"));pdc->TextOut(hpos3,rpos,s);
		s.Format(_T("Gate 4"));pdc->TextOut(hpos4,rpos,s);
		pdc->SelectObject(pFont);
 
		s.Format(_T("Delay"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		Tprintf(s, ConfigRec.gates.delay[chan][0]/10.0f);pdc->TextOut(hpos1,rpos,s);
		Tprintf(s, ConfigRec.gates.delay[chan][1]/10.0f);pdc->TextOut(hpos2,rpos,s);
		Tprintf(s, ConfigRec.gates.delay[chan][2]/10.0f);pdc->TextOut(hpos3,rpos,s);
		Tprintf(s, ConfigRec.gates.delay[chan][3]/10.0f);pdc->TextOut(hpos4,rpos,s);
		s.Format(_T("Range"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		Tprintf(s, ConfigRec.gates.range[chan][0]/10.0f);pdc->TextOut(hpos1,rpos,s);
		Tprintf(s, ConfigRec.gates.range[chan][1]/10.0f);pdc->TextOut(hpos2,rpos,s);
		Tprintf(s, ConfigRec.gates.range[chan][2]/10.0f);pdc->TextOut(hpos3,rpos,s);
		Tprintf(s, ConfigRec.gates.range[chan][3]/10.0f);pdc->TextOut(hpos4,rpos,s);
		s.Format(_T("Level"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%2d %%"), ConfigRec.gates.level[chan][0]);pdc->TextOut(hpos1,rpos,s);
		s.Format(_T("%2d %%"), ConfigRec.gates.level[chan][1]);pdc->TextOut(hpos2,rpos,s);
		s.Format(_T("%2d %%"), ConfigRec.gates.level[chan][2]);pdc->TextOut(hpos3,rpos,s);
		s.Format(_T("%2d %%"), ConfigRec.gates.level[chan][3]);pdc->TextOut(hpos4,rpos,s);
		s.Format(_T("Blanking"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		Tprintf(s, ConfigRec.gates.blank[chan][0]/10.0f);pdc->TextOut(hpos1,rpos,s);
		Tprintf(s, ConfigRec.gates.blank[chan][1]/10.0f);pdc->TextOut(hpos2,rpos,s);
		Tprintf(s, ConfigRec.gates.blank[chan][2]/10.0f);pdc->TextOut(hpos3,rpos,s);
		Tprintf(s, ConfigRec.gates.blank[chan][3]/10.0f);pdc->TextOut(hpos4,rpos,s);
		s.Format(_T("Trigger"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), gate_trigger_g1[ConfigRec.gates.trg_option[chan][0]]);pdc->TextOut(hpos1,rpos,s);
		s.Format(_T("%s"), gate_trigger_g1[ConfigRec.gates.trg_option[chan][1]]);pdc->TextOut(hpos2,rpos,s);
		s.Format(_T("%s"), gate_trigger_g1[ConfigRec.gates.trg_option[chan][2]]);pdc->TextOut(hpos3,rpos,s);
		s.Format(_T("%s"), gate_trigger_g1[ConfigRec.gates.trg_option[chan][3]]);pdc->TextOut(hpos4,rpos,s);
		s.Format(_T("RF/FW"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
// NOTE the string for rf/fw is recv_detector[]
		s.Format(_T("%s"), ConfigRec.gates.det_option[chan][0] ? "FW":"RF"); pdc->TextOut(hpos1,rpos,s);
		s.Format(_T("%s"), ConfigRec.gates.det_option[chan][1] ? "FW":"RF"); pdc->TextOut(hpos2,rpos,s);
		s.Format(_T("%s"), ConfigRec.gates.det_option[chan][2] ? "FW":"RF"); pdc->TextOut(hpos3,rpos,s);
		s.Format(_T("%s"), ConfigRec.gates.det_option[chan][3] ? "FW":"RF"); pdc->TextOut(hpos4,rpos,s);
		s.Format(_T("Polarity"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), ConfigRec.gates.polarity[chan][0] ? "-" : "+"); pdc->TextOut(hpos1,rpos,s);
		s.Format(_T("%s"), ConfigRec.gates.polarity[chan][1] ? "-" : "+"); pdc->TextOut(hpos2,rpos,s);
		s.Format(_T("%s"), ConfigRec.gates.polarity[chan][2] ? "-" : "+"); pdc->TextOut(hpos3,rpos,s);
		s.Format(_T("%s"), ConfigRec.gates.polarity[chan][3] ? "-" : "+"); pdc->TextOut(hpos4,rpos,s);
		s.Format(_T("Alarm"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), ConfigRec.alarm.trigger[chan][0] ? st_On : st_Off);pdc->TextOut(hpos1,rpos,s);
		s.Format(_T("%s"), ConfigRec.alarm.trigger[chan][1] ? st_On : st_Off);pdc->TextOut(hpos2,rpos,s);
		s.Format(_T("%s"), ConfigRec.alarm.trigger[chan][2] ? st_On : st_Off);pdc->TextOut(hpos3,rpos,s);
		s.Format(_T("%s"), ConfigRec.alarm.trigger[chan][3] ? st_On : st_Off);pdc->TextOut(hpos4,rpos,s);
		s.Format(_T("Alarm Polarity"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), ama_logic[ConfigRec.alarm.polarity[chan][0]]);pdc->TextOut(hpos1,rpos,s);
		s.Format(_T("%s"), ama_logic[ConfigRec.alarm.polarity[chan][1]]);pdc->TextOut(hpos2,rpos,s);
		s.Format(_T("%s"), ama_logic[ConfigRec.alarm.polarity[chan][2]]);pdc->TextOut(hpos3,rpos,s);
		s.Format(_T("%s"), ama_logic[ConfigRec.alarm.polarity[chan][3]]);pdc->TextOut(hpos4,rpos,s);

		s.Format(_T("TOF Start"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), tof_start_g1_print[ConfigRec.timeoff.trigger[chan][0]]);pdc->TextOut(hpos1,rpos,s);
		s.Format(_T("%s"), tof_start_g1_print[ConfigRec.timeoff.trigger[chan][1]]);pdc->TextOut(hpos2,rpos,s);
		s.Format(_T("%s"), tof_start_g1_print[ConfigRec.timeoff.trigger[chan][2]]);pdc->TextOut(hpos3,rpos,s);
		s.Format(_T("%s"), tof_start_g1_print[ConfigRec.timeoff.trigger[chan][3]]);pdc->TextOut(hpos4,rpos,s);
		s.Format(_T("TOF Stop"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		if ( ConfigRec.timeoff.trigger[chan][0] == 0 )
			{ s.Format(_T("----"));pdc->TextOut(hpos1,rpos,s); }
		else
			{ s.Format(_T("%s"), tof_stop_g1_print[ConfigRec.timeoff.stopon[chan][0]]);pdc->TextOut(hpos1,rpos,s); }
		if ( ConfigRec.timeoff.trigger[chan][1] == 0 )
			{ s.Format(_T("----"));pdc->TextOut(hpos2,rpos,s); }
		else
			{ s.Format(_T("%s"), tof_stop_g1_print[ConfigRec.timeoff.stopon[chan][1]]);pdc->TextOut(hpos2,rpos,s); }
		if ( ConfigRec.timeoff.trigger[chan][2] == 0 )
			{ s.Format(_T("----"));pdc->TextOut(hpos3,rpos,s); }
		else
			{ s.Format(_T("%s"), tof_stop_g1_print[ConfigRec.timeoff.stopon[chan][2]]);pdc->TextOut(hpos3,rpos,s); }
		if ( ConfigRec.timeoff.trigger[chan][3] == 0 )
			{ s.Format(_T("----"));pdc->TextOut(hpos4,rpos,s); }
		else
			{ s.Format(_T("%s"), tof_stop_g1_print[ConfigRec.timeoff.stopon[chan][3]]);pdc->TextOut(hpos4,rpos,s); }
		s.Format(_T("Gate TCG"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("--State"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%s"), ConfigRec.receiver.TcgRec[chan].GateOn[0] ? "On":"Off");pdc->TextOut(hpos1,rpos,s);
		s.Format(_T("%s"), ConfigRec.receiver.TcgRec[chan].GateOn[1] ? "On":"Off");pdc->TextOut(hpos2,rpos,s);
		s.Format(_T("%s"), ConfigRec.receiver.TcgRec[chan].GateOn[2] ? "On":"Off");pdc->TextOut(hpos3,rpos,s);
		s.Format(_T("%s"), ConfigRec.receiver.TcgRec[chan].GateOn[3] ? "On":"Off");pdc->TextOut(hpos4,rpos,s);
		s.Format(_T("--Gain"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
		s.Format(_T("%5.1f dB"), ConfigRec.receiver.TcgRec[chan].GateGain[0]-20.0f);pdc->TextOut(hpos1-LJustify,rpos,s);
		s.Format(_T("%5.1f dB"), ConfigRec.receiver.TcgRec[chan].GateGain[1]-20.0f);pdc->TextOut(hpos2-LJustify,rpos,s);
		s.Format(_T("%5.1f dB"), ConfigRec.receiver.TcgRec[chan].GateGain[2]-20.0f);pdc->TextOut(hpos3-LJustify,rpos,s);
		s.Format(_T("%5.1f dB"), ConfigRec.receiver.TcgRec[chan].GateGain[3]-20.0f);pdc->TextOut(hpos4-LJustify,rpos,s);


	}
	returnPos.y = rpos+2*RowHeight;
	returnPos.x = hpos4+pdc->GetOutputTextExtent("Threshold DetectXXXX").cx;
	return returnPos;
}

void CViewcfg::OnFilePrintcfg() 
{
	CDC dc,*pDC;
	CRect DrawRect(0,0,0,0);
	CPrintDialog dlg(FALSE);
	CString s;
	CTime   theTime = CTime::GetCurrentTime();
	CFont oFont,hFont,*pFont;
	LOGFONT lfFont;
	CPoint  returnLocation;
	CSize size;

	int nPageCount, page;
	int pageWidth,pageHeight;
	int RowHeight,RowHeight_h;
	int nHeight;
	int hsize;
	int temp;
	int channel = 0;
	int Xoffset;
	int Yoffset;
	BOOL NewPage = FALSE;

	dlg.GetDefaults();		//dlg.GetPrinterDC();
	dc.Attach(dlg.GetPrinterDC());
	pDC = &dc;

	if ( *pDC == NULL )
	{
		AfxMessageBox(_T("No Default Printer Available, Please Setup a Default Printer."));	
		return;  // no printer or print cancelled
	}

	DOCINFO di;   // setup DOCINFO structure
	::ZeroMemory(&di, sizeof(DOCINFO));
	di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = _T("Config Table");  // need date & wo info here ????
	
	hFont.CreatePointFont(120,_T("Arial"), pDC);
	oFont.CreatePointFont(70,_T("Arial"), pDC);

	pageWidth = dc.GetDeviceCaps(HORZRES);
	pageHeight = dc.GetDeviceCaps(VERTRES);

	nHeight = -((pDC->GetDeviceCaps(LOGPIXELSY))/72);

	pDC->SelectObject(&hFont);
	pFont = pDC->GetCurrentFont();
	pFont->GetLogFont( &lfFont );
	RowHeight_h = (int)(1.1 * abs(lfFont.lfHeight));

	pDC->SelectObject(&oFont);
	hsize = MAX_GATES*(pDC->GetOutputTextExtent("THRESHOLD DETECT").cx+3)+
		pDC->GetOutputTextExtent("Pulse Width").cx;
	pFont = pDC->GetCurrentFont();
	pFont->GetLogFont( &lfFont );
	// heading returns y= 605
	// channel report = 25 lines
	// system report = 10 lines
	
	RowHeight = (int)(1.1 * abs(lfFont.lfHeight));
	//Yoffset = 1*RowHeight;
	//Xoffset = 20;
	Xoffset = pageWidth/20;
	Yoffset = pageHeight/20 ;

	nPageCount = (MAX_CHANNEL_PRINT) / ((pageWidth/hsize) * ((pageHeight-(2*RowHeight))/(RowHeight*25))) + 1;
	CPoint print_origin(Xoffset,Yoffset);
	
	if ( dc.StartDoc(&di) > 0 )
	{
		BOOL bContinue = TRUE;
		for ( page=1; 
			bContinue && ( channel < MAX_CHANNEL_PRINT); 
			page++ )
		{
			dc.StartPage();
			NewPage = FALSE;

			if ( page == 1 ) {
				pDC->SelectObject(&hFont);
                s.Format(_T("Configuration Data - Page %d"), page);
				temp = PrintHeading(pDC, print_origin, pageWidth, nHeight, 
					s, "");
				print_origin.y = temp + RowHeight_h*4;
				pDC->SelectObject(&oFont);
			} 
			else
			{
				pDC->SelectObject(&hFont);
				print_origin.x = Xoffset;
				print_origin.y = Yoffset;
                s.Format(_T("Configuration Data - Page %d"), page);
				pDC->TextOut(print_origin.x,print_origin.y, s);              // input param string 1
				s = theTime.Format( " %A, %B %d, %Y, %I:%M %p" );
				size = pDC->GetOutputTextExtent(s);
				pDC->TextOut(pageWidth-(size.cx+Xoffset), print_origin.y, s );  // date & time
				print_origin.y += RowHeight_h*4;
			}
			pDC->SelectObject(&oFont);
			print_origin.y += RowHeight_h;

			while (  channel < MAX_CHANNEL_PRINT )
			{
				returnLocation = MakeCfgTable( &dc, print_origin, channel++, DrawRect, PRINTER);
				if ( (returnLocation.x + hsize) < pageWidth )
				{
					print_origin.x = returnLocation.x;
				}
				else
				{
					print_origin.x = Xoffset;
					print_origin.y = returnLocation.y + RowHeight_h*2;
					if ( (print_origin.y + RowHeight * 25 ) > pageHeight )
					{
						print_origin.x = Xoffset;
						print_origin.y = Yoffset;
						NewPage = TRUE;
						break;
					}
				}
			}
		
			if ( dc.EndPage() <= 0 )
			{
				bContinue = FALSE;
			}
		}
		
		if ( bContinue )
		{
			dc.StartPage();

			pDC->SelectObject(&hFont);
			print_origin.x = Xoffset;
			print_origin.y = Yoffset;
            s.Format(_T("Configuration Data - Page %d"), page);
			pDC->TextOut(print_origin.x,print_origin.y, s);              // input param string 1
			s = theTime.Format( " %A, %B %d, %Y, %I:%M %p" );
			size = pDC->GetOutputTextExtent(s);
			pDC->TextOut(pageWidth-(size.cx+Xoffset), print_origin.y, s );  // date & time
			print_origin.y += RowHeight_h*4;

			pDC->SelectObject(&oFont);
			print_origin.y += RowHeight_h;
			returnLocation = MakeCfgTableHead(&dc, print_origin, gChannel,
				DrawRect, PRINTER);

			print_origin.x = Xoffset;
			print_origin.y = returnLocation.y + RowHeight_h*4;
			returnLocation = MakeCfgTableSensor(&dc, print_origin, 
				DrawRect, PRINTER);
			print_origin.x = returnLocation.x;

			dc.EndPage();

			dc.EndDoc();
		}
		else
			dc.AbortDoc();
	}
}

CPoint CViewcfg::MakeCfgTableDmx(CDC *pdc, CPoint printOrigin, int channel, CRect Rect, int device)
{
	int hpos,hpos1,hpos2,hpos3;
	int rpos,i,n;
	int ta;			// default text alignment
	int RowHeight,ColWidth;
	double sense;
	CString s,t;
	CFont *pFont, oFont;
	LOGFONT  lfFont;
	CSize stringSize;
	CPoint returnPoint = printOrigin;

	pFont = pdc->GetCurrentFont();
	pFont->GetLogFont( &lfFont );
  
	pdc->SetBkMode(TRANSPARENT);
	RowHeight = (int) abs((lfFont.lfHeight));
	ColWidth = abs((lfFont.lfWidth));

	if ( device != PRINTER )
	{
		CBrush brush(RGB(128,128,128));
		Rect += printOrigin;
		//pdc->FillRect(&Rect,&CBrush(pdc->GetBkColor()));
		//pdc->FillRect(&Rect,&brush);
		//pdc->DrawEdge(&Rect,EDGE_BUMP,BF_RECT  );
		hpos  = printOrigin.x;
		hpos1 = printOrigin.x+200;
		hpos2 = printOrigin.x+350;
		hpos3 = printOrigin.x+125;
		rpos  = printOrigin.y;
		sense = 1.0;
	}
	else
	{
		hpos  = printOrigin.x;
        hpos2 = printOrigin.x+pdc->GetOutputTextExtent("Channel-Dependent Items for Channel").cx;
        hpos3 = printOrigin.x+pdc->GetOutputTextExtent("Channel-Dependent").cx;
		hpos1 = hpos3;
		rpos  = printOrigin.y;
		sense = 1.0;
	}

    s.Format(_T("Scope Demux Settings")); pdc->TextOut(hpos,rpos,s);
	rpos += RowHeight;
	ta=pdc->SetTextAlign(TA_RIGHT);
	pdc->TextOut(hpos1,rpos,"RF");
	pdc->TextOut(hpos2,rpos,"FW"); 
	pdc->SetTextAlign(ta);
	s.Format(_T("Shunt"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);

	s.Format(_T("%7d"), DmxCalRec.nShunt[0]); pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%7d"), DmxCalRec.nShunt[1]); pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
    s.Format(_T("Gate 1 Zero Ref"));rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);

	s.Format(_T("%7d"),DmxCalRec.nG1Ref[0][0]);pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%7d"),DmxCalRec.nG1Ref[1][0]);pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
    s.Format(_T("Gate 1 FS Ref"));rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);
	s.Format(_T("%7d"),DmxCalRec.nG1Ref[0][1]);pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%7d"),DmxCalRec.nG1Ref[1][1]);pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
    s.Format(_T("Gate 2 Zero Ref"));rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);
	s.Format(_T("%7d"),DmxCalRec.nG2Ref[0][0]);pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%7d"),DmxCalRec.nG2Ref[1][0]);pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
    s.Format(_T("Gate 2 FS Ref"));rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);
	s.Format(_T("%7d"),DmxCalRec.nG2Ref[0][1]);pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%7d"),DmxCalRec.nG2Ref[1][1]);pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
    s.Format(_T("Reject (FW Only)"));  rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);
	s.Format(_T("%7d"), DmxCalRec.nReject[1]); pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
    s.Format(_T("Gate 1 Level Gain")); rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);
	s.Format(_T("%7.2f"), DmxCalRec.fAg1[0]); pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%7.2f"), DmxCalRec.fAg1[1]); pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
    s.Format(_T("Gate 1 Level Offset")); rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);
	s.Format(_T("%7.2f"), DmxCalRec.fBg1[0]); pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%7.2f"), DmxCalRec.fBg1[1]); pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
    s.Format(_T("Gate 2 Level Gain")); rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);
	s.Format(_T("%7.2f"), DmxCalRec.fAg2[0]); pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%7.2f"), DmxCalRec.fAg2[1]); pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
    s.Format(_T("Gate 2 Level Offset")); rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);
	s.Format(_T("%7.2f"), DmxCalRec.fBg2[0]); pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%7.2f"), DmxCalRec.fBg2[1]); pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
    s.Format(_T("Reject Coefs Ax+B")); rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);
	s.Format(_T("%7.2f"), DmxCalRec.fArej); pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%7.2f"), DmxCalRec.fBrej); pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
	s.Format(_T("Null Offset")); rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);
	s.Format(_T("%7d"), DmxCalRec.nScope1Null[0]); pdc->TextOut(hpos1,rpos,s);
	s.Format(_T("%7d"), DmxCalRec.nScope1Null[1]); pdc->TextOut(hpos2,rpos,s);pdc->SetTextAlign(ta);
    s.Format(_T("Channel Gain Trim")); rpos += RowHeight; pdc->TextOut(hpos,rpos,s);pdc->SetTextAlign(TA_RIGHT);
	pdc->SetTextAlign(ta);
	rpos += RowHeight;
	n = ColWidth * 5;
	s.Format(_T("1-20 (RF)"));pdc->TextOut(hpos,rpos,s);
	for (i=0; i < 20; i++)
	{
		s.Format(_T("%2d"), DmxCalRec.nVgain[i][0]);pdc->TextOut(10*ColWidth+hpos+n*i,rpos,s);
	}
	rpos += RowHeight;
	s.Format(_T("21-40"));pdc->TextOut(hpos,rpos,s);
	for (i=20; i < 40; i++)
	{
		s.Format(_T("%2d"), DmxCalRec.nVgain[i][0]);pdc->TextOut(10*ColWidth+hpos+n*(i-20),rpos,s);
	}

	rpos += RowHeight;
	s.Format(_T("1-20 (FW)"));pdc->TextOut(hpos,rpos,s);
	for (i=0; i < 20; i++)
	{
		s.Format(_T("%2d"), DmxCalRec.nVgain[i][1]);pdc->TextOut(10*ColWidth+hpos+n*i,rpos,s);
	}
	rpos += RowHeight;
	s.Format(_T("21-40"));pdc->TextOut(hpos,rpos,s);
	for (i=20; i < 40; i++)
	{
		s.Format(_T("%2d"), DmxCalRec.nVgain[i][1]);pdc->TextOut(10*ColWidth+hpos+n*(i-20),rpos,s);
	}

	rpos += RowHeight;
	rpos += RowHeight;
    s.Format(_T("Channel Null Trim")); rpos += RowHeight; pdc->TextOut(hpos,rpos,s);
	rpos += RowHeight;
	s.Format(_T("1-20 (RF)"));pdc->TextOut(hpos,rpos,s);
	for (i=0; i < 20; i++)
	{
		s.Format(_T("%2d"), DmxCalRec.nChNull[i][0]);pdc->TextOut(10*ColWidth+hpos+n*i,rpos,s);
	}
	rpos += RowHeight;
	s.Format(_T("21-40"));pdc->TextOut(hpos,rpos,s);
	for (i=20; i < 40; i++)
	{
		s.Format(_T("%2d"), DmxCalRec.nChNull[i][0]);pdc->TextOut(10*ColWidth+hpos+n*(i-20),rpos,s);
	}

	rpos += RowHeight;
	s.Format(_T("1-20 (FW)"));pdc->TextOut(hpos,rpos,s);
	for (i=0; i < 20; i++)
	{
		s.Format(_T("%2d"), DmxCalRec.nChNull[i][1]);pdc->TextOut(10*ColWidth+hpos+n*i,rpos,s);
	}
	rpos += RowHeight;
	s.Format(_T("21-40"));pdc->TextOut(hpos,rpos,s);
	for (i=20; i < 40; i++)
	{
		s.Format(_T("%2d"), DmxCalRec.nChNull[i][1]);pdc->TextOut(10*ColWidth+hpos+n*(i-20),rpos,s);
	}

	

	returnPoint.x = hpos2+pdc->GetOutputTextExtent("Threshold DetectXXXX").cx;
	returnPoint.y = rpos+RowHeight;
	return returnPoint;
}