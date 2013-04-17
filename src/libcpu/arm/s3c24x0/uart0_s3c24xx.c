/*
 * Author    : dusk
 * dusk@2010, api update to new format
 */
 
#include "S3c2440x.h"
#include "uart.h"
#include "option.h"

static int uart_Init(const uart_cfg_t *cfg)
{
	//gpio set,GPH2,GPH3,
	//Ports  :  GPH10    GPH9  GPH8 GPH7  GPH6  GPH5 GPH4 GPH3 GPH2 GPH1  GPH0 
	//Signal : CLKOUT1 CLKOUT0 UCLK nCTS1 nRTS1 RXD1 TXD1 RXD0 TXD0 nRTS0 nCTS0
	//Binary :   10   ,  10     10 , 11    11  , 10   10 , 10   10 , 10    10
	rGPHCON &= 0xffffff0f;
	rGPHCON |= ((0x02 << 4) | (0x02 << 6));
	rGPHUP  |= ((0x01 << 2) | (0x01 << 3));

	//register set
	rUFCON0 = (3<<6) | (3<<4) | (1<<2) | (1<<1) | (1<<0);   //UART channel 0 FIFO control register
	rUMCON0 = 0;
	rULCON0 = 0x3;   //Line control register : Normal,No parity,1 stop,8 bits
	rUCON0  = 0x245; // Control register
	rUBRDIV0=( (int)((PCLK)/16./cfg->baud+0.5) -1 );   //Baud rate divisior register 0

	return 0;
}

static int uart_GetCh(void)
{
	while(!(rUFSTAT0 & 0x0f)); //Receive data ready
	return rURXH0;
}

static int uart_Poll(void)
{
	if(rUFSTAT0 & 0x0f)
		return 1;
	else
		return 0;
}

static int uart_SendCh(int bData)
{
	while(rUFSTAT0 & 0x200); //Wait until THR is empty.
	//Delay(4);
	rUTXH0 = bData;

	return 0;
}

static int uart_SendString(char *pbString)
{
	while(*pbString)
		uart_SendCh(*pbString++);
	return 0;
}

uart_bus_t uart0 = {
	.init = uart_Init,
	.putchar = uart_SendCh,
	.putstring = uart_SendString,
	.getchar = uart_GetCh,
	.poll = uart_Poll,
};
