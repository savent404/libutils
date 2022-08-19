/**
 * @file uart_utils.h
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-08-16
 *
 * Copyright 2022 savent_gate
 *
 */
#pragma once

#include "fifo_utils.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  // char (*uart_rx_poll)();
  // void (*uart_tx_poll)(char ch);
  void (*uart_rx_async)(char *ch, void *data);
  void (*uart_rx_async_abort)(void *data);
  void (*uart_tx_async)(const char *ch, void *data);
  void (*uart_tx_async_abort)(void *data);
} uart_io_t;

typedef enum {
  uart_status_idle = 0,
  uart_status_tx,
  uart_status_rx,
  uart_status_err,
} uart_status_t;

typedef struct {
  const uart_io_t *io;
  fifo_t *rx_fifo;
  fifo_t *tx_fifo;
  void *data;
  char rx_tmp, tx_tmp;
  uart_status_t status : 2;
  bool tx_enable : 1;
  bool rx_enable : 1;
} uart_t;

/**
 * @brief initialize uart instance, turn to idle
 *
 * @param inst
 * @param io
 * @param data_ptr
 * @param rx_fifo
 * @param tx_fifo
 */
void uart_init(uart_t *inst, const uart_io_t *io, void *data_ptr,
               fifo_t *rx_fifo, fifo_t *tx_fifo);

/**
 * @brief write data into tx buffer
 *
 * @param inst
 * @param c
 * @param num
 * @return size_t actually data writen in buffer
 */
size_t uart_write(uart_t *inst, const char *c, size_t num);

/**
 * @brief read data from rx buffer
 *
 * @param inst
 * @param c
 * @param num
 * @return size_t actually data readded from buffer
 */
size_t uart_read(uart_t *inst, char *c, size_t num);

/**
 * @brief enable async transmit
 *
 * @param inst
 */
void uart_enable_tx(uart_t *inst);

/**
 * @brief disable async transmit
 * 
 * @param inst 
 */
void uart_disable_tx(uart_t *inst);

/**
 * @brief enable async recv
 *
 * @param inst
 */
void uart_enable_rx(uart_t *inst);

/**
 * @brief disable async recv
 * 
 * @param inst 
 */
void uart_disable_rx(uart_t *inst);

/**
 * @brief get current status
 *
 * @param inst
 * @return uart_status_t
 */
uart_status_t uart_status(uart_t *inst);

/**
 * @brief rx isr handler
 *
 * @param inst
 */
void uart_isr_handle_rx(uart_t *inst);

/**
 * @brief tx isr handler
 *
 * @param inst
 */
void uart_isr_handle_tx(uart_t *inst);

#ifdef __cplusplus
}
#endif
