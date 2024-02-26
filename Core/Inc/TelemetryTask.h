#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "main.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "CanHandlerTask.h"
#include "ADCTask.h"
//#include "stdlib.h"

#ifndef TELEMETRYTASK_H_
#define TELEMETRYTASK_H_

void telemetryThread(void* argument);
void sendCANInt32(CANMessage* msg, uint32_t data, uint16_t stdId, uint32_t extId, uint8_t ide, uint8_t rtr, uint8_t dlc);
void sendCANInt16(CANMessage* msg, uint16_t data, uint16_t stdId, uint32_t extId, uint8_t ide, uint8_t rtr, uint8_t dlc);
void sendCANFloat(CANMessage* msg, float data, uint16_t stdId, uint32_t extId, uint8_t ide, uint8_t rtr, uint8_t dlc);
#endif