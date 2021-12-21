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
        ldr x0, =stack0
        mov x1, #1024*4
        mrs x2, mpidr_el1
        and x2, x2, #0xff
        add x2, x2, #1
        mul x1, x1, x2
        add x0, x0, x1
        mov sp, x0
        // jump to start() in start.c
        b start
spin:
        b .
