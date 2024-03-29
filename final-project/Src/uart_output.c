#include "main.h"
#include "uart_output.h"
#include "flash_config.h"

#include "stm32l4xx_hal.h"

output_configuration outputConfig;

// UART handle
UART_HandleTypeDef *_huart;

// transmission buffer
uint16_t _n = MAX_BUF_SIZE;
uint8_t _buf[2 * MAX_BUF_SIZE];

/**
 * Initialize the output handler.
 * @param whether to reconfigure.
 * @param huart the UART handle.
 */
void initOutput(bool reconfigurationRequested, UART_HandleTypeDef *huart, game_objects *game_objects)
{
  if (reconfigurationRequested)
  {
    outputConfig.background = ' ';
    outputConfig.border = '*';
    outputConfig.enemy = '@';
    outputConfig.projectile = '^';
    outputConfig.user = 'A';
    setOutputConfiguration(&outputConfig);
  }
  else
  {
    getOutputConfiguration(&outputConfig);
  }

  _huart = huart;

  // clear buffer
  memset(_buf, outputConfig.border, 2 * _n);

  // write initial playing area
  int32_t i = 1;
  for (uint32_t r = 1; r < SCR_HEIGHT - 1; r++)
  {
    i += SCR_WIDTH;
    memset(_buf + i + _n, outputConfig.background, SCR_WIDTH - 4);
  }

  // write carriage return characters
  i = -2;
  for (uint32_t r = 0; r < SCR_HEIGHT; r++)
  {
    i += SCR_WIDTH;
    _buf[i + _n] = '\n';
    _buf[i + 1 + _n] = '\r';
  }

  // set clear buffer
  memset(_buf, '\b', _n);

  // disable all text initially
  for (uint32_t line = 0; line < MAX_Y; line++)
  {
    clearText(game_objects->text, line);
  }
}

/**
 * Update the output buffer.
 * @param gameObjectsPointer the game objects pointer.
 */
void writeBuffer(void *gameObjectsPtr)
{
  game_objects *gameObjects = (game_objects*)gameObjectsPtr;
  user user = gameObjects->user;
  enemy *enemies = gameObjects->enemies;
  projectile *projectiles = gameObjects->projectiles;
  game_text *text = gameObjects->text;

  // clear objects from buffer
  for (int i = _n; i < 2 * _n; i++) {
    if (_buf[i] != outputConfig.background && _buf[i] != outputConfig.border && _buf[i] != '\r' && _buf[i] != '\n') {
      _buf[i] = outputConfig.background;
    }
  }

  // draw user
  int32_t x = user.x + 1; // add one to offset from border
  int32_t y = user.y + 1; // add one to offset from border
  _buf[x + y * SCR_WIDTH + _n] = outputConfig.user;

  // draw projectiles
  for (int i = 0; i < NUM_PROJECTILES; i++)
  {
    if (projectiles[i].enabled)
    {
      x = projectiles[i].x + 1;
      y = projectiles[i].y + 1;
      _buf[x + y * SCR_WIDTH + _n] = outputConfig.projectile;
    }
  }

  // draw enemies
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    if (enemies[i].enabled)
    {
      x = enemies[i].x + 1;
      y = enemies[i].y + 1;
      _buf[x + y * SCR_WIDTH + _n] = outputConfig.enemy;
    }
  }

  // draw text
  for (int i = 0; i < MAX_Y; i++)
  {
    if (text[i].enabled)
    {
      x = text[i].indentation + 1;
      for (int c = 0; c < MAX_X; c++)
      {
        if (text[i].text[c] == '\0')
        {
          break;
        }
        _buf[x + c + (i + 1) * SCR_WIDTH + _n] = text[i].text[c];
      }
    }
  }
}

/**
 * Update the output buffer.
 * @param gameObjectsSV the shared variable ID for the game objects.
 */
void updateBuffer(uint32_t gameObjectsSV)
{
  lockSharedVariableAndExecute(gameObjectsSV, OS_TIMEOUT, writeBuffer);
}

/**
 * Send the output buffer over UART.
 */
void transmitBuffer()
{
  hal_exec(HAL_UART_Transmit_DMA(_huart, _buf, 2 * _n));
}

/**
 * Send the output buffer over UART without using DMA.
 */
void transmitBufferNonDMA()
{
  hal_exec(HAL_UART_Transmit(_huart, _buf, 2 * _n, OS_TIMEOUT));
}

/**
 * Helper function to write a number in a string without sprintf.
 *
 * @param str the string to write in.
 * @param i the character where the number should start.
 * @param num the number to write.
 * @param digits the number of digits to include.
 */
void writeNumber(char *str, int i, int num, int digits)
{
  int n = i + digits - 1;
  while (n >= i)
  {
    str[n] = '0' + num % 10;

    num /= 10;
    --n;
  }
}

/**
 * Helper function for writing text on screen
 *
 * Note: (x, y) = (0, 0) will write the string in the top-left
 * corner of the playing field
 *
 * @param gameObjects game object
 * @param x the x-offset of the leftmost character of the text, -1 to center.
 * @param y the y-offset of the leftmost character of the text
 * @param str the string to write on screen
 * @return true if the text was successfully written to gameObjects
 */
bool writeText(game_text *text, int x, int y, char *str)
{
  int n = strlen(str);
  if (x == -1)
  {
    x = (MAX_X - n) / 2;
  }

  if (x >= 0 && x < MAX_X && // check bounds of x
      y >= 0 && y < MAX_Y)   // check bounds of y
  {
    n = MIN(n, MAX_X - x - 1);

    text[y].enabled = true;
    text[y].indentation = x;
    // copy string
    memcpy(text[y].text, str, n);
    text[y].text[n] = 0;
    return true;
  }
  return false;
}

/**
 * Helper function for clearing text on screen
 *
 * Note: (x, y) = (0, 0) will write the string in the top-left
 * corner of the playing field
 *
 * @param gameObjects game object
 * @param y the y-offset of the line to clear
 */
void clearText(game_text *text, int y)
{
  text[y].enabled = false;
}
