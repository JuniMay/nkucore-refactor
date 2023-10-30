#ifndef KERNEL_LIBS_PANIC_H_
#define KERNEL_LIBS_PANIC_H_

void panic_impl(const char* file, int line, const char* fmt, ...);

#define panic(fmt, ...) panic_impl(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define assert(cond) \
  do {               \
    if (!(cond)) {   \
      panic(#cond);  \
    }                \
  } while (0)

#endif  // KERNEL_LIBS_PANIC_H_