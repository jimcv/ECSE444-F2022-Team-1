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
void initOutput(bool reconfigurationRequested, UART_HandleTypeDef *huart)
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

