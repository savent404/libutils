/**
 * @file uart_utils.c
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-08-16
 *
 * Copyright 2022 savent_gate
 *
 */

#include "uart_utils.h"
#include "fifo_utils.h"
#include <assert.h>

void uart_init(uart_t *inst, const uart_io_t *io, void *data_ptr,
               fifo_t *rx_fifo, fifo_t *tx_fifo) {
  assert(inst);
  assert(io);
  assert(rx_fifo);
  assert(tx_fifo);
  inst->status = uart_status_idle;
  inst->rx_fifo = rx_fifo;
  inst->tx_fifo = tx_fifo;
  inst->io = io;
  inst->privdata = data_ptr;
  inst->rx_enable = false;
  inst->tx_enable = false;
}

size_t uart_write(uart_t *inst, const char *c, size_t num) {
  assert(inst);
  assert(c);
  assert(num);
  fifo_t *fifo = inst->tx_fifo;
  size_t write_len = fifo_capacity(fifo) - fifo_len(fifo);
  if (write_len > num)
    write_len = num;
  fifo_push(fifo, c, write_len);

  if (inst->tx_enable) {
    uart_enable_tx(inst);
  }
  return write_len;
}

size_t uart_read(uart_t *inst, char *c, size_t num) {
  assert(inst);
  assert(c);
  assert(num);
  fifo_t *fifo = inst->rx_fifo;
  size_t read_len = fifo_len(fifo);
  if (read_len > num)
    read_len = num;
  fifo_pop(fifo, c, read_len);
  return read_len;
}

void uart_enable_tx(uart_t *inst) {
  assert(inst);
  const uart_io_t *io = inst->io;
  fifo_t *fifo = inst->tx_fifo;
  void *privdata = inst->privdata;
  inst->tx_enable = true;
  switch (inst->status) {
  case uart_status_rx:
    io->uart_rx_async_abort(privdata);
  case uart_status_idle:
    if (fifo_len(fifo)) {
      fifo_pop(fifo, &inst->tx_tmp, 1);
      inst->status = uart_status_tx;
      io->uart_tx_async(&inst->tx_tmp, privdata);
    }
    break;
  case uart_status_tx:
    break;
  default:
    assert(false);
  }
}

void uart_disable_tx(uart_t *inst) {
  assert(inst);
  const uart_io_t *io = inst->io;
  void *privdata = inst->privdata;
  inst->tx_enable = false;
  switch (inst->status) {
  case uart_status_rx:
  case uart_status_idle:
    break;
  case uart_status_tx:
    io->uart_tx_async_abort(privdata);
    inst->status = uart_status_idle;
    break;
  default:
    assert(false);
  }
}

void uart_enable_rx(uart_t *inst) {
  assert(inst);
  const uart_io_t *io = inst->io;
  void *privdata = inst->privdata;
  inst->rx_enable = true;
  switch (inst->status) {
  case uart_status_tx:
    io->uart_tx_async_abort(privdata);
  case uart_status_idle:
    inst->status = uart_status_rx;
    io->uart_rx_async(&inst->rx_tmp, privdata);
    break;
  case uart_status_rx:
    break;
  default:
    assert(false);
  }
}

void uart_disable_rx(uart_t *inst) {
  assert(inst);
  const uart_io_t *io = inst->io;
  void *privdata = inst->privdata;
  inst->rx_enable = false;
  switch (inst->status) {
  case uart_status_tx:
  case uart_status_idle:
    break;
  case uart_status_rx:
    io->uart_rx_async_abort(privdata);
    break;
  default:
    assert(false);
  }
}

void uart_isr_handle_rx(uart_t *inst) {
  assert(inst);
  fifo_t *fifo = inst->rx_fifo;
  const uart_io_t *io = inst->io;
  void *privdata = inst->privdata;

  if (!fifo_full(fifo)) {
    fifo_push(fifo, &inst->rx_tmp, 1);
    io->uart_rx_async(&inst->rx_tmp, privdata);
  } else {
    // NOTE: completely disable uart rx if fifo is full
    inst->status = uart_status_idle;
    inst->rx_enable = false;
    io->uart_rx_async_abort(privdata);
  }
}

void uart_isr_handle_tx(uart_t *inst) {
  assert(inst);
  fifo_t *fifo = inst->tx_fifo;
  const uart_io_t *io = inst->io;
  void *privdata = inst->privdata;
  if (!fifo_len(fifo)) {
    if (inst->rx_enable && !fifo_full(inst->rx_fifo)) {
      inst->status = uart_status_rx;
      io->uart_rx_async(&inst->rx_tmp, privdata);
    } else {
      inst->status = uart_status_idle;
    }
  } else {
    fifo_pop(fifo, &inst->tx_tmp, 1);
    inst->io->uart_tx_async(&inst->tx_tmp, privdata);
  }
}
