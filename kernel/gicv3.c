#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "aarch64.h"
#include "defs.h"

//
// aarch64 Global Interrupt Controller (GICv3).
//

#define D_CTLR          0x0
#define D_TYPER         0x4
#define D_IGROUPR(n)    (0x80 + (uint64)(n) * 4)
#define D_ISENABLER(n)  (0x100 + (uint64)(n) * 4)
#define D_ICENABLER(n)  (0x180 + (uint64)(n) * 4)
#define D_ISPENDR(n)    (0x200 + (uint64)(n) * 4)
#define D_ICPENDR(n)    (0x280 + (uint64)(n) * 4)
#define D_IPRIORITYR(n) (0x400 + (uint64)(n) * 4)
#define D_ITARGETSR(n)  (0x800 + (uint64)(n) * 4)
#define D_ICFGR(n)      (0xc00 + (uint64)(n) * 4)

#define R_CTLR           0x0
#define R_WAKER          0x14

#define SGI_BASE  0x10000
#define R_IGROUPR0       (SGI_BASE+0x80)
#define R_ISENABLER0     (SGI_BASE+0x100)
#define R_ICENABLER0     (SGI_BASE+0x180)
#define R_ICPENDR0       (SGI_BASE+0x280)
#define R_IPRIORITYR(n)  (SGI_BASE+0x400+(n)*4)
#define R_ICFGR0         (SGI_BASE+0xc00)
#define R_ICFGR1         (SGI_BASE+0xc04)
#define R_IGRPMODR0      (SGI_BASE+0xd00)

static void gic_setup_ppi(uint32 cpuid, uint32 intid) __attribute__((unused));
static void gic_setup_spi(uint32 intid);

static struct {
  char *gicd;
  char *rdist_addrs[NCPU];
} gicv3;

static inline uint32
icc_igrpen1_el1()
{
  uint32 x;
  asm volatile("mrs %0, S3_0_C12_C12_7" : "=r"(x));
  return x;
}

static inline void
w_icc_igrpen1_el1(uint32 x)
{
  asm volatile("msr S3_0_C12_C12_7, %0" : : "r"(x));
}

static inline uint32
icc_pmr_el1()
{
  uint32 x;
  asm volatile("mrs %0, S3_0_C4_C6_0" : "=r"(x));
  return x;
}

static inline void
w_icc_pmr_el1(uint32 x)
{
  asm volatile("msr S3_0_C4_C6_0, %0" : : "r"(x));
}

static inline uint32
icc_iar1_el1()
{
  uint32 x;
  asm volatile("mrs %0, S3_0_C12_C12_0" : "=r"(x));
  return x;
}

static inline void
w_icc_eoir1_el1(uint32 x)
{
  asm volatile("msr S3_0_C12_C12_1, %0" : : "r"(x));
}

static inline uint32
icc_sre_el1()
{
  uint32 x;
  asm volatile("mrs %0, S3_0_C12_C12_5" : "=r"(x));
  return x;
}

static inline void
w_icc_sre_el1(uint32 x)
{
  asm volatile("msr S3_0_C12_C12_5, %0" : : "r"(x));
}

static void
gicd_write(uint32 off, uint32 val)
{
  *(volatile uint32 *)(gicv3.gicd + off) = val;
}

static uint32
gicd_read(uint32 off)
{
  return *(volatile uint32 *)(gicv3.gicd + off);
}

static void
gicr_write(uint32 cpuid, uint32 off, uint32 val)
{
  *(volatile uint32 *)(gicv3.rdist_addrs[cpuid] + off) = val;
}

static uint32
gicr_read(uint32 cpuid, uint32 off)
{
  return *(volatile uint32 *)(gicv3.rdist_addrs[cpuid] + off);
}

static void
giccinit()
{
  w_icc_igrpen1_el1(0);
  w_icc_pmr_el1(0xff);
}

static void
gicdinit()
{
  gicd_write(D_CTLR, 0);

  uint32 typer = gicd_read(D_TYPER);
  uint32 lines = typer & 0x1f;

  printf("lines %d\n", lines);

  for(int i = 0; i < lines; i++)
    gicd_write(D_IGROUPR(i), ~0);
}

static void 
gicrinit(uint32 cpuid)
{
  gicr_write(cpuid, R_CTLR, 0);

  w_icc_sre_el1(icc_sre_el1() | 1);

  gicr_write(cpuid, R_IGROUPR0, ~0);
  gicr_write(cpuid, R_IGRPMODR0, 0);

  uint32 waker = gicr_read(cpuid, R_WAKER);
  gicr_write(cpuid, R_WAKER, waker & ~(1<<1));
  while(gicr_read(cpuid, R_WAKER) & (1<<2))
    ;
}

static void
gic_enable()
{
  gicd_write(D_CTLR, (1<<1));
  w_icc_igrpen1_el1(1);
}

void
gicv3init()
{
  gicv3.gicd = (char *)GICV3;
  for(int i = 0; i < NCPU; i++) {
    gicv3.rdist_addrs[i] = (char *)(GICV3_REDIST + (i) * 0x20000);
  }

  gicdinit();

  gic_setup_spi(UART0_IRQ);
  gic_setup_spi(VIRTIO0_IRQ);
}

void
gicv3inithart()
{
  int cpu = cpuid();

  giccinit();
  gicrinit(cpu);

  gic_setup_ppi(cpu, TIMER0_IRQ);

  gic_enable();
}

static void
gic_enable_int(uint32 intid)
{
  uint32 is = gicd_read(D_ISENABLER(intid / 32));
  is |= 1 << (intid % 32);
  gicd_write(D_ISENABLER(intid / 32), is);
}

int
gic_int_enabled(uint32 intid)
{
  uint32 is = gicd_read(D_ISENABLER(intid / 32));
  return is & (1 << (intid % 32));
}

static void
gic_clear_pending(uint32 intid)
{
  uint32 ic = gicd_read(D_ICPENDR(intid / 32));
  ic |= 1 << (intid % 32);
  gicd_write(D_ICPENDR(intid / 32), ic);
}

static void
gic_set_prio0(uint32 intid)
{
  // set priority to 0
  uint32 p = gicd_read(D_IPRIORITYR(intid / 4));
  p &= ~((uint32)0xff << (intid % 4 * 8)); // set prio 0
  gicd_write(D_IPRIORITYR(intid / 4), p);
}

static void
gic_set_target(uint32 intid, uint32 cpuid)
{
  uint32 itargetsr = gicd_read(D_ITARGETSR(intid / 4));
  itargetsr &= ~((uint32)0xff << (intid % 4 * 8));
  gicd_write(D_ITARGETSR(intid / 4), itargetsr | ((uint32)(1 << cpuid) << (intid % 4 * 8)));
}

static void
gicr_enable_int(uint32 cpuid, uint32 intid)
{
  uint32 is = gicr_read(cpuid, R_ISENABLER0);
  is |= 1 << (intid % 32);
  gicr_write(cpuid, R_ISENABLER0, is);
}

static void
gicr_clear_pending(uint32 cpuid, uint32 intid)
{
  uint32 ic = gicr_read(cpuid, R_ICPENDR0);
  ic |= 1 << (intid % 32);
  gicr_write(cpuid, R_ICPENDR0, ic);
}

static void
gicr_set_prio0(uint32 cpuid, uint32 intid)
{
  uint32 p = gicr_read(cpuid, R_IPRIORITYR(intid / 4));
  p &= ~((uint32)0xff << (intid % 4 * 8)); // set prio 0
  gicr_write(cpuid, R_IPRIORITYR(intid / 4), p);
}

static void
gic_setup_ppi(uint32 cpuid, uint32 intid)
{
  gicr_set_prio0(cpuid, intid);
  gicr_clear_pending(cpuid, intid);
  gicr_enable_int(cpuid, intid);
}

static void
gic_setup_spi(uint32 intid)
{
  gic_set_prio0(intid);

  // all interrupts are handled by cpu0　
  gic_set_target(intid, 0);

  gic_clear_pending(intid);
  gic_enable_int(intid);
}


// irq from iar
int
gic_iar_irq(uint32 iar)
{
  return iar & 0x3ff;
}

// interrupt acknowledge register:
// ask GIC what interrupt we should serve.
uint32
gic_iar()
{
  return icc_iar1_el1();
}

// tell GIC we've served this IRQ.
void
gic_eoi(uint32 iar)
{
  w_icc_eoir1_el1(iar);
}

