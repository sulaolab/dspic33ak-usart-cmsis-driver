# dspic33ak-usart-cmsis-driver

CMSIS-Driver USART wrapper package for the dsPIC33AK UART HAL.

This repository provides a CMSIS-Driver USART wrapper together with a vendor copy of the dsPIC33AK UART HAL.

## Repository Layout

```text
src/
  hal_uart/
    dspic33ak_uart.c
    dspic33ak_uart.h
    dspic33ak_uart_device.c
    dspic33ak_uart_device.h
    dspic33ak_uart_reg.h
    dspic33ak_uart_rx_isr_ring.c
    dspic33ak_uart_rx_isr_ring.h
    UPSTREAM.md

tools/
  sync_hal_from_upstream.py

cmsis_driver/
  Driver_USART_dsPIC33AK.c
  Driver_USART_dsPIC33AK.h
  RTE_Device_USART_dsPIC33AK_example.h
  README.md

third_party/
  arm_cmsis_driver/
    README.md
    LICENSE.txt
    Include/
      Driver_Common.h
      Driver_USART.h
```

## Current Status

The HAL vendor copy has been imported from:

- https://github.com/sulaolab/dspic33ak-uart-hal

The CMSIS-Driver wrapper files are provided under `cmsis_driver/`.

The wrapper maps the ARM CMSIS-Driver USART API onto the UART HAL asynchronous
transfer model (non-blocking `Send`/`Receive` with completion and error events).
The initial implementation targets UART1 in asynchronous (UART) mode only:
8 data bits, no parity, 1 stop bit, no flow control. Other modes/options report
`ARM_DRIVER_ERROR_UNSUPPORTED`. See `cmsis_driver/README.md` for details.

`RTE_Device_USART_dsPIC33AK_example.h` is a USART-only example configuration file.

The wrapper currently includes this USART-only example header directly. For a
real application, either edit/copy this example for the target board, or adjust
the wrapper include to point to the application's own configuration header.

Do not use a generic `RTE_Device.h` in this repository, and do not add I2C/SPI/etc.
settings to this USART example file.

A minimal copy of the ARM CMSIS-Driver API headers (`Driver_USART.h`,
`Driver_Common.h`) is vendored under `third_party/arm_cmsis_driver/Include/` so
the wrapper builds without a separate CMSIS installation. See
`third_party/arm_cmsis_driver/README.md` for the source and license.

## Include Path

Applications or build systems should provide include paths for:

```text
src/hal_uart
cmsis_driver
third_party/arm_cmsis_driver/Include
```

`Driver_USART.h` is resolved from the vendored ARM CMSIS-Driver headers under
`third_party/arm_cmsis_driver/Include/` (Apache-2.0, copied unmodified). A
different CMSIS-Driver package may be substituted by adjusting this include path.

## Interrupt vectors

The HAL does not define the device interrupt vectors; the application owns them.
The `_UxRXInterrupt` vector must call `dspic33ak_uart_rx_irq_handler()`, and for
asynchronous TX the `_UxTXInterrupt` vector must call
`dspic33ak_uart_tx_irq_handler()`.

## HAL Synchronization

The HAL vendor copy under `src/hal_uart/` can be synchronized from the upstream HAL-only repository using:

```powershell
# Windows (Python launcher)
py -3 tools/sync_hal_from_upstream.py

# or, if python is on PATH
python tools/sync_hal_from_upstream.py
```

## Upstream HAL Policy

The HAL-only repository is the upstream source of truth:

- https://github.com/sulaolab/dspic33ak-uart-hal

HAL fixes should be applied to the upstream HAL repository first, then synchronized into this repository.

CMSIS-Driver wrapper changes should be made in this repository.

## License

The original dsPIC33AK UART CMSIS-Driver wrapper code in this repository is
licensed under the MIT No Attribution License (MIT-0). See `LICENSE`.

The vendored ARM CMSIS-Driver headers under
`third_party/arm_cmsis_driver/` are provided by ARM under the Apache License
2.0. See `third_party/arm_cmsis_driver/LICENSE.txt`.
