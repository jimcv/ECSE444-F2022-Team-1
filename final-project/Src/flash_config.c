#include "flash_config.h"
#include "main.h"

#include <stdlib.h>
/*
 * MX25R6435F_FLASH_SIZE
 * MX25R6435F_BLOCK_SIZE
 */

#define MIN(n, m) n <= m ? n : m

// global configuration variable
configuration _config;

// wrapper function to handle QSPI flash errors
#define flash_exec(call) call != QSPI_OK

/*
 * Initialize the flash configuration.
 */
bool initFlash()
{
  // initialize
  return flash_exec(BSP_QSPI_Init());
}

#define readStruct(address, type, ptr) flash_exec(BSP_QSPI_Read((uint8_t*)ptr, address, sizeof(type)))
#define writeStruct(address, type, ptr) flash_exec(BSP_QSPI_Write((uint8_t*)ptr, address, sizeof(type)))

/**
 * Read configuration from flash to local configuration store.
 * @returns true if the read was successful.
 */
bool readConfiguration()
{
  return readStruct(FLASH_BASE_ADDR, configuration, &_config);
}

/**
 * Write local configuration to flash. Clears the current block first.
 * @returns true if the write was successful.
 */
bool writeConfiguration()
{
  if (BSP_QSPI_Erase_Block(FLASH_BASE_ADDR))
  {
    return false;
  }

  _config.magic = MAGIC_NUMBER;
  return writeStruct(FLASH_BASE_ADDR, configuration, &_config);
}

/**
 * Copy the local configuration to a pointer.
 */
void getConfiguration(configuration *config)
{
  *config = _config;
}

/**
 * Determine if the current configuration is valid.
 * @returns true if the magic number matches the configured value.
 */
bool isConfigurationValid()
{
  return _config.magic == MAGIC_NUMBER;
}
