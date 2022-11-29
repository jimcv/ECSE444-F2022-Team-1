#include "main.h"

#ifndef __UART_OUTPUT_H
#define __UART_OUTPUT_H

/**
 * Initialize the output handler.
 * @param whether to reconfigure.
 * @param huart the UART handle.
 */
void initOutput(bool reconfigurationRequested, UART_HandleTypeDef *huart, game_objects *game_objects);

/**
 * Update the output buffer.
 * @param gameObjectsSV the shared variable ID for the game objects.
 */
void updateBuffer(uint32_t gameObjectsSV);

/**
 * Send the output buffer over UART.
 */
void transmitBuffer();

/**
 * Helper function for writing text on screen
 *
 * Note: (x, y) = (0, 0) will write the string in the top-left
 * corner of the playing field
 *
 * @param gameObjects game object
 * @param x the x-offset of the leftmost character of the text
 * @param y the y-offset of the leftmost character of the text
 * @param str the string to write on screen
 * @return true if the text was successfully written to gameObjects
 */
bool writeText(game_text *text, int x, int y, char *str);

/**
 * Helper function for clearing text on screen
 *
 * Note: (x, y) = (0, 0) will write the string in the top-left
 * corner of the playing field
 *
 * @param gameObjects game object
 * @param y the y-offset of the line to clear
 */
void clearText(game_text *text, int y);

#endif // __UART_OUTPUT_H
