        //
        // interrupts and exceptions while in supervisor
        // mode (EL1) come here.
        //
        // push all registers, call kerneltrap(), restore, return.
        //
.globl kerneltrap
.globl kernelvec
.align 4
kernelvec:
        // make room to save registers.
        sub sp, sp, #272

        // save the registers.
        stp x0, x1, [sp, #16 * 0]
        stp x2, x3, [sp, #16 * 1]
        stp x4, x5, [sp, #16 * 2]
        stp x6, x7, [sp, #16 * 3]
        stp x8, x9, [sp, #16 * 4]
        stp x10, x11, [sp, #16 * 5]
        stp x12, x13, [sp, #16 * 6]
        stp x14, x15, [sp, #16 * 7]
        stp x16, x17, [sp, #16 * 8]
        stp x18, x19, [sp, #16 * 9]
        stp x20, x21, [sp, #16 * 10]
        stp x22, x23, [sp, #16 * 11]
        stp x24, x25, [sp, #16 * 12]
        stp x26, x27, [sp, #16 * 13]
        stp x28, x29, [sp, #16 * 14]
        mrs x9, elr_el1
        mrs x10, spsr_el1
        add x11, sp, #272
        stp x30, x9, [sp, #16 * 15]
        stp x10, x11, [sp, #16 * 16]

        // call the C trap handler in trap.c
        bl kerneltrap

        // restore registers.
        ld ra, 0(sp)
        ld sp, 8(sp)
        ld gp, 16(sp)
        ld t0, 32(sp)
        ld t1, 40(sp)
        ld t2, 48(sp)
        ld s0, 56(sp)
        ld s1, 64(sp)
        ld a0, 72(sp)
        ld a1, 80(sp)
        ld a2, 88(sp)
        ld a3, 96(sp)
        ld a4, 104(sp)
        ld a5, 112(sp)
        ld a6, 120(sp)
        ld a7, 128(sp)
        ld s2, 136(sp)
        ld s3, 144(sp)
        ld s4, 152(sp)
        ld s5, 160(sp)
        ld s6, 168(sp)
        ld s7, 176(sp)
        ld s8, 184(sp)
        ld s9, 192(sp)
        ld s10, 200(sp)
        ld s11, 208(sp)
        ld t3, 216(sp)
        ld t4, 224(sp)
        ld t5, 232(sp)
        ld t6, 240(sp)

        addi sp, sp, 256

        // return to whatever we were doing in the kernel.
        eret

        //
        // machine-mode timer interrupt.
        //
.globl timervec
.align 4
timervec:
        // start.c has set up the memory that mscratch points to:
        // scratch[0,8,16] : register save area.
        // scratch[24] : address of CLINT's MTIMECMP register.
        // scratch[32] : desired interval between interrupts.
        
        csrrw a0, mscratch, a0
        sd a1, 0(a0)
        sd a2, 8(a0)
        sd a3, 16(a0)

        // schedule the next timer interrupt
        // by adding interval to mtimecmp.
        ld a1, 24(a0) # CLINT_MTIMECMP(hart)
        ld a2, 32(a0) # interval
        ld a3, 0(a1)
        add a3, a3, a2
        sd a3, 0(a1)

        // raise a supervisor software interrupt.
        li a1, 2
        csrw sip, a1

        ld a3, 16(a0)
        ld a2, 8(a0)
        ld a1, 0(a0)
        csrrw a0, mscratch, a0

        mret
