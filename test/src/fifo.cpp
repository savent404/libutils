/**
 * @file fifo.cpp
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-08-16
 *
 * Copyright 2022 savent_gate
 *
 */

#include "fifo_utils.h"
#include <cmath>
#include <gtest/gtest.h>

TEST(fifo, define) {
  FIFO_DEFINE(c, 128, char);
  fifo_t *c_ptr = FIFO_PTR(c);

  ASSERT_EQ(fifo_len(c_ptr), 0);
  ASSERT_FALSE(fifo_full(c_ptr));
}

TEST(fifo, simple_io) {
  FIFO_DEFINE(c, 128, unsigned char);
  fifo_t *ptr = FIFO_PTR(c);

  for (unsigned char i = 0; i < 127; i++) {
    ASSERT_FALSE(fifo_full(ptr));
    fifo_push(ptr, &i, 1);
    ASSERT_EQ(fifo_len(ptr), i + 1);
  }

  for (unsigned char i = 0; i < 127; i++) {
    unsigned char t;
    ASSERT_FALSE(fifo_len(ptr) == 0);
    fifo_pop(ptr, &t, 1);
  }

  ASSERT_EQ(fifo_len(ptr), 0);
}

TEST(fifo, simple_io_1) {
  FIFO_DEFINE(c, 128, int);
  fifo_t *ptr = FIFO_PTR(c);
  for (int i = 0; i < 1e5; i++) {
    int c = std::rand();
    int t;
    fifo_push(ptr, &c, 1);
    ASSERT_EQ(fifo_len(ptr), 1);
    fifo_pop(ptr, &t, 1);
    ASSERT_EQ(t, c);
  }
  ASSERT_EQ(fifo_len(ptr), 0);
}

TEST(fifo, simple_io_2) {
  FIFO_DEFINE(c, 128, int);
  fifo_t *ptr = FIFO_PTR(c);
  for (int i = 1; i < 128; i++) {
    int c = std::rand();
    static int t[128], p[128];
    for (int j = 0; j < i; j++)
      t[j] = c;
    fifo_push(ptr, &t, i);
    ASSERT_EQ(fifo_len(ptr), i);
    fifo_pop(ptr, &p, i);
    ASSERT_TRUE(memcmp(t, p, sizeof(int) * i) == 0);
  }
  ASSERT_EQ(fifo_len(ptr), 0);
}

TEST(fifo, simple_io_3) {
  FIFO_DEFINE(c, 128, int);
  fifo_t *ptr = FIFO_PTR(c);
  int c = std::rand(), p, pre_c = c;
  fifo_push(ptr, &c, 1);
  for (int i = 0; i < 126; i++) {
    ASSERT_EQ(fifo_len(ptr), i + 1);
    fifo_pop(ptr, &p, 1);
    ASSERT_EQ(p, c);
    ASSERT_EQ(fifo_len(ptr), i);
    std::rand();
    fifo_push(ptr, &c, 1);
    fifo_push(ptr, &c, 1);
  }
  ASSERT_TRUE(fifo_full(ptr));
}

TEST(fifo, simple_io_4) {
  FIFO_DEFINE(c, 128, int);
  fifo_t *ptr = FIFO_PTR(c);

  for (int i = 0; i < 127; i++) {
    fifo_push(ptr, &i, 1);
  }

  ASSERT_EQ(fifo_len(ptr), 127);

  for (int i = 0; i < 127; i++) {
    int t;
    fifo_peek(ptr, i, &t);
    ASSERT_EQ(t, i);
  }

  ASSERT_EQ(fifo_len(ptr), 127);
}
