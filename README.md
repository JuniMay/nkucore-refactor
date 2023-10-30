# NKUCORE

```text
##\   ##\ ##\   ##\ ##\   ##\  ######\   ######\  #######\  ########\
###\  ## |## | ##  |## |  ## |##  __##\ ##  __##\ ##  __##\ ##  _____|
####\ ## |## |##  / ## |  ## |## /  \__|## /  ## |## |  ## |## |
## ##\## |#####  /  ## |  ## |## |      ## |  ## |#######  |#####\
## \#### |##  ##<   ## |  ## |## |      ## |  ## |##  __##< ##  __|
## |\### |## |\##\  ## |  ## |## |  ##\ ## |  ## |## |  ## |## |
## | \## |## | \##\ \######  |\######  | ######  |## |  ## |########\
\__|  \__|\__|  \__| \______/  \______/  \______/ \__|  \__|\________|
```

A refactored version of ucore used by NKU OS course.

## Quick Start

Same as the original ucore. Please refer to the instruction manual of ucore.

Note that some bugs in the original ucore are fixed in this project, and the lates version of QEMU is recommended.

## Roadmap

- [x] Lab0.5 & Lab1: Boot and setup traps.
- [ ] Lab2: Physical memory management.
  - [x] Setup Sv39 paging in kernel.
  - [ ] Physical memory management.
- [ ] Lab3: Virtual memory management -- page replacement.
- [ ] Lab4: Process management.
- [ ] Lab5: User environment and system call.
- [ ] Lab6: Process scheduling.
- [ ] Lab7: Sync and mutual exclusion.
- [ ] Lab8: File system.

## About

This is a refactored version of ucore used by NKU OS course. The original ucore is a great project, but it's not easy to read and modify due to the old x86-based architecture. This project only support RISC-V 64 for now and will be more readable and maintainable (hopefully).

There are some differences between the implementation of this project and the original ucore, so this may not pass the test of the original `grade.sh`.

The structure and basic concepts remain the same but some implementations are simplified.
