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

#define EXTMEM    0x40000000L               // Start of extended memory
#define PHYSTOP   (EXTMEM + 128*1024*1024)  // Top physical memory

#define KERNBASE  0xffffff8000000000L     // First kernel virtual address
#define KERNLINK  (KERNBASE + EXTMEM)     // virtual address where kernel is linked

#define V2P(a) (((uint64)(a)) - KERNBASE)
#define P2V(a) ((void *)(((char *)(a)) + KERNBASE))

#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)    // same as P2V, but without casts

// one beyond the highest possible virtual address.
#define MAXVA (KERNBASE + (1ULL<<38))

// qemu puts UART registers here in physical memory.
#define UART0 (KERNBASE + 0x09000000L)
#define UART0_IRQ 33

// virtio mmio interface
#define VIRTIO0  (KERNBASE + 0x0a000000L)
#define VIRTIO0_IRQ  48

#define TIMER0_IRQ  27

// interrupt controller GICv3
#define GICV3         (KERNBASE + 0x08000000L)
#define GICV3_REDIST  (KERNBASE + 0x080a0000L)

// map kernel stacks beneath the trampoline,
// each surrounded by invalid guard pages.
#define KSTACK(p) (MAXVA - ((p)+1) * 2*PGSIZE)
