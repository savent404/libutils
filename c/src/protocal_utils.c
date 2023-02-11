/**
 * @file protocal_utils.c
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-02-11
 *
 * Copyright 2023 savent_gate
 *
 */
#include <assert.h>
#include <protocal_utils.h>

int protocal_find_frame(fifo_t *fifo, protocal_match_fn_t fn, void *buffer,
                        size_t buff_size) {
  int re;

  assert(fifo && fifo->type_len == 1);

  re = fn(fifo);

  if (re <= 0) {
    re = !re ? 1 : -re;
    for (int i = 0; i < re; i++) {
      fifo_pop(fifo, buffer, 1);
    }
  } else if (buff_size >= re) {
    fifo_pop(fifo, buffer, re);
    return re;
  }

  return 0;
}
