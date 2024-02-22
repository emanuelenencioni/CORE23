#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "main.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "CanHandlerTask.h"
//#include "stdlib.h"

#ifndef GEARTASK_H_
#define GEARTASK_H_

#define cutoff_duration 	100		//ms to hold up cutoff signal
#define cutoff_wait			200		//ms before actuate upshift
#define shift_hold			500		//ms to hold the gear pressed
#define clutch_engaging 	3000	//DAC value when clutch starts to grip
#define clutch_releasing 	300		//DAC value when clutch has about 100% grip (but not fully released)

void gearThread(void* argument);
void upShift();
#endif