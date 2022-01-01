#define PGSIZE 4096 // bytes per page
#define PGSHIFT 12  // bits of offset within a page

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#define PTE_VALID 1L  // level 0,1,2 descriptor: valid
#define PTE_TABLE 2L  // level 0,1,2 descriptor: table
#define PTE_V     3L  // level 3 descriptor: valid
// PTE_AF(Access Flag)
//
// 0 -- this block entry has not yet.
// 1 -- this block entry has been used.
#define PTE_AF  (1L << 10)
// PTE_AP(Access Permission) is 2bit field.
//        EL0   EL1
// 00 --   x    RW
// 01 --  RW    RW
// 10 --   x    RO
// 11 --  RO    RO
#define PTE_AP(ap)  (((ap) & 3L) << 6)
#define PTE_U   PTE_AP(1)
#define PTE_RO  PTE_AP(2)
#define PTE_URO PTE_AP(3)
#define PTE_PXN (1L << 53)   // Privileged eXecute Never
#define PTE_UXN (1L << 54)   // Unprivileged(user) eXecute Never
#define PTE_XN (PTE_PXN|PTE_UXN)  // eXecute Never

// attribute index
// index is set by mair_el1
#define AI_DEVICE_nGnRnE_IDX  0x0
#define AI_NORMAL_NC_IDX      0x1

// memory type
#define MT_DEVICE_nGnRnE  0x0
#define MT_NORMAL_NC      0x44

#define PTE_INDX(i) ((i & 7) << 2)
#define PTE_DEVICE  PTE_INDX(AI_DEVICE_nGnRnE_IDX)
#define PTE_NORMAL  PTE_INDX(AI_NORMAL_NC_IDX)

// shift a physical address to the right place for a PTE.
#define PA2PTE(pa)  ((uint64)(pa) & 0xfffffffff000)
#define PTE2PA(pte) ((uint64)(pte) & 0xfffffffff000)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)

//
//  39bit(=512GB) Virtual Address
//
//     63   39 38    30 29    21 20    12 11       0
//    +-------+--------+--------+--------+----------+
//    | TTBRn | level1 | level2 | level3 | page off |
//    +-------+--------+--------+--------+----------+
//
//

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK          0x1FF // 9 bits
#define PXSHIFT(level)  (39-(level)*9)
#define PX(level, va) ((((uint64)(va)) >> PXSHIFT(level)) & PXMASK)

typedef uint64 pte_t;
typedef uint64 *pagetable_t; // 512 PTEs

// translation control register
#define TCR_T0SZ(n)   ((n) & 0x3f)
#define TCR_IRGN0(n)  (((n) & 0x3) << 8)
#define TCR_ORGN0(n)  (((n) & 0x3) << 10)
#define TCR_SH0(n)    (((n) & 0x3) << 12)
#define TCR_TG0(n)    (((n) & 0x3) << 14)
#define TCR_T1SZ(n)   (((n) & 0x3f) << 16)
#define TCR_A1(n)     (((n) & 0x1) << 22)
#define TCR_EPD1(n)   (((n) & 0x1) << 23)
#define TCR_IRGN1(n)  (((n) & 0x3) << 24)
#define TCR_ORGN1(n)  (((n) & 0x3) << 26)
#define TCR_SH1(n)    (((n) & 0x3) << 28)
#define TCR_TG1(n)    (((n) & 0x3) << 30)
#define TCR_IPS(n)    (((n) & 0x7) << 32)
#define TCR_AS(n)     (((n) & 0x1) << 36)
#define TCR_TBI0(n)   (((n) & 0x1) << 37)
#define TCR_TBI1(n)   (((n) & 0x1) << 38)
