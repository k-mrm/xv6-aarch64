#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "aarch64.h"
#include "defs.h"

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
  main();
}

__attribute__((aligned(PGSIZE))) pte_t l1entrypgt[512];
__attribute__((aligned(PGSIZE))) pte_t l2entrypgt[512];
__attribute__((aligned(PGSIZE))) pte_t l1kpgt[512];
__attribute__((aligned(PGSIZE))) pte_t l2kpgt[512];

/*
void
entrypgtinit()
{
  // Phase 1
  for(uint64 va = 0x40000000; va < (uint64)end; va += 2*1024*1024) {
    l2entrypgt[PX(2,va)] = PA2PTE(va) | PTE_AF | PTE_INDX(AI_NORMAL_NC_IDX) | PTE_VALID;
    l1entrypgt[PX(1,va)] = PA2PTE(l2entrypgt) | PTE_TABLE | PTE_VALID;
  }

  // Phase 2
  for(uint64 va = 0xffffff8040000000; va < (uint64)P2V(end); va += 2*1024*1024) {
    l2kpgt[PX(2,va)] = PA2PTE(V2P(va)) | PTE_AF | PTE_INDX(AI_NORMAL_NC_IDX) | PTE_VALID;
    l1kpgt[PX(1,va)] = PA2PTE(l2kpgt) | PTE_TABLE | PTE_VALID;
  }
}
*/
