/*
 * File      : shell.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2011, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-06-02     Bernard      Add finsh_get_prompt function declaration
 */

#ifndef __SHELL_H__
#define __SHELL_H__

#include "config.h"
#include "finsh.h"
#include "devices.h"
#include "def.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#define FINSH_USING_HISTORY
#ifndef FINSH_THREAD_PRIORITY
#define FINSH_THREAD_PRIORITY 20
#endif
#ifndef FINSH_THREAD_STACK_SIZE
#define FINSH_THREAD_STACK_SIZE 2048
#endif
#define FINSH_CMD_SIZE		80

#define FINSH_OPTION_ECHO	0x01

#define FINSH_PROMPT		"finsh>>"

#ifdef FINSH_USING_HISTORY
enum input_stat
{
	WAIT_NORMAL,
	WAIT_SPEC_KEY,
	WAIT_FUNC_KEY,
};
	#ifndef FINSH_HISTORY_LINES
		#define FINSH_HISTORY_LINES 5
	#endif
#endif

struct finsh_shell
{
	// struct rt_semaphore rx_sem;
	xSemaphoreHandle rx_sem;

	enum input_stat stat;

	u8 echo_mode:1;
	u8 use_history:1;

#ifdef FINSH_USING_HISTORY
	u8 current_history;
	u16 history_count;

	char cmd_history[FINSH_HISTORY_LINES][FINSH_CMD_SIZE];
#endif

	struct finsh_parser parser;

	char line[FINSH_CMD_SIZE];
	u8 line_position;

	wm_device_t device;
};

void finsh_set_echo(u32 echo);
u32 finsh_get_echo(void);

void finsh_set_device(const char* device_name);
const char* finsh_get_device(void);

#endif
