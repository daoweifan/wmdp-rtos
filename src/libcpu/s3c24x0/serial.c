#include "FreeRTOS.h"
#include "task.h"
#include "debug.h"
#include "wm_error.h"
#include "devices.h"
#include "serial.h"
#include "string.h"
#include "config.h"

/**
 * @addtogroup S3C24X0
 */
/*@{*/

typedef struct {
	volatile u32 ulcon;
	volatile u32 ucon;
	volatile u32 ufcon;
	volatile u32 umcon;
	volatile u32 ustat;
	volatile u32 urxb;
	volatile u32 ufstat;
	volatile u32 umstat;
	volatile u32 utxh;
	volatile u32 urxh;
	volatile u32 ubrd;
} uart_hw;

struct s3c24x0serial
{
	struct wm_device parent;

	/* uart module field */
	uart_hw * hw_base;
	u32 baudrate;

	/* isr field */
	void (* InitISR)(void);

	/* reception field */
	u16 save_index, read_index;
	u8  *pbuf;
};

/* serial0 device define and buffer allocate */
static struct s3c24x0serial serial0;
static u8 uart0_rx_buf[CONFIG_UART0_RF_SZ];

/* local private function declare */
static void serial0_isr_init(void);
static void serial0_isr_handler(void);

/* WM-Thread Device Interface */
/**
 * This function initializes serial
 */
static int wm_serial_init (wm_device_t dev)
{
	struct s3c24x0serial* serial = (struct s3c24x0serial*) dev;
	int i;

	/* global register protect, disable interrupt */
	taskENTER_CRITICAL();

	/* UART0, UART1 port configure */
	rGPHCON |= 0xAA;
	/* PULLUP is disable */
	rGPHUP |= 0xF;
	/* enable interrupt */

	if ((dev->flag & WM_DEVICE_FLAG_DMA_RX) || \
		(dev->flag & WM_DEVICE_FLAG_DMA_TX)) {
		/* FIFO enable, just use it like DMA */
		serial->hw_base->ufcon = 0x0001;
	} else {
		/* FIFO disable, Tx/Rx FIFO clear */
		serial->hw_base->ufcon = 0x0000;
	}
	/* disable the flow control */
	serial->hw_base->umcon = 0x0;
	/* Normal,No parity,1 stop,8 bit */
	serial->hw_base->ulcon = 0x3;
	/*
	 * tx=level,rx=edge,disable timeout int.,enable rx error int.,
	 * normal,interrupt or polling
	 */
	serial->hw_base->ucon = 0x105;
	/* Set uart0 bps */
	serial->hw_base->ubrd = (u32)(PCLK / (serial->baudrate * 16)) - 1;
	/* output PCLK to UART0/1, PWMTIMER */
	rCLKCON |= 0x0D00;

	for (i = 0; i < 100; i++);

	if (dev->flag & WM_DEVICE_FLAG_INT_RX) {
		memset(serial->pbuf, 0, sizeof(serial->pbuf));
		serial->read_index = serial->save_index = 0;
		serial->InitISR();
	}

	taskEXIT_CRITICAL();

	dev->flag |= WM_DEVICE_FLAG_ACTIVATED;

	return WM_EOK;
}

static int wm_serial_open(wm_device_t dev, u16 oflag)
{
	WM_ASSERT(dev != WM_NULL);

	return WM_EOK;
}

static int wm_serial_close(wm_device_t dev)
{
	WM_ASSERT(dev != WM_NULL);

	if (dev->flag & WM_DEVICE_FLAG_INT_RX)
	{
		/* disable interrupt */
		// todo:::::
	}
	/* reset uart module, we can disable it's clock to save energy */
	//todo :::::

	return WM_EOK;
}

static int wm_serial_read (wm_device_t dev, int pos, void* buffer, int size)
{
	struct s3c24x0serial* serial = (struct s3c24x0serial*) dev;
	u8* ptr = (u8 *)buffer;
	int err_code = WM_EOK;

	if (dev->flag & WM_DEVICE_FLAG_INT_RX) {
		/* interrupt mode Rx */
		while (size) {
			if (serial->read_index != serial->save_index) {
				*ptr++ = serial->pbuf[serial->read_index];
				size --;

				/* disable interrupt */
				taskENTER_CRITICAL();

				serial->read_index ++;
				if (serial->read_index >= CONFIG_UART0_RF_SZ)
					serial->read_index = 0;

				/* enable interrupt */
				taskEXIT_CRITICAL();
			} else {
				/* set error code */
				err_code = -WM_EEMPTY;
				break;
			}
		}
	} else if (dev->flag & WM_DEVICE_FLAG_DMA_RX) {
		/* fifo polling mode, don't block */
		while (serial->hw_base->ufstat & 0x003f) {
			*ptr = serial->hw_base->urxh & 0xff;
			ptr ++;
		}
	} else {
		/* polling mode, don't block */
		// while ((u32)ptr - (u32)buffer < size) {
		while (serial->hw_base->ustat & USTAT_RCV_READY) {
			*ptr = serial->hw_base->urxh & 0xff;
			ptr ++;
		}
		// }
	}

	/* set error code */
	wm_set_errno(err_code);
	return (u32)ptr - (u32)buffer;
}

static int wm_serial_write (wm_device_t dev, int pos, const void* buffer, int size)
{
	struct s3c24x0serial* serial = (struct s3c24x0serial*) dev;
	u8* ptr = (u8 *)buffer;
	int err_code = WM_EOK;

	/* polling mode */
	if (dev->flag & WM_DEVICE_FLAG_DMA_TX) {
		if (size < S3C24X0_UART_FIFO_SIZE - (serial->hw_base->ufstat & 0x3f00)) {
			while (size) {
				if (*ptr == '\n') {
					serial->hw_base->utxh = '\r';
					if ((serial->hw_base->ufstat & 0x3f00) == S3C24X0_UART_FIFO_SIZE)
						break;
				}
				serial->hw_base->utxh = (*ptr & 0xFF);
				++ptr;
				--size;
			}
		} else {
			while (size & (serial->hw_base->ufstat & 0x3f00) < S3C24X0_UART_FIFO_SIZE) {
				if (*ptr == '\n') {
					serial->hw_base->utxh = '\r';
					if ((serial->hw_base->ufstat & 0x3f00) == S3C24X0_UART_FIFO_SIZE)
						break;
				}
				serial->hw_base->utxh = (*ptr & 0xFF);
				++ptr;
				--size;
			}
			if (((u32)ptr - (u32)buffer) < size)
				err_code = -WM_EEMPTY;
		}
	} else {
		while (size) {
			/*
			 * to be polite with serial console add a line feed
			 * to the carriage return character
			 */
			if (*ptr == '\n' && (dev->flag & WM_DEVICE_FLAG_STREAM)) {
				while (!(serial->hw_base->ustat & USTAT_TXB_EMPTY));
				serial->hw_base->utxh = '\r';
			}

			while (!(serial->hw_base->ustat & USTAT_TXB_EMPTY));
			serial->hw_base->utxh = (*ptr & 0xFF);

			++ptr;
			--size;
		}
	}

	/* set error code */
	wm_set_errno(err_code);
	return (u32)ptr - (u32)buffer;
}

static int wm_serial_control (wm_device_t dev, u8 cmd, void *args)
{
	WM_ASSERT(dev != RT_NULL);

	switch (cmd) {
	case WM_DEVICE_CTRL_SUSPEND:
		/* suspend device */
		dev->flag |= WM_DEVICE_FLAG_SUSPENDED;
		break;
	
	case WM_DEVICE_CTRL_RESUME:
		/* resume device */
		dev->flag &= ~WM_DEVICE_FLAG_SUSPENDED;
		break;
	
	default:
		break;
	}
	
	return WM_EOK;
}

/*
 * serial register
 */
int wm_hw_serial_init(void)
{
	int result;
	wm_device_t device;
	device = (wm_device_t) &serial0;

	/* init serial0 device private data */
	serial0.hw_base		= (uart_hw *) &U0BASE; 
	serial0.baudrate	= CONFIG_UART0_BR;
	serial0.InitISR		= serial0_isr_init;
	serial0.pbuf		= uart0_rx_buf;

	/* set device virtual interface */
	device->type 		= WM_Device_Class_Char;
	device->rx_indicate = WM_NULL;
	device->tx_complete = WM_NULL;
	device->init	 	= wm_serial_init;
	device->open 		= wm_serial_open;
	device->close 		= wm_serial_close;
	device->read 		= wm_serial_read;
	device->write 		= wm_serial_write;
	device->control 	= wm_serial_control;

	/* register uart0 on device subsystem */
	result = wm_device_register(device, "uart0", \
								WM_DEVICE_FLAG_RDWR | \
								WM_DEVICE_FLAG_STREAM);

	return result;
}

/* serial0 isr init, for every uart has a different init routine */
static void serial0_isr_init(void)
{
	ClearPending(BIT_UART0);
	ClearSubPending(BIT_SUB_RXD0);
	/* install uart0 isr */
	pISR_UART0 =  (unsigned int) serial0_isr_handler;
	EnableIrq(BIT_UART0);
	EnableSubIrq(BIT_SUB_RXD0);
}

/* ISR for serial interrupt */
static void serial0_isr_handler(void)
{
	struct s3c24x0serial* serial = &serial0;
	wm_device_t device = (wm_device_t) &serial0;
	
	/* interrupt mode receive */
	WM_ASSERT(device->flag & WM_DEVICE_FLAG_INT_RX);

	/* save on rx buffer */
	while (serial->hw_base->ustat & USTAT_RCV_READY) {
		serial->pbuf[serial->save_index] = serial->hw_base->urxh & 0xff;
		serial->save_index++;
		if (serial->save_index >= CONFIG_UART0_RF_SZ)
			serial->save_index = 0;
	
		/* if the next position is read index, discard this 'read char' */
		if (serial->save_index == serial->read_index)
		{
			serial->read_index ++;
			if (serial->read_index >= CONFIG_UART0_RF_SZ)
				serial->read_index = 0;
		}
	}

	/* invoke callback */
	if (device->rx_indicate != WM_NULL) {
		int rx_length;

		/* get rx length */
		rx_length = serial->read_index > serial->save_index ?
			CONFIG_UART0_RF_SZ - serial->read_index + serial->save_index :
			serial->save_index - serial->read_index;

		device->rx_indicate(device, rx_length);
	}

	ClearSubPending(BIT_SUB_RXD0);
	ClearPending(BIT_UART0);
}

/*@}*/
