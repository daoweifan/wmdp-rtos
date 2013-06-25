/*
 * File      : kservice.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2011, RT-Thread Development Team
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-16     Bernard      the first version
 */

#ifndef __RT_SERVICE_H__
#define __RT_SERVICE_H__

#include "devices.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup KernelService
 */
/*@{*/

void wm_show_version(void);
wm_device_t wm_console_get_device(void);
wm_device_t wm_console_set_device(const char *name);
void wm_kprintf(const char *fmt, ...);


#ifdef __cplusplus
}
#endif

#endif


