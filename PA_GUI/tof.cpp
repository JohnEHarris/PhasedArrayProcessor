// tof.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
// include only the parent dialog header asidlg.h for all child dialogs
#include "TscanDlg.h"
#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
bool bCtof;		// create or update flag
/////////////////////////////////////////////////////////////////////////////
// Ctof dialog
extern char *st_invd, *st_high, *st_low,*tof_start_g1[],*tof_start_g2[];
extern char *ama_triger[],*ama_prom_g1[],*ama_prom_g2[],*ama_log_pos,*ama_log_min;
extern char *tof_stop_g1[],*tof_stop_g2[];
Ctof::Ctof(CWnd* pParent /*=NULL*/)
	: CDialog(Ctof::IDD, pParent)
	{
	//{{AFX_DATA_INIT(Ctof)
	//}}AFX_DATA_INIT
	bCtof = TRUE;

	m_pUndo = new CONFIG_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
	}

void Ctof::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}

void Ctof::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Ctof)
	DDX_Control(pDX, IDC_BtnWll, m_btnWall);
	DDX_Control(pDX, IDC_CB_StOn, m_CBst);
	DDX_Control(pDX, IDC_CB_SpOn, m_CBsp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Ctof, CDialog)
	//{{AFX_MSG_MAP(Ctof)
	ON_BN_CLICKED(IDC_BTNAlrm, OnBTNAlrm)
	ON_BN_CLICKED(IDC_BtnLgc, OnBtnLgc)
	ON_BN_CLICKED(IDC_BtnRsl, OnBtnRsl)
	ON_BN_CLICKED(IDC_BtnWll, OnBtnWll)
	ON_CBN_SELCHANGE(IDC_CB_StOn, OnSelchangeCBStOn)
	ON_CBN_SELCHANGE(IDC_CB_SpOn, OnSelchangeCBSpOn)
	ON_WM_MOVE()
	ON_BN_CLICKED(ID_TOF_UNDO, OnTofUndo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Ctof message handlers
int LamAlarmOn(void)
{
static BYTE alarm_map[8] = {0,0,0x10,0x10,0,0,0x20,0};
int trig_sel;

	/* Evaluate trigger settings to determine if wall alarm is on or off. */
	if ( gGate == 0)
		{
		return (int) ( ConfigRec.alarm.laminar[gChannel][gGate] & 0x10);
		}

	// This is gate 1, the wall gate.
	
	if (ConfigRec.alarm.laminar[gChannel][gGate] != 0)	// on
		{	// may be on...check tof trigger select
		trig_sel = ConfigRec.timeoff.trigger[gChannel][gGate];
		ConfigRec.alarm.laminar[gChannel][gGate] = alarm_map[trig_sel & 7];
		}

	return (int) ( ConfigRec.alarm.laminar[gChannel][gGate] & 0x30);
}


#ifndef TWO_CHANNEL_4_GATE_BOARD

BOOL Ctof::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	CString s;
	short tmp;
	// TODO: Add extra initialization here
	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	if ( bCtof )
	{
		bCtof = FALSE;
		GetWindowPlacement(&wp);
		dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		pCTscanDlg->GetWindowLastPosition("TIME_OF_FLIGHT", &rect);
		if ( ( (rect.right - rect.left) >= dx ) &&
			 ( (rect.bottom - rect.top) >= dy ))
			{
			wp.rcNormalPosition = rect;
			SetWindowPlacement(&wp);
			}
	}
	gChannel %= MAX_CHANNEL;
#ifdef  TWO_CHANNEL_4_GATE_BOARD
	gGate &= 3;
#else
	gGate &= 3;
#endif
	//gShoe &= 10;
	if (gShoe < 0) gShoe = 0;
	if (gShoe > 9) gShoe = 9;
	s.Format(_T("   ch=%d  TOF g=%d"),gChannel +1,gGate+1);
	SetWindowText(s);
		if(gGate == 0)
			{
			s=st_invd;
			}
		else
			{
			if(ConfigRec.timeoff.resolution[gChannel] == TRUE)
				s=((LPSTR)st_high);
			else
				s=((LPSTR)st_low);
			}
		SetDlgItemText(IDC_BtnRsl,s);
		m_CBst.ResetContent( );
		if(gGate == 0)
		{
			// for gate zero, initialize the list of options
			// "Disable","Init Pulse","Threshold Det","Slave A","Slave B"
			for(tmp=0; tmp < 5; tmp++)
				m_CBst.AddString(tof_start_g1[tmp]);
		}

		else
			// for gate one, initialize the list of options
			// "Disable","Init Pulse","Threshold Det","Peak Detect",
			//				"Slave A","Slave B","Gate 1 I/F Echo"
			
			for(tmp=0; tmp < 7; tmp++)
				m_CBst.AddString(tof_start_g2[tmp]);

		// put the cursor on the last one selected
		m_CBst.SetCurSel(ConfigRec.timeoff.trigger[gChannel][gGate]);
		m_CBsp.ResetContent( );

		if(gGate == 0)
		{
			// for gate zero, initialize the list of options
			// "Threshold Detect","Max Peak"
			for(tmp=0; tmp < 1; tmp++)
				m_CBsp.AddString(tof_stop_g1[tmp]);
		}
		else {
			// for gate one, initialize the list of options
			// "Threshold Detect","Peak Detect","Max Peak"
			for(tmp=0;tmp<2;tmp++)
				m_CBsp.AddString(tof_stop_g2[tmp]);
		}
		m_CBsp.SetCurSel(ConfigRec.timeoff.stopon[gChannel][gGate]);

		// MERGE THE INFO FROM DLG_ALAR.C

		
			// The 'Alarm' button
			if ( (ConfigRec.alarm.trigger[gChannel][gGate] & 1) == 1)
				SetDlgItemText(IDC_BTNAlrm,(LPSTR)ama_triger[1]);	//gate on
			else
				SetDlgItemText(IDC_BTNAlrm,(LPSTR)ama_triger[0]);	// gate off

			// The 'Laminar/Wall' button
			if ( gGate == 0)
				{	// gate 1.. laminar
				if (LamAlarmOn() != 0)	// on
					SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g1[1]);	//on
				else
					SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g1[0]);	//off
				}
			else
				{	// wall gate
				if (LamAlarmOn() != 0)	// on
						SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g2[1]);	//on
				else
					SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g2[0]);	//off
				}



			if ( ConfigRec.alarm.polarity[gChannel][gGate] != 0)
				SetDlgItemText(IDC_BtnLgc,(LPSTR)ama_log_pos);
			else
				SetDlgItemText(IDC_BtnLgc,(LPSTR)ama_log_min);
			
	SendMsg(ALARM_LAMIN);
	SendMsg(TOF_TRIGGER);
			return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

#else    /* #ifdef TWO_CHANNEL_4_GATE_BOARD */

BOOL Ctof::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	CString s;
	short tmp;
	// TODO: Add extra initialization here
	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	if ( bCtof )
	{
		bCtof = FALSE;
		GetWindowPlacement(&wp);
		dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		pCTscanDlg->GetWindowLastPosition(_T("TIME_OF_FLIGHT"), &rect);
		if ( ( (rect.right - rect.left) >= dx ) &&
			 ( (rect.bottom - rect.top) >= dy ))
			{
			wp.rcNormalPosition = rect;
			SetWindowPlacement(&wp);
			}
	}
	gChannel %= MAX_CHANNEL;
	gGate &= 3;
	//gShoe &= 10; //wrong
	if (gShoe < 0) gShoe = 0;
	if (gShoe > 9) gShoe = 9;
	s.Format(_T("   ch=%d   TOF   g=%d"),gChannel +1,gGate+1);
	SetWindowText(s);
		if(gGate == 0)
			{
			s=st_invd;
			}
		else
			{
			if(ConfigRec.timeoff.resolution[gChannel] == TRUE)
				s=((LPSTR)st_high);
			else
				s=((LPSTR)st_low);
			}
		SetDlgItemText(IDC_BtnRsl,s);
		m_CBst.ResetContent( );
		if(gGate == 0)
		{
			// for gate zero, initialize the list of options
			// "Disable","Init Pulse","Threshold Det","Slave A","Slave B"
			for(tmp=0; tmp < 7; tmp++)
				{
				s = tof_start_g1[tmp];
				m_CBst.AddString(s);
				}
		}

		else
			// for gate one, initialize the list of options
			// "Disable","Init Pulse","Threshold Det","Peak Detect",
			//				"Slave A","Slave B","Gate 1 I/F Echo"
			
			for(tmp=0; tmp < 7; tmp++)
				{
				s = tof_start_g2[tmp];
				m_CBst.AddString(s);
				}

		// put the cursor on the last one selected
		m_CBst.SetCurSel(ConfigRec.timeoff.trigger[gChannel][gGate]);
		m_CBsp.ResetContent( );

		if(gGate == 0)
		{
			// for gate zero, initialize the list of options
			// "Threshold Detect","Max Peak"
			for(tmp=0; tmp < 2; tmp++)
				{
				s = tof_stop_g1[tmp];
				m_CBsp.AddString(s);
				}
		}
		else {
			// for gate one, initialize the list of options
			// "Threshold Detect","Peak Detect","Max Peak"
			for(tmp=0;tmp<2;tmp++)
				{
				s = tof_stop_g2[tmp];
				m_CBsp.AddString(s);
				}
		}
		m_CBsp.SetCurSel(ConfigRec.timeoff.stopon[gChannel][gGate]);

		// MERGE THE INFO FROM DLG_ALAR.C

		
			// The 'Alarm' button
			if ( (ConfigRec.alarm.trigger[gChannel][gGate] & 1) == 1)
				s = ama_triger[1];
				//SetDlgItemText(IDC_BTNAlrm,(LPSTR)ama_triger[1]);	//gate on
			else
				s = ama_triger[0];
				//SetDlgItemText(IDC_BTNAlrm,(LPSTR)ama_triger[0]);	// gate off
			SetDlgItemText(IDC_BTNAlrm,s);

			// The 'Laminar/Wall' button
			if ( gGate != 3)
				{	// gate 1,2, 3.. no button
				m_btnWall.ShowWindow (SW_HIDE);
				}
			else
				{	// wall gate
				m_btnWall.ShowWindow (SW_SHOW);
				if (ConfigRec.alarm.laminar[gChannel][gGate] != 0)	// on
					s = ama_prom_g2[1];
					//SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g2[1]);	//on
				else
					s = ama_prom_g2[0];
					//SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g2[0]);	//off
				SetDlgItemText(IDC_BtnWll,s);
				}



			if ( ConfigRec.alarm.polarity[gChannel][gGate] != 0)
				s = ama_log_min;
				//SetDlgItemText(IDC_BtnLgc,(LPSTR)ama_log_min);
			else
				s = ama_log_pos;
				//SetDlgItemText(IDC_BtnLgc,(LPSTR)ama_log_pos);
			SetDlgItemText(IDC_BtnLgc,s);
			
	SendMsg(ALARM_LAMIN);
	SendMsg(TOF_TRIGGER);
			return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

#endif   /* TWO_CHANNEL_4_GATE_BOARD */


Ctof::~Ctof()
	{
	delete m_pUndo;
	}

void Ctof::KillMe()
	{
	// Public access to OnCancel
	// Add to every modeless dialog
	OnCancel();
	}


void Ctof::Save_Pos()
	{
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
	}

void Ctof::OnCancel() 
	{
	// TODO: Add extra cleanup here

	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void Ctof::OnOK() 
	{
	// TODO: Add extra validation here
//		SendMsg(DEBUG_CONTROL);
	Save_Pos();
	SendMsg(ALARM_LAMIN);
	SendMsg(TOF_TRIGGER);
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

BOOL Ctof::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Tscandlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void Ctof::OnBTNAlrm() 
	{
	CString s;
	// TODO: Add your control notification handler code here
		ConfigRec.alarm.trigger[gChannel][gGate] ^= 1;	// toggle bit0
				if ( (ConfigRec.alarm.trigger[gChannel][gGate] & 1) == 1)
					s = ama_triger[1];
					//SetDlgItemText(IDC_BTNAlrm,(LPSTR)ama_triger[1]);	//on
  				else
  					{
					ConfigRec.alarm.trigger[gChannel][gGate] = 0; 
					s = ama_triger[0];
					//SetDlgItemText(IDC_BTNAlrm,(LPSTR)ama_triger[0]);	// off
					}
				SetDlgItemText(IDC_BTNAlrm,s);
				SendMsg(ALARM_TRIGGER);
	
	}


#ifndef  TWO_CHANNEL_4_GATE_BOARD

void Ctof::OnBtnLgc() 
	{
	// TODO: Add your control notification handler code here
				if(ConfigRec.alarm.polarity[gChannel][gGate] != 0)
					{
					ConfigRec.alarm.polarity[gChannel][gGate] = 0;
					SetDlgItemText(IDC_BtnLgc,(LPSTR)ama_log_min);
					}
				else
					{
					ConfigRec.alarm.polarity[gChannel][gGate] = (char)0xff;
					SetDlgItemText(IDC_BtnLgc,(LPSTR)ama_log_pos);
					}
				SendMsg(ALARM_POLARITY);
	
	}

#else  /* ifdef  TWO_CHANNEL_4_GATE_BOARD */

void Ctof::OnBtnLgc() 
	{
	CString s;
	// TODO: Add your control notification handler code here
				if(ConfigRec.alarm.polarity[gChannel][gGate] != 0)
					{
					ConfigRec.alarm.polarity[gChannel][gGate] = 0;
					s = ama_log_pos;
					//SetDlgItemText(IDC_BtnLgc,(LPSTR)ama_log_pos);
					}
				else
					{
					ConfigRec.alarm.polarity[gChannel][gGate] = (char)0xff;
					s = ama_log_min;
					//SetDlgItemText(IDC_BtnLgc,(LPSTR)ama_log_min);
					}
				SetDlgItemText(IDC_BtnLgc,s);
				SendMsg(ALARM_POLARITY);
	
	}

#endif  /* TWO_CHANNEL_4_GATE_BOARD */


void Ctof::OnBtnRsl() 
	{
	CString s;
	// TODO: Add your control notification handler code here
				if (gGate == 0)
					// no resolution associated with gGate 0
					return ;

				// for gGate 1 ..ie, 2nd gGate
				if(ConfigRec.timeoff.resolution[gChannel] == TRUE)
					{
					// was high, now set text to show low
					s = st_low;
					//SetDlgItemText(IDC_BtnRsl,(LPSTR)st_low);
					ConfigRec.timeoff.resolution[gChannel] = FALSE;
					}
				else
					{	// was low
					s = st_high;
					//SetDlgItemText(IDC_BtnRsl,(LPSTR)st_high);
					ConfigRec.timeoff.resolution[gChannel] = TRUE;
					}
				SetDlgItemText(IDC_BtnRsl,s);
//				i = TOF_RESOLUTION;	// what is this for ??
				// tell the instrument what the new resolution is
				SendMsg(TOF_RESOLUTION);
	
	}


#ifndef  TWO_CHANNEL_4_GATE_BOARD

void Ctof::OnBtnWll() 
{
	// TODO: Add your control notification handler code here
				if ( gGate == 0 )
					{	// gGate 1.. laminar
					ConfigRec.alarm.laminar[gChannel][gGate] ^= 0x10;	// toggle bit 4
					if (LamAlarmOn() != 0)	// on
						SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g1[1]);	//on
					else
						{
						SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g1[0]);	//off
						ConfigRec.alarm.laminar[gChannel][gGate] = 0;
						}
					}
				else
					{	// wall gGate
					if (ConfigRec.alarm.laminar[gChannel][gGate] != 0)
						ConfigRec.alarm.laminar[gChannel][gGate] = 0;
					else ConfigRec.alarm.laminar[gChannel][gGate] = 0x30;

					if (LamAlarmOn() != 0)	// on
						SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g2[1]);
					else
						SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g2[0]);	//off
					}
				SendMsg(ALARM_LAMIN);
	
}

#else  /* ifdef  TWO_CHANNEL_4_GATE_BOARD */

void Ctof::OnBtnWll() 
{
CString s;
	// TODO: Add your control notification handler code here
				if ( gGate != 3 )
					{	// gGate 1.. laminar
					}
				else
					{	// wall gGate
					if (ConfigRec.alarm.laminar[gChannel][gGate] != 0)
						ConfigRec.alarm.laminar[gChannel][gGate] = 0;
					else ConfigRec.alarm.laminar[gChannel][gGate] = 0x30;

					if (ConfigRec.alarm.laminar[gChannel][gGate] != 0)	// on
						s = ama_prom_g2[1];
						//SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g2[1]);
					else
						s = ama_prom_g2[0];
						//SetDlgItemText(IDC_BtnWll,(LPSTR)ama_prom_g2[0]);	//off
					}
				SetDlgItemText(IDC_BtnWll,s);
				SendMsg(ALARM_LAMIN);
	
}

#endif  /* TWO_CHANNEL_4_GATE_BOARD */


void Ctof::OnSelchangeCBStOn() 
{
	// TODO: Add your control notification handler code here
			ConfigRec.timeoff.trigger[gChannel][gGate] = m_CBst.GetCurSel ();
			// Reassess the condition of the wall/laminar alarm
			LamAlarmOn();
			SendMsg(ALARM_LAMIN);
			SendMsg(TOF_TRIGGER);
	
}

void Ctof::OnSelchangeCBSpOn() 
{
	// TODO: Add your control notification handler code here
				ConfigRec.timeoff.stopon[gChannel][gGate] = m_CBsp.GetCurSel ();
				SendMsg(TOF_STOPON);
	
}


void Ctof::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	// TODO: Add your message handler code here
		if ( !bCtof ) Save_Pos();
	
}

void Ctof::OnTofUndo() 
	{
	// TODO: Add your control notification handler code here
	if (MessageBox(	_T("Undo all changes since last SAVE operation?"),
			_T("Restore Configuration"),MB_YESNO)== IDYES)
	{
		//memcpy ( (void *) &ConfigRec, (void *) m_pUndo, sizeof(CONFIG_REC));
		UndoTofSettings();
	}

	Save_Pos();
	OnInitDialog();	
	}


void Ctof::UndoTofSettings()
{
	int iChannel, iGate;
	int ic, ig;
	iChannel = gChannel;
	iGate = gGate;
	for (ic=0; ic<MAX_CHANNEL; ic++)  //channel loop
	{
		gChannel = ic;
		SendMsg(CHANNEL_SELECT);
		for (ig=0; ig<MAX_GATE; ig++)  //gate loop
		{
			gGate = ig;
			pCTscanDlg->SendMsg(GATE_SELECT);

			ConfigRec.timeoff.trigger[ic][ig] = m_pUndo->timeoff.trigger[ic][ig] ;
			SendMsg(TOF_TRIGGER);

			ConfigRec.timeoff.stopon[ic][ig] = m_pUndo->timeoff.stopon[ic][ig] ;
			SendMsg(TOF_STOPON);
		}
	}

	gChannel = iChannel;
	SendMsg(CHANNEL_SELECT);

	gGate = iGate;
	SendMsg(GATE_SELECT);
}