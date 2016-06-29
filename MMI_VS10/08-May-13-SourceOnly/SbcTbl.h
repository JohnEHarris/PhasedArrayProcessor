typedef struct
	{
	HWND	id;		// control id for scroll bar
	int *	var;	// pointer to variable being scrolled
	int		max;	// max scroll value
	int		min;
	int		pudelta;	// page up delta
	int		pddelta;
	int		ludelta;	// line up delta
	int		lddelta;
	HWND	en;		// control id for edit text window
	}	SB_CONTROL_TBL_ENTRY;

int GetSBControlTblIndex ( HWND hCtrl, SB_CONTROL_TBL_ENTRY *sbt);