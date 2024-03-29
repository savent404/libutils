/**
 * @file uart_utils.h
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-08-16
 *
 * Copyright 2022 savent_gate
 *
 * @code
 * 
 * #include <fifo_utils.h>
 * static uart_t uart1;
 * static void example_rx_async(char *ch, void *privdata) {}
 * static void example_rx_async_abort(void* privdata) {}
 * static void example_tx_async(const char *ch, void* privdata) {}
 * static void example_tx_async_abort(const char *ch, void *privdata) {}
 * static void example_isr_rx_cb(void) {uart_isr_handle_rx(&huart1); }
 * static void example_isr_tx_cb(void) {uart_isr_handle_tx(&huart1); }
 * static uart_io_t example_io = {
 *   .uart_rx_async = example_rx_async,
 *   .uart_rx_async_abort = example_rx_async_abort,
 *   .uart_tx_async = example_tx_async,
 *   .uart_tx_async_abort = example_tx_async_abort,
 * };
 * FIFO_DEFINE(example_rx_fifo, 32, char);
 * FIFO_DEFINE(example_tx_fifo, 32, char);
 * 
 * int main(void)
 * {
 *   // register isr handler first
 *   xxxx_register_usart_it_rx_handler(example_isr_rx_cb);
 *   xxxx_register_usart_it_tx_handler(example_isr_tx_cb);
 * 
 *   uart_init(&uart1, &example_io, NULL,
 *     FIFO_PTR(example_rx_fifo), FIFO_PTR(example_tx_fifo));
 *   uart_enable_rx(&huart1);
 *   uart_write(&huart1, "Hello,World", 11);
 *   while (true) {
 *     int size;
 *     char buf[32];
 *     size = uart_read(&huart1, buf, 32);
 *     if (size)
 *       uart_write(&huart1, buf, size);
 *   }
 * }
 * 
 * @endcode
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
  void (*uart_rx_async)(char *ch, void *privdata);
  void (*uart_rx_async_abort)(void *privdata);
  void (*uart_tx_async)(const char *ch, void *privdata);
  void (*uart_tx_async_abort)(void *privdata);
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
  void *privdata;
  char rx_tmp, tx_tmp;
  uart_status_t status : 2;
  bool tx_enable : 1;
  bool rx_enable : 1;
} uart_t;

/**
 * @brief initialize uart instance, turn to idle
 *
 * @param inst instans of uart_utils
 * @param io   rx/tx io interface
 * @param private_data private data used in io interface
 * @param rx_fifo
 * @param tx_fifo
 */
void uart_init(uart_t *inst, const uart_io_t *io, void *private_data,
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
