# Upstream HAL Source

The files in this directory are a vendor copy of the dsPIC33AK UART HAL.

Upstream repository:

- Repository: https://github.com/sulaolab/dspic33ak-uart-hal
- Branch: fix/uart-irq-ifs2-ak128
- Source directory: src/

Synchronized into this repository under:

- Destination directory: src/hal_uart/

## Current Synchronized Revision

- Upstream commit: 26d18306c0e721b8c73070e19921b11e3ac470d4

This revision is synchronized from the upstream `fix/uart-irq-ifs2-ak128` branch.

## Update Policy

The HAL-only repository is the upstream source of truth.

Please apply HAL fixes and HAL feature changes to the upstream HAL repository first, then synchronize this vendor copy.

CMSIS-Driver wrapper changes belong in this repository.
