#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "aarch64.h"
#include "defs.h"

// armv8 generic timer driver

#define CNTV_CTL_ENABLE   (1<<0)
#define CNTV_CTL_IMASK    (1<<1)
#define CNTV_CTL_ISTATUS  (1<<2)

static void enable_timer(void);
static void disable_timer(void);
static void reload_timer(void);

void
timerinit()
{
  disable_timer();
  reload_timer();
  enable_timer();
}

static void
enable_timer()
{
  uint64 c = r_cntv_ctl_el0();
  c |= CNTV_CTL_ENABLE;
  c &= ~CNTV_CTL_IMASK;
  w_cntv_ctl_el0(c);
}

static void
disable_timer()
{
  uint64 c = r_cntv_ctl_el0();
  c &= ~CNTV_CTL_ENABLE;
  c |= CNTV_CTL_IMASK;
  w_cntv_ctl_el0(c);
}

static void
reload_timer()
{
  // interval 100ms
  uint64 interval = 100000;
  uint64 interval_clk = interval * (r_cntfrq_el0() / 1000000);

  w_cntv_tval_el0(interval_clk);
}

void
delay(uint32 cycles)
{
  uint64 start = r_cntvct_el0();

  while ((r_cntvct_el0() - start) < cycles)
    asm volatile("yield" ::: "memory");
}

void
timerintr()
{
  disable_timer();
  reload_timer();
  enable_timer();
}
