#ifndef __DEF_H__
#define __DEF_H__

#ifdef __cplusplus
extern "C" {
#endif


/* ********************************************************************* */
/* Module configuration */


/* ********************************************************************* */
/* Interface macro & data definition */

typedef enum _bool{false,true} bool;
typedef unsigned char  u8;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  s8;                    /* Signed    8 bit quantity                           */
typedef unsigned int   u16;                   /* Unsigned 16 bit quantity                           */
typedef signed   int   s16;                   /* Signed   16 bit quantity                           */
typedef unsigned long  u32;                   /* Unsigned 32 bit quantity                           */
typedef signed   long  s32;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 16-bit wide                    */
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */

#define BYTE           s8                     /* Define data types for backward compatibility ...   */
#define UBYTE          u8                     /* ... to uC/OS V1.xx.  Not actually needed for ...   */
#define WORD           s16                    /* ... uC/OS-II.                                      */
#define UWORD          u16
#define LONG           s32
#define ULONG          u32

/* maximum value of base type */
#define U8_MAX                    0xff            /* Maxium number of u8   */
#define U16_MAX                   0xffff          /* Maxium number of u16  */
#define U32_MAX                   0xffffffff      /* Maxium number of u32  */

#define False 0
#define True !False

#define WM_NULL                   ((void *)0)

/* Sets the result on bPort */
#define BIT_SET(bPort,bBitMask)        (bPort |= bBitMask)
#define BIT_CLR(bPort,bBitMask)        (bPort &= ~bBitMask)

/* Returns the result */
#define GET_BIT_SET(bPort,bBitMask)    (bPort | bBitMask)
#define GET_BIT_CLR(bPort,bBitMask)    (bPort & ~bBitMask)

/* Returns 0 if the condition is False & a non-zero value if it is True */
#define TEST_BIT_SET(bPort,bBitMask)   (bPort & bBitMask)
#define TEST_BIT_CLR(bPort,bBitMask)   ((~bPort) & bBitMask)

/**
 * @addtogroup Error
 */
/*@{*/
/* RT-Thread error code definitions */
#define WM_EOK                          0               /**< There is no error       */
#define WM_ERROR                        1               /**< A generic error happens */
#define WM_ETIMEOUT                     2               /**< Timed out               */
#define WM_EFULL                        3               /**< The resource is full    */
#define WM_EEMPTY                       4               /**< The resource is empty   */
#define WM_ENOMEM                       5               /**< No memory               */
#define WM_ENOSYS                       6               /**< No system               */
#define WM_EBUSY                        7               /**< Busy                    */
#define WM_EIO							8 				/**< IO error 				 */
/*@}*/

#ifdef __cplusplus
}
#endif

#endif /*__DEF_H__*/

