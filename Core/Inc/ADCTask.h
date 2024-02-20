#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "main.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#ifndef ADCTASK_H_
#define ADCTASK_H_

#define ADC_BUFFER_SIZE 12


void ADCThread(void* argument);
#endif