#ifndef ASSTATEHANDTASK_H_
#define ASSTATEHANDTASK_H_


#include "CanHandlerTask.h"


#define COUNTER_PERIOD_PWM 65535 // If u change here, change in the .ioc too.

void ASStateHandlerThread(void* argument);

// Functions to update the current AS state (Change the ASSI too)
void setASEmergency();
void setASFinished();
void setASOff();
void setASReady();
void setASDriving();

void actSiren();
void deactSiren();

#endif