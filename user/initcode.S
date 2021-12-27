// Initial process that execs /init.
// This code runs in user space.

#include "syscall.h"

// exec(init, argv)
.global start
start:
        ldr x0, =init
        ldr x1, =argv
        mov x7, #SYS_exec
        svc #0

// for(;;) exit();
exit:
        mov x7, #SYS_exit
        svc #0
        b exit

// char init[] = "/init\0";
init:
  .ascii "/init\0"

// char *argv[] = { init, 0 };
.p2align 3
argv:
  .quad init
  .quad 0
