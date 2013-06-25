/*
 * File      : kservice.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2011, RT-Thread Development Team
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-16     Bernard      the first version
 */

#include <stdarg.h>
#include <stdio.h>
#include "devices.h"
#include "debug.h"
#include "kservice.h"

static wm_device_t _console_device = WM_NULL;

/**
 * This function will show the version of rt-thread rtos
 */
void wm_show_version(void)
{
	wm_kprintf("\n \\ | /\n");
	wm_kprintf("- WMDP -     WM Develop Platform\n");
	wm_kprintf(" 2006 - 2011 Copyright by WM team\n");
}


/**
 * This function returns the device using in console.
 *
 * @return the device using in console or RT_NULL
 */
wm_device_t wm_console_get_device(void)
{
	return _console_device;
}

/**
 * This function will set a device as console device.
 * After set a device to console, all output of rt_kprintf will be
 * redirected to this new device.
 *
 * @param name the name of new console device
 *
 * @return the old console device handler
 */
wm_device_t wm_console_set_device(const char *name)
{
	wm_device_t new, old;

	/* save old device */
	old = _console_device;

	/* find new console device */
	new = wm_device_find_by_name(name);
	if (new != WM_NULL) {
		if (_console_device != WM_NULL) {
			/* close old console device */
			wm_device_close(_console_device);
		}

		/* set new console device */
		_console_device = new;
		wm_device_open(_console_device, WM_DEVICE_OFLAG_RDWR);
	}

	return old;
}

/**
 * This function will print a formatted string on system console
 *
 * @param fmt the format
 */
void wm_kprintf(const char *fmt, ...)
{
	static char wm_log_buf[64];
	int length;
	va_list ap;

	va_start(ap, fmt);
	length = vsprintf(wm_log_buf, fmt, ap);//Msg formate
	WM_ASSERT(_console_device);
	wm_device_write(_console_device, 0, wm_log_buf, length);
	va_end(ap);
}

