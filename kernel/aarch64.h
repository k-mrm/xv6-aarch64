// which core is this?
static inline uint64
cpuid()
{
  uint64 x;
  asm volatile("mrs %0, mpidr_el1" : "=r" (x) );
  return x & 0xff;
}

// Machine Status Register, mstatus

#define MSTATUS_MPP_MASK (3L << 11) // previous mode.
#define MSTATUS_MPP_M (3L << 11)
#define MSTATUS_MPP_S (1L << 11)
#define MSTATUS_MPP_U (0L << 11)
#define MSTATUS_MIE (1L << 3)    // machine-mode interrupt enable.

static inline uint64
r_mstatus()
{
  uint64 x;
  asm volatile("csrr %0, mstatus" : "=r" (x) );
  return x;
}

static inline void 
w_mstatus(uint64 x)
{
  asm volatile("csrw mstatus, %0" : : "r" (x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
static inline void 
w_mepc(uint64 x)
{
  asm volatile("csrw mepc, %0" : : "r" (x));
}

// Supervisor Status Register, sstatus

#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable

static inline uint64
r_sstatus()
{
  uint64 x;
  asm volatile("csrr %0, sstatus" : "=r" (x) );
  return x;
}

static inline void 
w_sstatus(uint64 x)
{
  asm volatile("csrw sstatus, %0" : : "r" (x));
}

// Supervisor Interrupt Pending
static inline uint64
r_sip()
{
  uint64 x;
  asm volatile("csrr %0, sip" : "=r" (x) );
  return x;
}

static inline void 
w_sip(uint64 x)
{
  asm volatile("csrw sip, %0" : : "r" (x));
}

// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software
static inline uint64
r_sie()
{
  uint64 x;
  asm volatile("csrr %0, sie" : "=r" (x) );
  return x;
}

static inline void 
w_sie(uint64 x)
{
  asm volatile("csrw sie, %0" : : "r" (x));
}

// Machine-mode Interrupt Enable
#define MIE_MEIE (1L << 11) // external
#define MIE_MTIE (1L << 7)  // timer
#define MIE_MSIE (1L << 3)  // software
static inline uint64
r_mie()
{
  uint64 x;
  asm volatile("csrr %0, mie" : "=r" (x) );
  return x;
}

static inline void 
w_mie(uint64 x)
{
  asm volatile("csrw mie, %0" : : "r" (x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
static inline void 
w_sepc(uint64 x)
{
  asm volatile("csrw sepc, %0" : : "r" (x));
}

static inline uint64
r_sepc()
{
  uint64 x;
  asm volatile("csrr %0, sepc" : "=r" (x) );
  return x;
}

// Vector Base Address Register in EL1
static inline void
w_vbar_el1(uint64 x)
{
  asm volatile("msr vbar_el1, %0" : : "r" (x));
}

static inline uint64
r_vbar_el1()
{
  uint64 x;
  asm volatile("mrs %0, vbar_el1" : "=r" (x) );
  return x;
}

static inline void
w_pmpaddr0(uint64 x)
{
  asm volatile("csrw pmpaddr0, %0" : : "r" (x));
}

// TODO: fix comment
// supervisor address translation and protection;
// holds the address of the page table.
static inline void
w_ttbr0_el1(uint64 x)
{
  asm volatile("msr ttbr0_el1, %0" : : "r" (x));
}

static inline uint64
r_ttbr0_el1(void)
{
  uint64 x;
  asm volatile("mrs %0, ttbr0_el1" : "=r" (x) );
  return x;
}

static inline void
w_ttbr1_el1(uint64 x)
{
  asm volatile("msr ttbr1_el1, %0" : : "r" (x));
}

static inline uint64
r_ttbr1_el1(void)
{
  uint64 x;
  asm volatile("mrs %0, ttbr1_el1" : "=r" (x) );
  return x;
}

// Supervisor Scratch register, for early trap handler in trampoline.S.
static inline void 
w_sscratch(uint64 x)
{
  asm volatile("csrw sscratch, %0" : : "r" (x));
}

static inline void 
w_mscratch(uint64 x)
{
  asm volatile("csrw mscratch, %0" : : "r" (x));
}

// Supervisor Trap Cause
static inline uint64
r_scause()
{
  uint64 x;
  asm volatile("csrr %0, scause" : "=r" (x) );
  return x;
}

// Supervisor Trap Value
static inline uint64
r_stval()
{
  uint64 x;
  asm volatile("csrr %0, stval" : "=r" (x) );
  return x;
}

// armv8 generic timer
static inline uint64
r_cntv_ctl_el0()
{
  uint64 x;
  asm volatile("mrs %0, cntv_ctl_el0" : "=r"(c) );
  return x;
}

static inline void
w_cntv_ctl_el0(uint64 x)
{
  asm volatile("msr cntv_ctl_el0, %0" : : "r"(x) );
}

static inline uint64
r_cntv_cval_el0()
{
  uint64 x;
  asm volatile("mrs %0, cntv_cval_el0" : "=r"(x) );
  return x;
}

static inline void
w_cntv_cval_el0(uint64 x) {
  asm volatile("msr cntv_cval_el0, %0" : : "r"(x) );
}

static inline uint64
r_cntvct_el0()
{
  uint64 x;
  asm volatile("mrs %0, cntvct_el0" : "=r"(x) );
  return x;
}

static inline uint64
r_cntfrq_el0()
{
  uint64 x;
  asm volatile("mrs %0, cntfrq_el0" : "=r"(x) );
  return x;
}

// enable device interrupts(irq)
static inline void
intr_on()
{
  asm volatile("msr daifclr, #0x2" ::: "memory");
}

// disable device interrupts(irq)
static inline void
intr_off()
{
  asm volatile("msr daifset, #0x2" ::: "memory");
}

static inline uint64
daif() {
  uint64 d;
  asm volatile("mrs %0, daif" : "=r" (d));
  return d;
}

// are device interrupts(irq) enabled?
static inline int
intr_get()
{
  uint64 x = daif();
  return (x & 0x2) != 0;
}

static inline uint64
r_sp()
{
  uint64 x;
  asm volatile("mov %0, sp" : "=r" (x) );
  return x;
}

static inline uint64
r_ra()
{
  uint64 x;
  asm volatile("mov %0, x30" : "=r" (x) );
  return x;
}

static inline void
isb()
{
  asm volatile("isb");
}

static inline void
dsb()
{
  asm volatile("dsb sy");
}

// flush the TLB.
static inline void
flush_tlb()
{
  asm volatile("dsb ishst");
  asm volatile("tlbi vmalle1is");
  asm volatile("dsb ish");
  isb();
}

#define PGSIZE 4096 // bytes per page
#define PGSHIFT 12  // bits of offset within a page

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#define PTE_V (3L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // 1 -> user can access
#define PTE_PXN (1UL << 53)   // privileged XN
#define PTE_UXN (1UL << 54)   // user XN

// shift a physical address to the right place for a PTE.
#define PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#define PTE2PA(pte) (((pte) >> 10) << 12)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK          0x1FF // 9 bits
#define PXSHIFT(level)  (PGSHIFT+(9*(level)))
#define PX(level, va) ((((uint64) (va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))

typedef uint64 pte_t;
typedef uint64 *pagetable_t; // 512 PTEs
