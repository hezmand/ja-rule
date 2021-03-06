/*
 * This is the stub for plib_spi.h used for the tests. It contains the bare
 * minimum required to implement the mock SPI symbols.
 */

#ifndef TESTS_HARMONY_INCLUDE_PERIPHERAL_SPI_PLIB_SPI_H_
#define TESTS_HARMONY_INCLUDE_PERIPHERAL_SPI_PLIB_SPI_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  SPI_ID_1,
  SPI_ID_2,
  SPI_ID_3,
  SPI_ID_4,
  SPI_NUMBER_OF_MODULES
} SPI_MODULE_ID;

typedef enum {
  SPI_COMMUNICATION_WIDTH_8BITS = 0,
  SPI_COMMUNICATION_WIDTH_16BITS = 1,
  SPI_COMMUNICATION_WIDTH_32BITS = 2
} SPI_COMMUNICATION_WIDTH;

typedef enum {
  SPI_CLOCK_POLARITY_IDLE_LOW = 0,
  SPI_CLOCK_POLARITY_IDLE_HIGH = 1
} SPI_CLOCK_POLARITY;

typedef enum {
  SPI_PIN_SLAVE_SELECT = 0,
  SPI_PIN_DATA_OUT = 1
} SPI_PIN;

void PLIB_SPI_Enable(SPI_MODULE_ID index);

bool PLIB_SPI_TransmitBufferIsFull(SPI_MODULE_ID index);

void PLIB_SPI_CommunicationWidthSelect(SPI_MODULE_ID index,
                                       SPI_COMMUNICATION_WIDTH width);

void PLIB_SPI_ClockPolaritySelect(SPI_MODULE_ID index,
                                  SPI_CLOCK_POLARITY polarity);
void PLIB_SPI_MasterEnable(SPI_MODULE_ID index);

void PLIB_SPI_BaudRateSet(SPI_MODULE_ID index, uint32_t clockFrequency,
                          uint32_t baudRate);

bool PLIB_SPI_IsBusy(SPI_MODULE_ID index);

void PLIB_SPI_FIFOEnable(SPI_MODULE_ID index);

void PLIB_SPI_BufferWrite(SPI_MODULE_ID index, uint8_t data);

void PLIB_SPI_SlaveSelectDisable(SPI_MODULE_ID index);

void PLIB_SPI_PinDisable(SPI_MODULE_ID index, SPI_PIN pin);

#ifdef  __cplusplus
}
#endif

#endif  // TESTS_HARMONY_INCLUDE_PERIPHERAL_SPI_PLIB_SPI_H_
