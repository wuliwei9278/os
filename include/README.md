## Directory Introduction

- include  — library header path
  - hal — hardware abstraction layer
  - rpvm — functionality above HAL

## Why using BCM2835?

- BCM2836 is the Broadcom chip used in the Raspberry Pi 2 Model B.
- However, the underlying architecture in BCM2836 is identical to BCM2835. The only significant difference is the removal of the ARM1176JZF-S processor and replacement with a quad-core Cortex-A7 cluster.