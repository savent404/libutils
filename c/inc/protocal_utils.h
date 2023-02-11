/**
 * @file protocal_utils.h
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-02-11
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <fifo_utils.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief match function of
 *
 * @note only use fifo_peek to read data, don't modify fifo's data!
 * @param[in] fifo fifo buffer that restored bytes
 * @retval 0 not match
 * @return N>0 frame matched, frame has N bytes
 * @return N<0 frame not mathed, should drop N bytes
 */
typedef int (*protocal_match_fn_t)(fifo_t *ptr);

/**
 * @brief find frame in fifo
 *
 * @param[in,out] fifo
 * @param[in] fn match function
 * @param[out] dest buffer
 * @param[in] dest buffer max size
 * @return N > 0 find a frame, frame size is N bytes
 * @return false no frame found
 */
int protocal_find_frame(fifo_t *fifo, protocal_match_fn_t fn, void *buffer,
                        size_t buff_size);

#ifdef __cplusplus
}
#endif
