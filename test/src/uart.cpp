/**
 * @file uart.cpp
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-08-16
 *
 * Copyright 2022 savent_gate
 *
 */

#include "uart_utils.h"
#include <gtest/gtest.h>
#include <list>
namespace {
extern "C" {

struct private_t {
  std::list<char> tx_buffer;
  std::list<char> rx_buffer;
  char *rx_ptr;
  uart_t *inst;

  enum {
    idle,
    tx,
    rx,
  } status;
};

static private_t *convert(void *p) { return reinterpret_cast<private_t *>(p); }

static void try2rx(private_t *p) {
  if (!p->rx_ptr || p->status != private_t::rx)
    return;
  while (!p->rx_buffer.empty()) {
    auto c = p->rx_buffer.front();
    p->rx_buffer.pop_front();
    *(p->rx_ptr) = c;
    uart_isr_handle_rx(p->inst);
  }
}

static void try2tx(private_t *p) {
  if (p->status != private_t::tx)
    return;
  while (p->inst->status == uart_status_tx) {
    uart_isr_handle_tx(p->inst);
  }
}

void rx_async(char *ch, void *data) {
  auto p = convert(data);
  p->rx_ptr = ch;
  p->status = private_t::rx;
  try2rx(p);
}

void tx_async(const char *ch, void *data) {
  auto p = convert(data);
  p->tx_buffer.push_back(*ch);
  p->status = private_t::tx;
  try2tx(p);
}

void rx_async_abort(void *data) {
  auto p = convert(data);
  p->status = private_t::idle;
}

void tx_async_abort(void *data) { rx_async_abort(data); }

private_t *tear_up() {
  auto p = new private_t;
  auto io = new uart_io_t;
  auto tx = new fifo_t;
  auto rx = new fifo_t;
  p->rx_ptr = nullptr;
  p->inst = new uart_t;

  io->uart_rx_async = rx_async;
  io->uart_rx_async_abort = rx_async_abort;
  io->uart_tx_async = tx_async;
  io->uart_tx_async_abort = tx_async_abort;

  rx->buffer = new char[128];
  rx->fifo_len = 128;
  rx->index_start = 0;
  rx->index_end = 0;
  rx->type_len = 1;

  tx->buffer = new char[128];
  tx->fifo_len = 128;
  tx->index_start = 0;
  tx->index_end = 0;
  tx->type_len = 1;

  uart_init(p->inst, io, p, rx, tx);

  return p;
}
}
} // namespace

TEST(uart, write) {
  auto p = tear_up();
  auto inst = p->inst;
  const char msg[] = "Hello,World";

  uart_write(inst, msg, sizeof(msg));
  ASSERT_EQ(p->tx_buffer.size(), 0);
  uart_enable_tx(inst);
  ASSERT_EQ(p->tx_buffer.size(), sizeof(msg));
  int i = 0;
  for (auto &c : p->tx_buffer)
    ASSERT_EQ(msg[i++], c);
}

TEST(uart, read) {
  auto p = tear_up();
  auto inst = p->inst;
  const char msg[] = "Hello,World";
  char tmp[sizeof(msg)];

  for (auto &c : msg)
    p->rx_buffer.push_back(c);
  uart_enable_rx(inst);
  ASSERT_EQ(uart_read(inst, tmp, sizeof(tmp)), sizeof(tmp));
  for (size_t i = 0; i < sizeof(tmp); i++) {
    ASSERT_EQ(msg[i], tmp[i]);
  }
}

TEST(uart, write_when_read) {
  auto p = tear_up();
  auto inst = p->inst;
  const char msg[] = "Hello,World";
  char tmp[sizeof(msg)];

  uart_enable_rx(inst);
  uart_enable_tx(inst);

  uart_write(inst, msg, sizeof(msg));
  ASSERT_EQ(p->tx_buffer.size(), sizeof(msg));

  for (auto &c : msg)
    p->rx_buffer.push_back(c);
  try2rx(p);

  ASSERT_EQ(uart_read(inst, tmp, sizeof(tmp)), sizeof(tmp));
  for (size_t i = 0; i < sizeof(tmp); i++) {
    ASSERT_EQ(tmp[i], msg[i]);
  }
}