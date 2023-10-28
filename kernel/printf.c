#include <stdarg.h>
#include "sbi.h"

static char digits[] = "0123456789abcdef";

static void print_integer(int x, int base, int sgn) {
    char buf[16];
    int i;
    uint32_t bits;

    if (sgn && (sgn = (x < 0))) {
        bits = -x;
    } else {
        bits = x;
    }

    i = 0;

    do {
        buf[i++] = digits[bits % base];
    } while ((bits /= base) != 0);

    if (sgn) {
        buf[i++] = '-';
    }

    while (--i >= 0) {
        sbi_console_putchar(buf[i]);
    }
}

static void print_ptr(uint64_t x) {
    int i;
    sbi_console_putchar('0');
    sbi_console_putchar('x');
    for (i = 60; i >= 0; i -= 4) {
        sbi_console_putchar(digits[(x >> i) & 0xf]);
    }
}

void printf(const char *fmt, ...) {
    va_list ap;
    int c;
    char *s;

    va_start(ap, fmt);

    while ((c = *fmt++) != '\0') {
        if (c != '%') {
            sbi_console_putchar(c);
            continue;
        }

        switch ((c = *fmt++)) {
            case 'c':
                sbi_console_putchar(va_arg(ap, int));
                break;
            case 'd':
                print_integer(va_arg(ap, int), 10, 1);
                break;
            case 'x':
                print_integer(va_arg(ap, int), 16, 0);
                break;
            case 'p':
                print_ptr(va_arg(ap, uint64_t));
                break;
            case 's':
                s = va_arg(ap, char *);
                while (*s != '\0') {
                    sbi_console_putchar(*s++);
                }
                break;
            case '%':
                sbi_console_putchar('%');
                break;
            default:
                sbi_console_putchar('%');
                sbi_console_putchar(c);
                break;
        }
    }

    va_end(ap);
}