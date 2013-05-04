/**************************************************************************
*   PROJECT     : ARM port for UCOS-II                                    *
*   AUTHOR      : daoweifan dusk                                          *
*                 EMAIL: daoweifan@hotmail.com                            *
**************************************************************************/
#include <stdio.h>
/*
 * init version, @dusk 
 * 2011,10,4
 */
#include "def.h"
#include "s3c2440x.h"
#include "option.h"

extern void FREERTOS_IRQHandler(void);
extern void UCOS_IRQHandler(void);
extern void vPortYieldProcessor(void);

static void __HaltUndef(void)
{
	printf("Undefined instruction exception!!!\n\r");
	while(1);
}

static void __HaltSwi(void)
{
	printf("SWI exception!!!\n\r");
	while(1);
}

static void __HaltPabort()
{
	printf("Pabort exception!!!\n\r");
	while(1);
}

static void __HaltDabort(void)
{
	printf("Dabort exception !!!\n\r");
	while(1);
}

static void __HaltFiq(void)
{
	printf("FIQ exception!!!\n\r");
	while(1);
}

static void __HaltIrq(void)
{
	printf("IRQ exception!!!\n\r");
	while(1);
}

static void HaltHandlers_Init(void)
{
	pISR_UNDEF = (unsigned)__HaltUndef;
	pISR_SWI   = (unsigned)__HaltSwi;
	pISR_PABORT= (unsigned)__HaltPabort;
	pISR_DABORT= (unsigned)__HaltDabort;
	pISR_FIQ   = (unsigned)__HaltFiq;
	pISR_IRQ   = (unsigned)__HaltIrq;
}


void C_IRQHandler(void)
{
	unsigned int wTemp;
	void (*pISRFunction)(void);
	wTemp = rINTOFFSET<<2;
	pISRFunction = (void(*)(void))(*(unsigned int *)(aISR_EINT0+wTemp));

#if	DEBUG_ISR
	// printf("offset = 0x%x, pISRFunction = 0x%x\n\r", wTemp, pISRFunction);
#endif

	(*pISRFunction)();
}

void s3c2440_isr_Init(void)
{
	HaltHandlers_Init();

	rINTMOD		= 0x0;				//All=IRQ mode
	rINTMSK		= BIT_ALLMSK;		//All interrupt is masked
	rINTSUBMSK	= BIT_SUB_ALLMSK;	//All sub-interrupt is masked

	pISR_IRQ = (unsigned int)FREERTOS_IRQHandler;
	pISR_SWI = (unsigned int)vPortYieldProcessor;
	// pISR_IRQ = (unsigned int)UCOS_IRQHandler;
}

