//===================================================================
// File Name : 2440lib.h
// Function  : S3C2440 
// Date      : February 26, 2002
// Version   : 0.0
// History
//  0.0 :Feb.20.2002:SOP     : Programming start
//  0.01:Mar.29.2002:purnnamu: For POWEROFF_wake_up, the START... label is added
//===================================================================

#ifndef __2440lib_h__
#define __2440lib_h__

#ifdef __cplusplus
extern "C" {
#endif

void ChangeMPllValue(int m,int p,int s);
void ChangeClockDivider(int hdivn_val,int pdivn_val);
void ChangeUPllValue(int m,int p,int s);

#ifdef __cplusplus
}
#endif

#endif  //__2440lib_h__
