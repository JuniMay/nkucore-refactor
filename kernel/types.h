#ifndef KERNEL_TYPES_H_
#define KERNEL_TYPES_H_

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long int64_t;

typedef uint64_t size_t;

typedef int bool;

#define true 1
#define false 0

#define NULL ((void*)0)

#endif  // KERNEL_TYPES_H_