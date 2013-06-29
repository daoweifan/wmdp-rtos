/*
 * File      : cmd.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-04-30     Bernard      first implementation
 */
#include <string.h>
#include "wm_list.h"
#include "finsh.h"
#include "kservice.h"

long hello(void)
{
	wm_kprintf("Hello WMDP!\n");

	return 0;
}
FINSH_FUNCTION_EXPORT(hello, say hello world);

// extern void rt_show_version(void);
long version(void)
{
	wm_kprintf("WMDP, VERSION1.0 2013-06-25\n");

	return 0;
}
FINSH_FUNCTION_EXPORT(version, show RT-Thread version information);

long list(void)
{
	struct finsh_syscall_item* syscall_item;
	struct finsh_sysvar_item*  sysvar_item;

	wm_kprintf("--Function List:\n");
	{
		struct finsh_syscall* index;
		for (index = _syscall_table_begin; index < _syscall_table_end; index ++)
		{
#ifdef CONFIG_FINSH_USING_DESCRIPTION
			wm_kprintf("%-16s -- %s\n", index->name, index->desc);
#else
			wm_kprintf("%s\n", index->name);
#endif
		}
	}

	/* list syscall list */
	syscall_item = global_syscall_list;
	while (syscall_item != NULL)
	{
		wm_kprintf("[l] %s\n", syscall_item->syscall.name);
		syscall_item = syscall_item->next;
	}

	wm_kprintf("--Variable List:\n");
	{
		struct finsh_sysvar* index;
		for (index = _sysvar_table_begin; index < _sysvar_table_end; index ++)
		{
#ifdef CONFIG_FINSH_USING_DESCRIPTION
			wm_kprintf("%-16s -- %s\n", index->name, index->desc);
#else
			wm_kprintf("%s\n", index->name);
#endif
		}
	}

	sysvar_item = global_sysvar_list;
	while (sysvar_item != NULL)
	{
		wm_kprintf("[l] %s\n", sysvar_item->sysvar.name);
		sysvar_item = sysvar_item->next;
	}

	return 0;
}
FINSH_FUNCTION_EXPORT(list, list all symbol in system)

static int str_is_prefix(const char* prefix, const char* str)
{
	while ((*prefix) && (*prefix == *str))
	{
		prefix ++;
		str ++;
	}

	if (*prefix == 0) return 0;
	return -1;
}

void list_prefix(char* prefix)
{
	struct finsh_syscall_item* syscall_item;
	struct finsh_sysvar_item*  sysvar_item;
	u16 func_cnt, var_cnt;
	const char* name_ptr;

	func_cnt = 0;
	var_cnt  = 0;
	name_ptr = RT_NULL;

	{
		struct finsh_syscall* index;
		for (index = _syscall_table_begin; index < _syscall_table_end; index ++)
		{
			if (str_is_prefix(prefix, index->name) == 0)
			{
				if (func_cnt == 0)
					wm_kprintf("--function:\n");

				func_cnt ++;
				/* set name_ptr */
				name_ptr = index->name;

#ifdef CONFIG_FINSH_USING_DESCRIPTION
				wm_kprintf("%-16s -- %s\n", index->name, index->desc);
#else
				wm_kprintf("%s\n", index->name);
#endif
			}
		}
	}

	/* list syscall list */
	syscall_item = global_syscall_list;
	while (syscall_item != NULL)
	{
		if (str_is_prefix(prefix, syscall_item->syscall.name) == 0)
		{
			if (func_cnt == 0)
				wm_kprintf("--function:\n");
			func_cnt ++;
			/* set name_ptr */
			name_ptr = syscall_item->syscall.name;

			wm_kprintf("[l] %s\n", syscall_item->syscall.name);
		}
		syscall_item = syscall_item->next;
	}

	{
		struct finsh_sysvar* index;
		for (index = _sysvar_table_begin; index < _sysvar_table_end; index ++)
		{
			if (str_is_prefix(prefix, index->name) == 0)
			{
				if (var_cnt == 0)
					wm_kprintf("--variable:\n");

				var_cnt ++;
				/* set name ptr */
				name_ptr = index->name;

#ifdef CONFIG_FINSH_USING_DESCRIPTION
				wm_kprintf("%-16s -- %s\n", index->name, index->desc);
#else
				wm_kprintf("%s\n", index->name);
#endif
			}
		}
	}

	sysvar_item = global_sysvar_list;
	while (sysvar_item != NULL)
	{
		if (str_is_prefix(prefix, sysvar_item->sysvar.name) == 0)
		{
			if (var_cnt == 0)
				wm_kprintf("--variable:\n");

			var_cnt ++;
			/* set name ptr */
			name_ptr = sysvar_item->sysvar.name;

			wm_kprintf("[l] %s\n", sysvar_item->sysvar.name);
		}
		sysvar_item = sysvar_item->next;
	}

	/* only one matched */
	if ((func_cnt + var_cnt) == 1)
	{
		strncpy(prefix, name_ptr, strlen(name_ptr));
	}
}

#ifdef CONFIG_FINSH_USING_SYMTAB
static int dummy = 0;
FINSH_VAR_EXPORT(dummy, finsh_type_int, dummy variable for finsh)
#endif
