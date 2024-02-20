#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "main.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "ADCTask.h"
#include "CanHandlerTask.h"
#include "stdlib.h"

#define APPS1 5
#define APPS2 6
#define MAX_DEVIATION 400 // 10% deviation

#ifndef ACCTASK_H_
#define ACCTASK_H_

void AccThread(void* argument);
#endif