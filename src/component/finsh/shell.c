/*
 * File      : shell.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-04-30     Bernard      the first verion for FinSH
 */

#include <string.h>
#include "finsh.h"
#include "shell.h"

/* finsh thread */
static struct rt_thread finsh_thread;
ALIGN(RT_ALIGN_SIZE)
static char finsh_thread_stack[FINSH_THREAD_STACK_SIZE];
struct finsh_shell* shell;

static int finsh_rx_ind(wm_device_t dev, int size)
{
	RT_ASSERT(shell != WM_NULL);

	// /* release semaphore to let finsh thread rx data */
	// rt_sem_release(&shell->rx_sem);

	return RT_EOK;
}

/**
 * @ingroup finsh
 *
 * This function sets the input device of finsh shell.
 *
 * @param device_name the name of new input device.
 */
void finsh_set_device(const char* device_name)
{
	wm_device_t dev = WM_NULL;

	RT_ASSERT(shell != WM_NULL);
	dev = wm_device_find(device_name);
	if (dev != WM_NULL && wm_device_open(dev, WM_DEVICE_OFLAG_RDWR) == RT_EOK)
	{
		if (shell->device != WM_NULL)
		{
			/* close old finsh device */
			wm_device_close(shell->device);
		}

		shell->device = dev;
		rt_device_set_rx_indicate(dev, finsh_rx_ind);
	}
	else
	{
		rt_kprintf("finsh: can not find device:%s\n", device_name);
	}
}

/**
 * @ingroup finsh
 *
 * This function returns current finsh shell input device.
 *
 * @return the finsh shell input device name is returned.
 */
const char* finsh_get_device()
{
	RT_ASSERT(shell != WM_NULL);
	return shell->device->name;
}

/**
 * @ingroup finsh
 *
 * This function set the echo mode of finsh shell.
 *
 * FINSH_OPTION_ECHO=0x01 is echo mode, other values are none-echo mode.
 *
 * @param echo the echo mode
 */
void finsh_set_echo(u32 echo)
{
	RT_ASSERT(shell != WM_NULL);
	shell->echo_mode = echo;
}

/**
 * @ingroup finsh
 *
 * This function gets the echo mode of finsh shell.
 *
 * @return the echo mode
 */
u32 finsh_get_echo()
{
	RT_ASSERT(shell != WM_NULL);

	return shell->echo_mode;
}

void finsh_auto_complete(char* prefix)
{
	extern void list_prefix(char* prefix);

	rt_kprintf("\n");
	list_prefix(prefix);
	rt_kprintf("%s%s", FINSH_PROMPT, prefix);
}

void finsh_run_line(struct finsh_parser* parser, const char *line)
{
	const char* err_str;

	rt_kprintf("\n");
	finsh_parser_run(parser, (unsigned char*)line);

	/* compile node root */
	if (finsh_errno() == 0)
	{
		finsh_compiler_run(parser->root);
	}
	else
	{
		err_str = finsh_error_string(finsh_errno());
		rt_kprintf("%s\n", err_str);
	}

	/* run virtual machine */
	if (finsh_errno() == 0)
	{
		char ch;
		finsh_vm_run();

		ch = (unsigned char)finsh_stack_bottom();
		if (ch > 0x20 && ch < 0x7e)
		{
			rt_kprintf("\t'%c', %d, 0x%08x\n",
				(unsigned char)finsh_stack_bottom(),
				(unsigned int)finsh_stack_bottom(),
				(unsigned int)finsh_stack_bottom());
		}
		else
		{
			rt_kprintf("\t%d, 0x%08x\n",
				(unsigned int)finsh_stack_bottom(),
				(unsigned int)finsh_stack_bottom());
		}
	}

    finsh_flush(parser);
}

#ifdef FINSH_USING_HISTORY
rt_bool_t finsh_handle_history(struct finsh_shell* shell, char ch)
{
	/*
	 * handle up and down key
	 * up key  : 0x1b 0x5b 0x41
	 * down key: 0x1b 0x5b 0x42
	 */
	if (ch == 0x1b)
	{
		shell->stat = WAIT_SPEC_KEY;
		return True;
	}

	if ((shell->stat == WAIT_SPEC_KEY))
	{
		if (ch == 0x5b)
		{
			shell->stat = WAIT_FUNC_KEY;
			return True;
		}

		shell->stat = WAIT_NORMAL;
		return False;
	}

	if (shell->stat == WAIT_FUNC_KEY)
	{
		shell->stat = WAIT_NORMAL;

		if (ch == 0x41) /* up key */
		{
			/* prev history */
			if (shell->current_history > 0)shell->current_history --;
			else
			{
				shell->current_history = 0;
				return True;
			}

			/* copy the history command */
			memcpy(shell->line, &shell->cmd_history[shell->current_history][0],
				FINSH_CMD_SIZE);
			shell->line_position = strlen(shell->line);
			shell->use_history = 1;
		}
		else if (ch == 0x42) /* down key */
		{
			/* next history */
			if (shell->current_history < shell->history_count - 1)
				shell->current_history ++;
			else
			{
				/* set to the end of history */
				if (shell->history_count != 0)
				{
					shell->current_history = shell->history_count - 1;
				}
				else return True;
			}

			memcpy(shell->line, &shell->cmd_history[shell->current_history][0],
				FINSH_CMD_SIZE);
			shell->line_position = strlen(shell->line);
			shell->use_history = 1;
		}

		if (shell->use_history)
		{
			rt_kprintf("\033[2K\r");
			rt_kprintf("%s%s", FINSH_PROMPT, shell->line);
			return True;;
		}
	}

	return False;
}

void finsh_push_history(struct finsh_shell* shell)
{
	if ((shell->use_history == 0) && (shell->line_position != 0))
	{
		/* push history */
		if (shell->history_count >= FINSH_HISTORY_LINES)
		{
			/* move history */
			int index;
			for (index = 0; index < FINSH_HISTORY_LINES - 1; index ++)
			{
				memcpy(&shell->cmd_history[index][0],
					&shell->cmd_history[index + 1][0], FINSH_CMD_SIZE);
			}
			memset(&shell->cmd_history[index][0], 0, FINSH_CMD_SIZE);
			memcpy(&shell->cmd_history[index][0], shell->line, shell->line_position);

			/* it's the maximum history */
			shell->history_count = FINSH_HISTORY_LINES;
		}
		else
		{
			memset(&shell->cmd_history[shell->history_count][0], 0, FINSH_CMD_SIZE);
			memcpy(&shell->cmd_history[shell->history_count][0], shell->line, shell->line_position);

			/* increase count and set current history position */
			shell->history_count ++;
		}
	}
	shell->current_history = shell->history_count;
}
#endif


struct finsh_shell _shell;

void finsh_thread_entry(void* parameter)
{
    char ch;

	/* normal is echo mode */
	shell->echo_mode = 1;

    finsh_init(&shell->parser);
	rt_kprintf(FINSH_PROMPT);

	while (1)
	{
		/* wait receive */
		if (rt_sem_take(&shell->rx_sem, RT_WAITING_FOREVER) != RT_EOK) continue;

		/* read one character from device */
		while (rt_device_read(shell->device, 0, &ch, 1) == 1)
		{
			/* handle history key */
			#ifdef FINSH_USING_HISTORY
			if (finsh_handle_history(shell, ch) == True) continue;
			#endif

			/* handle CR key */
			if (ch == '\r')
			{
				char next;

				if (rt_device_read(shell->device, 0, &next, 1) == 1)
					ch = next;
				else ch = '\r';
			}
			/* handle tab key */
			else if (ch == '\t')
			{
				/* auto complete */
				finsh_auto_complete(&shell->line[0]);
				/* re-calculate position */
				shell->line_position = strlen(shell->line);
				continue;
			}
			/* handle backspace key */
			else if (ch == 0x7f || ch == 0x08)
			{
				if (shell->line_position != 0)
				{
					rt_kprintf("%c %c", ch, ch);
				}
				if (shell->line_position <= 0) shell->line_position = 0;
				else shell->line_position --;
				shell->line[shell->line_position] = 0;
				continue;
			}

			/* handle end of line, break */
			if (ch == '\r' || ch == '\n')
			{
				/* change to ';' and break */
				shell->line[shell->line_position] = ';';

				#ifdef FINSH_USING_HISTORY
				finsh_push_history(shell);
				#endif

				if (shell->line_position != 0) finsh_run_line(&shell->parser, shell->line);
				else rt_kprintf("\n");

				rt_kprintf(FINSH_PROMPT);
				memset(shell->line, 0, sizeof(shell->line));
				shell->line_position = 0;

				break;
			}

			/* it's a large line, discard it */
			if (shell->line_position >= FINSH_CMD_SIZE) shell->line_position = 0;

			/* normal character */
			shell->line[shell->line_position] = ch; ch = 0;
			if (shell->echo_mode) rt_kprintf("%c", shell->line[shell->line_position]);
			shell->line_position ++;
			shell->use_history = 0; /* it's a new command */
		} /* end of device read */
	}
}

void finsh_system_function_init(const void* begin, const void* end)
{
	_syscall_table_begin = (struct finsh_syscall*) begin;
	_syscall_table_end = (struct finsh_syscall*) end;
}

void finsh_system_var_init(const void* begin, const void* end)
{
	_sysvar_table_begin = (struct finsh_sysvar*) begin;
	_sysvar_table_end = (struct finsh_sysvar*) end;
}

#if defined(__ICCARM__)               /* for IAR compiler */
  #ifdef FINSH_USING_SYMTAB
    #pragma section="FSymTab"
    #pragma section="VSymTab"
  #endif
#endif

/*
 * @ingroup finsh
 *
 * This function will initialize finsh shell
 */
void finsh_system_init(void)
{
	rt_err_t result;

#ifdef FINSH_USING_SYMTAB
	finsh_system_function_init(__section_begin("FSymTab"), __section_end("FSymTab"));
	finsh_system_var_init(__section_begin("VSymTab"), __section_end("VSymTab"));
#endif

	/* create or set shell structure */
	shell = &_shell;

	if (shell == WM_NULL)
	{
		rt_kprintf("no memory for shell\n");
		return;
	}
	
	memset(shell, 0, sizeof(struct finsh_shell));

	rt_sem_init(&(shell->rx_sem), "shrx", 0, 0);
	result = rt_thread_init(&finsh_thread,
		"tshell",
		finsh_thread_entry, WM_NULL,
		&finsh_thread_stack[0], sizeof(finsh_thread_stack),
		FINSH_THREAD_PRIORITY, 10);

	if (result == RT_EOK)
		rt_thread_startup(&finsh_thread);
}
