//===================================================================
// File Name : 2410slib.h
// Function  : S3C2410 
// Program   : Shin, On Pil (SOP)
// Date      : February 20, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (February 20,2002) -> SOP
//===================================================================

#include "def.h"
#ifndef __2410slib_h__
#define __2410slib_h__


__arm int SET_IF(void);
__arm void WR_IF(int cpsrValue);
__arm void CLR_IF(void);

__arm void MMU_EnableICache(void);
__arm void MMU_DisableICache(void);
__arm void MMU_EnableDCache(void);
__arm void MMU_DisableDCache(void);
__arm void MMU_EnableAlignFault(void);
__arm void MMU_DisableAlignFault(void);
__arm void MMU_EnableMMU(void);
__arm void MMU_DisableMMU(void);
__arm void MMU_SetTTBase(u32 base);
__arm void MMU_SetDomain(u32 domain);

__arm void MMU_SetFastBusMode(void);          //GCLK=HCLK
__arm void MMU_SetAsyncBusMode(void);         //GCLK=FCLK @(FCLK>=HCLK)

__arm void MMU_InvalidateIDCache(void);
__arm void MMU_InvalidateICache(void);
__arm void MMU_InvalidateICacheMVA(u32 mva);
__arm void MMU_PrefetchICacheMVA(u32 mva);
__arm void MMU_InvalidateDCache(void);
__arm void MMU_InvalidateDCacheMVA(u32 mva);
__arm void MMU_CleanDCacheMVA(u32 mva);
__arm void MMU_CleanInvalidateDCacheMVA(u32 mva);
__arm void MMU_CleanDCacheIndex(u32 index);
__arm void MMU_CleanInvalidateDCacheIndex(u32 index); 
__arm void MMU_WaitForInterrupt(void);
        
__arm void MMU_InvalidateTLB(void);
__arm void MMU_InvalidateITLB(void);
__arm void MMU_InvalidateITLBMVA(u32 mva);
__arm void MMU_InvalidateDTLB(void);
__arm void MMU_InvalidateDTLBMVA(u32 mva);

__arm void MMU_SetDCacheLockdownBase(u32 base);
__arm void MMU_SetICacheLockdownBase(u32 base);

__arm void MMU_SetDTLBLockdown(u32 baseVictim);
__arm void MMU_SetITLBLockdown(u32 baseVictim);

__arm void MMU_SetProcessId(u32 pid);

 
#endif   //__2410slib_h__
