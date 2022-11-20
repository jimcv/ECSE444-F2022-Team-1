#include "main.h"

#ifndef __UART_OUTPUT_H
#define __UART_OUTPUT_H

/**
 * Configuration and parameter initialization.
 * @param huart The UART handle.
 */
void initOutput(UART_HandleTypeDef *huart);

/**
 * Update the output buffer.
 * @param _user The user object.
 * @param _enemies The list of enemies.
 */
void updateBuffer(user *user, enemy enemies[NUM_ENEMIES], projectile projectiles[NUM_PROJECTILES]);

/**
 * Send the current buffer over UART.
 * @param buf The buffer to transmit.
 * @param n The number of characters to transmit.
 */
void transmitBuffer(uint8_t *buf, uint16_t n);

#endif // __UART_OUTPUT_H
