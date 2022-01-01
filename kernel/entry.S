#include "aarch64.h" 
#include "memlayout.h"
        // qemu -kernel loads the kernel at 0x40000000
        // and causes each CPU to jump there.
        // kernel.ld causes the following code to
        // be placed at 0x40000000.
.section ".text"
.global _entry
_entry:
        mrs x1, mpidr_el1
        and x1, x1, #3
        cbz x1, entry         // primary
        b entryothers         // secondary

entry:
        // set up pagetable
        // call entrypgtinit() in start.c
        bl entrypgtinit

entryothers:  // secondary CPU starts here
        // load pagetable
        adrp x0, l1entrypgt
        adrp x1, l1kpgt
        msr ttbr0_el1, x0
        msr ttbr1_el1, x1

        mov x3, x0
        mov x4, x1

        // setup tcr
        ldr x0, =(TCR_T0SZ(25)|TCR_T1SZ(25)|TCR_TG0(0)|TCR_TG1(2)|TCR_IPS(0))
        msr tcr_el1, x0

        // setup mair
        ldr x1, =((MT_DEVICE_nGnRnE<<(8*AI_DEVICE_nGnRnE_IDX)) | (MT_NORMAL_NC<<(8*AI_NORMAL_NC_IDX)))
        msr mair_el1, x1

        isb

        ldr x1, =_start   // x1 = VA(_start)

        // enable paging
        mrs x0, sctlr_el1
        orr x0, x0, #1
        msr sctlr_el1, x0

        br x1       // jump to higher address (0xffffff8000000000~)

_start:
        b .
        // set up a stack for C.
        // stack0 is declared in start.c,
        // with a 4096-byte stack per CPU.
        // sp = stack0 + (cpuid * 4096)
        // cpuid = mpidr_el1 & 0xff
        ldr x0, =stack0
        mov x1, #1024*4
        mrs x2, mpidr_el1
        and x2, x2, #0xff
        add x2, x2, #1
        mul x1, x1, x2
        add x0, x0, x1
        mov sp, x0
        // jump to start() in start.c
        b main

        b .   // spin
