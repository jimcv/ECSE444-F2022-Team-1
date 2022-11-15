#include "main.h"
#include "uart_output.h"

#include "stm32l4xx_hal.h"

// UART handle
UART_HandleTypeDef *_huart;

// transmission buffer
uint16_t _n = MAX_BUF_SIZE;
uint8_t _buf[MAX_BUF_SIZE] = {0};
uint8_t _clearBuf[MAX_BUF_SIZE] = {0};

// characters
char background = ' ';
char border = '*';

void initOutput(UART_HandleTypeDef *huart)
{
  _huart = huart;
  _n = (uint16_t)SCR_WIDTH;
  _n *= (uint16_t)SCR_HEIGHT;

  // clear buffer
  memset(_buf, border, _n);

  // write initial playing area
  int32_t i = 1;
  for (uint32_t r = 1; r < SCR_HEIGHT - 1; r++)
  {
    i += SCR_WIDTH;
    memset(_buf + i, background, SCR_WIDTH - 4);
  }

  // write carriage return characters
  i = -2;
  for (uint32_t r = 0; r < SCR_HEIGHT; r++)
  {
    i += SCR_WIDTH;
    _buf[i] = '\n';
    _buf[i+1] = '\r';
  }

  // set clear buffer
  memset(_clearBuf, '\b', _n);

  ITM_Port32(31) = SCR_WIDTH;
  ITM_Port32(31) = SCR_HEIGHT;
  ITM_Port32(31) = _n;
}

void updateBuffer(user *user, enemy enemies[NUM_ENEMIES], projectile projectiles[NUM_PROJECTILES])
{
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
