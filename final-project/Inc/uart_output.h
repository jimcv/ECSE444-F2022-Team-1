#include "main.h"

#ifndef __UART_OUTPUT_H
#define __UART_OUTPUT_H

/**
 * Initialize the output handler.
 * @param whether to reconfigure.
 * @param huart the UART handle.
 */
void initOutput(bool reconfigurationRequested, UART_HandleTypeDef *huart);

/**
 * Update the output buffer.
 * @param gameObjectsSV the shared variable ID for the game objects.
 */
void updateBuffer(uint32_t gameObjectsSV);

/**
 * Send the output buffer over UART.
 */
void transmitBuffer();

#endif // __UART_OUTPUT_H
