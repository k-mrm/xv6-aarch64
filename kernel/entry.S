#include "aarch64.h" 
#include "memlayout.h"
#include "param.h"

        // qemu -kernel loads the kernel at 0x40000000
        // and causes each CPU to jump there.
        // kernel.ld causes the following code to
        // be placed at 0x40000000.
.section ".text"
.global _entry
_entry:
        mrs x1, mpidr_el1
        and x1, x1, #0x3
        cbz x1, entry         // primary
        b entryothers         // secondary

entry:
        // clear .bss
        adrp x1, bss_start
        ldr w2, =bss_size
1:
        cbz w2, 2f
        str xzr, [x1], #8
        sub w2, w2, #1
        b 1b
2:

        // set up entry pagetable
        //
        // Phase 1.
        // map the kernel code identically.
        // map [0x40000000,PA(end)) to [0x40000000,PA(end))
        // memory type is normal
        //
        // Phase 2.
        // map the kernel code.
        // map [0xffffff8040000000,VA(end)) to [0x40000000,PA(end))
        // memory type is normal.

        // Phase 1
        // map [0x40000000,PA(end)) to [0x40000000,PA(end))
        adrp x0, l2entrypgt

        mov x1, #0x40000000
        ldr x2, =V2P_WO(end)-1

        lsr x3, x1, #PXSHIFT(2)
        and x3, x3, #PXMASK   // PX(2, x1)
        lsr x4, x2, #PXSHIFT(2)
        and x4, x4, #PXMASK   // PX(2, x2)
        mov x5, #(PTE_AF | PTE_INDX(AI_NORMAL_NC_IDX) | PTE_VALID) // entry attr
        orr x6, x1, x5      // block entry
l2epgt_loop:
        str x6, [x0, x3, lsl #3]  // l2entrypgt[l2idx] = block entry
        add x3, x3, #1          // next index
        add x6, x6, #0x200000   // next block, block size is 2MB
        cmp x3, x4
        b.ls l2epgt_loop    // if start va idx <= end va idx

        adrp x0, l1entrypgt

        lsr x3, x1, #PXSHIFT(1)
        and x3, x3, #PXMASK     // start va level1 index

        mov x4, #(PTE_TABLE | PTE_VALID)  // entry attr
        adrp x5, l2entrypgt
        orr x6, x4, x5    // table entry

        str x6, [x0, x3, lsl #3]    // l1entrypgt[l1idx] = table entry

        // Phase 2
        // map [0xffffff8040000000,VA(end)) to [0x40000000,PA(end))
        adrp x0, l2kpgt

        mov x1, #0x40000000   // start pa
        ldr x2, =V2P_WO(end)-1   // end pa
        mov x3, #KERNBASE
        add x4, x1, x3    // start va
        add x5, x2, x3    // end va

        lsr x6, x4, #PXSHIFT(2)
        and x6, x6, #PXMASK   // x6 = PX(2,x4)
        lsr x7, x5, #PXSHIFT(2)
        and x7, x7, #PXMASK   // x7 = PX(2,x5)
        mov x8, #(PTE_AF | PTE_INDX(AI_NORMAL_NC_IDX) | PTE_VALID) // entry attr
        orr x9, x1, x8      // block entry
l2kpgt_loop:
        str x9, [x0, x6, lsl #3]  // l2entrypgt[l2idx] = block entry
        add x6, x6, #1          // next index
        add x9, x9, #0x200000   // next block, block size is 2MB
        cmp x6, x7
        b.ls l2kpgt_loop    // if start va idx <= end va idx

        adrp x0, l1kpgt

        lsr x5, x4, #PXSHIFT(1)
        and x5, x5, #PXMASK  // x5 = PX(1,x4)

        mov x6, #(PTE_TABLE | PTE_VALID)  // entry attr
        adrp x7, l2kpgt
        orr x8, x6, x7    // table entry

        str x8, [x0, x5, lsl #3]    // l1kpgt[l1idx] = table entry

entryothers:  // secondary CPU starts here
        // load pagetable
        adrp x0, l1entrypgt
        adrp x1, l1kpgt
        msr ttbr0_el1, x0
        msr ttbr1_el1, x1

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
        // set up a stack for C.
        // stack0 is declared in start.c,
        // with a 4096-byte stack per CPU.
        // sp = stack0 + ((cpuid+1) * 4096)
        // cpuid = mpidr_el1 & 0xff
        ldr x0, =stack0
        mov x1, #1024*4
        mrs x2, mpidr_el1
        and x2, x2, #0x3
        add x2, x2, #1
        mul x1, x1, x2
        add x0, x0, x1
        mov sp, x0
        // jump to main()
        b main

        b .   // spin

.global psci_call
psci_call:
        hvc #0
        ret
