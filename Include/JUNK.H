
/* msg 4.  Set All Flaw type Nc's And Wall Nx	*/

typedef struct
	{
	BYTE	Long[2];	/* [0]=id nc, [1] = od nc	*/
	BYTE	Tran[2];
	BYTE	Oblq1[2];
	BYTE	Oblq2[2];
	BYTE	Oblq3[2];
	BYTE	Wall[2];
	BYTE	Tol;		/* Nc tolerance */
	BYTE	spare[3];
	}	C_MSG_04;
	
