        //
        // interrupts and exceptions while in EL1(kernel mode) and EL0(user mode)
        // come here.
        //
        // push all registers, call kerneltrap() or usertrap(), restore, return.
        //
.section ".text"

.global alltraps
.balign 0x800
alltraps:
// Current EL with sp0
  b .
.balign 0x80
  b .
.balign 0x80
  b .
.balign 0x80
  b .

// Current EL with spx
.balign 0x80
  b kernelvec
.balign 0x80
  b kernelvec
.balign 0x80
  b .
.balign 0x80
  b .

// Lower EL using aarch64
.balign 0x80
  b uservec
.balign 0x80
  b uservec
.balign 0x80
  b .
.balign 0x80
  b .

// Lower EL using aarch32
.balign 0x80
  b .
.balign 0x80
  b .
.balign 0x80
  b .
.balign 0x80
  b .
