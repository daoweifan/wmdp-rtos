#ifndef __WM_HW_SERIAL_H__
#define __WM_HW_SERIAL_H__

#include "def.h"
#include "devices.h"
#include "s3c2440x.h"

#define	USTAT_RCV_READY		0x01   	/* receive data ready */ 
#define	USTAT_TXB_EMPTY		0x02   	/* tx buffer empty */
#define BPS					115200	/* serial baudrate */

#define UART_RX_BUFFER_SIZE		64
#define UART_TX_BUFFER_SIZE		64


int wm_hw_serial_init(void);

#endif
