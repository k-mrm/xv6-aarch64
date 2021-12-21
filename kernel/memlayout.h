// Physical memory layout

// qemu -machine virt is set up like this,
// based on qemu's hw/arm/virt.c:
//
// 00000000 -- boot ROM, provided by qemu
// 08000000 -- GICv2
// 09000000 -- uart0 
// 0a000000 -- virtio disk 
// 40000000 -- boot ROM jumps here in machine mode
//             -kernel loads the kernel here
// unused RAM after 40000000.

// the kernel uses physical memory thus:
// 40000000 -- entry.S, then kernel text and data
// end -- start of kernel page allocation area
// PHYSTOP -- end RAM used by the kernel

// qemu puts UART registers here in physical memory.
#define UART0 0x09000000L
#define UART0_IRQ 33

// virtio mmio interface
#define VIRTIO0  0x0a000000L
#define VIRTIO0_IRQ  48

// interrupt controller GICv2
#define GICV2 0x08000000L

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x40000000 to PHYSTOP.
#define KERNBASE 0x40000000L
#define PHYSTOP (KERNBASE + 128*1024*1024)

// map the trampoline page to the highest address,
// in both user and kernel space.
#define TRAMPOLINE (MAXVA - PGSIZE)

// map kernel stacks beneath the trampoline,
// each surrounded by invalid guard pages.
#define KSTACK(p) (TRAMPOLINE - ((p)+1)* 2*PGSIZE)

// User memory layout.
// Address zero first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
//   ...
//   TRAPFRAME (p->trapframe, used by the trampoline)
//   TRAMPOLINE (the same page as in the kernel)
#define TRAPFRAME (TRAMPOLINE - PGSIZE)
