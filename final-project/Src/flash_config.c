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
#define flash_exec(call) call == QSPI_OK

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
 * @param config the pointer to write to.
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

/**
 * Copy an engine configuration model to the local store.
 * @param config the model to copy to the local store.
 */
void setEngineConfiguration(engine_configuration *config)
{
  _config.engine_config = *config;
}

/**
 * Copy the local engine configuration to a pointer.
 * @param config the pointer to write to.
 */
void getEngineConfiguration(engine_configuration *config)
{
  *config = _config.engine_config;
}

/**
 * Copy an input configuration model to the local store.
 * @param config the model to copy to the local store.
 */
void setInputConfiguration(input_configuration *config)
{
  _config.input_config = *config;
}

/**
 * Copy the local input configuration to a pointer.
 * @param config the pointer to write to.
 */
void getInputConfiguration(input_configuration *config)
{
  *config = _config.input_config;
}

/**
 * Copy an output configuration model to the local store.
 * @param config the model to copy to the local store.
 */
void setOutputConfiguration(output_configuration *config)
{
  _config.output_config = *config;
}

/**
 * Copy the local output configuration to a pointer.
 * @param config the pointer to write to.
 */
void getOutputConfiguration(output_configuration *config)
{
  *config = _config.output_config;
}
