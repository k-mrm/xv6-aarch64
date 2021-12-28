#include "types.h"

int psci_fn(uint64 id, uint64 arg1, uint64 arg2, uint64 arg3);

#define PSCI_OFF     0x84000008L
#define PSCI_RESET   0x84000009L
#define PSCI_CPUON   0xc4000003L

static inline int
psci_cpu_on(uint64 cpuid, uint64 ep)
{
  return psci_fn(PSCI_CPUON, cpuid, ep);
}
