//===================================================================
// File Name : 2440lib.c
// Function  : S3C2410 PLL,Uart, LED, Port Init
// Date      : March 20, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (February 20,2002) -> SOP
//===================================================================

#include "def.h"
#include "option.h"
#include "s3c2440x.h"
#include "2440lib.h"
#include "2440slib.h" 

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

//*************************[ MPLL ]*******************************
void ChangeMPllValue(int mdiv,int pdiv,int sdiv)
{
    rMPLLCON = (mdiv<<12) | (pdiv<<4) | sdiv;
}


//************************[ HCLK, PCLK ]***************************
/*
// for 2410.
void ChangeClockDivider(int hdivn,int pdivn)
{
     // hdivn,pdivn FCLK:HCLK:PCLK
     //     0,0         1:1:1 
     //     0,1         1:1:2 
     //     1,0         1:2:2
     //     1,1         1:2:4
    rCLKDIVN = (hdivn<<1) | pdivn;    

    if(hdivn)
        MMU_SetAsyncBusMode();
    else 
        MMU_SetFastBusMode();
}
*/
// Modified for 2440.
void ChangeClockDivider(int hdivn_val,int pdivn_val)
{
	int hdivn=2, pdivn=0;
	
     // hdivn_val (FCLK:HCLK)ratio hdivn
     // 11           1:1       (0)
     // 12           1:2       (1)
     // 13           1:3       (3) 
     // 14           1:4       (2)
     // pdivn_val (HCLK:PCLK)ratio pdivn
     // 11           1:1       (0)
     // 12           1:2       (1)
	switch(hdivn_val) {
		case 11: hdivn=0; break;
		case 12: hdivn=1; break;
		case 13:
		case 16: hdivn=3; break;
		case 14: 
		case 18: hdivn=2; break;
	}
	
	switch(pdivn_val) {
		case 11: pdivn=0; break;
		case 12: pdivn=1; break;
	}
	
	//Uart_Printf("Clock division change [hdiv:%x, pdiv:%x]\n", hdivn, pdivn);
	rCLKDIVN = (hdivn<<1) | pdivn;

	switch(hdivn_val) {
		case 16:		// when 1, HCLK=FCLK/8.
			rCAMDIVN = (rCAMDIVN & ~(3<<8)) | (1<<8); 
		break; 
		case 18: 	// when 1, HCLK=FCLK/6.
			rCAMDIVN = (rCAMDIVN & ~(3<<8)) | (1<<9); 
		break;
	}
	
    if(hdivn!=0)
        MMU_SetAsyncBusMode();
    else 
        MMU_SetFastBusMode();
}



//**************************[ UPLL ]*******************************
void ChangeUPllValue(int mdiv,int pdiv,int sdiv)
{
    rUPLLCON = (mdiv<<12) | (pdiv<<4) | sdiv;
}
