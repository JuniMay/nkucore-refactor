#ifndef KERNEL_LIBS_STRING_H_
#define KERNEL_LIBS_STRING_H_

#include "libs/types.h"

void memset(void* dst, uint8_t val, size_t size);
void memcpy(void* dst, const void* src, size_t size);

void strcpy(char* dst, const char* src);

#endif  // KERNEL_LIBS_STRING_H_