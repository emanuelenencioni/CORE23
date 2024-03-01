#include "ADCTask.h"
#include "CheckModeTask.h"
//#include "stdlib.h"

#ifndef TELEMETRYTASK_H_
#define TELEMETRYTASK_H_

void telemetryThread(void* argument);
void sendCANInt16(CANMessage* msg, uint16_t data1, uint16_t data2, uint16_t data3, uint16_t data4, uint16_t stdId, uint32_t extId, uint8_t ide, uint8_t rtr, uint8_t dlc);
#endif