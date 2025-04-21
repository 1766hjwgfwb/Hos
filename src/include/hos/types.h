#ifndef HOS_TYPES_H
#define HOS_TYPES_H

#define EOF (-1) /* Error indicator */

#define EOS '\0' /* End of string */

#define bool _Bool /* Boolean type */
#define NULL ((void*)0) /* Null pointer */
#define true 1 /* Boolean true */
#define false 0 /* Boolean false */


/* Type modifiers */
#define _packed __attribute__((packed))

typedef unsigned int size_t;


/* Basic types */
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;


/* Unsigned types */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;









#endif /* HOS_TYPES_H */