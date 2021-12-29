#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "aarch64.h"
#include "defs.h"
#include "psci.h"

void main();

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * NCPU];

// entry.S jumps here in supervisor mode (EL1) on stack0.
// in qemu-system-aarch64, default EL (Exeception Level) is 1.
void
start()
{
  // wakeup other cpu
  for(int id = 1; id < NCPU; id++)
    psci_cpuon(id, (uint64)main);

  main();
}
