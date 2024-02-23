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
void sendCANMessage(CANMessage* msg, uint16_t data, uint16_t stdId = 0, uint32_t extId = 0, uint8_t ide = 0, uint8_t rtr = 0, uint8_t dlc = 0);
#endif