#include <Arduino.h>


void CRC32_reset();
void CRC32_update(const uint8_t& data);
uint32_t CRC32_finalize();
