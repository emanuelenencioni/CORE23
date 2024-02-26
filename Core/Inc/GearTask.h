#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "main.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "CanHandlerTask.h"
#include "ADCTask.h"
#include "stdlib.h"

#ifndef GEARTASK_H_
#define GEARTASK_H_

#define CUTOFF_TIME 	100		//ms to hold up cutoff signal
//#define cutoff_wait			200		//ms before actuate upshift
#define SHIFT_HOLD			500		//ms to hold the gear pressed
#define RPM_THRESHOLD		5000	//RPM to cut-off
#define clutch_engaging 	3000	//DAC value when clutch starts to grip
#define clutch_releasing 	300		//DAC value when clutch has about 100% grip (but not fully released)

void gearThread(void* argument);
void upShift();
void downShift();
void upShiftCutOff();
void downShiftCutOff();
uint8_t gear_getCurrent();
#endif