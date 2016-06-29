//==========================================================================================
//==========================================================================================
// CSeekButton implementation
//
// Copyright 1998 Langis Pitre
//==========================================================================================
//==========================================================================================
#include "stdafx.h"
#include "SeekButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------------------
CSeekButton::CSeekButton(int InitialValue, int Direction, int Increment, int min, int max ) 
: CButton()
{
	mhWnd = NULL;
	SetParameters( mhWnd, InitialValue, Direction, Increment, min, max );
}

BEGIN_MESSAGE_MAP( CSeekButton, CButton )
	//{{AFX_MSG_MAP( CSeekButton )
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//<=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=><=>
// Name   : SetParameters
//          
// Descr. : 
//          
// Return : void
// Arg    : HWND hWnd        : 
// Arg    : int InitialValue : 
// Arg    : int Direction    : 
// Arg    : int Increment    : 
// Arg    : int min          : 
// Arg    : int max          : 
//------------------------------------------------------------------------------------------
void CSeekButton::SetParameters(HWND hWnd,
								int InitialValue, 
								int Direction, 
								int Increment,
								int min,
								int max )
{
	mhWnd         = hWnd;
	mCurrentValue = InitialValue;
	mDirection    = Direction;
	mIncrement    = Increment;
	mMin          = min;
	mMax          = max;
}

//------------------------------------------------------------------------------------------
// Descr. : Called when the user presses either the IDC_RWND or the IDC_FRWD button.
//          It initiates a seek loop ( forward or backward ) that lasts as long as the button
//          is pressed. The loop is terminated when a WM_LBUTTONUP message is received.
//          
//==========================================================================================
void CSeekButton::OnLButtonDown( UINT nFlags, CPoint point ) 
//==========================================================================================
{
	CButton::OnLButtonDown( nFlags, point );

	m_nBiter = 0;
	OnTimer(101);
	SetTimer(101,1000,NULL);

	/*
	int   nbiter = 0;
	MSG   mess;
	int   id  = GetDlgCtrlID( );

	HWND hWnd = GetSafeHwnd();

	if( mhWnd == NULL )
		mhWnd = GetParent()->GetSafeHwnd();

	while( !PeekMessage( &mess, hWnd, WM_LBUTTONUP, WM_LBUTTONUP, PM_REMOVE ) )
	{

		mCurrentValue += ( mDirection == UP_KEY )? mIncrement : -mIncrement;
		
		if( mCurrentValue < mMin )
		{
			mCurrentValue = mMax;
		}
		if( mCurrentValue >  mMax)
		{
			mCurrentValue = mMin;
		}
		TRACE("Value = %d\n", mCurrentValue );

		::SendMessage( mhWnd, WM_SEEKBUTTONVALUECHANGED, id, mCurrentValue );

		if( nbiter <1 )
		{
			Sleep(500);
		}
		else if( nbiter < 10 )
		{
			Sleep( 150 );
		}
		else if( nbiter < 20 )
		{
			Sleep( 150 );
		}
		else if( nbiter < 30 )
		{
			Sleep( 50 );
		}
		else 
		{
			Sleep( 25 );
		}
		
		nbiter++;

	}

	SendMessage( WM_LBUTTONUP );
	*/
}


void CSeekButton::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	int   id  = GetDlgCtrlID( );

	HWND hWnd = GetSafeHwnd();

	if( mhWnd == NULL )
		mhWnd = GetParent()->GetSafeHwnd();


	mCurrentValue += ( mDirection == UP_KEY )? mIncrement : -mIncrement;
		
	if( mCurrentValue < mMin )
	{
		mCurrentValue = mMin;
	}
	if( mCurrentValue >  mMax)
	{
		mCurrentValue = mMax;
	}
	TRACE("Value = %d\n", mCurrentValue );

	::SendMessage( mhWnd, WM_SEEKBUTTONVALUECHANGED, id, mCurrentValue );

	if (m_nBiter == 1) {
		//KillTimer(101);
		SetTimer(101,100,NULL);
	}
	m_nBiter++;

	CButton ::OnTimer(nIDEvent);
}

void CSeekButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(101);

	CButton ::OnLButtonUp(nFlags, point);
}

void CSeekButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnTimer(101);
	SetTimer(101,1000,NULL);

	CButton ::OnLButtonDblClk(nFlags, point);
}
