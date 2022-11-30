#ifndef __FLASH_H
#define __FLASH_H

#include "main.h"
#include "stm32l4s5xx.h"

#define FLASH_BASE_ADDR MX25R6435F_FLASH_SIZE - MX25R6435F_BLOCK_SIZE

// Game engine configuration model.
typedef struct __engine_configuration
{
  uint32_t maxPlayerHP;
  uint32_t maxEnemyHP;
  uint32_t enemySteps;
  uint32_t leaderboard[LEADERBOARD_SIZE];
} engine_configuration;

// I2C configuration model.
typedef struct __input_configuration
{
  int16_t gyro_offset[3];
} input_configuration;

// UART configuration model.
typedef struct __output_configuration
{
  char user;
  char enemy;
  char projectile;
  char background;
  char border;
} output_configuration;

// Main configuration model to read from flash.
#define MAGIC_NUMBER 1783320125
typedef struct __configuration
{
  // Magic number to identify valid configuration.
  uint32_t magic;
  // Game engine configuration.
  engine_configuration engine_config;
  // Input configuration.
  input_configuration input_config;
  // Output configuration.
  output_configuration output_config;
} configuration;

bool initFlash();
bool readConfiguration();
bool writeConfiguration();
void getConfiguration(configuration *config);
bool isConfigurationValid();

void setEngineConfiguration(engine_configuration *config);
void getEngineConfiguration(engine_configuration *config);

void setInputConfiguration(input_configuration *config);
void getInputConfiguration(input_configuration *config);

void setOutputConfiguration(output_configuration *config);
void getOutputConfiguration(output_configuration *config);

#endif // __FLASH_H
