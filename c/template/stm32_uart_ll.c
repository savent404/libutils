/**
 * @file stm32_uart_ll.c
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-08-19
 *
 * Copyright 2022 savent_gate
 *
 */

#include "uart_utils.h"
#include "usart.h"
#include <stdlib.h>

static uart_t *stm32_instance[4] = {NULL};
static const int stm32_instance_num =
    sizeof(stm32_instance) / sizeof(stm32_instance[0]);

static inline UART_HandleTypeDef *to_huart(void *data) {
  return (UART_HandleTypeDef *)data;
}

static void stm32_uart_rx_async(char *ch, void *data) {
  HAL_UART_Receive_IT(to_huart(data), (uint8_t *)ch, 1);
}

static void stm32_uart_tx_async(const char *ch, void *data) {
  HAL_UART_Transmit_IT(to_huart(data), (uint8_t *)ch, 1);
}

static void stm32_uart_tx_async_abort(void *data) {
  // no need to abort tx in interrupt mode
  // HAL_UART_AbortTransmit_IT(to_huart(data));
}

static void stm32_uart_rx_async_abort(void *data) {
  // no need to abort rx in interrupt mode
  // HAL_UART_AbortReceive_IT(to_huart(data));
}

static void stm32_isr_tx_handler(UART_HandleTypeDef *huart) {
  uart_t *inst;
  for (int i = 0; i < stm32_instance_num; i++) {
    inst = stm32_instance[i];
    if (to_huart(inst->privdata)->Instance == huart->Instance) {
      uart_isr_handle_tx(inst);
      return;
    }
  }
}

static void stm32_isr_rx_handler(UART_HandleTypeDef *huart) {
  uart_t *inst;
  for (int i = 0; i < stm32_instance_num; i++) {
    inst = stm32_instance[i];
    if (to_huart(inst->privdata)->Instance == huart->Instance) {
      uart_isr_handle_rx(inst);
      return;
    }
  }
}

uart_t *stm32_uart_init(UART_HandleTypeDef *huart) {
  uart_t *inst;
  uart_io_t *io;
  const int fifo_size = 32;
  int index = -1;

  for (int i = 0; i < stm32_instance_num; i++) {
    if (!stm32_instance[i]) {
      index = i;
      break;
    } else if (to_uart(stm32_instance[i]->privdata) &&
               to_uart(stm32_instance[i]->privdata)->Instance ==
                   huart->Instance) {
      return stm32_instance[i];
    }
  }

  if (!huart && index < 0)
    goto fatal1;

  inst = malloc(sizeof(*inst));
  if (!inst)
    goto fatal2;

  io = malloc(sizeof(*inst->io));
  if (!io)
    goto fatal3;
  io->uart_rx_async = stm32_uart_rx_async;
  io->uart_rx_async_abort = stm32_uart_rx_async_abort;
  io->uart_tx_async = stm32_uart_tx_async;
  io->uart_tx_async_abort = stm32_uart_tx_async_abort;
  inst->io = io;

  inst->rx_fifo = malloc(sizeof(*inst->rx_fifo));
  if (!inst->rx_fifo)
    goto fatal4;

  inst->tx_fifo = malloc(sizeof(*inst->tx_fifo));
  if (!inst->tx_fifo)
    goto fatal5;

  inst->privdata = huart;
  inst->status = uart_status_idle;
  inst->tx_enable = 0;
  inst->rx_enable = 0;

  inst->rx_fifo->buffer = malloc(fifo_size * 2);
  if (!inst->rx_fifo->buffer)
    goto fatal6;
  inst->rx_fifo->fifo_len = fifo_size;
  inst->rx_fifo->type_len = 1;
  inst->rx_fifo->index_start = 0;
  inst->rx_fifo->index_end = 0;

  inst->tx_fifo->fifo_len = fifo_size;
  inst->tx_fifo->type_len = 1;
  inst->tx_fifo->index_start = 0;
  inst->tx_fifo->index_end = 0;
  inst->tx_fifo->buffer = ((char *)inst->rx_fifo->buffer) + fifo_size;

  HAL_UART_RegisterCallback(huart, HAL_UART_TX_COMPLETE_CB_ID,
                            stm32_isr_tx_handler);
  HAL_UART_RegisterCallback(huart, HAL_UART_RX_COMPLETE_CB_ID,
                            stm32_isr_rx_handler);

  stm32_instance[index] = inst;

  return inst;
fatal6:
  free(inst->rx_fifo->buffer);
fatal5:
  free(inst->tx_fifo);
fatal4:
  free(inst->rx_fifo);
fatal3:
  free(io);
fatal2:
  free(inst);
fatal1:
  return NULL;
}

void stm32_uart_deinit(uart_t *inst) {
  int index = -1;

  for (int i = 0; i < stm32_instance_num; i++) {
    if (stm32_instance[i] == inst) {
      index = i;
      break;
    }
  }

  if (index < 0)
    return;

  uart_disable_rx(inst);
  uart_disable_tx(inst);
  stm32_instance[index] = NULL;
  free(inst->rx_fifo->buffer);
  free(inst->tx_fifo);
  free(inst->rx_fifo);
  free((void *)inst->io);
  free(inst);
}