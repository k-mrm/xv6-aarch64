        // qemu -kernel loads the kernel at 0x40000000
        // and causes each CPU to jump there.
        // kernel.ld causes the following code to
        // be placed at 0x40000000.
.section .text
.global _entry
_entry:
        // set up a stack for C.
        // stack0 is declared in start.c,
        // with a 4096-byte stack per CPU.
        // sp = stack0 + (cpuid * 4096)
        // cpuid = mpidr_el1 & 0xff
        ldr sp, stack0
        mov x0, #1024*4
        msr x1, mpidr_el1
        and x1, x1, #0xff
        add x1, x1, #1
        mul x0, x0, x1
        add sp, sp, x0
        // jump to start() in start.c
        b start
spin:
        b .
