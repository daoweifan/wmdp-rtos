/*
 * init version, @dusk 
 * 2013,2,26
 */
#include "FreeRTOS.h"
#include "task.h"
#include "serial.h"

extern void s3c2440_isr_Init(void);
extern void WDT_IRQInit(unsigned int wTicksPerSec);
#if 0
static void print_Hello( void *pvParameters );
static void print_haha( void *pvParameters );
static char gcs;
#endif

void board_Init(void)
{
	s3c2440_isr_Init();
	wm_hw_serial_init();
}

void main(void)
{
	u8 ch;
	board_Init();
	wm_device_t uart;
	uart = wm_device_find_by_name("uart0");
	wm_device_init(uart);
	portENABLE_INTERRUPTS();
	wm_device_write(uart, 0, "this device serial", 10);
	while(1) {
		if (wm_device_read(uart, 0, &ch, 1) > 0)
			wm_device_write(uart, 0, &ch, 1);
	}
	// WDT_IRQInit(10);
	// portENABLE_INTERRUPTS();
	
	// #pragma asm
	
	// asm("mov r0, #18 \n"
		// "SWI 0x0"
		// );
	
	// #pragma endasm
	/* Create task */
	// xTaskCreate( print_Hello, "hello", 128, NULL, tskIDLE_PRIORITY + 3, ( xTaskHandle * ) NULL );
	// xTaskCreate( print_haha, "haha", 128, NULL, tskIDLE_PRIORITY + 4, ( xTaskHandle * ) NULL );
	/* Start the scheduler. */
	// vTaskStartScheduler();

	/* We should never get here as control is now taken by the scheduler. */
	// while(1);
}
#if 0
static void print_Hello( void *pvParameters )
{
	(void *)pvParameters;
	int cnt;
	for(;;)
	{
		/* Delay for half the flash period then turn the LED on. */
		vTaskDelay(100);
		cnt = xTaskGetTickCount();
		// cnt ++;
		uart0.printf("systick: %d\n\r", cnt);
		uart0.printf("checksum: %x\n\r", gcs);
		// uart0.putstring("os tick running\n\r");
	}
}

static void print_haha( void *pvParameters )
{
	(void *)pvParameters;
	char cnt[20];
	char cs;
	int i;
	for(;;)
	{
		for (i=0; i<20; i++) {
			cs += cnt[i];
			gcs = cs;
		}
		/* Delay for half the flash period then turn the LED on. */
		vTaskDelay(100);
	}
}
#endif
