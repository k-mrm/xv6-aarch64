.section "text"

.global psci_fn
psci_fn:
  hvc #0
  ret
