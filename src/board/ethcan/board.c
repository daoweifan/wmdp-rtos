/*
 * init version, @dusk 
 * 2013,2,26
 */
#include "FreeRTOS.h"
#include "task.h"
#include "serial.h"

extern void s3c2440_isr_Init(void);
extern void WDT_IRQInit(unsigned int wTicksPerSec);

static void print_Hello( void *pvParameters );
static void print_Haha( void *pvParameters );

void board_Init(void)
{
	s3c2440_isr_Init();
	wm_hw_serial_init();
}

void main(void)
{
	board_Init();

	/* Create task */
	xTaskCreate( print_Hello, "hello", 128, NULL, tskIDLE_PRIORITY + 3, ( xTaskHandle * ) NULL );
	// xTaskCreate( print_haha, "haha", 128, NULL, tskIDLE_PRIORITY + 4, ( xTaskHandle * ) NULL );
	/* Start the scheduler. */
	vTaskStartScheduler();

	/* We should never get here as control is now taken by the scheduler. */
	while(1);
}

static void print_Hello( void *pvParameters )
{
	(void *)pvParameters;
	int num = 0;
	u8 str[] = "here i am, this is me!";
	wm_device_t uart;
	uart = wm_device_find_by_name("uart0");
	wm_device_init(uart);
	wm_device_write(uart, 0, "this device serial\r", sizeof("this device serial\r"));
	for(;;)
	{
		/* Delay for half the flash period then turn the LED on. */
		vTaskDelay(100);
		// num = wm_device_read(uart, 0, str, 16);
		wm_device_write(uart, 0, str, num++);
		wm_device_write(uart, 0, "\n\r", 2);
		if (num >= 16)
			num = 0;
	}
}
#if 0
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
