#ifndef _BASE_TYPES_H
#define _BASE_TYPES_H

#include <string.h>
#include <stdlib.h>

/*******************************************************************************
* 宏定义 *
*******************************************************************************/
#define TRUE    1
#define FALSE   0


#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
/*******************************************************************************
* 结束宏定义 *
*******************************************************************************/

/*******************************************************************************
* 全局类型 *
*******************************************************************************/ 
typedef signed char              Sint8;   // c
typedef signed short int         Sint16;   // w
typedef signed int               Sint32;  // i
typedef signed long long int     Sint64;  // ll

typedef unsigned char            Uint8;   // uc
typedef unsigned short int       Uint16;   // uw
typedef unsigned int             Uint32;  // ui
typedef unsigned long long int   Uint64;  // ull

typedef float                    Float;
typedef unsigned char            Bool;


/*******************************************************************************
* 结束全局类型 *
*******************************************************************************/   

#endif
