#ifndef __FLASH_H
#define __FLASH_H

#include "main.h"
#include "stm32l4s5xx.h"

#define FLASH_BASE_ADDR MX25R6435F_FLASH_SIZE - MX25R6435F_BLOCK_SIZE

// Main configuration model to read from flash.
#define MAGIC_NUMBER 1783320124
typedef struct __configuration
{
  // Magic number to identify valid configuration.
  uint32_t magic;
} configuration;

bool initFlash();
bool readConfiguration();
bool writeConfiguration();
void getConfiguration(configuration *config);
bool isConfigurationValid();

#endif // __FLASH_H
