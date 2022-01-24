// Context switch
//
//   void swtch(struct context *old, struct context *new);
//
// Save current registers in old. Load from new.


.global swtch
swtch:
        mov x9, x0
        mov x10, sp

        stp x10/*sp*/, x18, [x9, #16 * 0]
        stp x19, x20, [x9, #16 * 1]
        stp x21, x22, [x9, #16 * 2]
        stp x23, x24, [x9, #16 * 3]
        stp x25, x26, [x9, #16 * 4]
        stp x27, x28, [x9, #16 * 5]
        stp x29, x30, [x9, #16 * 6]

        mov x9, x1

        ldp x10/*sp*/, x18, [x9, #16 * 0]
        ldp x19, x20, [x9, #16 * 1]
        ldp x21, x22, [x9, #16 * 2]
        ldp x23, x24, [x9, #16 * 3]
        ldp x25, x26, [x9, #16 * 4]
        ldp x27, x28, [x9, #16 * 5]
        ldp x29, x30, [x9, #16 * 6]

        mov sp, x10
        
        ret
