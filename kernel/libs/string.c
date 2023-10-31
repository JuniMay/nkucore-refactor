#include "libs/string.h"


void memset(void* dst, uint8_t val, size_t size) {
    uint8_t* p = (uint8_t*)dst;
    for (size_t i = 0; i < size; i++) {
        p[i] = val;
    }
}

void memcpy(void* dst, const void* src, size_t size) {
    uint8_t* p = (uint8_t*)dst;
    const uint8_t* q = (const uint8_t*)src;
    for (size_t i = 0; i < size; i++) {
        p[i] = q[i];
    }
}

void strcpy(char* dst, const char* src) {
    while (*src != '\0') {
        *dst++ = *src++;
    }
    *dst = '\0';
}