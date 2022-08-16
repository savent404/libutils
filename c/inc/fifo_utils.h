/**
 * @file fifo_utils.h
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-08-16
 *
 * Copyright 2022 savent_gate
 *
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  size_t fifo_len;
  size_t type_len;
  size_t index_start;
  size_t index_end;
  void *buffer;
} fifo_t;

/**
 * @brief define a fifo
 * @param name fifo name
 * @param len fifo item length
 * @param type fifo item type
 * @note len must be equal 2^x
 */
#define FIFO_DEFINE(name, len, type)                                           \
  static char _fifo_buffer_##name[sizeof(type) * len];                         \
  fifo_t _fifo_##name = {.fifo_len = len,                                      \
                         .type_len = sizeof(type),                             \
                         .index_start = 0,                                     \
                         .index_end = 0,                                       \
                         .buffer = &_fifo_buffer_##name};

/**
 * @brief declear a fifo defined at other files
 */
#define FIFO_DECLEAR(name) extern fifo_t _fifo_##name;

/**
 * @brief return fifo's pointer
 */
#define FIFO_PTR(name) (&_fifo_##name)

/**
 * @brief return buffered item in fifo
 *
 * @param ptr
 * @return size_t
 */
size_t fifo_len(fifo_t *ptr);

/**
 * @brief return fifo's maxium item number
 *
 * @param ptr
 * @return size_t
 */
size_t fifo_capacity(fifo_t *ptr);

/**
 * @brief check fifo is full
 *
 * @param ptr
 * @return true full
 * @return false  not full
 */
bool fifo_full(fifo_t *ptr);

/**
 * @brief push item into fifo
 *
 * @param ptr
 * @param[in] data
 * @param num
 */
void fifo_push(fifo_t *ptr, const void *data, size_t num);

/**
 * @brief pop item from fifo
 *
 * @param ptr
 * @param[out] data
 * @param num
 */
void fifo_pop(fifo_t *ptr, void *data, size_t num);

#ifdef __cplusplus
}
#endif
