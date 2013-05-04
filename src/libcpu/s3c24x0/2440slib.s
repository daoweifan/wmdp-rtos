;=====================================================================
; File Name : 2410slib.s
; Function  : S3C2410  (Assembly)
; Program   : Shin, On Pil (SOP)
; Date      : March 09, 2002
; Version   : 0.0
; History
;   0.0 : Programming start (February 26,2002) -> SOP
;   0.1 : Port to IAR Compiler (2003.3.6) AIJI System Co., Ltd.
;=====================================================================

;Interrupt, FIQ/IRQ disable

NOINT  	EQU 		0x000000c0 	; 1100 0000
ENINT   EQU 		0x00000080
R1_I	EQU 		0x00001000 	;(1<<12)
R1_C	EQU 		0x00000004	;(1<<2)
R1_A	EQU 		0x00000002	;(1<<1)
R1_M    EQU 		0x00000001	;(1)
R1_iA	EQU 		0x80000000	;(1<<31)
R1_nF   EQU 		0x40000000	;(1<<30)

; Functions declared in this file.
    PUBLIC  MMU_EnableICache
    PUBLIC  MMU_DisableICache
	PUBLIC	MMU_EnableDCache
	PUBLIC	MMU_DisableDCache
	PUBLIC	MMU_EnableAlignFault
	PUBLIC	MMU_DisableAlignFault
	PUBLIC	MMU_EnableMMU
	PUBLIC	MMU_DisableMMU
	PUBLIC	MMU_SetFastBusMode
	PUBLIC	MMU_SetAsyncBusMode
	PUBLIC	MMU_SetTTBase
	PUBLIC	MMU_SetDomain
	PUBLIC	MMU_InvalidateIDCache
	PUBLIC	MMU_InvalidateICache
	PUBLIC	MMU_InvalidateICacheMVA
	PUBLIC	MMU_PrefetchICacheMVA
	PUBLIC	MMU_InvalidateDCache
	PUBLIC	MMU_InvalidateDCacheMVA
	PUBLIC	MMU_CleanDCacheMVA
	PUBLIC	MMU_CleanInvalidateDCacheMVA
	PUBLIC	MMU_CleanDCacheIndex
	PUBLIC	MMU_CleanInvalidateDCacheIndex
	PUBLIC	MMU_WaitForInterrupt
	PUBLIC	MMU_InvalidateTLB
	PUBLIC	MMU_InvalidateITLB
	PUBLIC	MMU_InvalidateITLBMVA
	PUBLIC	MMU_InvalidateDTLB
	PUBLIC	MMU_InvalidateDTLBMVA
	PUBLIC	MMU_SetDCacheLockdownBase
	PUBLIC	MMU_SetICacheLockdownBase
	PUBLIC	MMU_SetDTLBLockdown
	PUBLIC	MMU_SetITLBLockdown
	PUBLIC	MMU_SetProcessId
	
; Functions related to exception handling.	
    PUBLIC  ENABLE_INT
    PUBLIC  SET_IF
	PUBLIC	WR_IF
	PUBLIC	CLR_IF

  	rseg ICODE:CODE(2)
  	code32

;==============
; CPSR I,F bit
;==============
ENABLE_INT
    ;This function works only if the processor is in previliged mode.
   mrs 	r0,cpsr
   ldr	r1, =ENINT
   bic 	r0,r0,r1
   msr 	cpsr_cxsf,r0		
   mov 	pc,lr

;int SET_IF(void);
;The return value is current CPSR.
SET_IF
    ;This function works only if the processor is in previliged mode.
   mrs 	r0,cpsr
   mov 	r1,r0
   ldr	r0, =NOINT
   orr 	r1,r1,r0
   msr 	cpsr_cxsf,r1		
   mov 	pc,lr

;void WR_IF(int cpsrValue);
WR_IF
    ;This function works only if the processor is in previliged mode.
   msr 	cpsr_cxsf,r0		
   mov 	pc,lr


;void CLR_IF(void);
CLR_IF
    ;This function works only if the processor is in previliged mode.
   mrs 	r0,cpsr
   ldr	r1, =NOINT
   bic 	r0,r0,r1
   msr 	cpsr_cxsf,r0		
   mov 	pc,lr

;====================================
; MMU Cache/TLB/etc on/off functions
;====================================

;void MMU_EnableICache(void)
MMU_EnableICache
   mrc 	p15,0,r0,c1,c0,0
   ldr	r1, =R1_I
   orr 	r0,r0,r1
   mcr 	p15,0,r0,c1,c0,0
   mov 	pc,lr

;void MMU_DisableICache(void)
MMU_DisableICache
   mrc 	p15,0,r0,c1,c0,0
   ldr	r1, =R1_I
   bic 	r0,r0,r1
   mcr 	p15,0,r0,c1,c0,0
   mov 	pc,lr

;void MMU_EnableDCache(void)
MMU_EnableDCache
   mrc 	p15,0,r0,c1,c0,0
   ldr	r1, =R1_C
   orr 	r0,r0,r1
   mcr 	p15,0,r0,c1,c0,0
   mov 	pc,lr

;void MMU_DisableDCache(void)
MMU_DisableDCache
   mrc p15,0,r0,c1,c0,0
   bic r0,r0,#R1_C
   mcr p15,0,r0,c1,c0,0
   mov pc,lr

;void MMU_EnableAlignFault(void)
MMU_EnableAlignFault
   mrc 	p15,0,r0,c1,c0,0
   ldr	r1, =R1_A
   orr 	r0,r0,r1
   mcr 	p15,0,r0,c1,c0,0
   mov 	pc,lr

;void MMU_DisableAlignFault(void)
MMU_DisableAlignFault
   mrc 	p15,0,r0,c1,c0,0
   ldr	r1, =R1_A
   bic 	r0,r0,#R1_A
   mcr 	p15,0,r0,c1,c0,0
   mov 	pc,lr

;void MMU_EnableMMU(void)
MMU_EnableMMU
   mrc 	p15,0,r0,c1,c0,0
   ldr	r1, =R1_M
   orr 	r0,r0,r1
   mcr 	p15,0,r0,c1,c0,0
   mov 	pc,lr

;void MMU_DisableMMU(void)
MMU_DisableMMU
   mrc 	p15,0,r0,c1,c0,0
   ldr	r1, =R1_M
   bic 	r0,r0,#R1_M
   mcr 	p15,0,r0,c1,c0,0
   mov 	pc,lr

;void MMU_SetFastBusMode(void)
; FCLK:HCLK= 1:1
MMU_SetFastBusMode
   mrc 	p15,0,r0,c1,c0,0
   ldr	r1, =R1_iA
   ldr	r2, =R1_nF
   orr	r2, r2, r1
   bic 	r0,r0,r2
   mcr 	p15,0,r0,c1,c0,0
   mov 	pc,lr

;void MMU_SetAsyncBusMode(void)
; FCLK:HCLK= 1:2
MMU_SetAsyncBusMode
   mrc 	p15,0,r0,c1,c0,0
   ldr	r1, =R1_nF
   ldr	r2, =R1_iA
   orr	r2, r2, r1
   orr 	r0,r0,r2
   mcr 	p15,0,r0,c1,c0,0
   mov 	pc,lr

;=========================
; Set TTBase
;=========================
;void MMU_SetTTBase(int base)
MMU_SetTTBase
   ;ro=TTBase
   mcr 	p15,0,r0,c2,c0,0
   mov 	pc,lr

;=========================
; Set Domain
;=========================
;void MMU_SetDomain(int domain)
MMU_SetDomain
   ;ro=domain
   mcr 	p15,0,r0,c3,c0,0
   mov 	pc,lr

;=========================
; ICache/DCache functions
;=========================
;void MMU_InvalidateIDCache(void)
MMU_InvalidateIDCache
   mcr 	p15,0,r0,c7,c7,0
   mov 	pc,lr

;void MMU_InvalidateICache(void)
MMU_InvalidateICache
   mcr 	p15,0,r0,c7,c5,0
   mov 	pc,lr

;void MMU_InvalidateICacheMVA(U32 mva)
MMU_InvalidateICacheMVA
   ;r0=mva
   mcr 	p15,0,r0,c7,c5,1
   mov 	pc,lr
	
;void MMU_PrefetchICacheMVA(U32 mva)
MMU_PrefetchICacheMVA
   ;r0=mva
   mcr 	p15,0,r0,c7,c13,1
   mov 	pc,lr

;void MMU_InvalidateDCache(void)
MMU_InvalidateDCache
   mcr 	p15,0,r0,c7,c6,0
   mov 	pc,lr

;void MMU_InvalidateDCacheMVA(U32 mva)
MMU_InvalidateDCacheMVA
   ;r0=mva
   mcr 	p15,0,r0,c7,c6,1
   mov 	pc,lr

;void MMU_CleanDCacheMVA(U32 mva)
MMU_CleanDCacheMVA
   ;r0=mva
   mcr 	p15,0,r0,c7,c10,1
   mov 	pc,lr

;void MMU_CleanInvalidateDCacheMVA(U32 mva)
MMU_CleanInvalidateDCacheMVA
   ;r0=mva
   mcr 	p15,0,r0,c7,c14,1
   mov 	pc,lr

;void MMU_CleanDCacheIndex(U32 index)
MMU_CleanDCacheIndex
   ;r0=index
   mcr 	p15,0,r0,c7,c10,2
   mov 	pc,lr

;void MMU_CleanInvalidateDCacheIndex(U32 index)	
MMU_CleanInvalidateDCacheIndex
   ;r0=index
   mcr 	p15,0,r0,c7,c14,2
   mov 	pc,lr

;void MMU_WaitForInterrupt(void)
MMU_WaitForInterrupt
   mcr 	p15,0,r0,c7,c0,4
   mov 	pc,lr

;===============
; TLB functions
;===============
;voic MMU_InvalidateTLB(void)
MMU_InvalidateTLB
   mcr 	p15,0,r0,c8,c7,0
   mov 	pc,lr

;void MMU_InvalidateITLB(void)
MMU_InvalidateITLB
   mcr 	p15,0,r0,c8,c5,0
   mov 	pc,lr

;void MMU_InvalidateITLBMVA(U32 mva)
MMU_InvalidateITLBMVA
   ;ro=mva
   mcr 	p15,0,r0,c8,c5,1
   mov 	pc,lr

;void MMU_InvalidateDTLB(void)
MMU_InvalidateDTLB
	mcr p15,0,r0,c8,c6,0
	mov pc,lr

;void MMU_InvalidateDTLBMVA(U32 mva)
MMU_InvalidateDTLBMVA
	;r0=mva
	mcr p15,0,r0,c8,c6,1
	mov pc,lr

;=================
; Cache lock down
;=================
;void MMU_SetDCacheLockdownBase(U32 base)
MMU_SetDCacheLockdownBase
   ;r0= victim & lockdown base
   mcr 	p15,0,r0,c9,c0,0
   mov 	pc,lr
	
;void MMU_SetICacheLockdownBase(U32 base)
MMU_SetICacheLockdownBase
   ;r0= victim & lockdown base
   mcr 	p15,0,r0,c9,c0,1
   mov 	pc,lr

;=================
; TLB lock down
;=================
;void MMU_SetDTLBLockdown(U32 baseVictim)
MMU_SetDTLBLockdown
   ;r0= baseVictim
   mcr 	p15,0,r0,c10,c0,0
   mov 	pc,lr
	
;void MMU_SetITLBLockdown(U32 baseVictim)
MMU_SetITLBLockdown
   ;r0= baseVictim
   mcr 	p15,0,r0,c10,c0,1
   mov 	pc,lr

;============
; Process ID
;============
;void MMU_SetProcessId(U32 pid)
MMU_SetProcessId
   	;r0= pid
   	mcr 	p15,0,r0,c13,c0,0
   	mov 	pc,lr

   END
