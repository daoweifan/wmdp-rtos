/*
 * (C) Copyright 2013
 * Daowei Fan
 */
#include "def.h"
#include "wm_list.h"
 
#ifndef _DEVICES_H_
#define _DEVICES_H_

#define WM_NAME_MAX 0x08

/**
 * device (I/O) class type
 */
enum wm_device_class_type {
	WM_Device_Class_Char = 0,                       /**< character device                           */
	WM_Device_Class_Block,                          /**< block device                               */
	WM_Device_Class_NetIf,                          /**< net interface                              */
	WM_Device_Class_MTD,                            /**< memory device                              */
	WM_Device_Class_CAN,                            /**< CAN device                                 */
	WM_Device_Class_RTC,                            /**< RTC device                                 */
	WM_Device_Class_Sound,                          /**< Sound device                               */
	WM_Device_Class_Graphic,                        /**< Graphic device                             */
	WM_Device_Class_I2C,                            /**< I2C device                                 */
	WM_Device_Class_USBDevice,                      /**< USB slave device                           */
	WM_Device_Class_USBHost,                        /**< USB host bus                               */
	WM_Device_Class_SPIBUS, 						/**< SPI bus device                             */
	WM_Device_Class_SPIDevice,                      /**< SPI device                                 */
	WM_Device_Class_SDIO, 							/**< SDIO bus device                            */
	WM_Device_Class_Unknown                         /**< unknown device                             */
};


/**
 * device flags defitions
 */
#define WM_DEVICE_FLAG_DEACTIVATE       0x000       /**< device is not not initialized              */

#define WM_DEVICE_FLAG_RDONLY           0x001       /**< read only                                  */
#define WM_DEVICE_FLAG_WRONLY           0x002       /**< write only                                 */
#define WM_DEVICE_FLAG_RDWR             0x003       /**< read and write                             */

#define WM_DEVICE_FLAG_REMOVABLE        0x004       /**< removable device                           */
#define WM_DEVICE_FLAG_STANDALONE       0x008       /**< standalone device                          */
#define WM_DEVICE_FLAG_ACTIVATED        0x010       /**< device is activated                        */
#define WM_DEVICE_FLAG_SUSPENDED        0x020       /**< device is suspended                        */
#define WM_DEVICE_FLAG_STREAM           0x040       /**< stream mode                                */

#define WM_DEVICE_FLAG_INT_RX           0x100       /**< INT mode on Rx                             */
#define WM_DEVICE_FLAG_DMA_RX           0x200       /**< DMA mode on Rx                             */
#define WM_DEVICE_FLAG_INT_TX           0x400       /**< INT mode on Tx                             */
#define WM_DEVICE_FLAG_DMA_TX           0x800       /**< DMA mode on Tx                             */

#define WM_DEVICE_OFLAG_CLOSE           0x000       /**< device is closed                           */
#define WM_DEVICE_OFLAG_RDONLY          0x001       /**< read only access                           */
#define WM_DEVICE_OFLAG_WRONLY          0x002       /**< write only access                          */
#define WM_DEVICE_OFLAG_RDWR            0x003       /**< read and write                             */
#define WM_DEVICE_OFLAG_OPEN            0x008       /**< device is opened                           */

/**
 * general device commands
 */
#define WM_DEVICE_CTRL_RESUME           0x01        /**< resume device                              */
#define WM_DEVICE_CTRL_SUSPEND          0x02        /**< suspend device                             */

/**
 * special device commands
 */
#define WM_DEVICE_CTRL_CHAR_STREAM      0x10        /**< stream mode on char device                 */
#define WM_DEVICE_CTRL_BLK_GETGEOME     0x10        /**< get geometry information                   */
#define WM_DEVICE_CTRL_NETIF_GETMAC     0x10        /**< get mac address                            */
#define WM_DEVICE_CTRL_MTD_FORMAT       0x10        /**< format a MTD device                        */
#define WM_DEVICE_CTRL_RTC_GET_TIME     0x10        /**< get time                                   */
#define WM_DEVICE_CTRL_RTC_SET_TIME     0x11        /**< set time                                   */


typedef struct wm_device *wm_device_t;
/**
 * Device structure
 */
struct wm_device {
	char name[WM_NAME_MAX];                      /**< name of kernel object      */
	enum wm_device_class_type type;                 /**< device type                                */
	unsigned short flag, open_flag;                    /**< device flag and device open flag           */

	/* device call back */
	int (*rx_indicate)(wm_device_t dev, int size);
	int (*tx_complete)(wm_device_t dev, void* buffer);

	/* common device interface */
	int (*init)   (wm_device_t dev);
	int (*open)   (wm_device_t dev, u16 oflag);
	int (*close)  (wm_device_t dev);
	int (*read)   (wm_device_t dev, int pos, void *buffer, int size);
	int (*write)  (wm_device_t dev, int pos, const void *buffer, int size);
	int (*control)(wm_device_t dev, unsigned char cmd, void *args);

#ifdef WM_USING_DEVICE_SUSPEND
	int (*suspend) (wm_device_t dev);
	int (*resumed) (wm_device_t dev);
#endif

	void *user_data;                                /**< device private data */
	struct list_head list;
};

/* init head of device list table */
struct list_head* device_get_list(void);
int devices_init (void);

wm_device_t wm_device_find_by_name(const char *name);
int wm_device_register(wm_device_t dev, const char *name, u16 flags);
int wm_device_unregister(const char *devname);
int wm_device_init_all(void);

int wm_device_set_rx_indicate(wm_device_t dev, int (*rx_ind )(wm_device_t dev, int size));
int wm_device_set_tx_complete(wm_device_t dev, int (*tx_done)(wm_device_t dev, void *buffer));

int wm_device_init(wm_device_t dev);
int wm_device_open(wm_device_t dev, u16 oflag);
int wm_device_close(wm_device_t dev);
int wm_device_read(wm_device_t dev, int pos, void *buffer, int size);
int wm_device_write(wm_device_t dev, int pos, const void *buffer, int size);
int wm_device_control(wm_device_t dev, u8 cmd, void *arg);

#endif /* _DEVICES_H_ */


