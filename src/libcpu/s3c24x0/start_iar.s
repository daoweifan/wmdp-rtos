;=========================================
; NAME: 2440INIT.S
; DESC: C start up codes
;       Configure memory, ISR ,stacks
;       Initialize C-variables
; HISTORY:
;init file for s3c2440a
;init file for FriendlyARM
;initial version: modified by David
;=========================================
#include "option.inc"
#include "memcfg.inc"
#include "2440addr.inc"
#include "FreeRTOSConfig.h"
#include "ISR_Support.h"

USERMODE	EQU 	0x10
FIQMODE 	EQU 	0x11
IRQMODE 	EQU 	0x12
SVCMODE 	EQU 	0x13
ABORTMODE   EQU 	0x17
UNDEFMODE   EQU 	0x1b
SYSMODE     EQU 	0x1f
MODEMASK	EQU 	0x1f
NOINT   	EQU 	0xc0

	SECTION   IRQ_STACK:DATA:NOROOT(3)
	SECTION   FIQ_STACK:DATA:NOROOT(3)
	SECTION   UND_STACK:DATA:NOROOT(3)
	SECTION   ABT_STACK:DATA:NOROOT(3)
	SECTION   SVC_STACK:DATA:NOROOT(3)
	SECTION   CSTACK:DATA:NOROOT(3)

;---------------------------------------------------------------
; ?RESET
; Reset Vector.
; Normally, segment INTVEC is linked at address 0.
; For debugging purposes, INTVEC may be placed at other
; addresses.
; A debugger that honors the entry point will start the
; program in a normal way even if INTVEC is not at address 0.
;---------------------------------------------------------------
	EXTERN  ?main    ;The main entry of mon program
	EXTERN  C_IRQHandler ; IRQ handler
	SECTION .intvec:CODE:NOROOT(2)
	IMPORT  LINK
	IMPORT  vPortYieldProcessor
	PUBLIC  __iar_program_start
	CODE32  ; Always ARM mode after reset

__iar_program_start:
	b  ResetHandler    ;SYSTEM START
	b  HandlerUndef    ;handler for Undefined mode
	; b  HandlerSWI      ;handler for SWI interrupt
	b  vPortYieldProcessor
	b  HandlerPabort   ;handler for PAbort
	b  HandlerDabort   ;handler for DAbort
	b  .               ;reserved
	b  HandlerIRQ      ;handler for IRQ interrupt
	b  HandlerFIQ      ;handler for FIQ interrupt

HandlerIRQ:
	; sub	sp,sp,#4			;decrement sp(to store jump address)
	; stmfd	sp!,{r0}		;PUSH the work register to stack(lr does't push because it return to original address)
	; ldr		r0,=HandleIRQ	;load the address of HandleXXX to r0
	; ldr		r0,[r0]			;load the contents(service routine start address) of HandleXXX
	; str		r0,[sp,#4]		;store the contents(ISR) of HandleXXX to stack
	; ldmfd	sp!,{r0,pc}		;POP the work register and pc(jump to ISR)
; IRQ Handler share the same "context save&restore" module with Task Schedule
; The way of FREERTOS is different from UCOS and RT-Thread
; HandlerIRQ:
	portSAVE_CONTEXT					; Save the context of the current task.
	bl C_IRQHandler 					; Call C_IRQHandler
	portRESTORE_CONTEXT					; Restore the context of the selected task.

HandlerUndef:
	sub	sp,sp,#4        ;decrement sp(to store jump address)
	stmfd	sp!,{r0}        ;PUSH the work register to stack(lr does't push because it return to original address)
	ldr     r0,=HandleUndef	;load the address of HandleXXX to r0
	ldr     r0,[r0]         ;load the contents(service routine start address) of HandleXXX
	str     r0,[sp,#4]      ;store the contents(ISR) of HandleXXX to stack
	ldmfd   sp!,{r0,pc}     ;POP the work register and pc(jump to ISR)
HandlerSWI:
	sub	sp,sp,#4        ;decrement sp(to store jump address)
	stmfd	sp!,{r0}        ;PUSH the work register to stack(lr does't push because it return to original address)
	ldr     r0,=HandleSWI	;load the address of HandleXXX to r0
	ldr     r0,[r0]         ;load the contents(service routine start address) of HandleXXX
	str     r0,[sp,#4]      ;store the contents(ISR) of HandleXXX to stack
	ldmfd   sp!,{r0,pc}     ;POP the work register and pc(jump to ISR)
HandlerDabort:
	sub	sp,sp,#4        ;decrement sp(to store jump address)
	stmfd	sp!,{r0}        ;PUSH the work register to stack(lr does't push because it return to original address)
	ldr     r0,=HandleDabort	;load the address of HandleXXX to r0
	ldr     r0,[r0]         ;load the contents(service routine start address) of HandleXXX
	str     r0,[sp,#4]      ;store the contents(ISR) of HandleXXX to stack
	ldmfd   sp!,{r0,pc}     ;POP the work register and pc(jump to ISR)
HandlerPabort:
	sub	sp,sp,#4        ;decrement sp(to store jump address)
	stmfd	sp!,{r0}        ;PUSH the work register to stack(lr does't push because it return to original address)
	ldr     r0,=HandlePabort	;load the address of HandleXXX to r0
	ldr     r0,[r0]         ;load the contents(service routine start address) of HandleXXX
	str     r0,[sp,#4]      ;store the contents(ISR) of HandleXXX to stack
	ldmfd   sp!,{r0,pc}     ;POP the work register and pc(jump to ISR)

HandlerFIQ:
	sub	sp,sp,#4        ;decrement sp(to store jump address)
	stmfd	sp!,{r0}        ;PUSH the work register to stack(lr does't push because it return to original address)
	ldr     r0,=HandleFIQ	;load the address of HandleXXX to r0
	ldr     r0,[r0]         ;load the contents(service routine start address) of HandleXXX
	str     r0,[sp,#4]      ;store the contents(ISR) of HandleXXX to stack
	ldmfd   sp!,{r0,pc}     ;POP the work register and pc(jump to ISR)

;=======
; ENTRY, Initialize System Registers.
;=======
ResetHandler:
	ldr		r0,=WTCON       ;watch dog disable
	ldr		r1,=0x0
	str		r1,[r0]

	ldr		r0,=INTMSK
	ldr		r1,=0xffffffff  ;all interrupt disable
	str		r1,[r0]

	ldr		r0,=INTSUBMSK
	ldr		r1,=0x7fff		;all sub interrupt disable
	str		r1,[r0]

	;To reduce PLL lock time, adjust the LOCKTIME register.
	ldr		r0,=LOCKTIME
	ldr		r1,=0xffffff
	str		r1,[r0]
#if PLL_ON_START == 1
	; Added for confirm clock divide. for 2440.
	; Setting value Fclk:Hclk:Pclk
	ldr	r0,=CLKDIVN
	ldr	r1,=CLKDIV_VAL		; 0=1:1:1, 1=1:1:2, 2=1:2:2, 3=1:2:4, 4=1:4:4, 5=1:4:8, 6=1:3:3, 7=1:3:6.
	str	r1,[r0]
#if CLKDIV_VAL > 1 		; means Fclk:Hclk is not 1:1.
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000;R1_nF:OR:R1_iA
	mcr p15,0,r0,c1,c0,0
#else
	mrc p15,0,r0,c1,c0,0
	bic r0,r0,#0xc0000000;R1_iA:OR:R1_nF
	mcr p15,0,r0,c1,c0,0
#endif
	;Configure UPLL
	ldr	r0,=UPLLCON
	ldr	r1,=((U_MDIV<<12)+(U_PDIV<<4)+U_SDIV)  
	str	r1,[r0]
	nop	; Caution: After UPLL setting, at least 7-clocks delay must be inserted for setting hardware be completed.
	nop
	nop
	nop
	nop
	nop
	nop
	;Configure MPLL
	ldr	r0,=MPLLCON
	ldr	r1,=((M_MDIV<<12)+(M_PDIV<<4)+M_SDIV)
	str	r1,[r0]
#else
	;Configure MPLL
	ldr		r0,=MPLLCON
	ldr		r1,=0x0
	str		r1,[r0]
#endif

	;init the bus width control registers
	ldr	r0,=SMRDATA ;Set memory control registers
	ldr	r1,=BWSCON  ;BWSCON Address
	add	r2, r0, #52 ;End address of SMRDATA
SMRDATA_SET:
	ldr	r3, [r0], #4
	str	r3, [r1], #4
	cmp	r2, r0
	bne	SMRDATA_SET

InitStacks:
	mrs r0,cpsr
	bic r0,r0,#MODEMASK|NOINT

	orr r1,r0,#UNDEFMODE|NOINT
	msr cpsr_c,r1            ; UndefMode
	ldr SP, = SFE(UND_STACK)

	orr r1,r0,#ABORTMODE|NOINT
	msr cpsr_c,r1            ; AbortMode
	ldr SP, = SFE(ABT_STACK)

	orr r1,r0,#IRQMODE|NOINT
	msr cpsr_c,r1            ; IRQMode
	ldr SP, = SFE(IRQ_STACK)

	orr r1,r0,#FIQMODE|NOINT
	msr cpsr_c,r1            ; FIQMode
	ldr SP, =SFE(FIQ_STACK)

	orr r1,r0,#SVCMODE|NOINT
	msr cpsr_c,r1            ; SVCMode
	ldr SP, = SFE(SVC_STACK)

	orr r1,r0,#SYSMODE|NOINT
	msr cpsr_c,r1            ; USERMode/SYSMODE
	ldr SP, =SFE(CSTACK)

	orr r1,r0,#SVCMODE|NOINT
	msr cpsr_c,r1            ; SVCMode

	; Continue to Main for more IAR specific system startup
	b ?main

SMRDATA:
	DATA
	; Memory configuration should be optimized for best performance
	; The following parameter is not optimized.
	; Memory access cycle parameter strategy
	; 1) The memory settings is  safe parameters even at HCLK=75Mhz.
	; 2) SDRAM refresh period is for HCLK=75Mhz.
	DCD (0+(B1_BWSCON<<4)+(B2_BWSCON<<8)+(B3_BWSCON<<12)+(B4_BWSCON<<16)+(B5_BWSCON<<20)+(B6_BWSCON<<24)+(B7_BWSCON<<28))
	DCD ((B0_Tacs<<13)+(B0_Tcos<<11)+(B0_Tacc<<8)+(B0_Tcoh<<6)+(B0_Tah<<4)+(B0_Tacp<<2)+(B0_PMC))   ;GCS0
	DCD ((B1_Tacs<<13)+(B1_Tcos<<11)+(B1_Tacc<<8)+(B1_Tcoh<<6)+(B1_Tah<<4)+(B1_Tacp<<2)+(B1_PMC))   ;GCS1
	DCD ((B2_Tacs<<13)+(B2_Tcos<<11)+(B2_Tacc<<8)+(B2_Tcoh<<6)+(B2_Tah<<4)+(B2_Tacp<<2)+(B2_PMC))   ;GCS2
	DCD ((B3_Tacs<<13)+(B3_Tcos<<11)+(B3_Tacc<<8)+(B3_Tcoh<<6)+(B3_Tah<<4)+(B3_Tacp<<2)+(B3_PMC))   ;GCS3
	DCD ((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC))   ;GCS4
	DCD ((B5_Tacs<<13)+(B5_Tcos<<11)+(B5_Tacc<<8)+(B5_Tcoh<<6)+(B5_Tah<<4)+(B5_Tacp<<2)+(B5_PMC))   ;GCS5
	DCD ((B6_MT<<15)+(B6_Trcd<<2)+(B6_SCAN))    ;GCS6
	DCD ((B7_MT<<15)+(B7_Trcd<<2)+(B7_SCAN))    ;GCS7
	DCD ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Trc<<18)+(Tchr<<16)+REFCNT)
	DCD 0x32    ;SCLK power saving mode, BANKSIZE 128M/128M
	DCD 0x30    ;MRSR6 CL=3clk
	DCD 0x30    ;MRSR7 CL=3clk

	;this seg will be placed in ISR_VECTOR
	;ISR_VECTOR area is 0x3fffff00-0x3ffffff
	LTORG
	SECTION  ISR_VECTOR:DATA:ROOT 
_isr_init:
HandleReset 	DS32   1
HandleUndef 	DS32   1
HandleSWI   	DS32   1
HandlePabort    DS32   1
HandleDabort    DS32   1
HandleReserved  DS32   1
HandleIRQ   	DS32   1
HandleFIQ   	DS32   1

;Don't use the label 'IntVectorTable',
;The value of IntVectorTable is different with the address you think it may be.
;IntVectorTable
;@0x33FF_FF20
HandleEINT0		DS32   1
HandleEINT1		DS32   1
HandleEINT2		DS32   1
HandleEINT3		DS32   1
HandleEINT4_7	DS32   1
HandleEINT8_23	DS32   1
HandleCAM		DS32   1		; Added for 2440.
HandleBATFLT	DS32   1
HandleTICK		DS32   1
HandleWDT		DS32   1
HandleTIMER0	DS32   1
HandleTIMER1	DS32   1
HandleTIMER2	DS32   1
HandleTIMER3	DS32   1
HandleTIMER4	DS32   1
HandleUART2		DS32   1
;@0x33FF_FF60
HandleLCD 		DS32   1
HandleDMA0		DS32   1
HandleDMA1		DS32   1
HandleDMA2		DS32   1
HandleDMA3		DS32   1
HandleMMC		DS32   1
HandleSPI0		DS32   1
HandleUART1		DS32   1
HandleNFCON		DS32   1		; Added for 2440.
HandleUSBD		DS32   1
HandleUSBH		DS32   1
HandleIIC		DS32   1
HandleUART0		DS32   1
HandleSPI1		DS32   1
HandleRTC		DS32   1
HandleADC		DS32   1
HandleNOUSE 	DS32   24

	END
