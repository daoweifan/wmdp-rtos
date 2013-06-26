/*
 * init version, @dusk 
 * 2013,2,26
 */
#include "FreeRTOS.h"
#include "task.h"
#include "serial.h"
#include "kservice.h"

extern void s3c2440_isr_Init(void);
extern void WDT_IRQInit(unsigned int wTicksPerSec);

extern void finsh_system_init(void);

static void print_Hello( void *pvParameters ); 
// static void print_Haha( void *pvParameters );

void board_Init(void)
{
	s3c2440_isr_Init();
	wm_hw_serial_init();

	/*init all registed devices */
	wm_device_init_all();

	wm_device_t uart;
	uart = wm_device_find_by_name("uart0");
	wm_device_init(uart);
	wm_console_set_device("uart0");
}

void main(void)
{
	board_Init();

	/* Create task */
	xTaskCreate( print_Hello, "hello", 128, NULL, tskIDLE_PRIORITY + 3, ( xTaskHandle * ) NULL );
	// xTaskCreate( print_haha, "haha", 128, NULL, tskIDLE_PRIORITY + 4, ( xTaskHandle * ) NULL );
	/* Start the scheduler. */
	finsh_system_init();
	vTaskStartScheduler();

	/* We should never get here as control is now taken by the scheduler. */
	while(1);
}

static void print_Hello( void *pvParameters )
{
	(void *)pvParameters;
	u8 str[] = "here i am, this is me!";
	wm_device_t uart;
	uart = wm_device_find_by_name("uart0");
	wm_device_init(uart);
	wm_device_write(uart, 0, "this device serial\r", sizeof("this device serial\r"));
	wm_console_set_device("uart0");
	for(;;)
	{
		/* Delay for half the flash period then turn the LED on. */
		vTaskDelay(100);
		// num = wm_device_read(uart, 0, str, 16);
		wm_kprintf("%s, %d \n\r", str, xTaskGetTickCount());
		
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
