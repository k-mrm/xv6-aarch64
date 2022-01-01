#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "aarch64.h"
#include "defs.h"
#include "psci.h"

void _entry();
void main();
extern char end[];

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * NCPU];

// entry.S jumps here in supervisor mode (EL1) on stack0.
// in qemu-system-aarch64, default EL (Exeception Level) is 1.
void
start()
{
  // wakeup other cpu
  for(int id = 1; id < NCPU; id++)
    psci_cpuon(id, V2P(_entry));

  main();
}

__attribute__((aligned(PGSIZE))) pte_t l1entrypgt[512];
__attribute__((aligned(PGSIZE))) pte_t l2entrypgt[512];
__attribute__((aligned(PGSIZE))) pte_t l1kpgt[512];
__attribute__((aligned(PGSIZE))) pte_t l2kpgt1[512];
__attribute__((aligned(PGSIZE))) pte_t l2kpgt2[512];

// set up entry pagetable
//
// Phase 1.
// map the kernel code identically.
// map [0x40000000,PA(end)) to [0x40000000,PA(end))
// memory type is normal
//
// Phase 2.
// map the peripheral memory.
// map [0xffffff8008000000,0xffffff8040000000) to [0x8000000,0x40000000)
// memory type is device_nGnRnE.
//
// Phase 3.
// map the kernel code.
// map [0xffffff8040000000,VA(end)) to [0x40000000,PA(end))
// memory type is normal.
// 
void
entrypgtinit()
{
  pagetable_t pl1entrypgt = (pagetable_t)V2P(l1entrypgt);
  pagetable_t pl2entrypgt = (pagetable_t)V2P(l2entrypgt);
  pagetable_t pl1kpgt = (pagetable_t)V2P(l1kpgt);
  pagetable_t pl2kpgt1 = (pagetable_t)V2P(l2kpgt1);
  pagetable_t pl2kpgt2 = (pagetable_t)V2P(l2kpgt2);

  // Phase 1
  for(uint64 va = 0x40000000; va < V2P(end); va += 2*1024*1024) {
    pl2entrypgt[PX(2,va)] = PA2PTE(va) | PTE_AF | PTE_INDX(AI_NORMAL_NC_IDX) | PTE_VALID;
    pl1entrypgt[PX(1,va)] = PA2PTE(pl2entrypgt) | PTE_TABLE | PTE_VALID;
  }

  // Phase 2
  for(uint64 va = 0xffffff8008000000; va < 0xffffff8040000000; va += 2*1024*1024) {
    pl2kpgt1[PX(2,va)] = PA2PTE(V2P(va)) | PTE_AF | PTE_INDX(AI_DEVICE_nGnRnE_IDX) | PTE_VALID;
    pl1kpgt[PX(1,va)] = PA2PTE(pl2kpgt1) | PTE_TABLE | PTE_VALID;
  }

  // Phase 3
  for(uint64 va = 0xffffff8040000000; va < (uint64)end; va += 2*1024*1024) {
    pl2kpgt2[PX(2,va)] = PA2PTE(V2P(va)) | PTE_AF | PTE_INDX(AI_NORMAL_NC_IDX) | PTE_VALID;
    pl1kpgt[PX(1,va)] = PA2PTE(pl2kpgt2) | PTE_TABLE | PTE_VALID;
  }
}
