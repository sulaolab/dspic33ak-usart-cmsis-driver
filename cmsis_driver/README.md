# CMSIS-Driver USART Wrapper for dsPIC33AK UART HAL

## Overview

This directory provides a CMSIS-Driver USART wrapper for the dsPIC33AK UART HAL.

The wrapper maps the ARM CMSIS-Driver USART API onto the UART HAL asynchronous
transfer model (non-blocking `Send`/`Receive` with completion and error events).
The initial implementation targets UART1 in asynchronous (UART) mode only.

## File layout

```text
cmsis_driver/
  Driver_USART_dsPIC33AK.c
  Driver_USART_dsPIC33AK.h
  RTE_Device_USART_dsPIC33AK_example.h
  README.md
```

In this repository, these files are located under the top-level `cmsis_driver/` folder.

## Instance mapping

| CMSIS driver object | dsPIC33AK HAL instance |
|---|---|
| `Driver_USART1` | `DSPIC33AK_UART_INST_1` |

Only `Driver_USART1` is provided by the initial wrapper. Additional instances can
be added later following the same pattern.

## Supported features

- `ARM_USART_MODE_ASYNCHRONOUS` only
- 8 data bits, no parity, 1 stop bit, no flow control
- `Send()` (non-blocking) on top of `dspic33ak_uart_tx_start()`
- `Receive()` (non-blocking) on top of `dspic33ak_uart_rx_start()`
- `GetTxCount()` / `GetRxCount()`
- `Control(ARM_USART_CONTROL_TX, ...)` / `Control(ARM_USART_CONTROL_RX, ...)`
- `Control(ARM_USART_MODE_ASYNCHRONOUS | 8N1 | no-flow, baudrate)`
- `Control(ARM_USART_ABORT_SEND | ABORT_RECEIVE | ABORT_TRANSFER)`
- `GetStatus()` (tx_busy / rx_busy / rx_overflow / rx_framing_error / rx_parity_error)
- Signal events through the callback registered at `Initialize()`:
  - `ARM_USART_EVENT_SEND_COMPLETE` (all data submitted to the TX FIFO)
  - `ARM_USART_EVENT_RECEIVE_COMPLETE`
  - `ARM_USART_EVENT_RX_OVERFLOW`, `ARM_USART_EVENT_RX_FRAMING_ERROR`,
    `ARM_USART_EVENT_RX_PARITY_ERROR`

`ARM_USART_EVENT_SEND_COMPLETE` reflects the HAL "all data submitted to the
FIFO/register" sense, not physical shift-register-empty. The wrapper never
signals `ARM_USART_EVENT_TX_COMPLETE`, and `capabilities.event_tx_complete` is 0.

## Unsupported features and limitations

The initial wrapper intentionally does not support (capabilities report 0; calls
return `ARM_DRIVER_ERROR_UNSUPPORTED`):

- Synchronous master / synchronous slave
- Single-wire, IrDA, Smart Card
- RTS / CTS flow control and modem control lines
- 9-bit, 5/6/7-bit data, parity, 2 / 1.5 / 0.5 stop bits
- `Transfer()` (synchronous full-duplex)
- Physical transmit-complete event (`ARM_USART_EVENT_TX_COMPLETE`)
- `SetModemControl()` returns `ARM_DRIVER_ERROR_UNSUPPORTED`; `GetModemStatus()`
  returns an all-zero status

## Transfer-state requirements

- `Send()` requires TX enabled and a non-zero `tx_irq_priority`; otherwise the
  underlying `dspic33ak_uart_tx_start()` reports unsupported (a transfer with no
  servicing interrupt would never complete).
- `Receive()` requires RX enabled and ISR ring RX mode. The wrapper flushes the
  RX ISR ring at the start of `Receive()`, so a transfer captures only bytes that
  arrive after the call.
- `Control(ARM_USART_CONTROL_TX/RX, 0)` (disable) is rejected with
  `ARM_DRIVER_ERROR_BUSY` while a transfer on that direction is active.
- The buffer passed to `Send()` must stay valid until
  `ARM_USART_EVENT_SEND_COMPLETE`; the buffer passed to `Receive()` until
  `ARM_USART_EVENT_RECEIVE_COMPLETE` (the HAL transfers straight from/into it).
- The registered callback may run in interrupt (TX/RX ISR) context. Keep it
  short and non-blocking; do not call back into a blocking driver API from it.

## Interrupt vectors

The HAL does not define the device interrupt vectors. The application owns them:

- The `_UxRXInterrupt` vector must call `dspic33ak_uart_rx_irq_handler()`.
- For async TX, the `_UxTXInterrupt` vector must call
  `dspic33ak_uart_tx_irq_handler()`.

## Configuration through RTE_Device_USART_dsPIC33AK_example.h

`RTE_Device_USART_dsPIC33AK_example.h` is an example configuration file for this
USART CMSIS-Driver wrapper. It is not intended to be a shared application-level
`RTE_Device.h`; in an integrated application, copy the required USART definitions
into that application's own `RTE_Device.h` or equivalent configuration header. Do
not add I2C/SPI/etc. settings to this USART example file.

Example:

```c
#define RTE_USART1 1

#define RTE_USART1_UART_CLK_HZ       200000000u
#define RTE_USART1_BAUDRATE          230400u
#define RTE_USART1_TX_ENABLE         1u
#define RTE_USART1_RX_ENABLE         1u
#define RTE_USART1_RX_RING_SIZE      256u
#define RTE_USART1_RX_IRQ_PRIORITY   5u
#define RTE_USART1_TX_IRQ_PRIORITY   5u
```

Adjust `UART_CLK_HZ` / `BAUDRATE` / IRQ priorities / RX ring size to the target
board's UART clock and routing.

## Include path

The wrapper uses header names without hard-coded relative paths. The dsPIC33AK
UART HAL vendor copy is provided under `src/hal_uart/`, and a minimal copy of the
ARM CMSIS-Driver API headers under `third_party/arm_cmsis_driver/Include/`. The
build system should provide include paths for:

```text
src/hal_uart
cmsis_driver
third_party/arm_cmsis_driver/Include
```

`Driver_USART.h` is resolved from the vendored ARM CMSIS-Driver headers under
`third_party/arm_cmsis_driver/Include/` (Apache-2.0, copied unmodified). A
different CMSIS-Driver package may be substituted by adjusting this include path.

## Basic usage

```c
#include "Driver_USART_dsPIC33AK.h"

extern ARM_DRIVER_USART Driver_USART1;

static volatile uint32_t g_events;
static void usart_cb(uint32_t event) { g_events |= event; }

uint8_t rx[16];

Driver_USART1.Initialize(usart_cb);
Driver_USART1.PowerControl(ARM_POWER_FULL);
Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE,
                      230400);
Driver_USART1.Control(ARM_USART_CONTROL_TX, 1);
Driver_USART1.Control(ARM_USART_CONTROL_RX, 1);

Driver_USART1.Send("hello\r\n", 7);   /* completes with ARM_USART_EVENT_SEND_COMPLETE */
Driver_USART1.Receive(rx, sizeof(rx)); /* completes with ARM_USART_EVENT_RECEIVE_COMPLETE */
```

## Verified behavior

The wrapper has been verified on the perseus_512_96K project (dsPIC33AK512MPS512,
UART1 at 230400 baud, 8N1):

- `Send()` of 1 byte / short string / 200 bytes (larger than the TX FIFO) all
  complete with a single `ARM_USART_EVENT_SEND_COMPLETE` and matching `GetTxCount()`.
- `Receive(buf, 16)` completes with `ARM_USART_EVENT_RECEIVE_COMPLETE`,
  `GetRxCount() == 16`, and the received bytes match what was sent.
- A second `Send()` / `Receive()` while one is active returns `ARM_DRIVER_ERROR_BUSY`.
- `Control(ARM_USART_ABORT_SEND / ABORT_RECEIVE)` aborts an active transfer.
- Unsupported modes/options (synchronous, IrDA, RTS/CTS, parity, 2 stop bits)
  return `ARM_DRIVER_ERROR_UNSUPPORTED`.
