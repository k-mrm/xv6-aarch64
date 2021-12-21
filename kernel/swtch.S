// Context switch
//
//   void swtch(struct context *old, struct context *new);
//
// Save current registers in old. Load from new.


.global swtch
swtch:
        mov x9, x0
        mov x10, sp

        stp x10/*sp*/, x0, [x9, #16 * 0]
        stp x1, x2, [x9, #16 * 1]
        stp x3, x4, [x9, #16 * 2]
        stp x5, x6, [x9, #16 * 3]
        stp x7, x8, [x9, #16 * 4]
        stp x16, x17, [x9, #16 * 5]
        stp x18, x19, [x9, #16 * 6]
        stp x20, x21, [x9, #16 * 7]
        stp x22, x23, [x9, #16 * 8]
        stp x24, x25, [x9, #16 * 9]
        stp x26, x27, [x9, #16 * 10]
        stp x28, x29, [x9, #16 * 11]
        str x30, [x9, #16 * 12]

        mov x9, x1

        ldp x10/*sp*/, x0, [x9, #16 * 0]
        ldp x1, x2, [x9, #16 * 1]
        ldp x3, x4, [x9, #16 * 2]
        ldp x5, x6, [x9, #16 * 3]
        ldp x7, x8, [x9, #16 * 4]
        ldp x16, x17, [x9, #16 * 5]
        ldp x18, x19, [x9, #16 * 6]
        ldp x20, x21, [x9, #16 * 7]
        ldp x22, x23, [x9, #16 * 8]
        ldp x24, x25, [x9, #16 * 9]
        ldp x26, x27, [x9, #16 * 10]
        ldp x28, x29, [x9, #16 * 11]
        ldr x30, [x9, #16 * 12]

        mov sp, x10
        
        ret
/*
        sd ra, 0(a0)
        sd sp, 8(a0)
        sd s0, 16(a0)
        sd s1, 24(a0)
        sd s2, 32(a0)
        sd s3, 40(a0)
        sd s4, 48(a0)
        sd s5, 56(a0)
        sd s6, 64(a0)
        sd s7, 72(a0)
        sd s8, 80(a0)
        sd s9, 88(a0)
        sd s10, 96(a0)
        sd s11, 104(a0)

        ld ra, 0(a1)
        ld sp, 8(a1)
        ld s0, 16(a1)
        ld s1, 24(a1)
        ld s2, 32(a1)
        ld s3, 40(a1)
        ld s4, 48(a1)
        ld s5, 56(a1)
        ld s6, 64(a1)
        ld s7, 72(a1)
        ld s8, 80(a1)
        ld s9, 88(a1)
        ld s10, 96(a1)
        ld s11, 104(a1)
        
        ret
*/
