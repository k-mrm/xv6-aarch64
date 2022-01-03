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
