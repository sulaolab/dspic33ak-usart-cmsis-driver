#ifndef DRIVER_USART_DSPIC33AK_H
#define DRIVER_USART_DSPIC33AK_H

/*
 * CMSIS-Driver USART wrapper for the dsPIC33AK UART HAL.
 *
 * Thin mapping of the ARM CMSIS-Driver USART API onto the dsPIC33AK UART HAL
 * asynchronous transfer model (dspic33ak_uart_tx_start / _rx_start + events).
 * No ARM_USART_* / ARM_DRIVER_* types appear in the HAL; they are confined to
 * this wrapper.
 *
 * Initial scope: UART1 only, ARM_USART_MODE_ASYNCHRONOUS, 8 data bits, no parity,
 * 1 stop bit, no flow control. All other modes/options report
 * ARM_DRIVER_ERROR_UNSUPPORTED and are reported as unavailable in the
 * capabilities.
 */

#include "Driver_USART.h"

#ifdef __cplusplus
extern "C" {
#endif

extern ARM_DRIVER_USART Driver_USART1;

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_USART_DSPIC33AK_H */
