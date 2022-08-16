/**
 * @file fifo_utils.c
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-08-16
 *
 * Copyright 2022 savent_gate
 *
 */
#include "fifo_utils.h"
#include <assert.h>
#include <string.h>

static inline bool _is_aligned_fifo_len(fifo_t *ptr) {
  int bit_count = 0;
  size_t fifo_len = ptr->fifo_len;
  for (size_t i = 0; i < sizeof(size_t) * 8; i++, fifo_len >>= 1) {
    if (fifo_len & 1)
      bit_count++;
  }
  return bit_count == 1;
}

static inline void _push_item(void *buffer, const void *data,
                              size_t type_size) {
  memcpy(buffer, data, type_size);
}

static inline void _pop_item(const void *buffer, void *data, size_t type_size) {
  memcpy(data, buffer, type_size);
}

static inline size_t _index_in_arr_with_arr_aligned(int i, size_t arr_len) {
  return (i + arr_len) & (arr_len - 1);
}

static inline size_t _index_in_arr_normal(int i, size_t arr_len) {
  return (i + arr_len) % arr_len;
}

static inline void *_get_pointer(void *ptr, size_t index,
                                 size_t item_type_size) {
  char *p = (char *)ptr;
  return p + index * item_type_size;
}

size_t fifo_capacity(fifo_t *ptr) {
  assert(ptr);
  assert(_is_aligned_fifo_len(ptr));
  return ptr->fifo_len - 1;
}

size_t fifo_len(fifo_t *ptr) {
  assert(ptr);
  assert(_is_aligned_fifo_len(ptr));
  return _index_in_arr_with_arr_aligned(
      ptr->index_end - ptr->index_start + ptr->fifo_len, ptr->fifo_len);
}

bool fifo_full(fifo_t *ptr) {
  assert(ptr);
  return fifo_len(ptr) == ptr->fifo_len - 1;
}

void fifo_push(fifo_t *ptr, void *data, size_t num) {
  assert(ptr);
  assert(fifo_len(ptr) + num <= fifo_capacity(ptr));
  size_t index = ptr->index_end;
  size_t fifo_len = ptr->fifo_len;
  size_t type_size = ptr->type_len;
  char *buffer = (char *)ptr->buffer;
  for (size_t i = 0; i < num; i++) {
    _push_item(_get_pointer(buffer, index, type_size),
               _get_pointer(data, i, type_size), type_size);
    index = _index_in_arr_with_arr_aligned(index + 1, fifo_len);
  }
  ptr->index_end = index;
}

void fifo_pop(fifo_t *ptr, void *data, size_t num) {
  assert(ptr);
  assert(fifo_len(ptr) >= num);
  size_t index = ptr->index_start;
  size_t fifo_len = ptr->fifo_len;
  size_t type_size = ptr->type_len;
  char *buffer = (char *)ptr->buffer;
  for (size_t i = 0; i < num; i++) {
    _pop_item(_get_pointer(buffer, index, type_size),
              _get_pointer(data, i, type_size), type_size);
    index = _index_in_arr_with_arr_aligned(index + 1, fifo_len);
  }
  ptr->index_start = index;
}
