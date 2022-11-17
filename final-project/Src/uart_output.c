#include "main.h"
#include "uart_output.h"

#include "stm32l4xx_hal.h"

// UART handle
UART_HandleTypeDef *_huart;

// transmission buffer
uint16_t _n = MAX_BUF_SIZE;
uint8_t _buf[MAX_BUF_SIZE];
uint8_t _clearBuf[MAX_BUF_SIZE];

void initOutput(UART_HandleTypeDef *huart)
{
  _huart = huart;
  _n = (uint16_t) SCR_WIDTH;
  _n *= (uint16_t) SCR_HEIGHT;

  // clear buffer
  memset(_buf, BORDER, _n);

  // write initial playing area
  int32_t i = 1;
  for (uint32_t r = 1; r < SCR_HEIGHT - 1; r++)
  {
    i += SCR_WIDTH;
    memset(_buf + i, BACKGROUND, SCR_WIDTH - 4);
  }

  // write carriage return characters
  i = -2;
  for (uint32_t r = 0; r < SCR_HEIGHT; r++)
  {
    i += SCR_WIDTH;
    _buf[i] = '\n';
    _buf[i + 1] = '\r';
  }

  // set clear buffer
  memset(_clearBuf, '\b', _n);

  ITM_Port32(31) = SCR_WIDTH;
  ITM_Port32(31) = SCR_HEIGHT;
  ITM_Port32(31) = _n;
}

void updateBuffer(user *user, enemy enemies[NUM_ENEMIES], projectile projectiles[NUM_PROJECTILES])
{
  // clear objects from buffer
  for (int i = 0; i < MAX_BUF_SIZE; i++) {
    if (_buf[i] != BACKGROUND && _buf[i] != BORDER) {
      _buf[i] = BACKGROUND;
    }
  }

  // draw user
  int32_t x = *&user->rb.x + 1; // add one to offset from border
  int32_t y = *&user->rb.y + 1; // add one to offset from border
  _buf[x + y * SCR_WIDTH] = USER;

  // draw enemies
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    if (enemies[i].enabled)
    {
      x = enemies[i].rb.x + 1;
      y = enemies[i].rb.y + 1;
      _buf[x + y * SCR_WIDTH] = ENEMY;
    }
  }

  // draw projectiles
  for (int i = 0; i < NUM_PROJECTILES; i++)
  {
    if (projectiles[i].enabled)
    {
      x = projectiles[i].rb.x + 1;
      y = projectiles[i].rb.y + 1;
      _buf[x + y * SCR_WIDTH] = PROJECTILE;
    }
  }
  transmitBuffer(_buf, _n);
}

void transmitBuffer(uint8_t *buf, uint16_t n)
{
  hal_exec(HAL_UART_Transmit(_huart, buf, n, UART_TIMEOUT));
}

void resetCursor()
{
  transmitBuffer(_clearBuf, _n);
}
