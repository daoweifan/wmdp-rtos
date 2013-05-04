/*
 * File      : devices.h
 * This file is part of WMDP
 * COPYRIGHT (C) 2013 - 2100, WMDP Development Team
 * Change Logs:
 * Date           Author       Notes
 * 2013-04-21     Daowei Fan   the first version
 */

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "wm_list.h"
#include "debug.h"
#include "devices.h"


static LIST_HEAD(devs_head);

/**
 * This function registers a device driver with specified name.
 *
 * @param dev the pointer of device driver structure
 * @param name the device driver's name
 * @param flags the flag of device
 *
 * @return the error code, WM_EOK on initialization successfully.
 */
 int wm_device_register (wm_device_t dev, const char *name, u16 flags)
{
	if (sizeof(name) > WM_NAME_MAX)
		return WM_ERROR;
	if (name != NULL) {
		strcpy(dev->name, name);
		dev->flag = flags;
	}
	list_add_tail(&(dev->list), &devs_head);

	return WM_EOK;
}

/**
 * This function removes a previously registered device driver
 *
 * @param dev the pointer of device driver structure
 *
 * @return the error code, WM_EOK on successfully.
 */
int wm_device_unregister(const char *devname)
{
	wm_device_t dev;

	WM_ASSERT(devname != NULL);

	dev = wm_device_find_by_name(devname);

	list_del(&(dev->list));

	return WM_EOK;
}


/**
 * This function initializes all registered device driver
 *
 * @return the error code, RT_EOK on successfully.
 */
int wm_device_init_all(void)
{
	struct wm_device *dev;
	struct list_head *pos;
	int (*init)(wm_device_t dev);
	int result = WM_EOK;

	list_for_each(pos, &devs_head) {
		dev = list_entry(pos, struct wm_device, list);
		init = dev->init;
		if (init != NULL && !(dev->flag & WM_DEVICE_FLAG_ACTIVATED)) {
			result = init(dev);
			dev->flag |= WM_DEVICE_FLAG_ACTIVATED;
		}
	}

	return result;
}

/**
 * This function finds a device driver by specified name.
 *
 * @param name the device driver's name
 *
 * @return the registered device driver on successful, or WM_NULL on failure.
 */
wm_device_t wm_device_find_by_name(const char* name)
{
	struct list_head *pos;
	wm_device_t dev;

	if(!name)
		return WM_NULL;

	list_for_each(pos, &devs_head) {
		dev = list_entry(pos, struct wm_device, list);
		if(strcmp(dev->name, name) == 0)
			return dev;
	}

	return WM_NULL;
}

/**
 * This function will initialize the specified device
 *
 * @param dev the pointer of device driver structure
 * 
 * @return the result
 */
int wm_device_init(wm_device_t dev)
{
	int result = WM_EOK;
	int (*init)(wm_device_t dev);

	WM_ASSERT(dev != NULL);

	/* get device init handler */
	init = dev->init;
	if ((init != NULL) && !(dev->flag & WM_DEVICE_FLAG_ACTIVATED)) {
		result = init(dev);
		dev->flag |= WM_DEVICE_FLAG_ACTIVATED;
	} else {
		result = -WM_ENOSYS;
	}

	return result;
}


/**
 * This function will open a device
 *
 * @param dev the pointer of device driver structure
 * @param oflag the flags for device open
 *
 * @return the result
 */
int wm_device_open(wm_device_t dev, u16 oflag)
{
	int result;
	int (*open)(wm_device_t dev, u16 oflag);

	WM_ASSERT(dev != NULL);

	result = WM_EOK;

	/* if device is not initialized, initialize it. */
	if (!(dev->flag & WM_DEVICE_FLAG_ACTIVATED)) {
		result = dev->init(dev);
		if (result != WM_EOK) {
			return result;
		} else {
			dev->flag |= WM_DEVICE_FLAG_ACTIVATED;
		}
	}

	/* device is a stand alone device and opened */
	if ((dev->flag & WM_DEVICE_FLAG_STANDALONE) && (dev->open_flag & WM_DEVICE_OFLAG_OPEN))
		return -WM_EBUSY;

	/* call device open interface */
	open = dev->open;
	if (open != WM_NULL) {
		result = open(dev, oflag);
	} else {
		/* no this interface in device driver */
		result = -WM_ENOSYS;
	}

	/* set open flag */
	if (result == WM_EOK || result == -WM_ENOSYS)
		dev->open_flag = oflag | WM_DEVICE_OFLAG_OPEN;

	return result;
}

/**
 * This function will close a device
 *
 * @param dev the pointer of device driver structure
 *
 * @return the result
 */
int wm_device_close(wm_device_t dev)
{
	int result;
	int (*close)(wm_device_t dev);

	WM_ASSERT(dev != WM_NULL);

	/* call device close interface */
	close = dev->close;
	if (close != WM_NULL) {
		result = close(dev);
	} else {
		/* no this interface in device driver */
		result = -WM_ENOSYS;
	}

	/* set open flag */
	if (result == WM_EOK || result == -WM_ENOSYS)
		dev->open_flag = WM_DEVICE_OFLAG_CLOSE;

	return result;
}

/**
 * This function will read some data from a device.
 *
 * @param dev the pointer of device driver structure
 * @param pos the position of reading
 * @param buffer the data buffer to save read data
 * @param size the size of buffer
 *
 * @return the actually read size on successful, otherwise negative returned.
 *
 * @note since 0.4.0, the unit of size/pos is a block for block device.
 */
int wm_device_read(wm_device_t dev, int pos, void *buffer, int size)
{
	int (*read)(wm_device_t dev, int pos, void *buffer, int size);

	WM_ASSERT(dev != WM_NULL);

	/* call device read interface */
	read = dev->read;
	if (read != WM_NULL) {
		return read(dev, pos, buffer, size);
	}

	/* set error code */
	return -WM_ENOSYS;
}

/**
 * This function will write some data to a device.
 *
 * @param dev the pointer of device driver structure
 * @param pos the position of written
 * @param buffer the data buffer to be written to device
 * @param size the size of buffer
 *
 * @return the actually written size on successful, otherwise negative returned.
 *
 * @note since 0.4.0, the unit of size/pos is a block for block device.
 */
int wm_device_write(wm_device_t dev, int pos, const void *buffer, int size)
{
	int (*write)(wm_device_t dev, int pos, const void *buffer, int size);

	WM_ASSERT(dev != WM_NULL);

	/* call device write interface */
	write = dev->write;
	if (write != WM_NULL) {
		return write(dev, pos, buffer, size);
	}

	/* set error code */
	return -WM_ENOSYS;
}

/**
 * This function will perform a variety of control functions on devices.
 *
 * @param dev the pointer of device driver structure
 * @param cmd the command sent to device
 * @param arg the argument of command
 *
 * @return the result
 */
int wm_device_control(wm_device_t dev, u8 cmd, void *arg)
{
	int (*control)(wm_device_t dev, u8 cmd, void *arg);

	WM_ASSERT(dev != WM_NULL);

	/* call device write interface */
	control = dev->control;
	if (control != WM_NULL) {
		return control(dev, cmd, arg);
	}

	return -WM_ENOSYS;
}

/**
 * This function will set the indication callback function when device receives
 * data.
 *
 * @param dev the pointer of device driver structure
 * @param rx_ind the indication callback function
 *
 * @return RT_EOK
 */
int wm_device_set_rx_indicate(wm_device_t dev, int (*rx_ind)(wm_device_t dev, int size))
{
	WM_ASSERT(dev != WM_NULL);

	dev->rx_indicate = rx_ind;
	return WM_EOK;
}

/**
 * This function will set the indication callback function when device has written
 * data to physical hardware.
 *
 * @param dev the pointer of device driver structure
 * @param tx_done the indication callback function
 *
 * @return RT_EOK
 */
int wm_device_set_tx_complete(wm_device_t dev, int (*tx_done)(wm_device_t dev, void *buffer))
{
	WM_ASSERT(dev != WM_NULL);

	dev->tx_complete = tx_done;
	return WM_EOK;
}

struct list_head* device_get_list(void)
{
	return &(devs_head);
}

