/* config.h
 * 	daowei@2013 initial version
 */
 
#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "autoconfig.h"

#ifdef CONFIG_STM32F10X_LD
#define STM32F10X_LD
#endif

#ifdef CONFIG_STM32F10X_MD
#define STM32F10X_MD
#endif

#ifdef CONFIG_STM32F10X_HD
#define STM32F10X_HD
#endif

#ifdef CONFIG_STM32F10X_CL
#define STM32F10X_CL
#endif

#ifdef CONFIG_USE_STDPERIPH_DRIVER
#define USE_STDPERIPH_DRIVER
#endif

#ifdef CONFIG_HSE_VALUE
#define HSE_Value CONFIG_HSE_VALUE
#endif

/* Compiler Related Definitions */
#if defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
	#include <stdarg.h>
	#define SECTION(x)                  @ x
	#define UNUSED
	#define PRAGMA(x)                   _Pragma(#x)
	#define ALIGN(n)                    PRAGMA(data_alignment=n)
	#define rt_inline                   static inline
	#define RTT_API
#endif

#endif /*__CONFIG_H_*/
