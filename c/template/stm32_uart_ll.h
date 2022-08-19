/**
 * @file stm32_uart_ll.h
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-08-19
 *
 * Copyright 2022 savent_gate
 *
 */
#pragma once

#include "uart_utils.h"

#include <stm32f4xx_hal.h>

/**
 * @brief init uart instance from huart
 *
 * @param huart
 * @return uart_t*
 */
uart_t *stm32_uart_init(UART_HandleTypeDef *huart);

/**
 * @brief deinit uart instance(not huart)
 *
 * @param inst
 */
void stm32_uart_deinit(uart_t *inst);
