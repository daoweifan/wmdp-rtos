		RSEG ICODE:CODE
		CODE32

	EXTERN vTaskSwitchContext
	EXTERN C_IRQHandler

	PUBLIC vPortYieldProcessor
	PUBLIC vPortStartFirstTask
	PUBLIC FREERTOS_IRQHandler
	PUBLIC UCOS_IRQHandler

#include "ISR_Support.h"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Starting the first task is just a matter of restoring the context that
; was created by pxPortInitialiseStack().
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
vPortStartFirstTask:
	portRESTORE_CONTEXT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Manual context switch function.  This is the SWI hander.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
vPortYieldProcessor:
	ADD		LR, LR, #4			; Add 4 to the LR to make the LR appear exactly
								; as if the context was saved during and IRQ
								; handler.
								
	portSAVE_CONTEXT			; Save the context of the current task...
	LDR R0, =vTaskSwitchContext	; before selecting the next task to execute.
	mov     lr, pc
	BX R0
	portRESTORE_CONTEXT			; Restore the context of the selected task.


UCOS_IRQHandler:
	stmfd sp!,{r0-r3,r12,lr}
	bl C_IRQHandler
	ldmfd sp!,{r0-r3,r12,lr}
	subs pc,lr,#4

FREERTOS_IRQHandler:
	portSAVE_CONTEXT			; Save the context of the current task.
	bl C_IRQHandler 			; Call C_IRQHandler
	portRESTORE_CONTEXT 		; Restore the context of the selected task.

	END

