/*
 * File      : error.C
 * This file is part of WMDP
 * COPYRIGHT (C) 2013 - 2100, WMDP Development Team
 * Change Logs:
 * Date           Author       Notes
 * 2013-04-21     Daowei Fan   the first version
 */

/* global errno in RT-Thread */
static volatile int errno;

/*
 * This function will get errno
 *
 * @return errno
 */
int wm_get_errno(void)
{
	return errno;
}

/*
 * This function will set errno
 *
 * @param error the errno shall be set
 */
void wm_set_errno(int error)
{

	errno = error;

}
