#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "aarch64.h"
#include "defs.h"

//
// aarch64 Global Interrupt Controller (GICv2).
//

#define Reg(reg) ((volatile uint32 *)(GICV2 + (reg)))

// GICD base
#define RegD(reg)  Reg(reg)
// GICC base
#define RegC(reg)  Reg((reg) + 0x10000)

#define D_CTLR          0x0
#define D_TYPER         0x4
#define D_ISENABLER(n)  (0x100 + (uint64)(n) * 4)
#define D_ICENABLER(n)  (0x180 + (uint64)(n) * 4)
#define D_ISPENDR(n)    (0x200 + (uint64)(n) * 4)
#define D_ICPENDR(n)    (0x280 + (uint64)(n) * 4)
#define D_IPRIORITYR(n) (0x400 + (uint64)(n) * 4)
#define D_ITARGETSR(n)  (0x800 + (uint64)(n) * 4)
#define D_ICFGR(n)      (0xc00 + (uint64)(n) * 4)

#define C_CTLR  0x0 
#define C_PMR   0x4
#define C_IAR   0xc
#define C_EOIR  0x10
#define C_HPPIR 0x18
#define C_AIAR  0x20
#define C_AEOIR 0x24

void
giccinit()
{
  ;
}

void
gicdinit()
{
  ;
}

void
gicv2init()
{
  giccinit();
  gicdinit();
}
